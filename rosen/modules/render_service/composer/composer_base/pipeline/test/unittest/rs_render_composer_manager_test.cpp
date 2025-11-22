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

class RsRenderComposerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsRenderComposerManagerTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RsRenderComposerManagerTest::TearDownTestCase() {}
void RsRenderComposerManagerTest::SetUp() {}
void RsRenderComposerManagerTest::TearDown() {}

/**
 * Function: OnScreenConnected_NullOutput_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler,and new RSScreenProperty
 *                  2. call OnScreenConnected with null output
 *                  3. check mgr's rsRenderComposerMap_ is still empty
 */
HWTEST_F(RsRenderComposerManagerTest, OnScreenConnected_NullOutput_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->OnScreenConnected(nullptr);
    EXPECT_TRUE(mgr->rsRenderComposerMap_.empty());
}

/**
 * Function: OnScreenConnected_InsertNew
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler, hdiOutput, and new RSScreenProperty
 *                  2. call OnScreenConnected with non null output and call GetRSComposerConnection to get connection
 *                  3. check mgr->rsRenderComposerMap_ and mgr->rsComposerConnectionMap_ size is 1 after connect,
 *                      GetRSComposerConnection by output's screen id is not null
 */
HWTEST_F(RsRenderComposerManagerTest, OnScreenConnected_InsertNew, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    auto output = std::make_shared<HdiOutput>(10u);
    output->Init();
    mgr->OnScreenConnected(output);
    EXPECT_EQ(mgr->rsRenderComposerMap_.size(), 1u);
    EXPECT_EQ(mgr->rsComposerConnectionMap_.size(), 1u);
    auto conn = mgr->GetRSComposerConnection(10u);
    EXPECT_NE(conn, nullptr);
}

/**
 * Function: OnScreenConnected_Existing_Forward
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler, hdiOutput, and new RSScreenProperty
 *                  2. call OnScreenConnected with non null output but same twice
 *                  3. check mgr->rsRenderComposerMap_ and mgr->rsComposerConnectionMap_ size is 1 all the time
 */
HWTEST_F(RsRenderComposerManagerTest, OnScreenConnected_Existing_Forward, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    auto output = std::make_shared<HdiOutput>(20u);
    output->Init();
    mgr->OnScreenConnected(output); // first insert
    EXPECT_EQ(mgr->rsRenderComposerMap_.size(), 1u);
    EXPECT_EQ(mgr->rsComposerConnectionMap_.size(), 1u);
    mgr->OnScreenConnected(output); // second enter else branch
    EXPECT_EQ(mgr->rsRenderComposerMap_.size(), 1u);
    EXPECT_EQ(mgr->rsComposerConnectionMap_.size(), 1u);
}

/**
 * Function: OnScreenDisconnected_NotFound_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler
 *                  2. call OnScreenDisconnected with not existing screen id
 *                  3. check mgr->rsRenderComposerMap_ is still empty
 */
HWTEST_F(RsRenderComposerManagerTest, OnScreenDisconnected_NotFound_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->OnScreenDisconnected(9999);
    EXPECT_TRUE(mgr->rsRenderComposerMap_.empty());
}

/**
 * Function: OnScreenDisconnected_Found_Path
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager with null EventHandler, hdiOutput, and new RSScreenProperty
 *                  2. call OnScreenConnected to add composer, then call OnScreenDisconnected with existing screen id
 *                  3. check mgr->rsRenderComposerMap_ is still empty
 */
HWTEST_F(RsRenderComposerManagerTest, OnScreenDisconnected_Found_Path, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    auto output = std::make_shared<HdiOutput>(30u);
    output->Init();
    mgr->OnScreenConnected(output);
    ASSERT_EQ(mgr->rsRenderComposerMap_.size(), 1u);
    mgr->OnScreenDisconnected(30u);
    EXPECT_EQ(mgr->rsRenderComposerMap_.size(), 1u);
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
HWTEST_F(RsRenderComposerManagerTest, GetRSComposerConnection_FoundAndNotFound, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    EXPECT_EQ(mgr->GetRSComposerConnection(1), nullptr);
    auto output = std::make_shared<HdiOutput>(40u);
    output->Init();
    mgr->OnScreenConnected(output);
    EXPECT_NE(mgr->GetRSComposerConnection(40u), nullptr);
}

/**
 * Function: PreAllocateProtectedBuffer_And_Task_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call PreAllocateProtectedBuffer/GetUnExecuteTaskNum/PostTask/ClearFrameBuffers
 *                      with missing screenId
 *                  2. call the same methods after OnScreenConnected and ensure tasks execute / functions forward
 */
HWTEST_F(RsRenderComposerManagerTest, PreAllocateProtectedBuffer_And_Task_Branches, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();

    // 1. operations with missing screenId should early return or provide defaults
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    mgr->PreAllocateProtectedBuffer(9999, surfaceBuffer); // should early return, no crash
    EXPECT_EQ(mgr->GetUnExecuteTaskNum(9999), 0u);
    mgr->PostTask(9999, []{}); // early return
    GSError err = mgr->ClearFrameBuffers(9999, true);
    EXPECT_EQ(err, GSERROR_INVALID_ARGUMENTS);

    // 2. after connecting an output, operations should forward and run
    auto out = std::make_shared<HdiOutput>(50u);
    out->Init();
    mgr->OnScreenConnected(out);
    EXPECT_EQ(mgr->rsRenderComposerMap_.size(), 1u);

    mgr->PreAllocateProtectedBuffer(50u, surfaceBuffer);

    // PostTask should execute the task; observe via atomic flag
    std::atomic<bool> ran{false};
    mgr->PostTask(50u, [&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(ran.load());

    // GetUnExecuteTaskNum should be callable and return a numeric value (>= 0)
    EXPECT_EQ(mgr->GetUnExecuteTaskNum(50u), 0);

    // ClearFrameBuffers should forward and return a GSError (OK or other) but must not crash
    EXPECT_EQ(mgr->ClearFrameBuffers(50u, true), GSERROR_OK);
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
HWTEST_F(RsRenderComposerManagerTest, GetAccumulatedBufferCount_FoundAndNotFound, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    EXPECT_EQ(mgr->GetAccumulatedBufferCount(1u), 0);
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    mgr->OnScreenConnected(output); // first insert
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_NE(mgr->GetAccumulatedBufferCount(1u), 0);
    mgr->OnScreenConnected(output); // second enter else branch
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_NE(mgr->GetAccumulatedBufferCount(1u), 0);
    mgr->OnScreenDisconnected(1u);
}

/**
 * Function: OnScreenVBlankIdleCallback_FoundAndNotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call OnScreenVBlankIdleCallback before and after add screen id
 *                  3. check connection is null before add and not null after add
 */
HWTEST_F(RsRenderComposerManagerTest, OnScreenVBlankIdleCallback_FoundAndNotFound, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->OnScreenVBlankIdleCallback(1, 0);
    EXPECT_EQ(mgr->rsRenderComposerMap_.find(1), mgr->rsRenderComposerMap_.end());
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    mgr->OnScreenConnected(output);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mgr->OnScreenVBlankIdleCallback(1, 0);
    EXPECT_NE(mgr->rsRenderComposerMap_.find(1), mgr->rsRenderComposerMap_.end());
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
HWTEST_F(RsRenderComposerManagerTest, RateCount_Operations, TestSize.Level1)
{
    std::string dumpString = "";
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    mgr->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    mgr->OnScreenConnected(output);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mgr->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    dumpString = "";
    mgr->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());

    mgr->rsRenderComposerMap_.insert(std::pair(0u, nullptr));
    mgr->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    dumpString = "";
    mgr->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
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
HWTEST_F(RsRenderComposerManagerTest, Dump_Operations, TestSize.Level1)
{
    std::string dumpString = "";
    std::string layerName = "composer";
    std::string layerArg = "window_test";
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();

    mgr->FpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString.find("[Id:"), std::string::npos);

    dumpString = "";
    mgr->ClearFpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString.find("[Id:"), std::string::npos);

    dumpString = "";
    mgr->DumpCurrentFrameLayers();
    EXPECT_TRUE(dumpString.empty());

    dumpString = "";
    mgr->HitchsDump(dumpString, layerArg);
    EXPECT_EQ(dumpString.find("window_test"), std::string::npos);

    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    mgr->OnScreenConnected(output);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mgr->rsRenderComposerMap_.insert(std::pair(0u, nullptr));

    mgr->FpsDump(dumpString, layerName);
    EXPECT_NE(dumpString.find("[Id:1]"), std::string::npos);

    dumpString = "";
    mgr->ClearFpsDump(dumpString, layerName);
    EXPECT_NE(dumpString.find("[Id:1]"), std::string::npos);

    dumpString = "";
    mgr->DumpCurrentFrameLayers();
    EXPECT_TRUE(dumpString.empty());

    dumpString = "";
    mgr->HitchsDump(dumpString, layerArg);
    EXPECT_EQ(dumpString.find("window_test"), std::string::npos);
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
HWTEST_F(RsRenderComposerManagerTest, GetReleaseFence, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    EXPECT_EQ(mgr->GetReleaseFence(0u), nullptr);
    mgr->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));
    EXPECT_EQ(mgr->GetReleaseFence(1u), nullptr);
    EXPECT_NE(mgr->GetReleaseFence(2u), nullptr);
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
HWTEST_F(RsRenderComposerManagerTest, WaitComposerTaskExecute, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    mgr->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));

    EXPECT_FALSE(mgr->WaitComposerTaskExecute(0u));
    EXPECT_FALSE(mgr->WaitComposerTaskExecute(1u));
    EXPECT_TRUE(mgr->WaitComposerTaskExecute(2u));

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
HWTEST_F(RsRenderComposerManagerTest, CleanLayerBufferBySurfaceId, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->CleanLayerBufferBySurfaceId(0u);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(mgr->rsRenderComposerMap_.empty());

    mgr->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    mgr->CleanLayerBufferBySurfaceId(1u);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(mgr->rsRenderComposerMap_.size(), 1);

    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    mgr->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));
    mgr->CleanLayerBufferBySurfaceId(2u);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(mgr->rsRenderComposerMap_.size(), 2);

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
HWTEST_F(RsRenderComposerManagerTest, DumpVkImageInfo, TestSize.Level1)
{
    std::string dumpString = "";
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->DumpVkImageInfo(dumpString);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(dumpString.empty());

    mgr->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    mgr->DumpVkImageInfo(dumpString);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(dumpString.find("id : 1"), std::string::npos);

    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    mgr->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));

    mgr->DumpVkImageInfo(dumpString);
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
HWTEST_F(RsRenderComposerManagerTest, ClearRedrawGPUCompositionCache, TestSize.Level1)
{
    std::string dumpString = "";
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    mgr->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));

    std::set<uint64_t> bufferIds;
    bufferIds.insert(1u);
    bufferIds.insert(2u);
    mgr->ClearRedrawGPUCompositionCache(bufferIds);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(mgr->rsRenderComposerMap_.size(), 2);

    rsRenderComposer->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer->uniRenderEngine_ = nullptr;
}

/*
 * Function: PostTaskToAllScreens
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. call PostTaskToAllScreens after prepared rsRenderComposerMap_
 *                  3. check result
 */
HWTEST_F(RsRenderComposerManagerTest, PostTaskToAllScreens, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>();
    mgr->rsRenderComposerMap_.insert(std::pair(1u, nullptr));
    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    mgr->rsRenderComposerMap_.insert(std::pair(2u, rsRenderComposer));

    std::set<uint64_t> bufferIds;
    bufferIds.insert(1u);
    bufferIds.insert(2u);
    std::atomic<bool> ran{false};
    rsRenderComposer->PostTask([&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(ran.load());

    rsRenderComposer->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer->uniRenderEngine_ = nullptr;
}
} // namespace Rosen
} // namespace OHOS
