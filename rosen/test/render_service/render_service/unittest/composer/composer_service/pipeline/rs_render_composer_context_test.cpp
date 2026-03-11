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

#include <gtest/gtest.h>

#include "rs_render_composer_context.h"
#include "rs_layer.h"
#include "rs_render_surface_layer.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsRenderComposerContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsRenderComposerContextTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RsRenderComposerContextTest::TearDownTestCase() {}
void RsRenderComposerContextTest::SetUp() {}
void RsRenderComposerContextTest::TearDown() {}


/**
 * Function: GetRSLayersVec_Empty_ReturnsEmpty
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext
 *                  2. call GetNeedCompositionLayersVec()
 *                  3. check returned vector is empty
 */
HWTEST_F(RsRenderComposerContextTest, GetRSLayersVec_Empty_ReturnsEmpty, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    auto vec = ctx->GetNeedCompositionLayersVec();
    EXPECT_TRUE(vec.empty());
}

/**
 * Function: GetRSLayersVec_FiltersByisNeedComposition
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext and add two layers (one used, one unused)
 *                  2. call GetNeedCompositionLayersVec()
 *                  3. check returned vector only contains the used layer
 */
HWTEST_F(RsRenderComposerContextTest, GetRSLayersVec_FiltersByisNeedComposition, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    auto l1 = std::make_shared<RSRenderSurfaceLayer>();
    l1->SetIsNeedComposition(false);
    l1->SetSurfaceName("L1");
    auto l2 = std::make_shared<RSRenderSurfaceLayer>();
    l2->SetIsNeedComposition(true);
    l2->SetSurfaceName("L2");
    ctx->AddRSRenderLayer(1, l1);
    ctx->AddRSRenderLayer(2, l2);

    auto vec = ctx->GetNeedCompositionLayersVec();
    ASSERT_EQ(vec.size(), 1u);
    EXPECT_EQ(vec[0], l2);
}

/**
 * Function: GetRSRenderLayers_FoundAndNotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext and add layer with id 100
 *                  2. call GetRSRenderLayer() with id 100 and 200
 *                  3. check found 100 and not found 200
 */
/**
 * Function: GetRSRenderLayer_Found_And_NotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. add layer id=100
 *                  2. GetRSRenderLayer(100)!=nullptr, GetRSRenderLayer(200)==nullptr
 */
HWTEST_F(RsRenderComposerContextTest, GetRSRenderLayer_Found_And_NotFound, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetIsNeedComposition(true);
    ctx->AddRSRenderLayer(100, layer);
    auto found = ctx->GetRSRenderLayer(100);
    auto notFound = ctx->GetRSRenderLayer(200);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(notFound, nullptr);
}

/**
 * Function: AddRSRenderLayer_OverrideExisting
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext and add layer
 *                  2. re-add layer with same id but different instance
 *                  3. find layer id when first add, and check it's the new instance after override
 */
/**
 * Function: RemoveRSRenderLayer_Existing_And_NonExisting
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. add two layers
 *                  2. remove existing id then non-existing id; verify counts
 */
HWTEST_F(RsRenderComposerContextTest, RemoveRSRenderLayer_Existing_And_NonExisting, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    auto l1 = std::make_shared<RSRenderSurfaceLayer>();
    l1->SetIsNeedComposition(true);
    auto l2 = std::make_shared<RSRenderSurfaceLayer>();
    l2->SetIsNeedComposition(true);
    ctx->AddRSRenderLayer(1, l1);
    ctx->AddRSRenderLayer(2, l2);
    ASSERT_EQ(ctx->GetRSRenderLayerCount(), 2u);
    ctx->RemoveRSRenderLayer(1);
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);
    // removing non-existing should keep count unchanged
    ctx->RemoveRSRenderLayer(1000);
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);
}

/**
 * Function: AddRSRenderLayer_OverrideExisting
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. add a layer with id=10
 *                  2. add another different instance with same id
 *                  3. verify count remains 1 and stored pointer is the new instance
 */
HWTEST_F(RsRenderComposerContextTest, AddRSRenderLayer_OverrideExisting, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    auto first = std::make_shared<RSRenderSurfaceLayer>();
    first->SetIsNeedComposition(true);
    first->SetSurfaceName("first");
    ctx->AddRSRenderLayer(10, first);
    ASSERT_EQ(ctx->GetRSRenderLayerCount(), 1u);
    auto stored1 = ctx->GetRSRenderLayer(10);
    ASSERT_EQ(stored1, first);

    auto second = std::make_shared<RSRenderSurfaceLayer>();
    second->SetIsNeedComposition(true);
    second->SetSurfaceName("second");
    ctx->AddRSRenderLayer(10, second);

    // override should not increase count
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);
    auto stored2 = ctx->GetRSRenderLayer(10);
    EXPECT_EQ(stored2, second);
}

/**
 * Function: GetRSLayersVec_AllUnused_ReturnsEmpty
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. add two layers both with isNeedComposition=false
 *                  2. GetNeedCompositionLayersVec returns empty
 */
HWTEST_F(RsRenderComposerContextTest, GetRSLayersVec_AllUnused_ReturnsEmpty, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    auto a = std::make_shared<RSRenderSurfaceLayer>();
    a->SetIsNeedComposition(false);
    a->SetSurfaceName("A");
    auto b = std::make_shared<RSRenderSurfaceLayer>();
    b->SetIsNeedComposition(false);
    b->SetSurfaceName("B");
    ctx->AddRSRenderLayer(1, a);
    ctx->AddRSRenderLayer(2, b);
    auto vec = ctx->GetNeedCompositionLayersVec();
    EXPECT_TRUE(vec.empty());
}

/**
 * Function: RemoveRSRenderLayer_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext and add layer
 *                  2. remove layer by existing id and non-existing id
 *                  3. check layer count after add and removes
 */
HWTEST_F(RsRenderComposerContextTest, SetRSLayersVec_Override, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    auto base = std::make_shared<RSRenderSurfaceLayer>();
    base->SetIsNeedComposition(true);
    ctx->AddRSRenderLayer(1, base);
    ASSERT_EQ(ctx->GetRSRenderLayerCount(), 1u);

    // override by removing old and adding new layers
    ctx->RemoveRSRenderLayer(1);
    auto a = std::make_shared<RSRenderSurfaceLayer>();
    a->SetIsNeedComposition(true);
    auto b = std::make_shared<RSRenderSurfaceLayer>();
    b->SetIsNeedComposition(true);
    ctx->AddRSRenderLayer(2, a);
    ctx->AddRSRenderLayer(3, b);
    auto vec = ctx->GetNeedCompositionLayersVec();
    EXPECT_EQ(vec.size(), 2u);
}

/**
 * Function: RemoveRSRenderLayer_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext
 *                  2. add two layers
 *                  3. check GetRSRenderLayerCount() returns accurate count
 */
HWTEST_F(RsRenderComposerContextTest, SetRSLayersVec_WithEmptyVector, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    // without adding any layers, returned vector should be empty
    EXPECT_TRUE(ctx->GetNeedCompositionLayersVec().empty());
}

/**
 * Function: GetRSLayersVec_ToggleNeedComposition
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. add one layer with needComposition=true
 *                  2. verify it appears in vector
 *                  3. set needComposition=false and verify it is filtered out
 */
HWTEST_F(RsRenderComposerContextTest, GetRSLayersVec_ToggleNeedComposition, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetSurfaceName("toggle");
    layer->SetIsNeedComposition(true);
    ctx->AddRSRenderLayer(77, layer);
    auto vec1 = ctx->GetNeedCompositionLayersVec();
    ASSERT_EQ(vec1.size(), 1u);
    EXPECT_EQ(vec1[0], layer);

    layer->SetIsNeedComposition(false);
    auto vec2 = ctx->GetNeedCompositionLayersVec();
    EXPECT_TRUE(vec2.empty());
}

} // namespace Rosen
} // namespace OHOS
