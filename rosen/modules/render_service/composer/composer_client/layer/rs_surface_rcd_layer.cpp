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

#include "rs_surface_rcd_layer.h"
#include <memory>
#include "rs_layer_parcel.h"
#include "rs_layer_context.h"
#include "rs_surface_layer_parcel.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {
RSSurfaceRCDLayer::RSSurfaceRCDLayer(RSLayerId rsLayerId, std::shared_ptr<RSLayerContext> rsLayerContext) :
    RSSurfaceLayer(rsLayerId, rsLayerContext)
{
}

void RSSurfaceRCDLayer::AddRSLayerCmd(const std::shared_ptr<RSLayerCmd> layerCmd)
{
    if (layerCmd->GetRSLayerCmdType() == RSLayerCmdType::LAYER_CMD) {
        return;
    }
    std::shared_ptr<RSLayerParcel> layerParcel =
        std::make_shared<RSUpdateRSRCDLayerCmd>(GetRSLayerId(), layerCmd->CreateRenderLayerCmd());
    AddRSLayerParcel(layerParcel, GetRSLayerId());
}

void RSSurfaceRCDLayer::SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    pixelMap_ = pixelMap;
    SetRSLayerCmd<RSLayerPixelMapCmd, RSLayerCmdProperty<std::shared_ptr<Media::PixelMap>>>(
        RSLayerCmdType::PIXEL_MAP, pixelMap);
}

template<typename RSLayerCmdName, typename RSLayerPropertyName, typename T>
void RSSurfaceRCDLayer::SetRSLayerCmd(RSLayerCmdType rsLayerCmdType, T value)
{
    auto rsLayerProperty = std::make_shared<RSLayerPropertyName>(value);
    auto rsLayerCmd = std::make_shared<RSLayerCmdName>(rsLayerProperty);
    AddRSLayerCmd(rsLayerCmd);
}

std::shared_ptr<Media::PixelMap> RSSurfaceRCDLayer::GetPixelMap() const
{
    return pixelMap_;
}
} // namespace Rosen
} // namespace OHOS
