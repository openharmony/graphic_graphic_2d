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

namespace OHOS {
namespace Rosen {
RSLayerCacheManager& RSLayerCacheManager::Instance()
{
    static RSLayerCacheManager instance;
    return instance;
}

void RSLayerCacheManager::TryPrepareLayerCache(
    std::shared_ptr<DrawableV2::RSCanvasRenderNodeDrawable> drawable, Drawing::Canvas& canvas)
{
    if (UNLIKELY(!drawable->isDrawingCacheEnabled_)) {
        return;
    }

    bool shouldPaint = drawable->ShouldPaint() || (canvas.GetUICapture() && drawable->IsUiRangeCaptureEndNode());
    if (!shouldPaint) {
        drawable->SetDrawSkipType(DrawableV2::DrawSkipType::SHOULD_NOT_PAINT);
        return;
    }
    const auto& params = drawable->GetRenderParams();
    if (params == nullptr) {
        drawable->SetDrawSkipType(DrawableV2::DrawSkipType::RENDER_PARAMS_NULL);
        return;
    }
    Drawing::GPUResourceTag::SetCurrentNodeId(drawable->GetId());
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);

    float hdrBrightness = paintFilterCanvas->GetHDRBrightness();
    paintFilterCanvas->SetHDRBrightness(params->GetHDRBrightness());

    DrawableV2::RSRenderNodeSingleDrawableLocker singleLocker(drawable.get());
    if (UNLIKELY(!singleLocker.IsLocked())) {
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
        HILOG_COMM_ERROR("RSCanvasRenderNodeDrawable::OnDraw node %{public}" PRIu64 " onDraw!!!", drawable->GetId());
        if (RSSystemProperties::GetSingleDrawableLockerEnabled()) {
            drawable->SetDrawSkipType(DrawableV2::DrawSkipType::MULTI_ACCESS);
            return;
        }
    }
    if (params->GetHDRStatus() == HdrStatus::HDR_EFFECT) {
        paintFilterCanvas->ConvertToType(
            Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL, Drawing::ColorSpace::CreateSRGB());
    }

    drawable->GenerateCacheIfNeed(canvas, *params);
    paintFilterCanvas->SetHDRBrightness(hdrBrightness);
}

void RSLayerCacheManager::HandleLayerDrawables(Drawing::Canvas& canvas)
{
    for (auto drawableAdapter : layerDrawables_) {
        auto drawablePtr = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(drawableAdapter);
        if (!drawablePtr->GetRenderParams()) {
            continue;
        }
        RS_TRACE_NAME_FMT("LayerDrawable TryPrepareLayerCache, isOpinc:%d, id: %" PRId64 "",
            drawablePtr->GetRenderParams()->OpincIsSuggest(), drawablePtr->GetId());
        if (drawablePtr->GetRenderParams()->OpincIsSuggest() == false) {
            TryPrepareLayerCache(drawablePtr, canvas);
        }
    }
    layerDrawables_.clear();
}
} // namespace Rosen
} // namespace OHOS