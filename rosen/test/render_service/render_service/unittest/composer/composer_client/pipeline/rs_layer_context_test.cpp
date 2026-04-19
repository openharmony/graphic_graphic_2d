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

#include "consumer_surface.h"
#include "feature/hyper_graphic_manager/hgm_context.h"
#include "layer_backend/hdi_output.h"
#include "connection/rs_render_to_composer_connection.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "pipeline/rs_render_composer_agent.h"
#include "pipeline/rs_composer_client.h"
#include "pipeline/rs_render_composer_manager.h"
#include "rs_render_surface_layer.h"
#include "rs_surface_layer.h"
#include "screen_manager/rs_screen_property.h"
#include "surface_buffer_impl.h"
#include "transaction/rs_layer_transaction_data.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSLayerContextTest : public testing::Test {
public:
    static inline uint32_t screenId = 0;
    static inline std::shared_ptr<RSRenderComposerManager> sMgr = nullptr;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLayerContextTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    sMgr = std::make_shared<RSRenderComposerManager>(handler);
}

void RSLayerContextTest::TearDownTestCase()
{
    // No global singleton; ensure manager exists and no crash occurs.
    sMgr.reset();
}
void RSLayerContextTest::SetUp() {}
void RSLayerContextTest::TearDown() {}

/**
 * @tc.name: InitContextTest
 * @tc.desc: Test RSComposerContext
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerContextTest, InitContextTest, Function | SmallTest | Level2)
{
    auto output = std::make_shared<HdiOutput>(screenId);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto renderComposer = std::make_shared<RSRenderComposer>(output, property);
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    auto connect = sptr<RSRenderToComposerConnection>::MakeSptr("name", screenId, renderComposerAgent);
    // RSComposerContext API renamed
    auto context = std::make_shared<RSComposerContext>(connect);
    EXPECT_NE(context, nullptr);

    auto handle = context->GetRSLayerTransaction();
    EXPECT_NE(handle, nullptr);
    context->rsLayerTransactionHandler_ = std::make_shared<RSLayerTransactionHandler>();
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    context->rsLayerTransactionHandler_->SetRSComposerConnectionProxy(connection);
    EXPECT_NE(context->rsLayerTransactionHandler_->rsComposerConnection_, nullptr);
}

/**
 * @tc.name: LayerFuncTest
 * @tc.desc: Test Func CommitLayers AddRSLayer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerContextTest, LayerFuncTest, Function | SmallTest | Level2)
{
    auto output = std::make_shared<HdiOutput>(0);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    sMgr->OnScreenConnected(output, property);
    auto conn = sMgr->GetRSComposerConnection(0);
    sptr<IRSRenderToComposerConnection> ifaceConn = conn;
    auto client = RSComposerClient::Create(ifaceConn, nullptr);
    auto context = std::make_shared<RSComposerContext>(ifaceConn);
    EXPECT_NE(context, nullptr);
    std::shared_ptr<RSSurfaceLayer> layer = nullptr;
    context->AddRSLayer(layer);
    context->RemoveRSLayer(0);
    layer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    context->rsLayerTransactionHandler_ = nullptr;
    context->AddRSLayer(layer);
    context->AddRSLayer(layer);
    ComposerInfo composerInfo;
    EXPECT_FALSE(context->CommitLayers(composerInfo));
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 0u);

    context->rsLayerTransactionHandler_ = std::make_shared<RSLayerTransactionHandler>();
    EXPECT_FALSE(context->CommitLayers(composerInfo));
    auto rsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
    auto composerConnection = sptr<RSRenderToComposerConnection>::MakeSptr(
        "composer_conn", 0, renderComposerAgent);
    context->rsLayerTransactionHandler_->SetRSComposerConnectionProxy(composerConnection);
    EXPECT_FALSE(context->CommitLayers(composerInfo));
    context->rsLayerTransactionHandler_->rsLayerTransactionData_ = std::make_unique<RSLayerTransactionData>();
    class FakeLayerParcel : public RSLayerParcel {
    public:
        uint16_t GetRSLayerParcelType() const override { return 0; }
        RSLayerId GetRSLayerId() const override { return 0; }
        bool Marshalling(OHOS::MessageParcel& /*parcel*/) const override { return true; }
        void ApplyRSLayerCmd(std::shared_ptr<RSRenderComposerContext> /*context*/) override {}
    };
    auto parcel = std::make_shared<FakeLayerParcel>();
    std::shared_ptr<RSLayerParcel> baseParcel = std::static_pointer_cast<RSLayerParcel>(parcel);
    context->rsLayerTransactionHandler_->AddRSLayerParcel(0, baseParcel);
    EXPECT_TRUE(context->CommitLayers(composerInfo));
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 0u);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    rsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: DumpLayersInfoTest
 * @tc.desc: Test Func DumpLayersInfo
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerContextTest, DumpLayersInfoTest, Function | SmallTest | Level2)
{
    auto output = std::make_shared<HdiOutput>(screenId);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto renderComposer = std::make_shared<RSRenderComposer>(output, property);
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    auto connect = sptr<RSRenderToComposerConnection>::MakeSptr("name", screenId, renderComposerAgent);
    // RSComposerContext API renamed
    auto context = std::make_shared<RSComposerContext>(connect);
    EXPECT_NE(context, nullptr);

    std::string dumpString;
    context->DumpLayersInfo(dumpString);
    std::shared_ptr<RSSurfaceLayer> layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->SetNodeId(0);
    context->AddRSLayer(layer);
    context->DumpLayersInfo(dumpString);
    context->lastCommitLayersId_.push_back(0);
    context->DumpLayersInfo(dumpString);
}

/**
 * @tc.name: DumpCurrentFrameLayersTest
 * @tc.desc: Test Func DumpCurrentFrameLayers
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerContextTest, DumpCurrentFrameLayersTest, Function | SmallTest | Level2)
{
    auto output = std::make_shared<HdiOutput>(screenId);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto renderComposer = std::make_shared<RSRenderComposer>(output, property);
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    auto connect = sptr<RSRenderToComposerConnection>::MakeSptr("name", screenId, renderComposerAgent);
    // RSComposerContext API renamed
    auto context = std::make_shared<RSComposerContext>(connect);
    EXPECT_NE(context, nullptr);
    context->DumpCurrentFrameLayers();
    std::shared_ptr<RSSurfaceLayer> layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->SetNodeId(0);
    context->AddRSLayer(layer);
    context->DumpCurrentFrameLayers();
}

/**
 * @tc.name: ReleaseLayerBuffersTest
 * @tc.desc: Test Func ReleaseLayerBuffers
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerContextTest, ReleaseLayerBuffersTest, Function | SmallTest | Level2)
{
    auto output = std::make_shared<HdiOutput>(screenId);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto renderComposer = std::make_shared<RSRenderComposer>(output, property);
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    auto connect = sptr<RSRenderToComposerConnection>::MakeSptr("name", screenId, renderComposerAgent);
    // RSComposerContext API renamed
    auto context = std::make_shared<RSComposerContext>(connect);
    EXPECT_NE(context, nullptr);

    GraphicPresentTimestamp timeStamp;
    std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>> timestampVec;
    timestampVec.push_back({0, false, timeStamp});

    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseBufferFenceVec;
    releaseBufferFenceVec.push_back({0, nullptr, nullptr});
    context->ReleaseLayerBuffers(0, timestampVec, releaseBufferFenceVec);

    std::shared_ptr<RSSurfaceLayer> layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->SetRSLayerId(0);
    context->AddRSLayer(layer);
    sptr<IConsumerSurface> surface = sptr<ConsumerSurface>::MakeSptr("test");
    layer->SetSurface(surface);
    context->ReleaseLayerBuffers(0, timestampVec, releaseBufferFenceVec);
}

/**
 * @tc.name: ClearFrameBuffersTest
 * @tc.desc: Test Func ClearFrameBuffers
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerContextTest, ClearFrameBuffersTest, Function | SmallTest | Level2)
{
    auto output = std::make_shared<HdiOutput>(screenId);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto renderComposer = std::make_shared<RSRenderComposer>(output, property);
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    auto connect = sptr<RSRenderToComposerConnection>::MakeSptr("name", screenId, renderComposerAgent);
    // RSComposerContext API renamed
    auto context = std::make_shared<RSComposerContext>(connect);
    EXPECT_NE(context, nullptr);

    context->ClearFrameBuffers();
    context->CleanLayerBufferBySurfaceId(0);
    sptr<SurfaceBuffer> sbuffer = sptr<SurfaceBufferImpl>::MakeSptr();
    context->PreAllocProtectedFrameBuffers(sbuffer);
}
} // namespace Rosen
} // namespace OHOS