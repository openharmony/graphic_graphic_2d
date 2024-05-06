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

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "platform/ohos/rs_render_service_proxy.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline sptr<RSIRenderService> renderService = nullptr;
};

void RSProxyTest::SetUpTestCase()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    ASSERT_NE(remoteObject, nullptr);
    ASSERT_EQ(remoteObject->IsProxyObject(), true);
    renderService = iface_cast<RSRenderServiceProxy>(remoteObject);
}
void RSProxyTest::TearDownTestCase() {}
void RSProxyTest::SetUp() {}
void RSProxyTest::TearDown() {}

/**
 * @tc.name: CreateConnection Test
 * @tc.desc: CreateConnection Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSProxyTest, CreateConnection_Test, TestSize.Level1)
{
    ASSERT_NE(renderService, nullptr);
    sptr<RSIRenderServiceConnection> conn = renderService->CreateConnection(nullptr);
    ASSERT_EQ(conn, nullptr);
}

/**
 * @tc.name: CreateConnection Test
 * @tc.desc: CreateConnection Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSProxyTest, CreateConnection, TestSize.Level1)
{
    ASSERT_NE(renderService, nullptr);
    MessageParcel data;
    auto remoteObj = data.ReadRemoteObject();
    sptr<RSIConnectionToken>  token = new IRemoteStub<RSIConnectionToken>();
    ASSERT_NE(token, nullptr);
    sptr<RSIRenderServiceConnection> conn = renderService->CreateConnection(token);
    ASSERT_NE(conn, nullptr);
}

/**
 * @tc.name: SetRenderContext Test
 * @tc.desc: SetRenderContext Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSProxyTest, SetRenderContext, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosRaster raster(width, height);
    std::unique_ptr<RenderContext> renderContext = std::make_unique<RenderContext>();
    raster.SetRenderContext(renderContext.get());
    ASSERT_NE(raster.renderContext_, nullptr);
}
} // namespace Rosen
} // namespace OHOS