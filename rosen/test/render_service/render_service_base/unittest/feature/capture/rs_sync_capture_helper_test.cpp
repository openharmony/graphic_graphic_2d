/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <chrono>
#include <thread>
#include "gtest/gtest.h"
#include "feature/capture/rs_sync_capture_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSyncCaptureHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSyncCaptureHelperTest::SetUpTestCase() {}
void RSSyncCaptureHelperTest::TearDownTestCase() {}
void RSSyncCaptureHelperTest::SetUp() {}
void RSSyncCaptureHelperTest::TearDown() {}

/**
 * @tc.name: InsertCaptureCmdsExecutedFlag001
 * @tc.desc: test InsertCaptureCmdsExecutedFlag with invalid nodeId (0) is ignored
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, InsertCaptureCmdsExecutedFlag001, TestSize.Level1)
{
    RSSyncCaptureHelper helper;
    helper.InsertCaptureCmdsExecutedFlag(0, false);
    auto cmdFlag = helper.GetCaptureCmdsExecutedFlag(0);
    EXPECT_EQ(cmdFlag.first, true);
    EXPECT_EQ(cmdFlag.second, 0u);
}

/**
 * @tc.name: InsertCaptureCmdsExecutedFlag002
 * @tc.desc: test InsertCaptureCmdsExecutedFlag with flag=true overwrites entry
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, InsertCaptureCmdsExecutedFlag002, TestSize.Level1)
{
    RSSyncCaptureHelper helper;
    NodeId id = 100;
    helper.InsertCaptureCmdsExecutedFlag(id, true);
    auto cmdFlag = helper.GetCaptureCmdsExecutedFlag(id);
    EXPECT_EQ(cmdFlag.first, true);
    EXPECT_NE(cmdFlag.second, 0u);
}

/**
 * @tc.name: InsertCaptureCmdsExecutedFlag003
 * @tc.desc: test InsertCaptureCmdsExecutedFlag with flag=false inserts new entry
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, InsertCaptureCmdsExecutedFlag003, TestSize.Level1)
{
    RSSyncCaptureHelper helper;
    NodeId id = 101;
    helper.InsertCaptureCmdsExecutedFlag(id, false);
    auto cmdFlag = helper.GetCaptureCmdsExecutedFlag(id);
    EXPECT_EQ(cmdFlag.first, false);
    EXPECT_NE(cmdFlag.second, 0u);
}

/**
 * @tc.name: InsertCaptureCmdsExecutedFlag004
 * @tc.desc: test InsertCaptureCmdsExecutedFlag with flag=false does not overwrite existing entry
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, InsertCaptureCmdsExecutedFlag004, TestSize.Level1)
{
    RSSyncCaptureHelper helper;
    NodeId id = 102;
    helper.InsertCaptureCmdsExecutedFlag(id, true);
    auto before = helper.GetCaptureCmdsExecutedFlag(id);
    helper.InsertCaptureCmdsExecutedFlag(id, false);
    auto after = helper.GetCaptureCmdsExecutedFlag(id);
    EXPECT_EQ(after.first, true);
    EXPECT_EQ(after.second, before.second);
}

/**
 * @tc.name: InsertCaptureCmdsExecutedFlag005
 * @tc.desc: test InsertCaptureCmdsExecutedFlag evicts oldest entry when reaching size limit
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, InsertCaptureCmdsExecutedFlag005, TestSize.Level3)
{
    RSSyncCaptureHelper helper;
    NodeId oldestId = 1;
    helper.InsertCaptureCmdsExecutedFlag(oldestId, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    for (NodeId id = 2; id <= RSSyncCaptureHelper::MAX_CAPTURE_CMDS_FLAG_COUNT; ++id) {
        helper.InsertCaptureCmdsExecutedFlag(id, false);
    }
    NodeId newId = RSSyncCaptureHelper::MAX_CAPTURE_CMDS_FLAG_COUNT + 1;
    helper.InsertCaptureCmdsExecutedFlag(newId, false);
    auto oldestFlag = helper.GetCaptureCmdsExecutedFlag(oldestId);
    EXPECT_EQ(oldestFlag.first, true);
    EXPECT_EQ(oldestFlag.second, 0u);
    auto newFlag = helper.GetCaptureCmdsExecutedFlag(newId);
    EXPECT_EQ(newFlag.first, false);
}

/**
 * @tc.name: GetCaptureCmdsExecutedFlag001
 * @tc.desc: test GetCaptureCmdsExecutedFlag for existing entry
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, GetCaptureCmdsExecutedFlag001, TestSize.Level1)
{
    RSSyncCaptureHelper helper;
    NodeId id = 200;
    helper.InsertCaptureCmdsExecutedFlag(id, true);
    auto cmdFlag = helper.GetCaptureCmdsExecutedFlag(id);
    EXPECT_EQ(cmdFlag.first, true);
    EXPECT_NE(cmdFlag.second, 0u);
}

/**
 * @tc.name: GetCaptureCmdsExecutedFlag002
 * @tc.desc: test GetCaptureCmdsExecutedFlag for non-existing entry returns default
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, GetCaptureCmdsExecutedFlag002, TestSize.Level1)
{
    RSSyncCaptureHelper helper;
    auto cmdFlag = helper.GetCaptureCmdsExecutedFlag(999);
    EXPECT_EQ(cmdFlag.first, true);
    EXPECT_EQ(cmdFlag.second, 0u);
}

/**
 * @tc.name: EraseCaptureCmdsExecutedFlag001
 * @tc.desc: test EraseCaptureCmdsExecutedFlag erases existing and is safe for non-existing
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, EraseCaptureCmdsExecutedFlag001, TestSize.Level1)
{
    RSSyncCaptureHelper helper;
    NodeId id = 300;
    helper.InsertCaptureCmdsExecutedFlag(id, true);
    helper.EraseCaptureCmdsExecutedFlag(id);
    auto cmdFlag = helper.GetCaptureCmdsExecutedFlag(id);
    EXPECT_EQ(cmdFlag.first, true);
    EXPECT_EQ(cmdFlag.second, 0u);
    helper.EraseCaptureCmdsExecutedFlag(888);
}

/**
 * @tc.name: CleanupStaleEntries001
 * @tc.desc: test CleanupStaleEntries with empty map returns directly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, CleanupStaleEntries001, TestSize.Level1)
{
    RSSyncCaptureHelper helper;
    helper.CleanupStaleEntries(100);
}

/**
 * @tc.name: CleanupStaleEntries002
 * @tc.desc: test CleanupStaleEntries erases stale entries beyond max age
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, CleanupStaleEntries002, TestSize.Level2)
{
    RSSyncCaptureHelper helper;
    NodeId id = 400;
    helper.InsertCaptureCmdsExecutedFlag(id, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    helper.CleanupStaleEntries(50);
    auto cmdFlag = helper.GetCaptureCmdsExecutedFlag(id);
    EXPECT_EQ(cmdFlag.first, true);
    EXPECT_EQ(cmdFlag.second, 0u);
}

/**
 * @tc.name: CleanupStaleEntries003
 * @tc.desc: test CleanupStaleEntries keeps fresh entries within max age
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, CleanupStaleEntries003, TestSize.Level2)
{
    RSSyncCaptureHelper helper;
    NodeId id = 401;
    helper.InsertCaptureCmdsExecutedFlag(id, true);
    helper.CleanupStaleEntries(5000);
    auto cmdFlag = helper.GetCaptureCmdsExecutedFlag(id);
    EXPECT_EQ(cmdFlag.first, true);
    EXPECT_NE(cmdFlag.second, 0u);
}

/**
 * @tc.name: GetCurrentSteadyTimeMs001
 * @tc.desc: test GetCurrentSteadyTimeMs returns non-zero monotonic value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSyncCaptureHelperTest, GetCurrentSteadyTimeMs001, TestSize.Level1)
{
    RSSyncCaptureHelper helper;
    uint64_t t1 = helper.GetCurrentSteadyTimeMs();
    uint64_t t2 = helper.GetCurrentSteadyTimeMs();
    EXPECT_NE(t1, 0u);
    EXPECT_GE(t2, t1);
}
} // namespace OHOS::Rosen
