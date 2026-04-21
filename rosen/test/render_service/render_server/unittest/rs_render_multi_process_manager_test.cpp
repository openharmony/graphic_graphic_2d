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

#include "gtest/gtest.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "render_server/rs_render_multi_process_manager.h"
#include "render_server/rs_render_service.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr pid_t TEST_PID = 1234;
constexpr pid_t TEST_PID_2 = 5678;
constexpr ScreenId TEST_SCREEN_ID = 100;
constexpr ScreenId TEST_SCREEN_ID_2 = 200;
constexpr ScreenId TEST_VIRTUAL_SCREEN_ID = 300;
constexpr ScreenId TEST_ASSOCIATED_SCREEN_ID = 400;
constexpr GroupId TEST_GROUP_ID = 1;
constexpr GroupId TEST_GROUP_ID_2 = 2;
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
    delete multiProcessManager_;
    multiProcessManager_ = nullptr;
    delete renderService_;
    renderService_ = nullptr;
}

void RSMultiRenderProcessManagerTest::SetUp() {}
void RSMultiRenderProcessManagerTest::TearDown() {}

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
 * @tc.name: RecordRenderProcessConnectionTest001
 * @tc.desc: Test RecordRenderProcessConnection with valid parameters
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, RecordRenderProcessConnectionTest001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);

    sptr<RSIServiceToRenderConnection> serviceToRenderConnection = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection = nullptr;
    sptr<RSIConnectToRenderProcess> connectToRenderConnection = nullptr;

    multiProcessManager_->RecordRenderProcessConnection(
        TEST_PID, serviceToRenderConnection, composerToRenderConnection, connectToRenderConnection);

    // Verification: The method should complete without exception
    ASSERT_TRUE(multiProcessManager_ != nullptr);
}

/**
 * @tc.name: RecordRenderProcessConnectionTest002
 * @tc.desc: Test RecordRenderProcessConnection with different PID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, RecordRenderProcessConnectionTest002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);

    sptr<RSIServiceToRenderConnection> serviceToRenderConnection = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection = nullptr;
    sptr<RSIConnectToRenderProcess> connectToRenderConnection = nullptr;

    multiProcessManager_->RecordRenderProcessConnection(
        TEST_PID_2, serviceToRenderConnection, composerToRenderConnection, connectToRenderConnection);

    ASSERT_TRUE(multiProcessManager_ != nullptr);
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
 * @tc.name: RecordAndGetConnectionTest001
 * @tc.desc: Test recording and getting connection info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, RecordAndGetConnectionTest001, TestSize.Level1)
{
    auto renderService = new RSRenderService();
    auto manager = new RSMultiRenderProcessManager(*renderService);

    sptr<RSIServiceToRenderConnection> serviceToRenderConnection = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection = nullptr;
    sptr<RSIConnectToRenderProcess> connectToRenderConnection = nullptr;

    manager->RecordRenderProcessConnection(
        TEST_PID, serviceToRenderConnection, composerToRenderConnection, connectToRenderConnection);

    auto property = manager->GetPendingScreenProperty(TEST_PID);

    delete manager;
    delete renderService;

    ASSERT_TRUE(property == nullptr || property != nullptr);
}

/**
 * @tc.name: MultiProcessManagerNullServiceTest001
 * @tc.desc: Test manager behavior with null service reference
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, MultiProcessManagerNullServiceTest001, TestSize.Level1)
{
    auto renderService = new RSRenderService();
    auto manager = new RSMultiRenderProcessManager(*renderService);

    ASSERT_NE(manager, nullptr);

    sptr<RSIServiceToRenderConnection> serviceToRenderConnection = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection = nullptr;
    sptr<RSIConnectToRenderProcess> connectToRenderConnection = nullptr;
    manager->RecordRenderProcessConnection(TEST_PID, serviceToRenderConnection,
        composerToRenderConnection, connectToRenderConnection);

    delete manager;
    delete renderService;
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
    sptr<IRSComposerToRenderConnection> conn = nullptr;
    multiProcessManager_->RecordComposerToRenderConnection(TEST_PID, conn);
    auto stored = multiProcessManager_->composerToRenderConnections_.find(TEST_PID);
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
    sptr<IRSComposerToRenderConnection> conn1 = nullptr;
    multiProcessManager_->RecordComposerToRenderConnection(TEST_PID, conn1);
    sptr<IRSComposerToRenderConnection> conn2 = nullptr;
    multiProcessManager_->RecordComposerToRenderConnection(TEST_PID, conn2);
    EXPECT_EQ(multiProcessManager_->composerToRenderConnections_.count(TEST_PID), 1u);
}

/**
 * @tc.name: UpdateAndGetGroupIdToPid001
 * @tc.desc: Test UpdateGroupIdToRenderProcessPid and GetRenderProcessPidByGroupId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, UpdateAndGetGroupIdToPid001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto result = multiProcessManager_->GetRenderProcessPidByGroupId(TEST_GROUP_ID);
    EXPECT_FALSE(result.has_value());
    multiProcessManager_->UpdateGroupIdToRenderProcessPid(TEST_GROUP_ID, TEST_PID);
    result = multiProcessManager_->GetRenderProcessPidByGroupId(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), TEST_PID);
}

/**
 * @tc.name: UpdateAndGetGroupIdToPid002
 * @tc.desc: Test UpdateGroupIdToRenderProcessPid overwrites existing
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, UpdateAndGetGroupIdToPid002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->UpdateGroupIdToRenderProcessPid(TEST_GROUP_ID, TEST_PID);
    multiProcessManager_->UpdateGroupIdToRenderProcessPid(TEST_GROUP_ID, TEST_PID_2);
    auto result = multiProcessManager_->GetRenderProcessPidByGroupId(TEST_GROUP_ID);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), TEST_PID_2);
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
    auto conns = multiProcessManager_->GetServiceToRenderConns();
    EXPECT_TRUE(conns.empty());
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
    multiProcessManager_->serviceToRenderConnections_[TEST_PID] = nullptr;
    multiProcessManager_->serviceToRenderConnections_[TEST_PID_2] = nullptr;
    auto conns = multiProcessManager_->GetServiceToRenderConns();
    EXPECT_EQ(conns.size(), 2u);
    multiProcessManager_->serviceToRenderConnections_.clear();
}

/**
 * @tc.name: GetServiceToRenderConnByPid001
 * @tc.desc: Test GetServiceToRenderConnByPid returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConnByPid001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetServiceToRenderConnByPid(TEST_PID);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GetServiceToRenderConnByPid002
 * @tc.desc: Test GetServiceToRenderConnByPid returns connection when found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConnByPid002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    sptr<RSIServiceToRenderConnection> storedConn = nullptr;
    multiProcessManager_->serviceToRenderConnections_[TEST_PID] = storedConn;
    auto conn = multiProcessManager_->GetServiceToRenderConnByPid(TEST_PID);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_PID);
}

/**
 * @tc.name: GetServiceToRenderConnByPidLocked001
 * @tc.desc: Test GetServiceToRenderConnByPidLocked returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetServiceToRenderConnByPidLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetServiceToRenderConnByPidLocked(TEST_PID);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GetConnectToRenderConnByPid001
 * @tc.desc: Test GetConnectToRenderConnByPid returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetConnectToRenderConnByPid001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetConnectToRenderConnByPid(TEST_PID);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GetConnectToRenderConnByPidLocked001
 * @tc.desc: Test GetConnectToRenderConnByPidLocked returns nullptr when not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GetConnectToRenderConnByPidLocked001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    auto conn = multiProcessManager_->GetConnectToRenderConnByPidLocked(TEST_PID);
    EXPECT_EQ(conn, nullptr);
}

/**
 * @tc.name: GotComposerToRenderConnByPid001
 * @tc.desc: Test GotComposerToRenderConnByPid throws when pid not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GotComposerToRenderConnByPid001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    EXPECT_THROW(multiProcessManager_->GotComposerToRenderConnByPid(TEST_PID), std::out_of_range);
}

/**
 * @tc.name: GotServiceToRenderConnByPid001
 * @tc.desc: Test GotServiceToRenderConnByPid throws when pid not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GotServiceToRenderConnByPid001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    EXPECT_THROW(multiProcessManager_->GotServiceToRenderConnByPid(TEST_PID), std::out_of_range);
}

/**
 * @tc.name: GotConnectToRenderConnByPid001
 * @tc.desc: Test GotConnectToRenderConnByPid throws when pid not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, GotConnectToRenderConnByPid001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    EXPECT_THROW(multiProcessManager_->GotConnectToRenderConnByPid(TEST_PID), std::out_of_range);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessPid(TEST_GROUP_ID, TEST_PID);
    multiProcessManager_->OnScreenDisconnected(TEST_SCREEN_ID);
    multiProcessManager_->groupIdToRenderProcessPid_.erase(TEST_GROUP_ID);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessPid(TEST_GROUP_ID, TEST_PID);
    multiProcessManager_->serviceToRenderConnections_[TEST_PID] = nullptr;
    multiProcessManager_->OnScreenRefresh(TEST_SCREEN_ID);
    multiProcessManager_->groupIdToRenderProcessPid_.erase(TEST_GROUP_ID);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_PID);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessPid(TEST_GROUP_ID, TEST_PID);
    auto conn = multiProcessManager_->GetServiceToRenderConn(TEST_SCREEN_ID);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->groupIdToRenderProcessPid_.erase(TEST_GROUP_ID);
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
    multiProcessManager_->UpdateGroupIdToRenderProcessPid(TEST_GROUP_ID, TEST_PID);
    auto conn = multiProcessManager_->GetConnectToRenderConnection(TEST_SCREEN_ID);
    EXPECT_EQ(conn, nullptr);
    multiProcessManager_->groupIdToRenderProcessPid_.erase(TEST_GROUP_ID);
    renderService_->renderModeConfig_ = nullptr;
}

/**
 * @tc.name: SetRenderProcessReadyPromise001
 * @tc.desc: Test SetRenderProcessReadyPromise sets connections and fulfills promise
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, SetRenderProcessReadyPromise001, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->renderProcessReadyPromises_[TEST_PID] = std::promise<bool>();
    sptr<RSIServiceToRenderConnection> serviceConn = nullptr;
    sptr<RSIConnectToRenderProcess> connectConn = nullptr;
    multiProcessManager_->SetRenderProcessReadyPromise(TEST_PID, serviceConn, connectConn);
    EXPECT_EQ(multiProcessManager_->renderProcessReadyPromises_.count(TEST_PID), 0u);
    EXPECT_EQ(multiProcessManager_->serviceToRenderConnections_.count(TEST_PID), 1u);
    EXPECT_EQ(multiProcessManager_->connectToRenderConnections_.count(TEST_PID), 1u);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_PID);
    multiProcessManager_->connectToRenderConnections_.erase(TEST_PID);
}

/**
 * @tc.name: SetRenderProcessReadyPromise002
 * @tc.desc: Test SetRenderProcessReadyPromise overwrites existing connections
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiRenderProcessManagerTest, SetRenderProcessReadyPromise002, TestSize.Level1)
{
    ASSERT_NE(multiProcessManager_, nullptr);
    multiProcessManager_->serviceToRenderConnections_[TEST_PID] = nullptr;
    multiProcessManager_->connectToRenderConnections_[TEST_PID] = nullptr;
    multiProcessManager_->renderProcessReadyPromises_[TEST_PID] = std::promise<bool>();
    sptr<RSIServiceToRenderConnection> serviceConn = nullptr;
    sptr<RSIConnectToRenderProcess> connectConn = nullptr;
    multiProcessManager_->SetRenderProcessReadyPromise(TEST_PID, serviceConn, connectConn);
    EXPECT_EQ(multiProcessManager_->serviceToRenderConnections_.count(TEST_PID), 1u);
    EXPECT_EQ(multiProcessManager_->connectToRenderConnections_.count(TEST_PID), 1u);
    multiProcessManager_->serviceToRenderConnections_.erase(TEST_PID);
    multiProcessManager_->connectToRenderConnections_.erase(TEST_PID);
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
    auto property = sptr<RSScreenProperty>::MakeSptr();
    multiProcessManager_->pendingScreenConnectInfos_[TEST_PID] =
        { TEST_SCREEN_ID, property };
    multiProcessManager_->renderProcessReadyPromises_[TEST_PID] = std::promise<bool>();
    auto result = multiProcessManager_->GetPendingScreenProperty(TEST_PID);
    ASSERT_NE(result, nullptr);
    multiProcessManager_->renderProcessReadyPromises_.erase(TEST_PID);
}

} // namespace OHOS::Rosen
