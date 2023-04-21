/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "text_converter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
namespace {
class TextConverterTest : public testing::Test {
};

/**
 * @tc.name: ToUTF8
 * @tc.desc: Verify the ToUTF8
 * @tc.type:FUNC
 */
HWTEST_F(TextConverterTest, ToUTF8, TestSize.Level1)
{
    UTF16String u16 = {0x00, 0x20, 0x41, 0x7f, 0x00, 0x80, 0x100, 0x601, 0x7ff, 0x800, 0xffff};
    auto u8 = TextConverter::ToUTF8(u16);
    EXPECT_EQ(u8[0], 0x00);
    EXPECT_EQ(u8[1], 0x20);
    EXPECT_EQ(u8[2], 0x41);
    EXPECT_EQ(u8[3], 0x7f);
    EXPECT_EQ(u8[4], 0x00);

    EXPECT_EQ(u8[5], 0b11000010);
    EXPECT_EQ(u8[6], 0b10000000);

    EXPECT_EQ(u8[7], 0b11000100);
    EXPECT_EQ(u8[8], 0b10000000);

    EXPECT_EQ(u8[9], 0b11011000);
    EXPECT_EQ(u8[10], 0b10000001);

    EXPECT_EQ(u8[11], 0b11011111);
    EXPECT_EQ(u8[12], 0b10111111);

    EXPECT_EQ(u8[13], 0b11100000);
    EXPECT_EQ(u8[14], 0b10100000);
    EXPECT_EQ(u8[15], 0b10000000);

    EXPECT_EQ(u8[16], 0b11101111);
    EXPECT_EQ(u8[17], 0b10111111);
    EXPECT_EQ(u8[18], 0b10111111);
}

/**
 * @tc.name: ToStr
 * @tc.desc: Verify the ToStr
 * @tc.type:FUNC
 */
HWTEST_F(TextConverterTest, ToStr, TestSize.Level1)
{
    UTF16String u16 = {0x20, 0x41, 0x7f, 0x80, 0x100, 0x601, 0x7ff, 0x800, 0xffff};
    auto str = TextConverter::ToStr(u16);
    EXPECT_EQ(str[0], static_cast<char>(0x20));
    EXPECT_EQ(str[1], static_cast<char>(0x41));
    EXPECT_EQ(str[2], static_cast<char>(0x7f));

    EXPECT_EQ(str[3], static_cast<char>(0b11000010));
    EXPECT_EQ(str[4], static_cast<char>(0b10000000));

    EXPECT_EQ(str[5], static_cast<char>(0b11000100));
    EXPECT_EQ(str[6], static_cast<char>(0b10000000));

    EXPECT_EQ(str[7], static_cast<char>(0b11011000));
    EXPECT_EQ(str[8], static_cast<char>(0b10000001));

    EXPECT_EQ(str[9], static_cast<char>(0b11011111));
    EXPECT_EQ(str[10], static_cast<char>(0b10111111));

    EXPECT_EQ(str[11], static_cast<char>(0b11100000));
    EXPECT_EQ(str[12], static_cast<char>(0b10100000));
    EXPECT_EQ(str[13], static_cast<char>(0b10000000));

    EXPECT_EQ(str[14], static_cast<char>(0b11101111));
    EXPECT_EQ(str[15], static_cast<char>(0b10111111));
    EXPECT_EQ(str[16], static_cast<char>(0b10111111));
}

/**
 * @tc.name: ToUTF16
 * @tc.desc: Verify the ToUTF16
 * @tc.type:FUNC
 */
HWTEST_F(TextConverterTest, ToUTF16, TestSize.Level1)
{
    auto u16 = TextConverter::ToUTF16("\U0001ffff\uffff a1");
    EXPECT_EQ(u16[0], 0xd83f);
    EXPECT_EQ(u16[1], 0xdfff);
    EXPECT_EQ(u16[2], 0xffff);
    EXPECT_EQ(u16[3], static_cast<uint16_t>(' '));
    EXPECT_EQ(u16[4], static_cast<uint16_t>('a'));
    EXPECT_EQ(u16[5], static_cast<uint16_t>('1'));

    u16 = TextConverter::ToUTF16(UTF8String{
            0b11110000, 0b10011111, 0b10111111, 0b10111111, // 1ffff
            0b11101111, 0b10111111, 0b10111111, // ffff
            0x20, 0x61, 0x31
    });
    EXPECT_EQ(u16[0], 0xd83f);
    EXPECT_EQ(u16[1], 0xdfff);
    EXPECT_EQ(u16[2], 0xffff);
    EXPECT_EQ(u16[3], static_cast<uint16_t>(' '));
    EXPECT_EQ(u16[4], static_cast<uint16_t>('a'));
    EXPECT_EQ(u16[5], static_cast<uint16_t>('1'));

    u16 = TextConverter::ToUTF16(UTF32String{0x1ffff, 0xffff, 0x20, 0x61, 0x31});
    EXPECT_EQ(u16[0], 0xd83f);
    EXPECT_EQ(u16[1], 0xdfff);
    EXPECT_EQ(u16[2], 0xffff);
    EXPECT_EQ(u16[3], static_cast<uint16_t>(' '));
    EXPECT_EQ(u16[4], static_cast<uint16_t>('a'));
    EXPECT_EQ(u16[5], static_cast<uint16_t>('1'));
}

/**
 * @tc.name: ToUTF32
 * @tc.desc: Verify the ToUTF32
 * @tc.type:FUNC
 */
HWTEST_F(TextConverterTest, ToUTF32, TestSize.Level1)
{
    auto u32 = TextConverter::ToUTF32(UTF16String{0xd83f, 0xdfff, 0xffff, 0x20, 0x61, 0x31});
    EXPECT_EQ(u32[0], 0x1ffff);
    EXPECT_EQ(u32[1], 0xffff);
    EXPECT_EQ(u32[2], 0x20);
    EXPECT_EQ(u32[3], 0x61);
    EXPECT_EQ(u32[4], 0x31);
}
} // namespace
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
