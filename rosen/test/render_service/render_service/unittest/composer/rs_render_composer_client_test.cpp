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
#include "layer/rs_layer_factory.h"
#include "layer_backend/hdi_output.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "pipeline/rs_render_composer_agent.h"
#include "pipeline/rs_render_composer_client.h"
#include "pipeline/rs_render_composer_manager.h"
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
    static inline std::shared_ptr<RSRenderComposerClient> client = nullptr;
};

void RSRenderComposerClientTest::SetUpTestCase()
{
    RsVulkanContext::SetRecyclable(false);
}

void RSRenderComposerClientTest::TearDownTestCase()
{
    RSRenderComposerManager::GetInstance().rsRenderComposerMap_[screenId]->uniRenderEngine_ = nullptr;
}

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
    auto output = std::make_shared<HdiOutput>(screenId);
    RSRenderComposerManager::GetInstance().OnScreenConnected(output);

    client = std::make_shared<RSRenderComposerClient>(
        RSRenderComposerManager::GetInstance().rsComposerConnectionMap_[screenId]);
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
    client->AddRSLayer(layer);
}

/**
 * @tc.name: CommitRSLayerTest
 * @tc.desc: Test Func CommitRSLayer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderComposerClientTest, CommitRSLayerTest, Function | SmallTest | Level2)
{
    auto layer = std::make_shared<RSSurfaceLayer>();
    layer->SetTunnelHandleChange(true);
    client->AddRSLayer(layer);
    client->CommitRSLayer();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(RSRenderComposerManager::GetInstance().rsRenderComposerMap_[screenId]->unExecuteTaskNum_, 0);
}
} // namespace Rosen
} // namespace OHOS