// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_DMC_PRINT_HPP
#define LIMNMOCO_CM7_DMC_PRINT_HPP

#include "dmc.hpp"

class DmcPrint {
public:
    DmcPrint();

    void bind(Stream &stream);
    void transmit();

    void operator()(DmcHeader *packet);
    void operator()(DmcAck *packet);
    void operator()(DmcHi *packet);
    void operator()(DmcDevice *packet);
    void operator()(DmcDmx *packet);
    void operator()(DmcGioOut *packet);
    void operator()(DmcGioIn *packet);
    void operator()(DmcGioCam *packet);
    void operator()(DmcMotorStatus *packet);
    void operator()(DmcMotorMove *packet);
    void operator()(DmcMotorMoveResponse *packet);
    void operator()(DmcMotorStop *packet);
    void operator()(DmcMotorStopAll *packet);
    void operator()(DmcMotorGetPosition *packet);
    void operator()(DmcMotorResetPosition *packet);
    void operator()(DmcMotorJog *packet);
    void operator()(DmcMotorConfigure *packet);
    void operator()(DmcMotorSetSpeed *packet);
    void operator()(DmcMotorSetLimits *packet);
    void operator()(DmcMotorHardStop *packet);
    void operator()(DmcRtUploadMoveBegin *packet);
    void operator()(DmcRtUploadMoveAxis *packet);
    void operator()(DmcRtUploadMoveDmx *packet);
    void operator()(DmcRtUploadMoveTriggers *packet);
    void operator()(DmcRtUploadMoveEnd *packet);
    void operator()(DmcRtPositionFrame *packet);
    void operator()(DmcRtRunMove *packet);
    void operator()(DmcRtShootFrame *packet);
    void operator()(DmcRtShootFrame2 *packet);
    void operator()(DmcRtGo *packet);
    void operator()(DmcRtEnd *packet);
    void operator()(DmcRtJogAll *packet);
    void operator()(DmcRtStopLoop *packet);
    void operator()(DmcVirtConfig *packet);
    void operator()(DmcVirtConfigBoomSwingTrack *packet);
    void operator()(DmcVirtConfigSwingPan *packet);
    void operator()(DmcVirtMove *packet);
    void operator()(DmcVirtStop *packet);
    void operator()(DmcVirtJog *packet);
    void operator()(DmcVirtJogOnLine *packet);
    void operator()(DmcVirtGetPosition *packet);
    void operator()(DmcVirtAimPoint *packet);

private:
    void enqueue(uint8_t data);
    void enqueue(char data);
    void enqueue(uint8_t *data, uint32_t length);

    void enqueue(uint32_t data);
    void enqueue(int32_t data);
    void enqueue(float data);
    void enqueue(char const *data);

    void enqueue(char const *field, uint8_t data);
    void enqueue(char const *field, uint32_t data);
    void enqueue(char const *field, int32_t data);
    void enqueue(char const *field, float data);
    void enqueue(char const *field, uint8_t major, uint8_t minor, uint8_t rev);
    void enqueue(char const *field, uint32_t index, char value);
    void enqueue(char const *field, uint32_t index, uint32_t value);
    void enqueue(char const *field, uint32_t index, int32_t value);
    void enqueue(char const *field, uint32_t index, float value);
    void enqueue(char const *field, uint8_t *value, uint32_t length);
    void enqueue(char const *field, char const *value);

    void enqueue(DmcHeader *header);

    Stream *stream;

    static const uint32_t tx_length = 2048;
    uint8_t               tx_buffer[tx_length];
    uint32_t              tx_head;
    uint32_t              tx_tail;
};

#endif // !LIMNMOCO_CM7_DMC_PRINT_HPP

