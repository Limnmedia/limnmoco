// SPDX-License-Identifier: BSD-3-Clause
#include "VirtualPackets.h"
namespace limnmoco { namespace {
uint16_t u16(const std::vector<uint8_t>& p, int o) { return p[o] | (p[o+1] << 8); }
int32_t i32(const std::vector<uint8_t>& p, int o) { return static_cast<int32_t>(static_cast<uint32_t>(p[o]) | (static_cast<uint32_t>(p[o+1])<<8) | (static_cast<uint32_t>(p[o+2])<<16) | (static_cast<uint32_t>(p[o+3])<<24)); }
bool shape(const DmcFrame& f, uint16_t type, size_t size, std::string* e) { if (!e) return false; e->clear(); if (f.type != type || f.payload.size() != size) { *e="unexpected virtual packet type or length"; return false; } return true; }
} 
bool decode_virtual_move(const DmcFrame& f, VirtualMovePacket* p, std::string* e) { if (!p || !shape(f,kDmcVirtMove,5,e)) return false; p->axis=f.payload[0];p->position=i32(f.payload,1);return true; }
bool decode_virtual_stop(const DmcFrame& f, VirtualStopPacket* p, std::string* e) { if (!p || !shape(f,kDmcVirtStop,1,e)) return false;p->axis=f.payload[0];return true; }
bool decode_virtual_jog(const DmcFrame& f, VirtualJogPacket* p, std::string* e) { if (!p || !shape(f,kDmcVirtJog,7,e)) return false;p->axis=f.payload[0];p->speed=u16(f.payload,1);p->destination=i32(f.payload,3);return true; }
bool decode_virtual_line_jog(const DmcFrame& f, VirtualLineJogPacket* p, std::string* e) { if (!p || !shape(f,kDmcVirtJogOnLine,3,e)) return false;p->axis=f.payload[0];p->speed=static_cast<int16_t>(u16(f.payload,1));return true; }
bool decode_virtual_aim_point(const DmcFrame& f, VirtualAimPointPacket* p, std::string* e) { if (!p || !shape(f,kDmcVirtAimPoint,13,e)) return false;p->enabled=f.payload[0];p->x=i32(f.payload,1);p->y=i32(f.payload,5);p->z=i32(f.payload,9);return true; }
bool decode_virtual_get_position(const DmcFrame& f, std::string* e) { return shape(f,kDmcVirtGetPosition,0,e); }
} // namespace limnmoco
