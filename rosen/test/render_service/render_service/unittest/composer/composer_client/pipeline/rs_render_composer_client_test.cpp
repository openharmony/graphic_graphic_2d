/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <iservice_registry.h>
#include <memory>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "connection/rs_render_to_composer_connection.h"
#include "feature/hyper_graphic_manager/hgm_context.h"
#include "layer/rs_surface_layer.h"
#include "layer_backend/hdi_output.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "pipeline/rs_render_composer_agent.h"
#include "pipeline/rs_composer_client.h"
#include "pipeline/rs_render_composer_manager.h"
#include "screen_manager/rs_screen_property.h"
#include "transaction/rs_layer_transaction_data.h"
#include "rs_surface_layer.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderComposerClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline uint32_t screenId = 0;
    static inline std::shared_ptr<RSComposerClient> client = nullptr;
};

void RSRenderComposerClientTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}

void RSRenderComposerClientTest::TearDownTestCase() {}

void RSRenderComposerClientTest::SetUp() {}
void RSRenderComposerClientTest::TearDown() {}

/**
 * @tc.name: ClientCreateTest
 * @tc.desc: Test Create Client
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderComposerClientTest, ClientCreateTest, Function | SmallTest | Level2)
{
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto mgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
    auto output = std::make_shared<HdiOutput>(screenId);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    mgr->OnScreenConnected(output, property);
    auto conn = mgr->GetRSComposerConnection(screenId);
    sptr<IRSRenderToComposerConnection> ifaceConn = conn;

    client = RSComposerClient::Create(ifaceConn, nullptr, nullptr);
    EXPECT_NE(client, nullptr);
}

/**
 * @tc.name: LayerFuncTest
 * @tc.desc: Test Func AddRSLayer RemoveRSLayer ClearAllRSLayer GetRSLayer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderComposerClientTest, LayerFuncTest, Function | SmallTest | Level2)
{
    auto layer = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(client, nullptr);
    auto context = client->GetComposerContext();
    ASSERT_NE(context, nullptr);
    context->AddRSLayer(layer);
}

/**
 * @tc.name: CommitRSLayerTest
 * @tc.desc: Test Func CommitLayers
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderComposerClientTest, CommitRSLayerTest, Function | SmallTest | Level2)
{
    auto layer = std::make_shared<RSSurfaceLayer>();
    layer->SetTunnelHandleChange(true);
    ASSERT_NE(client, nullptr);
    auto context = client->GetComposerContext();
    ASSERT_NE(context, nullptr);
    context->AddRSLayer(layer);
    ComposerInfo composerInfo;
    client->CommitLayers(composerInfo);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 0u);
    client->IncUnExecuteTaskNum();
    client->IncUnExecuteTaskNum();
    client->IncUnExecuteTaskNum();
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 3);
    client->CommitLayers(composerInfo);
    client->SubUnExecuteTaskNum();
    client->SubUnExecuteTaskNum();
    client->SubUnExecuteTaskNum();
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 0u);
    client->NotifyComposerThreadCanExecuteTask();
}

/**
 * @tc.name: ReleaseLayerBuffersTest
 * @tc.desc: Test Func ReleaseLayerBuffers
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderComposerClientTest, ReleaseLayerBuffersTest, Function | SmallTest | Level2)
{
    ASSERT_NE(client, nullptr);
    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>> timestampVec;
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseBufferFenceVec;
    client->IncUnExecuteTaskNum();
    client->ReleaseLayerBuffers(0, timestampVec, releaseBufferFenceVec);
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 0u);
    client->IncUnExecuteTaskNum();
    client->IncUnExecuteTaskNum();
    client->IncUnExecuteTaskNum();
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 3);
    client->ReleaseLayerBuffers(0, timestampVec, releaseBufferFenceVec);
    client->SubUnExecuteTaskNum();
    client->SubUnExecuteTaskNum();
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 0u);
}

/**
 * @tc.name: SetOutputTest
 * @tc.desc: Test Func SetOutput
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderComposerClientTest, SetOutputTest, Function | SmallTest | Level2)
{
    auto output = std::make_shared<HdiOutput>(0);
    ASSERT_NE(client, nullptr);
    client->SetOutput(output);
    ASSERT_NE(client->GetOutput(), nullptr);
}

/**
 * @tc.name: ClearRedrawGPUCompositionCacheTest
 * @tc.desc: Test Func ClearRedrawGPUCompositionCache
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderComposerClientTest, ClearRedrawGPUCompositionCacheTest, Function | SmallTest | Level2)
{
    ASSERT_NE(client, nullptr);
    std::unordered_set<uint64_t> bufferIds;
    client->ClearRedrawGPUCompositionCache(bufferIds);
    std::shared_ptr<HdiOutput> output = std::make_shared<HdiOutput>(0);
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    std::shared_ptr<RSRenderComposer> rsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    std::shared_ptr<RSRenderComposerAgent> rsRenderComposerAgent =
        std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
    sptr<IRSRenderToComposerConnection> renderToComposerConn =
        sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, rsRenderComposerAgent);
    sptr<IRSComposerToRenderConnection> composerToRenderConn = nullptr;
    sptr<RSVsyncManagerAgent> rsVsyncManagerAgent =
        sptr<RSVsyncManagerAgent>::MakeSptr(nullptr, nullptr);
    auto clientNormal = RSComposerClient::Create(renderToComposerConn,
        composerToRenderConn, rsVsyncManagerAgent);
    EXPECT_NE(clientNormal, nullptr);
    clientNormal->ClearRedrawGPUCompositionCache(bufferIds);
}

/**
 * @tc.name: SetScreenBacklightTest
 * @tc.desc: Test Func SetScreenBacklight
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderComposerClientTest, SetScreenBacklightTest, Function | SmallTest | Level2)
{
    ASSERT_NE(client, nullptr);
    client->SetScreenBacklight(0);
    std::shared_ptr<HdiOutput> output = std::make_shared<HdiOutput>(0);
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    std::shared_ptr<RSRenderComposer> rsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    std::shared_ptr<RSRenderComposerAgent> rsRenderComposerAgent =
        std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
    sptr<IRSRenderToComposerConnection> renderToComposerConn =
        sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, rsRenderComposerAgent);
    sptr<IRSComposerToRenderConnection> composerToRenderConn = nullptr;
    sptr<RSVsyncManagerAgent> rsVsyncManagerAgent =
        sptr<RSVsyncManagerAgent>::MakeSptr(nullptr, nullptr);
    auto clientNormal = RSComposerClient::Create(renderToComposerConn,
        composerToRenderConn, rsVsyncManagerAgent);
    EXPECT_NE(clientNormal, nullptr);
    clientNormal->SetScreenBacklight(0);
}
} // namespace Rosen
} // namespace OHOS