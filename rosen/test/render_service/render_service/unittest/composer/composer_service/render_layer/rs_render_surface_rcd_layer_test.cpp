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
#include "rs_render_surface_rcd_layer.h"
#include "rs_render_layer_cmd.h"

using namespace OHOS::Rosen;
using namespace testing::ext;
using namespace OHOS;

/**
 * Function: IsScreenRCDLayer_True
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderSurfaceRCDLayer
 *                  2. expect IsScreenRCDLayer returns true
 */
HWTEST(RSRenderSurfaceRCDLayerTest, IsScreenRCDLayer_True, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    EXPECT_TRUE(layer->IsScreenRCDLayer());
}

/**
 * Function: UpdateRSLayerCmd_PixelMap_Null_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build PixelMap command with nullptr
 *                  2. update RSRenderSurfaceRCDLayer
 *                  3. expect no crash and command accepted
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_PixelMap_Null_NoCrash, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    std::shared_ptr<Media::PixelMap> pm = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::shared_ptr<Media::PixelMap>>>(pm);
    auto cmd = std::make_shared<RSRenderLayerPixelMapCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    SUCCEED();
}

/**
 * Function: UpdateRSLayerCmd_PixelMap_NonNull_SetsProperty
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build PixelMap command with non-null PixelMap
 *                  2. update RSRenderSurfaceRCDLayer
 *                  3. verify GetPixelMap returns the same pointer
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_PixelMap_NonNull_SetsProperty, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    // Create a tiny PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    auto upm = Media::PixelMap::Create(opts);
    ASSERT_NE(upm, nullptr);
    std::shared_ptr<Media::PixelMap> spm(upm.release());

    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::shared_ptr<Media::PixelMap>>>(spm);
    auto cmd = std::make_shared<RSRenderLayerPixelMapCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetPixelMap().get(), spm.get());
}

/**
 * Function: UpdateRSLayerCmd_Alpha_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: apply Alpha command and verify on RCD layer
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_Alpha_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    GraphicLayerAlpha alpha {
        .enGlobalAlpha = true,
        .enPixelAlpha = true,
        .alpha0 = 0,
        .alpha1 = 255,
        .gAlpha = 200,
    };
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerAlpha>>(alpha);
    auto cmd = std::make_shared<RSRenderLayerAlphaCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetAlpha().gAlpha, 200);
}

/**
 * Function: UpdateRSLayerCmd_Type_Transform_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: apply Type and Transform commands and verify
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_Type_Transform_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    auto propType = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerType>>(GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
    auto cmdType = std::make_shared<RSRenderLayerTypeCmd>(propType);
    layer->UpdateRSLayerCmd(cmdType);
    EXPECT_EQ(layer->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);

    auto propTr = std::make_shared<RSRenderLayerCmdProperty<GraphicTransformType>>(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    auto cmdTr = std::make_shared<RSRenderLayerTransformCmd>(propTr);
    layer->UpdateRSLayerCmd(cmdTr);
    EXPECT_EQ(layer->GetTransform(), GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/**
 * Function: UpdateRSLayerCmd_Regions_And_Timestamp_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: apply VisibleRegions, DirtyRegions and PresentTimestamp commands
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_Regions_And_Timestamp_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    std::vector<GraphicIRect> vis { GraphicIRect{0,0,5,5} };
    auto propV = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(vis);
    auto cmdV = std::make_shared<RSRenderLayerVisibleRegionsCmd>(propV);
    layer->UpdateRSLayerCmd(cmdV);
    ASSERT_EQ(layer->GetVisibleRegions().size(), 1u);

    std::vector<GraphicIRect> dirty { GraphicIRect{1,1,2,2} };
    auto propD = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(dirty);
    auto cmdD = std::make_shared<RSRenderLayerDirtyRegionsCmd>(propD);
    layer->UpdateRSLayerCmd(cmdD);
    ASSERT_EQ(layer->GetDirtyRegions().size(), 1u);

    GraphicPresentTimestamp ts { GRAPHIC_DISPLAY_PTS_TIMESTAMP, 999 };
    auto propTs = std::make_shared<RSRenderLayerCmdProperty<GraphicPresentTimestamp>>(ts);
    auto cmdTs = std::make_shared<RSRenderLayerPresentTimestampCmd>(propTs);
    layer->UpdateRSLayerCmd(cmdTs);
    EXPECT_EQ(layer->GetPresentTimestamp().time, 999);
}

/**
 * Function: UpdateRSLayerCmd_ColorSpace_And_Matrix_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply ColorDataSpace and Matrix commands and verify
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_ColorSpace_And_Matrix_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    auto propCs = std::make_shared<RSRenderLayerCmdProperty<GraphicColorDataSpace>>(GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);
    auto cmdCs = std::make_shared<RSRenderLayerColorDataSpaceCmd>(propCs);
    layer->UpdateRSLayerCmd(cmdCs);
    EXPECT_EQ(layer->GetColorDataSpace(), GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);

    GraphicMatrix m {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f};
    auto propM = std::make_shared<RSRenderLayerCmdProperty<GraphicMatrix>>(m);
    auto cmdM = std::make_shared<RSRenderLayerMatrixCmd>(propM);
    layer->UpdateRSLayerCmd(cmdM);
    EXPECT_EQ(layer->GetMatrix().scaleX, 1);
}

/**
 * Function: UpdateRSLayerCmd_LinearMatrix_And_CornerRadius_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply LayerLinearMatrix and CornerRadiusInfoForDRM
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_LinearMatrix_And_CornerRadius_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    std::vector<float> lm {3.0f, 2.0f, 1.0f};
    auto propLm = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(lm);
    auto cmdLm = std::make_shared<RSRenderLayerLayerLinearMatrixCmd>(propLm);
    layer->UpdateRSLayerCmd(cmdLm);
    ASSERT_EQ(layer->GetLayerLinearMatrix().size(), 3u);
    EXPECT_FLOAT_EQ(layer->GetLayerLinearMatrix()[0], 3.0f);

    std::vector<float> info {10.0f, 20.0f};
    auto propInfo = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(info);
    auto cmdInfo = std::make_shared<RSRenderLayerCornerRadiusInfoForDRMCmd>(propInfo);
    layer->UpdateRSLayerCmd(cmdInfo);
    ASSERT_EQ(layer->GetCornerRadiusInfoForDRM().size(), 2u);
}

/**
 * Function: UpdateRSLayerCmd_LayerMask_And_Flags_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply LayerMaskInfo and IsSupportedPresentTimestamp
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_LayerMask_And_Flags_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    auto propMask = std::make_shared<RSRenderLayerCmdProperty<LayerMask>>(LayerMask::LAYER_MASK_HBM_SYNC);
    auto cmdMask = std::make_shared<RSRenderLayerLayerMaskInfoCmd>(propMask);
    layer->UpdateRSLayerCmd(cmdMask);
    EXPECT_EQ(layer->GetLayerMaskInfo(), LayerMask::LAYER_MASK_HBM_SYNC);

    auto propFlag = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmdFlag = std::make_shared<RSRenderLayerIsSupportedPresentTimestampCmd>(propFlag);
    layer->UpdateRSLayerCmd(cmdFlag);
    EXPECT_TRUE(layer->GetIsSupportedPresentTimestamp());
}

/**
 * Function: UpdateRSLayerCmd_UseDeviceOffline_And_IgnoreAlpha_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply UseDeviceOffline and IgnoreAlpha
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_UseDeviceOffline_And_IgnoreAlpha_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    auto propOffline = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmdOffline = std::make_shared<RSRenderLayerUseDeviceOfflineCmd>(propOffline);
    layer->UpdateRSLayerCmd(cmdOffline);
    EXPECT_TRUE(layer->GetUseDeviceOffline());

    auto propIgnore = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmdIgnore = std::make_shared<RSRenderLayerIgnoreAlphaCmd>(propIgnore);
    layer->UpdateRSLayerCmd(cmdIgnore);
    EXPECT_TRUE(layer->GetIgnoreAlpha());
}

/**
 * Function: UpdateRSLayerCmd_AncoSrcRect_SurfaceUniqueId_CycleNum_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply AncoSrcRect, SurfaceUniqueId, CycleBuffersNum
 */
HWTEST(RSRenderSurfaceRCDLayerTest, UpdateRSLayerCmd_AncoSrcRect_SurfaceUniqueId_CycleNum_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    GraphicIRect r {2, 3, 4, 5};
    auto propRect = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
    auto cmdRect = std::make_shared<RSRenderLayerAncoSrcRectCmd>(propRect);
    layer->UpdateRSLayerCmd(cmdRect);
    EXPECT_EQ(layer->GetAncoSrcRect().w, 4);

    auto propId = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(999u);
    auto cmdId = std::make_shared<RSRenderLayerSurfaceUniqueIdCmd>(propId);
    layer->UpdateRSLayerCmd(cmdId);
    EXPECT_EQ(layer->GetSurfaceUniqueId(), 999u);

    auto propCycle = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(3u);
    auto cmdCycle = std::make_shared<RSRenderLayerCycleBuffersNumCmd>(propCycle);
    layer->UpdateRSLayerCmd(cmdCycle);
    EXPECT_EQ(layer->GetCycleBuffersNum(), 3u);
}

/**
 * Function: CopyLayerInfo_NullPtr_NoCrash
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: call CopyLayerInfo with nullptr; ensure pixelMap stays null
 */
HWTEST(RSRenderSurfaceRCDLayerTest, CopyLayerInfo_NullPtr_NoCrash, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceRCDLayer>();
    EXPECT_EQ(layer->GetPixelMap(), nullptr);
    layer->CopyLayerInfo(nullptr);
    EXPECT_EQ(layer->GetPixelMap(), nullptr);
}

/**
 * Function: CopyLayerInfo_FromBaseLayer_NoPixelMap
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: copy from RSRenderSurfaceLayer; base properties copied; pixelMap remains null
 */
HWTEST(RSRenderSurfaceRCDLayerTest, CopyLayerInfo_FromBaseLayer_NoPixelMap, TestSize.Level1)
{
    auto src = std::make_shared<RSRenderSurfaceLayer>();
    src->SetZorder(7);
    src->SetIsSupportedPresentTimestamp(true);

    auto dst = std::make_shared<RSRenderSurfaceRCDLayer>();
    dst->CopyLayerInfo(src);
    EXPECT_EQ(dst->GetZorder(), 7u);
    EXPECT_TRUE(dst->GetIsSupportedPresentTimestamp());
    EXPECT_EQ(dst->GetPixelMap(), nullptr);
}

/**
 * Function: CopyLayerInfo_FromRcdLayer_CopiesPixelMap
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: copy from RSRenderSurfaceRCDLayer; pixelMap and base properties copied
 */
HWTEST(RSRenderSurfaceRCDLayerTest, CopyLayerInfo_FromRcdLayer_CopiesPixelMap, TestSize.Level1)
{
    auto src = std::make_shared<RSRenderSurfaceRCDLayer>();
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    auto upm = Media::PixelMap::Create(opts);
    ASSERT_NE(upm, nullptr);
    std::shared_ptr<Media::PixelMap> spm(upm.release());
    src->SetPixelMap(spm);
    src->SetZorder(5);

    auto dst = std::make_shared<RSRenderSurfaceRCDLayer>();
    dst->CopyLayerInfo(src);
    EXPECT_EQ(dst->GetZorder(), 5u);
    ASSERT_NE(dst->GetPixelMap(), nullptr);
    EXPECT_EQ(dst->GetPixelMap().get(), spm.get());
}
