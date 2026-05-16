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

#include "feature/opinc/rs_layer_part_draw_cache_helper.h"

#include "drawable/rs_render_node_drawable.h"
#include "params/rs_render_params.h"

namespace OHOS::Rosen::DrawableV2 {
void RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(RSRenderNodeDrawable& drawable,
    const RSRenderParams& params, RSPaintFilterCanvas& curCanvas, RSPaintFilterCanvas& cacheCanvas, int nodeCount)
{
    if (!params.GetLayerPartRenderEnabled()) {
        return;
    }
    auto* layerPartDrawCache = drawable.TryGetLayerPartDrawCachePtr();
    if (layerPartDrawCache == nullptr) {
        return;
    }
    layerPartDrawCache->PushLayerPartRenderDirtyRegion(params, curCanvas, cacheCanvas, nodeCount);
}

void RSLayerPartDrawCacheHelper::LayerPartRenderClipDirtyRegion(RSRenderNodeDrawable& drawable,
    const RSRenderParams& params, RSPaintFilterCanvas& canvas)
{
    if (!params.GetLayerPartRenderEnabled()) {
        return;
    }
    auto* layerPartDrawCache = drawable.TryGetLayerPartDrawCachePtr();
    if (layerPartDrawCache == nullptr) {
        return;
    }
    layerPartDrawCache->LayerPartRenderClipDirtyRegion(canvas);
}

void RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(RSRenderNodeDrawable& drawable,
    const RSRenderParams& params, RSPaintFilterCanvas& canvas)
{
    if (!params.GetLayerPartRenderEnabled() || canvas.IsLayerPartRenderDirtyRegionStackEmpty()) {
        return;
    }
    auto* layerPartDrawCache = drawable.TryGetLayerPartDrawCachePtr();
    if (layerPartDrawCache == nullptr) {
        return;
    }
    layerPartDrawCache->PopLayerPartRenderDirtyRegion(canvas);
}

void RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(RSRenderNodeDrawable& drawable)
{
    drawable.GetLayerPartDrawCache().ResetUnchangeCount();
}
} // namespace OHOS::Rosen::DrawableV2
