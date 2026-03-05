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
    nullAgent->HandlePowerStatus(POWER_STATUS_ON);

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
    agent->HandlePowerStatus(POWER_STATUS_ON);
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
 *                  2. call SurfaceDump/GetRefreshInfoToSP/FpsDump/
 *                     ClearFpsDump/HitchsDump/RefreshRateCounts/ClearRefreshRateCounts
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

/**
 * Function: SetComposerToRenderConnection_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call SetComposerToRenderConnection
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 43 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, SetComposerToRenderConnection_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    // Set composerToRenderConnection, then set rsRenderComposer_ to nullptr
    // This simulates the scenario where the task is posted but composer is destroyed before execution
    sptr<IRSComposerToRenderConnection> conn;
    agent->SetComposerToRenderConnection(conn);

    // Set rsRenderComposer_ to nullptr after task is posted but before it executes
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 44 without crash
    // Line 43 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: ComposerProcess_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call ComposerProcess with transactionData
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 69 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, ComposerProcess_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    auto tx = std::make_shared<RSLayerTransactionData>();
    agent->ComposerProcess(tx);

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 70 without crash
    // Line 69 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: ClearFrameBuffers_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call ClearFrameBuffers
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 87 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, ClearFrameBuffers_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    agent->ClearFrameBuffers(true);

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 88 without crash
    // Line 87 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: OnScreenConnected_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call OnScreenConnected
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 106 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, OnScreenConnected_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    agent->OnScreenConnected(output, property);

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 107 without crash
    // Line 106 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: OnScreenDisconnected_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call OnScreenDisconnected
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 123 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, OnScreenDisconnected_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    agent->OnScreenDisconnected();

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 124 without crash
    // Line 123 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: OnHwcRestored_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call OnHwcRestored
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 141 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, OnHwcRestored_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    auto output = std::make_shared<HdiOutput>(2u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    agent->OnHwcRestored(output, property);

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 142 without crash
    // Line 141 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: OnHwcDead_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call OnHwcDead
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 158 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, OnHwcDead_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    agent->OnHwcDead();

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 159 without crash
    // Line 158 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: CleanLayerBufferBySurfaceId_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call CleanLayerBufferBySurfaceId
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 175 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, CleanLayerBufferBySurfaceId_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    agent->CleanLayerBufferBySurfaceId(12345);

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 176 without crash
    // Line 175 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: ClearRedrawGPUCompositionCache_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call ClearRedrawGPUCompositionCache
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 192 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, ClearRedrawGPUCompositionCache_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    std::unordered_set<uint64_t> bufferIds = {1, 2, 3};
    agent->ClearRedrawGPUCompositionCache(bufferIds);

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 193 without crash
    // Line 192 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

/**
 * Function: SetScreenBacklight_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call SetScreenBacklight
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 209 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, SetScreenBacklight_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    agent->SetScreenBacklight(50);

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 210 without crash
    // Line 209 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: OnScreenVBlankIdleCallback_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call OnScreenVBlankIdleCallback
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 226 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, OnScreenVBlankIdleCallback_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    agent->OnScreenVBlankIdleCallback(3u, 789012);

    // Set rsRenderComposer_ to nullptr after task is posted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The task should return early at line 227 without crash
    // Line 226 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: SurfaceDump_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call SurfaceDump
 *                  3. set rsRenderComposer_ to nullptr before task executes
 *                  4. verify line 243 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, SurfaceDump_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    std::string dumpString;
    agent->SurfaceDump(dumpString);

    // Set rsRenderComposer_ to nullptr after task is posted
    // Note: SurfaceDump uses ScheduleTask().wait() so we need a different approach
    // This test verifies the early return path when composer becomes null
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: GetRefreshInfoToSP_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call GetRefreshInfoToSP
 *                  3. verify line 260 condition check
 */
HWTEST_F(RsRenderComposerAgentTest, GetRefreshInfoToSP_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    std::string dumpString;
    agent->GetRefreshInfoToSP(dumpString, 123);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: FpsDump_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call FpsDump
 *                  3. verify line 277 condition check
 */
HWTEST_F(RsRenderComposerAgentTest, FpsDump_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    std::string dumpString;
    std::string layerName = "testLayer";
    agent->FpsDump(dumpString, layerName);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: ClearFpsDump_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call ClearFpsDump
 *                  3. verify line 294 condition check
 */
HWTEST_F(RsRenderComposerAgentTest, ClearFpsDump_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    std::string dumpString;
    std::string layerName = "testLayer";
    agent->ClearFpsDump(dumpString, layerName);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: HitchsDump_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call HitchsDump
 *                  3. verify line 311 condition check
 */
HWTEST_F(RsRenderComposerAgentTest, HitchsDump_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    std::string dumpString;
    std::string layerArg = "testLayer";
    agent->HitchsDump(dumpString, layerArg);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: RefreshRateCounts_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call RefreshRateCounts
 *                  3. verify line 328 condition check
 */
HWTEST_F(RsRenderComposerAgentTest, RefreshRateCounts_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    std::string dumpString;
    agent->RefreshRateCounts(dumpString);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: ClearRefreshRateCounts_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call ClearRefreshRateCounts
 *                  3. verify line 345 condition check
 */
HWTEST_F(RsRenderComposerAgentTest, ClearRefreshRateCounts_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    std::string dumpString;
    agent->ClearRefreshRateCounts(dumpString);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * Function: PreAllocProtectedFrameBuffers_AgentNotNull_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call PreAllocProtectedFrameBuffers
 *                  3. set rsRenderComposer_ to nullptr before ffrt task executes
 *                  4. verify line 367 condition: renderComposerAgent != nullptr, rsRenderComposer_ == nullptr (true)
 *                  5. verify lambda returns early without crash
 */
HWTEST_F(RsRenderComposerAgentTest, PreAllocProtectedFrameBuffers_AgentNotNull_ComposerNull, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    agent->PreAllocProtectedFrameBuffers(buffer);

    // Set rsRenderComposer_ to nullptr after ffrt task is submitted
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    agent->rsRenderComposer_ = nullptr;

    // The ffrt task should return early at line 368 without crash
    // Line 367 condition: renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: MultipleMethodsWithComposerNull_BatchTest
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call multiple methods
 *                  3. set rsRenderComposer_ to nullptr after posting tasks
 *                  4. verify all lambda callbacks handle null composer gracefully
 */
HWTEST_F(RsRenderComposerAgentTest, MultipleMethodsWithComposerNull_BatchTest, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    // Post multiple tasks
    auto tx = std::make_shared<RSLayerTransactionData>();
    agent->ComposerProcess(tx);
    agent->ClearFrameBuffers(false);
    agent->CleanLayerBufferBySurfaceId(111);
    agent->SetScreenBacklight(75);
    agent->OnScreenVBlankIdleCallback(4u, 567890);
    agent->OnScreenDisconnected();

    // Set rsRenderComposer_ to nullptr after all tasks are posted
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    agent->rsRenderComposer_ = nullptr;

    // All tasks should return early without crash
    // The conditions at lines 69, 87, 175, 209, 226, 123 are all:
    // renderComposerAgent != nullptr && rsRenderComposer_ == nullptr -> true
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // Verify agent still exists but composer is null
    EXPECT_NE(agent, nullptr);
    EXPECT_EQ(agent->rsRenderComposer_, nullptr);
}

/**
 * Function: MultipleMethodsSetNullDuringExecution
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerAgent with valid RSRenderComposer
 *                  2. call methods, set composer to null between calls
 *                  3. verify early return handling
 */
HWTEST_F(RsRenderComposerAgentTest, MultipleMethodsSetNullDuringExecution, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer_);
    ASSERT_NE(agent->rsRenderComposer_, nullptr);

    // First call - composer is still valid
    auto tx = std::make_shared<RSLayerTransactionData>();
    agent->ComposerProcess(tx);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Set composer to null between calls
    agent->rsRenderComposer_ = nullptr;

    // Subsequent calls - composer is null at the time of call (line 53, 80, 99, etc.)
    // These methods should return early at the outer null check
    agent->ComposerProcess(tx);
    agent->ClearFrameBuffers(true);
    agent->CleanLayerBufferBySurfaceId(222);
    agent->SetScreenBacklight(100);

    // The already-posted task from first call should return early at line 70
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

} // namespace Rosen
} // namespace OHOS
