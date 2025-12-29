/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include <hdi_backend.h>
#include <iremote_broker.h>
#include <iremote_stub.h>

#include "parameters.h"
#include "platform/ohos/transaction/zidl/rs_connect_to_render_process_proxy.h"
#include "render_server/rs_render_service.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
static inline sptr<RSConnectToRenderProcessProxy> rsConn_ = nullptr;
}
class RSConnectToRenderProcessProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSConnectToRenderProcessProxyTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService.Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    sptr<IRemoteObject> impl = nullptr;
    rsConn_ = sptr<RSConnectToRenderProcessProxy>::MakeSptr(impl);
}
void RSConnectToRenderProcessProxyTest::TearDownTestCase() {}
void RSConnectToRenderProcessProxyTest::SetUp() {}
void RSConnectToRenderProcessProxyTest::TearDown() {}

/**
 * @tc.name: CreateRenderConnectionTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSConnectToRenderProcessProxyTest, CreateRenderConnectionTest, TestSize.Level1)
{
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    rsConn_->CreateRenderConnection(token);
    ASSERT_TRUE(rsConn_);
}
} // namespace OHOS::Rosen
