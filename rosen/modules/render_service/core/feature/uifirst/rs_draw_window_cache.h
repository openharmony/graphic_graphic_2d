/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_DRAW_WINDOW_CACHE_H
#define RS_DRAW_WINDOW_CACHE_H

#include "image/image.h"
#include "params/rs_render_thread_params.h"
#include "params/rs_surface_render_params.h"

namespace OHOS {
namespace Rosen {
namespace DrawableV2 {
    class RSSurfaceRenderNodeDrawable;
}

/**
 * @brief Provides ability to draw and cache window content and children
*/
class RSDrawWindowCache {
public:
    RSDrawWindowCache() = default;
    ~RSDrawWindowCache();

    /**
     * @brief Draw and cache window content/children by offscreen buffer and draw back to target canvas
     *
     * @param surfaceDrawable Indicates the window to draw
     * @param canvas Indicates the target canvas to draw the window onto
     * @param bounds Indicates the bounds of the window rect
    */
    void DrawAndCacheWindowContent(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
        RSPaintFilterCanvas& canvas, const Drawing::Rect& bounds);

    /**
     * @brief Draw the cached window onto target canvas
     *
     * @param surfaceDrawable Indicates the window to draw
     * @param canvas Indicates the target canvas to draw the window onto
     * @param surfaceParams Indicates the render params
     * @return true if success, otherwise false
    */

    void ClearCache();

#ifdef RS_ENABLE_GPU
    bool DealWithCachedWindow(DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable,
        RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams, RSRenderThreadParams& uniParam);
#endif

    void DrawCrossNodeOffscreenDFX(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams,
        RSRenderThreadParams& uniParams, const Drawing::Color& color);
private:
    bool HasCache() const;
    std::shared_ptr<Drawing::Image> image_ = nullptr;
};
} // Rosen
} // OHOS

#endif // RS_DRAW_WINDOW_CACHE_H