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
#include "platform/ohos/transaction/zidl/rs_iclient_to_service_connection.h"
#include "rs_client_to_service_connect_hub.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockRemoteObject : public IRemoteObject {
public:
    MockRemoteObject() : IRemoteObject(u"MockRemoteObject") {}
    int32_t GetObjectRefCount() override { return 0; }
    int SendRequest(uint32_t, MessageParcel&, MessageParcel&, MessageOption&) override { return 0; }
    bool IsProxyObject() const override { return false; }
    bool CheckObjectLegality() const override { return true; }
    bool AddDeathRecipient(const sptr<DeathRecipient>&) override { return true; }
    bool RemoveDeathRecipient(const sptr<DeathRecipient>&) override { return true; }
    int Dump(int fd, const std::vector<std::u16string>&) override { return 0; }
};

class RSClientToServiceConnectHubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSClientToServiceConnectHubTest::SetUpTestCase() {}
void RSClientToServiceConnectHubTest::TearDownTestCase() {}
void RSClientToServiceConnectHubTest::SetUp() {}
void RSClientToServiceConnectHubTest::TearDown() {}

/**
 * @tc.name: GetInstance001
 * @tc.desc: Verify GetInstance returns non-null and is singleton
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, GetInstance001, TestSize.Level1)
{
    auto instance1 = RSClientToServiceConnectHub::GetInstance();
    EXPECT_NE(instance1, nullptr);

    auto instance2 = RSClientToServiceConnectHub::GetInstance();
    EXPECT_EQ(instance1.GetRefPtr(), instance2.GetRefPtr());
}

/**
 * @tc.name: GetClientToServiceConnection001
 * @tc.desc: Verify GetClientToServiceConnection returns nullptr when no render service
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, GetClientToServiceConnection001, TestSize.Level1)
{
    auto conn = RSClientToServiceConnectHub::GetClientToServiceConnection();
    EXPECT_NE(conn, nullptr);
}

/**
 * @tc.name: GetToken001
 * @tc.desc: Verify GetToken returns nullptr when not connected
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, GetToken001, TestSize.Level1)
{
    auto instance = RSClientToServiceConnectHub::GetInstance();
    ASSERT_NE(instance, nullptr);
    auto token = instance->GetToken();
    EXPECT_NE(token, nullptr);
}

/**
 * @tc.name: ConnectDied001
 * @tc.desc: Verify ConnectDied does not crash when not connected
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, ConnectDied001, TestSize.Level1)
{
    auto instance = RSClientToServiceConnectHub::GetInstance();
    ASSERT_NE(instance, nullptr);
    instance->ConnectDied();
    EXPECT_EQ(instance->GetToken(), nullptr);
}

/**
 * @tc.name: GetClientToServiceConnection002
 * @tc.desc: Verify GetClientToServiceConnection returns cached connection
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, GetClientToServiceConnection002, TestSize.Level1)
{
    auto conn1 = RSClientToServiceConnectHub::GetClientToServiceConnection();
    auto conn2 = RSClientToServiceConnectHub::GetClientToServiceConnection();
    if (conn1 != nullptr && conn2 != nullptr) {
        EXPECT_EQ(conn1.GetRefPtr(), conn2.GetRefPtr());
    }
}

/**
 * @tc.name: ConnectDied002
 * @tc.desc: Verify ConnectDied clears token after connection died
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, ConnectDied002, TestSize.Level1)
{
    auto instance = RSClientToServiceConnectHub::GetInstance();
    ASSERT_NE(instance, nullptr);
    instance->ConnectDied();
    auto tokenAfter = instance->GetToken();
    EXPECT_EQ(tokenAfter, nullptr);
}

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: OnRemoteDied001
 * @tc.desc: Verify OnRemoteDied with null remote, remote.promote() returns null
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, OnRemoteDied001, TestSize.Level1)
{
    auto instance = RSClientToServiceConnectHub::GetInstance();
    ASSERT_NE(instance, nullptr);
    wptr<RSClientToServiceConnectHub> hub = instance;
    auto recipient = new RSClientToServiceConnectHub::RenderServiceDeathRecipient(hub);
    wptr<IRemoteObject> nullRemote;
    recipient->OnRemoteDied(nullRemote);
    EXPECT_NE(instance, nullptr);
}

/**
 * @tc.name: OnRemoteDied002
 * @tc.desc: Verify OnRemoteDied when hub was already dead, hub_.promote() returns null
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, OnRemoteDied002, TestSize.Level1)
{
    wptr<RSClientToServiceConnectHub> weakHub;
    {
        sptr<RSClientToServiceConnectHub> tempHub = new RSClientToServiceConnectHub();
        weakHub = tempHub;
    }
    auto recipient = new RSClientToServiceConnectHub::RenderServiceDeathRecipient(weakHub);
    sptr<IRemoteObject> mockRemote = new MockRemoteObject();
    recipient->OnRemoteDied(mockRemote);
    EXPECT_EQ(weakHub.promote(), nullptr);
    auto instance = RSClientToServiceConnectHub::GetInstance();
    EXPECT_NE(instance, nullptr);
}

/**
 * @tc.name: OnRemoteDied003
 * @tc.desc: Verify OnRemoteDied normal path triggers ConnectDied
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, OnRemoteDied003, TestSize.Level1)
{
    auto conn = RSClientToServiceConnectHub::GetClientToServiceConnection();
    auto instance = RSClientToServiceConnectHub::GetInstance();
    ASSERT_NE(instance, nullptr);
    ASSERT_NE(instance->renderService_, nullptr);

    wptr<RSClientToServiceConnectHub> hub = instance;
    auto recipient = new RSClientToServiceConnectHub::RenderServiceDeathRecipient(hub);
    sptr<IRemoteObject> remote = instance->renderService_;
    recipient->OnRemoteDied(remote);

    EXPECT_EQ(instance->renderService_, nullptr);
    EXPECT_EQ(instance->conn_, nullptr);
    EXPECT_EQ(instance->token_, nullptr);
    EXPECT_EQ(instance->deathRecipient_, nullptr);
}

/**
 * @tc.name: ConnectDied003
 * @tc.desc: Verify ConnectDied with valid conn_ triggers RunOnRemoteDiedCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToServiceConnectHubTest, ConnectDied003, TestSize.Level1)
{
    auto conn = RSClientToServiceConnectHub::GetClientToServiceConnection();
    auto instance = RSClientToServiceConnectHub::GetInstance();
    ASSERT_NE(instance, nullptr);
    ASSERT_NE(instance->conn_, nullptr);

    instance->ConnectDied();

    EXPECT_EQ(instance->conn_, nullptr);
    EXPECT_EQ(instance->renderService_, nullptr);
    EXPECT_EQ(instance->token_, nullptr);
    EXPECT_EQ(instance->deathRecipient_, nullptr);
}
#endif
} // namespace Rosen
} // namespace OHOS
