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

#include "feature/layer/rs_layer_cache_manager.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "params/rs_render_params.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {

// GPUMEMORY
constexpr float LAYER_CACHE_GPU_MEMORY_THRESHOLD = 200; // 200MB
constexpr float LAYER_CACHE_SIZE_THRESHOLD = 40; // 40MB
constexpr int PER_PIXEL_SIZE = 4; // 1 pixel = 4 bytes

// LAYERDFX
constexpr int32_t BORDER_WIDTH = 6;
constexpr int32_t MARGIN = 20;
constexpr float RECT_PEN_ALPHA = 0.2f;

RSLayerCacheManager& RSLayerCacheManager::Instance()
{
    static RSLayerCacheManager instance;
    return instance;
}

void RSLayerCacheManager::LayerCacheRegionDfx(
    std::shared_ptr<DrawableV2::RSRenderNodeDrawableAdapter> drawable, RSPaintFilterCanvas& canvas)
{
    if (!RSSystemProperties::GetLayerDebugEnabled()) {
        return;
    }
    if (std::find(layerDrawables_.begin(), layerDrawables_.end(), drawable) == layerDrawables_.end()) {
        return;
    }

    auto layerRect = drawable->GetRenderParams()->GetBounds();

    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(0x8090EE90)); // green
    brush.SetAntiAlias(true);
    brush.SetAlphaF(RECT_PEN_ALPHA);
    std::shared_ptr<Drawing::Typeface> typeFace = nullptr;
    std::string position = "pos:[" + layerRect.ToString() + "]";

    RS_OPTIONAL_TRACE_NAME_FMT("Drawable LayerCacheRegion, id: %" PRId64 ", %s", drawable->GetId(), position.c_str());
    // font size 24
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromString(position.c_str(), Drawing::Font(typeFace, 24.0f, 0.6f, 0.0f));
    canvas.AttachBrush(brush);
    canvas.DrawRect(layerRect);
    canvas.DetachBrush();
    canvas.AttachBrush(Drawing::Brush());
    canvas.DrawTextBlob(textBlob.get(), layerRect.GetLeft() + BORDER_WIDTH,
        layerRect.GetTop() + MARGIN);
    canvas.DetachBrush();
}

bool RSLayerCacheManager::ShouldEnableLayerCache(
    std::shared_ptr<DrawableV2::RSCanvasRenderNodeDrawable> drawable, float& gpuMemMB, float& totalLayerCacheMB)
{
    auto& params = drawable->GetRenderParams();

    // totalLayerCacheMB > LAYER_CACHE_SIZE_THRESHOLD is not mandatory. You can cancel it when adding a scenario.
    bool shouldDisisableLayerCache = (gpuMemMB + totalLayerCacheMB) > LAYER_CACHE_GPU_MEMORY_THRESHOLD ||
                                     totalLayerCacheMB > LAYER_CACHE_SIZE_THRESHOLD;
    if (shouldDisisableLayerCache) {
        params->SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        return false;
    }

    Drawing::Rect bounds = params->GetFrameRect();
    auto width = bounds.GetWidth();
    auto height = bounds.GetHeight();
    totalLayerCacheMB += static_cast<float>(width * height * PER_PIXEL_SIZE) / (1024.f * 1024.f);

    shouldDisisableLayerCache = (gpuMemMB + totalLayerCacheMB) > LAYER_CACHE_GPU_MEMORY_THRESHOLD ||
                                totalLayerCacheMB > LAYER_CACHE_SIZE_THRESHOLD;
    if (shouldDisisableLayerCache) {
        params->SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        return false;
    }
    return true;
}

void RSLayerCacheManager::TryPrepareLayerCache(
    std::shared_ptr<DrawableV2::RSCanvasRenderNodeDrawable> drawable, RSPaintFilterCanvas& canvas)
{
    if (drawable == nullptr) {
        return;
    }
    if (UNLIKELY(!drawable->isDrawingCacheEnabled_)) {
        return;
    }

    if (!(drawable->ShouldPaint() || (canvas.GetUICapture() && drawable->IsUiRangeCaptureEndNode()))) {
        drawable->SetDrawSkipType(DrawableV2::DrawSkipType::SHOULD_NOT_PAINT);
        return;
    }
    const auto& params = drawable->GetRenderParams();
    if (params == nullptr) {
        drawable->SetDrawSkipType(DrawableV2::DrawSkipType::RENDER_PARAMS_NULL);
        return;
    }
    Drawing::GPUResourceTag::SetCurrentNodeId(drawable->GetId());

    float hdrBrightness = canvas.GetHDRBrightness();
    canvas.SetHDRBrightness(params->GetHDRBrightness());

    DrawableV2::RSRenderNodeSingleDrawableLocker singleLocker(drawable.get());
    if (UNLIKELY(!singleLocker.IsLocked())) {
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
        if (RSSystemProperties::GetSingleDrawableLockerEnabled()) {
            drawable->SetDrawSkipType(DrawableV2::DrawSkipType::MULTI_ACCESS);
            return;
        }
    }
    if (params->GetHDRStatus() == HdrStatus::HDR_EFFECT) {
        canvas.ConvertToType(
            Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL, Drawing::ColorSpace::CreateSRGB());
    }
    RS_OPTIONAL_TRACE_NAME_FMT("TryPrepareLayerCache is GenerateCacheIfNeed, id: %" PRId64 "", drawable->GetId());
    drawable->GenerateCacheIfNeed(canvas, *params);
    canvas.SetHDRBrightness(hdrBrightness);
}

void RSLayerCacheManager::HandleLayerDrawables(RSPaintFilterCanvas& canvas)
{
    if (!canvas.GetGPUContext()) {
        layerDrawables_.clear();
        return;
    }

    float totalLayerCacheMB = 0;
    for (auto& drawableAdapter : layerDrawables_) {
        auto drawablePtr = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(drawableAdapter);
        if (!drawablePtr->GetRenderParams()) {
            continue;
        }

        RS_OPTIONAL_TRACE_NAME_FMT("LayerDrawable TryPrepareLayerCache, isOpinc:%d, id: %" PRId64 "",
            drawablePtr->GetRenderParams()->OpincIsSuggest(), drawablePtr->GetId());

        size_t gpuMemBytes = 0;
        canvas.GetGPUContext()->GetResourceCacheUsage(nullptr, &gpuMemBytes);
        float gpuMemMB = static_cast<float>(gpuMemBytes)/(1024.f * 1024.f);
        bool isLayerEnabled = (drawablePtr->GetRenderParams()->OpincIsSuggest() == false) &&
                              ShouldEnableLayerCache(drawablePtr, gpuMemMB, totalLayerCacheMB);
        if (isLayerEnabled) {
            TryPrepareLayerCache(drawablePtr, canvas);
        }
    }
    layerDrawables_.clear();
}
} // namespace Rosen
} // namespace OHOS