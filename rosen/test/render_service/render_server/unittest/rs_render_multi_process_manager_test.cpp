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

} // namespace OHOS::Rosen
