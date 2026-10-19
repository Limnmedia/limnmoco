// SPDX-License-Identifier: BSD-3-Clause
#include <DmcFrame.h>
#include <gtest/gtest.h>

TEST(DmcFrame, EncodesAndDecodesExactWireFrames) {
  const limnmoco::DmcFrame source{0x12345678u, 0x0201u, {2, 0xa0, 0x86, 1, 0}};
  std::vector<uint8_t> bytes;
  std::string error;
  ASSERT_TRUE(limnmoco::dmc_encode_frame(source, &bytes, &error)) << error;
  EXPECT_EQ(bytes, (std::vector<uint8_t>{'D','F',0x78,0x56,0x34,0x12,1,2,5,0,2,0xa0,0x86,1,0,0xda,0x53}));
  limnmoco::DmcFrame decoded{};
  ASSERT_TRUE(limnmoco::dmc_decode_frame(bytes, &decoded, &error)) << error;
  EXPECT_EQ(decoded.id, source.id); EXPECT_EQ(decoded.type, source.type);
  EXPECT_EQ(decoded.payload, source.payload);
}
TEST(DmcFrame, RejectsTruncatedAndCorruptFrames) {
  limnmoco::DmcFrame frame{}; std::string error;
  EXPECT_FALSE(limnmoco::dmc_decode_frame({'D','F'}, &frame, &error));
  EXPECT_NE(error.find("header"), std::string::npos);
  std::vector<uint8_t> corrupt{'D','F',0,0,0,0,1,2,0,0,0,0};
  EXPECT_FALSE(limnmoco::dmc_decode_frame(corrupt, &frame, &error));
  EXPECT_NE(error.find("checksum"), std::string::npos);
}
