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
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
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
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
    auto output = std::make_shared<HdiOutput>(10u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 1u);
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
    auto output = std::make_shared<HdiOutput>(20u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property); // first insert
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 1u);
    EXPECT_EQ(mgr->rsComposerConnectionMap_.size(), 1u);
    mgr->OnScreenConnected(output, property); // second enter else branch
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 1u);
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
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
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
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
    auto output = std::make_shared<HdiOutput>(30u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    ASSERT_EQ(mgr->rsRenderComposerAgentMap_.size(), 1u);
    mgr->OnScreenDisconnected(30u);
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.size(), 1u);
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
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
    EXPECT_EQ(mgr->GetRSComposerConnection(1), nullptr);
    auto output = std::make_shared<HdiOutput>(40u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    EXPECT_NE(mgr->GetRSComposerConnection(40u), nullptr);
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
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
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
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
    EXPECT_EQ(mgr->rsRenderComposerAgentMap_.find(1), mgr->rsRenderComposerAgentMap_.end());
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto it = mgr->rsRenderComposerAgentMap_.find(1);
    ASSERT_NE(it, mgr->rsRenderComposerAgentMap_.end());
    it->second->OnScreenVBlankIdleCallback(1, 0);
    EXPECT_NE(mgr->rsRenderComposerAgentMap_.find(1), mgr->rsRenderComposerAgentMap_.end());
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
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
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
    std::shared_ptr<RSRenderComposerManager> mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);

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
    EXPECT_NE(dumpString.find("[Id:1]"), std::string::npos);

    dumpString = "";
    mgr->ClearFpsDump(dumpString, layerName);
    EXPECT_NE(dumpString.find("[Id:1]"), std::string::npos);

    dumpString = "";
    mgr->HitchsDump(dumpString, layerArg);
    EXPECT_EQ(dumpString.find("window_test"), std::string::npos);
}
} // namespace Rosen
} // namespace OHOS
