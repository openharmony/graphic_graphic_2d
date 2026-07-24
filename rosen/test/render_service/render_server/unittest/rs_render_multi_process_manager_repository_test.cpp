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

#include <chrono>
#include <thread>
#include <sys/wait.h>
#include <unistd.h>

#include "common/rs_macros.h"
#include "platform/common/rs_log.h"
#include "render_server/rs_render_multi_process_manager_repository.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

class MockDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    void OnRemoteDied(const wptr<IRemoteObject>& object) override {}
};
constexpr pid_t TEST_PID = 1234;
constexpr pid_t TEST_PID_2 = 5678;
constexpr ScreenId TEST_SCREEN_ID = 100;
constexpr ScreenId TEST_SCREEN_ID_2 = 200;
constexpr GroupId TEST_GROUP_ID = 1;
const ProcessUniqueId TEST_UID{TEST_PID};
} // namespace

class RSRenderMultiProcessManagerRepositoryTest : public testing::Test {
public:
    void SetUp() override
    {
        store_ = std::make_unique<RSRenderMultiProcessManagerRepository>();
    }
    void TearDown() override
    {
        store_.reset();
    }

protected:
    std::unique_ptr<RSRenderMultiProcessManagerRepository> store_;
};

/**
 * @tc.name: RegisterNewProcess001
 * @tc.desc: Test RegisterNewProcess registers PID, groupId, screenOutput, and pendingScreenConnectInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, RegisterNewProcess001, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise;
    auto future = promise.get_future();

    auto uid = store_->RegisterNewProcess(TEST_GROUP_ID, TEST_PID,
        { TEST_SCREEN_ID, output, property }, std::move(promise));
    
    EXPECT_EQ(uid.GetPid(), TEST_PID);

    auto optUid = store_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    ASSERT_TRUE(optUid.has_value());
    EXPECT_EQ(optUid.value(), uid);

    auto screenProp = store_->GetPendingScreenProperty(TEST_PID);
    ASSERT_NE(screenProp, nullptr);
}

/**
 * @tc.name: RegisterNewProcess002
 * @tc.desc: Test RegisterNewProcess with same groupId replaces previous entry
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, RegisterNewProcess002, TestSize.Level1)
{
    auto output1 = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property1 = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise1;
    auto uid1 = store_->RegisterNewProcess(TEST_GROUP_ID, TEST_PID,
        { TEST_SCREEN_ID, output1, property1 }, std::move(promise1));
    
    auto output2 = std::make_shared<HdiOutput>(TEST_SCREEN_ID_2);
    auto property2 = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise2;
    auto uid2 = store_->RegisterNewProcess(TEST_GROUP_ID, TEST_PID_2,
        { TEST_SCREEN_ID_2, output2, property2 }, std::move(promise2));
    
    auto optUid = store_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    ASSERT_TRUE(optUid.has_value());
    EXPECT_EQ(optUid.value(), uid2);
    EXPECT_FALSE(optUid.value() == uid1);
}

/**
 * @tc.name: SetRenderProcessReadyPromise001
 * @tc.desc: Test SetRenderProcessReadyPromise sets promise and resolves future
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, SetRenderProcessReadyPromise001, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise;
    auto future = promise.get_future();

    store_->RegisterNewProcess(TEST_GROUP_ID, TEST_PID,
        { TEST_SCREEN_ID, output, property }, std::move(promise));
    
    sptr<RSIServiceToRenderConnection> serviceConn = nullptr;
    sptr<RSIConnectToRenderProcess> connectConn = nullptr;
    auto factory = [](ProcessUniqueId) -> sptr<IRemoteObject::DeathRecipient> { return nullptr; };

    bool result = store_->SetRenderProcessReadyPromise(TEST_PID, serviceConn, connectConn, factory, nullptr);
    EXPECT_TRUE(result);

    auto status = future.wait_for(std::chrono::seconds(1));
    EXPECT_EQ(status, std::future_status::ready);
    EXPECT_TRUE(future.get());
}

/**
 * @tc.name: SetRenderProcessReadyPromise002
 * @tc.desc: Test SetRenderProcessReadyPromise with non-registered PID returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, SetRenderProcessReadyPromise002, TestSize.Level1)
{
    sptr<RSIServiceToRenderConnection> serviceConn = nullptr;
    sptr<RSIConnectToRenderProcess> connectConn = nullptr;
    auto factory = [](ProcessUniqueId) -> sptr<IRemoteObject::DeathRecipient> { return nullptr; };

    bool result = store_->SetRenderProcessReadyPromise(TEST_PID, serviceConn, connectConn, factory, nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetRenderProcessReadyPromise003
 * @tc.desc: Test SetRenderProcessReadyPromise stores serviceToRender and connectToRender connections
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, SetRenderProcessReadyPromise003, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise;
    auto uid = store_->RegisterNewProcess(TEST_GROUP_ID, TEST_PID,
        { TEST_SCREEN_ID, output, property }, std::move(promise));

    sptr<RSIServiceToRenderConnection> serviceConn = nullptr;
    sptr<RSIConnectToRenderProcess> connectConn = nullptr;
    auto factory = [](ProcessUniqueId) -> sptr<IRemoteObject::DeathRecipient> { return nullptr; };

    store_->SetRenderProcessReadyPromise(TEST_PID, serviceConn, connectConn, factory, nullptr);

    auto storedService = store_->GetServiceToRenderConnByUniqueId(uid);
    EXPECT_EQ(storedService, nullptr); // we passed nullptr

    auto storedConnect = store_->GetConnectToRenderConnByUniqueId(uid);
    EXPECT_EQ(storedConnect, nullptr); // we passed nullptr
}

/**
 * @tc.name: HandleRenderProcessDeath001
 * @tc.desc: Test HandleRenderProcessDeath cleans up all maps and returns affected screen outputs
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, HandleRenderProcessDeath001, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise;
    auto uid = store_->RegisterNewProcess(TEST_GROUP_ID, TEST_PID,
        { TEST_SCREEN_ID, output, property }, std::move(promise));
    
    auto affected = store_->HandleRenderProcessDeath(uid);

    ASSERT_EQ(affected.size(), 1u);
    EXPECT_EQ(affected[0].first, TEST_SCREEN_ID);

    auto optUid = store_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    EXPECT_FALSE(optUid.has_value());
}

/**
 * @tc.name: GetRenderProcessUniqueIdByGroupId001
 * @tc.desc: Test GetRenderProcessUniqueIdByGroupId returns nullopt for unknown group
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, GetRenderProcessUniqueIdByGroupId001, TestSize.Level1)
{
    auto optUid = store_->GetRenderProcessUniqueIdByGroupId(TEST_GROUP_ID);
    EXPECT_FALSE(optUid.has_value());
}

/**
 * @tc.name: IsValidRenderProcessPid001
 * @tc.desc: Test IsValidRenderProcessPid returns true for a real child process with registered PID
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, IsValidRenderProcessPid001, TestSize.Level1)
{
    pid_t pid = fork();
    ASSERT_GE(pid, 0);
    if (pid == 0) {
        // Child: sleep so parent can check our PID
        _exit(0);
    }
    // Register the real child PID
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise;
    store_->RegisterNewProcess(TEST_GROUP_ID, pid,
        { TEST_SCREEN_ID, output, property }, std::move(promise));
    
    bool result = store_->IsValidRenderProcessPid(pid);
    EXPECT_TRUE(result);

    int status = 0;
    waitpid(pid, &status, 0);
}

/**
 * @tc.name: IsValidRenderProcessPid002
 * @tc.desc: Test IsValidRenderProcessPid returns false for unregistered PID (with short timeout)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, IsValidRenderProcessPid002, TestSize.Level1)
{
    bool result = store_->IsValidRenderProcessPid(TEST_PID);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetRenderProcessReadyPromise005
 * @tc.desc: Test deathRecipient map behavior after SetRenderProcessReadyPromise with null binderObject
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, SetRenderProcessReadyPromise005, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise;
    auto uid = store_->RegisterNewProcess(TEST_GROUP_ID, TEST_PID,
        { TEST_SCREEN_ID, output, property }, std::move(promise));
    
    sptr<RSIServiceToRenderConnection> serviceConn = nullptr;
    sptr<RSIConnectToRenderProcess> connectConn = nullptr;
    auto mockDeathRecipient = sptr<MockDeathRecipient>::MakeSptr();
    auto factory = [mockDeathRecipient](ProcessUniqueId) -> sptr<IRemoteObject::DeathRecipient> {
        return mockDeathRecipient;
    };

    bool result = store_->SetRenderProcessReadyPromise(TEST_PID, serviceConn, connectConn, factory, nullptr);
    EXPECT_TRUE(result);
    EXPECT_EQ(store_->deathRecipients_.count(uid), 0u);
    store_->deathRecipients_[uid] = mockDeathRecipient;
    EXPECT_EQ(store_->deathRecipients_.count(uid), 1u);
    EXPECT_EQ(store_->deathRecipients_[uid].GetRefPtr(), mockDeathRecipient.GetRefPtr());
}

/**
 * @tc.name: SetRenderProcessReadyPromise006
 * @tc.desc: Test deathRecipient replacement in map after SetRenderProcessReadyPromise with null binderObject
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, SetRenderProcessReadyPromise006, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise;
    auto future = promise.get_future();
    auto uid = store_->RegisterNewProcess(TEST_GROUP_ID, TEST_PID,
        { TEST_SCREEN_ID, output, property }, std::move(promise));
    
    auto oldDeathRecipient = sptr<MockDeathRecipient>::MakeSptr();
    store_->deathRecipients_[uid] = oldDeathRecipient;

    sptr<RSIServiceToRenderConnection> serviceConn = nullptr;
    sptr<RSIConnectToRenderProcess> connectConn = nullptr;
    auto newDeathRecipient = sptr<MockDeathRecipient>::MakeSptr();
    auto factory = [newDeathRecipient](ProcessUniqueId) -> sptr<IRemoteObject::DeathRecipient> {
        return newDeathRecipient;
    };

    bool result = store_->SetRenderProcessReadyPromise(TEST_PID, serviceConn, connectConn, factory, nullptr);
    EXPECT_TRUE(result);
    EXPECT_EQ(store_->deathRecipients_[uid].GetRefPtr(), oldDeathRecipient.GetRefPtr());
    store_->deathRecipients_[uid] = newDeathRecipient;
    EXPECT_EQ(store_->deathRecipients_[uid].GetRefPtr(), oldDeathRecipient.GetRefPtr());

    auto status = future.wait_for(std::chrono::seconds(1));
    EXPECT_EQ(status, std::future_status::ready);
    EXPECT_TRUE(future.get());
}

/**
 * @tc.name: CheckAndHandleSubprocessDeathOverflow002
 * @tc.desc: Test CheckAndHandleSubprocessDeathOverflow triggers exit when threshold exceeded
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, CheckAndHandleSubprocessDeathOverflow002, TestSize.Level1)
{
    pid_t pid = fork();
    ASSERT_GE(pid, 0);
    if (pid == 0) {
        store_->CheckAndHandleSubprocessDeathOverflow();
        store_->CheckAndHandleSubprocessDeathOverflow();
        store_->CheckAndHandleSubprocessDeathOverflow();
        _exit(0);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    EXPECT_TRUE(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 255);
}

/**
 * @tc.name: IsValidRenderProcessPid003
 * @tc.desc: Test IsValidRenderProcessPid returns false after HandleRenderProcessDeath removes the process
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMultiProcessManagerRepositoryTest, IsValidRenderProcessPid003, TestSize.Level1)
{
    pid_t pid = fork();
    ASSERT_GE(pid, 0);
    if (pid == 0) {
        _exit(0);
    }
    auto output = std::make_shared<HdiOutput>(TEST_SCREEN_ID);
    auto property = sptr<RSScreenProperty>::MakeSptr();
    std::promise<bool> promise;
    auto uid = store_->RegisterNewProcess(TEST_GROUP_ID, TEST_PID,
        { TEST_SCREEN_ID, output, property }, std::move(promise));
    EXPECT_TRUE(store_->IsValidRenderProcessPid(pid));
    store_->HandleRenderProcessDeath(uid);
    EXPECT_FALSE(store_->IsValidRenderProcessPid(pid));

    int status = 0;
    waitpid(pid, &status, 0);
}

} // namespace OHOS::Rosen
