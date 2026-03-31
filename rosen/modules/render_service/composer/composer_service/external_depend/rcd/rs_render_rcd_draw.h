/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_EXTERNAL_DEPEND_RCD_RS_RENDER_RCD_DRAW_H
#define RENDER_SERVICE_COMPOSER_SERVICE_EXTERNAL_DEPEND_RCD_RS_RENDER_RCD_DRAW_H
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include "pipeline/rs_paint_filter_canvas.h"
#include "rs_render_surface_rcd_layer.h"

namespace OHOS {
namespace Rosen {
class RSRenderSurfaceRCDLayer;
class RSRenderRcdDraw {
public:
    using PixelMapPtr = std::shared_ptr<Media::PixelMap>;
    static void DrawRoundCorner(RSPaintFilterCanvas& canvas, const std::vector<RSLayerPtr>& layers);
    static void DrawRSRCDLayer(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSLayer>& layer);
    static bool BindPixelMapToDrawingImage(Drawing::Canvas& canvas,
        const std::shared_ptr<RSRenderSurfaceRCDLayer>& layer);
    static bool StoreWithoutDMA(Drawing::Canvas& canvas,
        const std::shared_ptr<RSRenderSurfaceRCDLayer>& layer, PixelMapPtr& pixelMap);
    static Drawing::ColorType PixelFormatToDrawingColorType(Media::PixelFormat pixelFormat);
    static Drawing::AlphaType AlphaTypeToDrawingAlphaType(Media::AlphaType alphaType);

    RSRenderRcdDraw() = default;
    virtual ~RSRenderRcdDraw() = default;
};
}
}
#endif // RENDER_SERVICE_COMPOSER_SERVICE_EXTERNAL_DEPEND_RCD_RS_RENDER_RCD_DRAW_H