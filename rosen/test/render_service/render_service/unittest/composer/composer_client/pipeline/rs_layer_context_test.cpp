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

#include "feature/hyper_graphic_manager/hgm_context.h"
#include "layer_backend/hdi_output.h"
#include "connection/rs_render_to_composer_connection.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "pipeline/rs_render_composer_agent.h"
#include "pipeline/rs_render_composer_client.h"
#include "pipeline/rs_render_composer_manager.h"
#include "screen_manager/rs_screen_property.h"
#include "transaction/rs_layer_transaction_data.h"
#include "rs_surface_layer.h"

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
    sMgr = std::make_shared<RSRenderComposerManager>(handler, nullptr);
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
    sptr<IRSRenderToComposerConnection> connnection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
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
    auto client = RSRenderComposerClient::Create(ifaceConn, nullptr, nullptr);
    auto layer = std::make_shared<RSSurfaceLayer>();
    auto context = std::make_shared<RSComposerContext>(ifaceConn);
    EXPECT_NE(context, nullptr);

    context->rsLayerTransactionHandler_ = nullptr;
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
    context->rsLayerTransactionHandler_->AddRSLayerParcel(baseParcel, 0);
    EXPECT_TRUE(context->CommitLayers(composerInfo));
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 0u);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    rsRenderComposer->uniRenderEngine_ = nullptr;
}
} // namespace Rosen
} // namespace OHOS