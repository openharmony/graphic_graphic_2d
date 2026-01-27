/**
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
#include <thread>
#include "rs_render_composer_agent.h"
#include "hdi_output.h"
#include "rs_layer_transaction_data.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderComposerAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderComposerAgentTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RSRenderComposerAgentTest::TearDownTestCase() {}
void RSRenderComposerAgentTest::SetUp() {}
void RSRenderComposerAgentTest::TearDown() {}

/**
 * Function: Call_Interface_With_Null_Composer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with not null RSRenderComposer
 *                  2. call Interface
 *                  3. check result
 */
HWTEST_F(RSRenderComposerAgentTest, Call_Interface_With_Null_Composer, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }

    std::shared_ptr<RSRenderComposer> nullComposer = nullptr;
    std::shared_ptr<RSRenderComposerAgent> composerAgent = std::make_shared<RSRenderComposerAgent>(nullComposer);
    EXPECT_TRUE(composerAgent->rsRenderComposer_ == nullptr);
    EXPECT_EQ(composerAgent->GetAccumulatedBufferCount(), 0);
    composerAgent->CleanLayerBufferBySurfaceId(0u);

    std::string dumpString = "";
    composerAgent->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());

    composerAgent->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());

    std::string layerName = "test";
    composerAgent->FpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());

    composerAgent->ClearFpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());

    composerAgent->DumpCurrentFrameLayers();
    EXPECT_TRUE(dumpString.empty());

    composerAgent->HitchsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());

    composerAgent->DumpVkImageInfo(dumpString);
    EXPECT_TRUE(dumpString.empty());

    std::set<uint64_t> bufferIds;
    composerAgent->ClearRedrawGPUCompositionCache(bufferIds);
    EXPECT_TRUE(bufferIds.empty());

    EXPECT_EQ(composerAgent->GetReleaseFence(), nullptr);
    EXPECT_FALSE(composerAgent->WaitComposerTaskExecute());
}

/**
 * Function: ForwardingMethods_NullAndNonNullBranches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. verify composerAgent with null composer returns/doesn't crash for forwarding methods
 *                  2. verify composerAgent with non-null composer forwards calls and tasks execute
 */
HWTEST_F(RSRenderComposerAgentTest, ForwardingMethods_NullAndNonNullBranches, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }

    // Null composer: methods should early return or provide default values
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    auto nullAgent = std::make_shared<RSRenderComposerAgent>(nullptr);
    nullAgent->OnScreenConnected(nullptr);
    nullAgent->OnScreenDisconnected();
    nullAgent->PreAllocateProtectedBuffer(surfaceBuffer);
    EXPECT_EQ(nullAgent->GetUnExecuteTaskNum(), 0u);
    nullAgent->PostTask([]{});
    EXPECT_EQ(nullAgent->ClearFrameBuffers(true), GSERROR_INVALID_ARGUMENTS);
    nullAgent->OnScreenVBlankIdleCallback(123456ULL);
    nullAgent->HandlePowerStatus(POWER_STATUS_ON);

    // Non-null composer: methods should forward to composer and execute without crash
    auto composerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
    ASSERT_TRUE(composerAgent->rsRenderComposer_ != nullptr);
    composerAgent->PreAllocateProtectedBuffer(surfaceBuffer);
    // OnScreenConnected/Disconnected
    auto out = std::make_shared<HdiOutput>(5u);
    out->Init();
    composerAgent->OnScreenConnected(out);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(composerAgent->rsRenderComposer_->hdiOutput_->GetScreenId(), 5u);
    composerAgent->OnScreenDisconnected();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(composerAgent->rsRenderComposer_->hdiOutput_, nullptr);

    composerAgent->PreAllocateProtectedBuffer(surfaceBuffer);

    // PostTask should schedule task on composer thread -> use an atomic flag to observe execution
    std::atomic<bool> ran{false};
    composerAgent->PostTask([&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(ran.load());

    // GetUnExecuteTaskNum should be callable
    EXPECT_EQ(composerAgent->GetUnExecuteTaskNum(), 0);
    // ClearFrameBuffers should return a GSError (may be OK or error depending on environment), ensure no crash
    EXPECT_EQ(composerAgent->ClearFrameBuffers(true), GSERROR_INVALID_ARGUMENTS);

    // HandlePowerStatus callback forwarding
    composerAgent->HandlePowerStatus(POWER_STATUS_ON);

    // VBlank callback forwarding
    composerAgent->OnScreenVBlankIdleCallback(98765ULL);
}

/**
 * Function: ComposerProcess_NonNullTransactionData_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with not null RSRenderComposer
 *                  2. call ComposerProcess with not null transactionData
 *                  3. check rsRenderComposer is not null
 */
HWTEST_F(RSRenderComposerAgentTest, ComposerProcess_NonNullTransactionData_NoCrash, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
    std::shared_ptr<RSRenderComposerAgent> composerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
    ASSERT_TRUE(composerAgent->rsRenderComposer_ != nullptr);

    composerAgent->ComposerProcess(nullptr);
    auto tx = std::make_shared<RSLayerTransactionData>();
    ASSERT_NO_FATAL_FAILURE(composerAgent->ComposerProcess(tx));
}

/**
 * Function: ComposerProcess_NullTransactionData_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with null RSRenderComposer
 *                  2. call ComposerProcess with null transactionData
 *                  3. check rsRenderComposer is still null
 */
HWTEST_F(RSRenderComposerAgentTest, ComposerProcess_NullTransactionData_NoCrash, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposer> rsRenderComposer = nullptr;
    std::shared_ptr<RSRenderComposerAgent> composerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
    composerAgent->ComposerProcess(nullptr);
    ASSERT_TRUE(composerAgent->rsRenderComposer_ == nullptr);
}
} // namespace Rosen
} // namespace OHOS
