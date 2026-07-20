/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "layer/rs_surface_solid_filled_color_layer.h"
#include "pipeline/rs_composer_client.h"
#include "pipeline/rs_render_composer_agent.h"
#include "pipeline/rs_render_composer_manager.h"
#include "pipeline/rs_surface_handler.h"
#include "rs_composer_context.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint64_t TEST_LAYER_ID = 1200u;
} // namespace

class RSSurfaceSolidFilledColorLayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceSolidFilledColorLayerTest::SetUpTestCase() {}
void RSSurfaceSolidFilledColorLayerTest::TearDownTestCase() {}
void RSSurfaceSolidFilledColorLayerTest::SetUp() {}
void RSSurfaceSolidFilledColorLayerTest::TearDown() {}

/**
 * Function: Create_ContextNullptr_ReturnNullptr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Create with nullptr context
 *                  2. verify nullptr is returned
 *                   Cover branch: context == nullptr (line 35 true)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ContextNullptr_ReturnNullptr, TestSize.Level1)
{
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, nullptr);
    EXPECT_EQ(layer, nullptr);
}

/**
 * Function: Create_ContextValidNoCache_NewLayerCreated
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create valid context
 *                  2. call Create when no cached layer exists
 *                  3. verify new layer is created and added to context
 *                   Cover branch: context != nullptr (line 35 false), layer == nullptr (line 40 first condition false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ContextValidNoCache_NewLayerCreated, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());

    auto cachedLayer = context->GetRSLayer(TEST_LAYER_ID);
    EXPECT_NE(cachedLayer, nullptr);
}

/**
 * Function: Create_CachedLayerNotSolidFilledColor_NewLayerCreated
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create context and add non-solid-filled-color layer
 *                  2. call Create for same layer id
 *                  3. verify new solid filled color layer is created
 *                   Cover branch: context != nullptr (line 35 false),
 *                                  layer != nullptr && !IsSolidFilledColorLayer() (line 40 false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_CachedLayerNotSolidFilledColor_NewLayerCreated, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto normalLayer = std::make_shared<RSSurfaceLayer>(TEST_LAYER_ID, context);
    normalLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    context->AddRSLayer(normalLayer);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}

/**
 * Function: Create_CachedSolidFilledColorLayer_ReuseCached
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create context and solid filled color layer
 *                  2. call Create for same layer id
 *                  3. verify cached layer is reused
 *                   Cover branch: context != nullptr (line 35 false),
 *                                  layer != nullptr && IsSolidFilledColorLayer() (line 40 true)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_CachedSolidFilledColorLayer_ReuseCached, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer1 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer1, nullptr);
    EXPECT_TRUE(layer1->IsSolidFilledColorLayer());

    auto layer2 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer2, nullptr);
    EXPECT_TRUE(layer2->IsSolidFilledColorLayer());

    EXPECT_EQ(layer1.get(), layer2.get());
}

/**
 * Function: Create_MultipleLayers_EachHasUniqueId
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create context
 *                  2. create multiple solid filled color layers with different ids
 *                  3. verify each has unique pointer
 *                   Cover branch: context != nullptr (line 35 false), layer == nullptr (line 40 first condition false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_MultipleLayers_EachHasUniqueId, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer1 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    auto layer2 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID + 1, context);

    ASSERT_NE(layer1, nullptr);
    ASSERT_NE(layer2, nullptr);
    EXPECT_NE(layer1.get(), layer2.get());
}

/**
 * Function: Create_ValidContext_ReturnsSolidFilledColorLayer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create valid context
 *                  2. call Create
 *                  3. verify returned layer is solid filled color layer
 *                   Cover branch: context != nullptr (line 35 false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ValidContext_ReturnsSolidFilledColorLayer, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}

/**
 * Function: Create_ReuseCachedLayer_RsLayerIdUpdated
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create cached solid filled color layer
 *                  2. call Create with new layer id
 *                  3. verify layer id is updated
 *                   Cover branch: layer != nullptr && IsSolidFilledColorLayer() (line 40 true)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ReuseCachedLayer_RsLayerIdUpdated, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer1 = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer1, nullptr);
    EXPECT_EQ(layer1->GetRSLayerId(), TEST_LAYER_ID);

    constexpr uint64_t NEW_LAYER_ID = TEST_LAYER_ID + 100;
    auto layer2 = RSSurfaceSolidFilledColorLayer::Create(NEW_LAYER_ID, context);
    ASSERT_NE(layer2, nullptr);

    EXPECT_EQ(layer2->GetRSLayerId(), NEW_LAYER_ID);
}

/**
 * Function: IsSolidFilledColorLayer_AlwaysTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer
 *                  2. call IsSolidFilledColorLayer
 *                  3. verify always returns true
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, IsSolidFilledColorLayer_AlwaysTrue, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}

/**
 * Function: Create_ContextWithNullptrInnerContext_StillWorks
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create context with nullptr inner context
 *                  2. call Create
 *                  3. verify layer is created successfully
 *                   Cover branch: context != nullptr (line 35 false)
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Create_ContextWithNullptrInnerContext_StillWorks, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);

    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}

/**
 * Function: Inherited_SetAlpha
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetAlpha/GetAlpha interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetAlpha, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicLayerAlpha alpha;
    alpha.enPixelAlpha = true;
    layer->SetAlpha(alpha);
    EXPECT_EQ(layer->GetAlpha().enPixelAlpha, true);
}

/**
 * Function: Inherited_SetZorder
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetZorder/GetZorder interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetZorder, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetZorder(100);
    EXPECT_EQ(layer->GetZorder(), 100u);
}

/**
 * Function: Inherited_SetType
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetType/GetType interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetType, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicLayerType type = GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC;
    layer->SetType(type);
    EXPECT_EQ(layer->GetType(), type);
}

/**
 * Function: Inherited_SetTransform
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTransform/GetTransform interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetTransform, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_90;
    layer->SetTransform(transform);
    EXPECT_EQ(layer->GetTransform(), transform);
}

/**
 * Function: Inherited_SetCompositionType
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetCompositionType/GetCompositionType interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetCompositionType, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicCompositionType type = GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT;
    layer->SetCompositionType(type);
    EXPECT_EQ(layer->GetCompositionType(), type);
}

/**
 * Function: Inherited_SetBlendType
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetBlendType/GetBlendType interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetBlendType, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicBlendType type = GraphicBlendType::GRAPHIC_BLEND_CLEAR;
    layer->SetBlendType(type);
    EXPECT_EQ(layer->GetBlendType(), type);
}

/**
 * Function: Inherited_SetCropRect
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetCropRect/GetCropRect interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetCropRect, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicIRect crop = {1, 2, 100, 200};
    layer->SetCropRect(crop);
    EXPECT_EQ(layer->GetCropRect().x, crop.x);
    EXPECT_EQ(layer->GetCropRect().w, crop.w);
}

/**
 * Function: Inherited_SetLayerSize
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerSize/GetLayerSize interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetLayerSize, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicIRect rect = {10, 20, 300, 400};
    layer->SetLayerSize(rect);
    EXPECT_EQ(layer->GetLayerSize().x, rect.x);
    EXPECT_EQ(layer->GetLayerSize().h, rect.h);
}

/**
 * Function: Inherited_SetBoundSize
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetBoundSize/GetBoundSize interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetBoundSize, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicIRect rect = {5, 10, 150, 250};
    layer->SetBoundSize(rect);
    EXPECT_EQ(layer->GetBoundSize().y, rect.y);
    EXPECT_EQ(layer->GetBoundSize().w, rect.w);
}

/**
 * Function: Inherited_SetVisibleRegions
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetVisibleRegions/GetVisibleRegions interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetVisibleRegions, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    std::vector<GraphicIRect> regions = {{0, 0, 100, 100}, {10, 10, 50, 50}};
    layer->SetVisibleRegions(regions);
    EXPECT_EQ(layer->GetVisibleRegions().size(), regions.size());
}

/**
 * Function: Inherited_SetDirtyRegions
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetDirtyRegions/GetDirtyRegions interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetDirtyRegions, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    std::vector<GraphicIRect> regions = {{0, 0, 50, 50}};
    layer->SetDirtyRegions(regions);
    EXPECT_EQ(layer->GetDirtyRegions().size(), regions.size());
}

/**
 * Function: Inherited_SetPreMulti
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetPreMulti/IsPreMulti interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetPreMulti, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetPreMulti(true);
    EXPECT_EQ(layer->IsPreMulti(), true);
}

/**
 * Function: Inherited_SetLayerColor
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerColor/GetLayerColor interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetLayerColor, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicLayerColor color = {100, 150, 200, 255};
    layer->SetLayerColor(color);
    EXPECT_EQ(layer->GetLayerColor().r, color.r);
    EXPECT_EQ(layer->GetLayerColor().g, color.g);
}

/**
 * Function: Inherited_SetBackgroundColor
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetBackgroundColor/GetBackgroundColor interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetBackgroundColor, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicLayerColor color = {50, 75, 100, 128};
    layer->SetBackgroundColor(color);
    EXPECT_EQ(layer->GetBackgroundColor().r, color.r);
}

/**
 * Function: Inherited_SetColorDataSpace
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetColorDataSpace/GetColorDataSpace interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetColorDataSpace, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicColorDataSpace colorSpace = GraphicColorDataSpace::GRAPHIC_GAMUT_BT709;
    layer->SetColorDataSpace(colorSpace);
    EXPECT_EQ(layer->GetColorDataSpace(), colorSpace);
}

/**
 * Function: Inherited_SetColorTransform
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetColorTransform/GetColorTransform interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetColorTransform, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    std::vector<float> matrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    layer->SetColorTransform(matrix);
    EXPECT_EQ(layer->GetColorTransform().size(), matrix.size());
}

/**
 * Function: Inherited_SetMatrix
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetMatrix/GetMatrix interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetMatrix, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicMatrix matrix;
    matrix.scaleX = 2.0f;
    matrix.scaleY = 2.0f;
    layer->SetMatrix(matrix);
    EXPECT_EQ(layer->GetMatrix().scaleX, matrix.scaleX);
}

/**
 * Function: Inherited_SetGravity
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetGravity/GetGravity interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetGravity, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    int32_t gravity = 1;
    layer->SetGravity(gravity);
    EXPECT_EQ(layer->GetGravity(), gravity);
}

/**
 * Function: Inherited_SetUniRenderFlag
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetUniRenderFlag/GetUniRenderFlag interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetUniRenderFlag, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetUniRenderFlag(true);
    EXPECT_EQ(layer->GetUniRenderFlag(), true);
}

/**
 * Function: Inherited_SetTunnelHandleChange
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTunnelHandleChange/GetTunnelHandleChange interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetTunnelHandleChange, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetTunnelHandleChange(true);
    EXPECT_EQ(layer->GetTunnelHandleChange(), true);
}

/**
 * Function: Inherited_SetTunnelLayerId
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTunnelLayerId/GetTunnelLayerId interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetTunnelLayerId, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    uint64_t tunnelLayerId = 12345u;
    layer->SetTunnelLayerId(tunnelLayerId);
    EXPECT_EQ(layer->GetTunnelLayerId(), tunnelLayerId);
}

/**
 * Function: Inherited_SetTunnelLayerProperty
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTunnelLayerProperty/GetTunnelLayerProperty interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetTunnelLayerProperty, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    uint32_t property = 0x01;
    layer->SetTunnelLayerProperty(property);
    EXPECT_EQ(layer->GetTunnelLayerProperty(), property);
}

/**
 * Function: Inherited_SetTunnelLayerGeneration
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTunnelLayerGeneration/GetTunnelLayerGeneration interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetTunnelLayerGeneration, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    uint64_t generation = 999u;
    layer->SetTunnelLayerGeneration(generation);
    EXPECT_EQ(layer->GetTunnelLayerGeneration(), generation);
}

/**
 * Function: Inherited_SetSdrNit
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSdrNit/GetSdrNit interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetSdrNit, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    float sdrNit = 500.0f;
    layer->SetSdrNit(sdrNit);
    EXPECT_EQ(layer->GetSdrNit(), sdrNit);
}

/**
 * Function: Inherited_SetDisplayNit
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetDisplayNit/GetDisplayNit interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetDisplayNit, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    float displayNit = 1000.0f;
    layer->SetDisplayNit(displayNit);
    EXPECT_EQ(layer->GetDisplayNit(), displayNit);
}

/**
 * Function: Inherited_SetBrightnessRatio
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetBrightnessRatio/GetBrightnessRatio interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetBrightnessRatio, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    float ratio = 1.5f;
    layer->SetBrightnessRatio(ratio);
    EXPECT_EQ(layer->GetBrightnessRatio(), ratio);
}

/**
 * Function: Inherited_SetLayerLinearMatrix
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerLinearMatrix/GetLayerLinearMatrix interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetLayerLinearMatrix, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    std::vector<float> matrix = {0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f};
    layer->SetLayerLinearMatrix(matrix);
    EXPECT_EQ(layer->GetLayerLinearMatrix().size(), matrix.size());
}

/**
 * Function: Inherited_SetLayerSourceTuning
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerSourceTuning/GetLayerSourceTuning interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetLayerSourceTuning, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    int32_t source = 1;
    layer->SetLayerSourceTuning(source);
    EXPECT_EQ(layer->GetLayerSourceTuning(), source);
}

/**
 * Function: Inherited_SetRotationFixed
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetRotationFixed/GetRotationFixed interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetRotationFixed, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetRotationFixed(true);
    EXPECT_EQ(layer->GetRotationFixed(), true);
}

/**
 * Function: Inherited_SetLayerArsr
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerArsr/GetLayerArsr interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetLayerArsr, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetLayerArsr(false);
    EXPECT_EQ(layer->GetLayerArsr(), false);
}

/**
 * Function: Inherited_SetLayerCopybit
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerCopybit/GetLayerCopybit interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetLayerCopybit, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetLayerCopybit(true);
    EXPECT_EQ(layer->GetLayerCopybit(), true);
}

/**
 * Function: Inherited_SetNeedBilinearInterpolation
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetNeedBilinearInterpolation/GetNeedBilinearInterpolation interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetNeedBilinearInterpolation, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetNeedBilinearInterpolation(true);
    EXPECT_EQ(layer->GetNeedBilinearInterpolation(), true);
}

/**
 * Function: Inherited_SetIsMaskLayer
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetIsMaskLayer/GetIsMaskLayer interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetIsMaskLayer, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetIsMaskLayer(true);
    EXPECT_EQ(layer->GetIsMaskLayer(), true);
}

/**
 * Function: Inherited_SetNodeId
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetNodeId/GetNodeId interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetNodeId, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    uint64_t nodeId = 123456u;
    layer->SetNodeId(nodeId);
    EXPECT_EQ(layer->GetNodeId(), nodeId);
}

/**
 * Function: Inherited_SetAncoFlags
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetAncoFlags/GetAncoFlags interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetAncoFlags, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    uint32_t flags = 0x02;
    layer->SetAncoFlags(flags);
    EXPECT_EQ(layer->GetAncoFlags(), flags);
}

/**
 * Function: Inherited_SetDelegateMode
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetDelegateMode/GetDelegateMode interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetDelegateMode, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetDelegateMode(true);
    EXPECT_EQ(layer->GetDelegateMode(), true);
}

/**
 * Function: Inherited_SetSurfaceUniqueId
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSurfaceUniqueId/GetSurfaceUniqueId interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetSurfaceUniqueId, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    uint64_t uniqueId = 98765u;
    layer->SetSurfaceUniqueId(uniqueId);
    EXPECT_EQ(layer->GetSurfaceUniqueId(), uniqueId);
}

/**
 * Function: Inherited_SetSurfaceName
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSurfaceName/GetSurfaceName interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetSurfaceName, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    std::string name = "test_layer_surface";
    layer->SetSurfaceName(name);
    EXPECT_EQ(layer->GetSurfaceName(), name);
}

/**
 * Function: Inherited_SetCycleBuffersNum
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetCycleBuffersNum/GetCycleBuffersNum interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetCycleBuffersNum, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    uint32_t num = 3;
    layer->SetCycleBuffersNum(num);
    EXPECT_EQ(layer->GetCycleBuffersNum(), num);
}

/**
 * Function: Inherited_SetIsNeedComposition
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetIsNeedComposition/GetIsNeedComposition interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetIsNeedComposition, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetIsNeedComposition(true);
    EXPECT_EQ(layer->GetIsNeedComposition(), true);
}

/**
 * Function: Inherited_SetUseDeviceOffline
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetUseDeviceOffline/GetUseDeviceOffline interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetUseDeviceOffline, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetUseDeviceOffline(true);
    EXPECT_EQ(layer->GetUseDeviceOffline(), true);
}

/**
 * Function: Inherited_SetIgnoreAlpha
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetIgnoreAlpha/GetIgnoreAlpha interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetIgnoreAlpha, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetIgnoreAlpha(true);
    EXPECT_EQ(layer->GetIgnoreAlpha(), true);
}

/**
 * Function: Inherited_SetSplitLayerTag
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSplitLayerTag/GetSplitLayerTag interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetSplitLayerTag, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetSplitLayerTag(true);
    EXPECT_EQ(layer->GetSplitLayerTag(), true);
}

/**
 * Function: Inherited_SetRSLayerId
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetRSLayerId/GetRSLayerId interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetRSLayerId, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    RSLayerId newId = 9999u;
    layer->SetRSLayerId(newId);
    EXPECT_EQ(layer->GetRSLayerId(), newId);
}

/**
 * Function: Inherited_SetPresentTimestamp
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetPresentTimestamp/GetPresentTimestamp interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetPresentTimestamp, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicPresentTimestamp timestamp;
    timestamp.type = GRAPHIC_DISPLAY_PTS_DELAY;
    timestamp.time = 12345678;
    layer->SetPresentTimestamp(timestamp);
    EXPECT_EQ(layer->GetPresentTimestamp().type, timestamp.type);
    EXPECT_EQ(layer->GetPresentTimestamp().time, timestamp.time);
}

/**
 * Function: Inherited_SetIsSupportedPresentTimestamp
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetIsSupportedPresentTimestamp/GetIsSupportedPresentTimestamp interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetIsSupportedPresentTimestamp, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    layer->SetIsSupportedPresentTimestamp(true);
    EXPECT_EQ(layer->GetIsSupportedPresentTimestamp(), true);
}

/**
 * Function: Inherited_SetVcldInfo
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetVcldInfo/GetVcldInfo interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetVcldInfo, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    RSVcldParam vcld;
    vcld.enable = true;
    vcld.radius = 10.0f;
    layer->SetVcldInfo(vcld);
    EXPECT_EQ(layer->GetVcldInfo().enable, vcld.enable);
    EXPECT_EQ(layer->GetVcldInfo().radius, vcld.radius);
}

/**
 * Function: Inherited_SetCornerRadiusInfoForDRM
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetCornerRadiusInfoForDRM/GetCornerRadiusInfoForDRM interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetCornerRadiusInfoForDRM, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    std::vector<float> corners = {10.0f, 20.0f, 30.0f, 40.0f};
    layer->SetCornerRadiusInfoForDRM(corners);
    EXPECT_EQ(layer->GetCornerRadiusInfoForDRM().size(), corners.size());
}

/**
 * Function: Inherited_SetWindowsName
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetWindowsName/GetWindowsName interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetWindowsName, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    std::vector<std::string> names = {"window1", "window2"};
    layer->SetWindowsName(names);
    EXPECT_EQ(layer->GetWindowsName().size(), names.size());
}

/**
 * Function: Inherited_SetLayerMaskInfo
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerMaskInfo/GetLayerMaskInfo interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetLayerMaskInfo, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    LayerMask mask = LayerMask::LAYER_MASK_NORMAL;
    layer->SetLayerMaskInfo(mask);
    EXPECT_EQ(layer->GetLayerMaskInfo(), mask);
}

/**
 * Function: Inherited_IsAncoNative
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited IsAncoNative interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_IsAncoNative, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    EXPECT_EQ(layer->IsAncoNative(), false);
}

/**
 * Function: Inherited_SetAncoSrcRect
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetAncoSrcRect/GetAncoSrcRect interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetAncoSrcRect, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicIRect rect = {0, 0, 100, 100};
    layer->SetAncoSrcRect(rect);
    EXPECT_EQ(layer->GetAncoSrcRect().x, rect.x);
    EXPECT_EQ(layer->GetAncoSrcRect().w, rect.w);
}

/**
 * Function: Inherited_SetSolidColorLayerProperty
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSolidColorLayerProperty/GetSolidColorLayerProperty interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetSolidColorLayerProperty, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicSolidColorLayerProperty prop;
    prop.zOrder = 10;
    prop.layerRect = {0, 0, 100, 200};
    layer->SetSolidColorLayerProperty(prop);
    EXPECT_EQ(layer->GetSolidColorLayerProperty().zOrder, prop.zOrder);
    EXPECT_EQ(layer->GetSolidColorLayerProperty().layerRect.w, prop.layerRect.w);
}

/**
 * Function: Inherited_SetDelegateModeCropRect
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetDelegateModeCropRect/GetDelegateModeCropRect interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetDelegateModeCropRect, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicIRect rect = {5, 10, 200, 300};
    layer->SetDelegateModeCropRect(rect);
    EXPECT_EQ(layer->GetDelegateModeCropRect().x, rect.x);
    EXPECT_EQ(layer->GetDelegateModeCropRect().w, rect.w);
}

/**
 * Function: Inherited_SetMetaData
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetMetaData/GetMetaData interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetMetaData, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    std::vector<GraphicHDRMetaData> metaData;
    GraphicHDRMetaData item;
    item.key = GRAPHIC_MATAKEY_RED_PRIMARY_Y;
    item.value = 100;
    metaData.push_back(item);
    layer->SetMetaData(metaData);
    EXPECT_EQ(layer->GetMetaData()[0].key, GRAPHIC_MATAKEY_RED_PRIMARY_Y);
}

/**
 * Function: Inherited_SetMetaDataSet
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetMetaDataSet/GetMetaDataSet interface
 */
HWTEST_F(RSSurfaceSolidFilledColorLayerTest, Inherited_SetMetaDataSet, TestSize.Level1)
{
    auto context = std::make_shared<RSComposerContext>(nullptr);
    ASSERT_NE(context, nullptr);
    auto layer = RSSurfaceSolidFilledColorLayer::Create(TEST_LAYER_ID, context);
    ASSERT_NE(layer, nullptr);

    GraphicHDRMetaDataSet metaDataSet;
    metaDataSet.key = GRAPHIC_MATAKEY_BLUE_PRIMARY_X;
    layer->SetMetaDataSet(metaDataSet);
    EXPECT_EQ(layer->GetMetaDataSet().key, GRAPHIC_MATAKEY_BLUE_PRIMARY_X);
}
} // namespace Rosen
} // namespace OHOS