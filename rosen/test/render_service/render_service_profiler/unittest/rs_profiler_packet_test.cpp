/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <unordered_set>

#include "gtest/gtest.h"
#include "rs_profiler_packet.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProfilerPacketTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: BinaryHelperTest
 * @tc.desc: Test public methods of BimaryHelper
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerPacketTest, BinaryHelperTest, testing::ext::TestSize.Level1)
{
    const std::vector<char> raw = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    EXPECT_EQ(BinaryHelper::Type(raw.data()), PackageID::RS_PROFILER_HEADER);
    EXPECT_EQ(BinaryHelper::BinaryCount(raw.data()), 0x04030201);
    EXPECT_EQ(BinaryHelper::Pid(raw.data()), 0x0605);

    auto payload = BinaryHelper::Data(raw.data(), raw.size());
    EXPECT_EQ(payload.front(), 1);
    EXPECT_EQ(payload.back(), 15);
}

/*
 * @tc.name: PacketTypeTest
 * @tc.desc: Test Packet types
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerPacketTest, PacketTypeTest, testing::ext::TestSize.Level1)
{
    Packet p { Packet::BINARY };
    EXPECT_TRUE(p.IsBinary());
    EXPECT_FALSE(p.IsCommand());
    p.SetType(Packet::COMMAND);
    EXPECT_TRUE(p.IsCommand());
    EXPECT_FALSE(p.IsBinary());
}

/*
 * @tc.name: PacketReadWriteTest
 * @tc.desc: Test Packet read/write different types
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerPacketTest, PacketReadWriteTest, testing::ext::TestSize.Level1)
{
    uint8_t u8 = 17;
    int8_t i8 = -6;
    uint16_t u16 = 234;
    int16_t i16 = 79;
    uint32_t u32 = 43689832;
    int32_t i32 = -7898;
    uint64_t u64 = 85092554523;
    int64_t i64 = -9580984;
    std::string str = "test packet";
    std::vector<int> vec = { 0, -1, 1, -2, 2, -3, 3 };

    Packet p { Packet::BINARY };
    p.Write(u8);
    p.Write(i8);
    p.Write(u16);
    p.Write(i16);
    p.Write(u32);
    p.Write(i32);
    p.Write(u64);
    p.Write(i64);
    p.Write(str);
    p.Write(vec);

    EXPECT_EQ(u8, p.Read<uint8_t>());
    EXPECT_EQ(i8, p.Read<int8_t>());
    EXPECT_EQ(u16, p.Read<uint16_t>());
    EXPECT_EQ(i16, p.Read<int16_t>());
    EXPECT_EQ(u32, p.Read<uint32_t>());
    EXPECT_EQ(i32, p.Read<int32_t>());
    EXPECT_EQ(u64, p.Read<uint64_t>());
    EXPECT_EQ(i64, p.Read<int64_t>());
    EXPECT_EQ(str, p.Read<std::string>(11));
    EXPECT_EQ(vec, p.Read<std::vector<int>>(7));
}

/*
 * @tc.name: PacketReadWriteRawTest
 * @tc.desc: Test Packet read/write different types
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerPacketTest, PacketReadWriteRawTest, testing::ext::TestSize.Level1)
{
    uint32_t u32 = 8979832;
    std::string str = "lorum ipsum";
    std::vector<int> vec = { 2, 1, 0, -1, -2 };

    Packet p { Packet::BINARY };
    p.Write((void*)&u32, sizeof(u32));
    p.Write(str.data(), str.size());
    p.Write(vec.data(), vec.size() * sizeof(int));

    uint32_t u32Res = 0;
    p.Read(&u32Res, sizeof(u32Res));
    EXPECT_EQ(u32, u32Res);

    std::string strRes;
    strRes.resize(11);
    p.Read(strRes.data(), 11);
    EXPECT_EQ(str, strRes);

    std::vector<int> vecRes;
    vecRes.resize(5);
    p.Read(vecRes.data(), 5 * sizeof(int));
    EXPECT_EQ(vec, vecRes);
}

/*
 * @tc.name: PacketLengthTest
 * @tc.desc: Test Packet length calculation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerPacketTest, PacketLengthTest, testing::ext::TestSize.Level1)
{
    Packet p { Packet::BINARY };
    EXPECT_EQ(p.GetLength(), Packet::HEADER_SIZE);
    EXPECT_EQ(p.GetPayloadLength(), 0);

    p.Write(uint32_t { 5 });
    p.Write(double { 24.453 });
    p.Write(std::string { "qweasdzxc" });
    EXPECT_EQ(p.GetLength(), Packet::HEADER_SIZE + 4 + 8 + 9);
    EXPECT_EQ(p.GetPayloadLength(), 4 + 8 + 9);
}

/*
 * @tc.name: PacketInternalPointersTest
 * @tc.desc: Test Packet pointers correctness on read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerPacketTest, PacketInternalPointersTest, testing::ext::TestSize.Level1)
{
    Packet p { Packet::BINARY };
    EXPECT_EQ(p.writePointer_, Packet::HEADER_SIZE);
    EXPECT_EQ(p.readPointer_, Packet::HEADER_SIZE);
    EXPECT_EQ(p.writePointer_, p.readPointer_);

    p.Write(uint32_t { 78 });
    EXPECT_EQ(p.writePointer_, Packet::HEADER_SIZE + 4);
    EXPECT_EQ(p.readPointer_, Packet::HEADER_SIZE);
    p.Write(double { 90.234 });
    EXPECT_EQ(p.writePointer_, Packet::HEADER_SIZE + 4 + 8);
    EXPECT_EQ(p.readPointer_, Packet::HEADER_SIZE);
    p.Write(std::string { "qweasdzxc" });
    EXPECT_EQ(p.writePointer_, Packet::HEADER_SIZE + 4 + 8 + 9);
    EXPECT_EQ(p.readPointer_, Packet::HEADER_SIZE);

    p.Read<double>();
    EXPECT_EQ(p.writePointer_, Packet::HEADER_SIZE + 4 + 8 + 9);
    EXPECT_EQ(p.readPointer_, Packet::HEADER_SIZE + 8);
    p.Read<uint32_t>();
    EXPECT_EQ(p.writePointer_, Packet::HEADER_SIZE + 4 + 8 + 9);
    EXPECT_EQ(p.readPointer_, Packet::HEADER_SIZE + 8 + 4);
    p.Read<std::string>(4);
    EXPECT_EQ(p.writePointer_, Packet::HEADER_SIZE + 4 + 8 + 9);
    EXPECT_EQ(p.readPointer_, Packet::HEADER_SIZE + 8 + 4 + 4);
    p.Read<std::string>(5);
    EXPECT_EQ(p.writePointer_, Packet::HEADER_SIZE + 4 + 8 + 9);
    EXPECT_EQ(p.readPointer_, Packet::HEADER_SIZE + 8 + 4 + 4 + 5);
    EXPECT_EQ(p.readPointer_, p.writePointer_);
}

/*
 * @tc.name: PacketReleaseTest
 * @tc.desc: Test Packet release correctness
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerPacketTest, PacketReleaseTest, testing::ext::TestSize.Level1)
{
    Packet p { Packet::BINARY };
    const std::vector<char> rawEmpty { 0, 5, 0, 0, 0 };
    EXPECT_EQ(p.data_.size(), Packet::HEADER_SIZE);
    EXPECT_EQ(p.data_, rawEmpty);
    p.Write(uint64_t(9));
    p.Write(std::string { "abc" });

    auto raw = p.Release();
    const std::vector<char> rawUninitialized { 3, 5, 0, 0, 0 };
    EXPECT_EQ(p.data_.size(), Packet::HEADER_SIZE);
    EXPECT_EQ(p.data_, rawUninitialized);
    EXPECT_EQ(p.GetType(), Packet::UNKNOWN);
    EXPECT_EQ(p.GetLength(), Packet::HEADER_SIZE);
    EXPECT_EQ(p.GetPayloadLength(), 0);
    std::vector<char> rawExpected { 0, 16, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 'a', 'b', 'c' };
    EXPECT_EQ(raw, rawExpected);
}

} // namespace OHOS::Rosen