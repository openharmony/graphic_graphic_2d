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

#include <gtest/gtest.h>

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "render_server/rs_render_multi_process_manager.h"
#include "render_server/rs_render_multi_process_manager_repository.h"
#include "rs_render_pipeline_agent.h"
#include "render_server/rs_render_service.h"
#include "screen_manager/rs_screen_property.h"
#include "transaction/rs_service_to_render_connection.h"
#include "transaction/rs_connect_to_render_process.h"
#include "rs_composer_to_render_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr pid_t TEST_PID = 1234;
constexpr pid_t TEST_PID_2 = 5678;
constexpr pid_t TEST_PID_3 = 124435;
constexpr ScreenId TEST_SCREEN_ID = 100;
constexpr ScreenId TEST_VIRTUAL_SCREEN_ID = 300;
constexpr ScreenId TEST_ASSOCIATED_SCREEN_ID = 400;
constexpr GroupId TEST_GROUP_ID = 1;
constexpr GroupId TEST_GROUP_ID_2 = 2;
const ProcessUniqueId TEST_PROCESS_UNIQUE_ID{TEST_PID};
const ProcessUniqueId TEST_PROCESS_UNIQUE_ID_2{TEST_PID_2};
const ProcessUniqueId TEST_PROCESS_UNIQUE_ID_3{TEST_PID_3};
} // namespace

class RSMultiRenderProcessManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static RSRenderService* renderService_;
    static sptr<RSMultiRenderProcessManager> multiProcessManager_;
};

RSRenderService* RSMultiRenderProcessManagerTest::renderService_ = nullptr;
sptr<RSMultiRenderProcessManager> RSMultiRenderProcessManagerTest::multiProcessManager_ = nullptr;

void RSMultiRenderProcessManagerTest::SetUpTestCase()
{
    renderService_ = new RSRenderService();
    multiProcessManager_ = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService_);
}

void RSMultiRenderProcessManagerTest::TearDownTestCase()
{
    if (multiProcessManager_ != nullptr) {
        for (auto& [processUniqueId, deathRecipient] : multiProcessManager_->stateStore_.deathRecipients_) {
            auto serviceConn = multiProcessManager_->stateStore_.GetServiceToRenderConnByUniqueId(processUniqueId);
            if (serviceConn != nullptr) {
                serviceConn->AsObject()->RemoveDeathRecipient(deathRecipient);
            }
        }
        multiProcessManager_->stateStore_.serviceToRenderConnections_.clear();
        multiProcessManager_->stateStore_.connectToRenderConnections_.clear();
        multiProcessManager_->stateStore_.composerToRenderConnections_.clear();
        multiProcessManager_->stateStore_.deathRecipients_.clear();
        multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.clear();
        multiProcessManager_->stateStore_.processToScreenOutputMap_.clear();
        multiProcessManager_->stateStore_.virtualToPhysicalScreenMap_.clear();
        multiProcessManager_->stateStore_.pendingScreenConnectInfos_.clear();
        multiProcessManager_->stateStore_.renderProcessReadyPromises_.clear();
        multiProcessManager_->stateStore_.subprocessDeathTimes_.clear();
        multiProcessManager_ = nullptr;
    }
    delete renderService_;
    renderService_ = nullptr;
}

void RSMultiRenderProcessManagerTest::SetUp() {}
void RSMultiRenderProcessManagerTest::TearDown()
{
    if (multiProcessManager_ != nullptr) {
        // Unregister death recipients from binder objects before clearing
        for (auto& [processUniqueId, deathRecipient] : multiProcessManager_->stateStore_.deathRecipients_) {
            auto serviceConn = multiProcessManager_->stateStore_.GetServiceToRenderConnByUniqueId(processUniqueId);
            if (serviceConn != nullptr) {
                serviceConn->AsObject()->RemoveDeathRecipient(deathRecipient);
            }
        }
        // Clear connections first (release IRemoteObject refs), then deathRecipients
        multiProcessManager_->stateStore_.serviceToRenderConnections_.clear();
        multiProcessManager_->stateStore_.connectToRenderConnections_.clear();
        multiProcessManager_->stateStore_.composerToRenderConnections_.clear();
        multiProcessManager_->stateStore_.deathRecipients_.clear();
        multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.clear();
        multiProcessManager_->stateStore_.processToScreenOutputMap_.clear();
        multiProcessManager_->stateStore_.virtualToPhysicalScreenMap_.clear();
        multiProcessManager_->stateStore_.pendingScreenConnectInfos_.clear();
        multiProcessManager_->stateStore_.renderProcessReadyPromises_.clear();
        multiProcessManager_->stateStore_.subprocessDeathTimes_.clear();
    }
}

/**
 * @tc.name: CreateMultiProcessManagerTest001
 * @tc.desc: Test creating RSMultiRenderProcessManager instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, CreateMultiProcessManagerTest001, TestSize.Level1)
{
    auto renderService = new RSRenderService();
    auto manager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);

    ASSERT_NE(manager, nullptr);

    manager = nullptr;
    delete renderService;
}

/**
 * @tc.name: GetPendingScreenPropertyTest001
 * @tc.desc: Test GetPendingScreenProperty with non-existent PID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetPendingScreenPropertyTest001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);

    auto property = multiProcessManager_->GetPendingScreenProperty(TEST_PID);

    // Should return nullptr for non-existent PID
    ASSERT_EQ(property, nullptr);
}

/**
 * @tc.name: GetPendingScreenPropertyTest002
 * @tc.desc: Test GetPendingScreenProperty with different PID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetPendingScreenPropertyTest002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);

    auto property = multiProcessManager_->GetPendingScreenProperty(TEST_PID_2);

    // Should return nullptr for non-existent PID
    ASSERT_EQ(property, nullptr);
}

/**
 * @tc.name: CreateMultipleManagersTest001
 * @tc.desc: Test creating multiple RSMultiRenderProcessManager instances
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, CreateMultipleManagersTest001, TestSize.Level1)
{
    auto renderService1 = new RSRenderService();
    auto renderService2 = new RSRenderService();

    auto manager1 = new RSMultiRenderProcessManager(*renderService1);
    auto manager2 = new RSMultiRenderProcessManager(*renderService2);

    ASSERT_NE(manager1, nullptr);
    ASSERT_NE(manager2, nullptr);
    ASSERT_NE(manager1, manager2);

    delete manager2;
    delete manager1;
    delete renderService2;
    delete renderService1;
}

/**
 * @tc.name: GetIpcPersistenceManager001
 * @tc.desc: Test GetIpcPersistenceManager returns non-null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetIpcPersistenceManager001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto ipcManager = multiProcessManager_->GetIpcPersistenceManager();
    ASSERT_NE(ipcManager, nullptr);
}

/**
 * @tc.name: RecordComposerToRenderConnection001
 * @tc.desc: Test RecordComposerToRenderConnection stores connection in map
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, RecordComposerToRenderConnection001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    sptr<IRSComposerToRenderConnection> conn = nullptr;
    multiProcessManager_->RecordComposerToRenderConnection(TEST_PID, conn);
    auto stored = multiProcessManager_->stateStore_.composerToRenderConnections_.find(TEST_PROCESS_UNIQUE_ID);
    ASSERT_NE(stored, multiProcessManager_->stateStore_.composerToRenderConnections_.end());
    EXPECT_EQ(stored->second, nullptr);
}

/**
 * @tc.name: RecordComposerToRenderConnection002
 * @tc.desc: Test RecordComposerToRenderConnection overwrites existing entry
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, RecordComposerToRenderConnection002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    sptr<IRSComposerToRenderConnection> conn1 = nullptr;
    multiProcessManager_->RecordComposerToRenderConnection(TEST_PID, conn1);
    sptr<IRSComposerToRenderConnection> conn2 = nullptr;
    multiProcessManager_->RecordComposerToRenderConnection(TEST_PID, conn2);
    EXPECT_EQ(multiProcessManager_->stateStore_.composerToRenderConnections_.count(TEST_PROCESS_UNIQUE_ID), 1u);
}

/**
 * @tc.name: UpdateAndGetGroupIdToPid001
 * @tc.desc: Test UpdateGroupIdToRenderProcessUniqueId and GetRenderProcessUniqueIdByGroupId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, UpdateAndGetGroupIdToPid001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto result = multiProcessManager_->stateStore_.GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    EXPECT_FALSE(result.has_value());
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    result = multiProcessManager_->stateStore_.GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetPid(), TEST_PID);
}

/**
 * @tc.name: FindVirtualToPhysicalScreenMap001
 * @tc.desc: Test FindVirtualToPhysicalScreenMap returns original screenId when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, FindVirtualToPhysicalScreenMap001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto result = multiProcessManager_->stateStore_.FindVirtualToPhysicalScreenMap(TEST_VIRTUAL_SCREEN_ID);
    EXPECT_EQ(result, TEST_VIRTUAL_SCREEN_ID);
}

/**
 * @tc.name: GetGroupIdByScreenId001
 * @tc.desc: Test GetGroupIdByScreenId returns default when screenId not in config
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetGroupIdByScreenId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    auto groupId = multiProcessManager_->GetGroupIdByScreenId(TEST_SCREEN_ID);
    EXPECT_EQ(groupId, TEST_GROUP_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: GetGroupIdByScreenId002
 * @tc.desc: Test GetGroupIdByScreenId returns mapped groupId when found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetGroupIdByScreenId002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID_2)
        .Build();
    auto groupId = multiProcessManager_->GetGroupIdByScreenId(TEST_SCREEN_ID);
    EXPECT_EQ(groupId, TEST_GROUP_ID_2);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: OnScreenPropertyChanged001
 * @tc.desc: Test OnScreenPropertyChanged with null property returns early
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnScreenPropertyChanged001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    sptr<ScreenPropertyBase> property = nullptr;
    multiProcessManager_->OnScreenPropertyChanged(
        TEST_SCREEN_ID, ScreenPropertyType::IS_VIRTUAL, property);
}

/**
 * @tc.name: OnScreenPropertyChanged002
 * @tc.desc: Test OnScreenPropertyChanged with null connection returns early
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnScreenPropertyChanged002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    sptr<ScreenPropertyBase> property = sptr<ScreenProperty<bool>>::MakeSptr();
    multiProcessManager_->OnScreenPropertyChanged(
        TEST_SCREEN_ID, ScreenPropertyType::IS_VIRTUAL, property);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: OnScreenDisconnected001
 * @tc.desc: Test OnScreenDisconnected with no pid mapping returns early
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnScreenDisconnected001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    multiProcessManager_->OnScreenDisconnected(TEST_SCREEN_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: OnScreenDisconnected002
 * @tc.desc: Test OnScreenDisconnected with pid but no connection returns early
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnScreenDisconnected002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->OnScreenDisconnected(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: OnScreenRefresh001
 * @tc.desc: Test OnScreenRefresh with no pid mapping returns early
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnScreenRefresh001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    multiProcessManager_->OnScreenRefresh(TEST_SCREEN_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: OnScreenRefresh002
 * @tc.desc: Test OnScreenRefresh with pid but null connection returns early
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnScreenRefresh002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    multiProcessManager_->OnScreenRefresh(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: OnVirtualScreenDisconnected001
 * @tc.desc: Test OnVirtualScreenDisconnected with no mapping returns early
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnVirtualScreenDisconnected001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->OnVirtualScreenDisconnected(TEST_VIRTUAL_SCREEN_ID);
}

/**
 * @tc.name: OnVirtualScreenDisconnected002
 * @tc.desc: Test OnVirtualScreenDisconnected with mapping but null conn returns early
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnVirtualScreenDisconnected002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    multiProcessManager_->OnVirtualScreenDisconnected(TEST_VIRTUAL_SCREEN_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: OnVirtualScreenConnected001
 * @tc.desc: Test OnVirtualScreenConnected with null connection returns early
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnVirtualScreenConnected001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: GetServiceToRenderConn001
 * @tc.desc: Test GetServiceToRenderConn returns nullptr when no pid mapping
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConn001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    auto conn = multiProcessManager_->GetServiceToRenderConn(TEST_SCREEN_ID);
    EXPECT_EQ(conn, nullptr);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: GetServiceToRenderConn002
 * @tc.desc: Test GetServiceToRenderConn returns nullptr when pid found but no conn
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConn002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    auto conn = multiProcessManager_->GetServiceToRenderConn(TEST_SCREEN_ID);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: GetConnectToRenderConnection001
 * @tc.desc: Test GetConnectToRenderConnection returns nullptr when no pid mapping
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetConnectToRenderConnection001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    auto conn = multiProcessManager_->GetConnectToRenderConnection(TEST_SCREEN_ID);
    EXPECT_EQ(conn, nullptr);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: GetConnectToRenderConnection002
 * @tc.desc: Test GetConnectToRenderConnection returns nullptr when pid found but no conn
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetConnectToRenderConnection002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    auto conn = multiProcessManager_->GetConnectToRenderConnection(TEST_SCREEN_ID);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: GetPendingScreenPropertyWithPromise001
 * @tc.desc: Test GetPendingScreenProperty returns property when promise exists
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetPendingScreenPropertyWithPromise001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->stateStore_.pendingScreenConnectInfos_[TEST_PROCESS_UNIQUE_ID] =
        { TEST_SCREEN_ID, property };
    multiProcessManager_->stateStore_.renderProcessReadyPromises_[TEST_PROCESS_UNIQUE_ID] = std::promise<bool>();
    auto result = multiProcessManager_->GetPendingScreenProperty(TEST_PID);
    ASSERT_NE(result, nullptr);
    multiProcessManager_->stateStore_.renderProcessReadyPromises_.erase(TEST_PROCESS_UNIQUE_ID);
}

// Newly Added Phase2

HWTEST_F(RSMultiRenderProcessManagerTest, OnScreenPropertyChanged003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    sptr<ScreenPropertyBase> property = sptr<ScreenProperty<bool>>::MakeSptr();
    multiProcessManager_->OnScreenPropertyChanged(
        TEST_SCREEN_ID, ScreenPropertyType::IS_VIRTUAL, property);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnScreenDisconnected003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    sptr<RSIServiceToRenderConnection> conn = nullptr;
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = conn;
    multiProcessManager_->OnScreenDisconnected(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnVirtualScreenConnected002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_ASSOCIATED_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    sptr<RSIServiceToRenderConnection> conn = nullptr;
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = conn;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.virtualToPhysicalScreenMap_.clear();
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnVirtualScreenDisconnected003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_ASSOCIATED_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    sptr<RSIServiceToRenderConnection> conn = nullptr;
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = conn;
    multiProcessManager_->stateStore_.InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    multiProcessManager_->OnVirtualScreenDisconnected(TEST_VIRTUAL_SCREEN_ID);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.virtualToPhysicalScreenMap_.clear();
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnVirtualScreenConnected003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_ASSOCIATED_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    multiProcessManager_->stateStore_.InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    constexpr ScreenId OLD_PHYSICAL_SCREEN_ID = 500;
    multiProcessManager_->stateStore_.InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, OLD_PHYSICAL_SCREEN_ID);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    sptr<RSIServiceToRenderConnection> oldScreenConn = nullptr;
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = oldScreenConn;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.virtualToPhysicalScreenMap_.clear();
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnVirtualScreenConnected004, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_ASSOCIATED_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    renderProcess->runner_ = runner;
    renderProcess->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderProcess->renderPipeline_ = pipeline;
    auto renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*renderProcess);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderProcessAgent, renderPipelineAgent);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
    EXPECT_EQ(multiProcessManager_->stateStore_.processToScreenOutputMap_.count(TEST_PROCESS_UNIQUE_ID), 1u);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.processToScreenOutputMap_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.virtualToPhysicalScreenMap_.clear();
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnVirtualScreenConnected005, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_ASSOCIATED_SCREEN_ID, TEST_GROUP_ID_2)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    renderProcess->runner_ = runner;
    renderProcess->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderProcess->renderPipeline_ = pipeline;
    auto renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*renderProcess);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderProcessAgent, renderPipelineAgent);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
    EXPECT_EQ(multiProcessManager_->stateStore_.processToScreenOutputMap_.count(TEST_PROCESS_UNIQUE_ID), 0u);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.virtualToPhysicalScreenMap_.clear();
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: GetPendingScreenProperty002
 * @tc.desc: Test GetPendingScreenProperty returns nullptr when extract fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetPendingScreenProperty002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    multiProcessManager_->stateStore_.renderProcessReadyPromises_[TEST_PROCESS_UNIQUE_ID] = std::promise<bool>();
    auto result = multiProcessManager_->GetPendingScreenProperty(TEST_PID);
    ASSERT_EQ(result, nullptr);
    multiProcessManager_->stateStore_.renderProcessReadyPromises_.erase(TEST_PROCESS_UNIQUE_ID);
}

/**
 * @tc.name: HandleRenderProcessDeath001
 * @tc.desc: Test HandleRenderProcessDeath clears all connections
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, HandleRenderProcessDeath001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    multiProcessManager_->stateStore_.connectToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    multiProcessManager_->stateStore_.composerToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert({TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID});
    multiProcessManager_->HandleRenderProcessDeath(TEST_PROCESS_UNIQUE_ID);
    EXPECT_EQ(multiProcessManager_->stateStore_.serviceToRenderConnections_.count(TEST_PROCESS_UNIQUE_ID), 0u);
    EXPECT_EQ(multiProcessManager_->stateStore_.connectToRenderConnections_.count(TEST_PROCESS_UNIQUE_ID), 0u);
    EXPECT_EQ(multiProcessManager_->stateStore_.composerToRenderConnections_.count(TEST_PROCESS_UNIQUE_ID), 0u);
    EXPECT_EQ(multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.count(TEST_GROUP_ID), 0u);
    EXPECT_EQ(multiProcessManager_->stateStore_.processToScreenOutputMap_.count(TEST_PROCESS_UNIQUE_ID), 0u);
}

/**
 * @tc.name: HandleRenderProcessDeath002
 * @tc.desc: Test HandleRenderProcessDeath sets promise to false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, HandleRenderProcessDeath002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    multiProcessManager_->stateStore_.renderProcessReadyPromises_[TEST_PROCESS_UNIQUE_ID] = std::promise<bool>();
    multiProcessManager_->HandleRenderProcessDeath(TEST_PROCESS_UNIQUE_ID);
    EXPECT_EQ(multiProcessManager_->stateStore_.renderProcessReadyPromises_.count(TEST_PROCESS_UNIQUE_ID), 0u);
}

/**
 * @tc.name: SetRenderProcessReadyPromise001
 * @tc.desc: Test SetRenderProcessReadyPromise returns false when promise not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, SetRenderProcessReadyPromise001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    localManager->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSIServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    sptr<RSIConnectToRenderProcess> connectConn =
        sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    auto result = localManager->SetRenderProcessReadyPromise(TEST_PID, serviceConn, connectConn);
    EXPECT_FALSE(result);
    localManager->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    localManager->stateStore_.connectToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
}

/**
 * @tc.name: HandleExistingGroup001
 * @tc.desc: Test HandleExistingGroup returns nullptr when serviceToRenderConnection is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, HandleExistingGroup001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    multiProcessManager_->stateStore_.composerToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto result = multiProcessManager_->OnScreenConnected(TEST_SCREEN_ID, output, property);
    EXPECT_EQ(result, nullptr);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.composerToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: HandleExistingGroup002
 * @tc.desc: Test HandleExistingGroup returns nullptr when composerToRenderConn is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, HandleExistingGroup002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    multiProcessManager_->stateStore_.composerToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto result = multiProcessManager_->OnScreenConnected(TEST_SCREEN_ID, output, property);
    EXPECT_EQ(result, nullptr);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.composerToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, HandleExistingGroup003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    multiProcessManager_->stateStore_.composerToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    multiProcessManager_->stateStore_.connectToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto result = multiProcessManager_->OnScreenConnected(TEST_SCREEN_ID, output, property);
    EXPECT_EQ(result, nullptr);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.composerToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.connectToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, HandleExistingGroup004, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    multiProcessManager_->stateStore_.composerToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    multiProcessManager_->stateStore_.connectToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto result = multiProcessManager_->OnScreenConnected(TEST_SCREEN_ID, output, property);
    EXPECT_EQ(result, nullptr);
    multiProcessManager_->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.composerToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.connectToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, HandleExistingGroup005, TestSize.Level1)
{
    auto localRenderService = std::make_unique<RSRenderService>();
    auto localManager = std::make_unique<RSMultiRenderProcessManager>(*localRenderService);
    ASSERT_NE(localManager, nullptr);
    localRenderService->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    localManager->stateStore_.groupIdToRenderProcessUniqueId_.insert_or_assign(
        TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    localManager->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    localManager->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    auto composerConn = sptr<RSComposerToRenderConnection>::MakeSptr();
    localManager->stateStore_.composerToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = composerConn;
    localManager->stateStore_.connectToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    auto property = sptr<RSScreenProperty>::MakeSptr();
    localManager->stateStore_.AddScreenOutputToProcess(TEST_PROCESS_UNIQUE_ID, TEST_SCREEN_ID, output);
    EXPECT_EQ(localManager->stateStore_.processToScreenOutputMap_.count(TEST_PROCESS_UNIQUE_ID), 1u);
    localManager->stateStore_.groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    localManager->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    localManager->stateStore_.composerToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    localManager->stateStore_.connectToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    localManager->stateStore_.processToScreenOutputMap_.erase(TEST_PROCESS_UNIQUE_ID);
    localRenderService->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnRemoteDiedTokenCannotBePromoted001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    sptr<IRemoteObject> remoteObj = nullptr;
    wptr<IRemoteObject> weakRemote = remoteObj;
    auto deathRecipient = sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient>::MakeSptr(
        TEST_PROCESS_UNIQUE_ID, wptr<RSMultiRenderProcessManager>(localManager));
    deathRecipient->OnRemoteDied(weakRemote);
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnRemoteDiedManagerDestroyed001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient> deathRecipient =
        sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient>::MakeSptr(
            TEST_PROCESS_UNIQUE_ID, wptr<RSMultiRenderProcessManager>(localManager));
    sptr<IRemoteObject> remoteObj = nullptr;
    wptr<IRemoteObject> weakRemote = remoteObj;
    deathRecipient->OnRemoteDied(weakRemote);
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnRemoteDiedNormalPath001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    localManager->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient> deathRecipient =
        sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient>::MakeSptr(
            TEST_PROCESS_UNIQUE_ID, wptr<RSMultiRenderProcessManager>(localManager));
    localManager->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    localManager->stateStore_.connectToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    localManager->stateStore_.composerToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = nullptr;
    localManager->stateStore_.groupIdToRenderProcessUniqueId_.insert({TEST_GROUP_ID, TEST_PROCESS_UNIQUE_ID});
    wptr<IRemoteObject> weakRemote = serviceConn->AsObject();
    deathRecipient->OnRemoteDied(weakRemote);
    EXPECT_EQ(localManager->stateStore_.serviceToRenderConnections_.count(TEST_PROCESS_UNIQUE_ID), 0u);
    EXPECT_EQ(localManager->stateStore_.connectToRenderConnections_.count(TEST_PROCESS_UNIQUE_ID), 0u);
    EXPECT_EQ(localManager->stateStore_.composerToRenderConnections_.count(TEST_PROCESS_UNIQUE_ID), 0u);
    EXPECT_EQ(localManager->stateStore_.groupIdToRenderProcessUniqueId_.count(TEST_GROUP_ID), 0u);
    EXPECT_EQ(localManager->stateStore_.processToScreenOutputMap_.count(TEST_PROCESS_UNIQUE_ID), 0u);
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnVirtualScreenConnected006, TestSize.Level1)
{
    auto localRenderService = std::make_unique<RSRenderService>();
    auto localManager = std::make_unique<RSMultiRenderProcessManager>(*localRenderService);
    ASSERT_NE(localManager, nullptr);
    localRenderService->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    localManager->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
}

HWTEST_F(RSMultiRenderProcessManagerTest, SetRenderProcessReadyPromise002Success, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->stateStore_.processToScreenOutputMap_[TEST_PROCESS_UNIQUE_ID].emplace_back(
        TEST_SCREEN_ID, output);
    multiProcessManager_->stateStore_.renderProcessReadyPromises_[TEST_PROCESS_UNIQUE_ID] = std::promise<bool>();
    auto& promiseRef = multiProcessManager_->stateStore_.renderProcessReadyPromises_[TEST_PROCESS_UNIQUE_ID];
    auto future = promiseRef.get_future();
    promiseRef.set_value(true);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSIServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    sptr<RSIConnectToRenderProcess> connectConn =
        sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->stateStore_.serviceToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = serviceConn;
    multiProcessManager_->stateStore_.connectToRenderConnections_[TEST_PROCESS_UNIQUE_ID] = connectConn;
    multiProcessManager_->stateStore_.renderProcessReadyPromises_.erase(TEST_PROCESS_UNIQUE_ID);
    EXPECT_EQ(future.get(), true);
    EXPECT_EQ(multiProcessManager_->stateStore_.serviceToRenderConnections_.count(TEST_PROCESS_UNIQUE_ID), 1u);
    EXPECT_EQ(multiProcessManager_->stateStore_.connectToRenderConnections_.count(TEST_PROCESS_UNIQUE_ID), 1u);
    EXPECT_EQ(multiProcessManager_->stateStore_.renderProcessReadyPromises_.count(TEST_PROCESS_UNIQUE_ID), 0u);
    multiProcessManager_->stateStore_.serviceToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
    multiProcessManager_->stateStore_.connectToRenderConnections_.erase(TEST_PROCESS_UNIQUE_ID);
}
} // namespace OHOS::Rosen
