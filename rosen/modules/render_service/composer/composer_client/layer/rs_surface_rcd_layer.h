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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_LAYER_RS_SURFACE_RCD_LAYER_H
#define RENDER_SERVICE_COMPOSER_CLIENT_LAYER_RS_SURFACE_RCD_LAYER_H

#include <string>
#include "surface.h"
#include "sync_fence.h"
#include "hdi_display_type.h"
#include "common/rs_anco_type.h"
#include "iconsumer_surface.h"
#include "pixel_map.h"
#include "rs_surface_layer.h"
#include "rs_layer_cmd.h"
#include "rs_render_layer_cmd_property.h"

namespace OHOS {
namespace Rosen {

class RSSurfaceRCDLayer : public RSSurfaceLayer {
public:
    RSSurfaceRCDLayer(RSLayerId rsLayerId = 0, std::shared_ptr<RSLayerContext> rsLayerContext = nullptr);
    virtual ~RSSurfaceRCDLayer() = default;
    virtual bool IsScreenRCDLayer() const override { return true; };
    void SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap);
    std::shared_ptr<Media::PixelMap> GetPixelMap() const;
private:
    template<typename RSLayerCmdName, typename RSLayerPropertyName, typename T>
    void SetRSLayerCmd(RSLayerCmdType rsLayerCmdType, T value);
    void AddRSLayerCmd(const std::shared_ptr<RSLayerCmd> layerCmd) override;
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_CLIENT_LAYER_RS_SURFACE_RCD_LAYER_H
