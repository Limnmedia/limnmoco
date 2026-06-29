// SPDX-License-Identifier: BSD-3-Clause

#include <cstring>

#include "config.hpp"
#include "dmc.hpp"

#include "debug.hpp"

DmcStream::DmcStream()
    : dmc_device(
    DEVICE_NAME,
	FW_MAJOR,
	FW_MINOR,
	FW_REV,
	MOTOR_COUNT,
	DMX_COUNT,
	GIO_OUT,
	GIO_IN,
	HW_LIMIT,
	FRAME_COUNT,
	CAPABILITIES,
	PROTOCOL_VERSION
    )
    , stream(nullptr)
    , state(STATE_WAIT_D)
    , index(0)
    , length(0)
    , tx_queue_head(0)
    , tx_queue_tail(0)
{
    memset(rx_buffer, 0, sizeof(rx_buffer));
    memset(tx_queue, 0, sizeof(tx_queue));
}

void DmcStream::bind(Stream &stream) {
    this->stream = &stream;

    // According to the DMC Spec, the DMC device must 
    // send a Device Capabilities Packet on bootup.
    // This seems as good a place as any to enqueue 
    // a DmcDevice for sending.
    enqueue(&dmc_device, sizeof(DmcDevice));
}

void DmcStream::reset() {
    state  = STATE_WAIT_D;
    index  = 0;
    length = 0;
}

void DmcStream::receive() {
    debug_pulse(PIN_DBG_0);
    // #TODO: Limit ourselves to 32 bytes incoming at a time
    while (stream->available() > 0) {
        uint8_t b = stream->read();

        switch (state) {
        case STATE_WAIT_D: {
            debug_pulse(PIN_DBG_1);
            if (b == 'D') {
                rx_buffer[index++] = b;
                state = STATE_WAIT_F;
            }
            break;
        }

        case STATE_WAIT_F: {
            debug_pulse(PIN_DBG_2);
            if (b == 'F') {
                rx_buffer[index++] = b;
                state = STATE_READ_HEADER;
                break;
            }

            if (b == 'D') {
                // still waiting for byte 'F'
                // we can ignore the extra 'D'
                break;
            }
            // #NOTE: The only state transition into 'F'
            //  is from 'D', and if the byte immediately following
            //  'D' is not 'F' the packet is malformed.
            reset();
            break;
        }

        case STATE_READ_HEADER: {
            debug_pulse(PIN_DBG_3);
            rx_buffer[index++] = b;

            if (index == sizeof(DmcHeader)) {
                //length = (rx_buffer[8] << 8) | rx_buffer[9];
                memcpy(&length, rx_buffer + 8, sizeof(length));

                size_t total = sizeof(DmcHeader) + length + 2;
                if (total > DMC_MSG_MAX_LENGTH) {
                    reset(); // packet too large
                    break;
                }

                state  = STATE_READ_PAYLOAD;
                length = total;
            }

            break;
        }

        case STATE_READ_PAYLOAD: {
            debug_pulse(PIN_DBG_4);
            rx_buffer[index++] = b;

            if (index < length) {
                break;
            }

            state = STATE_CHECKSUM;
            [[fallthrough]];
        }

        case STATE_CHECKSUM: {
            uint16_t cs = checksum(rx_buffer, length);
            if (cs == 0) {
                debug_pulse(PIN_DBG_5);
                packet_switch(rx_buffer, length);
            } else {
                debug_pulse(PIN_DBG_6);
                uint32_t id;
                uint16_t type;
                memcpy(&id, rx_buffer + 2, sizeof(uint32_t));
                memcpy(&type, rx_buffer + 6, sizeof(uint16_t));
                ack(id, type, DMC_ACK_ERR_CHECKSUM);
            }

            reset();
            break;
        }
        }
    }
}

void DmcStream::transmit() {
    debug_pulse(PIN_DBG_0);
    if (tx_queue_head == tx_queue_tail) {
        return;
    }

    debug_pulse(PIN_DBG_1);
    QueuedPacket *packet = &tx_queue[tx_queue_tail];

    // how much left of the packet?
    int waiting = packet->length - packet->index;

    // only write out what we need to, or what we can, whichever is less
    int available = (32 <= waiting) ? 32 : waiting;
    
    while (available--) {
        debug_pulse(PIN_DBG_2);
        stream->write(packet->buffer[packet->index++]);
    }

    debug_pulse(PIN_DBG_3);
    // is the packet fully transmitted?
    if (packet->index >= packet->length) {
        debug_pulse(PIN_DBG_4);
        tx_queue_tail = (tx_queue_tail + 1) % tx_queue_length;
    }
}

void DmcStream::enqueue(void *packet, uint16_t length) {
    //debug_pulse(PIN_DBG_3);
    uint8_t next = (tx_queue_head + 1) % tx_queue_length;
    if (next == tx_queue_tail) {
        return; // TX queue full, drop the packet
    }

    uint16_t cb = checkbytes(checksum(packet, length));

    QueuedPacket *slot = &tx_queue[tx_queue_head];
    memcpy(slot->buffer, packet, length);
    memcpy(slot->buffer + length, (uint8_t *)(&cb), sizeof(cb));
    slot->length = length + sizeof(cb);
    slot->index  = 0;

    tx_queue_head = next;
}

void DmcStream::ack(uint32_t id, uint16_t type, uint32_t response) {
    DmcAck packet(id, type, response);
    enqueue(&packet, sizeof(packet));
}

void DmcStream::ack(DmcHeader &header, uint32_t response) {
    DmcAck packet(header.id, header.type, response);
    enqueue(&packet, sizeof(packet));
}

uint16_t DmcStream::checksum(void *buffer, size_t length) {
    uint8_t *data = (uint8_t *)buffer;
    uint16_t sum1 = 0, sum2 = 0;

    while (length) {
        size_t tlen = ((length >= 20) ? 20 : length);
        length -= tlen;
        do {
            sum2 += sum1 += *data++;
            tlen--;
        } while (tlen);
        sum1 %= 0xff;
        sum2 %= 0xff;
    }
    return (sum2 << 8) | sum1;
}

uint16_t DmcStream::checkbytes(uint16_t checksum) {
    uint8_t c0, c1, f0, f1;
    f0 = checksum & 0xFF;
    f1 = (checksum >> 8) & 0xFF;
    c0 = 0xFF - ((f0 + f1) % 0xFF);
    c1 = 0xFF - ((f0 + c0) % 0xFF);
    return (c1 << 8) | c0;
}

void DmcStream::packet_switch(void *buffer, size_t length) {
    //debug_pulse(PIN_DBG_0);
    DmcHeader *header = reinterpret_cast<DmcHeader *>(buffer);

    //switch (header->type & (~DMC_MSG_FLAG_ACK)) {
    switch(header->type) {
        case DMC_MSG_HI:
            on_hi(reinterpret_cast<DmcHi *>(buffer));
            break;

        case DMC_MSG_DMX:
            on_dmx(reinterpret_cast<DmcDmx *>(buffer));
            break;

        case DMC_MSG_GIO_OUT:
            on_gio_out(reinterpret_cast<DmcGioOut *>(buffer));
            break;

        case DMC_MSG_GIO_IN:
            on_gio_in(reinterpret_cast<DmcAck *>(buffer));
            break;

        case DMC_MSG_GIO_CAM:
            on_gio_cam(reinterpret_cast<DmcGioCam *>(buffer));
            break;

        case DMC_MSG_MOTOR_STATUS:
            on_motor_status(reinterpret_cast<DmcAck *>(buffer));
            break;

        case DMC_MSG_MOTOR_MOVE:
            on_motor_move(reinterpret_cast<DmcMotorMove *>(buffer));
            break;

        case DMC_MSG_MOTOR_STOP:
            on_motor_stop(reinterpret_cast<DmcMotorStop *>(buffer));
            break;

        case DMC_MSG_MOTOR_STOP_ALL:
            on_motor_stop_all(reinterpret_cast<DmcMotorStopAll *>(buffer));
            break;

        case DMC_MSG_MOTOR_GET_POSITION:
            on_motor_get_position(reinterpret_cast<DmcAck *>(buffer));
            break;

        case DMC_MSG_MOTOR_RESET_POSITION:
            on_motor_reset_position(reinterpret_cast<DmcMotorResetPosition *>(buffer));
            break;

        case DMC_MSG_MOTOR_JOG:
            on_motor_jog(reinterpret_cast<DmcMotorJog *>(buffer));
            break;

        case DMC_MSG_MOTOR_CONFIGURE:
            on_motor_configure(reinterpret_cast<DmcMotorConfigure *>(buffer));
            break;

        case DMC_MSG_MOTOR_SET_SPEED:
            on_motor_set_speed(reinterpret_cast<DmcMotorSetSpeed *>(buffer));
            break;

        case DMC_MSG_MOTOR_SET_LIMITS:
            on_motor_set_limits(reinterpret_cast<DmcMotorSetLimits *>(buffer));
            break;

        case DMC_MSG_MOTOR_HARD_STOP:
            on_motor_hard_stop(reinterpret_cast<DmcAck *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_BEGIN:
            on_rt_upload_move_begin(reinterpret_cast<DmcRtUploadMoveBegin *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_AXIS:
            on_rt_upload_move_axis(reinterpret_cast<DmcRtUploadMoveAxis *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_DMX:
            on_rt_upload_move_dmx(reinterpret_cast<DmcRtUploadMoveDmx *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS:
            on_rt_upload_move_triggers(reinterpret_cast<DmcRtUploadMoveTriggers *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_END:
            on_rt_upload_move_end(reinterpret_cast<DmcRtUploadMoveEnd *>(buffer));
            break;

        case DMC_MSG_RT_POSITION_FRAME:
            on_rt_position_frame(reinterpret_cast<DmcRtPositionFrame *>(buffer));
            break;

        case DMC_MSG_RT_RUN_MOVE:
            on_rt_run_move(reinterpret_cast<DmcRtRunMove *>(buffer));
            break;

        case DMC_MSG_RT_SHOOT_FRAME:
            on_rt_shoot_frame(reinterpret_cast<DmcRtShootFrame *>(buffer));
            break;

        case DMC_MSG_RT_SHOOT_FRAME2:
            on_rt_shoot_frame_2(reinterpret_cast<DmcRtShootFrame2 *>(buffer));
            break;

        case DMC_MSG_RT_GO:
            on_rt_go(reinterpret_cast<DmcRtGo *>(buffer));
            break;

        case DMC_MSG_RT_END:
            on_rt_end(reinterpret_cast<DmcRtEnd *>(buffer));
            break;

        case DMC_MSG_RT_JOG_ALL:
            on_rt_jog_all(reinterpret_cast<DmcRtJogAll *>(buffer));
            break;

        case DMC_MSG_RT_STOP_LOOP:
            on_rt_stop_loop(reinterpret_cast<DmcRtStopLoop *>(buffer));
            break;

        case DMC_MSG_VIRT_CONFIG:
            on_virt_config(reinterpret_cast<DmcVirtConfig *>(buffer));
            break;

        case DMC_MSG_VIRT_MOVE:
            on_virt_move(reinterpret_cast<DmcVirtMove *>(buffer));
            break;

        case DMC_MSG_VIRT_STOP:
            on_virt_stop(reinterpret_cast<DmcVirtStop *>(buffer));
            break;

        case DMC_MSG_VIRT_JOG:
            on_virt_jog(reinterpret_cast<DmcVirtJog *>(buffer));
            break;

        case DMC_MSG_VIRT_GET_POSITION:
            on_virt_get_position(reinterpret_cast<DmcAck *>(buffer));
            break;

        case DMC_MSG_VIRT_JOG_ON_LINE:
            on_virt_jog_on_line(reinterpret_cast<DmcVirtJogOnLine *>(buffer));
            break;

        case DMC_MSG_VIRT_AIM_POINT:
            on_virt_aim_point(reinterpret_cast<DmcVirtAimPoint *>(buffer));
            break;

        default:
            on_unknown(header);
            break;
    }
}

void DmcStream::on_hi(DmcHi *hi) {
    //debug_pulse(PIN_DBG_2);
    dmc_device.header.id = hi->header.id;
    enqueue(&dmc_device, sizeof(DmcDevice));
}


