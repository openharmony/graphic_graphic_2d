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

#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <chrono>
#include "rs_render_composer_manager.h"
#include "hdi_output.h"
#include "event_handler.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderComposerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderComposerManagerTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RSRenderComposerManagerTest::TearDownTestCase() {}
void RSRenderComposerManagerTest::SetUp() {}
void RSRenderComposerManagerTest::TearDown() {}

/**
 * Function: SetScreenPowerOnChanged
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call SetScreenPowerOnChanged with different composer
 *                  3. check result
 */
HWTEST_F(RSRenderComposerManagerTest, SetScreenPowerOnChanged, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    // cannot find in map
    manager->SetScreenPowerOnChanged(0u, false);

    // composer is nullptr
    manager->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    manager->SetScreenPowerOnChanged(1u, false);

    // normal composer
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    manager->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));
    EXPECT_GT(manager->rsRenderComposerMap_.size(), 0);
    manager->SetScreenPowerOnChanged(2u, false);

    // composer without output
    auto rsRenderComposer2 = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer2->runner_) {
        rsRenderComposer2->runner_->Run();
    }
    rsRenderComposer2->hdiOutput_ = nullptr;
    manager->rsRenderComposerMap_.insert(std::pair(3u, rsRenderComposer2));
    EXPECT_GT(manager->rsRenderComposerMap_.size(), 0);
    manager->SetScreenPowerOnChanged(3u, false);

    // clear resources
    for (auto& [id, composer] : manager->rsRenderComposerMap_) {
        if (composer) {
            composer->frameBufferSurfaceOhosMap_.clear();
            composer->uniRenderEngine_ = nullptr;
        }
    }
}

/**
 * Function: Dump_Operations
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call Dump Function
 *                  3. check dumpString
 */
HWTEST_F(RSRenderComposerManagerTest, Dump_Operations, TestSize.Level1)
{
    std::string dumpString = "";
    std::string layerName = "composer";
    std::string layerArg = "window_test";
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();

    manager->FpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString.find("[Id:"), std::string::npos);

    dumpString = "";
    manager->ClearFpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString.find("[Id:"), std::string::npos);

    dumpString = "";
    manager->DumpCurrentFrameLayers();
    EXPECT_TRUE(dumpString.empty());

    dumpString = "";
    manager->HitchsDump(dumpString, layerArg);
    EXPECT_EQ(dumpString.find("window_test"), std::string::npos);

    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    manager->OnScreenConnected(output);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    manager->rsRenderComposerMap_.insert(std::pair(0u, nullptr));

    manager->FpsDump(dumpString, layerName);
    EXPECT_NE(dumpString.find("[Id:1]"), std::string::npos);

    dumpString = "";
    manager->ClearFpsDump(dumpString, layerName);
    EXPECT_NE(dumpString.find("[Id:1]"), std::string::npos);

    dumpString = "";
    manager->DumpCurrentFrameLayers();
    EXPECT_TRUE(dumpString.empty());

    dumpString = "";
    manager->HitchsDump(dumpString, layerArg);
    EXPECT_EQ(dumpString.find("window_test"), std::string::npos);
}

#ifdef RS_ENABLE_VK
/**
 * Function: PreAllocateProtectedBuffer_And_Task_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call PreAllocateProtectedBuffer/GetUnExecuteTaskNum/PostTask/ClearFrameBuffers
 *                      with missing screenId
 *                  2. call the same methods after OnScreenConnected and ensure tasks execute / functions forward
 */
HWTEST_F(RSRenderComposerManagerTest, PreAllocateProtectedBuffer_And_Task_Branches, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();

    // 1. operations with missing screenId should early return or provide defaults
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    manager->PreAllocateProtectedBuffer(9999, surfaceBuffer); // should early return, no crash
    EXPECT_EQ(manager->GetUnExecuteTaskNum(9999), 0u);
    manager->PostTask(9999, []{}); // early return
    GSError err = manager->ClearFrameBuffers(9999, true);
    EXPECT_EQ(err, GSERROR_INVALID_ARGUMENTS);

    // 2. after connecting an output, operations should forward and run
    auto out = std::make_shared<HdiOutput>(50u);
    out->Init();
    manager->OnScreenConnected(out);
    EXPECT_EQ(manager->rsRenderComposerMap_.size(), 1u);

    manager->PreAllocateProtectedBuffer(50u, surfaceBuffer);

    // PostTask should execute the task; observe via atomic flag
    std::atomic<bool> ran{false};
    manager->PostTask(50u, [&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(ran.load());

    // GetUnExecuteTaskNum should be callable and return a numeric value (>= 0)
    EXPECT_EQ(manager->GetUnExecuteTaskNum(50u), 0);

    // ClearFrameBuffers should forward and return a GSError (OK or other) but must not crash
    EXPECT_EQ(manager->ClearFrameBuffers(50u, true), GSERROR_OK);
}
#endif

/**
 * Function: OnScreenVBlankIdleCallback_FoundAndNotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call OnScreenVBlankIdleCallback before and after add screen id
 *                  3. check connection is null before add and not null after add
 */
HWTEST_F(RSRenderComposerManagerTest, OnScreenVBlankIdleCallback_FoundAndNotFound, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->OnScreenVBlankIdleCallback(1, 0);
    EXPECT_EQ(manager->rsRenderComposerMap_.find(1), manager->rsRenderComposerMap_.end());
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    manager->OnScreenConnected(output);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    manager->OnScreenVBlankIdleCallback(1, 0);
    EXPECT_NE(manager->rsRenderComposerMap_.find(1), manager->rsRenderComposerMap_.end());
}

/**
 * Function: GetRSComposerConnection_FoundAndNotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler, hdiOutput, and new RSScreenProperty
 *                  2. call GetRSComposerConnection before and after add screen id
 *                  3. check connection is null before add and not null after add
 */
HWTEST_F(RSRenderComposerManagerTest, GetRSComposerConnection_FoundAndNotFound, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    EXPECT_EQ(manager->GetRSComposerConnection(1), nullptr);
    auto output = std::make_shared<HdiOutput>(40u);
    output->Init();
    manager->OnScreenConnected(output);
    EXPECT_NE(manager->GetRSComposerConnection(40u), nullptr);
}

/**
 * Function: RateCount_Operations
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call RefreshRateCounts and ClearRefreshRateCounts
 *                  3. check dumpString
 */
HWTEST_F(RSRenderComposerManagerTest, RateCount_Operations, TestSize.Level1)
{
    std::string dumpString = "";
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    manager->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    manager->OnScreenConnected(output);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    manager->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    dumpString = "";
    manager->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());

    manager->rsRenderComposerMap_.insert(std::pair(0u, nullptr));
    manager->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    dumpString = "";
    manager->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
}

/**
 * Function: GetAccumulatedBufferCount_FoundAndNotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager before and after connecte
 *                  2. call GetAccumulatedBufferCount before and after add screen id
 *                  3. check connection is null before add and not null after add
 */
HWTEST_F(RSRenderComposerManagerTest, GetAccumulatedBufferCount_FoundAndNotFound, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    EXPECT_EQ(manager->GetAccumulatedBufferCount(1u), 0);
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    manager->OnScreenConnected(output); // first insert
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(manager->GetAccumulatedBufferCount(1u), 0);
    manager->OnScreenConnected(output); // second enter else branch
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(manager->GetAccumulatedBufferCount(1u), 0);
    manager->OnScreenDisconnected(1u);
}

/**
 * Function: PostTaskWithInnerDelay
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: call PostTaskWithInnerDelay
 */
HWTEST_F(RSRenderComposerManagerTest, PostTaskWithInnerDelay, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    
    // composer is nullptr
    manager->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    std::atomic<bool> ran{false};
    manager->PostTaskWithInnerDelay(1u, [&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_FALSE(ran.load());

    // normal composer
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    manager->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));
    manager->PostTaskWithInnerDelay(2u, [&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(ran.load());
}

/**
 * Function: HandlePowerStatus_FoundAndNotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call HandlePowerStatus before and after add screen id
 *                  3. check connection is null before add and not null after add
 */
HWTEST_F(RSRenderComposerManagerTest, HandlePowerStatus_FoundAndNotFound, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->HandlePowerStatus(1, POWER_STATUS_ON);
    EXPECT_EQ(manager->rsRenderComposerMap_.find(1), manager->rsRenderComposerMap_.end());
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    manager->OnScreenConnected(output);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    manager->HandlePowerStatus(1, POWER_STATUS_ON);
    EXPECT_NE(manager->rsRenderComposerMap_.find(1), manager->rsRenderComposerMap_.end());
}

/**
 * Function: OnScreenDisconnected_Found_Path
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler, hdiOutput, and new RSScreenProperty
 *                  2. call OnScreenConnected to add composer, then call OnScreenDisconnected with existing screen id
 *                  3. check manager->rsRenderComposerMap_ is still empty
 */
HWTEST_F(RSRenderComposerManagerTest, OnScreenDisconnected_Found_Path, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    auto output = std::make_shared<HdiOutput>(30u);
    output->Init();
    manager->OnScreenConnected(output);
    ASSERT_EQ(manager->rsRenderComposerMap_.size(), 1u);
    manager->OnScreenDisconnected(30u);
    EXPECT_EQ(manager->rsRenderComposerMap_.size(), 1u);
}

/**
 * Function: GetReleaseFence
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call GetReleaseFence after prepared rsRenderComposerMap_
 *                  3. check result
 */
HWTEST_F(RSRenderComposerManagerTest, GetReleaseFence, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    EXPECT_TRUE(manager->GetReleaseFence(0u)->Get() == -1);
    manager->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));
    EXPECT_TRUE(manager->GetReleaseFence(1u) == 0);
    EXPECT_NE(manager->GetReleaseFence(2u), nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    rsRenderComposer->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: WaitComposerTaskExecute
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call WaitComposerTaskExecute after prepared rsRenderComposerMap_
 *                  3. check result
 */
HWTEST_F(RSRenderComposerManagerTest, WaitComposerTaskExecute, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    manager->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));

    EXPECT_FALSE(manager->WaitComposerTaskExecute(0u));
    EXPECT_FALSE(manager->WaitComposerTaskExecute(1u));
    EXPECT_TRUE(manager->WaitComposerTaskExecute(2u));

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    rsRenderComposer->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: CleanLayerBufferBySurfaceId
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call CleanLayerBufferBySurfaceId after prepared rsRenderComposerMap_
 *                  3. check result
 */
HWTEST_F(RSRenderComposerManagerTest, CleanLayerBufferBySurfaceId, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->CleanLayerBufferBySurfaceId(0u);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(manager->rsRenderComposerMap_.empty());

    manager->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    manager->CleanLayerBufferBySurfaceId(1u);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(manager->rsRenderComposerMap_.size(), 1);

    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    manager->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));
    manager->CleanLayerBufferBySurfaceId(2u);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(manager->rsRenderComposerMap_.size(), 2);

    rsRenderComposer->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: DumpVkImageInfo
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call DumpVkImageInfo after prepared rsRenderComposerMap_
 *                  3. check dumpString
 */
HWTEST_F(RSRenderComposerManagerTest, DumpVkImageInfo, TestSize.Level1)
{
    std::string dumpString = "";
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->DumpVkImageInfo(dumpString);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(dumpString.empty());

    manager->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    manager->DumpVkImageInfo(dumpString);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(dumpString.find("id : 1"), std::string::npos);

    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    manager->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));

    manager->DumpVkImageInfo(dumpString);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_NE(dumpString.find("id : 2"), std::string::npos);

    rsRenderComposer->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: ClearRedrawGPUCompositionCache
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call ClearRedrawGPUCompositionCache after prepared rsRenderComposerMap_
 *                  3. check result
 */
HWTEST_F(RSRenderComposerManagerTest, ClearRedrawGPUCompositionCache, TestSize.Level1)
{
    std::string dumpString = "";
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    manager->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));

    std::set<uint64_t> bufferIds;
    bufferIds.insert(1u);
    bufferIds.insert(2u);
    manager->ClearRedrawGPUCompositionCache(bufferIds);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(manager->rsRenderComposerMap_.size(), 2);

    rsRenderComposer->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: PostTaskToAllScreens
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call PostTaskToAllScreens after prepared rsRenderComposerMap_
 *                  3. check result
 */
HWTEST_F(RSRenderComposerManagerTest, PostTaskToAllScreens, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    manager->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));

    std::atomic<bool> ran{false};
    manager->PostTaskToAllScreens([&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(ran.load());

    rsRenderComposer->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: OnScreenDisconnected_NotFound_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler
 *                  2. call OnScreenDisconnected with not existing screen id
 *                  3. check manager->rsRenderComposerMap_ is still empty
 */
HWTEST_F(RSRenderComposerManagerTest, OnScreenDisconnected_NotFound_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->OnScreenDisconnected(9999);
    EXPECT_TRUE(manager->rsRenderComposerMap_.empty());
}

/**
 * Function: OnScreenConnected_Existing_Forward
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler, hdiOutput, and new RSScreenProperty
 *                  2. call OnScreenConnected with non null output but same twice
 *                  3. check manager->rsRenderComposerMap_ and manager->rsComposerConnectionMap_ size is 1 all the time
 */
HWTEST_F(RSRenderComposerManagerTest, OnScreenConnected_Existing_Forward, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    auto output = std::make_shared<HdiOutput>(20u);
    output->Init();
    manager->OnScreenConnected(output); // first insert
    EXPECT_EQ(manager->rsRenderComposerMap_.size(), 1u);
    EXPECT_EQ(manager->rsComposerConnectionMap_.size(), 1u);
    manager->OnScreenConnected(output); // second enter else branch
    EXPECT_EQ(manager->rsRenderComposerMap_.size(), 1u);
    EXPECT_EQ(manager->rsComposerConnectionMap_.size(), 1u);
}

/**
 * Function: OnScreenConnected_InsertNew
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler, hdiOutput, and new RSScreenProperty
 *                  2. call OnScreenConnected with non null output and call GetRSComposerConnection to get connection
 *                  3. check manager->rsRenderComposerMap_ and manager->rsComposerConnectionMap_ size is 1 after connect,
 *                      GetRSComposerConnection by output's screen id is not null
 */
HWTEST_F(RSRenderComposerManagerTest, OnScreenConnected_InsertNew, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    auto output = std::make_shared<HdiOutput>(10u);
    output->Init();
    manager->OnScreenConnected(output);
    EXPECT_EQ(manager->rsRenderComposerMap_.size(), 1u);
    EXPECT_EQ(manager->rsComposerConnectionMap_.size(), 1u);
    auto conn = manager->GetRSComposerConnection(10u);
    EXPECT_NE(conn, nullptr);
}

/**
 * Function: OnScreenConnected_NullOutput_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler,and new RSScreenProperty
 *                  2. call OnScreenConnected with null output
 *                  3. check manager's rsRenderComposerMap_ is still empty
 */
HWTEST_F(RSRenderComposerManagerTest, OnScreenConnected_NullOutput_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> manager = std::make_shared<RSRenderComposerManager>();
    manager->OnScreenConnected(nullptr);
    EXPECT_TRUE(manager->rsRenderComposerMap_.empty());
}
} // namespace Rosen
} // namespace OHOS
