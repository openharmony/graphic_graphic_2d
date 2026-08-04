/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <iostream>
#include "transaction/rs_interfaces.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "platform/ohos/transaction/zidl/rs_client_to_render_connection_proxy.h"
#include <iremote_stub.h>
#include "transaction/rs_application_agent_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockRSIConnectToRenderProcess : public IRemoteStub<RSIConnectToRenderProcess> {
public:
    bool RemoveConnection(uint64_t) override { return true; }
    std::pair<sptr<RSIClientToRenderConnection>, uint64_t> CreateRenderConnection(
        uint64_t, const sptr<RSIConnectionToken>&, bool) override
    {
        return {nullptr, MOCK_ID};
    }
    static constexpr uint64_t MOCK_ID = 42;
};

class RSRenderServiceConnectHubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline sptr<RSIConnectionToken> token_;
};

void RSRenderServiceConnectHubTest::SetUpTestCase()
{
    token_ = new IRemoteStub<RSIConnectionToken>();
}
void RSRenderServiceConnectHubTest::TearDownTestCase()
{
    token_ = nullptr;
}
void RSRenderServiceConnectHubTest::SetUp() {}
void RSRenderServiceConnectHubTest::TearDown() {}

/**
 * @tc.name: GetRenderService
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: I6X9V1
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetRenderService, TestSize.Level1)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    ASSERT_NE(renderService.first, nullptr);
    ASSERT_NE(renderService.second, nullptr);
}

/**
 * @tc.name: GetRenderServiceConnectionTest
 * @tc.desc: Verify function GetRenderServiceConnection
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetRenderServiceConnectionTest, TestSize.Level1)
{
    EXPECT_NE(RSRenderServiceConnectHub::GetInstance()->GetRenderServiceConnection().first, nullptr);
    EXPECT_NE(RSRenderServiceConnectHub::GetInstance()->GetRenderServiceConnection().second, nullptr);
    RSRenderServiceConnectHub::GetInstance()->conn_ = nullptr;
    RSRenderServiceConnectHub::GetInstance()->renderService_ = nullptr;
    EXPECT_NE(RSRenderServiceConnectHub::GetInstance()->GetRenderServiceConnection().first, nullptr);
    EXPECT_NE(RSRenderServiceConnectHub::GetInstance()->GetRenderServiceConnection().second, nullptr);
}

/**
 * @tc.name: ConnectDiedTest
 * @tc.desc: Verify function ConnectDied
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(RSRenderServiceConnectHubTest, ConnectDiedTest, TestSize.Level1)
{
    RSRenderServiceConnectHub::GetInstance()->ConnectDied();
    RSRenderServiceConnectHub::GetInstance()->conn_ = nullptr;
    RSRenderServiceConnectHub::GetInstance()->ConnectDied();
    EXPECT_NE(RSRenderServiceConnectHub::GetInstance(), nullptr);
}

/**
 * @tc.name: RSRenderServiceConnectHubContructAndDestructTest001
 * @tc.desc: Verify RenderServiceConnectHub Contruct And Destruct
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(RSRenderServiceConnectHubTest, RSRenderServiceConnectHubContructAndDestructTest001, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    ASSERT_EQ(connHub->renderService_, nullptr);
    RSRenderServiceConnectHub::SetOnDiedCallback(RSOnDiedCallbackCode::APPLICATION_AGENT, []() {
        std::cout << "Run Callback" << std::endl;
    });
    connHub->Destroy();
}

#ifdef OHOS_PLATFORM
#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: RSApplicationAgentImplTest
 * @tc.desc: Verify RSApplicationAgentImplTest dlclose
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectHubTest, RSApplicationAgentImplTest, TestSize.Level1)
{
    // RSApplicationAgentImpl already is nullptr
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    RSRenderServiceConnectHub::GetClientToServiceConnection();
    RSApplicationAgentImpl::Instance();
    RSApplicationAgentImpl::Destroy();
    RSApplicationAgentImpl::Destroy();
    RSRenderServiceConnectHub::Destroy();
    RSRenderServiceConnectHub::Init();
    RSApplicationAgentImpl::Instance();

    auto connHub2 = RSRenderServiceConnectHub::GetInstance();
    RSRenderServiceConnectHub::GetClientToServiceConnection();
    RSRenderServiceConnectHub::SetOnDiedCallback(RSOnDiedCallbackCode::APPLICATION_AGENT, nullptr);
    EXPECT_EQ(connHub2->OnDiedCallbacks_.size(), 1);
    RSRenderServiceConnectHub::RemoveOnDiedCallback(RSOnDiedCallbackCode::APPLICATION_AGENT, false);
    EXPECT_EQ(connHub2->OnDiedCallbacks_.size(), 0);
    connHub2->OnDiedCallbacks_[1] = nullptr;
    RSRenderServiceConnectHub::SetOnDiedCallback(RSOnDiedCallbackCode::APPLICATION_AGENT, []() {
        std::cout << "Runing APPLICATION_AGENT callback" << std::endl;
    });
    RSRenderServiceConnectHub::Destroy();
    RSRenderServiceConnectHub::Init();
    auto instance3 = RSApplicationAgentImpl::Instance();
    EXPECT_NE(instance3, nullptr);
}
#endif
#endif
/**
 * @tc.name: SetOnConnectCallbackTest001
 * @tc.desc: branch 1 - instance_ is nullptr, outer if false, callback not invoked
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, SetOnConnectCallbackTest001, TestSize.Level1)
{
    auto savedInstance = RSRenderServiceConnectHub::instance_;
    RSRenderServiceConnectHub::instance_ = nullptr;
    bool invoked = false;
    auto cb = [&invoked](sptr<RSIClientToRenderConnection>&) { invoked = true; };
    RSRenderServiceConnectHub::SetOnConnectCallback(cb);
    RSRenderServiceConnectHub::onConnectCallback_ = nullptr;
    RSRenderServiceConnectHub::instance_ = savedInstance;
    ASSERT_FALSE(invoked);
}

/**
 * @tc.name: SetOnConnectCallbackTest002
 * @tc.desc: branch 2 - renderConn_ is nullptr, outer if false, callback not invoked
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, SetOnConnectCallbackTest002, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    auto savedRenderConn = connHub->renderConn_;
    connHub->renderConn_ = nullptr;
    bool invoked = false;
    auto cb = [&invoked](sptr<RSIClientToRenderConnection>&) { invoked = true; };
    RSRenderServiceConnectHub::SetOnConnectCallback(cb);
    RSRenderServiceConnectHub::onConnectCallback_ = nullptr;
    connHub->renderConn_ = savedRenderConn;
    ASSERT_FALSE(invoked);
}

/**
 * @tc.name: SetOnConnectCallbackTest003
 * @tc.desc: branch 3 - cb is nullptr, inner if false, stored callback empty
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, SetOnConnectCallbackTest003, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    auto savedRenderConn = connHub->renderConn_;
    if (!savedRenderConn) {
        connHub->renderConn_ = new RSClientToRenderConnectionProxy(nullptr);
    }
    RSRenderServiceConnectHub::SetOnConnectCallback(nullptr);
    connHub->renderConn_ = savedRenderConn;
    ASSERT_FALSE((bool)RSRenderServiceConnectHub::onConnectCallback_);
}

/**
 * @tc.name: SetOnConnectCallbackTest004
 * @tc.desc: branch 4 - cb non-null and renderConn_ non-null, inner if true, callback invoked
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, SetOnConnectCallbackTest004, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    auto savedRenderConn = connHub->renderConn_;
    if (!savedRenderConn) {
        connHub->renderConn_ = new RSClientToRenderConnectionProxy(nullptr);
    }
    bool invoked = false;
    auto cb = [&invoked](sptr<RSIClientToRenderConnection>&) { invoked = true; };
    RSRenderServiceConnectHub::SetOnConnectCallback(cb);
    RSRenderServiceConnectHub::onConnectCallback_ = nullptr;
    connHub->renderConn_ = savedRenderConn;
    ASSERT_TRUE(invoked);
}

/**
 * @tc.name: SetOnDiedCallbackTest001
 * @tc.desc: branch 1 - instance is nullptr, return early, callback not stored
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, SetOnDiedCallbackTest001, TestSize.Level1)
{
    auto saved = RSRenderServiceConnectHub::GetInstance();
    auto key = static_cast<int32_t>(RSOnDiedCallbackCode::APPLICATION_AGENT);
    saved->OnDiedCallbacks_.erase(key);
    RSRenderServiceConnectHub::instance_ = nullptr;
    RSRenderServiceConnectHub::SetOnDiedCallback(RSOnDiedCallbackCode::APPLICATION_AGENT, []() {});
    RSRenderServiceConnectHub::instance_ = saved;
    ASSERT_EQ(saved->OnDiedCallbacks_.count(key), 0);
}

/**
 * @tc.name: SetOnDiedCallbackTest002
 * @tc.desc: branch 2 - instance is not nullptr, callback stored in OnDiedCallbacks_
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, SetOnDiedCallbackTest002, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    auto key = static_cast<int32_t>(RSOnDiedCallbackCode::APPLICATION_AGENT);
    RSRenderServiceConnectHub::SetOnDiedCallback(RSOnDiedCallbackCode::APPLICATION_AGENT, []() {});
    bool stored = connHub->OnDiedCallbacks_.count(key) > 0;
    connHub->OnDiedCallbacks_.erase(key);
    ASSERT_TRUE(stored);
}

/**
 * @tc.name: RemoveOnDiedCallbackTest001
 * @tc.desc: branch 1 - isDestructionProcess is true, return early, callback not erased
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, RemoveOnDiedCallbackTest001, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    auto key = static_cast<int32_t>(RSOnDiedCallbackCode::APPLICATION_AGENT);
    connHub->OnDiedCallbacks_[key] = []() {};
    RSRenderServiceConnectHub::RemoveOnDiedCallback(RSOnDiedCallbackCode::APPLICATION_AGENT, true);
    bool preserved = connHub->OnDiedCallbacks_.count(key) > 0;
    connHub->OnDiedCallbacks_.erase(key);
    ASSERT_TRUE(preserved);
}

/**
 * @tc.name: RemoveOnDiedCallbackTest002
 * @tc.desc: branch 2 - isDestructionProcess is false and instance is nullptr, return early
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, RemoveOnDiedCallbackTest002, TestSize.Level1)
{
    auto saved = RSRenderServiceConnectHub::GetInstance();
    auto key = static_cast<int32_t>(RSOnDiedCallbackCode::APPLICATION_AGENT);
    saved->OnDiedCallbacks_[key] = []() {};
    RSRenderServiceConnectHub::instance_ = nullptr;
    RSRenderServiceConnectHub::RemoveOnDiedCallback(RSOnDiedCallbackCode::APPLICATION_AGENT, false);
    RSRenderServiceConnectHub::instance_ = saved;
    bool preserved = saved->OnDiedCallbacks_.count(key) > 0;
    saved->OnDiedCallbacks_.erase(key);
    ASSERT_TRUE(preserved);
}

/**
 * @tc.name: RemoveOnDiedCallbackTest003
 * @tc.desc: branch 3 - isDestructionProcess is false and instance is not nullptr, callback erased
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, RemoveOnDiedCallbackTest003, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    auto key = static_cast<int32_t>(RSOnDiedCallbackCode::APPLICATION_AGENT);
    connHub->OnDiedCallbacks_[key] = []() {};
    RSRenderServiceConnectHub::RemoveOnDiedCallback(RSOnDiedCallbackCode::APPLICATION_AGENT, false);
    ASSERT_EQ(connHub->OnDiedCallbacks_.count(key), 0);
}

/**
 * @tc.name: GetDefaultTokenMaskIdTest001
 * @tc.desc: branch 1 - connHub is nullptr, return INVALID_TOKEN_MASK_ID
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetDefaultTokenMaskIdTest001, TestSize.Level1)
{
    auto saved = RSRenderServiceConnectHub::GetInstance();
    RSRenderServiceConnectHub::instance_ = nullptr;
    auto result = RSRenderServiceConnectHub::GetDefaultTokenMaskId();
    RSRenderServiceConnectHub::instance_ = saved;
    ASSERT_EQ(result, INVALID_TOKEN_MASK_ID);
}

/**
 * @tc.name: GetDefaultTokenMaskIdTest002
 * @tc.desc: branch 2 - connHub is not nullptr, calls GetDefaultTokenMaskIdInner
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetDefaultTokenMaskIdTest002, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    auto savedMap = connHub->connRenderProcesses_;
    connHub->connRenderProcesses_.clear();
    constexpr uint64_t TEST_ID = 88888;
    RenderProcessInfo info;
    info.token = new IRemoteStub<RSIConnectionToken>();
    connHub->connRenderProcesses_[TEST_ID] = info;
    auto result = RSRenderServiceConnectHub::GetDefaultTokenMaskId();
    connHub->connRenderProcesses_ = savedMap;
    ASSERT_EQ(result, TEST_ID);
}

/**
 * @tc.name: GetRenderProcessTokenMaskIdTest001
 * @tc.desc: branch 1 - connectToRenderRemote is nullptr, return INVALID_TOKEN_MASK_ID
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetRenderProcessTokenMaskIdTest001, TestSize.Level1)
{
    sptr<IRemoteObject> remote = nullptr;
    ASSERT_EQ(RSRenderServiceConnectHub::GetRenderProcessTokenMaskId(remote),
        INVALID_TOKEN_MASK_ID);
}

/**
 * @tc.name: GetRenderProcessTokenMaskIdTest002
 * @tc.desc: branch 2 - connectToRenderRemote not null but connHub is nullptr, return INVALID
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetRenderProcessTokenMaskIdTest002, TestSize.Level1)
{
    auto saved = RSRenderServiceConnectHub::GetInstance();
    sptr<IRemoteObject> remote = new IRemoteStub<RSIConnectionToken>();
    RSRenderServiceConnectHub::instance_ = nullptr;
    auto result = RSRenderServiceConnectHub::GetRenderProcessTokenMaskId(remote);
    RSRenderServiceConnectHub::instance_ = saved;
    ASSERT_EQ(result, INVALID_TOKEN_MASK_ID);
}

/**
 * @tc.name: GetRenderProcessTokenMaskIdTest003
 * @tc.desc: branch 3 - oldTokenMaskId found in map, return oldTokenMaskId
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetRenderProcessTokenMaskIdTest003, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    auto savedMap = connHub->connRenderProcesses_;
    connHub->connRenderProcesses_.clear();
    sptr<IRemoteObject> remote = new IRemoteStub<RSIConnectionToken>();
    constexpr uint64_t TEST_ID = 77777;
    RenderProcessInfo info;
    info.token = new IRemoteStub<RSIConnectionToken>();
    info.connectToRenderRemote = remote;
    info.clientToRenderConnection = new RSClientToRenderConnectionProxy(nullptr);
    connHub->connRenderProcesses_[TEST_ID] = info;
    auto result = RSRenderServiceConnectHub::GetRenderProcessTokenMaskId(remote);
    connHub->connRenderProcesses_ = savedMap;
    ASSERT_EQ(result, TEST_ID);
}

/**
 * @tc.name: GetRenderProcessTokenMaskIdTest004
 * @tc.desc: branch 4 - oldTokenMaskId is INVALID and iface_cast fails, return INVALID
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetRenderProcessTokenMaskIdTest004, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    auto savedMap = connHub->connRenderProcesses_;
    connHub->connRenderProcesses_.clear();
    sptr<IRemoteObject> remote = new IRemoteStub<RSIConnectionToken>();
    auto result = RSRenderServiceConnectHub::GetRenderProcessTokenMaskId(remote);
    connHub->connRenderProcesses_ = savedMap;
    ASSERT_EQ(result, INVALID_TOKEN_MASK_ID);
}

/**
 * @tc.name: OnRemoteDiedTest001
 * @tc.desc: branch 1 - remoteSptr is nullptr (can't promote), return early
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, OnRemoteDiedTest001, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    constexpr uint64_t TEST_ID = 55555;
    RenderProcessInfo info;
    info.token = new IRemoteStub<RSIConnectionToken>();
    connHub->connRenderProcesses_[TEST_ID] = info;
    wptr<RSRenderServiceConnectHub> wptrConnHub = connHub;
    RSConnectRenderProcessDeathRecipient recipient(wptrConnHub, TEST_ID);
    wptr<IRemoteObject> nullRemote;
    recipient.OnRemoteDied(nullRemote);
    bool preserved = connHub->connRenderProcesses_.count(TEST_ID) > 0;
    connHub->connRenderProcesses_.erase(TEST_ID);
    ASSERT_TRUE(preserved);
}

/**
 * @tc.name: OnRemoteDiedTest002
 * @tc.desc: branch 2 - rsConnHub is nullptr (connHub was dead), return early
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, OnRemoteDiedTest002, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    constexpr uint64_t TEST_ID = 44444;
    RenderProcessInfo info;
    info.token = new IRemoteStub<RSIConnectionToken>();
    connHub->connRenderProcesses_[TEST_ID] = info;
    wptr<RSRenderServiceConnectHub> nullWptr;
    RSConnectRenderProcessDeathRecipient recipient(nullWptr, TEST_ID);
    sptr<IRemoteObject> remote = new IRemoteStub<RSIConnectionToken>();
    recipient.OnRemoteDied(remote);
    bool preserved = connHub->connRenderProcesses_.count(TEST_ID) > 0;
    connHub->connRenderProcesses_.erase(TEST_ID);
    ASSERT_TRUE(preserved);
}

/**
 * @tc.name: OnRemoteDiedTest003
 * @tc.desc: branch 3 - callback is nullptr, skip callback, call ConnectRenderProcessDied
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, OnRemoteDiedTest003, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    constexpr uint64_t TEST_ID = 33333;
    RenderProcessInfo info;
    info.token = new IRemoteStub<RSIConnectionToken>();
    connHub->connRenderProcesses_[TEST_ID] = info;
    wptr<RSRenderServiceConnectHub> wptrConnHub = connHub;
    RSConnectRenderProcessDeathRecipient recipient(wptrConnHub, TEST_ID);
    sptr<IRemoteObject> remote = new IRemoteStub<RSIConnectionToken>();
    recipient.OnRemoteDied(remote);
    ASSERT_EQ(connHub->connRenderProcesses_.count(TEST_ID), 0);
}

/**
 * @tc.name: OnRemoteDiedTest004
 * @tc.desc: branch 4 - callback is not nullptr, invoke callback and ConnectRenderProcessDied
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectHubTest, OnRemoteDiedTest004, TestSize.Level1)
{
    auto connHub = RSRenderServiceConnectHub::GetInstance();
    constexpr uint64_t TEST_ID = 22222;
    RenderProcessInfo info;
    info.token = new IRemoteStub<RSIConnectionToken>();
    connHub->connRenderProcesses_[TEST_ID] = info;
    wptr<RSRenderServiceConnectHub> wptrConnHub = connHub;
    RSConnectRenderProcessDeathRecipient recipient(wptrConnHub, TEST_ID);
    bool invoked = false;
    recipient.SetOnRenderProcessDiedCallback([&invoked]() { invoked = true; });
    sptr<IRemoteObject> remote = new IRemoteStub<RSIConnectionToken>();
    recipient.OnRemoteDied(remote);
    ASSERT_TRUE(invoked);
    ASSERT_EQ(connHub->connRenderProcesses_.count(TEST_ID), 0);
}


} // namespace Rosen
} // namespace OHOS