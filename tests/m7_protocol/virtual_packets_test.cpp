// SPDX-License-Identifier: BSD-3-Clause
#include <VirtualPackets.h>
#include <gtest/gtest.h>
TEST(VirtualPackets, DecodesSignedWireFields) {
  std::string e; limnmoco::VirtualMovePacket move{};
  ASSERT_TRUE(limnmoco::decode_virtual_move({7,limnmoco::kDmcVirtMove,{2,0xa0,0x86,0x01,0x80}},&move,&e)) << e;
  EXPECT_EQ(move.axis,2); EXPECT_EQ(move.position,INT32_MIN + 100000);
  limnmoco::VirtualJogPacket jog{};
  ASSERT_TRUE(limnmoco::decode_virtual_jog({8,limnmoco::kDmcVirtJog,{3,0x10,0x27,0xff,0xff,0xff,0xff}},&jog,&e));
  EXPECT_EQ(jog.speed,10000); EXPECT_EQ(jog.destination,-1);
  limnmoco::VirtualAimPointPacket aim{};
  ASSERT_TRUE(limnmoco::decode_virtual_aim_point({9,limnmoco::kDmcVirtAimPoint,{1,0x2e,0xfb,0xff,0xff,0x2e,0x16,0,0,0xd8,0xdc,0xff,0xff}},&aim,&e));
  EXPECT_EQ(aim.x,-1234); EXPECT_EQ(aim.y,5678); EXPECT_EQ(aim.z,-9000);
}
TEST(VirtualPackets, RejectsWrongCommandShapes) {
  std::string e; limnmoco::VirtualStopPacket stop{};
  EXPECT_FALSE(limnmoco::decode_virtual_stop({0,limnmoco::kDmcVirtStop,{1,2}},&stop,&e));
  EXPECT_NE(e.find("length"),std::string::npos);
  EXPECT_FALSE(limnmoco::decode_virtual_get_position({0,limnmoco::kDmcVirtGetPosition,{0}},&e));
}
