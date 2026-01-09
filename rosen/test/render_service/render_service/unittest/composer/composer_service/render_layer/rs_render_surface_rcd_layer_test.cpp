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
