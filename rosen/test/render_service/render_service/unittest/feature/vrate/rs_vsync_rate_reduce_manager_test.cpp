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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "limit_number.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "feature/vrate/rs_vsync_rate_reduce_manager.h"
#include "pipeline/rs_test_util.h"
#include "system/rs_system_parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
constexpr float V_VAL_LEVEL_1 = 1.0f;
constexpr float V_VAL_LEVEL_2 = 0.8f;
constexpr float V_VAL_LEVEL_3 = 0.6f;
constexpr float V_VAL_LEVEL_4 = 0.4f;
constexpr float V_VAL_LEVEL_5 = 0.2f;
constexpr float V_VAL_LEVEL_6 = 0.1f;
constexpr float CONTINUOUS_RATIO_LEVEL_0 = 4.0f / 8.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_1 = 3.0f / 8.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_2 = 2.0f / 8.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_3 = 1.0f / 8.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_4 = 1.0f / 10.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_5 = 1.0f / 12.0f;
constexpr int32_t DEFAULT_RATE = 1;
}

class MockVSyncDistributor : public VSyncDistributor {
public:
    MockVSyncDistributor(): VSyncDistributor(nullptr, "") {}
    MOCK_METHOD3(SetQosVSyncRate, VsyncError(uint64_t, int32_t, bool));
};

class RSVsyncRateReduceManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSVsyncRateReduceManagerTest::SetUpTestCase() {}
void RSVsyncRateReduceManagerTest::TearDownTestCase() {}
void RSVsyncRateReduceManagerTest::SetUp() {}
void RSVsyncRateReduceManagerTest::TearDown() {}

/**
 * @tc.name: EnqueueFrameDuration001
 * @tc.desc: Test EnqueueFrameDuration, collect last 3 frames duration to estimate RT balance
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, EnqueueFrameDuration001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    auto func = [&manager = rateReduceManager] (int size, float val) {
        manager.frameDurations_.clear();
        for (int i = 0; i < size; i++) {
            manager.EnqueueFrameDuration(val);
        }
        return manager.frameDurations_.size();
    };
    float basePeriodRatio = 1.0f;
    EXPECT_EQ(1, func(1, basePeriodRatio)); // collect last 1 frames duration
    EXPECT_EQ(2, func(2, basePeriodRatio)); // collect last 2 frames duration
    EXPECT_EQ(3, func(3, basePeriodRatio)); // collect last 3 frames duration
    EXPECT_EQ(3, func(4, basePeriodRatio)); // collect last 4 frames duration
    EXPECT_EQ(3, func(5, basePeriodRatio)); // collect last 5 frames duration
}

/**
 * @tc.name: Init001
 * @tc.desc: Test Init processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, Init001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    sptr<MockVSyncDistributor> vSyncDistributor = new MockVSyncDistributor();
    rateReduceManager.Init(vSyncDistributor);
    if (rateReduceManager.GetVRateDeviceSupport()) {
        EXPECT_EQ(true, rateReduceManager.GetVRateReduceEnabled());
    }
    EXPECT_NE(nullptr, rateReduceManager.appVSyncDistributor_);
}

/**
 * @tc.name: ResetFrameValues001
 * @tc.desc: Test ResetFrameValues processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, ResetFrameValues001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    sptr<MockVSyncDistributor> vSyncDistributor = new MockVSyncDistributor();
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "surfaceNode";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect = {0, 0, 50, 50};
    surfaceNode->SetDstRect(rect);
    Occlusion::Rect selfDrawRect(0, 0, 100, 100);
    Occlusion::Region selfDrawRegion{selfDrawRect};
    surfaceNode->SetVisibleRegion(selfDrawRegion);
    ScreenInfo screenInfo;

    NodeId nodeId = 1;
    rateReduceManager.appVSyncDistributor_ = vSyncDistributor;
    rateReduceManager.vRateReduceEnabled_ = false;
    rateReduceManager.ResetFrameValues(120);
    EXPECT_EQ(false, rateReduceManager.vRateConditionQualified_);
    rateReduceManager.PushWindowNodeId(nodeId);
    EXPECT_EQ(0, rateReduceManager.oneFramePeriod_);
    EXPECT_EQ(true, rateReduceManager.curAllMainAndLeashWindowNodesIds_.empty());

    rateReduceManager.lastVisMapForVSyncVisLevel_.emplace(nodeId, RSVisibleLevel::RS_ALL_VISIBLE);
    rateReduceManager.ClearLastVisMapForVsyncRate();
    EXPECT_EQ(false, rateReduceManager.lastVisMapForVSyncVisLevel_.empty());
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    EXPECT_EQ(true, rateReduceManager.surfaceVRateMap_.empty());

    rateReduceManager.FrameDurationBegin();
    EXPECT_EQ(0, rateReduceManager.curTime_);
    int noneZero = 100;
    rateReduceManager.curTime_ = noneZero;
    rateReduceManager.FrameDurationEnd();
    EXPECT_NE(0, rateReduceManager.curTime_);
    rateReduceManager.isReduceBySystemAnimatedScenes_ = true;
    rateReduceManager.SetIsReduceBySystemAnimatedScenes(false);
    EXPECT_EQ(true, rateReduceManager.isReduceBySystemAnimatedScenes_);

    rateReduceManager.vRateConditionQualified_ = true;
    rateReduceManager.oneFramePeriod_ = 0;
    rateReduceManager.frameDurations_.clear();
    rateReduceManager.FrameDurationEnd();
    EXPECT_EQ(true, rateReduceManager.frameDurations_.empty());
}

/**
 * @tc.name: ResetFrameValues002
 * @tc.desc: Test ResetFrameValues processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, ResetFrameValues002, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    sptr<MockVSyncDistributor> vSyncDistributor = new MockVSyncDistributor();
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "surfaceNode";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect = {0, 0, 50, 50};
    surfaceNode->SetDstRect(rect);
    Occlusion::Rect selfDrawRect(0, 0, 100, 100);
    Occlusion::Region selfDrawRegion{selfDrawRect};
    surfaceNode->SetVisibleRegion(selfDrawRegion);
    ScreenInfo screenInfo;

    NodeId nodeId = 1;
    rateReduceManager.appVSyncDistributor_ = vSyncDistributor;
    rateReduceManager.vRateReduceEnabled_ = true;
    rateReduceManager.ResetFrameValues(120);
    EXPECT_EQ(true, rateReduceManager.vRateConditionQualified_);
    rateReduceManager.PushWindowNodeId(nodeId);
    EXPECT_NE(0, rateReduceManager.oneFramePeriod_);
    EXPECT_EQ(false, rateReduceManager.curAllMainAndLeashWindowNodesIds_.empty());

    rateReduceManager.lastVisMapForVSyncVisLevel_.emplace(nodeId, RSVisibleLevel::RS_ALL_VISIBLE);
    rateReduceManager.ClearLastVisMapForVsyncRate();
    EXPECT_EQ(true, rateReduceManager.lastVisMapForVSyncVisLevel_.empty());
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    EXPECT_EQ(false, rateReduceManager.surfaceVRateMap_.empty());

    rateReduceManager.FrameDurationBegin();
    EXPECT_LT(0, rateReduceManager.curTime_);
    rateReduceManager.curTime_ = 100;
    rateReduceManager.FrameDurationEnd();
    EXPECT_EQ(0, rateReduceManager.curTime_);
    rateReduceManager.isReduceBySystemAnimatedScenes_ = true;
    rateReduceManager.SetIsReduceBySystemAnimatedScenes(false);
    EXPECT_EQ(false, rateReduceManager.isReduceBySystemAnimatedScenes_);

    rateReduceManager.ResetFrameValues(0);
    EXPECT_EQ(false, rateReduceManager.vRateConditionQualified_);
    rateReduceManager.curTime_ = 0;
    rateReduceManager.FrameDurationBegin();
    EXPECT_EQ(0, rateReduceManager.curTime_);
    rateReduceManager.curTime_ = 100;
    rateReduceManager.FrameDurationEnd();
    EXPECT_NE(0, rateReduceManager.curTime_);
}

/**
 * @tc.name: CollectSurfaceVsyncInfo001
 * @tc.desc: Test CollectSurfaceVsyncInfo processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, CollectSurfaceVsyncInfo001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);

    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "surfaceNode";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(surfaceNode, nullptr);
    RectI rect = {0, 0, 50, 50};
    surfaceNode->SetDstRect(rect);
    Occlusion::Rect selfDrawRect(0, 0, 100, 100);
    Occlusion::Region selfDrawRegion{selfDrawRect};
    surfaceNode->SetVisibleRegion(selfDrawRegion);

    RSSurfaceRenderNodeConfig config2;
    config2.id = 12;
    config2.name = "surfaceNode2";
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(config2, rsContext->weak_from_this());
    ASSERT_NE(surfaceNode2, nullptr);

    RectI rect2 = {0, 0, 50, 50};
    surfaceNode2->SetDstRect(rect2);
    Occlusion::Rect selfDrawRect2(0, 0, 100, 100);
    Occlusion::Region selfDrawRegion2{selfDrawRect2};
    surfaceNode2->SetVisibleRegion(selfDrawRegion2);

    RSVsyncRateReduceManager rateReduceManager;
    rateReduceManager.vRateReduceEnabled_ = true;
    rateReduceManager.vRateConditionQualified_ = true;
    ScreenInfo screenInfo;
    int count = 2;
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode2);
    EXPECT_EQ(count, rateReduceManager.surfaceVRateMap_.size());
}

/**
 * @tc.name: CheckNeedNotify001
 * @tc.desc: Test CheckNeedNotify processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, CheckNeedNotify001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    NodeId nodeId = 1;
    NodeId nodeId2 = 2;
    int rate1 = 2;
    int rate2 = 3;
    rateReduceManager.lastFocusedNodeId_ = 0;
    rateReduceManager.focusedNodeId_ = nodeId;
    rateReduceManager.isSystemAnimatedScenes_ = true;
    EXPECT_EQ(true, rateReduceManager.CheckNeedNotify());
    EXPECT_EQ(false, rateReduceManager.CheckNeedNotify());
    EXPECT_EQ(true, rateReduceManager.isReduceBySystemAnimatedScenes_);

    rateReduceManager.isSystemAnimatedScenes_ = false;
    rateReduceManager.curAllMainAndLeashWindowNodesIds_.emplace_back(nodeId);
    rateReduceManager.lastVSyncRateMap_.emplace(nodeId2, rate2);
    rateReduceManager.vSyncRateMap_.emplace(nodeId, rate1);
    EXPECT_EQ(true, rateReduceManager.CheckNeedNotify());
    auto& vSyncRateMap = rateReduceManager.vSyncRateMap_;
    EXPECT_NE(vSyncRateMap.end(), vSyncRateMap.find(nodeId2));
    EXPECT_EQ(false, rateReduceManager.CheckNeedNotify());
    EXPECT_NE(vSyncRateMap.end(), vSyncRateMap.find(nodeId));
}

/**
 * @tc.name: NotifyVRates001
 * @tc.desc: Test NotifyVRates processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, NotifyVRates001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    sptr<MockVSyncDistributor> vSyncDistributor = new MockVSyncDistributor();
    rateReduceManager.appVSyncDistributor_ = vSyncDistributor;

    VsyncError error = VsyncError::GSERROR_OK;
    EXPECT_CALL(*vSyncDistributor, SetQosVSyncRate(0, 0, false)).WillRepeatedly(testing::Return(error));
    vSyncDistributor->SetQosVSyncRate(0, 0, false);

    rateReduceManager.vRateReduceEnabled_ = true;
    rateReduceManager.vRateConditionQualified_ = true;
    rateReduceManager.SetUniVsync();

    NodeId nodeId = 1;
    NodeId nodeId2 = 2;
    int rate1 = 2;
    int rate2 = 3;
    rateReduceManager.lastFocusedNodeId_ = 0;
    rateReduceManager.focusedNodeId_ = nodeId;
    rateReduceManager.lastVSyncRateMap_.emplace(nodeId, rate1);
    rateReduceManager.lastVSyncRateMap_.emplace(nodeId2, rate1);
    rateReduceManager.vSyncRateMap_.emplace(nodeId, rate1);
    rateReduceManager.vSyncRateMap_.emplace(nodeId2, rate2);
    rateReduceManager.NotifyVRates();
    EXPECT_EQ(rateReduceManager.lastVSyncRateMap_, rateReduceManager.vSyncRateMap_);

    rateReduceManager.lastVSyncRateMap_.clear();
    rateReduceManager.vSyncRateMap_.clear();
    rateReduceManager.lastFocusedNodeId_ = 0;
    rateReduceManager.focusedNodeId_ = nodeId;
    rateReduceManager.lastVSyncRateMap_.emplace(nodeId, rate1);
    rateReduceManager.lastVSyncRateMap_.emplace(nodeId2, rate1);
    rateReduceManager.vSyncRateMap_.emplace(nodeId, rate1);
    rateReduceManager.NotifyVRates();
    EXPECT_EQ(rateReduceManager.lastVSyncRateMap_, rateReduceManager.vSyncRateMap_);
}

/**
 * @tc.name: UpdateRatesLevel001
 * @tc.desc: Test UpdateRatesLevel processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, UpdateRatesLevel001, TestSize.Level1)
{
    constexpr float workloadTimes[] = {1.0f, 1.5f, 2.0f, 2.5f};
    RSVsyncRateReduceManager rateReduceManager;
    auto func = [&manager = rateReduceManager] (int size, float val) {
        for (int i = 0; i < size; i++) {
            manager.EnqueueFrameDuration(val);
        }
        return manager.UpdateRatesLevel();
    };
    float delta = 0.3f;
    rateReduceManager.curRatesLevel_ = 0;
    EXPECT_EQ(0, func(3, workloadTimes[0] - delta)); // lower than 1.0 times
    rateReduceManager.curRatesLevel_ = 0;
    EXPECT_EQ(1, func(3, workloadTimes[0] + delta)); // larger than 1.0 times
    rateReduceManager.curRatesLevel_ = 0;
    EXPECT_EQ(2, func(3, workloadTimes[1] + delta)); // larger than 1.5 times
    rateReduceManager.curRatesLevel_ = 0;
    EXPECT_EQ(3, func(3, workloadTimes[2] + delta)); // larger than 2.0 times
    rateReduceManager.curRatesLevel_ = 0;
    EXPECT_EQ(4, func(3, workloadTimes[3] + delta)); // larger than 2.5 times
    EXPECT_EQ(4, func(3, workloadTimes[3] + delta));
    //curRatesLevel_ decrease one by one
    EXPECT_EQ(3, func(4, workloadTimes[0] - delta));
    EXPECT_EQ(2, func(4, workloadTimes[0] - delta));
    EXPECT_EQ(1, func(4, workloadTimes[0] - delta));
    EXPECT_EQ(0, func(4, workloadTimes[0] - delta));
    EXPECT_EQ(0, func(4, workloadTimes[0] - delta));

    rateReduceManager.curRatesLevel_ = 1;
    rateReduceManager.frameDurations_.clear();
    EXPECT_EQ(rateReduceManager.curRatesLevel_, func(2, workloadTimes[3] - delta)); // lower than 2.5 times
}

/**
 * @tc.name: CalcRates001
 * @tc.desc: Test CalcRates processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, CalcRates001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    rateReduceManager.rsRefreshRate_ = 60;
    rateReduceManager.curRatesLevel_ = 2;
    constexpr int VSYNC_RATE_TABLE_2[] = {2, 2, 3, 3};
    rateReduceManager.isSystemAnimatedScenes_ = true;
    NodeId nodeId = 1;
    SurfaceVRateInfo surfaceVRateInfo;
    surfaceVRateInfo.nodeId = nodeId;
    surfaceVRateInfo.name = "surfaceNode";
    surfaceVRateInfo.visibleRegion = Occlusion::Region(Occlusion::Rect(10, 10, 100, 100));
    surfaceVRateInfo.appWindowArea = 1000;

    rateReduceManager.surfaceVRateMap_[nodeId] = surfaceVRateInfo;
    rateReduceManager.CalcRates();
    auto iter = rateReduceManager.vSyncRateMap_.find(nodeId);
    ASSERT_NE(rateReduceManager.vSyncRateMap_.end(), iter);
    EXPECT_EQ(2, iter->second);

    rateReduceManager.isSystemAnimatedScenes_ = false;
    rateReduceManager.surfaceVRateMap_.emplace(nodeId, surfaceVRateInfo);
    rateReduceManager.vSyncRateMap_.clear();
    rateReduceManager.CalcRates();
    iter = rateReduceManager.vSyncRateMap_.find(nodeId);
    ASSERT_EQ(rateReduceManager.vSyncRateMap_.end(), iter);

    int vRate = VSYNC_RATE_TABLE_2[1];
    surfaceVRateInfo.appWindowArea = 50000;
    rateReduceManager.surfaceVRateMap_.clear();
    rateReduceManager.surfaceVRateMap_.emplace(nodeId, surfaceVRateInfo);
    rateReduceManager.vSyncRateMap_.clear();
    rateReduceManager.CalcRates();
    iter = rateReduceManager.vSyncRateMap_.find(nodeId);
    ASSERT_NE(rateReduceManager.vSyncRateMap_.end(), iter);
    EXPECT_EQ(vRate, iter->second);
}

/**
 * @tc.name: GetRateByBalanceLevel001
 * @tc.desc: Test GetRateByBalanceLevel processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, GetRateByBalanceLevel001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    rateReduceManager.curRatesLevel_ = 1;
    rateReduceManager.rsRefreshRate_ = 120;
    EXPECT_EQ(1, rateReduceManager.GetRateByBalanceLevel(0.8));
    EXPECT_EQ(2, rateReduceManager.GetRateByBalanceLevel(0.6));
    EXPECT_EQ(2, rateReduceManager.GetRateByBalanceLevel(0.4));
    EXPECT_EQ(3, rateReduceManager.GetRateByBalanceLevel(0.2));
    EXPECT_EQ(DEFAULT_RATE, rateReduceManager.GetRateByBalanceLevel(1.0));
    EXPECT_EQ(std::numeric_limits<int>::max(), rateReduceManager.GetRateByBalanceLevel(0.0));

    rateReduceManager.curRatesLevel_ = 3;
    rateReduceManager.rsRefreshRate_ = 60;
    EXPECT_EQ(2, rateReduceManager.GetRateByBalanceLevel(0.8));
    EXPECT_EQ(2, rateReduceManager.GetRateByBalanceLevel(0.6));
    EXPECT_EQ(2, rateReduceManager.GetRateByBalanceLevel(0.4));
    EXPECT_EQ(2, rateReduceManager.GetRateByBalanceLevel(0.2));
    EXPECT_EQ(DEFAULT_RATE, rateReduceManager.GetRateByBalanceLevel(1.0));
    EXPECT_EQ(std::numeric_limits<int>::max(), rateReduceManager.GetRateByBalanceLevel(0.0));

    rateReduceManager.curRatesLevel_ = 5;
    rateReduceManager.rsRefreshRate_ = 60;
    EXPECT_EQ(1, rateReduceManager.GetRateByBalanceLevel(0.8));
}

/**
 * @tc.name: CalcMaxVisibleRect001
 * @tc.desc: Test CalcMaxVisibleRect processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, CalcMaxVisibleRect001, TestSize.Level1)
{
    int appWindowArea = 100000;
    Occlusion::Rect bounds(0, 0, 100, 100);
    Occlusion::Rect subRect(40, 20, 100, 90);
    RSVsyncRateReduceManager rateReduceManager;

    Occlusion::Region region(bounds);
    auto rect = rateReduceManager.CalcMaxVisibleRect(region, appWindowArea);
    EXPECT_EQ(false, rect.IsEmpty());

    Occlusion::Region subRegion(subRect);
    auto rectI = rateReduceManager.CalcMaxVisibleRect(region.Sub(subRegion), appWindowArea).ToRectI();
    EXPECT_LT(rectI.width_, rectI.height_);
}

/**
 * @tc.name: CalcVValByAreas001
 * @tc.desc: Test CalcVValByAreas processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, CalcVValByAreas001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    Occlusion::Region region;
    int windowArea = 1000;
    int deltaArea = 50;
    int maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_1;
    int visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_0 - deltaArea;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_1);
    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_1 - deltaArea;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_0;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_1);

    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_2;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_1 - deltaArea;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_2);
    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_2 - deltaArea;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_1;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_2);

    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_3;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_2 - deltaArea;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_3);
    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_3 - deltaArea;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_2;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_3);

    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_4;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_3 - deltaArea;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_4);
    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_4 - deltaArea;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_3;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_4);

    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_5;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_4 - deltaArea;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_5);
    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_5 - deltaArea;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_4;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_5);

    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_5 - deltaArea;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_4 - 1;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_6);
    maxVisRectArea = windowArea * CONTINUOUS_RATIO_LEVEL_5 - 1;
    visTotalArea = windowArea * CONTINUOUS_RATIO_LEVEL_5 - deltaArea;
    EXPECT_EQ(rateReduceManager.CalcVValByAreas(windowArea, maxVisRectArea, visTotalArea), V_VAL_LEVEL_6);
}

/**
 * @tc.name: SetFocusedNodeId001
 * @tc.desc: Test SetFocusedNodeId processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, SetFocusedNodeId001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    rateReduceManager.vRateReduceEnabled_ = true;
    rateReduceManager.focusedNodeId_ = 0;
    NodeId nodeId = 1;
    rateReduceManager.SetFocusedNodeId(nodeId);
    EXPECT_EQ(nodeId, rateReduceManager.focusedNodeId_);

    rateReduceManager.vRateReduceEnabled_ = false;
    rateReduceManager.focusedNodeId_ = 0;
    rateReduceManager.SetFocusedNodeId(nodeId);
    EXPECT_NE(nodeId, rateReduceManager.focusedNodeId_);
}

/**
 * @tc.name: CollectSurfaceVsyncInfo002
 * @tc.desc: Test CollectSurfaceVsyncInfo processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, CollectSurfaceVsyncInfo002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "surfaceNode";
    RSVsyncRateReduceManager rateReduceManager;
    rateReduceManager.vRateReduceEnabled_ = true;
    rateReduceManager.vRateConditionQualified_ = true;
    ScreenInfo screenInfo;

    auto buildFunc = [&config, &rateReduceManager](std::shared_ptr<RSSurfaceRenderNode>& node) {
        rateReduceManager.surfaceVRateMap_.clear();
        RectI rect = {0, 0, 50, 50};
        Occlusion::Rect selfDrawRect(0, 0, 100, 100);
        Occlusion::Region selfDrawRegion{selfDrawRect};
        ASSERT_NE(node, nullptr);
        node->SetDstRect(rect);
        node->SetVisibleRegion(selfDrawRegion);
        config.surfaceWindowType = SurfaceWindowType::DEFAULT_WINDOW;
        config.nodeType = RSSurfaceNodeType::DEFAULT;
        config.name = "surfaceNode";
    };

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    buildFunc(surfaceNode);
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    EXPECT_EQ(false, rateReduceManager.surfaceVRateMap_.empty());

    surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    buildFunc(surfaceNode);
    RectI rectEmpty;
    surfaceNode->SetDstRect(rectEmpty);
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    EXPECT_EQ(true, rateReduceManager.surfaceVRateMap_.empty());

    config.nodeType = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    buildFunc(surfaceNode);
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    EXPECT_EQ(true, rateReduceManager.surfaceVRateMap_.empty());
}

/**
 * @tc.name: CollectSurfaceVsyncInfo003
 * @tc.desc: Test CollectSurfaceVsyncInfo processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, CollectSurfaceVsyncInfo003, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "surfaceNode";
    RSVsyncRateReduceManager rateReduceManager;
    rateReduceManager.vRateReduceEnabled_ = true;
    rateReduceManager.vRateConditionQualified_ = true;
    ScreenInfo screenInfo;

    auto buildFunc = [&config, &rateReduceManager](std::shared_ptr<RSSurfaceRenderNode>& node) {
        rateReduceManager.surfaceVRateMap_.clear();
        RectI rect = {0, 0, 50, 50};
        Occlusion::Rect selfDrawRect(0, 0, 100, 100);
        Occlusion::Region selfDrawRegion{selfDrawRect};
        ASSERT_NE(node, nullptr);
        node->SetDstRect(rect);
        node->SetVisibleRegion(selfDrawRegion);
        config.surfaceWindowType = SurfaceWindowType::DEFAULT_WINDOW;
        config.nodeType = RSSurfaceNodeType::DEFAULT;
        config.name = "surfaceNode";
    };

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    buildFunc(surfaceNode);
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    EXPECT_EQ(false, rateReduceManager.surfaceVRateMap_.empty());

    config.surfaceWindowType = SurfaceWindowType::SYSTEM_SCB_WINDOW;
    surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    buildFunc(surfaceNode);
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    EXPECT_EQ(true, rateReduceManager.surfaceVRateMap_.empty());

    // IsHardwareEnabledTopSurface
    config.name = "pointer window";
    config.nodeType = RSSurfaceNodeType::CURSOR_NODE;
    surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    buildFunc(surfaceNode);
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    EXPECT_EQ(true, rateReduceManager.surfaceVRateMap_.empty());

    surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    buildFunc(surfaceNode);
    auto& properties = surfaceNode->renderContent_->renderProperties_;
    properties.boundsGeo_ = nullptr;
    rateReduceManager.CollectSurfaceVsyncInfo(screenInfo, *surfaceNode);
    EXPECT_EQ(true, rateReduceManager.surfaceVRateMap_.empty());
}

/**
 * @tc.name: NotifyVSyncRates001
 * @tc.desc: Test NotifyVSyncRates processing.
 * @tc.type: FUNC
 * @tc.require: issueIAWXLO
 */
HWTEST_F(RSVsyncRateReduceManagerTest, NotifyVSyncRates001, TestSize.Level1)
{
    RSVsyncRateReduceManager rateReduceManager;
    sptr<MockVSyncDistributor> vSyncDistributor = new MockVSyncDistributor();
    rateReduceManager.appVSyncDistributor_ = vSyncDistributor;
    rateReduceManager.vRateReduceEnabled_ = true;
    rateReduceManager.vRateConditionQualified_ = true;
    VsyncError error = VsyncError::GSERROR_OK;
    EXPECT_CALL(*vSyncDistributor, SetQosVSyncRate(0, 0, false)).WillRepeatedly(testing::Return(error));

    NodeId nodeId = 1;
    NodeId nodeId2 = 2;
    std::map<NodeId, RSVisibleLevel> vSyncVisLevelMap;
    rateReduceManager.lastVisMapForVSyncVisLevel_.emplace(nodeId, RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE);
    rateReduceManager.lastVisMapForVSyncVisLevel_.emplace(nodeId2, RSVisibleLevel::RS_ALL_VISIBLE);
    vSyncVisLevelMap.emplace(nodeId2, RSVisibleLevel::RS_INVISIBLE);
    rateReduceManager.NotifyVSyncRates(vSyncVisLevelMap);
    EXPECT_EQ(rateReduceManager.lastVisMapForVSyncVisLevel_, vSyncVisLevelMap);

    NodeId nodeId3 = 3;
    NodeId nodeId4 = 4;
    rateReduceManager.lastVisMapForVSyncVisLevel_.clear();
    rateReduceManager.lastVisMapForVSyncVisLevel_.emplace(nodeId, RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE);
    rateReduceManager.lastVisMapForVSyncVisLevel_.emplace(nodeId2, RSVisibleLevel::RS_ALL_VISIBLE);
    vSyncVisLevelMap.emplace(nodeId2, RSVisibleLevel::RS_INVISIBLE);
    vSyncVisLevelMap.emplace(nodeId3, RSVisibleLevel::RS_INVISIBLE);
    vSyncVisLevelMap.emplace(nodeId4, RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE);
    rateReduceManager.NotifyVSyncRates(vSyncVisLevelMap);
    EXPECT_EQ(rateReduceManager.lastVisMapForVSyncVisLevel_, vSyncVisLevelMap);
}
} // namespace OHOS::Rosen
