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

class MockRemoteObject : public IRemoteObject {
public:
    MockRemoteObject() : IRemoteObject(u"MockRemoteObject") {}
    int32_t GetObjectRefCount() override { return 0; }
    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient) override { return true; }
    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient) override { return true; }
    int Dump(int fd, const std::vector<std::u16string>& args) override { return 0; }
    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
        MessageOption& option) override { return 0; }
};

class MockDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    void OnRemoteDied(const wptr<IRemoteObject>& object) override {}
};
constexpr pid_t TEST_PID = 1234;
constexpr ScreenId TEST_SCREEN_ID = 100;
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
} // namespace OHOS::Rosen
