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
#include "screen_manager/rs_screen_property.h"
#include "surface_buffer_impl.h"
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
    sptr<RSScreenProperty> property = new RSScreenProperty();
    rsRenderComposer_ = std::make_shared<RSRenderComposer>(output, property);
}
void RsRenderComposerAgentTest::TearDownTestCase()
{
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

    agent->ComposerProcess(nullptr);
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
    auto nullAgent = std::make_shared<RSRenderComposerAgent>(nullptr);
    nullAgent->OnScreenConnected(nullptr, nullptr);
    nullAgent->OnScreenDisconnected();
    EXPECT_EQ(nullAgent->ClearFrameBuffers(true), GSERROR_INVALID_ARGUMENTS);
    nullAgent->OnScreenVBlankIdleCallback(static_cast<ScreenId>(0), 123456ULL);

    // Non-null composer: methods should forward to composer and execute without crash
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_TRUE(agent->rsRenderComposer_ != nullptr);
    // OnScreenConnected/Disconnected
    auto out = std::make_shared<HdiOutput>(5u);
    out->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    agent->OnScreenConnected(out, property);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    agent->OnScreenDisconnected();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // ClearFrameBuffers should return a GSError (may be OK or error depending on environment), ensure no crash
    EXPECT_EQ(agent->ClearFrameBuffers(true), GSERROR_OK);

    // VBlank callback forwarding
    agent->OnScreenVBlankIdleCallback(static_cast<ScreenId>(5u), 98765ULL);
}

/**
 * Function: HwcRestored_And_HwcDead_NullAndNonNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call OnHwcRestored/OnHwcDead on null agent (early return)
 *                  2. call OnHwcRestored/OnHwcDead on non-null agent (forward tasks)
 */
HWTEST_F(RsRenderComposerAgentTest, HwcRestored_And_HwcDead_NullAndNonNull, TestSize.Level1)
{
    // Null composer
    auto nullAgent = std::make_shared<RSRenderComposerAgent>(nullptr);
    nullAgent->OnHwcRestored(nullptr, nullptr);
    nullAgent->OnHwcDead();

    // Non-null composer
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_TRUE(agent->rsRenderComposer_ != nullptr);
    auto out = std::make_shared<HdiOutput>(6u);
    out->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    agent->OnHwcRestored(out, property);
    agent->OnHwcDead();
    // allow async tasks to run
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: Call_Interface_With_Null_Composer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with not null RSRenderComposer
 *                  2. call Interface
 *                  3. check result
 */
HWTEST_F(RsRenderComposerAgentTest, Call_Interface_With_Null_Composer, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposer> nullComposer = nullptr;
    std::shared_ptr<RSRenderComposerAgent> agent = std::make_shared<RSRenderComposerAgent>(nullComposer);
    EXPECT_TRUE(agent->rsRenderComposer_ == nullptr);
    agent->CleanLayerBufferBySurfaceId(0u);

    std::string dumpString = "";
    agent->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());

    agent->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());

    std::string layerName = "test";
    agent->FpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());

    agent->ClearFpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());

    agent->HitchsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());

    std::unordered_set<uint64_t> bufferIds;
    agent->ClearRedrawGPUCompositionCache(bufferIds);
    EXPECT_TRUE(bufferIds.empty());
}

/**
 * Function: SetComposerToRenderConnection_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. null-composer early return
 *                  2. non-null composer forwards task with nullptr connection (no crash)
 */
HWTEST_F(RsRenderComposerAgentTest, SetComposerToRenderConnection_Branches, TestSize.Level1)
{
    // Null composer branch
    auto nullAgent = std::make_shared<RSRenderComposerAgent>(nullptr);
    sptr<IRSComposerToRenderConnection> nullConn;
    nullAgent->SetComposerToRenderConnection(nullConn);

    // Non-null composer branch
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);
    agent->SetComposerToRenderConnection(nullConn);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: PreAllocProtectedFrameBuffers_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call with nullptr buffer → no submit
 *                  2. call with non-null buffer → submit path executes without crash
 */
HWTEST_F(RsRenderComposerAgentTest, PreAllocProtectedFrameBuffers_Branches, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);
    // nullptr branch
    sptr<SurfaceBuffer> nullBuf;
    agent->PreAllocProtectedFrameBuffers(nullBuf);
    // non-null branch
    sptr<SurfaceBuffer> buf = new SurfaceBufferImpl();
    agent->PreAllocProtectedFrameBuffers(buf);
    agent->rsRenderComposer_ = nullptr;
    agent->PreAllocProtectedFrameBuffers(buf);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: SetScreenBacklight_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. null composer → early return
 *                  2. non-null composer → forward task
 */
HWTEST_F(RsRenderComposerAgentTest, SetScreenBacklight_Branches, TestSize.Level1)
{
    auto nullAgent = std::make_shared<RSRenderComposerAgent>(nullptr);
    nullAgent->SetScreenBacklight(10);

    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);
    agent->SetScreenBacklight(20);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: MiscForwarding_WithNonNull_Composer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call CleanLayerBufferBySurfaceId / ClearRedrawGPUCompositionCache
 *                  2. call SurfaceDump/GetRefreshInfoToSP/FpsDump/ClearFpsDump/HitchsDump/RefreshRateCounts/ClearRefreshRateCounts
 */
HWTEST_F(RsRenderComposerAgentTest, MiscForwarding_WithNonNull_Composer, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);
    agent->CleanLayerBufferBySurfaceId(0u);
    agent->ClearRedrawGPUCompositionCache({});

    std::string dumpString;
    std::string layerArg = "layer";
    agent->SurfaceDump(dumpString);
    agent->GetRefreshInfoToSP(dumpString, static_cast<NodeId>(0));
    agent->FpsDump(dumpString, layerArg);
    agent->ClearFpsDump(dumpString, layerArg);
    agent->HitchsDump(dumpString, layerArg);
    agent->RefreshRateCounts(dumpString);
    agent->ClearRefreshRateCounts(dumpString);

    agent->rsRenderComposer_ = nullptr;
    agent->SurfaceDump(dumpString);
    agent->GetRefreshInfoToSP(dumpString, static_cast<NodeId>(0));
    agent->FpsDump(dumpString, layerArg);
    agent->ClearFpsDump(dumpString, layerArg);
    agent->HitchsDump(dumpString, layerArg);
    agent->RefreshRateCounts(dumpString);
    agent->ClearRefreshRateCounts(dumpString);
}
} // namespace Rosen
} // namespace OHOS
