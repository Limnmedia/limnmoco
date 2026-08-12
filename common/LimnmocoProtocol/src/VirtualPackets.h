// SPDX-License-Identifier: BSD-3-Clause
#ifndef LIMNMOCO_VIRTUAL_PACKETS_H_
#define LIMNMOCO_VIRTUAL_PACKETS_H_
#include "DmcFrame.h"
#include <cstdint>
#include <string>
namespace limnmoco {
constexpr uint16_t kDmcVirtMove = 0x0201, kDmcVirtStop = 0x0202, kDmcVirtJog = 0x0203;
constexpr uint16_t kDmcVirtGetPosition = 0x0205, kDmcVirtJogOnLine = 0x0206, kDmcVirtAimPoint = 0x0207;
constexpr float kVirtualPositionScale = 100000.0f;
struct VirtualMovePacket { uint8_t axis; int32_t position; };
struct VirtualStopPacket { uint8_t axis; };
struct VirtualJogPacket { uint8_t axis; uint16_t speed; int32_t destination; };
struct VirtualLineJogPacket { uint8_t axis; int16_t speed; };
struct VirtualAimPointPacket { uint8_t enabled; int32_t x; int32_t y; int32_t z; };
bool decode_virtual_move(const DmcFrame &, VirtualMovePacket *, std::string *);
bool decode_virtual_stop(const DmcFrame &, VirtualStopPacket *, std::string *);
bool decode_virtual_jog(const DmcFrame &, VirtualJogPacket *, std::string *);
bool decode_virtual_line_jog(const DmcFrame &, VirtualLineJogPacket *, std::string *);
bool decode_virtual_aim_point(const DmcFrame &, VirtualAimPointPacket *, std::string *);
bool decode_virtual_get_position(const DmcFrame &, std::string *);
} // namespace limnmoco
#endif
