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
#include "rs_surface_layer.h"
#include "rs_composer_context.h"
#include "surface_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLayerContextProbe : public RSComposerContext {
public:
    using RSComposerContext::GetRSLayerTransaction;
    using RSComposerContext::AddRSLayer;
    using RSComposerContext::RemoveRSLayer;
    using RSComposerContext::GetRSLayer;
};

class RSSurfaceLayerTest : public Test {};

/**
 * Function: SurfaceLayer_Create_NullContext
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. pass null context to Create
 *                  2. expect nullptr returned
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_Create_NullContext, TestSize.Level1)
{
    std::shared_ptr<RSComposerContext> ctx;
    auto layer = RSSurfaceLayer::Create(ctx, 1u);
    EXPECT_EQ(layer, nullptr);
}

/**
 * Function: SurfaceLayer_Regions_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set visible and dirty regions
 *                  2. expect transaction handler becomes non-empty
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_Regions_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 106u);
    ASSERT_NE(layer, nullptr);

    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    std::vector<GraphicIRect> vrs {{0,0,10,10}};
    std::vector<GraphicIRect> drs {{1,1,8,8}};
    layer->SetVisibleRegions(vrs);
    layer->SetDirtyRegions(drs);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_BlendAndCrop_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set blend type and crop rect
 *                  2. expect transaction handler becomes non-empty
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_BlendAndCrop_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 107u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    layer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    GraphicIRect crop {2,2,6,6};
    layer->SetCropRect(crop);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_SizeAndColor_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set layer/bound size
 *                  2. set layer/background colors
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_SizeAndColor_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 108u);
    ASSERT_NE(layer, nullptr);

    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    GraphicIRect lr {0,0,100,100};
    GraphicIRect br {0,0,120,120};
    layer->SetLayerSize(lr);
    layer->SetBoundSize(br);
    GraphicLayerColor lc {255,0,0,255};
    GraphicLayerColor bc {0,0,255,255};
    layer->SetLayerColor(lc);
    layer->SetBackgroundColor(bc);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_ColorTransformAndSpace_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set color transform matrix and data space
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_ColorTransformAndSpace_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 109u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    std::vector<float> mat {1,0,0, 0,1,0, 0,0,1};
    layer->SetColorTransform(mat);
    layer->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_COLOR_DATA_SPACE_SRGB);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_MetaData_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set meta data and meta data set
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_MetaData_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 110u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    std::vector<GraphicHDRMetaData> md {{0,0}};
    GraphicHDRMetaDataSet mds {0,0,0,0};
    layer->SetMetaData(md);
    layer->SetMetaDataSet(mds);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_FlagsAndIds_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set various flags and ids
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_FlagsAndIds_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 111u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    layer->SetPreMulti(true);
    layer->SetGravity(1);
    layer->SetUniRenderFlag(true);
    layer->SetTunnelHandleChange(true);
    layer->SetTunnelLayerId(999u);
    layer->SetTunnelLayerProperty(123u);
    layer->SetSdrNit(600.0f);
    layer->SetDisplayNit(650.0f);
    layer->SetBrightnessRatio(0.8f);
    std::vector<float> lin {1,0,0, 0,1,0, 0,0,1};
    layer->SetLayerLinearMatrix(lin);
    layer->SetLayerSourceTuning(2);
    std::vector<std::string> names {"winA","winB"};
    layer->SetWindowsName(names);
    layer->SetRotationFixed(true);
    layer->SetLayerArsr(false);
    layer->SetLayerCopybit(true);
    layer->SetNeedBilinearInterpolation(true);
    layer->SetIsMaskLayer(true);
    layer->SetNodeId(777u);
    layer->SetAncoFlags(static_cast<uint32_t>(AncoFlags::FORCE_REFRESH));
    layer->SetSurfaceUniqueId(888u);
    sptr<SurfaceBuffer> pre = SurfaceBuffer::Create();
    layer->SetPreBuffer(pre);
    layer->SetCycleBuffersNum(3u);
    layer->SetSurfaceName(std::string("sfc-name"));
    layer->SetUseDeviceOffline(true);
    layer->SetIgnoreAlpha(true);
    GraphicIRect anco {0,0,5,5};
    layer->SetAncoSrcRect(anco);
    EXPECT_FALSE(handler->IsEmpty());
}
/**
 * Function: SurfaceLayer_Create_NewAndExisting
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. create new layer and add to context
 *                  2. create again to reuse existing layer
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_Create_NewAndExisting, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer1 = RSSurfaceLayer::Create(ctx, 100u);
    ASSERT_NE(layer1, nullptr);
    EXPECT_EQ(layer1->GetRSLayerId(), 100u);

    auto layer2 = RSSurfaceLayer::Create(ctx, 100u);
    ASSERT_NE(layer2, nullptr);
    EXPECT_EQ(layer2.get(), layer1.get());
    EXPECT_EQ(layer2->GetRSLayerId(), 100u);
}

/**
 * Function: SurfaceLayer_Setters_NoChange_NoParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. call setters with same values
 *                  2. expect transaction handler stays empty
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_Setters_NoChange_NoParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 101u);
    ASSERT_NE(layer, nullptr);

    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    GraphicLayerAlpha alpha {0};
    layer->SetAlpha(alpha);
    // setting same again should not add parcel
    layer->SetAlpha(alpha);

    layer->SetZorder(0);
    layer->SetZorder(0);

    GraphicLayerType type = GRAPHIC_LAYER_TYPE_GRAPHIC;
    layer->SetType(type);
    EXPECT_EQ(layer->GetType(), type);

    GraphicTransformType type2 = GRAPHIC_ROTATE_NONE;
    layer->SetTransform(type2);
    EXPECT_EQ(layer->GetTransform(), type2);

    GraphicCompositionType type3 = GRAPHIC_COMPOSITION_CLIENT;
    layer->SetCompositionType(type3);
    EXPECT_EQ(layer->GetCompositionType(), type3);

    GraphicLayerAlpha alpha {1};
    layer->SetAlpha(alpha);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_BufferAndFence_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set buffer and acquire fence
 *                  2. expect transaction handler becomes non-empty
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_BufferAndFence_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 103u);
    ASSERT_NE(layer, nullptr);

    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    sptr<SurfaceBuffer> sb = SurfaceBuffer::Create();
    sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
    layer->SetBuffer(sb, fence);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_Timestamps_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set supported flag and present timestamp
 *                  2. expect transaction handler becomes non-empty
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_Timestamps_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 104u);
    ASSERT_NE(layer, nullptr);

    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    layer->SetIsSupportedPresentTimestamp(true);
    GraphicPresentTimestamp ts { GRAPHIC_DISPLAY_PTS_TIMESTAMP, 123456 };
    layer->SetPresentTimestamp(ts);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_Destructor_AddsDestroyParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. create layer and let it go out of scope
 *                  2. expect context removes layer and transaction has parcel
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_Destructor_AddsDestroyParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    {
        auto layer = RSSurfaceLayer::Create(ctx, 105u);
        ASSERT_NE(layer, nullptr);
        EXPECT_NE(ctx->GetRSLayer(105u), nullptr);
    }
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(ctx->GetRSLayer(105u) == nullptr);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_TypeTransformComposition_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set type/transform/composition
 *                  2. expect transaction handler becomes non-empty
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_TypeTransformComposition_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 112u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    layer->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
    layer->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    layer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_CornerRadius_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set DRM corner radius info
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_CornerRadius_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 113u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    std::vector<float> drm {4.0f, 4.0f, 4.0f, 4.0f};
    layer->SetCornerRadiusInfoForDRM(drm);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_TunnelHandle_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set tunnel handle
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_TunnelHandle_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 114u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    sptr<SurfaceTunnelHandle> handle = new SurfaceTunnelHandle();
    layer->SetTunnelHandle(handle);
    EXPECT_EQ(layer->GetTunnelHandle(), handle);

    layer->SetTunnelLayerId(100);
    EXPECT_EQ(layer->GetTunnelLayerId(), 100);

    layer->SetTunnelLayerProperty(1000);
    EXPECT_EQ(layer->GetTunnelLayerProperty(), 1000);

    GraphicPresentTimestamp timestamp;
    timestamp.type = GRAPHIC_DISPLAY_PTS_DELAY;
    layer->SetPresentTimestamp(timestamp);
    EXPECT_EQ(layer->GetPresentTimestamp().type, GRAPHIC_DISPLAY_PTS_DELAY);

    layer->SetIsSupportedPresentTimestamp(false);
    EXPECT_EQ(layer->GetIsSupportedPresentTimestamp(), false);

    layer->SetSdrNit(200);
    EXPECT_EQ(layer->GetSdrNit(), 200);

    layer->SetDisplayNit(300);
    EXPECT_EQ(layer->GetDisplayNit(), 300);

    layer->SetBrightnessRatio(300);
    EXPECT_EQ(layer->GetBrightnessRatio(), 300);

    std::vector<float> layerLinearMatrix;
    layerLinearMatrix.push_back(100);
    layer->SetLayerLinearMatrix(layerLinearMatrix);
    EXPECT_EQ(layer->GetLayerLinearMatrix()[0], 100);

    layer->SetLayerSourceTuning(100);
    EXPECT_EQ(layer->GetLayerSourceTuning(), 100);
}

/**
 * Function: SurfaceLayer_Zorder_Change_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. change zorder
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_Zorder_Change_AddsParcel, TestSize.Level1)
{
    std::vector<std::string> windowsName;
    windowsName.push_back("window");
    layer->SetWindowsName(windowsName);
    EXPECT_EQ(layer->GetWindowsName()[0], "window");

    layer->SetRotationFixed(false);
    EXPECT_EQ(layer->GetRotationFixed(), false);

    layer->SetLayerArsr(false);
    EXPECT_EQ(layer->GetLayerArsr(), false);

    layer->SetLayerCopybit(false);
    EXPECT_EQ(layer->GetLayerCopybit(), false);

    layer->SetNeedBilinearInterpolation(true);
    EXPECT_EQ(layer->GetNeedBilinearInterpolation(), true);

    layer->SetIsMaskLayer(false);
    EXPECT_EQ(layer->GetIsMaskLayer(), false);

    layer->SetZorder(5);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_Matrix_Change_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. change matrix
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_Matrix_Change_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 117u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    GraphicMatrix m2 {1,0,0, 0,0.5f,0, 0,0,1};
    layer->SetMatrix(m2);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_AcquireFence_Change_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. change acquire fence
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_AcquireFence_Change_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 118u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
    layer->SetAcquireFence(fence);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_SolidColor_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set solid color layer property (always delivered)
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_SolidColor_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 119u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    GraphicSolidColorLayerProperty prop { .compositionType = 1, .zOrder = 3 };
    layer->SetSolidColorLayerProperty(prop);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_IsNeedComposition_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set isNeedComposition (always delivered)
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_IsNeedComposition_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 120u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    layer->SetIsNeedComposition(true);
    EXPECT_FALSE(handler->IsEmpty());
}

/**
 * Function: SurfaceLayer_LayerMaskInfo_AddsParcel
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set layer mask info
 */
HWTEST_F(RSSurfaceLayerTest, SurfaceLayer_LayerMaskInfo_AddsParcel, TestSize.Level1)
{
    auto ctx = std::make_shared<RSLayerContextProbe>();
    auto layer = RSSurfaceLayer::Create(ctx, 121u);
    ASSERT_NE(layer, nullptr);
    auto handler = ctx->GetRSLayerTransaction();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->IsEmpty());

    layer->SetLayerMaskInfo(LayerMask::LAYER_MASK_NORMAL);
    EXPECT_FALSE(handler->IsEmpty());
}
} // namespace OHOS::Rosen