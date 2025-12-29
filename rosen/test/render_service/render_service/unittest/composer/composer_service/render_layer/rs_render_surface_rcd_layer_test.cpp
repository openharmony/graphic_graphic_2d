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
