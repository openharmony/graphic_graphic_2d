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
    SUCCEED();
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
 * Function: UpdateRSLayerCmd_PresentTimestamp_And_SupportFlag_Applied
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: apply PresentTimestamp and IsSupportedPresentTimestamp commands and verify
 */
HWTEST(RSRenderSurfaceLayerTest, UpdateRSLayerCmd_PresentTimestamp_And_SupportFlag_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceLayer>();
    GraphicPresentTimestamp ts { GRAPHIC_DISPLAY_PTS_TIMESTAMP, 12345678 };
    auto propTs = std::make_shared<RSRenderLayerCmdProperty<GraphicPresentTimestamp>>(ts);
    auto cmdTs = std::make_shared<RSRenderLayerPresentTimestampCmd>(propTs);
    layer->UpdateRSLayerCmd(cmdTs);
    EXPECT_EQ(layer->GetPresentTimestamp().time, 12345678);

    auto propSupp = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmdSupp = std::make_shared<RSRenderLayerIsSupportedPresentTimestampCmd>(propSupp);
    layer->UpdateRSLayerCmd(cmdSupp);
    EXPECT_TRUE(layer->GetIsSupportedPresentTimestamp());
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
