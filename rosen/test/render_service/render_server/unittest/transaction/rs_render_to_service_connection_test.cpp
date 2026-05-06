/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <iremote_broker.h>
#include <refbase.h>

#include "irs_render_to_composer_connection.h"
#include "parameters.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "rs_render_process_manager.h"
#include "rs_render_service.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/public/rs_screen_manager_agent.h"
#include "transaction/rs_render_to_service_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
sptr<RSRenderToServiceConnection> g_rsConn = nullptr;
}

class RSRenderToServiceConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderToServiceConnectionTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService);
    sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent =
        sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService.renderProcessManager_);
    auto rsScreenManager = sptr<RSScreenManager>::MakeSptr();
    sptr<RSScreenManagerAgent> screenManagerAgent = sptr<RSScreenManagerAgent>::MakeSptr(rsScreenManager);
    g_rsConn = sptr<RSRenderToServiceConnection>::MakeSptr(renderServiceAgent,
        renderProcessManagerAgent, screenManagerAgent);
}
void RSRenderToServiceConnectionTest::TearDownTestCase() {}
void RSRenderToServiceConnectionTest::SetUp() {}
void RSRenderToServiceConnectionTest::TearDown() {}

/**
 * @tc.name: NotifyRpHgmFrameRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderToServiceConnectionTest, NotifyRpHgmFrameRateTest, TestSize.Level1)
{
    uint64_t timestamp = 1;
    uint64_t vsyncId = 1;
    sptr<HgmProcessToServiceInfo> processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
    ASSERT_TRUE(g_rsConn->NotifyRpHgmFrameRate(timestamp, vsyncId, processToServiceInfo));
}

/**
 * @tc.name: NotifyScreenSwitchFinishedTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderToServiceConnectionTest, NotifyScreenSwitchFinishedTest, TestSize.Level1)
{
    ScreenId screenId = 1;
    g_rsConn->NotifyScreenSwitchFinished(screenId);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: NotifyRenderProcessInitFinishedTest
 * @tc.desc: Test NotifyRenderProcessInitFinished with null remote objects
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderToServiceConnectionTest, NotifyRenderProcessInitFinishedTest, TestSize.Level1)
{
    sptr<IRemoteObject> serviceToRenderConnection = nullptr;
    sptr<IRemoteObject> connectToRenderConnection = nullptr;
    auto ret = g_rsConn->NotifyRenderProcessInitFinished(serviceToRenderConnection, connectToRenderConnection);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: SendProcessInfoNullInfoTest
 * @tc.desc: Test SendProcessInfo with null connectToServiceInfo
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderToServiceConnectionTest, SendProcessInfoNullInfoTest, TestSize.Level1)
{
    sptr<ConnectToServiceInfo> connectToServiceInfo = nullptr;
    auto ret = g_rsConn->SendProcessInfo(connectToServiceInfo);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: SendProcessInfoNullComposerConnTest
 * @tc.desc: Test SendProcessInfo with null composerToRenderConnection_
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderToServiceConnectionTest, SendProcessInfoNullComposerConnTest, TestSize.Level1)
{
    auto connectToServiceInfo = sptr<ConnectToServiceInfo>::MakeSptr(nullptr, nullptr);
    auto ret = g_rsConn->SendProcessInfo(connectToServiceInfo);
    EXPECT_EQ(ret, nullptr);
}

} // namespace OHOS::Rosen
