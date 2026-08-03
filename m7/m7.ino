// SPDX-License-Identifier: BSD-3-Clause
// source code based on the original dmc-lite sketch by Dyami Caliri
// at DZED Systems, Inc. (https://www.dragonframe.com)
// used with permission from the orginal author.
// Copyright 2023 by DZED Systems LLC
// Copyright 2026 by Limnmedia LLC

/**
 * \file m7.ino
 * \brief limnmoco M7 core sketch
 *
 * \note ensure that the Target core is the Main core
 * and the Flash split is 1.5MB M7 + 0.5MB M4
 */

#include "config.h"
#include "dfx.h"
#include "dmc_msg.h"
#include "motion.h"
#include "ik.h"
#include "coordinated_motion.h"
#include "dbg.h"

#include <RPC.h>

#ifdef CORE_CM4
#error "Make sure to target the Main core with flash split 1.5MB M7 + 0.5MB M4"
#endif

int killSwitchState;

void sendHello(uint32_t id);

uint32_t _triggers;
void setTriggers(uint32_t triggers);

RingBufferN<1024> messageBuffer;
uint32_t messageQueue; // queued messages

/*
 * Program state information
 */
static int32_t moveState;

/*
 * Message state machine variables.
 */
static int32_t loadMoveState;

/*
 * Uploaded move data
 */
static AxisMoveData move[MOTOR_COUNT];
static int32_t moveStartFrame;
static int32_t moveFrameCount;
static int32_t movePositionFrame;
uint16_t hardLimits = 0;

static uint8_t syncTriggers;
static uint8_t triggerMask;
static uint8_t triggerData[FRAME_COUNT];

/*
 * Motor state information.
 */
static Motor motors[MOTOR_CAM_COUNT]; // extra motor is camera

static uint8_t motorsSendPosition;
static uint32_t motorDirection;

/*
 * HardStop globals
 */
static uint32_t hardStopCounter;
static uint32_t stopAllLastTime;
static uint8_t exceptionCode;

/*
 * Jog/move globals
 */
static Motor frameTimeMotor;
static uint8_t frameTimeMotorDir;
static uint8_t frameTimeStopCounter;
static uint8_t playBlipStarted;
static uint8_t playBlipActive;
static uint8_t playBlipDuration;
static uint32_t playBlipLocation;   // 0x01 logic out, 0x02 relay out, 0x04 dmx channel
static uint16_t playBlipDmxChannel; // channel for DMX blip

static uint8_t cameraTriggerCountdown;

static uint16_t leds;

static GoMotionMove goMotionMove;
static GoMotionOverride goMotionOverride[MOTOR_COUNT];

static uint16_t usbLedCounter;

uint8_t triggerOutState;

uint32_t lastPositionTime = 0;
uint8_t hardStop = 0;
uint8_t limitStopMotor = 0;
uint8_t limitStopMotorForward = 0;
int8_t switchInput;
int8_t switchInputCounter = 0;
uint8_t motorsMoving = 0;

/*
 * Virtual globals
 */

struct Virtual {
  uint8_t boomIndex;
  uint8_t swingIndex;
  uint8_t trackIndex;
  uint8_t panIndex;
  uint8_t tiltIndex;
  uint8_t rollIndex;
  uint8_t aimEnabled;

  float boomLength;
  float boomExtension;
  float boomDisplacement;
  float nodalOffsetX;
  float nodalOffsetY;
  float nodalOffsetZ;

  BoomCompensationTable boomCompensation;
  uint8_t boomCompensationEnabled;
  float safeDistance;

  float track;
  float EW;
  float NS;
  float pan;
  float tilt;
  float roll;

  //float prevTrack;
  //float prevEW;
  //float prevNS;
  //float prevPan;
  //float prevTilt;
  //1float prevRoll;

  // The virtual pose is relative to the pose at the time the virtual
  // configuration is accepted.  fkOrigin is the corresponding absolute FK
  // result, so static boom reach and nodal offsets do not become virtual
  // movement.
  VirtualPose virtualOrigin;
  VirtualPose fkOrigin;
  uint8_t fkOriginValid;

  float aimX;
  float aimY;
  float aimZ;

  float T;
  float s;
  float b;
  float p;
  float t;
  float r;
};

static Virtual _virtual;

int32_t msg_motor_move(uint8_t motor, int32_t position);
int32_t msg_motor_stop(uint8_t motor);
int32_t msg_motor_jog(uint8_t motor, uint16_t speed, int32_t dest);

int32_t msg_virt_move(uint8_t motor, int32_t position);
int32_t msg_virt_stop(uint8_t motor);
int32_t msg_virt_jog(uint8_t motor, uint16_t speed, int32_t dest);
int32_t msg_virt_jog_on_line(uint8_t axis, uint16_t speed);
int32_t msg_virt_aim_point();
int32_t msg_virt_get_position(uint32_t msg_id);

void virt_update_positions();
void virt_kinematics();
VirtualPose virtualPoseForIk(const VirtualPose &pose);
int32_t virt_inverse_kinematics();
bool boomMotorUnitsToGeometricAngle(float motorUnits, float *boomDegrees);
bool boomGeometricAngleToMotorUnits(float boomDegrees, float *motorUnits);

void initMotor(Motor *m);
int32_t updateMotorVelocity(Motor *m, float timeSegment);
void clearAxisMove(AxisMoveData *axis);
int32_t calculateJogDestination(Motor *motor, int32_t target, bool *reverse);
void scheduleVirtualJogMotor(Motor *motor, int32_t target, uint16_t speed,
                             int32_t motorIndex);
CoordinatedMotionAxis makeCoordinatedJogAxis(Motor *motor, float target,
                                             uint16_t speed, uint8_t index);
void stopAll(uint8_t emergency);
void jogMotor(Motor *m, int32_t target, int32_t motorIndex);
void setMovePositionFrame(int32_t frame);
void positionFrame(int32_t frame);
void go(uint8_t *motorsMoving);
int32_t updateMotorVelocities();
int32_t stopMotor(Motor *motor, int32_t motorIndex, int8_t emergency);
void sendMotorPositions();
void clearGomoMove(GoMotionMove *move);
void setCamera(uint8_t cameraValue);
void setMotorDir(int32_t motorIndex, float dir);
int32_t getMotorDir(int32_t m);
void writeOutputMessage();
void transmitMessages();

uint8_t motorDataLoaded = 0;

struct DmcSharedData
{
  volatile uint32_t motorDataLoaded;
  volatile int64_t nextSpeed[MOTOR_CAM_COUNT];
  volatile int64_t accum[MOTOR_CAM_COUNT];
  volatile uint32_t motorDirection;
  volatile uint8_t cameraValue;
  volatile uint16_t cameraOpenAngle;
  volatile uint16_t cameraCloseAngle;
};

DmcSharedData *sharedData;
uintptr_t sharedDataPointer()
{
  return (uintptr_t)sharedData;
}

int8_t logicSwitchInput()
{
#ifdef LOGIC_SWITCH_PIN
  return !digitalRead(LOGIC_SWITCH_PIN);
#else
  return 0;
#endif
}

int8_t killSwitchInput()
{
#ifdef KILL_SWITCH_PIN
#ifdef KILL_SWITCH_NORMALLY_CLOSED
  return digitalRead(KILL_SWITCH_PIN);
#else
  return !digitalRead(KILL_SWITCH_PIN);
#endif
#else
  return 0;
#endif
}

void setCamera(uint8_t val)
{
  sharedData->cameraValue = val;
}



void setup()
{
  int m;

  sharedData = (DmcSharedData *)0x3800fd00;
  memset(sharedData, 0, sizeof(DmcSharedData));

  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial4.begin(115200);

  dbg_pin(PIN_DBG_0);
  dbg_pin(PIN_DBG_1);
  dbg_pin(PIN_DBG_2);
  dbg_pin(PIN_DBG_3);
  dbg_pin(PIN_DBG_4);
  dbg_pin(PIN_DBG_5);
  dbg_pin(PIN_DBG_6);
  dbg_pin(PIN_DBG_7);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

#ifdef LOGIC_SWITCH_PIN
  pinMode(LOGIC_SWITCH_PIN, INPUT_PULLUP);
#endif

  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  for (int i = 0; i < 3; ++i)
  {
    digitalWrite(LEDB, LOW);
    delay(200);
    digitalWrite(LEDB, HIGH);
    delay(200);
  }

#ifdef KILL_SWITCH_PIN
  pinMode(KILL_SWITCH_PIN, INPUT_PULLUP);
#endif
  killSwitchState = killSwitchInput();

  RPC.begin();

  RPC.bind("sharedDataPointer", sharedDataPointer);

  sharedData->motorDataLoaded = 0;
  sharedData->motorDirection = 0;
  for (m = 0; m < MOTOR_CAM_COUNT; ++m)
  {
    sharedData->nextSpeed[m] = 0;
  }

  stopAllLastTime = 0;
  exceptionCode = 0;
  motorsSendPosition = 0;
  moveState = MOVE_STATE_JOG;
  syncTriggers = 0;

  pinMode(LOGIC_OUT_0, OUTPUT);
  digitalWrite(LOGIC_OUT_0, LOW);
  pinMode(LOGIC_OUT_1, OUTPUT);
  digitalWrite(LOGIC_OUT_1, LOW);

  triggerOutState = (1 << 2);

  dmc_msg_init();
  messageQueue = 0;

  loadMoveState = MOVE_LOAD_NONE;

  // initialize move structure
  moveStartFrame = 1;
  moveFrameCount = 0;
  movePositionFrame = -1;
  for (m = 0; m < MOTOR_COUNT; ++m)
  {
    clearAxisMove(&move[m]);
  }

  memset(triggerData, 0, FRAME_COUNT);

  // initialize motor structures
  for (m = 0; m < MOTOR_CAM_COUNT; ++m)
  {
    initMotor(&motors[m]);

    sharedData->nextSpeed[m] = 0;
    setMotorDir(m, 1);

    SET_MOTOR_POSITION(m, 0);
  }
  motors[MOTOR_COUNT].config = DMC_MOTOR_CONFIG_ENABLED;
  motors[MOTOR_COUNT].maxAcceleration = 40000;
  motors[MOTOR_COUNT].maxVelocity = 40000;

  initMotor(&frameTimeMotor);
  frameTimeStopCounter = 0;
  playBlipStarted = 0;
  playBlipActive = 0;
  playBlipDuration = 0;
  playBlipLocation = 0;
  playBlipDmxChannel = 0;
  cameraTriggerCountdown = 0;

  clearGomoMove(&goMotionMove);

  setCamera(CAMERA_OFF);
  setTriggers(0);

  leds = 0;

  // clear virtuals
  memset(&_virtual, 0, sizeof(Virtual));

  // send hello message on startup
  sendHello(0);

  switchInput = logicSwitchInput();
}

void loop()
{
  int32_t updatedVelocities = updateMotorVelocities();
  uint16_t cmd = 0;

  uint8_t switchToggled = 0;
  int m;
  int i;
  Motor *motorPtr;
  int eStopOn = 0;

  transmitMessages();

  if (updatedVelocities)
  {
    if (usbLedCounter)
    {
      --usbLedCounter;
    }

    eStopOn = killSwitchInput();
    if (eStopOn != killSwitchState)
    {
      killSwitchState = eStopOn;
      digitalWrite(LEDR, killSwitchState ? LOW : HIGH);
    }

    if (logicSwitchInput())
    {
      ++switchInputCounter;
      if (switchInputCounter > 5)
      {
        switchInputCounter = 5;
        if (!switchInput)
        {
          switchInput = 1;
          switchToggled = 1;
        }
      }
    }
    else
    {
      --switchInputCounter;
      if (switchInputCounter < -5)
      {
        switchInputCounter = -5;
        if (switchInput)
        {
          switchInput = 0;
          switchToggled = 1;
        }
      }
    }

    if (switchToggled)
    {
      dmc_msg_prepare(DMC_MSG_GIO_IN, 0);
      dmc_msg_out_dword(switchInput);
      writeOutputMessage();
    }
    if (!hardStop)
    {
      if (motorsMoving && eStopOn) // add other conditions
      {
        hardStopCounter = 0;
        hardStop = 1;
        stopAll(1);

        messageQueue |= DMC_MSG_FLAG_MOTOR_HARD_STOP;
      }
    }
    else
    {
      hardStopCounter++;
      if (!motorsMoving && hardStopCounter > 100)
      {
        hardStop = 0;

        if (limitStopMotor)
        {
          limitStopMotor = 0;
        }
      }
    }

    motorsMoving = 0;

    for (m = 0; m < MOTOR_COUNT; ++m)
    {
      motorPtr = &motors[m];
      if (motorPtr->moving)
      {
        motorsMoving = 1;
        break;
      }
    }

    if (moveState == MOVE_STATE_ALL_JOG && frameTimeMotor.moving)
    {
      motorsMoving = 1;
    }

    if (playBlipActive)
    {
      --playBlipActive;

      if (playBlipLocation & 0x01)
      {
        if (playBlipActive)
          BIT_SET(_triggers, 0);
        else
          BIT_CLEAR(_triggers, 0);
      }
      if (playBlipLocation & 0x02)
      {
        if (playBlipActive)
          BIT_SET(_triggers, 1);
        else
          BIT_CLEAR(_triggers, 1);
      }
      setTriggers(_triggers);
    }
  }

  if ((usbLedCounter / 10) % 2)
  {
    digitalWrite(LEDB, LOW);
  }
  else
  {
    digitalWrite(LEDB, HIGH);
  }

  unsigned long msTime = millis();

  // send motor positions
  if (updatedVelocities)
  {
    if (lastPositionTime > msTime)
      lastPositionTime = 0; // rollover
    if ((motorsSendPosition && (moveState != MOVE_STATE_JOG || msTime - lastPositionTime > 100)) ||
        (motorsMoving && (msTime - lastPositionTime > 50)))
    {
      sendMotorPositions();
      motorsSendPosition = 0;
      msg_virt_get_position(0);
    }

    if (moveState == MOVE_STATE_ALL_JOG && !motorsMoving && goMotionMove.state == GO_MO_PREROLL)
    {
      moveState = MOVE_STATE_SHOOT_PREROLL;
      for (m = 0; m < MOTOR_COUNT; ++m)
      {
        motorPtr = &motors[m];
        if ((motorPtr->config & DMC_MOTOR_CONFIG_ENABLED) && !(motorPtr->config & DMC_MOTOR_CONFIG_LIVE_CONTROL))
        {
          calculatePointToPoint(motorPtr, (int32_t)goMotionMove.accelPosition[m], 0.0f);
        }
      }
      motorsMoving = 1;
    }
    else if (moveState == MOVE_STATE_SHOOT_PREROLL && !motorsMoving)
    {
      moveState = MOVE_STATE_SHOOT_WAIT;
      setTriggers(0);
    }

    if (moveState == MOVE_STATE_SHOOT_WAIT && switchToggled)
    {
      go(&motorsMoving);
      messageQueue |= DMC_MSG_FLAG_RT_GO;
    }

    if (cameraTriggerCountdown)
    {
      --cameraTriggerCountdown;
      if (!cameraTriggerCountdown)
      {
        setCamera(CAMERA_OFF);
      }
    }
  }

  {
    uint8_t len = 64;
    while (len && Serial.available())
    {

      int readData = Serial.read();
      Serial1.write(readData);

      --len;

      uint8_t csumValid = 0;
      uint32_t msgId = 0;
      uint16_t msgLength = 0;
      cmd = dmc_msg_process_character(readData, &csumValid, &msgId, &msgLength);
      if (cmd)
      {

        int32_t responseCode = DMC_ACK_OK;

        if (!csumValid)
        {
          responseCode = DMC_ACK_ERR_CHECKSUM;
        }
        else if (hardStop && cmd != DMC_MSG_MOTOR_GET_POSITION && cmd >= DMC_MSG_MOTOR_MOVE)
        {
          responseCode = DMC_ACK_ERR_HARD_LOW;
        }
        else
        {
          if (cmd == DMC_MSG_HI)
          {
            responseCode = 0;
            sendHello(msgId);
          }
          else if (cmd == DMC_MSG_GIO_OUT)
          {
            uint32_t gio = dmc_msg_read_dword();
            setTriggers(gio);
          }
          else if (cmd == DMC_MSG_GIO_CAM)
          {
            uint32_t cam = dmc_msg_read_dword();
            setCamera(cam);
          }
          else if (cmd == DMC_MSG_MOTOR_MOVE)
          {
            if (msgLength != 5)
            {
              responseCode = DMC_ACK_ERR_GENERAL;
            }
            else
            {
              int32_t motor    = dmc_msg_read_byte();
              int32_t position = dmc_msg_read_dword();
              responseCode = msg_motor_move(motor, position);
            }
          }
          else if (cmd == DMC_MSG_MOTOR_RESET_POSITION)
          {
            if (motorsMoving)
            {
              responseCode = DMC_ACK_ERR_MOVING;
            }
            else if (msgLength != 5)
            {
              responseCode = DMC_ACK_ERR_GENERAL;
            }
            else
            {
              int32_t motor = dmc_msg_read_byte();

              if (!IN_RANGE(motor, 1, MOTOR_COUNT))
              {
                responseCode = DMC_ACK_ERR_RANGE;
              }
              else
              {
                --motor;
                int32_t pos = dmc_msg_read_dword();
                motors[motor].position = pos;
                SET_MOTOR_POSITION(motor, pos);
                sendMotorPositions();
              }
            }
          }
          else if (cmd == DMC_MSG_MOTOR_STATUS)
          {
            uint32_t status = 0;

            for (i = 0; i < MOTOR_COUNT; ++i)
            {
              if (motors[i].moving)
                BIT_SET(status, i);
            }

            responseCode = 0;

            dmc_msg_prepare(DMC_MSG_MOTOR_STATUS, msgId);
            dmc_msg_out_dword(status);
            dmc_msg_out_byte(0); // DMX status
            writeOutputMessage();
          }
          else if (cmd == DMC_MSG_MOTOR_STOP)
          {
            uint8_t motor = dmc_msg_read_byte();
            responseCode = msg_motor_stop(motor);
          }
          else if (cmd == DMC_MSG_MOTOR_STOP_ALL)
          {
            uint32_t flags = 0;
            if (!dmc_msg_read_at_end())
              flags = dmc_msg_read_dword();
            if (motorsMoving && !flags)
            {
              hardStopCounter = 0;
              hardStop = 1;
            }
            stopAll(0);
          }
          else if (cmd == DMC_MSG_MOTOR_JOG)
          {
            uint8_t motor  = dmc_msg_read_byte();
            uint16_t speed = dmc_msg_read_word();
            int32_t dest   = dmc_msg_read_dword();
            responseCode   = msg_motor_jog(motor, speed, dest);
          }
          else if (cmd == DMC_MSG_MOTOR_SET_SPEED)
          {
            int32_t motor = dmc_msg_read_byte();
            if (!IN_RANGE(motor, 1, MOTOR_COUNT))
            {
              responseCode = DMC_ACK_ERR_RANGE;
            }
            else
            {
              --motor;
              motorPtr = &motors[motor];

              motorPtr->maxVelocity = fmaxf(10.0f, (float)dmc_msg_read_dword());
              motorPtr->maxAcceleration = dmc_msg_read_dword();
              if (motorPtr->maxAcceleration == 0)
              {
                motorPtr->maxAcceleration = motorPtr->maxVelocity * 0.5f;
              }
            }
          }
          else if (cmd == DMC_MSG_MOTOR_CONFIGURE)
          {
            int32_t motor = dmc_msg_read_byte();
            if (!IN_RANGE(motor, 1, MOTOR_COUNT))
            {
              responseCode = DMC_ACK_ERR_RANGE;
            }
            else
            {
              --motor;
              motors[motor].config = dmc_msg_read_byte();
            }
          }
          else if (cmd == DMC_MSG_MOTOR_SET_LIMITS)
          {
            int32_t motor = dmc_msg_read_byte();
            if (!IN_RANGE(motor, 1, MOTOR_COUNT))
            {
              responseCode = DMC_ACK_ERR_RANGE;
            }
            else
            {
              --motor;
              motorPtr = &motors[motor];

              motorPtr->limitLowEnabled = dmc_msg_read_byte();
              motorPtr->limitLow = dmc_msg_read_dword();
              motorPtr->limitHighEnabled = dmc_msg_read_byte();
              motorPtr->limitHigh = dmc_msg_read_dword();

              dmc_msg_read_byte(); // HW_SET: physical limit switch input this motor is wired to
                                   // (0 = none, 1-127 = input #N, 0x80 flag swaps high/low sense
                                   // for reversed motor orientation). Skipped — no dedicated
                                   // hardware limit inputs on this board.
            }
          }
          else if (cmd == DMC_MSG_MOTOR_GET_POSITION)
          {
            sendMotorPositions();
          }
          else if (cmd == DMC_MSG_RT_UPLOAD_MOVE_BEGIN)
          {
            loadMoveState = MOVE_LOAD_FRAME;

            triggerMask = 0;
            memset(triggerData, 0, FRAME_COUNT);

            moveStartFrame = dmc_msg_read_dword();
            moveFrameCount = 1 + (dmc_msg_read_dword() - moveStartFrame);

            for (m = 0; m < MOTOR_COUNT; ++m)
            {
              clearAxisMove(&move[m]);
              move[m].frameCount = moveFrameCount;
              move[m].position[0] = 0;
              move[m].position[1] = 0;
            }
          }
          else if (cmd == DMC_MSG_RT_UPLOAD_MOVE_AXIS)
          {
            int32_t motor = dmc_msg_read_byte();
            uint32_t index = dmc_msg_read_dword();
            uint32_t finalData = (index & DMC_DMX_FLAG_FINAL_SET);
            index &= ~DMC_DMX_FLAG_FINAL_SET;

            if (!IN_RANGE(motor, 1, MOTOR_COUNT) || !IN_RANGE((int32_t)index, 0, moveFrameCount))
            {
              responseCode = DMC_ACK_ERR_RANGE;
            }
            else
            {
              --motor;
              index++;

              int32_t *positions = move[motor].position;
              while (!dmc_msg_read_at_end())
              {
                positions[index++] = dmc_msg_read_dword();
                if ((int32_t)index > moveFrameCount)
                  break;
              }
              if (finalData)
              {
                const int32_t finalPos = positions[index - 1];
                while ((int32_t)index <= moveFrameCount)
                  positions[index++] = finalPos;
              }
            }
          }
          else if (cmd == DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS)
          {
            triggerMask = dmc_msg_read_dword();

            while (!dmc_msg_read_at_end())
            {
              uint32_t index = dmc_msg_read_dword();
              uint32_t value = dmc_msg_read_dword();

              if ((int32_t)index < moveFrameCount)
              {
                triggerData[index + 1] = value; // let's use 1-based to match everything else
              }
            }
          }
          else if (cmd == DMC_MSG_RT_UPLOAD_MOVE_END)
          {
            if (loadMoveState == MOVE_LOAD_FRAME)
            {
              for (m = 0; m < MOTOR_COUNT; ++m)
              {
                int32_t dx = move[m].position[2] - move[m].position[1];
                move[m].position[0] = move[m].position[1] - dx; // calculate frame 0 (before move start)
              }
            }
            loadMoveState = MOVE_LOAD_NONE;
          }
          else if (cmd == DMC_MSG_RT_POSITION_FRAME)
          {
            int32_t frame = dmc_msg_read_dword();

            playBlipLocation = 0;

            if (moveState == MOVE_STATE_SHOOT)
            {
              responseCode = DMC_ACK_ERR_MOVING;
            }
            else if (moveFrameCount)
            {
              positionFrame(frame);
            }
            else
            {
              responseCode = DMC_ACK_ERR_GENERAL;
            }
          }
          else if (cmd == DMC_MSG_RT_RUN_MOVE)
          {
            if (motorsMoving && moveState != MOVE_STATE_ALL_JOG)
            {
              responseCode = DMC_ACK_ERR_MOVING;
            }
            else if (movePositionFrame == -1)
            {
              responseCode = DMC_ACK_ERR_NOT_IN_POSITION;
            }
            else
            {
              clearGomoMove(&goMotionMove);

              float fps = dmc_msg_read_dword() * 0.001f;
              int32_t startFrame = dmc_msg_read_dword();
              int32_t endFrame = dmc_msg_read_dword();

              float preRollTime = dmc_msg_read_dword() * 0.001f;
              float postRollTime = dmc_msg_read_dword() * 0.001f;

              syncTriggers = dmc_msg_read_byte() ? 1 : 0;

              playBlipLocation = dmc_msg_read_dword();
              playBlipActive = 0;
              playBlipStarted = 0;

              playBlipDmxChannel = dmc_msg_read_word();
              playBlipDuration = dmc_msg_read_word() / 5;
              if (playBlipDuration == 0)
                playBlipDuration = 20;

              goMotionMove.flags = dmc_msg_read_word();
              if (goMotionMove.flags & (DMC_RT_PLAYBACK_PING_PONG | DMC_RT_PLAYBACK_LOOP))
              {
                postRollTime = 0;
              }

              if (goMotionMove.flags & DMC_RT_CAMERA_STILLS)
              {
                sharedData->cameraOpenAngle = dmc_msg_read_word();
                sharedData->cameraCloseAngle = dmc_msg_read_word();
                if (sharedData->cameraOpenAngle >= sharedData->cameraCloseAngle)
                {
                  sharedData->cameraOpenAngle = 1024;
                  sharedData->cameraCloseAngle = 2048;
                }
                else
                {
                  sharedData->cameraOpenAngle = (sharedData->cameraOpenAngle * 4095L) / 360;
                  sharedData->cameraCloseAngle = (sharedData->cameraCloseAngle * 4095L) / 360;
                }
              }
              else
              {
                sharedData->cameraOpenAngle = 0;
                sharedData->cameraCloseAngle = 0;
              }

              calculateRunLive(motors, move, &goMotionMove, startFrame, endFrame, fps, preRollTime, postRollTime);

              for (m = 0; m < MOTOR_COUNT; ++m)
              {
                if ((motors[m].config & DMC_MOTOR_CONFIG_ENABLED) &&
                    goMotionMove.accelPosition[m] != motors[m].position)
                {
                  if ((motors[m].limitLowEnabled && goMotionMove.accelPosition[m] < motors[m].limitLow) ||
                      (motors[m].limitHighEnabled && goMotionMove.accelPosition[m] > motors[m].limitHigh))
                  {
                    responseCode = DMC_ACK_ERR_PREROLL;
                    break;
                  }
                  int32_t finalPos =
                    (int32_t)(goMotionMove.decelPosition[m] + goMotionMove.decelVelocity[m] * (postRollTime * 0.5f));
                  if ((motors[m].limitLowEnabled && finalPos < motors[m].limitLow) ||
                      (motors[m].limitHighEnabled && finalPos > motors[m].limitHigh))
                  {
                    responseCode = DMC_ACK_ERR_POSTROLL;
                    break;
                  }
                }
              }

              if (responseCode == DMC_ACK_OK)
              {
                // jog all motors back to start frame
                if (moveState != MOVE_STATE_ALL_JOG)
                {
                  frameTimeMotor.position =
                    FRAME_TO_POSITION(movePositionFrame); // only reset frame position if we've been jogging
                }
                frameTimeMotor.maxAcceleration = FRAME_TO_POSITION(4);
                frameTimeMotor.maxVelocity = FRAME_TO_POSITION(fps);

                moveState = MOVE_STATE_ALL_JOG;

                if (startFrame < moveStartFrame)
                  startFrame = moveStartFrame;
                if (startFrame > moveStartFrame + moveFrameCount - 1)
                  startFrame = moveStartFrame + moveFrameCount - 1;

                frameTimeStopCounter = 0;
                calculatePointToPoint(&frameTimeMotor, FRAME_TO_POSITION(startFrame), 0.0f);

                motorsMoving = 1;
              }
            }
          }
          else if (cmd == DMC_MSG_RT_SHOOT_FRAME)
          {
            memset(goMotionOverride, 0, sizeof(GoMotionOverride) * MOTOR_COUNT);

            // sf [frame] [dir] [exposure time ms] [blur rate]
            int32_t frame = dmc_msg_read_dword();
            if (!IN_RANGE(frame, moveStartFrame, moveStartFrame + moveFrameCount - 1))
            {
              responseCode = DMC_ACK_ERR_RANGE;
            }
            else if (motorsMoving)
            {
              responseCode = DMC_ACK_ERR_MOVING;
            }
            else
            {
              frame = 1 + (frame - moveStartFrame);

              int32_t dir = dmc_msg_read_byte();
              int32_t exposure = dmc_msg_read_dword();
              int32_t blur = dmc_msg_read_word();

              clearGomoMove(&goMotionMove);

              if (blur == 0)
              {
                blur = 1000;
              }

              while (!dmc_msg_read_at_end())
              {
                int32_t channel = dmc_msg_read_byte() - 1;
                goMotionOverride[channel].enabled = 1;
                int32_t posA = dmc_msg_read_dword();
                int32_t posB = dmc_msg_read_dword();
                goMotionOverride[channel].posA = posA;
                goMotionOverride[channel].posB = posB;
              }

              if (dir == 0)
                dir = -1;

              calculateGoMotionMove(motors, move, &goMotionMove, frame, dir, exposure, blur, goMotionOverride);

              moveState = MOVE_STATE_SHOOT_PREROLL;
              for (m = 0; m < MOTOR_COUNT; ++m)
              {
                if ((motors[m].config & DMC_MOTOR_CONFIG_ENABLED) &&
                    !(motors[m].config & DMC_MOTOR_CONFIG_LIVE_CONTROL) && (motors[m].config & DMC_MOTOR_CONFIG_BLUR) &&
                    goMotionMove.accelPosition[m] != motors[m].position)
                {
                  calculatePointToPoint(&motors[m], (int32_t)goMotionMove.accelPosition[m], 0.5f);
                }
              }
            }
          }
          else if (cmd == DMC_MSG_RT_SHOOT_FRAME2)
          {
            memset(goMotionOverride, 0, sizeof(GoMotionOverride) * MOTOR_COUNT);

            int32_t frame = dmc_msg_read_dword();
            if (!IN_RANGE(frame, moveStartFrame, moveStartFrame + moveFrameCount - 1))
            {
              responseCode = DMC_ACK_ERR_RANGE;
            }
            else if (motorsMoving)
            {
              responseCode = DMC_ACK_ERR_MOVING;
            }
            else
            {
              int32_t exposure = dmc_msg_read_dword();
              int16_t shutterOpen = dmc_msg_read_word();
              int16_t shutterClose = dmc_msg_read_word();

              clearGomoMove(&goMotionMove);

              if (exposure == 0)
              {
                exposure = 1000;
              }

              while (!dmc_msg_read_at_end())
              {
                int32_t channel = dmc_msg_read_byte() - 1;
                int32_t posA = dmc_msg_read_dword();
                int32_t posB = dmc_msg_read_dword();
                if (channel >= 0 && channel < MOTOR_COUNT)
                {
                  goMotionOverride[channel].enabled = 1;
                  goMotionOverride[channel].posA = posA;
                  goMotionOverride[channel].posB = posB;
                }
              }

              calculateGoMotionMove2(motors, move, &goMotionMove, frame, exposure, shutterOpen, shutterClose,
                                     goMotionOverride);

              moveState = MOVE_STATE_SHOOT_PREROLL;
              for (m = 0; m < MOTOR_COUNT; ++m)
              {
                if ((motors[m].config & DMC_MOTOR_CONFIG_ENABLED) &&
                    !(motors[m].config & DMC_MOTOR_CONFIG_LIVE_CONTROL) && (motors[m].config & DMC_MOTOR_CONFIG_BLUR) &&
                    goMotionMove.accelPosition[m] != motors[m].position)
                {
                  calculatePointToPoint(&motors[m], (int32_t)goMotionMove.accelPosition[m], 0.0f);
                }
              }
            }
          }
          else if (cmd == DMC_MSG_RT_GO)
          {
            if (moveState == MOVE_STATE_SHOOT_WAIT)
            {
              go(&motorsMoving);
            }
            else
            {
              responseCode = DMC_ACK_ERR_NOT_IN_POSITION;
            }
          }
          else if (cmd == DMC_MSG_RT_STOP_LOOP)
          {
            goMotionMove.flags &= ~(DMC_RT_PLAYBACK_PING_PONG | DMC_RT_PLAYBACK_LOOP); // stop after next loop iteration
          }
          else if (cmd == DMC_MSG_RT_JOG_ALL)
          {
            if (motorsMoving && moveState != MOVE_STATE_ALL_JOG)
            {
              responseCode = DMC_ACK_ERR_MOVING;
            }
            else if (movePositionFrame == -1)
            {
              responseCode = DMC_ACK_ERR_NOT_IN_POSITION;
            }
            else
            {
              // setup frameTimeMotor to represent time
              if (moveState != MOVE_STATE_ALL_JOG)
              {
                frameTimeMotor.position =
                  FRAME_TO_POSITION(movePositionFrame); // only reset frame position if we've been jogging
              }
              frameTimeMotor.maxAcceleration = FRAME_TO_POSITION(4);
              frameTimeMotor.maxVelocity = FRAME_TO_POSITION(dmc_msg_read_dword() * 0.001f);

              moveState = MOVE_STATE_ALL_JOG;

              frameTimeStopCounter = 0;
              calculatePointToPoint(&frameTimeMotor, FRAME_TO_POSITION(dmc_msg_read_dword()), 0.0f);
            }
          }
          else if (cmd == DMC_MSG_VIRT_CONFIG) {
            // read type
            uint8_t type = dmc_msg_read_byte();
            if (type == DMC_VIRT_TYPE_NONE) {
              // no config data. reset virtual state
              memset(&_virtual, 0, sizeof(Virtual));
              for (uint32_t index = 0; index < MOTOR_COUNT; ++index) {
                Motor *motor  = &motors[index];
                motor->config &= ~(DMC_MOTOR_CONFIG_VIRT);
              }
            }
            else if (type == DMC_VIRT_TYPE_BOOM_SWING_TRACK) {
              // #TODO: add range validation to motor indices.
              bool config_good = false;
              do {
                _virtual.boomIndex     = (uint8_t)dmc_msg_read_dword();
                if (!IN_RANGE(_virtual.boomIndex, 1, MOTOR_COUNT)) {
                  break;
                }
                Motor *motorPtr        = &motors[_virtual.boomIndex - 1];
                motorPtr->SPU          = (float)dmc_msg_read_dword() / VIRT_SPU_SCALE;
                _virtual.NS            = (float)dmc_msg_read_dword() / VIRT_SCALE;
                motorPtr->config      |= DMC_MOTOR_CONFIG_VIRT;

                _virtual.swingIndex    = (uint8_t)dmc_msg_read_dword();
                if (!IN_RANGE(_virtual.swingIndex, 1, MOTOR_COUNT)) {
                  break;
                }
                motorPtr               = &motors[_virtual.swingIndex - 1];
                motorPtr->SPU          = (float)dmc_msg_read_dword() / VIRT_SPU_SCALE;
                _virtual.EW            = (float)dmc_msg_read_dword() / VIRT_SCALE;
                motorPtr->config      |= DMC_MOTOR_CONFIG_VIRT;

                _virtual.trackIndex    = (uint8_t)dmc_msg_read_dword();
                if (!IN_RANGE(_virtual.trackIndex, 1, MOTOR_COUNT)) {
                  break;
                }
                motorPtr               = &motors[_virtual.trackIndex - 1];
                motorPtr->SPU          = (float)dmc_msg_read_dword() / VIRT_SPU_SCALE;
                _virtual.track         = (float)dmc_msg_read_dword() / VIRT_SCALE;
                motorPtr->config      |= DMC_MOTOR_CONFIG_VIRT;

                _virtual.panIndex      = (uint8_t)dmc_msg_read_dword();
                if (!IN_RANGE(_virtual.panIndex, 1, MOTOR_COUNT)) {
                  break;
                }
                motorPtr               = &motors[_virtual.panIndex - 1];
                motorPtr->SPU          = (float)dmc_msg_read_dword() / VIRT_SPU_SCALE;
                _virtual.pan           = (float)dmc_msg_read_dword() / VIRT_SCALE;
                motorPtr->config      |= DMC_MOTOR_CONFIG_VIRT;

                _virtual.tiltIndex     = (uint8_t)dmc_msg_read_dword();
                if (!IN_RANGE(_virtual.tiltIndex, 1, MOTOR_COUNT)) {
                  break;
                }
                motorPtr               = &motors[_virtual.tiltIndex - 1];
                motorPtr->SPU          = (float)dmc_msg_read_dword() / VIRT_SPU_SCALE;
                _virtual.tilt          = (float)dmc_msg_read_dword() / VIRT_SCALE;
                motorPtr->config      |= DMC_MOTOR_CONFIG_VIRT;

                _virtual.rollIndex     = (uint8_t)dmc_msg_read_dword();
                if (!IN_RANGE(_virtual.rollIndex, 1, MOTOR_COUNT)) {
                  break;
                }
                motorPtr               = &motors[_virtual.rollIndex - 1];
                motorPtr->SPU          = (float)dmc_msg_read_dword() / VIRT_SPU_SCALE;
                _virtual.roll          = (float)dmc_msg_read_dword() / VIRT_SCALE;
                motorPtr->config      |= DMC_MOTOR_CONFIG_VIRT;

                _virtual.boomLength    = (float)dmc_msg_read_dword() / LEN_SCALE;
                _virtual.boomExtension = (float)dmc_msg_read_dword() / LEN_SCALE;
                _virtual.boomDisplacement = _virtual.boomLength + _virtual.boomExtension;
  
                _virtual.nodalOffsetX = (float)dmc_msg_read_dword() / LEN_SCALE;
                _virtual.nodalOffsetY = (float)dmc_msg_read_dword() / LEN_SCALE;
                _virtual.nodalOffsetZ = (float)dmc_msg_read_dword() / LEN_SCALE;

                _virtual.boomCompensationEnabled = 0;
                const int32_t compensationBytes =
                  DMC_VIRT_CONFIG_BOOM_COMPENSATION_ANGLES * (int32_t)sizeof(uint32_t);
                if (dmc_msg_read_left() >= compensationBytes) {
                  // Dragonframe sends signed boom motor-axis positions in
                  // VIRT_SCALE fixed-point units for -60 through +60 degrees.
                  for (int i = 0; i < DMC_VIRT_CONFIG_BOOM_COMPENSATION_ANGLES; ++i) {
                    _virtual.boomCompensation.motorUnits[i] =
                      (float)(int32_t)dmc_msg_read_dword() / VIRT_SCALE;
                  }
                  if (!limnmoco::boom_compensation_table_is_valid(
                        _virtual.boomCompensation)) {
                    break;
                  }
                  _virtual.boomCompensationEnabled = 1;
                }

                if (dmc_msg_read_left() >= (int32_t)sizeof(uint32_t)) {
                  // read safe distance
                  _virtual.safeDistance = (float)dmc_msg_read_dword() / LEN_SCALE;
                }
                // Dragonframe may append protocol-extension fields after the known
                // boom-compensation table and safe-distance fields. They do not
                // affect this firmware version, so deliberately leave them unread.

                // #TODO: support camera aim point
                _virtual.aimX = 0;
                _virtual.aimY = 0;
                _virtual.aimZ = 0;
                _virtual.aimEnabled = 0;

                _virtual.virtualOrigin = VirtualPose{
                  _virtual.track, _virtual.EW, _virtual.NS,
                  _virtual.pan, _virtual.tilt, _virtual.roll};

                float originBoomDegrees = 0.0f;
                if (!boomMotorUnitsToGeometricAngle(
                      (float)(motors[_virtual.boomIndex - 1].position /
                              motors[_virtual.boomIndex - 1].SPU),
                      &originBoomDegrees)) {
                  break;
                }
                _virtual.fkOrigin = solve_fk(
                  originBoomDegrees,
                  (float)(motors[_virtual.swingIndex - 1].position /
                          motors[_virtual.swingIndex - 1].SPU),
                  (float)(motors[_virtual.trackIndex - 1].position /
                          motors[_virtual.trackIndex - 1].SPU),
                  (float)(motors[_virtual.panIndex - 1].position /
                          motors[_virtual.panIndex - 1].SPU),
                  (float)(motors[_virtual.tiltIndex - 1].position /
                          motors[_virtual.tiltIndex - 1].SPU),
                  (float)(motors[_virtual.rollIndex - 1].position /
                          motors[_virtual.rollIndex - 1].SPU),
                  CraneGeometry{_virtual.boomLength, _virtual.boomExtension,
                                _virtual.nodalOffsetX, _virtual.nodalOffsetY,
                                _virtual.nodalOffsetZ});
                _virtual.fkOriginValid = 1;

                config_good = true;
              } while (false);

              responseCode = config_good ? DMC_ACK_OK : DMC_ACK_ERR_GENERAL;
            } else {
              // we don't support:
              // DMC_VIRT_TYPE_SWING_PAN
              // DMC_VIRT_TYPE_Y_SWING_TRACK
              // DMC_VIRT_TYPE_X_Y_Z
              responseCode = DMC_ACK_ERR_UNSUPPORTED;
            }
          }
          else if (cmd == DMC_MSG_VIRT_MOVE) {
            // a virtual move is defined by the coordination
            // of movement of multiple motors. So the basic
            // move motor command should be a good building
            // block for the implementation of virtual move.
            // we just need to consider which motors are involved
            // in the virtual move, and then translate that into
            // motor move commands for the involved motors.
            dbg(PIN_DBG_0);
            int32_t motor = dmc_msg_read_byte();
            int32_t position = dmc_msg_read_dword();
            responseCode = msg_virt_move(motor, position);
          }
          else if (cmd == DMC_MSG_VIRT_STOP) {
            dbg(PIN_DBG_1);
            uint8_t motor = dmc_msg_read_byte();
            responseCode = msg_virt_stop(motor);
          }
          else if (cmd == DMC_MSG_VIRT_JOG) {
            dbg(PIN_DBG_2);
            Serial4.write(0xA0);
            uint8_t motor  = dmc_msg_read_byte();
            uint16_t speed = dmc_msg_read_word();
            int32_t dest   = dmc_msg_read_dword();
            responseCode = msg_virt_jog(motor, speed, dest);
          }
          else if (cmd == DMC_MSG_VIRT_GET_POSITION) {
            msg_virt_get_position(msgId);
            responseCode = 0;
          }
          else if (cmd == DMC_MSG_VIRT_JOG_ON_LINE) {
            // #TODO:
            uint8_t  axis  = dmc_msg_read_byte();
            uint16_t speed = dmc_msg_read_word();

            msg_virt_jog_on_line(axis, speed);

            responseCode = DMC_ACK_OK;
          }
          else if (cmd == DMC_MSG_VIRT_AIM_POINT) {
            _virtual.aimEnabled = dmc_msg_read_byte();
            _virtual.aimX = dmc_msg_read_dword();
            _virtual.aimY = dmc_msg_read_dword();
            _virtual.aimZ = dmc_msg_read_dword();

            msg_virt_aim_point();

            dmc_msg_prepare(cmd, msgId);
            dmc_msg_out_byte(_virtual.aimEnabled);
            dmc_msg_out_dword(_virtual.aimX);
            dmc_msg_out_dword(_virtual.aimY);
            dmc_msg_out_dword(_virtual.aimZ);
            writeOutputMessage();
            responseCode = 0; // #NOTE: aim point response has already been written
          }
          else // unsupported
          {
            responseCode = DMC_ACK_ERR_UNSUPPORTED;
          }
        }

        if (responseCode)
        {
          dmc_msg_prepare(cmd | DMC_MSG_FLAG_ACK, msgId);
          dmc_msg_out_dword(responseCode);
          writeOutputMessage();
        }

        break; // exit if command was processed
      }
    }

    if (len != 64)
    {
      if (!usbLedCounter)
      {
        usbLedCounter = 20;
      }
      else if (usbLedCounter < 10)
      {
        usbLedCounter += 20;
      }
    }
  }
}

int32_t msg_motor_move(uint8_t motor, int32_t position) {
  if (!IN_RANGE(motor, 1, MOTOR_COUNT)) {
    return DMC_ACK_ERR_RANGE;
  }

  --motor;
  Motor *motorPtr = &motors[motor];
  if (moveState != MOVE_STATE_JOG
    && motorsMoving
    && !(motorPtr->config & DMC_MOTOR_CONFIG_LIVE_CONTROL)) {
    return DMC_ACK_ERR_MOVING;
  }

  if (position != motorPtr->position
    || motorPtr->moving) {
    calculatePointToPoint(&motors[motor], position, 0.0f);
    moveState = MOVE_STATE_JOG;
    syncTriggers = 0;
  } else {
    sendMotorPositions();
  }

  if (!(motorPtr->config & DMC_MOTOR_CONFIG_LIVE_CONTROL)) {
    movePositionFrame = -1;
  }

  return DMC_ACK_OK;
}

int32_t msg_motor_stop(uint8_t motor) {
  if (!IN_RANGE(motor, 1, MOTOR_COUNT)) {
    return DMC_ACK_ERR_RANGE;
  }

  --motor;
  Motor *motorPtr = &motors[motor];
  if (moveState == MOVE_STATE_JOG
  || (motorPtr->config & DMC_MOTOR_CONFIG_LIVE_CONTROL)) {
    stopMotor(motorPtr, motor, 0);
    if (!(motorPtr->config & DMC_MOTOR_CONFIG_LIVE_CONTROL)) {
      moveState = MOVE_STATE_JOG;
      syncTriggers = 0;
    }
  }

  return DMC_ACK_OK;
}

int32_t msg_motor_jog(uint8_t motor, uint16_t speed, int32_t dest) {
  if (!IN_RANGE(motor, 1, MOTOR_COUNT)) {
    return DMC_ACK_ERR_RANGE;
  }

  --motor;
  Motor *motorPtr = &motors[motor];
  if (moveState != MOVE_STATE_JOG
    && motorsMoving
    && !(motorPtr->config & DMC_MOTOR_CONFIG_LIVE_CONTROL)) {
    return DMC_ACK_ERR_MOVING;
  }

  float maxVelocity = motorPtr->maxVelocity;
  float maxAcceleration = motorPtr->maxAcceleration;
  float accelSeconds = maxVelocity / maxAcceleration;

  float speedAdjustment = speed * 0.0001f;
  motorPtr->maxVelocity = fmaxf(4.0f, (motorPtr->maxVelocity * speedAdjustment));
  motorPtr->maxAcceleration = fmaxf(4.0f, (motorPtr->maxAcceleration * speedAdjustment));
  motorPtr->maxAcceleration =
    fmaxf((float)motorPtr->maxAcceleration, fabsf(motorPtr->currentVelocity) / accelSeconds);

  jogMotor(motorPtr, dest, motor);

  motorPtr->maxVelocity = maxVelocity;
  motorPtr->maxAcceleration = maxAcceleration;

  if (!(motorPtr->config & DMC_MOTOR_CONFIG_LIVE_CONTROL)) {
    moveState = MOVE_STATE_JOG;
    movePositionFrame = -1;
    syncTriggers = 0;
  }

  return DMC_ACK_OK;
}

void virt_kinematics() {
  Motor *trackMotor = &motors[_virtual.trackIndex - 1];
  Motor *swingMotor = &motors[_virtual.swingIndex - 1];
  Motor *boomMotor  = &motors[_virtual.boomIndex  - 1];
  Motor *panMotor   = &motors[_virtual.panIndex   - 1];
  Motor *tiltMotor  = &motors[_virtual.tiltIndex  - 1];
  Motor *rollMotor  = &motors[_virtual.rollIndex  - 1];

  float swingDeg = (float)(swingMotor->position / swingMotor->SPU);
  float boomDeg = 0.0f;
  if (!boomMotorUnitsToGeometricAngle(
        (float)(boomMotor->position / boomMotor->SPU), &boomDeg)) {
    return;
  }
  float track    = (float)(trackMotor->position / trackMotor->SPU);
  float panDeg   = (float)(panMotor->position / panMotor->SPU);
  float tiltDeg  = (float)(tiltMotor->position / tiltMotor->SPU);
  float rollDeg  = (float)(rollMotor->position / rollMotor->SPU);

  Serial4.print("\nBFK");
  Serial4.print("sp");
  Serial4.print(swingMotor->position);
  Serial4.print("bp");
  Serial4.print(boomMotor->position);
  Serial4.print("Tp");
  Serial4.print(trackMotor->position);
  Serial4.print("pp");
  Serial4.print(panMotor->position);
  Serial4.print("tp");
  Serial4.print(tiltMotor->position);
  Serial4.print("rp");
  Serial4.print(rollMotor->position);
  Serial4.print("EW");
  Serial4.print(_virtual.EW);
  Serial4.print("NS");
  Serial4.print(_virtual.NS);
  Serial4.print("vp");
  Serial4.print(_virtual.pan);
  Serial4.print("vt");
  Serial4.print(_virtual.tilt);
  Serial4.print("vr");
  Serial4.print(_virtual.roll);
  Serial4.print("\n");

  VirtualPose fk = solve_fk(
    boomDeg, swingDeg, track, panDeg, tiltDeg, rollDeg,
    CraneGeometry{_virtual.boomLength, _virtual.boomExtension,
                  _virtual.nodalOffsetX, _virtual.nodalOffsetY, _virtual.nodalOffsetZ});

  if (_virtual.fkOriginValid) {
    _virtual.track = _virtual.virtualOrigin.vtrack +
                     (fk.vtrack - _virtual.fkOrigin.vtrack);
    _virtual.EW    = _virtual.virtualOrigin.vew +
                     (fk.vew - _virtual.fkOrigin.vew);
    _virtual.NS    = _virtual.virtualOrigin.vheight +
                     (fk.vheight - _virtual.fkOrigin.vheight);
    _virtual.pan   = _virtual.virtualOrigin.vpanDeg +
                     (fk.vpanDeg - _virtual.fkOrigin.vpanDeg);
    _virtual.tilt  = _virtual.virtualOrigin.vtiltDeg +
                     (fk.vtiltDeg - _virtual.fkOrigin.vtiltDeg);
    _virtual.roll  = _virtual.virtualOrigin.vrollDeg +
                     (fk.vrollDeg - _virtual.fkOrigin.vrollDeg);
  }

  Serial4.print("\nAFK");
  Serial4.print("sp");
  Serial4.print(swingMotor->position);
  Serial4.print("bp");
  Serial4.print(boomMotor->position);
  Serial4.print("Tp");
  Serial4.print(trackMotor->position);
  Serial4.print("pp");
  Serial4.print(panMotor->position);
  Serial4.print("tp");
  Serial4.print(tiltMotor->position);
  Serial4.print("rp");
  Serial4.print(rollMotor->position);
  Serial4.print("vT");
  Serial4.print(_virtual.track);
  Serial4.print("EW");
  Serial4.print(_virtual.EW);
  Serial4.print("NS");
  Serial4.print(_virtual.NS);
  Serial4.print("vp");
  Serial4.print(_virtual.pan);
  Serial4.print("vt");
  Serial4.print(_virtual.tilt);
  Serial4.print("vr");
  Serial4.print(_virtual.roll);
  Serial4.print("\n");
}

bool boomMotorUnitsToGeometricAngle(float motorUnits, float *boomDegrees) {
  if (!_virtual.boomCompensationEnabled) {
    *boomDegrees = motorUnits;
    return true;
  }
  return limnmoco::boom_motor_units_to_angle(
    _virtual.boomCompensation, motorUnits, boomDegrees);
}

bool boomGeometricAngleToMotorUnits(float boomDegrees, float *motorUnits) {
  if (!_virtual.boomCompensationEnabled) {
    *motorUnits = boomDegrees;
    return true;
  }
  return limnmoco::boom_angle_to_motor_units(
    _virtual.boomCompensation, boomDegrees, motorUnits);
}

VirtualPose virtualPoseForIk(const VirtualPose &pose) {
  if (!_virtual.fkOriginValid) {
    return pose;
  }

  return VirtualPose{
    _virtual.fkOrigin.vtrack + (pose.vtrack - _virtual.virtualOrigin.vtrack),
    _virtual.fkOrigin.vew + (pose.vew - _virtual.virtualOrigin.vew),
    _virtual.fkOrigin.vheight + (pose.vheight - _virtual.virtualOrigin.vheight),
    _virtual.fkOrigin.vpanDeg + (pose.vpanDeg - _virtual.virtualOrigin.vpanDeg),
    _virtual.fkOrigin.vtiltDeg + (pose.vtiltDeg - _virtual.virtualOrigin.vtiltDeg),
    _virtual.fkOrigin.vrollDeg + (pose.vrollDeg - _virtual.virtualOrigin.vrollDeg),
  };
}

int32_t virt_inverse_kinematics() {
  Serial4.write(0xA1);
  CraneSolveResult result = solve_ik(
    virtualPoseForIk(VirtualPose{
      _virtual.track, _virtual.EW, _virtual.NS,
      _virtual.pan, _virtual.tilt, _virtual.roll}),
    CraneGeometry{_virtual.boomLength, _virtual.boomExtension,
                  _virtual.nodalOffsetX, _virtual.nodalOffsetY, _virtual.nodalOffsetZ});

  if (result.boomClamped || result.swingClamped) {
    return DMC_ACK_ERR_RANGE;
  }

  _virtual.T = result.track;
  _virtual.s = result.swingDeg;
  _virtual.b = result.boomDeg;
  _virtual.p = _virtual.pan - result.swingDeg;
  _virtual.t = _virtual.tilt;
  _virtual.r = _virtual.roll;

  Serial4.print("\nIK");
  Serial4.print("T");
  Serial4.print(_virtual.T);
  Serial4.print("s");
  Serial4.print(_virtual.s);
  Serial4.print("b");
  Serial4.print(_virtual.b);
  Serial4.print("p");
  Serial4.print(_virtual.p);
  Serial4.print("t");
  Serial4.print(_virtual.t);
  Serial4.print("r");
  Serial4.print(_virtual.r);
  Serial4.print("\n");

  Motor *trackMotor = &motors[_virtual.trackIndex - 1];
  Motor *swingMotor = &motors[_virtual.swingIndex - 1];
  Motor *boomMotor  = &motors[_virtual.boomIndex  - 1];
  Motor *panMotor   = &motors[_virtual.panIndex   - 1];
  Motor *tiltMotor  = &motors[_virtual.tiltIndex  - 1];
  Motor *rollMotor  = &motors[_virtual.rollIndex  - 1];

  float boomMotorUnits = 0.0f;
  if (!boomGeometricAngleToMotorUnits(_virtual.b, &boomMotorUnits)) {
    return DMC_ACK_ERR_RANGE;
  }

  const CoordinatedMotionAxis axes[] = {
    {_virtual.trackIndex - 1,
      {trackMotor->position, _virtual.T * trackMotor->SPU,
       trackMotor->maxVelocity, trackMotor->maxAcceleration}},
    {_virtual.swingIndex - 1,
      {swingMotor->position, _virtual.s * swingMotor->SPU,
       swingMotor->maxVelocity, swingMotor->maxAcceleration}},
    {_virtual.boomIndex - 1,
      {boomMotor->position, boomMotorUnits * boomMotor->SPU,
       boomMotor->maxVelocity, boomMotor->maxAcceleration}},
    {_virtual.panIndex - 1,
      {panMotor->position, _virtual.p * panMotor->SPU,
       panMotor->maxVelocity, panMotor->maxAcceleration}},
    {_virtual.tiltIndex - 1,
      {tiltMotor->position, _virtual.t * tiltMotor->SPU,
       tiltMotor->maxVelocity, tiltMotor->maxAcceleration}},
    {_virtual.rollIndex - 1,
      {rollMotor->position, _virtual.r * rollMotor->SPU,
       rollMotor->maxVelocity, rollMotor->maxAcceleration}},
  };

  if (!coordinated_motion_start(motors, axes, sizeof(axes) / sizeof(axes[0]))) {
    return DMC_ACK_ERR_RANGE;
  }
  moveState = MOVE_STATE_JOG;
  movePositionFrame = -1;
  syncTriggers = 0;

  Serial4.write(0xA2);
  return DMC_ACK_OK;
}

// TODO: follow up with Dyami about adding a virtual move upload message so a
// full 6-DOF pose can be validated against the current crane configuration in
// one batch, rather than per-axis MSG_VIRT_MOVE calls.
int32_t msg_virt_move(uint8_t motor, int32_t position) {
  float target = ((float)position / VIRT_SCALE);
  if (motor == DMC_VIRT_TRACK) {
    _virtual.track = target;
  } else if (motor == DMC_VIRT_EW) {
    _virtual.EW    = target;
  } else if (motor == DMC_VIRT_NS) {
    _virtual.NS    = target;
  } else if (motor == DMC_VIRT_PAN) {
    _virtual.pan   = target;
  } else if (motor == DMC_VIRT_TILT) {
    _virtual.tilt  = target;
  } else if (motor == DMC_VIRT_ROLL) {
    _virtual.roll  = target;
  }

  return virt_inverse_kinematics();
}

int32_t msg_virt_stop(uint8_t motor) {
  // #NOTE: this code seems to work just fine. we are trying to reuse diyamis'
  //        code as much as possible, and interfacing with the stop command of 
  //        the motors within virtual movement appears to be fine for now.
  if (coordinated_motion_active()) {
    coordinated_motion_reset();
  }

  if (motor == DMC_VIRT_TRACK) {
    msg_motor_stop(_virtual.trackIndex);
    return DMC_ACK_OK;
  }

  if (motor == DMC_VIRT_EW) {
    msg_motor_stop(_virtual.swingIndex);
    msg_motor_stop(_virtual.trackIndex);
    msg_motor_stop(_virtual.panIndex);
    return DMC_ACK_OK;
  }

  if (motor == DMC_VIRT_NS) {
    msg_motor_stop(_virtual.boomIndex);
    msg_motor_stop(_virtual.trackIndex);
    return DMC_ACK_OK;
  }

  if (motor == DMC_VIRT_PAN) {
    msg_motor_stop(_virtual.panIndex);
    return DMC_ACK_OK;
  }

  if (motor == DMC_VIRT_TILT) {
    msg_motor_stop(_virtual.tiltIndex);
    return DMC_ACK_OK;
  }

  if (motor == DMC_VIRT_ROLL) {
    msg_motor_stop(_virtual.rollIndex);
    return DMC_ACK_OK;
  }

  return DMC_ACK_ERR_GENERAL;
}

int32_t msg_virt_jog(uint8_t motor, uint16_t speed, int32_t dest) {
  if (speed == 0 || !IN_RANGE(motor, DMC_VIRT_TRACK, DMC_VIRT_ROLL)) {
    return DMC_ACK_ERR_RANGE;
  }

  // Refresh the virtual pose from measured motor positions before choosing
  // the next look-ahead target. DESTINATION is a step target on the primary
  // physical axis; SPEED selects the existing jog velocity profile.
  virt_kinematics();

  Motor *primaryMotor = nullptr;
  uint8_t primaryIndex = 0;
  if (motor == DMC_VIRT_TRACK) {
    primaryIndex = _virtual.trackIndex;
  } else if (motor == DMC_VIRT_NS) {
    primaryIndex = _virtual.boomIndex;
  } else if (motor == DMC_VIRT_EW) {
    primaryIndex = _virtual.swingIndex;
  } else if (motor == DMC_VIRT_PAN) {
    primaryIndex = _virtual.panIndex;
  } else if (motor == DMC_VIRT_TILT) {
    primaryIndex = _virtual.tiltIndex;
  } else if (motor == DMC_VIRT_ROLL) {
    primaryIndex = _virtual.rollIndex;
  }
  primaryMotor = &motors[primaryIndex - 1];

  const float primaryMaxVelocity = primaryMotor->maxVelocity;
  const float primaryMaxAcceleration = primaryMotor->maxAcceleration;
  const float primaryAccelSeconds =
    primaryMaxVelocity / primaryMaxAcceleration;
  const float speedAdjustment = speed * 0.0001f;
  primaryMotor->maxVelocity =
    fmaxf(4.0f, primaryMaxVelocity * speedAdjustment);
  primaryMotor->maxAcceleration =
    fmaxf(4.0f, primaryMaxAcceleration * speedAdjustment);
  primaryMotor->maxAcceleration = fmaxf(
    primaryMotor->maxAcceleration,
    fabsf(primaryMotor->currentVelocity) / primaryAccelSeconds);

  bool reverse = false;
  const int32_t primaryTarget =
    calculateJogDestination(primaryMotor, dest, &reverse);
  primaryMotor->maxVelocity = primaryMaxVelocity;
  primaryMotor->maxAcceleration = primaryMaxAcceleration;
  if (reverse) {
    msg_virt_stop(motor);
    return DMC_ACK_OK;
  }

  Motor *trackMotor = &motors[_virtual.trackIndex - 1];
  Motor *swingMotor = &motors[_virtual.swingIndex - 1];
  Motor *boomMotor  = &motors[_virtual.boomIndex  - 1];
  Motor *panMotor   = &motors[_virtual.panIndex   - 1];
  Motor *tiltMotor  = &motors[_virtual.tiltIndex  - 1];
  Motor *rollMotor  = &motors[_virtual.rollIndex  - 1];

  const float currentSwing = (float)(swingMotor->position / swingMotor->SPU);
  float currentBoom = 0.0f;
  if (!boomMotorUnitsToGeometricAngle(
        (float)(boomMotor->position / boomMotor->SPU), &currentBoom)) {
    return DMC_ACK_ERR_RANGE;
  }
  const float currentTrack = (float)(trackMotor->position / trackMotor->SPU);
  const float currentPan   = (float)(panMotor->position / panMotor->SPU);
  const float currentTilt  = (float)(tiltMotor->position / tiltMotor->SPU);
  const float currentRoll  = (float)(rollMotor->position / rollMotor->SPU);

  const CraneGeometry geometry{
    _virtual.boomLength, _virtual.boomExtension,
    _virtual.nodalOffsetX, _virtual.nodalOffsetY, _virtual.nodalOffsetZ};
  const VirtualPose currentAbsolute = solve_fk(
    currentBoom, currentSwing, currentTrack,
    currentPan, currentTilt, currentRoll, geometry);

  float targetSwing = currentSwing;
  float targetBoom  = currentBoom;
  float targetTrack = currentTrack;
  float targetPan   = currentPan;
  float targetTilt  = currentTilt;
  float targetRoll  = currentRoll;
  if (motor == DMC_VIRT_TRACK) {
    targetTrack = (float)(primaryTarget / trackMotor->SPU);
  } else if (motor == DMC_VIRT_NS) {
    if (!boomMotorUnitsToGeometricAngle(
          (float)(primaryTarget / boomMotor->SPU), &targetBoom)) {
      return DMC_ACK_ERR_RANGE;
    }
  } else if (motor == DMC_VIRT_EW) {
    targetSwing = (float)(primaryTarget / swingMotor->SPU);
  } else if (motor == DMC_VIRT_PAN) {
    targetPan = (float)(primaryTarget / panMotor->SPU);
  } else if (motor == DMC_VIRT_TILT) {
    targetTilt = (float)(primaryTarget / tiltMotor->SPU);
  } else if (motor == DMC_VIRT_ROLL) {
    targetRoll = (float)(primaryTarget / rollMotor->SPU);
  }

  const VirtualPose targetAbsolute = solve_fk(
    targetBoom, targetSwing, targetTrack,
    targetPan, targetTilt, targetRoll, geometry);
  VirtualPose targetVirtual = VirtualPose{
    _virtual.track, _virtual.EW, _virtual.NS,
    _virtual.pan, _virtual.tilt, _virtual.roll};

  // Change only the requested virtual coordinate. IK then supplies the
  // compensating physical axes needed to keep the other virtual coordinates
  // fixed while the crane follows that virtual axis.
  if (motor == DMC_VIRT_TRACK) {
    targetVirtual.vtrack += targetAbsolute.vtrack - currentAbsolute.vtrack;
  } else if (motor == DMC_VIRT_EW) {
    targetVirtual.vew += targetAbsolute.vew - currentAbsolute.vew;
  } else if (motor == DMC_VIRT_NS) {
    targetVirtual.vheight += targetAbsolute.vheight - currentAbsolute.vheight;
  } else if (motor == DMC_VIRT_PAN) {
    targetVirtual.vpanDeg += targetPan - currentPan;
  } else if (motor == DMC_VIRT_TILT) {
    targetVirtual.vtiltDeg += targetTilt - currentTilt;
  } else if (motor == DMC_VIRT_ROLL) {
    targetVirtual.vrollDeg += targetRoll - currentRoll;
  }

  const CraneSolveResult result =
    solve_ik(virtualPoseForIk(targetVirtual), geometry);
  if (result.boomClamped || result.swingClamped) {
    return DMC_ACK_ERR_RANGE;
  }

  CoordinatedMotionAxis axes[3]{};
  uint8_t axisCount = 0;
  if (motor == DMC_VIRT_TRACK) {
    axes[axisCount++] = makeCoordinatedJogAxis(
      trackMotor, result.track * trackMotor->SPU, speed,
      _virtual.trackIndex - 1);
  } else if (motor == DMC_VIRT_NS) {
    float boomMotorUnits = 0.0f;
    if (!boomGeometricAngleToMotorUnits(result.boomDeg, &boomMotorUnits)) {
      return DMC_ACK_ERR_RANGE;
    }
    axes[axisCount++] = makeCoordinatedJogAxis(
      boomMotor, boomMotorUnits * boomMotor->SPU, speed,
      _virtual.boomIndex - 1);
    axes[axisCount++] = makeCoordinatedJogAxis(
      trackMotor, result.track * trackMotor->SPU, speed,
      _virtual.trackIndex - 1);
  } else if (motor == DMC_VIRT_EW) {
    axes[axisCount++] = makeCoordinatedJogAxis(
      swingMotor, result.swingDeg * swingMotor->SPU, speed,
      _virtual.swingIndex - 1);
    axes[axisCount++] = makeCoordinatedJogAxis(
      trackMotor, result.track * trackMotor->SPU, speed,
      _virtual.trackIndex - 1);
    axes[axisCount++] = makeCoordinatedJogAxis(
      panMotor, (targetVirtual.vpanDeg - result.swingDeg) * panMotor->SPU,
      speed, _virtual.panIndex - 1);
  } else if (motor == DMC_VIRT_PAN) {
    axes[axisCount++] = makeCoordinatedJogAxis(
      panMotor, (targetVirtual.vpanDeg - result.swingDeg) * panMotor->SPU,
      speed, _virtual.panIndex - 1);
  } else if (motor == DMC_VIRT_TILT) {
    axes[axisCount++] = makeCoordinatedJogAxis(
      tiltMotor, targetVirtual.vtiltDeg * tiltMotor->SPU, speed,
      _virtual.tiltIndex - 1);
  } else if (motor == DMC_VIRT_ROLL) {
    axes[axisCount++] = makeCoordinatedJogAxis(
      rollMotor, targetVirtual.vrollDeg * rollMotor->SPU, speed,
      _virtual.rollIndex - 1);
  }

  if (!coordinated_motion_start(motors, axes, axisCount)) {
    return DMC_ACK_ERR_RANGE;
  }
  moveState = MOVE_STATE_JOG;
  movePositionFrame = -1;
  syncTriggers = 0;

  return DMC_ACK_OK;
}

int32_t msg_virt_jog_on_line(uint8_t axis, uint16_t speed) {
  switch(axis) {
    case DMC_VIRT_JOG_ON_LINE_AXIS_X:
      // jogging in the X axis is equivalent to a East West move
      break;

    case DMC_VIRT_JOG_ON_LINE_AXIS_Y:
      // jogging in the Y axis is equivalent to a North SOuth move
      break;

    case DMC_VIRT_JOG_ON_LINE_AXIS_Z:
      // jogging in the Z axis is equivalent to a Forward Backward move
      break;

    case DMC_VIRT_JOG_ON_LINE_AXIS_PAN:
      // jogging along pan? does that mean a rotation about pan?
      // or is it another way of saying East west?
      break;

    case DMC_VIRT_JOG_ON_LINE_AXIS_TILT:
      // jogging along tilt? same questions as pan, tilt moves in 
      // the north south direction. 
      // if it's just rotate the camera, can't dragonframe just send a 
      // motor_move packet?
      // if it's move in the north south direction, can't dragonframe 
      // just send a jog with DMC_VIRT_NS as the target?
      // if it's a crane movement distinct from either, then how is the crane 
      // expected to move? 
      break;

    default:
      break;
  }

  return 0;
}

int32_t msg_virt_aim_point() {
}

int32_t msg_virt_get_position(uint32_t msg_id) {
    virt_kinematics();
    dmc_msg_prepare(DMC_MSG_VIRT_GET_POSITION, msg_id);
    dmc_msg_out_dword((int32_t)(_virtual.track * VIRT_SCALE));
    dmc_msg_out_dword((int32_t)(_virtual.EW * VIRT_SCALE));
    dmc_msg_out_dword((int32_t)(_virtual.NS * VIRT_SCALE));
    dmc_msg_out_dword((int32_t)(_virtual.pan * VIRT_SCALE));
    dmc_msg_out_dword((int32_t)(_virtual.tilt * VIRT_SCALE));
    dmc_msg_out_dword((int32_t)(_virtual.roll * VIRT_SCALE));
    writeOutputMessage();
}

void initMotor(Motor *m)
{
  memset(m, 0, sizeof(Motor));
  m->maxVelocity = 5000.0f;
  m->maxAcceleration = 20000.0f;
}

void clearGomoMove(GoMotionMove *move)
{
  frameTimeMotorDir = 1;
  memset(move, 0, sizeof(GoMotionMove));
}

void clearAxisMove(AxisMoveData *axisData)
{
  memset(axisData, 0, sizeof(AxisMoveData));
}

int32_t calculateJogDestination(Motor *motor, int32_t target, bool *reverse)
{
  float maxVelocity = motor->maxVelocity;
  float maxAcceleration = motor->maxAcceleration;
  float vi = motor->currentVelocity;

  int32_t dir = (target > motor->position) ? 1 : -1;
  *reverse = vi * dir < 0;
  if (*reverse)
    return motor->position;

  if (fabsf(target - motor->position) < 0.001f)
    return motor->position;

  // given current velocity vi
  // compute distance so that decel starts after 0.5 seconds
  // time to accel
  // time at maxvelocity
  // time to decel
  float accelTime = 0, atMaxVelocityTime = 0;
  float decelTime = 0.5f;
  if (fabsf(vi) < maxVelocity)
  {
    accelTime = (maxVelocity - fabsf(vi)) / maxAcceleration;
    if (accelTime < decelTime)
    {
      atMaxVelocityTime = decelTime - accelTime;
    }
    else
    {
      accelTime = decelTime;
    }
  }
  else
  {
    atMaxVelocityTime = decelTime;
  }
  float maxVelocityReached = fabsf(vi) + maxAcceleration * accelTime;

  float delta = fabsf(vi) * accelTime + (0.5f * maxAcceleration * accelTime * accelTime) +
                atMaxVelocityTime * maxVelocityReached +
                0.5f * (maxVelocityReached * maxVelocityReached) / maxAcceleration; // = 0.5 * a * t^2 -> t = (v/a)

  int32_t dest = lround(motor->position + dir * delta);

  // now clamp to target
  if ((dir == 1 && dest > target) || (dir == -1 && dest < target))
  {
    dest = target;
  }

  return dest;
}

void scheduleVirtualJogMotor(Motor *motor, int32_t target, uint16_t speed,
                             int32_t motorIndex)
{
  if (fabsf(target - motor->position) < 0.001f)
    return;

  float maxVelocity = motor->maxVelocity;
  float maxAcceleration = motor->maxAcceleration;
  float accelSeconds = maxVelocity / maxAcceleration;
  float speedAdjustment = speed * 0.0001f;

  motor->maxVelocity = fmaxf(4.0f, maxVelocity * speedAdjustment);
  motor->maxAcceleration = fmaxf(4.0f, maxAcceleration * speedAdjustment);
  motor->maxAcceleration = fmaxf(
    motor->maxAcceleration, fabsf(motor->currentVelocity) / accelSeconds);

  calculatePointToPoint(motor, target, 0.0f);

  motor->maxVelocity = maxVelocity;
  motor->maxAcceleration = maxAcceleration;
  moveState = MOVE_STATE_JOG;
  movePositionFrame = -1;
  syncTriggers = 0;
  (void)motorIndex;
}

CoordinatedMotionAxis makeCoordinatedJogAxis(Motor *motor, float target,
                                             uint16_t speed, uint8_t index)
{
  const float speedAdjustment = speed * 0.0001f;
  return CoordinatedMotionAxis{
    index,
    {motor->position, target,
     fmaxf(4.0f, motor->maxVelocity * speedAdjustment),
     fmaxf(4.0f, motor->maxAcceleration * speedAdjustment)}};
}

void jogMotor(Motor *motor, int32_t target, int32_t motorIndex)
{
  bool reverse = false;
  int32_t dest = calculateJogDestination(motor, target, &reverse);
  if (reverse)
  {
    stopMotor(motor, motorIndex, 0);
    return;
  }

  if (dest == motor->position)
    return;

  calculatePointToPoint(motor, dest, 0.0f);
}

void setMovePositionFrame(int32_t frame)
{
  movePositionFrame = frame;

  if (syncTriggers)
  {
    int32_t index = BOUND(1, 1 + (frame - moveStartFrame), moveFrameCount);

    if (triggerMask)
    {
      uint8_t value = triggerData[index];

      uint32_t maskOut = triggerMask;
      maskOut = ~maskOut;

      setTriggers((_triggers & maskOut) | value);
    }
  }
}

void go(uint8_t *motorsMoving)
{
  int32_t m;

  if (moveState == MOVE_STATE_SHOOT_WAIT)
  {
    moveState = MOVE_STATE_SHOOT;
    goMotionMove.state = GO_MO_PRE_ACCEL;
    for (m = 0; m < MOTOR_COUNT; ++m)
    {
      if ((motors[m].config & DMC_MOTOR_CONFIG_ENABLED) && !(motors[m].config & DMC_MOTOR_CONFIG_LIVE_CONTROL) &&
          (goMotionMove.mode == GO_MO_MODE_RUN_LIVE || (motors[m].config & DMC_MOTOR_CONFIG_BLUR)) &&
          move[m].frameCount)
      {
        *motorsMoving = 1;
        motors[m].moving = 1;
      }
    }
    if (goMotionMove.mode == GO_MO_MODE_SHOOTING || goMotionMove.mode == GO_MO_MODE_SHOOTING2)
    {
      setCamera(CAMERA_METER);
    }
    else
    {
      if (goMotionMove.flags & DMC_RT_CAMERA_VIDEO)
      {
        setCamera(CAMERA_METER | CAMERA_SHUTTER);
        cameraTriggerCountdown = 5;
      }
      playBlipStarted = 0;
      playBlipActive = playBlipDuration;
    }
  }
}

void positionFrame(int32_t frame)
{
  int32_t m;

  int32_t frameIndex = BOUND(1, 1 + (frame - moveStartFrame), moveFrameCount);

  moveState = MOVE_STATE_JOG;

  for (m = 0; m < MOTOR_COUNT; ++m)
  {
    if ((motors[m].config & DMC_MOTOR_CONFIG_ENABLED) && !(motors[m].config & DMC_MOTOR_CONFIG_LIVE_CONTROL) &&
        move[m].frameCount)
    {
      int32_t moveFrame = frameIndex > move[m].frameCount ? move[m].frameCount : frameIndex;
      calculatePointToPoint(&motors[m], move[m].position[moveFrame], 0.5f);
    }
  }

  motors[MOTOR_COUNT].currentVelocity = 0;
  motors[MOTOR_COUNT].position = frame * 4096;
  SET_MOTOR_POSITION(MOTOR_COUNT, motors[MOTOR_COUNT].position);

  setMovePositionFrame(frame);
}

void sendHello(uint32_t id)
{
  dmc_msg_prepare(DMC_MSG_HI, id);

  int i = 0;

  dmc_msg_out_byte('l');
  ++i;
  dmc_msg_out_byte('i');
  ++i;
  dmc_msg_out_byte('m');
  ++i;
  dmc_msg_out_byte('n');
  ++i;
  dmc_msg_out_byte('m');
  ++i;
  dmc_msg_out_byte('o');
  ++i;
  dmc_msg_out_byte('c');
  ++i;
  dmc_msg_out_byte('o');
  ++i;

  for (; i < 32; ++i)
    dmc_msg_out_byte(0);

  dmc_msg_out_byte(DMC_VERSION_MAJOR);
  dmc_msg_out_byte(DMC_VERSION_MINOR);
  dmc_msg_out_byte(DMC_VERSION_REV);
  dmc_msg_out_byte(MOTOR_COUNT);
  //dmc_msg_out_byte(32);
  dmc_msg_out_word(0);           // DMX channel count
  //dmc_msg_out_word(512);
  dmc_msg_out_byte(GIO_OUTPUTS); // GIO OUT count
  dmc_msg_out_byte(GIO_INPUTS);  // GIO IN count
  dmc_msg_out_byte(0);           // HW LIMIT SET count
  dmc_msg_out_dword(FRAME_COUNT);
  // NOTE: Monday July 20, 2026:
  //       Dragonframe rejects the device as not supporting virtuals when only DMC_CAP_VIRTUAL_BOOM_SWING_TRACK is
  //       set in the capabilities field. The current workaround is to report support for all virtuals flags, which 
  //       while it works, does imply that the device is more feature rich that it actually is. We need to follow 
  //       up with Diyami about what is going on in Dragonframe so we can provide a more specific capabilities response.
  dmc_msg_out_dword(DMC_CAP_REAL_TIME | DMC_CAP_GO_MOTION | DMC_CAP_GO_MOTION2 | DMC_CAP_COUPLE_MOTORS |
                    DMC_CAP_REAL_TIME_LOOP | DMC_CAP_REAL_TIME_CAMERA | DMC_CAP_VIRTUAL_BOOM_SWING_TRACK |
                    DMC_CAP_VIRTUAL_SWING_PAN | DMC_CAP_VIRTUAL_Y_SWING_TRACK | DMC_CAP_VIRTUAL_X_Y_Z); // capabilities
  dmc_msg_out_word(2); // protocol version

  writeOutputMessage();
}

void sendMotorPositions()
{
  lastPositionTime = millis();

  int i;

  dmc_msg_prepare(DMC_MSG_MOTOR_GET_POSITION, 0);
  dmc_msg_out_dword((uint32_t)(frameTimeMotor.position / 100));
  for (i = 0; i < MOTOR_COUNT; ++i)
    dmc_msg_out_dword((int32_t)(motors[i].position));
  writeOutputMessage();
}

void writeOutputMessage()
{
  uint16_t msgLength = 0;
  uint8_t *buf = dmc_msg_finalize_output(&msgLength);
  if (buf && msgLength)
  {
    if (msgLength < messageBuffer.availableForStore())
    {
      while (msgLength--)
      {
        messageBuffer.store_char(*buf++);
      }
    }
    // otherwise we are dropping the message. so sad!
  }
}

void transmitMessages()
{
  if ((messageQueue & DMC_MSG_FLAG_MOTOR_HARD_STOP) && messageBuffer.availableForStore() >= 24)
  {
    messageQueue &= ~DMC_MSG_FLAG_MOTOR_HARD_STOP;
    dmc_msg_prepare(DMC_MSG_MOTOR_HARD_STOP, 0);
    dmc_msg_out_byte(0);
    writeOutputMessage();
  }
  if ((messageQueue & DMC_MSG_FLAG_RT_GO) && messageBuffer.availableForStore() >= 24)
  {
    messageQueue &= ~DMC_MSG_FLAG_RT_GO;
    dmc_msg_prepare(DMC_MSG_RT_GO, 0);
    writeOutputMessage();
  }
  if ((messageQueue & DMC_MSG_FLAG_RT_END) && messageBuffer.availableForStore() >= 24)
  {
    messageQueue &= ~DMC_MSG_FLAG_RT_END;
    dmc_msg_prepare(DMC_MSG_RT_END, 0);
    writeOutputMessage();
  }

  uint16_t outMsgIndex = std::min(messageBuffer.available(), 32); // Serial.availableForWrite always returns 0

  if (!outMsgIndex)
    return;

  if (!usbLedCounter)
  {
    usbLedCounter = 20;
  }
  else if (usbLedCounter < 10)
  {
    usbLedCounter += 20;
  }

  // there is no consideration of which byte is the command byte! oh no!

  while (outMsgIndex--)
  {
    uint32_t c = messageBuffer.read_char();
    Serial.write(c);
    Serial2.write(c);
  }
}

void setTriggers(uint32_t triggers)
{
  digitalWrite(LOGIC_OUT_0, (triggers & 0x01) ? HIGH : LOW);
  digitalWrite(LOGIC_OUT_1, (triggers & 0x02) ? HIGH : LOW);
}

/*
 * Calculates the motor velocity and step count.
 */
int32_t updateMotorVelocities()
{
  if (sharedData->motorDataLoaded)
  {
    return 0;
  }

  int m = 0;
  float xn, dx;
  uint8_t shouldHardStop = 0;
  float t, tSqr;
  Motor *motor = 0;

  for (m = 0; m < MOTOR_COUNT; ++m)
  {
    motor = &motors[m];

    if (!motor->moving && motor->wasMoving)
    {
      float newPos = (float)(sharedData->accum[m] / double(0x100000000));
      if (!exceptionCode && fabsf(newPos - motor->position) > 10)
      {
        exceptionCode = 100;
        shouldHardStop = 1;

        dmc_msg_prepare(DMC_MSG_MOTOR_HARD_STOP, 0);
        dmc_msg_out_byte(exceptionCode);
        dmc_msg_out_byte(m + 1);
        writeOutputMessage();
      }
      motor->position = newPos;
    }

    motor->wasMoving = motor->moving;
    motor->currentVelocity = 0;
  }

  if (moveState == MOVE_STATE_JOG || moveState == MOVE_STATE_SHOOT_PREROLL)
  {
    if (coordinated_motion_active())
    {
      int32_t coordinatedDirections[MOTOR_COUNT] = {};
      coordinated_motion_update(motors, DATA_RATE_TIME_SEGMENT,
                                 coordinatedDirections);
      for (m = 0; m < MOTOR_COUNT; ++m)
      {
        setMotorDir(m, coordinatedDirections[m]);
      }
    }
    else
    {
      for (m = 0; m < MOTOR_COUNT; ++m)
      {
        motor = &motors[m];

        if (motor->moving)
        {
          int32_t dir = updateMotorVelocity(motor, DATA_RATE_TIME_SEGMENT);
          setMotorDir(m, dir);
        }
      }
    }
  }
  else if (moveState == MOVE_STATE_SHOOT)
  {
    t = goMotionMove.time + DATA_RATE_TIME_SEGMENT;
    goMotionMove.runningTime += DATA_RATE_TIME_SEGMENT;

    if (goMotionMove.mode == GO_MO_MODE_SHOOTING2)
    {
      if (goMotionMove.runningTime >= goMotionMove.shutterCloseTime)
      {
        if (sharedData->cameraValue)
          setCamera(CAMERA_OFF);
      }
      else if (goMotionMove.runningTime >= goMotionMove.shutterOpenTime && !(sharedData->cameraValue & CAMERA_SHUTTER))
      {
        setCamera(CAMERA_SHUTTER | CAMERA_METER);
      }
    }

    if (goMotionMove.state == GO_MO_PRE_ACCEL)
    {
      if (t >= goMotionMove.preAccelDuration)
      {
        goMotionMove.state = GO_MO_ACCEL;
        t -= goMotionMove.preAccelDuration;
      }
    }

    if (goMotionMove.state == GO_MO_ACCEL)
    {
      if (t < goMotionMove.accelDuration)
      {
        tSqr = t * t;
        for (m = 0; m < MOTOR_COUNT; ++m)
        {
          motor = &motors[m];
          if (motor->moving && !(motor->config & DMC_MOTOR_CONFIG_LIVE_CONTROL) &&
              (goMotionMove.mode == GO_MO_MODE_RUN_LIVE || (motor->config & DMC_MOTOR_CONFIG_BLUR)))
          {
            xn = (goMotionMove.accelPosition[m] +
                  goMotionMove.acceleration[m] * tSqr); // accel was already multiplied * 0.5

            dx = xn - motor->position;

            motor->currentVelocity = (float)dx * DATA_RATE;

            setMotorDir(m, dx);
            motor->position = xn;
          }
        }
      }
      else
      {
        movePositionFrame = -1; // to trigger update on next pos
        goMotionMove.state = GO_MO_MOVE;
        t -= goMotionMove.accelDuration;

        if (goMotionMove.mode == GO_MO_MODE_SHOOTING)
        {
          setCamera(CAMERA_METER | CAMERA_SHUTTER);
        }
      }
    }

    if (goMotionMove.state == GO_MO_MOVE)
    {
      if (t < goMotionMove.moveDuration)
      {
        float moveTime = goMotionMove.moveStartTime + t * goMotionMove.moveTimeSegment;
        if (goMotionMove.mode == GO_MO_MODE_RUN_LIVE)
        {
          frameTimeMotor.position = FRAME_TO_POSITION(moveTime);

          if ((int32_t)moveTime != movePositionFrame)
          {
            setMovePositionFrame((int32_t)moveTime);
            motorsSendPosition = 1;
          }
        }

        int motorCount = MOTOR_COUNT;
        if (goMotionMove.flags & DMC_RT_CAMERA_STILLS)
        {
          motorCount = MOTOR_CAM_COUNT;
          motors[MOTOR_COUNT].moving = 1;
        }
        else
        {
          motors[MOTOR_COUNT].currentVelocity = 0;
        }

        for (m = 0; m < motorCount; ++m)
        {
          motor = &motors[m];

          if (motor->moving && !(motor->config & DMC_MOTOR_CONFIG_LIVE_CONTROL) &&
              (goMotionMove.mode == GO_MO_MODE_RUN_LIVE || (motor->config & DMC_MOTOR_CONFIG_BLUR)))
          {
            if (m == MOTOR_COUNT)
            {
                xn = moveTime * 4096;
            }
            else if (goMotionOverride[m].enabled)
            {
              xn = (goMotionMove.moveP0[m] + goMotionMove.moveV[m] * t);
            }
            else
            {
              xn = evaluateMove(&move[m], moveTime);
            }
            dx = xn - motor->position;

            motor->currentVelocity = (float)dx * DATA_RATE;

            setMotorDir(m, dx);
            motor->position = xn;
          }
        }
      }
      else
      {
        float moveTime = goMotionMove.moveStartTime + goMotionMove.moveDuration * goMotionMove.moveTimeSegment;
        frameTimeMotor.position = FRAME_TO_POSITION(moveTime);
        goMotionMove.state = GO_MO_DECEL;

        motors[MOTOR_COUNT].currentVelocity = 0; // stop camera

        t -= goMotionMove.moveDuration;
        if (goMotionMove.mode == GO_MO_MODE_SHOOTING)
        {
          setCamera(CAMERA_OFF);
        }
        else if (goMotionMove.mode == GO_MO_MODE_RUN_LIVE)
        {
          playBlipActive = DATA_RATE / 10;
          sendMotorPositions();

          messageQueue |= DMC_MSG_FLAG_RT_END;
        }
      }
    }

    if (goMotionMove.state == GO_MO_DECEL)
    {
      if (t < goMotionMove.decelDuration)
      {
        tSqr = t * t;
        for (m = 0; m < MOTOR_COUNT; ++m)
        {
          motor = &motors[m];
          if (motor->moving && !(motor->config & DMC_MOTOR_CONFIG_LIVE_CONTROL) &&
              (goMotionMove.mode == GO_MO_MODE_RUN_LIVE || (motor->config & DMC_MOTOR_CONFIG_BLUR)))
          {
            xn = (goMotionMove.decelPosition[m] + goMotionMove.decelVelocity[m] * t +
                  goMotionMove.deceleration[m] * tSqr); // accel was already multiplied * 0.5

            dx = xn - motor->position;

            motor->currentVelocity = dx * DATA_RATE;

            setMotorDir(m, dx);
            motor->position = xn;
          }
        }
      }
      else
      {
        goMotionMove.state = GO_MO_POST_DECEL;
        t -= goMotionMove.decelDuration;
      }
    }

    if (goMotionMove.state == GO_MO_POST_DECEL)
    {
      if (t >= goMotionMove.postDecelDuration)
      {
        moveState = MOVE_STATE_JOG;
        goMotionMove.state = GO_MO_INACTIVE;

        for (m = 0; m < MOTOR_COUNT; ++m)
        {
          Motor *motor = &motors[m];
          if (!(motor->config & DMC_MOTOR_CONFIG_LIVE_CONTROL))
          {
            motor->moving = 0;
          }
        }
        motorsSendPosition = 1;

        if (goMotionMove.mode == GO_MO_MODE_RUN_LIVE)
        {
          uint8_t motorsMoving = 0;
          if (goMotionMove.flags & DMC_RT_PLAYBACK_PING_PONG)
          {
            calculateRunLivePingPong(&goMotionMove);
            moveState = MOVE_STATE_SHOOT_WAIT;
            t = 0;
            go(&motorsMoving);
          }
          else if (goMotionMove.flags & DMC_RT_PLAYBACK_LOOP)
          {
            calculateRunLiveLoop(&goMotionMove);
            moveState = MOVE_STATE_SHOOT_WAIT;
            t = 0;
            go(&motorsMoving);
          }
          else
          {
            positionFrame(lround(goMotionMove.moveEndTime));
          }
        }
        playBlipLocation = 0;

        if (goMotionMove.flags & DMC_RT_CAMERA_VIDEO)
        {
          setCamera(CAMERA_METER | CAMERA_SHUTTER);
          cameraTriggerCountdown = 5;
        }
        else
        {
          setCamera(CAMERA_OFF);
        }
      }
    }

    for (m = 0; m < MOTOR_COUNT; ++m)
    {
      motor = &motors[m];
      if ((motor->config & DMC_MOTOR_CONFIG_ENABLED) && (motor->config & DMC_MOTOR_CONFIG_LIVE_CONTROL) &&
          motor->moving)
      {
        int32_t dir = updateMotorVelocity(motor, DATA_RATE_TIME_SEGMENT);
        setMotorDir(m, dir);
      }
    }

    goMotionMove.time = t;
  }
  else if (moveState == MOVE_STATE_ALL_JOG)
  {
    int32_t dir = updateMotorVelocity(&frameTimeMotor, DATA_RATE_TIME_SEGMENT); // update time
    if (dir)
    {
      frameTimeMotorDir = (dir == 1) ? 1 : 0;
    }

    // run any 'live control' motors
    for (m = 0; m < MOTOR_COUNT; ++m)
    {
      motor = &motors[m];
      const uint8_t liveControlEnabled = (DMC_MOTOR_CONFIG_ENABLED | DMC_MOTOR_CONFIG_LIVE_CONTROL);
      if ((motor->config & liveControlEnabled) == liveControlEnabled && motor->moving)
      {
        int32_t dir = updateMotorVelocity(motor, DATA_RATE_TIME_SEGMENT);
        setMotorDir(m, dir);
      }
    }

    if (frameTimeMotor.moving)
    {
      t = POSITION_TO_FRAME(frameTimeMotor.position);

      if ((int32_t)t != movePositionFrame)
      {
        setMovePositionFrame((int32_t)t);
        motorsSendPosition = 1;
      }

      t = 1 + (t - moveStartFrame);

      for (m = 0; m < MOTOR_COUNT; ++m)
      {
        motor = &motors[m];
        if ((motor->config & DMC_MOTOR_CONFIG_ENABLED) && move[m].frameCount)
        {
          if (!(motor->config & DMC_MOTOR_CONFIG_LIVE_CONTROL))
          {
            xn = evaluateMove(&move[m], t);

            dx = xn - motor->position;

            motor->moving = 1;
            motor->currentVelocity = dx * DATA_RATE;

            setMotorDir(m, dx);

            motor->position = xn;
          }
        }
      }
    }
    else
    {
      // if frameTime is not moving, must stop motors
      for (m = 0; m < MOTOR_COUNT; ++m)
      {
        motor = &motors[m];
        if ((motor->config & DMC_MOTOR_CONFIG_ENABLED) && !(motor->config & DMC_MOTOR_CONFIG_LIVE_CONTROL))
        {
          motor->moving = 0;
        }
      }

      // if we no longer need to be in jog_all, change state to normal jog
      if (goMotionMove.state == GO_MO_INACTIVE)
      {
        moveState = MOVE_STATE_JOG;
      }
    }
  }
  else if (moveState == MOVE_STATE_SHOOT_WAIT)
  {
    for (m = 0; m < MOTOR_COUNT; ++m)
    {
      motor = &motors[m];
      if ((motor->config & DMC_MOTOR_CONFIG_ENABLED) && (motor->config & DMC_MOTOR_CONFIG_LIVE_CONTROL) &&
          motor->moving)
      {
        int32_t dir = updateMotorVelocity(motor, DATA_RATE_TIME_SEGMENT);
        setMotorDir(m, dir);
      }
    }
  }

  for (m = 0; m < MOTOR_CAM_COUNT; ++m)
  {
    motor = &motors[m];
    int64_t speed = (int64_t)(roundf(motor->currentVelocity * 21474.83648f));
    sharedData->nextSpeed[m] = speed;

    if (motor->config & (DMC_MOTOR_CONFIG_COUPLE | DMC_MOTOR_CONFIG_COUPLE_R))
    {
      int m2 = m + 1;
      if (m2 < MOTOR_COUNT)
      {
        int32_t dir = getMotorDir(m);
        dir = (dir) ? 1 : -1;
        if (motor->config & DMC_MOTOR_CONFIG_COUPLE_R)
        {
          speed = -speed;
          dir = -dir;
        }
        setMotorDir(m2, dir);
        sharedData->nextSpeed[m2] = speed;
        ++m;
      }
    }
  }

  sharedData->motorDirection = motorDirection;
  sharedData->motorDataLoaded = 1;

  if (shouldHardStop)
  {
    stopAll((shouldHardStop == 2) ? 1 : 0);
  }

  return 1;
}

void setMotorDir(int32_t m, float d)
{
  if (d > 0.0001f)
  {
    motorDirection |= (1U << m);
  }
  else if (d < -0.0001f)
  {
    motorDirection &= ~(1U << m);
  }
}

int32_t getMotorDir(int32_t m)
{
  return ((motorDirection & (1U << m)) != 0);
}

int32_t updateMotorVelocity(Motor *motor, float timeSegment)
{
  int32_t dir = 0;

  if (motor->moving)
  {
    if (motor->stopping == 2)
    {
      motor->currentVelocity *= 0.975f;

      float dx = motor->currentVelocity * DATA_RATE_TIME_SEGMENT;

      if (dx > 0.0001f)
      {
        dir = 1;
      }
      else if (dx < -0.0001f)
      {
        dir = -1;
      }
      else
      {
        dx = 0;
        motor->stopping = 0;
        motor->moving = 0;
        motor->currentVelocity = 0;
        motorsSendPosition = 1;
      }
      motor->position += dx;
    }
    else
    {
      MotorMove *mv = &motor->moves[motor->currentMove];

      if (mv->time == 0.0f) // this is same because I would have actually set it to zero
      {
        motor->moving = 0;
        motor->currentVelocity = 0;

        motorsSendPosition = 1;
      }
      else
      {
        motor->currentMoveTime += timeSegment;
        if (motor->currentMoveTime >= mv->time)
        {
          motor->currentMoveTime -= mv->time;
          ++motor->currentMove;
          mv = &motor->moves[motor->currentMove];
        }
        float t = motor->currentMoveTime;
        float xn = (mv->position + mv->velocity * t + mv->acceleration * t * t); // accel was already multiplied * 0.5

        float dx = xn - motor->position;

        motor->currentVelocity = dx * DATA_RATE;

        if (dx > 0.0001f)
        {
          dir = 1;
        }
        else if (dx < -0.0001f)
        {
          dir = -1;
        }

        motor->position = xn;
      }
    }
  }
  return dir;
}

void stopAll(uint8_t emergency)
{
  int32_t m;

  unsigned long msTime = millis();
  if (stopAllLastTime < msTime && (msTime - stopAllLastTime) < 1000)
  {
    emergency = 1;
  }

  motors[MOTOR_COUNT].currentVelocity = 0; // stop camera
  if (goMotionMove.flags & DMC_RT_CAMERA_VIDEO)
  {
    setCamera(CAMERA_METER | CAMERA_SHUTTER);
    cameraTriggerCountdown = 5;
  }

  stopAllLastTime = msTime;

  if (moveState == MOVE_STATE_ALL_JOG)
  {
    frameTimeMotor.maxAcceleration = FRAME_TO_POSITION(4);
    float maxAcceleration = frameTimeMotor.maxAcceleration;
    if (frameTimeStopCounter == 0)
    {
      frameTimeMotor.maxAcceleration =
        fmaxf(FRAME_TO_POSITION(2), fmaxf(frameTimeMotor.maxVelocity, fabsf(frameTimeMotor.currentVelocity)) * 0.5f);
    }
    else if (frameTimeStopCounter == 1)
    {
      frameTimeMotor.maxAcceleration =
        fmaxf(FRAME_TO_POSITION(1), fmaxf(frameTimeMotor.maxVelocity, fabsf(frameTimeMotor.currentVelocity)) * 2.0f);
    }
    else
    {
      return;
    }
    ++frameTimeStopCounter;

    stopMotor(&frameTimeMotor, -2, 0);
    frameTimeMotor.maxAcceleration = maxAcceleration;
  }
  else
  {
    moveState = MOVE_STATE_JOG;

    for (m = 0; m < MOTOR_COUNT; ++m)
    {
      float maxAcceleration = motors[m].maxAcceleration;

      motors[m].maxAcceleration =
        fmaxf(motors[m].maxAcceleration, fabsf(motors[m].currentVelocity)); // let's hard stop here, folks

      if (goMotionMove.state && goMotionMove.mode != GO_MO_MODE_RUN_LIVE)
      {
        motors[m].maxAcceleration *= 0.5f;
      }

      stopMotor(&motors[m], m, emergency);

      motors[m].maxAcceleration = maxAcceleration;
    }
  }
  goMotionMove.state = GO_MO_INACTIVE;
}

int32_t stopMotor(Motor *motor, int32_t motorIndex, int8_t emergency)
{
  if (!motor->moving || motor->stopping)
    return 0;

  memset((char *)motor->moves, 0, P2P_MOVE_COUNT * sizeof(MotorMove));

  float v = motor->currentVelocity;

  float maxA = motor->maxAcceleration;
  if (motorIndex >= MOTOR_COUNT || motorIndex == -1)
  {
    maxA *= 1.5f;
  }

  float t = fabsf(v / maxA);

  // for frame time, try to land on frame
  if (motor == &frameTimeMotor)
  {
    float accel = (v > 0) ? -maxA : maxA;
    int32_t pos = (int32_t)(motor->position + v * t + 0.5f * accel * t * t);

    int32_t startPos = FRAME_TO_POSITION(moveStartFrame);
    int32_t endPos = FRAME_TO_POSITION(moveStartFrame + moveFrameCount);
    int32_t changed = 0;

    if (pos < startPos)
    {
      pos = startPos;
      changed = 1;
    }
    else if (pos > endPos)
    {
      pos = endPos;
      changed = 1;
    }
    else if (pos % 100000L)
    {
      pos = 100000L * ((pos / 100000L) + ((v > 0) ? 1 : 0));
      changed = 2;
    }

    if (changed)
    {
      t = 2.0f * fabsf((pos - motor->position) / v);
      maxA = fabsf(v / t);

      if (t > 2.0f && changed == 2 && fabsf(motor->position - pos) < 100000)
      {
        float mv = frameTimeMotor.maxVelocity;
        frameTimeMotor.maxVelocity = FRAME_TO_POSITION(1);
        calculatePointToPoint(&frameTimeMotor, pos, 0.0f);
        frameTimeMotor.maxVelocity = mv;
        return 0;
      }
    }
  }

  // consider limits!
  if (v > 0 && motor->limitHighEnabled)
  {
    if (motor->limitHigh > motor->position)
    {
      float a = (v * v) / (2.0f * (motor->limitHigh - motor->position));
      if (a > maxA)
      {
        maxA = a;
        t = fabsf(v / maxA);
      }
    }
  }
  else if (v < 0 && motor->limitLowEnabled)
  {
    if (motor->limitLow < motor->position)
    {
      float a = (v * v) / (2.0f * (motor->position - motor->limitLow));
      if (a > maxA)
      {
        maxA = a;
        t = fabsf(v / maxA);
      }
    }
  }

  motor->stopping = 1;
  motor->moves[0].time = t;
  motor->moves[0].position = motor->position;
  motor->moves[0].velocity = v;
  motor->moves[0].acceleration = (v > 0) ? -maxA : maxA;

  motor->moves[1].time = 0;
  motor->moves[1].position =
    (motor->moves[0].position + motor->moves[0].velocity * t + 0.5f * motor->moves[0].acceleration * t * t);
  int32_t intPos = (int32_t)motor->moves[1].position;
  if (fabsf(motor->moves[1].position - intPos) > 0.01f)
  {
    if (v > 0)
      motor->moves[1].position = intPos + 1;
    else
      motor->moves[1].position = intPos - 1;
  }
  motor->moves[1].velocity = 0;
  motor->moves[1].acceleration = 0;

  motor->moves[0].acceleration *= 0.5f;

  motor->currentMoveTime = 0;
  motor->currentMove = 0;

  movePositionFrame = -1;

  return 1;
}
