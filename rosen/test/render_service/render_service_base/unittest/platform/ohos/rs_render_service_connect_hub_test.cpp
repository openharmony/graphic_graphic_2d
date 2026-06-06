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
#include <iremote_stub.h>
#include "transaction/rs_application_agent_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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
    RSApplicationAgentImpl::Instance();

    RSRenderServiceConnectHub::Init();
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
} // namespace Rosen
} // namespace OHOS