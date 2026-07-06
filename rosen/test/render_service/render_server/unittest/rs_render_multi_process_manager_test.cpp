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
constexpr uint64_t TEST_TIMESTAMP = 100;
constexpr uint64_t TEST_TIMESTAMP_2 = 200;

const ProcessUniqueId TEST_TOKEN{TEST_PID, TEST_TIMESTAMP};
const ProcessUniqueId TEST_TOKEN_2{TEST_PID_2, TEST_TIMESTAMP_2};
const ProcessUniqueId TEST_TOKEN_3{TEST_PID_3, TEST_TIMESTAMP};
} // namespace

class RSMultiRenderProcessManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static RSRenderService* renderService_;
    static RSMultiRenderProcessManager* multiProcessManager_;
};

RSRenderService* RSMultiRenderProcessManagerTest::renderService_ = nullptr;
RSMultiRenderProcessManager* RSMultiRenderProcessManagerTest::multiProcessManager_ = nullptr;

void RSMultiRenderProcessManagerTest::SetUpTestCase()
{
    renderService_ = new RSRenderService();
    multiProcessManager_ = new RSMultiRenderProcessManager(*renderService_);
}

void RSMultiRenderProcessManagerTest::TearDownTestCase()
{
    if (multiProcessManager_ != nullptr) {
        multiProcessManager_->deathRecipients_.clear();
        delete multiProcessManager_;
        multiProcessManager_ = nullptr;
    }
    delete renderService_;
    renderService_ = nullptr;
}

void RSMultiRenderProcessManagerTest::SetUp() {}
void RSMultiRenderProcessManagerTest::TearDown()
{
    if (multiProcessManager_ != nullptr) {
        multiProcessManager_->serviceToRenderConnections_.clear();
        multiProcessManager_->connectToRenderConnections_.clear();
        multiProcessManager_->composerToRenderConnections_.clear();
        multiProcessManager_->groupIdToRenderProcessUniqueId_.clear();
        multiProcessManager_->processToScreenOutputMap_.clear();
        multiProcessManager_->virtualToPhysicalScreenMap_.clear();
        multiProcessManager_->pendingScreenConnectInfos_.clear();
        multiProcessManager_->renderProcessReadyPromises_.clear();
        multiProcessManager_->subprocessDeathTimes_.clear();
        multiProcessManager_->validRenderProcessUniqueIds_.clear();
        multiProcessManager_->deathRecipients_.clear();
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
    auto manager = new RSMultiRenderProcessManager(*renderService);

    ASSERT_NE(manager, nullptr);

    delete manager;
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
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    sptr<IRSComposerToRenderConnection> conn = nullptr;
    multiProcessManager_->RecordComposerToRenderConnection(TEST_PID, conn);
    auto stored = multiProcessManager_->composerToRenderConnections_.find(TEST_TOKEN);
    ASSERT_NE(stored, multiProcessManager_->composerToRenderConnections_.end());
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
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    sptr<IRSComposerToRenderConnection> conn1 = nullptr;
    multiProcessManager_->RecordComposerToRenderConnection(TEST_PID, conn1);
    sptr<IRSComposerToRenderConnection> conn2 = nullptr;
    multiProcessManager_->RecordComposerToRenderConnection(TEST_PID, conn2);
    EXPECT_EQ(multiProcessManager_->composerToRenderConnections_.count(TEST_TOKEN), 1u);
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
    auto result = multiProcessManager_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    EXPECT_FALSE(result.has_value());
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    result = multiProcessManager_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetPid(), TEST_PID);
}

/**
 * @tc.name: UpdateAndGetGroupIdToPid002
 * @tc.desc: Test UpdateGroupIdToRenderProcessUniqueId overwrites existing
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, UpdateAndGetGroupIdToPid002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN_2);
    auto result = multiProcessManager_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetPid(), TEST_PID_2);
}

/**
 * @tc.name: GetServiceToRenderConns001
 * @tc.desc: Test GetServiceToRenderConns returns empty initially
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConns001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    EXPECT_EQ(multiProcessManager_->serviceToRenderConnections_.size(), 0u);
}

/**
 * @tc.name: GetServiceToRenderConns002
 * @tc.desc: Test GetServiceToRenderConns returns stored connections
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConns002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = nullptr;
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN_2] = nullptr;
    auto conns = multiProcessManager_->GetServiceToRenderConns();
    EXPECT_EQ(conns.size(), 2u);
    multiProcessManager_->serviceToRenderConnections_.clear();
}

/**
 * @tc.name: GetServiceToRenderConnByUniqueId001
 * @tc.desc: Test GetServiceToRenderConnByUniqueId returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConnByUniqueId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetServiceToRenderConnByUniqueId(TEST_TOKEN);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GetServiceToRenderConnByUniqueId002
 * @tc.desc: Test GetServiceToRenderConnByUniqueId returns connection when found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConnByUniqueId002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    sptr<RSIServiceToRenderConnection> storedConn = nullptr;
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = storedConn;
    auto conn = multiProcessManager_->GetServiceToRenderConnByUniqueId(TEST_TOKEN);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
}

/**
 * @tc.name: GetServiceToRenderConnByUniqueIdLocked001
 * @tc.desc: Test GetServiceToRenderConnByUniqueIdLocked returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConnByUniqueIdLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetServiceToRenderConnByUniqueIdLocked(TEST_TOKEN);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GetConnectToRenderConnByUniqueId001
 * @tc.desc: Test GetConnectToRenderConnByUniqueId returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetConnectToRenderConnByUniqueId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetConnectToRenderConnByUniqueId(TEST_TOKEN);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GetConnectToRenderConnByUniqueIdLocked001
 * @tc.desc: Test GetConnectToRenderConnByUniqueIdLocked returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetConnectToRenderConnByUniqueIdLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetConnectToRenderConnByUniqueIdLocked(TEST_TOKEN);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GotComposerToRenderConnByUniqueId001
 * @tc.desc: Test GotComposerToRenderConnByUniqueId throws when pid not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GotComposerToRenderConnByUniqueId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    EXPECT_EQ(multiProcessManager_->composerToRenderConnections_.count(TEST_TOKEN), 0u);
}

/**
 * @tc.name: GotServiceToRenderConnByUniqueId001
 * @tc.desc: Test GotServiceToRenderConnByUniqueId throws when pid not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GotServiceToRenderConnByUniqueId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetServiceToRenderConnByUniqueId(TEST_TOKEN);
    ASSERT_EQ(conn, nullptr);
}

/**
 * @tc.name: GotConnectToRenderConnByUniqueId001
 * @tc.desc: Test GotConnectToRenderConnByUniqueId throws when pid not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GotConnectToRenderConnByUniqueId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetConnectToRenderConnByUniqueId(TEST_TOKEN);
    ASSERT_EQ(conn, nullptr);
}

/**
 * @tc.name: InsertVirtualToPhysicalScreenMap001
 * @tc.desc: Test InsertVirtualToPhysicalScreenMap new insert returns INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, InsertVirtualToPhysicalScreenMap001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto oldId = multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    EXPECT_EQ(oldId, INVALID_SCREEN_ID);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
}

/**
 * @tc.name: InsertVirtualToPhysicalScreenMap002
 * @tc.desc: Test InsertVirtualToPhysicalScreenMap update with different associated returns old
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, InsertVirtualToPhysicalScreenMap002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    constexpr ScreenId NEW_ASSOCIATED_ID = 500;
    auto oldId = multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, NEW_ASSOCIATED_ID);
    EXPECT_EQ(oldId, TEST_ASSOCIATED_SCREEN_ID);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
}

/**
 * @tc.name: InsertVirtualToPhysicalScreenMap003
 * @tc.desc: Test InsertVirtualToPhysicalScreenMap with same associated returns INVALID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, InsertVirtualToPhysicalScreenMap003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    auto oldId = multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    EXPECT_EQ(oldId, INVALID_SCREEN_ID);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
}

/**
 * @tc.name: DeleteVirtualToPhysicalScreenMap001
 * @tc.desc: Test DeleteVirtualToPhysicalScreenMap returns nullopt when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, DeleteVirtualToPhysicalScreenMap001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto result = multiProcessManager_->DeleteVirtualToPhysicalScreenMap(TEST_VIRTUAL_SCREEN_ID);
    EXPECT_FALSE(result.has_value());
}

/**
 * @tc.name: DeleteVirtualToPhysicalScreenMap002
 * @tc.desc: Test DeleteVirtualToPhysicalScreenMap returns mapped value when found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, DeleteVirtualToPhysicalScreenMap002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    auto result = multiProcessManager_->DeleteVirtualToPhysicalScreenMap(TEST_VIRTUAL_SCREEN_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), TEST_ASSOCIATED_SCREEN_ID);
    auto result2 = multiProcessManager_->DeleteVirtualToPhysicalScreenMap(TEST_VIRTUAL_SCREEN_ID);
    EXPECT_FALSE(result2.has_value());
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
    auto result = multiProcessManager_->FindVirtualToPhysicalScreenMap(TEST_VIRTUAL_SCREEN_ID);
    EXPECT_EQ(result, TEST_VIRTUAL_SCREEN_ID);
}

/**
 * @tc.name: FindVirtualToPhysicalScreenMap002
 * @tc.desc: Test FindVirtualToPhysicalScreenMap returns mapped value when found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, FindVirtualToPhysicalScreenMap002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    auto result = multiProcessManager_->FindVirtualToPhysicalScreenMap(TEST_VIRTUAL_SCREEN_ID);
    EXPECT_EQ(result, TEST_ASSOCIATED_SCREEN_ID);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
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
 * @tc.name: CheckGroupIdByScreenId001
 * @tc.desc: Test CheckGroupIdByScreenId returns nullopt when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, CheckGroupIdByScreenId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .Build();
    auto result = multiProcessManager_->CheckGroupIdByScreenId(TEST_SCREEN_ID);
    EXPECT_FALSE(result.has_value());
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: CheckGroupIdByScreenId002
 * @tc.desc: Test CheckGroupIdByScreenId returns groupId when found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, CheckGroupIdByScreenId002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    auto result = multiProcessManager_->CheckGroupIdByScreenId(TEST_SCREEN_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), TEST_GROUP_ID);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->OnScreenDisconnected(TEST_SCREEN_ID);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = nullptr;
    multiProcessManager_->OnScreenRefresh(TEST_SCREEN_ID);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
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
    multiProcessManager_->InsertVirtualToPhysicalScreenMap(
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    auto conn = multiProcessManager_->GetServiceToRenderConn(TEST_SCREEN_ID);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    auto conn = multiProcessManager_->GetConnectToRenderConnection(TEST_SCREEN_ID);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
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
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->pendingScreenConnectInfos_[TEST_TOKEN] =
        { TEST_SCREEN_ID, property };
    multiProcessManager_->renderProcessReadyPromises_[TEST_TOKEN] = std::promise<bool>();
    auto result = multiProcessManager_->GetPendingScreenProperty(TEST_PID);
    ASSERT_NE(result, nullptr);
    multiProcessManager_->renderProcessReadyPromises_.erase(TEST_TOKEN);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = nullptr;
    sptr<ScreenPropertyBase> property = sptr<ScreenProperty<bool>>::MakeSptr();
    multiProcessManager_->OnScreenPropertyChanged(
        TEST_SCREEN_ID, ScreenPropertyType::IS_VIRTUAL, property);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    sptr<RSIServiceToRenderConnection> conn = nullptr;
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = conn;
    multiProcessManager_->OnScreenDisconnected(TEST_SCREEN_ID);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    sptr<RSIServiceToRenderConnection> conn = nullptr;
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = conn;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    sptr<RSIServiceToRenderConnection> conn = nullptr;
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = conn;
    multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    multiProcessManager_->OnVirtualScreenDisconnected(TEST_VIRTUAL_SCREEN_ID);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    constexpr ScreenId OLD_PHYSICAL_SCREEN_ID = 500;
    multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, OLD_PHYSICAL_SCREEN_ID);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    sptr<RSIServiceToRenderConnection> oldScreenConn = nullptr;
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = oldScreenConn;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
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
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_.count(TEST_TOKEN), 1u);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->processToScreenOutputMap_.erase(TEST_TOKEN);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
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
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->OnVirtualScreenConnected(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID, property);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_.count(TEST_TOKEN), 0u);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
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
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    multiProcessManager_->renderProcessReadyPromises_[TEST_TOKEN] = std::promise<bool>();
    auto result = multiProcessManager_->GetPendingScreenProperty(TEST_PID);
    ASSERT_EQ(result, nullptr);
    multiProcessManager_->renderProcessReadyPromises_.erase(TEST_TOKEN);
}

/**
 * @tc.name: GetComposerToRenderConnByUniqueId001
 * @tc.desc: Test GetComposerToRenderConnByUniqueId returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetComposerToRenderConnByUniqueId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetComposerToRenderConnByUniqueId(TEST_TOKEN);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GetComposerToRenderConnByUniqueId002
 * @tc.desc: Test GetComposerToRenderConnByUniqueId returns connection when found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetComposerToRenderConnByUniqueId002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    sptr<IRSComposerToRenderConnection> storedConn = nullptr;
    multiProcessManager_->composerToRenderConnections_[TEST_TOKEN] = storedConn;
    auto conn = multiProcessManager_->GetComposerToRenderConnByUniqueId(TEST_TOKEN);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->composerToRenderConnections_.erase(TEST_TOKEN);
}

/**
 * @tc.name: GetComposerToRenderConnByUniqueIdLocked001
 * @tc.desc: Test GetComposerToRenderConnByUniqueIdLocked returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetComposerToRenderConnByUniqueIdLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetComposerToRenderConnByUniqueIdLocked(TEST_TOKEN);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GetComposerToRenderConnByUniqueIdLocked002
 * @tc.desc: Test GetComposerToRenderConnByUniqueIdLocked returns nullptr when found but value is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetComposerToRenderConnByUniqueIdLocked002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    sptr<IRSComposerToRenderConnection> storedConn = nullptr;
    multiProcessManager_->composerToRenderConnections_[TEST_TOKEN] = storedConn;
    auto conn = multiProcessManager_->GetComposerToRenderConnByUniqueIdLocked(TEST_TOKEN);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->composerToRenderConnections_.erase(TEST_TOKEN);
}

/**
 * @tc.name: GetComposerToRenderConnByUniqueIdLocked003
 * @tc.desc: Test GetComposerToRenderConnByUniqueIdLocked returns non-nullptr when found with valid value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetComposerToRenderConnByUniqueIdLocked003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    sptr<IRSComposerToRenderConnection> storedConn = sptr<RSComposerToRenderConnection>::MakeSptr();
    multiProcessManager_->composerToRenderConnections_[TEST_TOKEN] = storedConn;
    auto conn = multiProcessManager_->GetComposerToRenderConnByUniqueIdLocked(TEST_TOKEN);
    ASSERT_NE(conn, nullptr);
    EXPECT_EQ(conn, storedConn);
    multiProcessManager_->composerToRenderConnections_.erase(TEST_TOKEN);
}

/**
 * @tc.name: CheckAndHandleSubprocessDeathOverflow001
 * @tc.desc: Test CheckAndHandleSubprocessDeathOverflow when empty deque
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, CheckAndHandleSubprocessDeathOverflow001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->subprocessDeathTimes_.clear();
    multiProcessManager_->CheckAndHandleSubprocessDeathOverflow();
    EXPECT_EQ(multiProcessManager_->subprocessDeathTimes_.size(), 1u);
    multiProcessManager_->subprocessDeathTimes_.clear();
}

/**
 * @tc.name: CheckAndHandleSubprocessDeathOverflow002
 * @tc.desc: Test CheckAndHandleSubprocessDeathOverflow when deque has old entries
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, CheckAndHandleSubprocessDeathOverflow002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto oldTime = std::chrono::steady_clock::now() - std::chrono::seconds(200);
    multiProcessManager_->subprocessDeathTimes_.push_back(oldTime);
    multiProcessManager_->CheckAndHandleSubprocessDeathOverflow();
    EXPECT_EQ(multiProcessManager_->subprocessDeathTimes_.size(), 1u);
    multiProcessManager_->subprocessDeathTimes_.clear();
}

/**
 * @tc.name: AddScreenOutputToProcess001
 * @tc.desc: Test AddScreenOutputToProcess adds screen output
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, AddScreenOutputToProcess001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->AddScreenOutputToProcess(TEST_TOKEN, TEST_SCREEN_ID, output);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_.count(TEST_TOKEN), 1u);
    multiProcessManager_->processToScreenOutputMap_.erase(TEST_TOKEN);
}

/**
 * @tc.name: RemoveScreenOutputFromProcess001
 * @tc.desc: Test RemoveScreenOutputFromProcess when pid not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, RemoveScreenOutputFromProcess001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->RemoveScreenOutputFromProcess(TEST_TOKEN, TEST_SCREEN_ID);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_.count(TEST_TOKEN), 0u);
}

/**
 * @tc.name: RemoveScreenOutputFromProcess002
 * @tc.desc: Test RemoveScreenOutputFromProcess when screenId not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, RemoveScreenOutputFromProcess002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->processToScreenOutputMap_[TEST_TOKEN].emplace_back(TEST_SCREEN_ID, output);
    multiProcessManager_->RemoveScreenOutputFromProcess(TEST_TOKEN, TEST_VIRTUAL_SCREEN_ID);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_[TEST_TOKEN].size(), 1u);
    multiProcessManager_->processToScreenOutputMap_.erase(TEST_TOKEN);
}

/**
 * @tc.name: RemoveScreenOutputFromProcess003
 * @tc.desc: Test RemoveScreenOutputFromProcess removes and clears pid when empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, RemoveScreenOutputFromProcess003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->processToScreenOutputMap_[TEST_TOKEN].emplace_back(TEST_SCREEN_ID, output);
    multiProcessManager_->RemoveScreenOutputFromProcess(TEST_TOKEN, TEST_SCREEN_ID);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_.count(TEST_TOKEN), 0u);
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
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = nullptr;
    multiProcessManager_->connectToRenderConnections_[TEST_TOKEN] = nullptr;
    multiProcessManager_->composerToRenderConnections_[TEST_TOKEN] = nullptr;
    multiProcessManager_->groupIdToRenderProcessUniqueId_.insert({TEST_GROUP_ID, TEST_TOKEN});
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->processToScreenOutputMap_[TEST_TOKEN].emplace_back(TEST_SCREEN_ID, output);
    multiProcessManager_->HandleRenderProcessDeath(TEST_TOKEN);
    EXPECT_EQ(multiProcessManager_->serviceToRenderConnections_.count(TEST_TOKEN), 0u);
    EXPECT_EQ(multiProcessManager_->connectToRenderConnections_.count(TEST_TOKEN), 0u);
    EXPECT_EQ(multiProcessManager_->composerToRenderConnections_.count(TEST_TOKEN), 0u);
    EXPECT_EQ(multiProcessManager_->groupIdToRenderProcessUniqueId_.count(TEST_GROUP_ID), 0u);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_.count(TEST_TOKEN), 0u);
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
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    multiProcessManager_->renderProcessReadyPromises_[TEST_TOKEN] = std::promise<bool>();
    multiProcessManager_->HandleRenderProcessDeath(TEST_TOKEN);
    EXPECT_EQ(multiProcessManager_->renderProcessReadyPromises_.count(TEST_TOKEN), 0u);
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
    localManager->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSIServiceToRenderConnection> serviceConn = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    sptr<RSIConnectToRenderProcess> connectConn = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    auto result = localManager->SetRenderProcessReadyPromise(TEST_PID, serviceConn, connectConn);
    EXPECT_FALSE(result);
    localManager->serviceToRenderConnections_.erase(TEST_TOKEN);
    localManager->connectToRenderConnections_.erase(TEST_TOKEN);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = nullptr;
    multiProcessManager_->composerToRenderConnections_[TEST_TOKEN] = nullptr;
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto result = multiProcessManager_->OnScreenConnected(TEST_SCREEN_ID, output, property);
    EXPECT_EQ(result, nullptr);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->composerToRenderConnections_.erase(TEST_TOKEN);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    multiProcessManager_->composerToRenderConnections_[TEST_TOKEN] = nullptr;
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto result = multiProcessManager_->OnScreenConnected(TEST_SCREEN_ID, output, property);
    EXPECT_EQ(result, nullptr);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->composerToRenderConnections_.erase(TEST_TOKEN);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: HandleExistingGroup003
 * @tc.desc: Test HandleExistingGroup returns nullptr when connectToRender is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, HandleExistingGroup003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    multiProcessManager_->composerToRenderConnections_[TEST_TOKEN] = nullptr;
    multiProcessManager_->connectToRenderConnections_[TEST_TOKEN] = nullptr;
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto result = multiProcessManager_->OnScreenConnected(TEST_SCREEN_ID, output, property);
    EXPECT_EQ(result, nullptr);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->composerToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->connectToRenderConnections_.erase(TEST_TOKEN);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: HandleExistingGroup004
 * @tc.desc: Test HandleExistingGroup returns nullptr when NotifyScreenConnectInfoToRender fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, HandleExistingGroup004, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    multiProcessManager_->composerToRenderConnections_[TEST_TOKEN] = nullptr;
    multiProcessManager_->connectToRenderConnections_[TEST_TOKEN] = nullptr;
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    auto result = multiProcessManager_->OnScreenConnected(TEST_SCREEN_ID, output, property);
    EXPECT_EQ(result, nullptr);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->composerToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->connectToRenderConnections_.erase(TEST_TOKEN);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: HandleExistingGroup005
 * @tc.desc: Test HandleExistingGroup AddScreenOutputToProcess is called
 * @tc.type: FUNC
 * @tc.require:
 */
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
    localManager->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    localManager->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    localManager->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    auto composerConn = sptr<RSComposerToRenderConnection>::MakeSptr();
    localManager->composerToRenderConnections_[TEST_TOKEN] = composerConn;
    localManager->connectToRenderConnections_[TEST_TOKEN] = nullptr;
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    localManager->AddScreenOutputToProcess(TEST_TOKEN, TEST_SCREEN_ID, output);
    EXPECT_EQ(localManager->processToScreenOutputMap_.count(TEST_TOKEN), 1u);
    localManager->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    localManager->serviceToRenderConnections_.erase(TEST_TOKEN);
    localManager->composerToRenderConnections_.erase(TEST_TOKEN);
    localManager->connectToRenderConnections_.erase(TEST_TOKEN);
    localManager->processToScreenOutputMap_.erase(TEST_TOKEN);
    localRenderService->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: OnRemoteDiedTokenCannotBePromoted001
 * @tc.desc: Test OnRemoteDied when token.promote() returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnRemoteDiedTokenCannotBePromoted001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    sptr<IRemoteObject> remoteObj = nullptr;
    wptr<IRemoteObject> token = remoteObj;
    auto deathRecipient = sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient>::MakeSptr(
        TEST_TOKEN, wptr<RSMultiRenderProcessManager>(localManager));
    deathRecipient->OnRemoteDied(token);
}

/**
 * @tc.name: OnRemoteDiedManagerDestroyed001
 * @tc.desc: Test OnRemoteDied when manager has been destroyed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, OnRemoteDiedManagerDestroyed001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient> deathRecipient =
        sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient>::MakeSptr(
            TEST_TOKEN, wptr<RSMultiRenderProcessManager>(localManager));
    sptr<IRemoteObject> remoteObj = nullptr;
    wptr<IRemoteObject> token = remoteObj;
    deathRecipient->OnRemoteDied(token);
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnRemoteDiedNormalPath001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    localManager->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient> deathRecipient =
        sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient>::MakeSptr(
            TEST_TOKEN, wptr<RSMultiRenderProcessManager>(localManager));
    localManager->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    localManager->connectToRenderConnections_[TEST_TOKEN] = nullptr;
    localManager->composerToRenderConnections_[TEST_TOKEN] = nullptr;
    localManager->groupIdToRenderProcessUniqueId_.insert({TEST_GROUP_ID, TEST_TOKEN});
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    localManager->processToScreenOutputMap_[TEST_TOKEN].emplace_back(TEST_SCREEN_ID, output);
    wptr<IRemoteObject> token = serviceConn->AsObject();
    deathRecipient->OnRemoteDied(token);
    EXPECT_EQ(localManager->serviceToRenderConnections_.count(TEST_TOKEN), 0u);
    EXPECT_EQ(localManager->connectToRenderConnections_.count(TEST_TOKEN), 0u);
    EXPECT_EQ(localManager->composerToRenderConnections_.count(TEST_TOKEN), 0u);
    EXPECT_EQ(localManager->groupIdToRenderProcessUniqueId_.count(TEST_GROUP_ID), 0u);
    EXPECT_EQ(localManager->processToScreenOutputMap_.count(TEST_TOKEN), 0u);
}

HWTEST_F(RSMultiRenderProcessManagerTest, RegisterDeathRecipientAddFailed001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    localManager->RegisterDeathRecipient(TEST_TOKEN_3, serviceConn->AsObject());
    EXPECT_EQ(localManager->deathRecipients_.count(TEST_TOKEN_3), 0u);
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

HWTEST_F(RSMultiRenderProcessManagerTest, GetRenderProcessUniqueIdByGroupId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto result = multiProcessManager_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    EXPECT_FALSE(result.has_value());
}

HWTEST_F(RSMultiRenderProcessManagerTest, GetRenderProcessUniqueIdByGroupId002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    auto result = multiProcessManager_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetPid(), TEST_PID);
    EXPECT_EQ(result.value().GetTimestamp(), TEST_TIMESTAMP);
}

HWTEST_F(RSMultiRenderProcessManagerTest, GetRenderProcessUniqueIdByGroupIdLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto result = multiProcessManager_->GetRenderProcessUniqueIdByGroupIdLocked(TEST_GROUP_ID);
    EXPECT_FALSE(result.has_value());
}

HWTEST_F(RSMultiRenderProcessManagerTest, GetRenderProcessUniqueIdByGroupIdLocked002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    auto result = multiProcessManager_->GetRenderProcessUniqueIdByGroupIdLocked(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetPid(), TEST_PID);
}

HWTEST_F(RSMultiRenderProcessManagerTest, UpdateGroupIdToRenderProcessUniqueIdLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueIdLocked(TEST_GROUP_ID, TEST_TOKEN);
    auto result = multiProcessManager_->GetRenderProcessUniqueIdByGroupIdLocked(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetPid(), TEST_PID);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueIdLocked(TEST_GROUP_ID, TEST_TOKEN_2);
    result = multiProcessManager_->GetRenderProcessUniqueIdByGroupIdLocked(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetPid(), TEST_PID_2);
}

HWTEST_F(RSMultiRenderProcessManagerTest, RemoveGroupIdByRenderProcessUniqueId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->RemoveGroupIdByRenderProcessUniqueId(TEST_TOKEN);
    auto result = multiProcessManager_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    EXPECT_FALSE(result.has_value());
}

HWTEST_F(RSMultiRenderProcessManagerTest, RemoveGroupIdByRenderProcessUniqueIdLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->RemoveGroupIdByRenderProcessUniqueIdLocked(TEST_TOKEN_2);
    auto result = multiProcessManager_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
}

HWTEST_F(RSMultiRenderProcessManagerTest, RemoveGroupIdByRenderProcessUniqueIdLocked002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID_2, TEST_TOKEN);
    multiProcessManager_->RemoveGroupIdByRenderProcessUniqueIdLocked(TEST_TOKEN);
    EXPECT_FALSE(multiProcessManager_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID).has_value());
    EXPECT_FALSE(multiProcessManager_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID_2).has_value());
}

HWTEST_F(RSMultiRenderProcessManagerTest, GetValidRenderProcessUniqueIdByPid001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto result = multiProcessManager_->GetValidRenderProcessUniqueIdByPid(TEST_PID);
    EXPECT_FALSE(result.has_value());
}

HWTEST_F(RSMultiRenderProcessManagerTest, GetValidRenderProcessUniqueIdByPid002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto result = multiProcessManager_->GetValidRenderProcessUniqueIdByPid(TEST_PID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetPid(), TEST_PID);
}

HWTEST_F(RSMultiRenderProcessManagerTest, GetValidRenderProcessUniqueIdByPidLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto result = multiProcessManager_->GetValidRenderProcessUniqueIdByPidLocked(TEST_PID);
    EXPECT_FALSE(result.has_value());
}

HWTEST_F(RSMultiRenderProcessManagerTest, GetValidRenderProcessUniqueIdByPidLocked002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto result = multiProcessManager_->GetValidRenderProcessUniqueIdByPidLocked(TEST_PID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetPid(), TEST_PID);
}

HWTEST_F(RSMultiRenderProcessManagerTest, IsValidRenderProcessPid001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    EXPECT_TRUE(multiProcessManager_->IsValidRenderProcessPid(TEST_PID));
}

HWTEST_F(RSMultiRenderProcessManagerTest, AddValidRenderProcessUniqueId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    EXPECT_EQ(multiProcessManager_->validRenderProcessUniqueIds_.count(TEST_TOKEN), 1u);
}

HWTEST_F(RSMultiRenderProcessManagerTest, RemoveValidRenderProcessUniqueId001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    multiProcessManager_->RemoveValidRenderProcessUniqueId(TEST_TOKEN);
    EXPECT_EQ(multiProcessManager_->validRenderProcessUniqueIds_.count(TEST_TOKEN), 0u);
}

HWTEST_F(RSMultiRenderProcessManagerTest, IsValidRenderProcessUniqueIdLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    EXPECT_FALSE(multiProcessManager_->IsValidRenderProcessUniqueIdLocked(TEST_TOKEN));
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    EXPECT_TRUE(multiProcessManager_->IsValidRenderProcessUniqueIdLocked(TEST_TOKEN));
}

HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConnByUniqueId002Found, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    auto conn = multiProcessManager_->GetServiceToRenderConnByUniqueId(TEST_TOKEN);
    ASSERT_NE(conn, nullptr);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
}

HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConnByUniqueIdLocked002Found, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    auto conn = multiProcessManager_->GetServiceToRenderConnByUniqueIdLocked(TEST_TOKEN);
    ASSERT_NE(conn, nullptr);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
}

HWTEST_F(RSMultiRenderProcessManagerTest, GotServiceToRenderConnByUniqueId002Valid, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    auto conn = multiProcessManager_->GotServiceToRenderConnByUniqueId(TEST_TOKEN);
    ASSERT_NE(conn, nullptr);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
}

HWTEST_F(RSMultiRenderProcessManagerTest, GotConnectToRenderConnByUniqueId002Valid, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSConnectToRenderProcess> connectConn =
        sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->connectToRenderConnections_[TEST_TOKEN] = connectConn;
    auto conn = multiProcessManager_->GotConnectToRenderConnByUniqueId(TEST_TOKEN);
    ASSERT_NE(conn, nullptr);
    multiProcessManager_->connectToRenderConnections_.erase(TEST_TOKEN);
}

HWTEST_F(RSMultiRenderProcessManagerTest, GotComposerToRenderConnByUniqueId002Valid, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    sptr<IRSComposerToRenderConnection> storedConn = sptr<RSComposerToRenderConnection>::MakeSptr();
    multiProcessManager_->composerToRenderConnections_[TEST_TOKEN] = storedConn;
    auto conn = multiProcessManager_->GotComposerToRenderConnByUniqueId(TEST_TOKEN);
    ASSERT_NE(conn, nullptr);
    multiProcessManager_->composerToRenderConnections_.erase(TEST_TOKEN);
}

HWTEST_F(RSMultiRenderProcessManagerTest, AddScreenOutputToProcessLockedInvalidToken001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->AddScreenOutputToProcessLocked(TEST_TOKEN, TEST_SCREEN_ID, output);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_.count(TEST_TOKEN), 0u);
}

HWTEST_F(RSMultiRenderProcessManagerTest, AddScreenOutputToProcessLockedUpdateExisting001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto output1 = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto output2 = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->AddScreenOutputToProcessLocked(TEST_TOKEN, TEST_SCREEN_ID, output1);
    multiProcessManager_->AddScreenOutputToProcessLocked(TEST_TOKEN, TEST_SCREEN_ID, output2);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_[TEST_TOKEN].size(), 1u);
    multiProcessManager_->processToScreenOutputMap_.erase(TEST_TOKEN);
}

HWTEST_F(RSMultiRenderProcessManagerTest, RemoveScreenOutputFromProcess004NonEmpty, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto output1 = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto output2 = std::make_shared<HdiOutput>(TEST_VIRTUAL_SCREEN_ID);
    multiProcessManager_->AddScreenOutputToProcess(TEST_TOKEN, TEST_SCREEN_ID, output1);
    multiProcessManager_->AddScreenOutputToProcess(TEST_TOKEN, TEST_VIRTUAL_SCREEN_ID, output2);
    multiProcessManager_->RemoveScreenOutputFromProcess(TEST_TOKEN, TEST_SCREEN_ID);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_.count(TEST_TOKEN), 1u);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_[TEST_TOKEN].size(), 1u);
    multiProcessManager_->processToScreenOutputMap_.erase(TEST_TOKEN);
}

HWTEST_F(RSMultiRenderProcessManagerTest, HandleRenderProcessDeath003NoCallback, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    bool callbackCalled = false;
    multiProcessManager_->renderProcessDeathCallback_ =
        [&callbackCalled](std::vector<std::pair<ScreenId, std::shared_ptr<HdiOutput>>>&) {
        callbackCalled = true;
    };
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    multiProcessManager_->HandleRenderProcessDeath(TEST_TOKEN);
    EXPECT_FALSE(callbackCalled);
}

HWTEST_F(RSMultiRenderProcessManagerTest, HandleRenderProcessDeath004WithCallback, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    bool callbackCalled = false;
    multiProcessManager_->renderProcessDeathCallback_ =
        [&callbackCalled](std::vector<std::pair<ScreenId, std::shared_ptr<HdiOutput>>>&) {
        callbackCalled = true;
    };
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    multiProcessManager_->processToScreenOutputMap_[TEST_TOKEN].emplace_back(TEST_SCREEN_ID, output);
    multiProcessManager_->HandleRenderProcessDeath(TEST_TOKEN);
    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(multiProcessManager_->processToScreenOutputMap_.count(TEST_TOKEN), 0u);
}

HWTEST_F(RSMultiRenderProcessManagerTest, SetRenderProcessReadyPromise002Success, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    multiProcessManager_->renderProcessReadyPromises_[TEST_TOKEN] = std::promise<bool>();
    auto& promiseRef = multiProcessManager_->renderProcessReadyPromises_[TEST_TOKEN];
    auto future = promiseRef.get_future();
    promiseRef.set_value(true);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSIServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    sptr<RSIConnectToRenderProcess> connectConn =
        sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = serviceConn;
    multiProcessManager_->connectToRenderConnections_[TEST_TOKEN] = connectConn;
    multiProcessManager_->renderProcessReadyPromises_.erase(TEST_TOKEN);
    EXPECT_EQ(future.get(), true);
    EXPECT_EQ(multiProcessManager_->serviceToRenderConnections_.count(TEST_TOKEN), 1u);
    EXPECT_EQ(multiProcessManager_->connectToRenderConnections_.count(TEST_TOKEN), 1u);
    EXPECT_EQ(multiProcessManager_->renderProcessReadyPromises_.count(TEST_TOKEN), 0u);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->connectToRenderConnections_.erase(TEST_TOKEN);
}

HWTEST_F(RSMultiRenderProcessManagerTest, RegisterDeathRecipientLocked001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    auto pipeline = std::make_shared<RSRenderPipeline>();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);
    sptr<RSServiceToRenderConnection> serviceConn =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    auto oldRecipient = sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient>::MakeSptr(
        TEST_TOKEN, wptr<RSMultiRenderProcessManager>(localManager));
    localManager->deathRecipients_[TEST_TOKEN] = oldRecipient;
    localManager->AddValidRenderProcessUniqueId(TEST_TOKEN);
    localManager->RegisterDeathRecipientLocked(TEST_TOKEN, serviceConn->AsObject());
    EXPECT_EQ(localManager->deathRecipients_.count(TEST_TOKEN), 1u);
    localManager->deathRecipients_.clear();
}

HWTEST_F(RSMultiRenderProcessManagerTest, UnregisterDeathRecipient001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    auto oldRecipient = sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient>::MakeSptr(
        TEST_TOKEN, wptr<RSMultiRenderProcessManager>(localManager));
    localManager->deathRecipients_[TEST_TOKEN] = oldRecipient;
    localManager->UnregisterDeathRecipient(TEST_TOKEN);
    EXPECT_EQ(localManager->deathRecipients_.count(TEST_TOKEN), 0u);
}

HWTEST_F(RSMultiRenderProcessManagerTest, UnregisterDeathRecipientLocked002, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto localManager = sptr<RSMultiRenderProcessManager>::MakeSptr(*renderService);
    ASSERT_NE(localManager, nullptr);
    auto oldRecipient = sptr<RSMultiRenderProcessManager::RenderProcessDeathRecipient>::MakeSptr(
        TEST_TOKEN, wptr<RSMultiRenderProcessManager>(localManager));
    localManager->deathRecipients_[TEST_TOKEN] = oldRecipient;
    localManager->UnregisterDeathRecipientLocked(TEST_TOKEN);
    EXPECT_EQ(localManager->deathRecipients_.count(TEST_TOKEN), 0u);
}

HWTEST_F(RSMultiRenderProcessManagerTest, OnScreenPropertyChanged004, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    renderService_->renderModeConfig_ = RenderModeConfigBuilder()
        .SetIsMultiProcessModeEnabled(true)
        .SetDefaultRenderProcess(TEST_GROUP_ID)
        .SetScreenIdToGroupId(TEST_ASSOCIATED_SCREEN_ID, TEST_GROUP_ID)
        .Build();
    multiProcessManager_->InsertVirtualToPhysicalScreenMap(
        TEST_VIRTUAL_SCREEN_ID, TEST_ASSOCIATED_SCREEN_ID);
    multiProcessManager_->UpdateGroupIdToRenderProcessUniqueId(TEST_GROUP_ID, TEST_TOKEN);
    multiProcessManager_->serviceToRenderConnections_[TEST_TOKEN] = nullptr;
    sptr<ScreenPropertyBase> property = sptr<ScreenProperty<bool>>::MakeSptr();
    multiProcessManager_->OnScreenPropertyChanged(
        TEST_VIRTUAL_SCREEN_ID, ScreenPropertyType::IS_VIRTUAL, property);
    multiProcessManager_->groupIdToRenderProcessUniqueId_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_TOKEN);
    multiProcessManager_->virtualToPhysicalScreenMap_.clear();
    renderService_->renderModeConfig_ = nullptr;
}

HWTEST_F(RSMultiRenderProcessManagerTest, ProcessUniqueIdEquality001, TestSize.Level1)
{
    ProcessUniqueId token1{TEST_PID, TEST_TIMESTAMP};
    ProcessUniqueId token2{TEST_PID, TEST_TIMESTAMP};
    ProcessUniqueId token3{TEST_PID_2, TEST_TIMESTAMP};
    ProcessUniqueId token4{TEST_PID, TEST_TIMESTAMP_2};
    EXPECT_TRUE(token1 == token2);
    EXPECT_FALSE(token1 == token3);
    EXPECT_FALSE(token1 == token4);
    ProcessUniqueId::Hash hasher;
    EXPECT_EQ(hasher(token1), hasher(token2));
}

HWTEST_F(RSMultiRenderProcessManagerTest, ProcessUniqueIdInContainers001, TestSize.Level1)
{
    std::unordered_set<ProcessUniqueId, ProcessUniqueId::Hash> tokenSet;
    tokenSet.insert(TEST_TOKEN);
    tokenSet.insert(TEST_TOKEN_2);
    EXPECT_EQ(tokenSet.size(), 2u);
    EXPECT_EQ(tokenSet.count(TEST_TOKEN), 1u);
    tokenSet.erase(TEST_TOKEN);
    EXPECT_EQ(tokenSet.count(TEST_TOKEN), 0u);
    std::unordered_map<ProcessUniqueId, int, ProcessUniqueId::Hash> tokenMap;
    tokenMap[TEST_TOKEN] = 1;
    tokenMap[TEST_TOKEN_2] = 2;
    EXPECT_EQ(tokenMap.size(), 2u);
    EXPECT_EQ(tokenMap[TEST_TOKEN], 1);
    tokenMap.erase(TEST_TOKEN);
    EXPECT_EQ(tokenMap.count(TEST_TOKEN), 0u);
}

/**
 * @tc.name: GetValidRenderProcessUniqueIdByPidLocked003
 * @tc.desc: Test GetValidRenderProcessUniqueIdByPidLocked when tokens exist but pid does not match
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetValidRenderProcessUniqueIdByPidLocked003, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN);
    multiProcessManager_->AddValidRenderProcessUniqueId(TEST_TOKEN_2);
    auto result = multiProcessManager_->GetValidRenderProcessUniqueIdByPidLocked(TEST_PID_3);
    EXPECT_FALSE(result.has_value());
}
} // namespace OHOS::Rosen
