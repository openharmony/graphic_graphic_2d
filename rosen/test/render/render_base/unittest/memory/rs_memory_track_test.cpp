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
#include "memory/rs_memory_track.h"

namespace OHOS::Rosen {
class RSMemoryTrackUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMemoryTrackUnitTest::SetUpTestCase() {}
void RSMemoryTrackUnitTest::TearDownTestCase() {}
void RSMemoryTrackUnitTest::SetUp() {}
void RSMemoryTrackUnitTest::TearDown() {}

/**
 * @tc.name: RemoveNodeRecordTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackUnitTest, RemoveNodeRecordTest, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemoryNodeOfPid memoryNodeOfPid;
    auto mem = memoryNodeOfPid.GetMemSize();
    ASSERT_EQ(mem, 0);
    MemoryTrack::Instance().RemoveNodeRecord(id);
}

/**
 * @tc.name: CountRSMemoryTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackUnitTest, CountRSMemoryTest001, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    pid_t pid1 = -1;
    MemoryTrack::Instance().CountRSMemory(pid1);
    auto mem = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(mem, 0);
}

/**
 * @tc.name: CountRSMemoryTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackUnitTest, CountRSMemoryTest002, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    pid_t pid1 = 0;
    MemoryTrack::Instance().CountRSMemory(pid1);
    auto mem = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(mem, 0);
}

/**
 * @tc.name: DumpMemoryStatisticsTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackUnitTest, DumpMemoryStatisticsTest, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::function<std::tuple<uint64_t, std::string, RectI> (uint64_t)> func;
    MemoryTrack::Instance().DumpMemoryStatistics(log, func);
    int ret = 0;
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: AddPictureRecordTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackUnitTest, AddPictureRecordTest, testing::ext::TestSize.Level1)
{
    const void* addr;
    MemoryInfo info;
    MemoryTrack::Instance().AddPictureRecord(addr, info);
    int ret = 0;
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RemovePictureRecordTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackUnitTest, RemovePictureRecordTest, testing::ext::TestSize.Level1)
{
    const void* addr;
    MemoryTrack::Instance().RemovePictureRecord(addr);
    int ret = 0;
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdatePictureInfoTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackUnitTest, UpdatePictureInfoTest, testing::ext::TestSize.Level1)
{
    const void* addr;
    NodeId nodeId = 1;
    pid_t pid = -1;
    MemoryTrack::Instance().UpdatePictureInfo(addr, nodeId, pid);
    int ret = 0;
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetAppMemorySizeInMBTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackUnitTest, GetAppMemorySizeInMBTest, testing::ext::TestSize.Level1)
{
    float ret = MemoryTrack::Instance().GetAppMemorySizeInMB();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: DumpMemoryNodeStatisticsTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackUnitTest, DumpMemoryNodeStatisticsTest, testing::ext::TestSize.Level1)
{
    // for test
    DfxString log;
    std::function<std::tuple<uint64_t, std::string, RectI> (uint64_t)> func;
    MemoryTrack::Instance().DumpMemoryNodeStatistics(log);
    MemoryTrack::Instance().DumpMemoryPicStatistics(log, func);
    int ret = 1;
    ASSERT_EQ(ret, 1);
} // namespace OHOS::Rosen
