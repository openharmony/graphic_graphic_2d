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
    EXPECT_EQ(layer->GetTransformType(), GraphicTransformType::GRAPHIC_ROTATE_NONE);

    GraphicIRect r {1, 2, 3, 4};
    layer->SetCropRect(r);
    EXPECT_EQ(layer->GetCropRect().x, 1);
    EXPECT_EQ(layer->GetCropRect().y, 2);
    EXPECT_EQ(layer->GetCropRect().w, 3);
    EXPECT_EQ(layer->GetCropRect().h, 4);
}
