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
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "pipeline/rs_render_composer_agent.h"
#include "pipeline/rs_render_composer_client.h"
#include "pipeline/rs_render_composer_manager.h"
#include "rs_surface_layer.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSLayerContextTest : public testing::Test {
public:
    static inline uint32_t screenId = 0;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLayerContextTest::SetUpTestCase()
{
    RsVulkanContext::SetRecyclable(false);
}

void RSLayerContextTest::TearDownTestCase()
{
    RSRenderComposerManager::GetInstance().rsRenderComposerMap_[screenId]->uniRenderEngine_  = nullptr;
}
void RSLayerContextTest::SetUp() {}
void RSLayerContextTest::TearDown() {}

/**
 * @tc.name: InitContextTest
 * @tc.desc: Test RSLayerContext
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerContextTest, InitContextTest, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RSLayerContext>();
    EXPECT_NE(context, nullptr);

    auto handle = context->GetRSLayerTransaction();
    EXPECT_NE(handle, nullptr);

    context->rsLayerTransactionHandler_ = nullptr;
    auto output = std::make_shared<HdiOutput>(screenId);
    auto renderComposer = std::make_shared<RSRenderComposer>(output);
    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
    auto connect = sptr<RSRenderToComposerConnection>::MakeSptr("name", screenId, renderComposerAgent);
    context->SetRSRenderComposerClientConnection(connect);

    context->rsLayerTransactionHandler_ = std::make_shared<RSLayerTransactionHandler>();
    context->SetRSRenderComposerClientConnection(connect);
    EXPECT_EQ(context->rsLayerTransactionHandler_->rsComposerConnection_, connect);
}

/**
 * @tc.name: LayerFuncTest
 * @tc.desc: Test Func CommitRSLayer AddRSLayer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerContextTest, LayerFuncTest, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RSLayerContext>();
    EXPECT_NE(context, nullptr);

    context->rsLayerTransactionHandler_ = nullptr;
    auto output = std::make_shared<HdiOutput>(0);
    RSRenderComposerManager::GetInstance().OnScreenConnected(output);
    auto client = std::make_shared<RSRenderComposerClient>(
        RSRenderComposerManager::GetInstance().rsComposerConnectionMap_[0]);
    auto layer = std::make_shared<RSSurfaceLayer>();
    context->AddRSLayer(layer);
    context->CommitRSLayer();
    EXPECT_EQ(RSRenderComposerManager::GetInstance().rsRenderComposerMap_[0]->unExecuteTaskNum_, 0);

    context->rsLayerTransactionHandler_ = std::make_shared<RSLayerTransactionHandler>();
    context->CommitRSLayer();
    context->ClearAllLayers();
    EXPECT_EQ(RSRenderComposerManager::GetInstance().rsRenderComposerMap_[0]->unExecuteTaskNum_, 0);
}
} // namespace Rosen
} // namespace OHOS