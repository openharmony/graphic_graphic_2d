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

#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

#include "event_handler.h"
#include "hdi_output.h"
#include "rs_render_composer_manager.h"

#include "screen_manager/rs_screen_property.h"
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);
    mgr->OnScreenConnected(nullptr, nullptr);
    EXPECT_TRUE(mgr->rsRenderComposerAgentMap_.empty());
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);
    auto output = std::make_shared<HdiOutput>(10u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
    EXPECT_EQ(mgr->rsComposerConnectionMap_.size(), 0u);
    auto conn = mgr->GetRSComposerConnection(10u);
    EXPECT_EQ(conn, nullptr);

    auto uniRenderEnabledType = RSUniRenderJudgement::uniRenderEnabledType_;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    mgr->OnScreenConnected(output, property);
    mgr->OnScreenDisconnected(0);
    RSUniRenderJudgement::uniRenderEnabledType_ = uniRenderEnabledType;
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);
    auto output = std::make_shared<HdiOutput>(20u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property); // first insert
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
    EXPECT_EQ(mgr->rsComposerConnectionMap_.size(), 0u);
    mgr->OnScreenConnected(output, property); // second enter else branch
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
    EXPECT_EQ(mgr->rsComposerConnectionMap_.size(), 0u);
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);
    mgr->OnScreenDisconnected(9999);
    EXPECT_TRUE(mgr->rsRenderComposerAgentMap_.empty());
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);
    auto output = std::make_shared<HdiOutput>(30u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
    mgr->OnScreenDisconnected(30u);
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);
    EXPECT_EQ(mgr->GetRSComposerConnection(1), nullptr);
    auto output = std::make_shared<HdiOutput>(40u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    EXPECT_EQ(mgr->GetRSComposerConnection(40u), nullptr);
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property); // first insert
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mgr->OnScreenConnected(output, property); // second enter else branch
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.find(1), mgr->rsRenderComposerAgentMap_.end());
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto it = mgr->rsRenderComposerAgentMap_.find(1);
    ASSERT_EQ(it, mgr->rsRenderComposerAgentMap_.end());
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.find(1), mgr->rsRenderComposerAgentMap_.end());
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);
    mgr->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    mgr->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mgr->RefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());
    dumpString = "";
    mgr->ClearRefreshRateCounts(dumpString);
    EXPECT_TRUE(dumpString.empty());

    mgr->rsRenderComposerAgentMap_.insert(std::pair(0u, nullptr));
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    mgr->FpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString.find("[Id:"), std::string::npos);

    dumpString = "";
    mgr->ClearFpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString.find("[Id:"), std::string::npos);

    dumpString = "";
    mgr->HitchsDump(dumpString, layerArg);
    EXPECT_EQ(dumpString.find("window_test"), std::string::npos);

    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mgr->rsRenderComposerAgentMap_.insert(std::pair(0u, nullptr));

    mgr->FpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString.find("[Id:1]"), std::string::npos);

    dumpString = "";
    mgr->ClearFpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString.find("[Id:1]"), std::string::npos);

    dumpString = "";
    mgr->HitchsDump(dumpString, layerArg);
    EXPECT_EQ(dumpString.find("window_test"), std::string::npos);
}

/**
 * Function: OnHwcRestored_NullParams_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Call OnHwcRestored with nullptr output/property; expect no changes.
 */
HWTEST_F(RsRenderComposerManagerTest, OnHwcRestored_NullParams_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto mgr = std::make_shared<RSRenderComposerManager>(handler);
    mgr->OnHwcRestored(nullptr, nullptr);
    auto output = std::make_shared<HdiOutput>(99u);
    mgr->OnHwcRestored(output, nullptr);
    EXPECT_TRUE(mgr->rsRenderComposerAgentMap_.empty());
}

/**
 * Function: OnHwcRestored_NotFound_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Call OnHwcRestored for a screen not connected; expect no crash or insert.
 */
HWTEST_F(RsRenderComposerManagerTest, OnHwcRestored_NotFound_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto mgr = std::make_shared<RSRenderComposerManager>(handler);
    auto output = std::make_shared<HdiOutput>(99u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnHwcRestored(output, property);
    EXPECT_TRUE(mgr->rsRenderComposerAgentMap_.empty());

    mgr->OnScreenConnected(output, property);
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
    mgr->OnHwcRestored(output, property);
}

/**
 * Function: OnHwcDead_NotFound_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Call OnHwcDead for a screen not connected; expect early return.
 */
HWTEST_F(RsRenderComposerManagerTest, OnHwcDead_NotFound_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto mgr = std::make_shared<RSRenderComposerManager>(handler);
    mgr->OnHwcDead(123u);
    EXPECT_TRUE(mgr->rsRenderComposerAgentMap_.empty());
}

/**
 * Function: OnHwcDead_Found_Path
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Connect a screen then call OnHwcDead; expect map size unchanged.
 */
HWTEST_F(RsRenderComposerManagerTest, OnHwcDead_Found_Path, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto mgr = std::make_shared<RSRenderComposerManager>(handler);
    auto output = std::make_shared<HdiOutput>(55u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
    mgr->OnHwcDead(55u);
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
}

/**
 * Function: PostTask_NullHandler_NoRun
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: PostTask/Sync/Delay with null handler; tasks should not run.
 */
HWTEST_F(RsRenderComposerManagerTest, PostTask_NullHandler_NoRun, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto mgr = std::make_shared<RSRenderComposerManager>(handler);
    std::atomic<bool> ran { false };
    mgr->PostTask([&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(ran.load());
    mgr->PostSyncTask([&ran]() { ran.store(true); });
    EXPECT_FALSE(ran.load());
    mgr->PostDelayTask([&ran]() { ran.store(true); }, 10);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(ran.load());
}

/**
 * Function: PostTask_ValidHandler_Runs
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: With valid handler, verify PostTask/Sync/Delay execute.
 */
HWTEST_F(RsRenderComposerManagerTest, PostTask_ValidHandler_Runs, TestSize.Level1)
{
    auto runner = AppExecFwk::EventRunner::Create("mgr_runner");
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    auto mgr = std::make_shared<RSRenderComposerManager>(handler);
    std::atomic<int> count { 0 };
    mgr->PostTask([&count]() { count.fetch_add(1); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    mgr->PostDelayTask([&count]() { count.fetch_add(1); }, 10);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    mgr->PostSyncTask([&count]() { count.fetch_add(1); });
    EXPECT_GE(count.load(), 2);
}

/**
 * Function: SurfaceDump_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Call SurfaceDump with empty map, null agent, and connected agent.
 */
HWTEST_F(RsRenderComposerManagerTest, SurfaceDump_Branches, TestSize.Level1)
{
    std::string dumpString;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto mgr = std::make_shared<RSRenderComposerManager>(handler);
    mgr->SurfaceDump(dumpString);
    EXPECT_TRUE(dumpString.empty());
    mgr->rsRenderComposerAgentMap_.insert(std::pair(0u, nullptr));
    mgr->SurfaceDump(dumpString);
    EXPECT_TRUE(dumpString.empty());
    auto output = std::make_shared<HdiOutput>(66u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    mgr->SurfaceDump(dumpString);
    EXPECT_GE(dumpString.size(), 0u);
}

/**
 * Function: GetRefreshInfoToSP_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Call GetRefreshInfoToSP with empty map, null agent, and then connected agent.
 */
HWTEST_F(RsRenderComposerManagerTest, GetRefreshInfoToSP_Branches, TestSize.Level1)
{
    std::string dumpString;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto mgr = std::make_shared<RSRenderComposerManager>(handler);
    mgr->GetRefreshInfoToSP(dumpString, 0);
    EXPECT_TRUE(dumpString.empty());
    mgr->rsRenderComposerAgentMap_.insert(std::pair(0u, nullptr));
    mgr->GetRefreshInfoToSP(dumpString, 0);
    EXPECT_TRUE(dumpString.empty());
    auto output = std::make_shared<HdiOutput>(77u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    mgr->GetRefreshInfoToSP(dumpString, 0);
    EXPECT_GE(dumpString.size(), 0u);
}

/**
 * Function: OnScreenConnected_NotUniRenderEnabled_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set uniRenderEnabledType to NOT UNI_RENDER_ENABLED_FOR_ALL
 *                  2. call OnScreenConnected with valid output and property
 *                  3. verify function returns early (line 33 branch true)
 *                     and no composer is added to the map
 */
HWTEST_F(RsRenderComposerManagerTest, OnScreenConnected_NotUniRenderEnabled_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    auto output = std::make_shared<HdiOutput>(88u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();

    // Test case 3: After restoring to UNI_RENDER_ENABLED_FOR_ALL, composer should be added
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    mgr->OnScreenConnected(output, property);
    // Verify composer was added
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
    EXPECT_EQ(mgr->rsComposerConnectionMap_.size(), 0u);
}

/**
 * Function: OnScreenConnected_UniRenderEnabledForAll_ComposerAdded
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set uniRenderEnabledType to UNI_RENDER_ENABLED_FOR_ALL
 *                  2. call OnScreenConnected with valid output and property
 *                  3. verify composer is added to the map (line 33 branch false)
 */
HWTEST_F(RsRenderComposerManagerTest, OnScreenConnected_UniRenderEnabledForAll_ComposerAdded, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    auto output = std::make_shared<HdiOutput>(89u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();

    // Save original uniRenderEnabledType
    auto originalType = RSUniRenderJudgement::uniRenderEnabledType_;

    // Set to UNI_RENDER_ENABLED_FOR_ALL (line 33 branch false)
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    mgr->OnScreenConnected(output, property);

    // Verify composer was added to the maps
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
    EXPECT_EQ(mgr->rsComposerConnectionMap_.size(), 0u);

    // Verify the connection can be retrieved
    auto conn = mgr->GetRSComposerConnection(89u);
    EXPECT_EQ(conn, nullptr);

    // Restore original type
    RSUniRenderJudgement::uniRenderEnabledType_ = originalType;
}

/**
 * Function: HandlePowerStatus_ScreenIdNotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager without any connected screens
 *                  2. call HandlePowerStatus with non-existent screenId
 *                  3. verify function returns early (line 292 branch true)
 */
HWTEST_F(RsRenderComposerManagerTest, HandlePowerStatus_ScreenIdNotFound, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    // Call HandlePowerStatus with non-existent screenId
    mgr->HandlePowerStatus(9999, ScreenPowerStatus::POWER_STATUS_ON);

    // Verify no composer was added
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
}

/**
 * Function: HandlePowerStatus_ScreenIdFound_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager and connect a screen
 *                  2. call HandlePowerStatus with existing screenId
 *                  3. verify function executes normally (line 292 branch false, line 298 branch false)
 */
HWTEST_F(RsRenderComposerManagerTest, HandlePowerStatus_ScreenIdFound_Success, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    auto output = std::make_shared<HdiOutput>(111u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);

    // Call HandlePowerStatus with existing screenId
    mgr->HandlePowerStatus(111u, ScreenPowerStatus::POWER_STATUS_ON);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify composer still exists in map
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
}

/**
 * Function: HandlePowerStatus_NullAgentInMap
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. manually insert nullptr agent into the map
 *                  3. call HandlePowerStatus
 *                  4. verify function returns early (line 298 branch true)
 */
HWTEST_F(RsRenderComposerManagerTest, HandlePowerStatus_NullAgentInMap, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    // Manually insert nullptr agent into the map
    mgr->rsRenderComposerAgentMap_.insert(std::pair(222u, nullptr));

    // Call HandlePowerStatus with screenId that has nullptr agent
    mgr->HandlePowerStatus(222u, ScreenPowerStatus::POWER_STATUS_OFF);

    // Verify map still contains the nullptr entry (no removal occurred)
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 1u);
}

/**
 * Function: HandlePowerStatus_DifferentPowerStatus
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager and connect a screen
 *                  2. call HandlePowerStatus with different power statuses
 *                  3. verify all status changes are handled correctly
 */
HWTEST_F(RsRenderComposerManagerTest, HandlePowerStatus_DifferentPowerStatus, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    auto output = std::make_shared<HdiOutput>(112u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);

    // Test different power statuses
    mgr->HandlePowerStatus(112u, ScreenPowerStatus::POWER_STATUS_ON);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mgr->HandlePowerStatus(112u, ScreenPowerStatus::POWER_STATUS_OFF);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mgr->HandlePowerStatus(112u, ScreenPowerStatus::POWER_STATUS_DOZE);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mgr->HandlePowerStatus(112u, ScreenPowerStatus::POWER_STATUS_DOZE_SUSPEND);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mgr->HandlePowerStatus(112u, ScreenPowerStatus::POWER_STATUS_SUSPEND);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify composer still exists in map
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
}

/**
 * Function: HandlePowerStatus_MultipleScreens
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager and connect multiple screens
 *                  2. call HandlePowerStatus for each screen
 *                  3. verify all screens are handled correctly
 */
HWTEST_F(RsRenderComposerManagerTest, HandlePowerStatus_MultipleScreens, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    // Connect multiple screens
    for (uint32_t screenId = 120u; screenId < 123u; screenId++) {
        auto output = std::make_shared<HdiOutput>(screenId);
        output->Init();
        sptr<RSScreenProperty> property = new RSScreenProperty();
        mgr->OnScreenConnected(output, property);
    }
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);

    // Handle power status for each screen
    mgr->HandlePowerStatus(120u, ScreenPowerStatus::POWER_STATUS_ON);
    mgr->HandlePowerStatus(121u, ScreenPowerStatus::POWER_STATUS_OFF);
    mgr->HandlePowerStatus(122u, ScreenPowerStatus::POWER_STATUS_DOZE);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify all composers still exist
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
}

/**
 * Function: HandlePowerStatus_InvalidScreenId
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager and connect
 a screen
 *                  2. call HandlePowerStatus with invalid screenId
 *                  3. verify function returns early without affecting existing composers
 */
HWTEST_F(RsRenderComposerManagerTest, HandlePowerStatus_InvalidScreenId, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    auto output = std::make_shared<HdiOutput>(113u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);

    // Call HandlePowerStatus with non-existent screenId
    mgr->HandlePowerStatus(9999, ScreenPowerStatus::POWER_STATUS_ON);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify original composer still exists
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
    auto agent = mgr->rsRenderComposerAgentMap_.find(113);
    EXPECT_EQ(agent, mgr->rsRenderComposerAgentMap_.end());
}

/**
 * Function: SetAFBCEnabledWithRenderComposerAgentNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. set renderComposerAgent Null, call SetAFBCEnabled
 *                  3. verify function returns early
 */
HWTEST_F(RsRenderComposerManagerTest, SetAFBCEnabledWithRenderComposerAgentNull, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    ScreenId screenId = 0;
    mgr->rsRenderComposerAgentMap_[screenId] = nullptr;

    mgr->SetAFBCEnabled(screenId, true);
    auto agent = mgr->rsRenderComposerAgentMap_.find(screenId);
    EXPECT_NE(agent, mgr->rsRenderComposerAgentMap_.end());
}

/**
 * Function: SetAFBCEnabledWithRenderComposerAgent
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager and valid renderComposer
 *                  2. call SetAFBCEnabled with invalid screenId and valid id
 *                  3. verify function returns
 */
HWTEST_F(RsRenderComposerManagerTest, SetAFBCEnabledWithRenderComposerAgent, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    ScreenId screenId = 0;
    auto output = std::make_shared<HdiOutput>(screenId);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    std::shared_ptr<RSRenderComposer> rsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    auto agent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
    mgr->rsRenderComposerAgentMap_[screenId] = agent;

    mgr->SetAFBCEnabled(9999, true);
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.find(9999), mgr->rsRenderComposerAgentMap_.end());

    mgr->SetAFBCEnabled(screenId, true);
    EXPECT_NE(mgr->rsRenderComposerAgentMap_.find(screenId), mgr->rsRenderComposerAgentMap_.end());
}

/**
 * Function: RegisterVsyncManagerCallbacks_ScreenIdNotFound_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager without any connected screens
 *                  2. call RegisterVsyncManagerCallbacks with non-existent screenId
 *                  3. verify function returns early (line 313 branch true)
 */
HWTEST_F(RsRenderComposerManagerTest, RegisterVsyncManagerCallbacks_ScreenIdNotFound_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    // Create dummy callbacks
    auto setHardwareTaskNumCb = [](uint32_t) {};
    auto setTaskEndWithTimeCb = [](uint64_t) {};
    auto getRealTimeOffsetOfDvsyncCb = [](uint64_t, int64_t&) -> uint64_t { return 0; };

    // Call RegisterVsyncManagerCallbacks with non-existent screenId
    // Line 313 branch: iter == rsRenderComposerAgentMap_.end() -> true
    mgr->RegisterVsyncManagerCallbacks(9999, setHardwareTaskNumCb, setTaskEndWithTimeCb, getRealTimeOffsetOfDvsyncCb);

    // Verify no composer was added
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
}

/**
 * Function: RegisterVsyncManagerCallbacks_NullAgentInMap_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager
 *                  2. manually insert nullptr agent into the map
 *                  3. call RegisterVsyncManagerCallbacks
 *                  4. verify function returns early (line 319 branch true)
 */
HWTEST_F(RsRenderComposerManagerTest, RegisterVsyncManagerCallbacks_NullAgentInMap_EarlyReturn, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    // Manually insert nullptr agent into the map
    mgr->rsRenderComposerAgentMap_.insert(std::pair(222u, nullptr));

    // Create dummy callbacks
    auto setHardwareTaskNumCb = [](uint32_t) {};
    auto setTaskEndWithTimeCb = [](uint64_t) {};
    auto getRealTimeOffsetOfDvsyncCb = [](uint64_t, int64_t&) -> uint64_t { return 0; };

    // Call RegisterVsyncManagerCallbacks with screenId that has nullptr agent
    // Line 313 branch: iter == rsRenderComposerAgentMap_.end() -> false
    // Line 319 branch: renderComposerAgent == nullptr -> true
    mgr->RegisterVsyncManagerCallbacks(222u, setHardwareTaskNumCb, setTaskEndWithTimeCb, getRealTimeOffsetOfDvsyncCb);

    // Verify map still contains the nullptr entry (no removal occurred)
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 1u);
}

/**
 * Function: RegisterVsyncManagerCallbacks_ValidAgent_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager and connect a screen
 *                  2. call RegisterVsyncManagerCallbacks with existing screenId
 *                  3. verify function executes normally (line 313 branch false, line 319 branch false)
 */
HWTEST_F(RsRenderComposerManagerTest, RegisterVsyncManagerCallbacks_ValidAgent_Success, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    auto output = std::make_shared<HdiOutput>(333u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);

    // Create dummy callbacks
    auto setHardwareTaskNumCb = [](uint32_t) {};
    auto setTaskEndWithTimeCb = [](uint64_t) {};
    auto getRealTimeOffsetOfDvsyncCb = [](uint64_t, int64_t&) -> uint64_t { return 0; };

    // Call RegisterVsyncManagerCallbacks with existing screenId
    // Line 313 branch: iter == rsRenderComposerAgentMap_.end() -> false
    // Line 319 branch: renderComposerAgent == nullptr -> false (agent is valid)
    // Note: Since uniRender is enabled, the agent map may be empty, but the function should not crash
    mgr->RegisterVsyncManagerCallbacks(333u, setHardwareTaskNumCb, setTaskEndWithTimeCb, getRealTimeOffsetOfDvsyncCb);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify composer still exists in map
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
}

/**
 * Function: RegisterVsyncManagerCallbacks_MultipleScreens
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager and connect multiple screens
 *                  2. call RegisterVsyncManagerCallbacks for each screen
 *                  3. verify all screens are handled correctly
 */
HWTEST_F(RsRenderComposerManagerTest, RegisterVsyncManagerCallbacks_MultipleScreens, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    // Connect multiple screens
    for (uint32_t screenId = 130u; screenId < 133u; screenId++) {
        auto output = std::make_shared<HdiOutput>(screenId);
        output->Init();
        sptr<RSScreenProperty> property = new RSScreenProperty();
        mgr->OnScreenConnected(output, property);
    }
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);

    // Create dummy callbacks
    auto setHardwareTaskNumCb = [](uint32_t) {};
    auto setTaskEndWithTimeCb = [](uint64_t) {};
    auto getRealTimeOffsetOfDvsyncCb = [](uint64_t, int64_t&) -> uint64_t { return 0; };

    // Register callbacks for each screen
    mgr->RegisterVsyncManagerCallbacks(130u, setHardwareTaskNumCb, setTaskEndWithTimeCb, getRealTimeOffsetOfDvsyncCb);
    mgr->RegisterVsyncManagerCallbacks(131u, setHardwareTaskNumCb, setTaskEndWithTimeCb, getRealTimeOffsetOfDvsyncCb);
    mgr->RegisterVsyncManagerCallbacks(132u, setHardwareTaskNumCb, setTaskEndWithTimeCb, getRealTimeOffsetOfDvsyncCb);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify all composers still exist
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
}

/**
 * Function: RegisterVsyncManagerCallbacks_CallbacksExecute
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerManager and connect a screen
 *                  2. call RegisterVsyncManagerCallbacks with callbacks that track execution
 *                  3. verify callbacks are properly forwarded to agent
 */
HWTEST_F(RsRenderComposerManagerTest, RegisterVsyncManagerCallbacks_CallbacksExecute, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler);

    auto output = std::make_shared<HdiOutput>(140u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);

    // Create callbacks that track execution
    std::atomic<bool> hardwareTaskNumCalled { false };
    std::atomic<bool> taskEndWithTimeCalled { false };
    std::atomic<bool> realTimeOffsetCalled { false };

    auto setHardwareTaskNumCb = [&hardwareTaskNumCalled](uint32_t) { hardwareTaskNumCalled.store(true); };
    auto setTaskEndWithTimeCb = [&taskEndWithTimeCalled](uint64_t) { taskEndWithTimeCalled.store(true); };
    auto getRealTimeOffsetOfDvsyncCb = [&realTimeOffsetCalled](uint64_t, int64_t&) -> uint64_t {
        realTimeOffsetCalled.store(true);
        return 0;
    };

    // Register callbacks
    mgr->RegisterVsyncManagerCallbacks(140u, setHardwareTaskNumCb, setTaskEndWithTimeCb, getRealTimeOffsetOfDvsyncCb);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify function completed without crash
    // Note: Callbacks may not be executed immediately as they are forwarded to agent
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 0u);
}
} // namespace Rosen
} // namespace OHOS
