/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <memory>
#include <vector>
#include <thread>
#include <chrono>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "feature/frame_stability/rs_frame_stability_manager.h"
#include "ipc_callbacks/rs_frame_stability_callback_stub.h"
#include "transaction/rs_frame_stability_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class MockRSFrameStabilityCallbackStub : public RSFrameStabilityCallbackStub {
public:
    MockRSFrameStabilityCallbackStub() = default;
    virtual ~MockRSFrameStabilityCallbackStub() noexcept = default;

    MOCK_METHOD(void, OnFrameStabilityChanged, (bool isStable), (override));
};

class RSFrameStabilityManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    static constexpr uint64_t TEST_TARGET_ID_1 = 1;
    static constexpr uint64_t TEST_TARGET_ID_2 = 2;
    static constexpr pid_t TEST_PID = 1000;
    static constexpr uint32_t TEST_STABLE_DURATION_MS = 100;
    static constexpr float TEST_CHANGE_PERCENT = 0.5f;
};

void RSFrameStabilityManagerTest::SetUpTestCase() {}

void RSFrameStabilityManagerTest::TearDownTestCase() {}

void RSFrameStabilityManagerTest::SetUp() {}

void RSFrameStabilityManagerTest::TearDown() {}

/**
 * @tc.name: GetInstance_Singleton
 * @tc.desc: Verify singleton pattern, GetInstance returns same instance
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, GetInstance_Singleton, TestSize.Level1)
{
    auto& manager1 = RSFrameStabilityManager::GetInstance();
    auto& manager2 = RSFrameStabilityManager::GetInstance();
    EXPECT_EQ(&manager1, &manager2);
}

/**
 * @tc.name: RegisterFrameStabilityDetection_Lifecycle
 * @tc.desc: Test frame stability detection registration and unregistration lifecycle
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RegisterFrameStabilityDetection_Lifecycle, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());

    int32_t result = RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS));

    result = RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::TARGET_ID_ALREADY_REGISTERED));

    result = RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS));

    result = RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::TARGET_ID_NOT_REGISTERED));
}

/**
 * @tc.name: RegisterFrameStabilityDetection_NullCallback
 * @tc.desc: Test registration with null callback function
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RegisterFrameStabilityDetection_NullCallback, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    int32_t result = RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, nullptr);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::NULL_CALLBACK));
}

/**
 * @tc.name: StartFrameStabilityCollection_Lifecycle
 * @tc.desc: Test frame stability collection start and query lifecycle
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, StartFrameStabilityCollection_Lifecycle, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    int32_t result = RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(
        TEST_PID, target, config);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS));

    result = RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(
        TEST_PID, target, config);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::TARGET_ID_ALREADY_REGISTERED));

    bool resultValue = false;
    result = RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target, resultValue);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS));

    result = RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target, resultValue);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::TARGET_ID_NOT_REGISTERED));
}

/**
 * @tc.name: GetFrameStabilityResult_ThresholdComparison
 * @tc.desc: Test threshold comparison logic with different change percentages
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, GetFrameStabilityResult_ThresholdComparison, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = 0.5f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(TEST_PID, target, config);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    bool result = false;
    RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target, result);
    EXPECT_TRUE(result);

    RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(TEST_PID, target, config);
    config.changePercent = 0.1f;
    dirtyRegions.clear();
    dirtyRegions.push_back({0, 0, 1000, 1000});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target, result);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: RecordCurrentFrameDirty_EmptyContexts
 * @tc.desc: Test recording dirty regions with no registered contexts
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordCurrentFrameDirty_EmptyContexts, TestSize.Level1)
{
    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);
    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);
}

/**
 * @tc.name: RecordDirtyToDetector_StateTransitions
 * @tc.desc: Test detector state transitions from unstable to stable
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_StateTransitions, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = 10;
    config.changePercent = 0.5f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(false)).Times(1);
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(true)).Times(1);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 1000, 1000});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    std::this_thread::sleep_for(std::chrono::milliseconds(15));

    dirtyRegions.clear();
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: RecordDirtyToCollector_AccumulateRegions
 * @tc.desc: Test collector accumulating multiple dirty regions
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToCollector_AccumulateRegions, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = 0.5f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(TEST_PID, target, config);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    dirtyRegions.clear();
    dirtyRegions.push_back({100, 100, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    bool result = false;
    RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target, result);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: MultipleTargets_Independent
 * @tc.desc: Test multiple independent targets working simultaneously
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, MultipleTargets_Independent, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;

    FrameStabilityTarget target1;
    target1.id = TEST_TARGET_ID_1;
    target1.type = FrameStabilityTargetType::SCREEN;

    FrameStabilityTarget target2;
    target2.id = TEST_TARGET_ID_2;
    target2.type = FrameStabilityTargetType::WINDOW;

    auto mockCallback1 = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    auto mockCallback2 = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback1, OnFrameStabilityChanged(_)).Times(0);
    EXPECT_CALL(*mockCallback2, OnFrameStabilityChanged(false)).Times(1);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target1, config, mockCallback1);
    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target2, config, mockCallback2);

    std::vector<RectI> dirtyRegions1;
    dirtyRegions1.push_back({0, 0, 100, 100});

    std::vector<RectI> dirtyRegions2;
    dirtyRegions2.push_back({0, 0, 800, 800});

    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions1, 1000.0f * 1000.0f);
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_2, dirtyRegions2, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target1);
    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target2);
}

/**
 * @tc.name: EdgeCases_ZeroAndEmpty
 * @tc.desc: Test zero value and empty value boundary cases
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, EdgeCases_ZeroAndEmpty, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = 0.0f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(false)).Times(1);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 1, 1});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);

    RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(TEST_PID, target, config);
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 0.0f);

    bool result = false;
    RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target, result);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: EdgeCases_MultipleDirtyRegions
 * @tc.desc: Test handling of multiple dirty regions
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, EdgeCases_MultipleDirtyRegions, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = 0.01f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(false)).Times(1);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    dirtyRegions.push_back({100, 100, 100, 100});
    dirtyRegions.push_back({200, 200, 100, 100});

    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);

    RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(TEST_PID, target, config);
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    bool result = false;
    RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target, result);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: EdgeCases_OverlappingDirtyRegions
 * @tc.desc: Test handling of overlapping dirty regions
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, EdgeCases_OverlappingDirtyRegions, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = 0.01f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(false)).Times(1);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    dirtyRegions.push_back({50, 50, 100, 100});
    dirtyRegions.push_back({100, 100, 100, 100});

    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: EdgeCases_VeryShortStableDuration
 * @tc.desc: Test very short stable duration
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, EdgeCases_VeryShortStableDuration, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = 1;
    config.changePercent = 0.5f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(true)).Times(1);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    dirtyRegions.clear();
    dirtyRegions.push_back({10, 10, 10, 10});
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: EdgeCases_MultipleTargetsDifferentTypes
 * @tc.desc: Test multiple targets with different types
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, EdgeCases_MultipleTargetsDifferentTypes, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;

    FrameStabilityTarget target1;
    target1.id = TEST_TARGET_ID_1;
    target1.type = FrameStabilityTargetType::SCREEN;

    FrameStabilityTarget target2;
    target2.id = TEST_TARGET_ID_2;
    target2.type = FrameStabilityTargetType::WINDOW;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target1, config, mockCallback);
    RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(TEST_PID, target2, config);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});

    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(TEST_TARGET_ID_2, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target1);

    bool result = false;
    RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target2, result);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: HandleStabilityTimeout_TargetNotFound
 * @tc.desc: Test handling timeout when target not found
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, HandleStabilityTimeout_TargetNotFound, TestSize.Level1)
{
    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);
    RSFrameStabilityManager::GetInstance().HandleStabilityTimeout(TEST_TARGET_ID_1);
}

/**
 * @tc.name: HandleStabilityTimeout_NoPendingTask
 * @tc.desc: Test handling timeout when no pending task
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, HandleStabilityTimeout_NoPendingTask, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    {
        std::lock_guard<std::mutex> lock(RSFrameStabilityManager::GetInstance().detectorMutex_);
        auto it = RSFrameStabilityManager::GetInstance().screenDetectorContexts_.find(TEST_TARGET_ID_1);
        if (it != RSFrameStabilityManager::GetInstance().screenDetectorContexts_.end()) {
            it->second->hasPendingStabilityTask = false;
        }
    }
    RSFrameStabilityManager::GetInstance().HandleStabilityTimeout(TEST_TARGET_ID_1);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: HandleStabilityTimeout_StateAlreadyStable
 * @tc.desc: Test handling timeout when state already stable
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, HandleStabilityTimeout_StateAlreadyStable, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    {
        std::lock_guard<std::mutex> lock(RSFrameStabilityManager::GetInstance().detectorMutex_);
        auto it = RSFrameStabilityManager::GetInstance().screenDetectorContexts_.find(TEST_TARGET_ID_1);
        if (it != RSFrameStabilityManager::GetInstance().screenDetectorContexts_.end()) {
            it->second->state = DetectionState::STABLE;
        }
    }

    RSFrameStabilityManager::GetInstance().HandleStabilityTimeout(TEST_TARGET_ID_1);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: HandleStabilityTimeout_StateNotStable
 * @tc.desc: Test handling timeout when state not stable
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, HandleStabilityTimeout_StateNotStable, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(true)).Times(1);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    RSFrameStabilityManager::GetInstance().HandleStabilityTimeout(TEST_TARGET_ID_1);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: TriggerCallback_NullCallback
 * @tc.desc: Test triggering callback with null callback
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, TriggerCallback_NullCallback, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);

    // target not found
    RSFrameStabilityManager::GetInstance().TriggerCallback(TEST_TARGET_ID_1, true);

    // null callback
    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    {
        std::lock_guard<std::mutex> lock(RSFrameStabilityManager::GetInstance().detectorMutex_);
        auto it = RSFrameStabilityManager::GetInstance().screenDetectorContexts_.find(TEST_TARGET_ID_1);
        if (it != RSFrameStabilityManager::GetInstance().screenDetectorContexts_.end()) {
            it->second->callback = nullptr;
        }
    }

    RSFrameStabilityManager::GetInstance().TriggerCallback(TEST_TARGET_ID_1, true);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: CalculateRegionPercentage_ZeroScreenArea
 * @tc.desc: Test region percentage calculation with zero screen area
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, CalculateRegionPercentage_ZeroScreenArea, TestSize.Level1)
{
    Occlusion::Region region;
    float result = RSFrameStabilityManager::GetInstance().CalculateRegionPercentage(region, 0.0f);
    EXPECT_EQ(result, 0.0f);

    result = RSFrameStabilityManager::GetInstance().CalculateRegionPercentage(region, -1.0f);
    EXPECT_EQ(result, 0.0f);
}

/**
 * @tc.name: CalculateRegionPercentage_NormalCase
 * @tc.desc: Test region percentage calculation in normal cases
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, CalculateRegionPercentage_NormalCase, TestSize.Level1)
{
    Occlusion::Region region(Occlusion::Rect(0, 0, 1000, 1000));
    float result = RSFrameStabilityManager::GetInstance().CalculateRegionPercentage(region, 1000.0f * 1000.0f);
    EXPECT_FLOAT_EQ(result, 1.0f);

    Occlusion::Region region2(Occlusion::Rect(0, 0, 500, 500));
    result = RSFrameStabilityManager::GetInstance().CalculateRegionPercentage(region2, 1000.0f * 1000.0f);
    EXPECT_FLOAT_EQ(result, 0.25f);
}

/**
 * @tc.name: RecordDirtyToDetector_NotRegistered
 * @tc.desc: Test recording dirty regions to detector when not registered
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_NotRegistered, TestSize.Level1)
{
    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);
    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);
}

/**
 * @tc.name: RecordDirtyToDetector_NullCallback
 * @tc.desc: Test recording dirty regions to detector with null callback
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_NullCallback, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    {
        std::lock_guard<std::mutex> lock(RSFrameStabilityManager::GetInstance().detectorMutex_);
        auto it = RSFrameStabilityManager::GetInstance().screenDetectorContexts_.find(TEST_TARGET_ID_1);
        if (it != RSFrameStabilityManager::GetInstance().screenDetectorContexts_.end()) {
            it->second->callback = nullptr;
        }
    }

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: RecordDirtyToDetector_PercentageBelowThreshold
 * @tc.desc: Test recording dirty regions when percentage below threshold
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_PercentageBelowThreshold, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = 0.5f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 10, 10});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: RecordDirtyToDetector_PercentageAboveThreshold
 * @tc.desc: Test recording dirty regions when percentage above threshold
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_PercentageAboveThreshold, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = 0.1f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(1);
    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 500, 500});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: RecordDirtyToDetector_StateAlreadyNotStable
 * @tc.desc: Test recording dirty regions when state already not stable
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_StateAlreadyNotStable, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = 0.1f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    {
        std::lock_guard<std::mutex> lock(RSFrameStabilityManager::GetInstance().detectorMutex_);
        auto it = RSFrameStabilityManager::GetInstance().screenDetectorContexts_.find(TEST_TARGET_ID_1);
        if (it != RSFrameStabilityManager::GetInstance().screenDetectorContexts_.end()) {
            it->second->state = DetectionState::NOTSTABLE;
        }
    }

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 500, 500});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: RecordDirtyToDetector_NoPendingTaskWhenAboveThreshold
 * @tc.desc: Test recording dirty regions with no pending task when above threshold
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_NoPendingTaskWhenAboveThreshold, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = 0.1f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(1);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    {
        std::lock_guard<std::mutex> lock(RSFrameStabilityManager::GetInstance().detectorMutex_);
        auto it = RSFrameStabilityManager::GetInstance().screenDetectorContexts_.find(TEST_TARGET_ID_1);
        if (it != RSFrameStabilityManager::GetInstance().screenDetectorContexts_.end()) {
            it->second->hasPendingStabilityTask = false;
        }
    }

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 500, 500});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: RecordDirtyToDetector_ElapsedBelowStableDuration
 * @tc.desc: Test recording dirty regions when elapsed time below stable duration
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_ElapsedBelowStableDuration, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = 1000;
    config.changePercent = 0.5f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);
    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: RecordDirtyToDetector_ElapsedAboveStableDuration
 * @tc.desc: Test recording dirty regions when elapsed time above stable duration
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_ElapsedAboveStableDuration, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = 10;
    config.changePercent = 0.5f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(true)).Times(1);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    std::this_thread::sleep_for(std::chrono::milliseconds(15));

    dirtyRegions.clear();
    dirtyRegions.push_back({10, 10, 10, 10});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: RecordDirtyToDetector_StateAlreadyStableWhenElapsed
 * @tc.desc: Test recording dirty regions when state already stable after elapsed time
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToDetector_StateAlreadyStableWhenElapsed, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = 10;
    config.changePercent = 0.5f;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);

    RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    std::this_thread::sleep_for(std::chrono::milliseconds(15));

    {
        std::lock_guard<std::mutex> lock(RSFrameStabilityManager::GetInstance().detectorMutex_);
        auto it = RSFrameStabilityManager::GetInstance().screenDetectorContexts_.find(TEST_TARGET_ID_1);
        if (it != RSFrameStabilityManager::GetInstance().screenDetectorContexts_.end()) {
            it->second->state = DetectionState::STABLE;
        }
    }

    dirtyRegions.clear();
    dirtyRegions.push_back({10, 10, 10, 10});
    RSFrameStabilityManager::GetInstance().RecordDirtyToDetector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, target);
}

/**
 * @tc.name: RecordDirtyToCollector_NotRegistered
 * @tc.desc: Test recording dirty regions to collector when not registered
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToCollector_NotRegistered, TestSize.Level1)
{
    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());
    EXPECT_CALL(*mockCallback, OnFrameStabilityChanged(_)).Times(0);
    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordDirtyToCollector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);
}

/**
 * @tc.name: RecordDirtyToCollector_NormalCase
 * @tc.desc: Test recording dirty regions to collector in normal case
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToCollector_NormalCase, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(TEST_PID, target, config);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordDirtyToCollector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    bool result = false;
    RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target, result);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: RecordDirtyToCollector_MultipleRegions
 * @tc.desc: Test recording multiple dirty regions to collector
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RecordDirtyToCollector_MultipleRegions, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.id = TEST_TARGET_ID_1;
    target.type = FrameStabilityTargetType::SCREEN;

    RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(TEST_PID, target, config);

    std::vector<RectI> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    dirtyRegions.push_back({100, 100, 100, 100});
    dirtyRegions.push_back({200, 200, 100, 100});
    RSFrameStabilityManager::GetInstance().RecordDirtyToCollector(TEST_TARGET_ID_1, dirtyRegions, 1000.0f * 1000.0f);

    bool result = false;
    RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, target, result);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: RegisterFrameStabilityDetection_MaxConnectionsExceeded
 * @tc.desc: Test registration when maximum connections exceeded
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, RegisterFrameStabilityDetection_MaxConnectionsExceeded, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.type = FrameStabilityTargetType::SCREEN;

    auto mockCallback = sptr<MockRSFrameStabilityCallbackStub>(new MockRSFrameStabilityCallbackStub());

    std::vector<FrameStabilityTarget> registeredTargets;

    for (uint64_t i = 1; i <= MAX_FRAME_STABILITY_CONNECTION_NUM; i++) {
        target.id = i;
        int32_t result = RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
            TEST_PID, target, config, mockCallback);
        EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS));
        registeredTargets.push_back(target);
    }

    target.id = MAX_FRAME_STABILITY_CONNECTION_NUM + 1;
    int32_t result = RSFrameStabilityManager::GetInstance().RegisterFrameStabilityDetection(
        TEST_PID, target, config, mockCallback);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::CONNECTIONS_OCCUPIED));

    for (const auto& regTarget : registeredTargets) {
        RSFrameStabilityManager::GetInstance().UnregisterFrameStabilityDetection(TEST_PID, regTarget);
    }
}

/**
 * @tc.name: StartFrameStabilityCollection_MaxConnectionsExceeded
 * @tc.desc: Test collection start when maximum connections exceeded
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSFrameStabilityManagerTest, StartFrameStabilityCollection_MaxConnectionsExceeded, TestSize.Level1)
{
    FrameStabilityConfig config;
    config.stableDuration = TEST_STABLE_DURATION_MS;
    config.changePercent = TEST_CHANGE_PERCENT;
    FrameStabilityTarget target;
    target.type = FrameStabilityTargetType::SCREEN;

    std::vector<FrameStabilityTarget> registeredTargets;

    for (uint64_t i = 1; i <= MAX_FRAME_STABILITY_CONNECTION_NUM; i++) {
        target.id = i;
        int32_t result = RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(
            TEST_PID, target, config);
        EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::SUCCESS));
        registeredTargets.push_back(target);
    }

    target.id = MAX_FRAME_STABILITY_CONNECTION_NUM + 1;
    int32_t result = RSFrameStabilityManager::GetInstance().StartFrameStabilityCollection(
        TEST_PID, target, config);
    EXPECT_EQ(result, static_cast<int32_t>(FrameStabilityErrorCode::CONNECTIONS_OCCUPIED));

    for (const auto& regTarget : registeredTargets) {
        bool dummyResult = false;
        RSFrameStabilityManager::GetInstance().GetFrameStabilityResult(TEST_PID, regTarget, dummyResult);
    }
}
} // namespace OHOS::Rosen