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

#ifndef RS_LAYER_PART_DRAW_CACHE_HELPER_H
#define RS_LAYER_PART_DRAW_CACHE_HELPER_H

#include "feature/opinc/rs_layer_part_draw_cache.h"

namespace OHOS::Rosen {
class RSRenderParams;
class RSPaintFilterCanvas;

namespace DrawableV2 {
class RSRenderNodeDrawable;

class RSLayerPartDrawCacheHelper {
public:
    static void PushLayerPartRenderDirtyRegion(RSRenderNodeDrawable& drawable, const RSRenderParams& params,
        RSPaintFilterCanvas& curCanvas, RSPaintFilterCanvas& cacheCanvas, int nodeCount);
    static void LayerPartRenderClipDirtyRegion(RSRenderNodeDrawable& drawable,
        const RSRenderParams& params, RSPaintFilterCanvas& canvas);
    static void PopLayerPartRenderDirtyRegion(RSRenderNodeDrawable& drawable,
        const RSRenderParams& params, RSPaintFilterCanvas& canvas);
    static void ResetUpdateLayerPartRenderCache(RSRenderNodeDrawable& drawable);
};
} // namespace DrawableV2
} // namespace OHOS::Rosen

#endif // RS_LAYER_PART_DRAW_CACHE_HELPER_H
