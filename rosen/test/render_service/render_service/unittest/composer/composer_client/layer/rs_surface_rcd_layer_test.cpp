/**
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
#include <memory>
#include "rs_surface_rcd_layer.h"
#include "rs_composer_context.h"
#include "rs_render_to_composer_connection.h"
#include "surface_type.h"
#include "iconsumer_surface.h"
#include "consumer_surface.h"
#include "surface_tunnel_handle.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLayerContextProbe : public RSComposerContext {
public:
    explicit RSLayerContextProbe(const sptr<IRSRenderToComposerConnection>& conn) : RSComposerContext(conn) {}
    using RSComposerContext::GetRSLayerTransaction;
    using RSComposerContext::AddRSLayer;
    using RSComposerContext::RemoveRSLayer;
    using RSComposerContext::GetRSLayer;
};

class RSSurfaceRCDLayerTest : public Test {};

/**
 * Function: RCDLayer_Create_NullContext
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. pass null context to Create
 *                  2. expect nullptr returned
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Create_NullContext, TestSize.Level1)
{
    std::shared_ptr<RSComposerContext> ctx;
    auto layer = RSSurfaceRCDLayer::Create(1u, ctx);
    EXPECT_EQ(layer, nullptr);
}

/**
 * Function: RCDLayer_Create_NewAndExisting
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. create new layer and add to context
 *                  2. create again to reuse existing layer
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Create_NewAndExisting, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer1 = RSSurfaceRCDLayer::Create(200u, ctx);
    ASSERT_NE(layer1, nullptr);
    EXPECT_EQ(layer1->GetRSLayerId(), 200u);

    auto layer2 = RSSurfaceRCDLayer::Create(200u, ctx);
    ASSERT_NE(layer2, nullptr);
    EXPECT_EQ(layer2.get(), layer1.get());
    EXPECT_EQ(layer2->GetRSLayerId(), 200u);
}

/**
 * Function: RCDLayer_SetPixelMap_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set non-null pixel map
 *                  2. expect transaction handler becomes non-empty
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_SetPixelMap_AddsParcel, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(201u, ctx);
    ASSERT_NE(layer, nullptr);

    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_FALSE(handler->IsEmpty());

    // Construct a minimal PixelMap instance
    std::shared_ptr<Media::PixelMap> pm(new Media::PixelMap());
    static_cast<RSSurfaceRCDLayer*>(layer.get())->SetPixelMap(pm);
    EXPECT_FALSE(handler->IsEmpty());
    EXPECT_EQ(static_cast<RSSurfaceRCDLayer*>(layer.get())->GetPixelMap().get(), pm.get());
}

/**
 * Function: RCDLayer_IsScreenFlag
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. verify IsScreenRCDLayer returns true
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_IsScreenFlag, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(202u, ctx);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsScreenRCDLayer());
}
/**
 * Function: RCDLayer_SetPixelMap_Null_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set null pixel map
 *                  2. expect transaction handler becomes non-empty
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_SetPixelMap_Null_AddsParcel, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(203u, ctx);
    ASSERT_NE(layer, nullptr);

    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_FALSE(handler->IsEmpty());

    std::shared_ptr<Media::PixelMap> pm;
    static_cast<RSSurfaceRCDLayer*>(layer.get())->SetPixelMap(pm);
    EXPECT_FALSE(handler->IsEmpty());
    EXPECT_EQ(static_cast<RSSurfaceRCDLayer*>(layer.get())->GetPixelMap(), pm);
}

/**
 * Function: RCDLayer_Inherited_SetAlpha
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetAlpha/GetAlpha interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetAlpha, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(204u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicLayerAlpha alpha;
    alpha.enPixelAlpha = true;
    layer->SetAlpha(alpha);
    EXPECT_EQ(layer->GetAlpha().enPixelAlpha, true);
}

/**
 * Function: RCDLayer_Inherited_SetZorder
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetZorder/GetZorder interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetZorder, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(205u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetZorder(50);
    EXPECT_EQ(layer->GetZorder(), 50u);
}

/**
 * Function: RCDLayer_Inherited_SetType
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetType/GetType interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetType, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(206u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicLayerType type = GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC;
    layer->SetType(type);
    EXPECT_EQ(layer->GetType(), type);
}

/**
 * Function: RCDLayer_Inherited_SetTransform
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTransform/GetTransform interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetTransform, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(207u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_90;
    layer->SetTransform(transform);
    EXPECT_EQ(layer->GetTransform(), transform);
}

/**
 * Function: RCDLayer_Inherited_SetCompositionType
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetCompositionType/GetCompositionType interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetCompositionType, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(208u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicCompositionType type = GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT;
    layer->SetCompositionType(type);
    EXPECT_EQ(layer->GetCompositionType(), type);
}

/**
 * Function: RCDLayer_Inherited_SetBlendType
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetBlendType/GetBlendType interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetBlendType, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(209u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicBlendType type = GraphicBlendType::GRAPHIC_BLEND_CLEAR;
    layer->SetBlendType(type);
    EXPECT_EQ(layer->GetBlendType(), type);
}

/**
 * Function: RCDLayer_Inherited_SetCropRect
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetCropRect/GetCropRect interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetCropRect, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(210u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicIRect crop = {1, 2, 100, 200};
    layer->SetCropRect(crop);
    EXPECT_EQ(layer->GetCropRect().x, crop.x);
    EXPECT_EQ(layer->GetCropRect().w, crop.w);
}

/**
 * Function: RCDLayer_Inherited_SetLayerSize
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerSize/GetLayerSize interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetLayerSize, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(211u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicIRect rect = {10, 20, 300, 400};
    layer->SetLayerSize(rect);
    EXPECT_EQ(layer->GetLayerSize().x, rect.x);
    EXPECT_EQ(layer->GetLayerSize().h, rect.h);
}

/**
 * Function: RCDLayer_Inherited_SetBoundSize
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetBoundSize/GetBoundSize interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetBoundSize, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(212u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicIRect rect = {5, 10, 150, 250};
    layer->SetBoundSize(rect);
    EXPECT_EQ(layer->GetBoundSize().y, rect.y);
    EXPECT_EQ(layer->GetBoundSize().w, rect.w);
}

/**
 * Function: RCDLayer_Inherited_SetVisibleRegions
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetVisibleRegions/GetVisibleRegions interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetVisibleRegions, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(213u, ctx);
    ASSERT_NE(layer, nullptr);

    std::vector<GraphicIRect> regions = {{0, 0, 100, 100}, {10, 10, 50, 50}};
    layer->SetVisibleRegions(regions);
    EXPECT_EQ(layer->GetVisibleRegions().size(), regions.size());
}

/**
 * Function: RCDLayer_Inherited_SetDirtyRegions
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetDirtyRegions/GetDirtyRegions interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetDirtyRegions, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(214u, ctx);
    ASSERT_NE(layer, nullptr);

    std::vector<GraphicIRect> regions = {{0, 0, 50, 50}};
    layer->SetDirtyRegions(regions);
    EXPECT_EQ(layer->GetDirtyRegions().size(), regions.size());
}

/**
 * Function: RCDLayer_Inherited_SetPreMulti
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetPreMulti/IsPreMulti interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetPreMulti, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(215u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetPreMulti(true);
    EXPECT_EQ(layer->IsPreMulti(), true);
}

/**
 * Function: RCDLayer_Inherited_SetLayerColor
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerColor/GetLayerColor interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetLayerColor, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(216u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicLayerColor color = {100, 150, 200, 255};
    layer->SetLayerColor(color);
    EXPECT_EQ(layer->GetLayerColor().r, color.r);
    EXPECT_EQ(layer->GetLayerColor().g, color.g);
}

/**
 * Function: RCDLayer_Inherited_SetBackgroundColor
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetBackgroundColor/GetBackgroundColor interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetBackgroundColor, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(217u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicLayerColor color = {50, 75, 100, 128};
    layer->SetBackgroundColor(color);
    EXPECT_EQ(layer->GetBackgroundColor().r, color.r);
}

/**
 * Function: RCDLayer_Inherited_SetColorDataSpace
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetColorDataSpace/GetColorDataSpace interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetColorDataSpace, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(218u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicColorDataSpace colorSpace = GraphicColorDataSpace::GRAPHIC_GAMUT_BT709;
    layer->SetColorDataSpace(colorSpace);
    EXPECT_EQ(layer->GetColorDataSpace(), colorSpace);
}

/**
 * Function: RCDLayer_Inherited_SetColorTransform
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetColorTransform/GetColorTransform interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetColorTransform, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(219u, ctx);
    ASSERT_NE(layer, nullptr);

    std::vector<float> matrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    layer->SetColorTransform(matrix);
    EXPECT_EQ(layer->GetColorTransform().size(), matrix.size());
}

/**
 * Function: RCDLayer_Inherited_SetMatrix
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetMatrix/GetMatrix interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetMatrix, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(220u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicMatrix matrix;
    matrix.scaleX = 2.0f;
    matrix.scaleY = 2.0f;
    layer->SetMatrix(matrix);
    EXPECT_EQ(layer->GetMatrix().scaleX, matrix.scaleX);
}

/**
 * Function: RCDLayer_Inherited_SetGravity
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetGravity/GetGravity interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetGravity, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(221u, ctx);
    ASSERT_NE(layer, nullptr);

    int32_t gravity = 1;
    layer->SetGravity(gravity);
    EXPECT_EQ(layer->GetGravity(), gravity);
}

/**
 * Function: RCDLayer_Inherited_SetUniRenderFlag
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetUniRenderFlag/GetUniRenderFlag interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetUniRenderFlag, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(222u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetUniRenderFlag(true);
    EXPECT_EQ(layer->GetUniRenderFlag(), true);
}

/**
 * Function: RCDLayer_Inherited_SetTunnelHandleChange
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTunnelHandleChange/GetTunnelHandleChange interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetTunnelHandleChange, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(223u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetTunnelHandleChange(true);
    EXPECT_EQ(layer->GetTunnelHandleChange(), true);
}

/**
 * Function: RCDLayer_Inherited_SetTunnelLayerId
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTunnelLayerId/GetTunnelLayerId interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetTunnelLayerId, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(224u, ctx);
    ASSERT_NE(layer, nullptr);

    uint64_t tunnelLayerId = 12345u;
    layer->SetTunnelLayerId(tunnelLayerId);
    EXPECT_EQ(layer->GetTunnelLayerId(), tunnelLayerId);
}

/**
 * Function: RCDLayer_Inherited_SetTunnelLayerProperty
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTunnelLayerProperty/GetTunnelLayerProperty interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetTunnelLayerProperty, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(225u, ctx);
    ASSERT_NE(layer, nullptr);

    uint32_t property = 0x01;
    layer->SetTunnelLayerProperty(property);
    EXPECT_EQ(layer->GetTunnelLayerProperty(), property);
}

/**
 * Function: RCDLayer_Inherited_SetTunnelLayerGeneration
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTunnelLayerGeneration/GetTunnelLayerGeneration interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetTunnelLayerGeneration, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(226u, ctx);
    ASSERT_NE(layer, nullptr);

    uint64_t generation = 999u;
    layer->SetTunnelLayerGeneration(generation);
    EXPECT_EQ(layer->GetTunnelLayerGeneration(), generation);
}

/**
 * Function: RCDLayer_Inherited_SetSdrNit
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSdrNit/GetSdrNit interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetSdrNit, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(227u, ctx);
    ASSERT_NE(layer, nullptr);

    float sdrNit = 500.0f;
    layer->SetSdrNit(sdrNit);
    EXPECT_EQ(layer->GetSdrNit(), sdrNit);
}

/**
 * Function: RCDLayer_Inherited_SetDisplayNit
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetDisplayNit/GetDisplayNit interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetDisplayNit, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(228u, ctx);
    ASSERT_NE(layer, nullptr);

    float displayNit = 1000.0f;
    layer->SetDisplayNit(displayNit);
    EXPECT_EQ(layer->GetDisplayNit(), displayNit);
}

/**
 * Function: RCDLayer_Inherited_SetBrightnessRatio
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetBrightnessRatio/GetBrightnessRatio interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetBrightnessRatio, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(229u, ctx);
    ASSERT_NE(layer, nullptr);

    float ratio = 1.5f;
    layer->SetBrightnessRatio(ratio);
    EXPECT_EQ(layer->GetBrightnessRatio(), ratio);
}

/**
 * Function: RCDLayer_Inherited_SetLayerLinearMatrix
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerLinearMatrix/GetLayerLinearMatrix interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetLayerLinearMatrix, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(230u, ctx);
    ASSERT_NE(layer, nullptr);

    std::vector<float> matrix = {0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f};
    layer->SetLayerLinearMatrix(matrix);
    EXPECT_EQ(layer->GetLayerLinearMatrix().size(), matrix.size());
}

/**
 * Function: RCDLayer_Inherited_SetLayerSourceTuning
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerSourceTuning/GetLayerSourceTuning interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetLayerSourceTuning, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(231u, ctx);
    ASSERT_NE(layer, nullptr);

    int32_t source = 1;
    layer->SetLayerSourceTuning(source);
    EXPECT_EQ(layer->GetLayerSourceTuning(), source);
}

/**
 * Function: RCDLayer_Inherited_SetRotationFixed
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetRotationFixed/GetRotationFixed interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetRotationFixed, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(232u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetRotationFixed(true);
    EXPECT_EQ(layer->GetRotationFixed(), true);
}

/**
 * Function: RCDLayer_Inherited_SetLayerArsr
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerArsr/GetLayerArsr interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetLayerArsr, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(233u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetLayerArsr(false);
    EXPECT_EQ(layer->GetLayerArsr(), false);
}

/**
 * Function: RCDLayer_Inherited_SetLayerCopybit
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerCopybit/GetLayerCopybit interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetLayerCopybit, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(234u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetLayerCopybit(true);
    EXPECT_EQ(layer->GetLayerCopybit(), true);
}

/**
 * Function: RCDLayer_Inherited_SetNeedBilinearInterpolation
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetNeedBilinearInterpolation/GetNeedBilinearInterpolation interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetNeedBilinearInterpolation, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(235u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetNeedBilinearInterpolation(true);
    EXPECT_EQ(layer->GetNeedBilinearInterpolation(), true);
}

/**
 * Function: RCDLayer_Inherited_SetIsMaskLayer
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetIsMaskLayer/GetIsMaskLayer interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetIsMaskLayer, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(236u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetIsMaskLayer(true);
    EXPECT_EQ(layer->GetIsMaskLayer(), true);
}

/**
 * Function: RCDLayer_Inherited_SetNodeId
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetNodeId/GetNodeId interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetNodeId, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(237u, ctx);
    ASSERT_NE(layer, nullptr);

    uint64_t nodeId = 123456u;
    layer->SetNodeId(nodeId);
    EXPECT_EQ(layer->GetNodeId(), nodeId);
}

/**
 * Function: RCDLayer_Inherited_SetAncoFlags
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetAncoFlags/GetAncoFlags interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetAncoFlags, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(238u, ctx);
    ASSERT_NE(layer, nullptr);

    uint32_t flags = 0x02;
    layer->SetAncoFlags(flags);
    EXPECT_EQ(layer->GetAncoFlags(), flags);
}

/**
 * Function: RCDLayer_Inherited_SetDelegateMode
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetDelegateMode/GetDelegateMode interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetDelegateMode, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(239u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetDelegateMode(true);
    EXPECT_EQ(layer->GetDelegateMode(), true);
}

/**
 * Function: RCDLayer_Inherited_SetSurface
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSurface/GetSurface interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetSurface, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(240u, ctx);
    ASSERT_NE(layer, nullptr);

    sptr<IConsumerSurface> surface = IConsumerSurface::Create("test_surface");
    layer->SetSurface(surface);
    EXPECT_EQ(layer->GetSurface(), surface);
}

/**
 * Function: RCDLayer_Inherited_SetSurfaceUniqueId
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSurfaceUniqueId/GetSurfaceUniqueId interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetSurfaceUniqueId, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(241u, ctx);
    ASSERT_NE(layer, nullptr);

    uint64_t uniqueId = 98765u;
    layer->SetSurfaceUniqueId(uniqueId);
    EXPECT_EQ(layer->GetSurfaceUniqueId(), uniqueId);
}

/**
 * Function: RCDLayer_Inherited_SetSurfaceName
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSurfaceName/GetSurfaceName interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetSurfaceName, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(242u, ctx);
    ASSERT_NE(layer, nullptr);

    std::string name = "test_layer_surface";
    layer->SetSurfaceName(name);
    EXPECT_EQ(layer->GetSurfaceName(), name);
}

/**
 * Function: RCDLayer_Inherited_SetCycleBuffersNum
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetCycleBuffersNum/GetCycleBuffersNum interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetCycleBuffersNum, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(243u, ctx);
    ASSERT_NE(layer, nullptr);

    uint32_t num = 3;
    layer->SetCycleBuffersNum(num);
    EXPECT_EQ(layer->GetCycleBuffersNum(), num);
}

/**
 * Function: RCDLayer_Inherited_SetIsNeedComposition
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetIsNeedComposition/GetIsNeedComposition interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetIsNeedComposition, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(244u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetIsNeedComposition(true);
    EXPECT_EQ(layer->GetIsNeedComposition(), true);
}

/**
 * Function: RCDLayer_Inherited_SetUseDeviceOffline
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetUseDeviceOffline/GetUseDeviceOffline interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetUseDeviceOffline, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(245u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetUseDeviceOffline(true);
    EXPECT_EQ(layer->GetUseDeviceOffline(), true);
}

/**
 * Function: RCDLayer_Inherited_SetIgnoreAlpha
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetIgnoreAlpha/GetIgnoreAlpha interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetIgnoreAlpha, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(246u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetIgnoreAlpha(true);
    EXPECT_EQ(layer->GetIgnoreAlpha(), true);
}

/**
 * Function: RCDLayer_Inherited_SetSplitLayerTag
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSplitLayerTag/GetSplitLayerTag interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetSplitLayerTag, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(247u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetSplitLayerTag(true);
    EXPECT_EQ(layer->GetSplitLayerTag(), true);
}

/**
 * Function: RCDLayer_Inherited_SetRSLayerId
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetRSLayerId/GetRSLayerId interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetRSLayerId, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(248u, ctx);
    ASSERT_NE(layer, nullptr);

    RSLayerId newId = 9999u;
    layer->SetRSLayerId(newId);
    EXPECT_EQ(layer->GetRSLayerId(), newId);
}

/**
 * Function: RCDLayer_Inherited_SetPresentTimestamp
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetPresentTimestamp/GetPresentTimestamp interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetPresentTimestamp, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(249u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicPresentTimestamp timestamp;
    timestamp.type = GRAPHIC_DISPLAY_PTS_DELAY;
    timestamp.time = 12345678;
    layer->SetPresentTimestamp(timestamp);
    EXPECT_EQ(layer->GetPresentTimestamp().type, timestamp.type);
    EXPECT_EQ(layer->GetPresentTimestamp().time, timestamp.time);
}

/**
 * Function: RCDLayer_Inherited_SetIsSupportedPresentTimestamp
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetIsSupportedPresentTimestamp/GetIsSupportedPresentTimestamp interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetIsSupportedPresentTimestamp, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(250u, ctx);
    ASSERT_NE(layer, nullptr);

    layer->SetIsSupportedPresentTimestamp(true);
    EXPECT_EQ(layer->GetIsSupportedPresentTimestamp(), true);
}

/**
 * Function: RCDLayer_Inherited_SetVcldInfo
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetVcldInfo/GetVcldInfo interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetVcldInfo, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(251u, ctx);
    ASSERT_NE(layer, nullptr);

    RSVcldParam vcld;
    vcld.enable = true;
    vcld.radius = 10.0f;
    layer->SetVcldInfo(vcld);
    EXPECT_EQ(layer->GetVcldInfo().enable, vcld.enable);
    EXPECT_EQ(layer->GetVcldInfo().radius, vcld.radius);
}

/**
 * Function: RCDLayer_Inherited_SetCornerRadiusInfoForDRM
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetCornerRadiusInfoForDRM/GetCornerRadiusInfoForDRM interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetCornerRadiusInfoForDRM, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(252u, ctx);
    ASSERT_NE(layer, nullptr);

    std::vector<float> corners = {10.0f, 20.0f, 30.0f, 40.0f};
    layer->SetCornerRadiusInfoForDRM(corners);
    EXPECT_EQ(layer->GetCornerRadiusInfoForDRM().size(), corners.size());
}

/**
 * Function: RCDLayer_Inherited_SetWindowsName
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetWindowsName/GetWindowsName interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetWindowsName, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(253u, ctx);
    ASSERT_NE(layer, nullptr);

    std::vector<std::string> names = {"window1", "window2"};
    layer->SetWindowsName(names);
    EXPECT_EQ(layer->GetWindowsName().size(), names.size());
}

/**
 * Function: RCDLayer_Inherited_SetLayerMaskInfo
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetLayerMaskInfo/GetLayerMaskInfo interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetLayerMaskInfo, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(254u, ctx);
    ASSERT_NE(layer, nullptr);

    LayerMask mask = LayerMask::LAYER_MASK_NORMAL;
    layer->SetLayerMaskInfo(mask);
    EXPECT_EQ(layer->GetLayerMaskInfo(), mask);
}

/**
 * Function: RCDLayer_Inherited_IsAncoNative
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited IsAncoNative interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_IsAncoNative, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(255u, ctx);
    ASSERT_NE(layer, nullptr);

    EXPECT_EQ(layer->IsAncoNative(), false);
}

/**
 * Function: RCDLayer_Inherited_SetTunnelHandle
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetTunnelHandle/GetTunnelHandle interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetTunnelHandle, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(256u, ctx);
    ASSERT_NE(layer, nullptr);

    sptr<SurfaceTunnelHandle> handle = sptr<SurfaceTunnelHandle>::MakeSptr();
    layer->SetTunnelHandle(handle);
    EXPECT_EQ(layer->GetTunnelHandle(), handle);
}

/**
 * Function: RCDLayer_Inherited_SetAncoSrcRect
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetAncoSrcRect/GetAncoSrcRect interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetAncoSrcRect, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(257u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicIRect rect = {0, 0, 100, 100};
    layer->SetAncoSrcRect(rect);
    EXPECT_EQ(layer->GetAncoSrcRect().x, rect.x);
    EXPECT_EQ(layer->GetAncoSrcRect().w, rect.w);
}

/**
 * Function: RCDLayer_Inherited_SetSolidColorLayerProperty
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetSolidColorLayerProperty/GetSolidColorLayerProperty interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetSolidColorLayerProperty, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(258u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicSolidColorLayerProperty prop;
    prop.zOrder = 10;
    prop.layerRect = {0, 0, 100, 200};
    layer->SetSolidColorLayerProperty(prop);
    EXPECT_EQ(layer->GetSolidColorLayerProperty().zOrder, prop.zOrder);
    EXPECT_EQ(layer->GetSolidColorLayerProperty().layerRect.w, prop.layerRect.w);
}

/**
 * Function: RCDLayer_Inherited_SetDelegateModeCropRect
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. test inherited SetDelegateModeCropRect/GetDelegateModeCropRect interface
 */
HWTEST_F(RSSurfaceRCDLayerTest, RCDLayer_Inherited_SetDelegateModeCropRect, TestSize.Level1)
{
    sptr<IRSRenderToComposerConnection> connection = sptr<RSRenderToComposerConnection>::MakeSptr("test", 0, nullptr);
    auto ctx = std::make_shared<RSLayerContextProbe>(connection);
    auto layer = RSSurfaceRCDLayer::Create(259u, ctx);
    ASSERT_NE(layer, nullptr);

    GraphicIRect rect = {5, 10, 200, 300};
    layer->SetDelegateModeCropRect(rect);
    EXPECT_EQ(layer->GetDelegateModeCropRect().x, rect.x);
    EXPECT_EQ(layer->GetDelegateModeCropRect().w, rect.w);
}
} // namespace OHOS::Rosen
