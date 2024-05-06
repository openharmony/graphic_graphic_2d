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

#include "gtest/gtest.h"
#include "rs_profiler_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProfilerUtilsTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: UtilsTimeTest
 * @tc.desc: RSProfiler Utils Time Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerUtilsTest, UtilsTimeTest, testing::ext::TestSize.Level1)
{
    EXPECT_NE(Utils::Now(), 0);
    EXPECT_DOUBLE_EQ(Utils::ToSeconds(500000000), 0.5);
    EXPECT_EQ(Utils::ToNanoseconds(0.25), 250000000);
}

/*
 * @tc.name: UtilsFileTest
 * @tc.desc: RSProfiler Utils File Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerUtilsTest, UtilsFileTest, testing::ext::TestSize.Level1)
{
    // open not existing file
    auto fd = Utils::FileOpen(",,,,,,,,,,,,,", "rb");
    EXPECT_FALSE(Utils::IsFileValid(fd));
    EXPECT_EQ(fd, nullptr);
    EXPECT_EQ(Utils::FileSize(fd), 0);
    EXPECT_EQ(Utils::FileTell(fd), 0);

    fd = Utils::FileOpen("file.json", "invalid open options");
    EXPECT_FALSE(Utils::IsFileValid(fd));
    EXPECT_EQ(fd, nullptr);
    EXPECT_EQ(Utils::FileSize(fd), 0);
    EXPECT_EQ(Utils::FileTell(fd), 0);
}

/*
 * @tc.name: UtilsFileInMemoryTest
 * @tc.desc: RSProfiler Utils File recording in memory Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerUtilsTest, UtilsFileInMemoryTest, testing::ext::TestSize.Level1)
{
    auto fd = Utils::FileOpen("RECORD_IN_MEMORY", "rwbe");
    EXPECT_EQ(fd, reinterpret_cast<FILE*>(1));
    EXPECT_TRUE(Utils::IsFileValid(fd));
    EXPECT_EQ(Utils::FileSize(fd), 0);
    EXPECT_EQ(Utils::FileTell(fd), 0);

    std::vector<char> raw { 0, 1, 2, 3 };
    Utils::FileWrite(fd, raw.data(), raw.size());
    std::vector<char> recovered(4, '\0');
    Utils::FileSeek(fd, 0, SEEK_SET);
    Utils::FileRead(fd, recovered.data(), 4);
    EXPECT_EQ(raw, recovered);
    EXPECT_EQ(Utils::FileTell(fd), 4);

    uint16_t u16 = 0;
    Utils::FileSeek(fd, 1, SEEK_SET);
    Utils::FileRead(fd, &u16, sizeof(u16));
    EXPECT_EQ(u16, 0x0201);

    fd = Utils::FileOpen("RECORD_IN_MEMORY", "wbe");
    EXPECT_TRUE(Utils::IsFileValid(fd));
    EXPECT_EQ(Utils::FileSize(fd), 0);
    EXPECT_EQ(Utils::FileTell(fd), 0);
}

/*
 * @tc.name: UtilsSplitTest
 * @tc.desc: RSProfiler Utils string split Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerUtilsTest, UtilsSplitTest, testing::ext::TestSize.Level1)
{
    std::string str = "qwe asd zxc";
    std::vector<std::string> result = { "qwe", "asd", "zxc" };
    EXPECT_EQ(Utils::Split(str), result);
}

/*
 * @tc.name: UtilsArgListTest
 * @tc.desc: RSProfiler Utils ArgList public API Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerUtilsTest, UtilsArgListTest, testing::ext::TestSize.Level1)
{
    std::vector<std::string> args { "123", "-5", "45738957893", "837983", "1.5", "678.789", "qwe" };
    ArgList list { args };

    EXPECT_EQ(list.Uint32(0), 123);
    EXPECT_EQ(list.Int32(1), -5);
    EXPECT_EQ(list.Uint64(2), 45738957893);
    EXPECT_EQ(list.Int64(3), 837983);
    EXPECT_EQ(list.Fp32(4), 1.5f);
    EXPECT_EQ(list.Fp64(5), 678.789);
    EXPECT_EQ(list.String(6), "qwe");
    EXPECT_EQ(list.Count(), 7);
    EXPECT_TRUE(list.String(7).empty());
    EXPECT_TRUE(list.String(100).empty());

    EXPECT_FALSE(list.Empty());
    list.Clear();
    EXPECT_TRUE(list.Empty());
}

/*
 * @tc.name: UtilsIdRoutineTest
 * @tc.desc: RSProfiler Utils identifiers combining routine Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerUtilsTest, UtilsIdRoutineTest, testing::ext::TestSize.Level1)
{
    constexpr uint64_t id = 0x0102030405060708;
    constexpr uint64_t nodeId = 0x000000000a0b0c0d;
    constexpr pid_t pid = 0x10203040;

    EXPECT_EQ(Utils::ExtractPid(id), 0x01020304);
    EXPECT_EQ(Utils::GetMockPid(pid), 0x50203040);
    EXPECT_EQ(Utils::ExtractNodeId(id), 0x0000000005060708);
    EXPECT_EQ(Utils::ComposeNodeId(pid, nodeId), 0x102030400a0b0c0d);
    EXPECT_EQ(Utils::ComposeMockNodeId(id, nodeId), 0x450607080a0b0c0d);
    EXPECT_EQ(Utils::GetRootNodeId(pid), 0x1020304000000001);
    EXPECT_EQ(Utils::PatchNodeId(id), 0x4102030405060708);
    EXPECT_FALSE(Utils::IsNodeIdPatched(id));
    EXPECT_TRUE(Utils::IsNodeIdPatched(Utils::PatchNodeId(id)));
}

} // namespace OHOS::Rosen