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
    auto context = std::make_shared<RSComposerContext>();
    EXPECT_NE(context, nullptr);

    auto handle = context->GetRSLayerTransaction();
    EXPECT_NE(handle, nullptr);

    context->rsLayerTransactionHandler_ = nullptr;
    auto output = std::make_shared<HdiOutput>(screenId);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto renderComposer = std::make_shared<RSRenderComposer>(output, property);
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    auto connect = sptr<RSRenderToComposerConnection>::MakeSptr("name", screenId, renderComposerAgent);
    // RSComposerContext API renamed
    context->SetRenderComposerClientConnection(connect);

    context->rsLayerTransactionHandler_ = std::make_shared<RSLayerTransactionHandler>();
    context->SetRenderComposerClientConnection(connect);
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
    auto context = std::make_shared<RSComposerContext>();
    EXPECT_NE(context, nullptr);

    context->rsLayerTransactionHandler_ = nullptr;
    auto output = std::make_shared<HdiOutput>(0);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    sMgr->OnScreenConnected(output, property);
    auto conn = sMgr->GetRSComposerConnection(0);
    sptr<IRSRenderToComposerConnection> ifaceConn = conn;
    auto client = RSRenderComposerClient::Create(false, ifaceConn, nullptr, nullptr);
    auto layer = std::make_shared<RSSurfaceLayer>();
    context->AddRSLayer(layer);
    ComposerInfo composerInfo;
    EXPECT_TRUE(context->CommitLayers(composerInfo));
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 0u);

    context->rsLayerTransactionHandler_ = std::make_shared<RSLayerTransactionHandler>();
    EXPECT_TRUE(context->CommitLayers(composerInfo));
    context->ClearAllRSLayers();
    EXPECT_EQ(client->GetUnExecuteTaskNum(), 0u);
}
} // namespace Rosen
} // namespace OHOS