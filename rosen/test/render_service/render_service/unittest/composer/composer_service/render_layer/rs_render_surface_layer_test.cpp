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
#include <limits>
#include "rs_render_surface_layer.h"
#include "rs_render_layer_cmd.h"

using namespace OHOS::Rosen;
using namespace OHOS;
using namespace testing;
using namespace testing::ext;
class RSRenderSurfaceLayerTest : public Test {};
/**
 * Function: UpdateRSLayerCmd_Zorder_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer and Zorder command
 *                  2. call UpdateRSLayerCmd
 *                  3. expect GetZorder equals set value
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_Zorder_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(9);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetZorder(), 9u);
}

/**
 * Function: UpdateRSLayerCmd_PreMulti_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer and PreMulti command
 *                  2. call UpdateRSLayerCmd
 *                  3. expect IsPreMulti equals set value
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_PreMulti_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmd = std::make_shared<RSRenderLayerPreMultiCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_TRUE(layer->IsPreMulti());
}

/**
 * Function: UpdateRSLayerCmd_IsMaskLayer_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer and IsMaskLayer command
 *                  2. call UpdateRSLayerCmd
 *                  3. expect GetIsMaskLayer equals set value
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_IsMaskLayer_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmd = std::make_shared<RSRenderLayerIsMaskLayerCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_TRUE(layer->GetIsMaskLayer());
}

/**
 * Function: UpdateRSLayerCmd_Gravity_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer and Gravity command
 *                  2. call UpdateRSLayerCmd
 *                  3. expect GetGravity equals set value
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_Gravity_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(3);
    auto cmd = std::make_shared<RSRenderLayerGravityCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetGravity(), 3);
}

/**
 * Function: UpdateRSLayerCmd_ColorDataSpace_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer and ColorDataSpace command
 *                  2. call UpdateRSLayerCmd
 *                  3. expect GetColorDataSpace equals set value
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_ColorDataSpace_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicColorDataSpace>>(
        GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);
    auto cmd = std::make_shared<RSRenderLayerColorDataSpaceCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetColorDataSpace(), GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);
}

/**
 * Function: UpdateRSLayerCmd_PixelMap_Unhandled_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer and PixelMap command (not handled by RSRenderSurfaceLayer)
 *                  2. call UpdateRSLayerCmd
 *                  3. expect no crash
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_PixelMap_Unhandled_NoCrash, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::shared_ptr<Media::PixelMap> pm = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::shared_ptr<Media::PixelMap>>>(pm);
    auto cmd = std::make_shared<RSRenderLayerPixelMapCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
}

/**
 * Function: Basic_Getter_Setter_Work
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set Type/Transform/CropRect
 *                  2. verify getters return written values
 */
HWTEST(RSRenderSurfaceLayerTest, Basic_Getter_Setter_Work, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
    EXPECT_EQ(layer->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);

    layer->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    EXPECT_EQ(layer->GetTransform(), GraphicTransformType::GRAPHIC_ROTATE_NONE);

    GraphicIRect r {1, 2, 3, 4};
    layer->SetCropRect(r);
    EXPECT_EQ(layer->GetCropRect().x, 1);
    EXPECT_EQ(layer->GetCropRect().y, 2);
    EXPECT_EQ(layer->GetCropRect().w, 3);
    EXPECT_EQ(layer->GetCropRect().h, 4);
}

/**
 * Function: UpdateRSLayerCmd_Alpha_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply Alpha command and verify value
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_Alpha_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicLayerAlpha alpha {
        .enGlobalAlpha = true,
        .enPixelAlpha = true,
        .alpha0 = 0,
        .alpha1 = 255,
        .gAlpha = 128,
    };
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerAlpha>>(alpha);
    auto cmd = std::make_shared<RSRenderLayerAlphaCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetAlpha().gAlpha, 128);
}

/**
 * Function: UpdateRSLayerCmd_Type_And_Transform_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply Type and Transform commands and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_Type_And_Transform_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
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
 * Function: UpdateRSLayerCmd_LayerColor_And_Matrix_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply LayerColor and Matrix commands and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_LayerColor_And_Matrix_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicLayerColor c { .r = 10, .g = 20, .b = 30, .a = 40 };
    auto propC = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerColor>>(c);
    auto cmdC = std::make_shared<RSRenderLayerLayerColorCmd>(propC);
    layer->UpdateRSLayerCmd(cmdC);
    EXPECT_EQ(layer->GetLayerColor().r, 10);

    GraphicMatrix m {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f};
    auto propM = std::make_shared<RSRenderLayerCmdProperty<GraphicMatrix>>(m);
    auto cmdM = std::make_shared<RSRenderLayerMatrixCmd>(propM);
    layer->UpdateRSLayerCmd(cmdM);
    EXPECT_EQ(layer->GetMatrix().scaleX, 1);
}

/**
 * Function: UpdateRSLayerCmd_Visible_And_Dirty_Regions_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply VisibleRegions and DirtyRegions commands and verify sizes
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_Visible_And_Dirty_Regions_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<GraphicIRect> vis { GraphicIRect{0,0,10,10} };
    auto propV = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(vis);
    auto cmdV = std::make_shared<RSRenderLayerVisibleRegionsCmd>(propV);
    layer->UpdateRSLayerCmd(cmdV);
    ASSERT_EQ(layer->GetVisibleRegions().size(), 1u);

    std::vector<GraphicIRect> dirty { GraphicIRect{1,1,5,5}, GraphicIRect{2,2,3,3} };
    auto propD = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(dirty);
    auto cmdD = std::make_shared<RSRenderLayerDirtyRegionsCmd>(propD);
    layer->UpdateRSLayerCmd(cmdD);
    ASSERT_EQ(layer->GetDirtyRegions().size(), 2u);
}

/**
 * Function: UpdateRSLayerCmd_TunnelHandleChange_And_NodeId_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply TunnelHandleChange and NodeId commands and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_TunnelHandleChange_And_NodeId_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto propChg = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmdChg = std::make_shared<RSRenderLayerTunnelHandleChangeCmd>(propChg);
    layer->UpdateRSLayerCmd(cmdChg);
    EXPECT_TRUE(layer->GetTunnelHandleChange());

    auto propNode = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(777u);
    auto cmdNode = std::make_shared<RSRenderLayerNodeIdCmd>(propNode);
    layer->UpdateRSLayerCmd(cmdNode);
    EXPECT_EQ(layer->GetNodeId(), 777u);
}

/**
 * Function: UpdateRSLayerCmd_WindowsName_And_SurfaceName_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply WindowsName and SurfaceName commands and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_WindowsName_And_SurfaceName_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<std::string> windowsName {"winA", "winB"};
    auto propWin = std::make_shared<RSRenderLayerCmdProperty<std::vector<std::string>>>(windowsName);
    auto cmdWin = std::make_shared<RSRenderLayerWindowsNameCmd>(propWin);
    layer->UpdateRSLayerCmd(cmdWin);
    ASSERT_EQ(layer->GetWindowsName().size(), 2u);
    EXPECT_EQ(layer->GetWindowsName()[0], "winA");

    std::string surfaceName {"surface_main"};
    auto propSurf = std::make_shared<RSRenderLayerCmdProperty<std::string>>(surfaceName);
    auto cmdSurf = std::make_shared<RSRenderLayerSurfaceNameCmd>(propSurf);
    layer->UpdateRSLayerCmd(cmdSurf);
    EXPECT_EQ(layer->GetSurfaceName(), "surface_main");
}

/**
 * Function: UpdateRSLayerCmd_SdrNit_DisplayNit_BrightnessRatio_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply SdrNit/DisplayNit/BrightnessRatio and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_SdrNit_DisplayNit_BrightnessRatio_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto propSdr = std::make_shared<RSRenderLayerCmdProperty<float>>(250.5f);
    auto cmdSdr = std::make_shared<RSRenderLayerSdrNitCmd>(propSdr);
    layer->UpdateRSLayerCmd(cmdSdr);
    EXPECT_FLOAT_EQ(layer->GetSdrNit(), 250.5f);

    auto propDisp = std::make_shared<RSRenderLayerCmdProperty<float>>(600.25f);
    auto cmdDisp = std::make_shared<RSRenderLayerDisplayNitCmd>(propDisp);
    layer->UpdateRSLayerCmd(cmdDisp);
    EXPECT_FLOAT_EQ(layer->GetDisplayNit(), 600.25f);

    auto propRatio = std::make_shared<RSRenderLayerCmdProperty<float>>(0.5f);
    auto cmdRatio = std::make_shared<RSRenderLayerBrightnessRatioCmd>(propRatio);
    layer->UpdateRSLayerCmd(cmdRatio);
    EXPECT_FLOAT_EQ(layer->GetBrightnessRatio(), 0.5f);
}

/**
 * Function: UpdateRSLayerCmd_ColorTransform_And_LayerLinearMatrix_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply ColorTransform and LayerLinearMatrix and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_ColorTransform_And_LayerLinearMatrix_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<float> ct {1.0f, 2.0f, 3.0f};
    auto propCt = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(ct);
    auto cmdCt = std::make_shared<RSRenderLayerColorTransformCmd>(propCt);
    layer->UpdateRSLayerCmd(cmdCt);
    ASSERT_EQ(layer->GetColorTransform().size(), 3u);
    EXPECT_FLOAT_EQ(layer->GetColorTransform()[1], 2.0f);

    std::vector<float> lm {9.0f, 8.0f, 7.0f};
    auto propLm = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(lm);
    auto cmdLm = std::make_shared<RSRenderLayerLayerLinearMatrixCmd>(propLm);
    layer->UpdateRSLayerCmd(cmdLm);
    ASSERT_EQ(layer->GetLayerLinearMatrix().size(), 3u);
    EXPECT_FLOAT_EQ(layer->GetLayerLinearMatrix()[0], 9.0f);
}

/**
 * Function: UpdateRSLayerCmd_CornerRadiusInfoForDRM_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply CornerRadiusInfoForDRM and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_CornerRadiusInfoForDRM_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<float> info {10.0f, 20.0f};
    auto propInfo = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(info);
    auto cmdInfo = std::make_shared<RSRenderLayerCornerRadiusInfoForDRMCmd>(propInfo);
    layer->UpdateRSLayerCmd(cmdInfo);
    ASSERT_EQ(layer->GetCornerRadiusInfoForDRM().size(), 2u);
    EXPECT_FLOAT_EQ(layer->GetCornerRadiusInfoForDRM()[1], 20.0f);
}

/**
 * Function: UpdateRSLayerCmd_LayerMaskInfo_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply LayerMaskInfo and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_LayerMaskInfo_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto propMask = std::make_shared<RSRenderLayerCmdProperty<LayerMask>>(LayerMask::LAYER_MASK_HBM_SYNC);
    auto cmdMask = std::make_shared<RSRenderLayerLayerMaskInfoCmd>(propMask);
    layer->UpdateRSLayerCmd(cmdMask);
    EXPECT_EQ(layer->GetLayerMaskInfo(), LayerMask::LAYER_MASK_HBM_SYNC);
}

/**
 * Function: UpdateRSLayerCmd_UseDeviceOffline_And_IgnoreAlpha_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply UseDeviceOffline and IgnoreAlpha and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_UseDeviceOffline_And_IgnoreAlpha_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
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
 * Function: UpdateRSLayerCmd_AncoSrcRect_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply AncoSrcRect and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_AncoSrcRect_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicIRect r {5, 6, 7, 8};
    auto propRect = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
    auto cmdRect = std::make_shared<RSRenderLayerAncoSrcRectCmd>(propRect);
    layer->UpdateRSLayerCmd(cmdRect);
    EXPECT_EQ(layer->GetAncoSrcRect().x, 5);
    EXPECT_EQ(layer->GetAncoSrcRect().h, 8);
}

/**
 * Function: UpdateRSLayerCmd_SolidColorLayerProperty_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply SolidColorLayerProperty and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_SolidColorLayerProperty_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicSolidColorLayerProperty prop { .zOrder = 3, .transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE,
        .layerRect = { 1, 2, 10, 20 }, .compositionType = GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT,
        .layerColor = { 12, 34, 56, 78 } };
    auto scProp = std::make_shared<RSRenderLayerCmdProperty<GraphicSolidColorLayerProperty>>(prop);
    auto cmd = std::make_shared<RSRenderLayerSolidColorLayerPropertyCmd>(scProp);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetSolidColorLayerProperty().zOrder, 3);
    EXPECT_EQ(layer->GetSolidColorLayerProperty().layerRect.w, 10);
}

/**
 * Function: UpdateRSLayerCmd_SurfaceUniqueId_And_CycleBuffersNum_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply SurfaceUniqueId and CycleBuffersNum and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_SurfaceUniqueId_And_CycleBuffersNum_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto propId = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(12345u);
    auto cmdId = std::make_shared<RSRenderLayerSurfaceUniqueIdCmd>(propId);
    layer->UpdateRSLayerCmd(cmdId);
    EXPECT_EQ(layer->GetSurfaceUniqueId(), 12345u);

    auto propCycle = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(4u);
    auto cmdCycle = std::make_shared<RSRenderLayerCycleBuffersNumCmd>(propCycle);
    layer->UpdateRSLayerCmd(cmdCycle);
    EXPECT_EQ(layer->GetCycleBuffersNum(), 4u);
}

/**
 * Function: UpdateRSLayerCmd_TunnelHandle_NullPtr_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply TunnelHandle with nullptr and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_TunnelHandle_NullPtr_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    sptr<SurfaceTunnelHandle> handle = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceTunnelHandle>>>(handle);
    auto cmd = std::make_shared<RSRenderLayerTunnelHandleCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetTunnelHandle(), nullptr);
}

/**
 * Function: UpdateRSLayerCmd_BackgroundColor_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply BackgroundColor and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_BackgroundColor_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicLayerColor bg { .r = 11, .g = 22, .b = 33, .a = 44 };
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerColor>>(bg);
    auto cmd = std::make_shared<RSRenderLayerBackgroundColorCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetBackgroundColor().g, 22);
    EXPECT_EQ(layer->GetBackgroundColor().a, 44);
}

/**
 * Function: UpdateRSLayerCmd_BoundSize_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply BoundSize and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_BoundSize_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicIRect r {7, 8, 9, 10};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
    auto cmd = std::make_shared<RSRenderLayerBoundSizeCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetBoundSize().x, 7);
    EXPECT_EQ(layer->GetBoundSize().h, 10);
}

/**
 * Function: UpdateRSLayerCmd_CropRect_Command_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply CropRect via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_CropRect_Command_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicIRect r {3, 4, 5, 6};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
    auto cmd = std::make_shared<RSRenderLayerCropRectCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetCropRect().y, 4);
    EXPECT_EQ(layer->GetCropRect().w, 5);
}

/**
 * Function: UpdateRSLayerCmd_TunnelLayerId_And_Property_Command_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply TunnelLayerId & TunnelLayerProperty via commands and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_TunnelLayerId_And_Property_Command_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto propId = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(888u);
    auto cmdId = std::make_shared<RSRenderLayerTunnelLayerIdCmd>(propId);
    layer->UpdateRSLayerCmd(cmdId);
    EXPECT_EQ(layer->GetTunnelLayerId(), 888u);

    auto propProp = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(0x1234u);
    auto cmdProp = std::make_shared<RSRenderLayerTunnelLayerPropertyCmd>(propProp);
    layer->UpdateRSLayerCmd(cmdProp);
    EXPECT_EQ(layer->GetTunnelLayerProperty(), 0x1234u);
}

/**
 * Function: UpdateRSLayerCmd_IsNeedComposition_Toggle
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: toggle IsNeedComposition true/false via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_IsNeedComposition_Toggle, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
        auto cmd = std::make_shared<RSRenderLayerIsNeedCompositionCmd>(prop);
        layer->UpdateRSLayerCmd(cmd);
        EXPECT_TRUE(layer->GetIsNeedComposition());
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(false);
        auto cmd = std::make_shared<RSRenderLayerIsNeedCompositionCmd>(prop);
        layer->UpdateRSLayerCmd(cmd);
        EXPECT_FALSE(layer->GetIsNeedComposition());
    }
}

/**
 * Function: UpdateRSLayerCmd_LayerSize_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply LayerSize via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_LayerSize_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicIRect r {12, 13, 100, 200};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
    auto cmd = std::make_shared<RSRenderLayerLayerSizeCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetLayerSize().x, 12);
    EXPECT_EQ(layer->GetLayerSize().w, 100);
}

/**
 * Function: UpdateRSLayerCmd_Rotation_Arsr_Copybit_Bilinear_Cmds_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply RotationFixed/LayerArsr/LayerCopybit/NeedBilinearInterpolation via command
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_Rotation_Arsr_Copybit_Bilinear_Cmds_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerRotationFixedCmd>(
        std::make_shared<RSRenderLayerCmdProperty<bool>>(true)));
    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerLayerArsrCmd>(
        std::make_shared<RSRenderLayerCmdProperty<bool>>(false)));
    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerLayerCopybitCmd>(
        std::make_shared<RSRenderLayerCmdProperty<bool>>(true)));
    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerNeedBilinearInterpolationCmd>(
        std::make_shared<RSRenderLayerCmdProperty<bool>>(true)));

    EXPECT_TRUE(layer->GetRotationFixed());
    EXPECT_FALSE(layer->GetLayerArsr());
    EXPECT_TRUE(layer->GetLayerCopybit());
    EXPECT_TRUE(layer->GetNeedBilinearInterpolation());
}

/**
 * Function: UpdateRSLayerCmd_Buffer_And_PreBuffer_Null_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply Buffer/PreBuffer with nullptr via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_Buffer_And_PreBuffer_Null_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    {
        sptr<SurfaceBuffer> sb = nullptr;
        auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceBuffer>>>(sb);
        auto cmd = std::make_shared<RSRenderLayerBufferCmd>(prop);
        layer->UpdateRSLayerCmd(cmd);
        EXPECT_EQ(layer->GetBuffer(), sb);
    }
    {
        sptr<SurfaceBuffer> pre = nullptr;
        auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceBuffer>>>(pre);
        auto cmd = std::make_shared<RSRenderLayerPreBufferCmd>(prop);
        layer->UpdateRSLayerCmd(cmd);
        EXPECT_EQ(layer->GetPreBuffer(), pre);
    }
}

/**
 * Function: IsAncoNative_FlagTrueAndFalse
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: verify IsAncoNative for flag set vs unset
 */
HWTEST(RSRenderSurfaceLayerTest, IsAncoNative_FlagTrueAndFalse, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    // Set flag with ANCO_NATIVE_NODE bit set
    uint32_t nativeFlag = static_cast<uint32_t>(AncoFlags::ANCO_NATIVE_NODE);
    auto propSet = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(nativeFlag);
    auto cmdSet = std::make_shared<RSRenderLayerAncoFlagsCmd>(propSet);
    layer->UpdateRSLayerCmd(cmdSet);
    EXPECT_TRUE(layer->IsAncoNative());

    // Clear by setting a flag without the bit
    uint32_t noNative = static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE);
    auto propClr = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(noNative);
    auto cmdClr = std::make_shared<RSRenderLayerAncoFlagsCmd>(propClr);
    layer->UpdateRSLayerCmd(cmdClr);
    EXPECT_FALSE(layer->IsAncoNative());
}

/**
 * Function: CopyLayerInfo_CopiesSelectedFields
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: set fields on source layer, copy to target, verify selected fields
 */
HWTEST(RSRenderSurfaceLayerTest, CopyLayerInfo_CopiesSelectedFields, TestSize.Level1)
{
    auto src = std::make_shared<RSRenderSurfaceLayer>();
    auto dst = std::make_shared<RSRenderSurfaceLayer>();

    // Configure source via commands
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerZorderCmd>(
        std::make_shared<RSRenderLayerCmdProperty<int32_t>>(5)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerTypeCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicLayerType>>(GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerLayerSizeCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(GraphicIRect{10, 20, 30, 40})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerBoundSizeCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(GraphicIRect{1, 2, 3, 4})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerVisibleRegionsCmd>(
        std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(std::vector<GraphicIRect>{{0,0,5,5}})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerDirtyRegionsCmd>(
        std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(std::vector<GraphicIRect>{{1,1,2,2}})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerCropRectCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(GraphicIRect{3, 4, 5, 6})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerMatrixCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicMatrix>>(
            GraphicMatrix{1.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,1.f})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerGravityCmd>(
        std::make_shared<RSRenderLayerCmdProperty<int32_t>>(7)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerAlphaCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicLayerAlpha>>(GraphicLayerAlpha{true,true,0,255,64})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerTransformCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicTransformType>>(GraphicTransformType::GRAPHIC_ROTATE_NONE)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerBlendTypeCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicBlendType>>(GraphicBlendType::GRAPHIC_BLEND_SRCOVER)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerColorTransformCmd>(
        std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(std::vector<float>{1.f,2.f,3.f})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerColorDataSpaceCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicColorDataSpace>>(
            GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerLayerColorCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicLayerColor>>(GraphicLayerColor{1,2,3,4})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerBackgroundColorCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicLayerColor>>(GraphicLayerColor{5,6,7,8})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerCornerRadiusInfoForDRMCmd>(
        std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(std::vector<float>{0.1f, 0.2f})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerWindowsNameCmd>(
        std::make_shared<RSRenderLayerCmdProperty<std::vector<std::string>>>(std::vector<std::string>{"w1"})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerAncoFlagsCmd>(
        std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(static_cast<uint32_t>(AncoFlags::ANCO_NATIVE_NODE))));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerIsMaskLayerCmd>(
        std::make_shared<RSRenderLayerCmdProperty<bool>>(true)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerLayerMaskInfoCmd>(
        std::make_shared<RSRenderLayerCmdProperty<LayerMask>>(LayerMask::LAYER_MASK_HBM_SYNC)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerCycleBuffersNumCmd>(
        std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(2u)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerSurfaceNameCmd>(
        std::make_shared<RSRenderLayerCmdProperty<std::string>>(std::string("src_surf"))));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerSolidColorLayerPropertyCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicSolidColorLayerProperty>>(
            GraphicSolidColorLayerProperty{9, GraphicTransformType::GRAPHIC_ROTATE_NONE, {0,0,1,1},
                GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT, {1,1,1,1}})));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerIsNeedCompositionCmd>(
        std::make_shared<RSRenderLayerCmdProperty<bool>>(false)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerUseDeviceOfflineCmd>(
        std::make_shared<RSRenderLayerCmdProperty<bool>>(true)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerIgnoreAlphaCmd>(
        std::make_shared<RSRenderLayerCmdProperty<bool>>(true)));
    src->UpdateRSLayerCmd(std::make_shared<RSRenderLayerAncoSrcRectCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(GraphicIRect{11,12,13,14})));

    dst->CopyLayerInfo(src);

    EXPECT_EQ(dst->GetZorder(), 5u);
    EXPECT_EQ(dst->GetLayerSize().w, 30);
    EXPECT_EQ(dst->GetVisibleRegions().size(), 1u);
    EXPECT_EQ(dst->GetCropRect().x, 3);
    EXPECT_EQ(dst->GetGravity(), 7);
    EXPECT_EQ(dst->GetAlpha().gAlpha, 64);
    EXPECT_EQ(dst->GetBlendType(), GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    EXPECT_EQ(dst->GetColorDataSpace(), GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);
    EXPECT_EQ(dst->GetBackgroundColor().a, 8);
    EXPECT_TRUE(dst->GetIsSupportedPresentTimestamp());
    EXPECT_EQ(dst->GetPresentTimestamp().time, 999);
    ASSERT_EQ(dst->GetWindowsName().size(), 1u);
    EXPECT_TRUE(dst->IsAncoNative());
    EXPECT_TRUE(dst->GetIsMaskLayer());
    EXPECT_EQ(dst->GetLayerMaskInfo(), LayerMask::LAYER_MASK_HBM_SYNC);
    EXPECT_EQ(dst->GetCycleBuffersNum(), 2u);
    EXPECT_EQ(dst->GetSurfaceName(), "src_surf");
    EXPECT_FALSE(dst->GetIsNeedComposition());
    EXPECT_TRUE(dst->GetUseDeviceOffline());
    EXPECT_TRUE(dst->GetIgnoreAlpha());
    EXPECT_EQ(dst->GetAncoSrcRect().y, 12);
}

/**
 * Function: UpdateRSLayerCmd_Composition_And_Blend_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply CompositionType and BlendType commands and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_Composition_And_Blend_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto propComp = std::make_shared<RSRenderLayerCmdProperty<GraphicCompositionType>>(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    auto cmdComp = std::make_shared<RSRenderLayerCompositionTypeCmd>(propComp);
    layer->UpdateRSLayerCmd(cmdComp);
    EXPECT_EQ(layer->GetCompositionType(), GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    auto propBlend = std::make_shared<RSRenderLayerCmdProperty<GraphicBlendType>>(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    auto cmdBlend = std::make_shared<RSRenderLayerBlendTypeCmd>(propBlend);
    layer->UpdateRSLayerCmd(cmdBlend);
    EXPECT_EQ(layer->GetBlendType(), GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
}

/**
 * Function: UpdateRSLayerCmd_TunnelHandle_NonNull_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply TunnelHandle command with non-null handle and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_TunnelHandle_NonNull_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    sptr<SurfaceTunnelHandle> handle = sptr<SurfaceTunnelHandle>::MakeSptr();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceTunnelHandle>>>(handle);
    auto cmd = std::make_shared<RSRenderLayerTunnelHandleCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetTunnelHandle(), handle);
}

/**
 * Function: TunnelLayerId_And_Property_Setter_Getter
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: set TunnelLayerId and TunnelLayerProperty and verify getters
 */
HWTEST(RSRenderSurfaceLayerTest, TunnelLayerId_And_Property_Setter_Getter, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetTunnelLayerId(555u);
    layer->SetTunnelLayerProperty(0xABCDu);
    EXPECT_EQ(layer->GetTunnelLayerId(), 555u);
    EXPECT_EQ(layer->GetTunnelLayerProperty(), 0xABCDu);
}

/**
 * Function: RotationFixed_Arsr_Copybit_Bilinear_Toggles
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: toggle RotationFixed/LayerArsr/LayerCopybit/NeedBilinearInterpolation
 */
HWTEST(RSRenderSurfaceLayerTest, RotationFixed_Arsr_Copybit_Bilinear_Toggles, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetRotationFixed(true);
    layer->SetLayerArsr(false);
    layer->SetLayerCopybit(true);
    layer->SetNeedBilinearInterpolation(true);
    EXPECT_TRUE(layer->GetRotationFixed());
    EXPECT_FALSE(layer->GetLayerArsr());
    EXPECT_TRUE(layer->GetLayerCopybit());
    EXPECT_TRUE(layer->GetNeedBilinearInterpolation());

    layer->SetRotationFixed(false);
    layer->SetLayerArsr(true);
    layer->SetLayerCopybit(false);
    layer->SetNeedBilinearInterpolation(false);
    EXPECT_FALSE(layer->GetRotationFixed());
    EXPECT_TRUE(layer->GetLayerArsr());
    EXPECT_FALSE(layer->GetLayerCopybit());
    EXPECT_FALSE(layer->GetNeedBilinearInterpolation());
}

/**
 * Function: AcquireFence_And_Buffer_Set_Get
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: set acquire fence and buffers and verify getters
 */
HWTEST(RSRenderSurfaceLayerTest, AcquireFence_And_Buffer_Set_Get, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    sptr<SyncFence> fence = SyncFence::InvalidFence();
    layer->SetAcquireFence(fence);
    EXPECT_EQ(layer->GetAcquireFence(), fence);

    sptr<SurfaceBuffer> sb = nullptr;
    layer->SetBuffer(sb);
    EXPECT_EQ(layer->GetBuffer(), sb);

    sptr<SurfaceBuffer> pre = nullptr;
    layer->SetPreBuffer(pre);
    EXPECT_EQ(layer->GetPreBuffer(), pre);
}

/**
 * Function: UpdateRSLayerCmd_AcquireFence_Command_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply AcquireFence via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_AcquireFence_Command_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    sptr<SyncFence> fence = SyncFence::InvalidFence();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SyncFence>>>(fence);
    auto cmd = std::make_shared<RSRenderLayerAcquireFenceCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetAcquireFence(), fence);
}

/**
 * Function: UpdateRSLayerCmd_VisibleRegions_Empty_Command_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply empty VisibleRegions via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_VisibleRegions_Empty_Command_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<GraphicIRect> vis; // empty
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(vis);
    auto cmd = std::make_shared<RSRenderLayerVisibleRegionsCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    ASSERT_EQ(layer->GetVisibleRegions().size(), 0u);
}

/**
 * Function: UpdateRSLayerCmd_DirtyRegions_Empty_Command_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply empty DirtyRegions via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_DirtyRegions_Empty_Command_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<GraphicIRect> dirty; // empty
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(dirty);
    auto cmd = std::make_shared<RSRenderLayerDirtyRegionsCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    ASSERT_EQ(layer->GetDirtyRegions().size(), 0u);
}

/**
 * Function: UpdateRSLayerCmd_WindowsName_Empty_Command_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply empty WindowsName via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_WindowsName_Empty_Command_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<std::string> windowsName; // empty
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<std::string>>>(windowsName);
    auto cmd = std::make_shared<RSRenderLayerWindowsNameCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    ASSERT_EQ(layer->GetWindowsName().size(), 0u);
}

/**
 * Function: UpdateRSLayerCmd_SurfaceName_Empty_Command_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply empty SurfaceName via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_SurfaceName_Empty_Command_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::string name; // empty
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::string>>(name);
    auto cmd = std::make_shared<RSRenderLayerSurfaceNameCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetSurfaceName(), "");
}

/**
 * Function: UpdateRSLayerCmd_ColorTransform_Empty_Command_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply empty ColorTransform via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_ColorTransform_Empty_Command_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<float> ct; // empty
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(ct);
    auto cmd = std::make_shared<RSRenderLayerColorTransformCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    ASSERT_EQ(layer->GetColorTransform().size(), 0u);
}

/**
 * Function: UpdateRSLayerCmd_LayerLinearMatrix_Empty_Command_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply empty LayerLinearMatrix via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_LayerLinearMatrix_Empty_Command_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<float> lm; // empty
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(lm);
    auto cmd = std::make_shared<RSRenderLayerLayerLinearMatrixCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    ASSERT_EQ(layer->GetLayerLinearMatrix().size(), 0u);
}

/**
 * Function: UpdateRSLayerCmd_RSLayerId_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply RSLayerId via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_RSLayerId_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(1001u);
    auto cmd = std::make_shared<RSRenderLayerRSLayerIdCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetRSLayerId(), 1001u);
}

/**
 * Function: UpdateRSLayerCmd_LayerSourceTuning_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply LayerSourceTuning via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_LayerSourceTuning_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(15);
    auto cmd = std::make_shared<RSRenderLayerLayerSourceTuningCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetLayerSourceTuning(), 15);
}

/**
 * Function: UpdateRSLayerCmd_UniRenderFlag_Toggle
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: toggle UniRenderFlag true/false via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_UniRenderFlag_Toggle, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
        auto cmd = std::make_shared<RSRenderLayerUniRenderFlagCmd>(prop);
        layer->UpdateRSLayerCmd(cmd);
        EXPECT_TRUE(layer->GetUniRenderFlag());
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(false);
        auto cmd = std::make_shared<RSRenderLayerUniRenderFlagCmd>(prop);
        layer->UpdateRSLayerCmd(cmd);
        EXPECT_FALSE(layer->GetUniRenderFlag());
    }
}

/**
 * Function: UpdateRSLayerCmd_MetaData_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply MetaData via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_MetaData_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<GraphicHDRMetaData> metas(2); // default entries
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicHDRMetaData>>>(metas);
    auto cmd = std::make_shared<RSRenderLayerMetaDataCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    ASSERT_EQ(layer->GetMetaData().size(), 2u);
}

/**
 * Function: UpdateRSLayerCmd_MetaDataSet_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply MetaDataSet via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_MetaDataSet_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicHDRMetaDataSet set{};
    set.key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X;
    set.metaData = {1, 2, 3};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicHDRMetaDataSet>>(set);
    auto cmd = std::make_shared<RSRenderLayerMetaDataSetCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetMetaDataSet().key, GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X);
    ASSERT_EQ(layer->GetMetaDataSet().metaData.size(), 3u);
}

/**
 * Function: UpdateRSLayerCmd_MetaData_Empty_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply empty MetaData via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_MetaData_Empty_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<GraphicHDRMetaData> metas; // empty
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicHDRMetaData>>>(metas);
    auto cmd = std::make_shared<RSRenderLayerMetaDataCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    ASSERT_EQ(layer->GetMetaData().size(), 0u);
}

/**
 * Function: UpdateRSLayerCmd_MetaDataSet_EmptyData_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply MetaDataSet with empty data and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_MetaDataSet_EmptyData_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicHDRMetaDataSet set{};
    set.key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_BLUE_PRIMARY_X;
    set.metaData = {}; // empty
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicHDRMetaDataSet>>(set);
    auto cmd = std::make_shared<RSRenderLayerMetaDataSetCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetMetaDataSet().key, GraphicHDRMetadataKey::GRAPHIC_MATAKEY_BLUE_PRIMARY_X);
    ASSERT_EQ(layer->GetMetaDataSet().metaData.size(), 0u);
}

/**
 * Function: UpdateRSLayerCmd_AcquireFence_Null_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply AcquireFence with nullptr via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_AcquireFence_Null_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    sptr<SyncFence> fence = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SyncFence>>>(fence);
    auto cmd = std::make_shared<RSRenderLayerAcquireFenceCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetAcquireFence(), nullptr);
}

/**
 * Function: UpdateRSLayerCmd_SurfaceName_Long_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply very long SurfaceName via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_SurfaceName_Long_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::string longName(1024, 'X');
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::string>>(longName);
    auto cmd = std::make_shared<RSRenderLayerSurfaceNameCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetSurfaceName().size(), 1024u);
}

/**
 * Function: UpdateRSLayerCmd_WindowsName_Long_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply very long WindowsName entry via command and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_WindowsName_Long_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::string longEntry(512, 'W');
    std::vector<std::string> windowsName { longEntry };
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<std::string>>>(windowsName);
    auto cmd = std::make_shared<RSRenderLayerWindowsNameCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    ASSERT_EQ(layer->GetWindowsName().size(), 1u);
    EXPECT_EQ(layer->GetWindowsName()[0].size(), 512u);
}

/**
 * Function: UpdateRSLayerCmd_ColorTransform_LargeVector_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply large ColorTransform vector and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_ColorTransform_LargeVector_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    std::vector<float> ct(64, 0.5f);
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(ct);
    auto cmd = std::make_shared<RSRenderLayerColorTransformCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    ASSERT_EQ(layer->GetColorTransform().size(), 64u);
    EXPECT_FLOAT_EQ(layer->GetColorTransform()[63], 0.5f);
}

/**
 * Function: UpdateRSLayerCmd_Rect_Negative_And_Zero_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply negative/zero rects for LayerSize/BoundSize/CropRect
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_Rect_Negative_And_Zero_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerLayerSizeCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(GraphicIRect{-10, -20, 0, 0})));
    EXPECT_EQ(layer->GetLayerSize().x, -10);
    EXPECT_EQ(layer->GetLayerSize().w, 0);

    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerBoundSizeCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(GraphicIRect{-5, -6, 1, 0})));
    EXPECT_EQ(layer->GetBoundSize().y, -6);
    EXPECT_EQ(layer->GetBoundSize().h, 0);

    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerCropRectCmd>(
        std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(GraphicIRect{-1, -2, -3, -4})));
    EXPECT_EQ(layer->GetCropRect().x, -1);
    EXPECT_EQ(layer->GetCropRect().w, -3);
}

/**
 * Function: UpdateRSLayerCmd_BrightnessRatio_EdgeValues_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply BrightnessRatio 0.0 and 1.0 and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_BrightnessRatio_EdgeValues_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerBrightnessRatioCmd>(
        std::make_shared<RSRenderLayerCmdProperty<float>>(0.0f)));
    EXPECT_FLOAT_EQ(layer->GetBrightnessRatio(), 0.0f);

    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerBrightnessRatioCmd>(
        std::make_shared<RSRenderLayerCmdProperty<float>>(1.0f)));
    EXPECT_FLOAT_EQ(layer->GetBrightnessRatio(), 1.0f);
}

/**
 * Function: UpdateRSLayerCmd_CycleBuffersNum_And_Ids_Boundary_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply boundary values for CycleBuffersNum, RSLayerId, SurfaceUniqueId, NodeId
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_CycleBuffersNum_And_Ids_Boundary_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerCycleBuffersNumCmd>(
        std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(0u)));
    EXPECT_EQ(layer->GetCycleBuffersNum(), 0u);

    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerRSLayerIdCmd>(
        std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(0ull)));
    EXPECT_EQ(layer->GetRSLayerId(), 0ull);

    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerSurfaceUniqueIdCmd>(
        std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(std::numeric_limits<uint64_t>::max())));
    EXPECT_EQ(layer->GetSurfaceUniqueId(), std::numeric_limits<uint64_t>::max());

    layer->UpdateRSLayerCmd(std::make_shared<RSRenderLayerNodeIdCmd>(
        std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(std::numeric_limits<uint64_t>::max())));
    EXPECT_EQ(layer->GetNodeId(), std::numeric_limits<uint64_t>::max());
}
