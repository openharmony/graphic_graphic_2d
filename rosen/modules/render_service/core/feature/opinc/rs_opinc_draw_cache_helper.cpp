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

#include "rs_opinc_draw_cache_helper.h"

#include "drawable/rs_render_node_drawable.h"
#include "feature/opinc/rs_opinc_draw_cache.h"
#include "params/rs_render_params.h"

namespace OHOS::Rosen {

thread_local bool RSOpincDrawCacheHelper::inOpincSuggestSubtree_ = false;

bool RSOpincDrawCacheHelper::PrepareOpincSubtree(
    DrawableV2::RSRenderNodeDrawable& drawable, RSRenderParams& params)
{
    // Root node marks the subtree scope.
    if (params.OpincIsSuggest()) {
        inOpincSuggestSubtree_ = true;
    }

    // If not in the opinc subtree, continue directly.
    if (!inOpincSuggestSubtree_) {
        return true;
    }

    // In the opinc subtree, get or lazily create opincDrawCache_.
    return drawable.GetOpincDrawCache().PreDrawableCacheState(params);
}

void RSOpincDrawCacheHelper::FinishOpincSubtree(RSRenderParams& params)
{
    // Root node restores the subtree scope flag.
    if (params.OpincIsSuggest()) {
        inOpincSuggestSubtree_ = false;
    }
}

void RSOpincDrawCacheHelper::ProcessBeforeDrawCache(
    DrawableV2::RSRenderNodeDrawable& drawable, Drawing::Canvas& canvas, RSRenderParams& params)
{
    auto* opincDrawCache = drawable.TryGetOpincDrawCachePtr();
    if (opincDrawCache != nullptr) {
        opincDrawCache->BeforeDrawCache(canvas, params);
    }
}

void RSOpincDrawCacheHelper::ProcessAfterDrawCache(
    DrawableV2::RSRenderNodeDrawable& drawable, Drawing::Canvas& canvas, RSRenderParams& params)
{
    auto* opincDrawCache = drawable.TryGetOpincDrawCachePtr();
    if (opincDrawCache != nullptr) {
        opincDrawCache->AfterDrawCache(canvas, params);
        opincDrawCache->DrawOpincDisabledDfx(canvas, params);
    }
}

bool RSOpincDrawCacheHelper::GetOpincCachedMark(DrawableV2::RSRenderNodeDrawable& drawable)
{
    auto* opincDrawCache = drawable.TryGetOpincDrawCachePtr();
    return opincDrawCache != nullptr ? opincDrawCache->OpincGetCachedMark() : false;
}

Drawing::Rect RSOpincDrawCacheHelper::GetOpincUnionArea(DrawableV2::RSRenderNodeDrawable& drawable)
{
    auto* opincDrawCache = drawable.TryGetOpincDrawCachePtr();
    if (opincDrawCache != nullptr && opincDrawCache->IsComputeDrawAreaSucc()) {
        return opincDrawCache->GetOpListUnionArea();
    }
    return Drawing::Rect();
}

void RSOpincDrawCacheHelper::AddOpincCacheMem(
    DrawableV2::RSRenderNodeDrawable& drawable, int64_t cacheMem)
{
    auto* opincDrawCache = drawable.TryGetOpincDrawCachePtr();
    if (opincDrawCache != nullptr) {
        opincDrawCache->AddOpincCacheMem(cacheMem);
    }
}

void RSOpincDrawCacheHelper::ReduceOpincCacheMem(
    DrawableV2::RSRenderNodeDrawable& drawable, int64_t cacheMem)
{
    auto* opincDrawCache = drawable.TryGetOpincDrawCachePtr();
    if (opincDrawCache != nullptr) {
        opincDrawCache->ReduceOpincCacheMem(cacheMem);
    }
}

bool RSOpincDrawCacheHelper::TryDrawOpincAutoCache(
    DrawableV2::RSRenderNodeDrawable& drawable, RSPaintFilterCanvas& canvas,
    Drawing::Image& cacheImage, const Drawing::SamplingOptions& samplingOptions,
    std::vector<std::pair<RectI, std::string>>& autoCacheRenderNodeInfos)
{
    auto* opincDrawCache = drawable.TryGetOpincDrawCachePtr();
    if (opincDrawCache != nullptr && opincDrawCache->IsComputeDrawAreaSucc() &&
        opincDrawCache->DrawAutoCache(canvas, cacheImage, samplingOptions,
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT)) {
        opincDrawCache->DrawAutoCacheDfx(canvas, autoCacheRenderNodeInfos);
        return true;
    }
    return false;
}

void RSOpincDrawCacheHelper::OpincCanvasUnionTranslate(
    DrawableV2::RSRenderNodeDrawable& drawable, RSPaintFilterCanvas& canvas)
{
    auto* opincDrawCache = drawable.TryGetOpincDrawCachePtr();
    if (opincDrawCache != nullptr) {
        opincDrawCache->OpincCanvasUnionTranslate(canvas);
    }
}

void RSOpincDrawCacheHelper::ResumeOpincCanvasTranslate(
    DrawableV2::RSRenderNodeDrawable& drawable, RSPaintFilterCanvas& canvas)
{
    auto* opincDrawCache = drawable.TryGetOpincDrawCachePtr();
    if (opincDrawCache != nullptr) {
        opincDrawCache->ResumeOpincCanvasTranslate(canvas);
    }
}

} // namespace OHOS::Rosen