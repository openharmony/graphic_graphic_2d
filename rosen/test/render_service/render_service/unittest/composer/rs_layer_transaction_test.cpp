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

#include "gtest/gtest.h"
#include <thread>

#include "hdi_output.h"
#include "rs_render_to_composer_connection.h"
#include "rs_render_composer.h"
#include "rs_render_composer_agent.h"
#include "rs_layer_transaction_handler.h"
#include "rs_surface_layer.h"

using namespace testing;
using namespace testing::ext;


namespace OHOS {
namespace Rosen {
class RSLayerTransactionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSLayerTransactionHandler> handler_ = nullptr;
    static inline std::shared_ptr<RSRenderComposer> rsRenderComposer = nullptr;
    static inline uint32_t screenId = 0;
};

void RSLayerTransactionTest::SetUp() {}

void RSLayerTransactionTest::TearDown() {}

void RSLayerTransactionTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    handler_ = std::make_shared<RSLayerTransactionHandler>();
    auto output = std::make_shared<HdiOutput>(screenId);
    rsRenderComposer = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer->runner_) {
        rsRenderComposer->runner_->Run();
    }
}

void RSLayerTransactionTest::TearDownTestCase()
{
    rsRenderComposer->uniRenderEngine_  = nullptr;
}

/**
 * @tc.name: SetRSComposerConnectionTest
 * @tc.desc: Test SetRSComposerConnection
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerTransactionTest, SetRSComposerConnectionTest, Level1)
{
    ASSERT_NE(handler_, nullptr);
    sptr<RSRenderToComposerConnection> composerConnection = nullptr;
    handler_->SetRSComposerConnection(composerConnection);
    ASSERT_EQ(handler_->rsComposerConnection_, nullptr);

    auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(rsRenderComposer);
    composerConnection = sptr<RSRenderToComposerConnection>::MakeSptr(
        "composer_conn", screenId, renderComposerAgent);
    handler_->SetRSComposerConnection(composerConnection);
    ASSERT_EQ(handler_->rsComposerConnection_, composerConnection);
}

/**
 * @tc.name: CommitLayersTransactionTest
 * @tc.desc: Test CommitLayersTransaction
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerTransactionTest, CommitLayersTransactionTest, Level1)
{
    ASSERT_NE(handler_, nullptr);
    ASSERT_NE(handler_->rsComposerConnection_, nullptr);
    handler_->CommitLayersTransaction();
}

/**
 * @tc.name: AddLayerTest
 * @tc.desc: Test AddLayer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerTransactionTest, AddLayerTest, Level1)
{
    ASSERT_NE(handler_, nullptr);
    ASSERT_NE(handler_->rsLayerTransactionData_, nullptr);
    ASSERT_EQ(handler_->rsLayerTransactionData_->layersVec_.size(), 0);
    std::shared_ptr<RSLayer> layer = std::make_shared<RSSurfaceLayer>();
    handler_->AddLayer(layer);
    ASSERT_EQ(handler_->rsLayerTransactionData_->layersVec_.size(), 1);
}

/**
 * @tc.name: ClearAllLayersTest
 * @tc.desc: Test ClearAllLayers
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLayerTransactionTest, ClearAllLayersTest, Level1)
{
    ASSERT_NE(handler_, nullptr);
    handler_->ClearAllLayers();
    ASSERT_EQ(handler_->rsLayerTransactionData_->layersVec_.size(), 0);
}
} // namespace Rosen
} // namespace OHOS