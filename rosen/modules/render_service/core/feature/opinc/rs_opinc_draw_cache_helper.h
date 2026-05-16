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

#ifndef RS_OPINC_DRAW_CACHE_HELPER_H
#define RS_OPINC_DRAW_CACHE_HELPER_H

#include <string>
#include <utility>
#include <vector>

#include "common/rs_common_def.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSRenderNodeDrawable;
class RSOpincDrawCache;
} // namespace DrawableV2
class RSRenderParams;
class RSPaintFilterCanvas;

class RSOpincDrawCacheHelper {
public:
    static bool PrepareOpincSubtree(
        DrawableV2::RSRenderNodeDrawable& drawable, RSRenderParams& params);

    static void FinishOpincSubtree(RSRenderParams& params);

    static void ProcessBeforeDrawCache(
        DrawableV2::RSRenderNodeDrawable& drawable, Drawing::Canvas& canvas, RSRenderParams& params);

    static void ProcessAfterDrawCache(
        DrawableV2::RSRenderNodeDrawable& drawable, Drawing::Canvas& canvas, RSRenderParams& params);

    static bool GetOpincCachedMark(DrawableV2::RSRenderNodeDrawable& drawable);

    static Drawing::Rect GetOpincUnionArea(DrawableV2::RSRenderNodeDrawable& drawable);

    static void AddOpincCacheMem(DrawableV2::RSRenderNodeDrawable& drawable, int64_t cacheMem);

    static void ReduceOpincCacheMem(DrawableV2::RSRenderNodeDrawable& drawable, int64_t cacheMem);

    static bool TryDrawOpincAutoCache(DrawableV2::RSRenderNodeDrawable& drawable,
        RSPaintFilterCanvas& canvas, Drawing::Image& cacheImage,
        const Drawing::SamplingOptions& samplingOptions,
        std::vector<std::pair<RectI, std::string>>& autoCacheRenderNodeInfos);

    static void OpincCanvasUnionTranslate(DrawableV2::RSRenderNodeDrawable& drawable, RSPaintFilterCanvas& canvas);

    static void ResumeOpincCanvasTranslate(DrawableV2::RSRenderNodeDrawable& drawable, RSPaintFilterCanvas& canvas);

private:
    static thread_local bool inOpincSuggestSubtree_;
};
} // namespace OHOS::Rosen

#endif // RS_OPINC_DRAW_CACHE_HELPER_H