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

class RsRenderComposerAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSRenderComposer> rsRenderComposer_ = nullptr;
};

void RsRenderComposerAgentTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    rsRenderComposer_ = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer_->runner_) {
        rsRenderComposer_->runner_->Run();
    }
}
void RsRenderComposerAgentTest::TearDownTestCase()
{
    rsRenderComposer_->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer_->uniRenderEngine_ = nullptr;
}
void RsRenderComposerAgentTest::SetUp() {}
void RsRenderComposerAgentTest::TearDown() {}


/**
 * Function: ComposerProcess_NullTransactionData_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with null RSRenderComposer
 *                  2. call ComposerProcess with null transactionData
 *                  3. check rsRenderComposer_ is still null
 */
HWTEST_F(RsRenderComposerAgentTest, ComposerProcess_NullTransactionData_NoCrash, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposer> nullComposer = nullptr;
    std::shared_ptr<RSRenderComposerAgent> agent = std::make_shared<RSRenderComposerAgent>(nullComposer);
    agent->ComposerProcess(nullptr);
    ASSERT_TRUE(agent->rsRenderComposer_ == nullptr);
}

/**
 * Function: ComposerProcess_NonNullTransactionData_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with not null RSRenderComposer
 *                  2. call ComposerProcess with not null transactionData
 *                  3. check rsRenderComposer_ is not null
 */
HWTEST_F(RsRenderComposerAgentTest, ComposerProcess_NonNullTransactionData_NoCrash, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerAgent> agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_TRUE(agent->rsRenderComposer_ != nullptr);

    auto tx = std::make_shared<RSLayerTransactionData>();
    ASSERT_NO_FATAL_FAILURE(agent->ComposerProcess(tx));
}

/**
 * Function: ForwardingMethods_NullAndNonNullBranches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. verify agent with null composer returns/doesn't crash for forwarding methods
 *                  2. verify agent with non-null composer forwards calls and tasks execute
 */
HWTEST_F(RsRenderComposerAgentTest, ForwardingMethods_NullAndNonNullBranches, TestSize.Level1)
{
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

    // Non-null composer: methods should forward to composer and execute without crash
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_TRUE(agent->rsRenderComposer_ != nullptr);
    nullAgent->PreAllocateProtectedBuffer(surfaceBuffer);
    // OnScreenConnected/Disconnected
    auto out = std::make_shared<HdiOutput>(5u);
    out->Init();
    agent->OnScreenConnected(out);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(agent->rsRenderComposer_->hdiOutput_->GetScreenId(), 5u);
    agent->OnScreenDisconnected();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(agent->rsRenderComposer_->hdiOutput_, nullptr);

    agent->PreAllocateProtectedBuffer(surfaceBuffer);

    // PostTask should schedule task on composer thread -> use an atomic flag to observe execution
    std::atomic<bool> ran{false};
    agent->PostTask([&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(ran.load());

    // GetUnExecuteTaskNum should be callable
    EXPECT_EQ(agent->GetUnExecuteTaskNum(), 0);
    // ClearFrameBuffers should return a GSError (may be OK or error depending on environment), ensure no crash
    EXPECT_EQ(agent->ClearFrameBuffers(true), GSERROR_INVALID_ARGUMENTS);

    // VBlank callback forwarding
    agent->OnScreenVBlankIdleCallback(98765ULL);
}

/**
 * Function: Call_Interface_With_Null_Composer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with null RSRenderComposer
 *                  2. call Interface
 *                  3. check result
 */
HWTEST_F(RsRenderComposerAgentTest, Call_Interface_With_Null_Composer, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposer> nullComposer = nullptr;
    std::shared_ptr<RSRenderComposerAgent> agent = std::make_shared<RSRenderComposerAgent>(nullComposer);
    EXPECT_TRUE(agent->rsRenderComposer_ == nullptr);
    EXPECT_EQ(agent->GetAccumulatedBufferCount(), 0);
    agent->CleanLayerBufferBySurfaceId(0);

    std::string dumpString;
    agent->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());

    agent->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());

    std::string layerName = "test";
    agent->FpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());

    agent->ClearFpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());

    agent->DumpCurrentFrameLayers();
    EXPECT_TRUE(dumpString.empty());

    agent->HitchsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());

    agent->DumpVkImageInfo(dumpString);
    EXPECT_TRUE(dumpString.empty());

    std::set<uint64_t> bufferIds;
    agent->ClearRedrawGPUCompositionCache(bufferIds);
    EXPECT_TRUE(bufferIds.empty());

    EXPECT_EQ(agent->GetReleaseFence(), nullptr);
    EXPECT_FALSE(agent->WaitComposerTaskExecute());
}
} // namespace Rosen
} // namespace OHOS
