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

#include "rs_multi_display_post_processing_utils.h"

#include "platform/common/rs_log.h"
#include "common/rs_optional_trace.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "rs_layer_transaction_data.h"
#include "rs_multi_display_feature_param.h"
#include "rs_multi_display_interpolation_filter.h"

namespace OHOS::Rosen {
std::optional<InterpolationParams> RSMultiDisplayPostProcessingUtils::GetActiveInterpolationParams(
    ScreenId screenId, uint32_t width, uint32_t height)
{
    if (!RSMultiDisplayFeatureParam::IsInterpolationFeatureEnable()) {
        return std::nullopt;
    }
    auto interpolationParam = RSMultiDisplayFeatureParam::GetInterpolationParams(screenId);
    if (!interpolationParam.has_value()) {
        return std::nullopt;
    }
    if (width != interpolationParam->realWidth || height != interpolationParam->realHeight) {
        RS_LOGW("%{public}s: screenId:%{public}" PRIu64 " resolution mismatch, render:[%{public}u x %{public}u]"
            ", config real:[%{public}u x %{public}u], interpolation inactive",
            __func__, screenId, width, height, interpolationParam->realWidth, interpolationParam->realHeight);
        return std::nullopt;
    }
    return interpolationParam;
}

bool RSMultiDisplayPostProcessingUtils::AdjustBufferConfigForInterpolation(uint32_t phyWidth,
    uint32_t phyHeight, BufferRequestConfig& bufferConfig)
{
    auto physicalWidth = static_cast<int32_t>(phyWidth);
    auto physicalHeight = static_cast<int32_t>(phyHeight);
    if (physicalWidth <= 0 || physicalHeight <= 0) {
        RS_LOGW("%{public}s: invalid physical size [%{public}d x %{public}d]"
                ", keep render-size buffer", __func__, physicalWidth, physicalHeight);
        return false;
    }
    bufferConfig.width = physicalWidth;
    bufferConfig.height = physicalHeight;
    return true;
}

bool RSMultiDisplayPostProcessingUtils::CheckFilterParamsIsValid(
    const RSMultiDisplayInterpolationFilterParams& filterParams, const ComposerScreenInfo& composerScreenInfo)
{
    if (ROSEN_LE(filterParams.paramA, 0.0f) || ROSEN_LE(filterParams.paramN, 0.0f)) {
        return false;
    }
    if (ROSEN_NE(filterParams.realWidth, static_cast<float>(composerScreenInfo.width)) ||
        ROSEN_NE(filterParams.realHeight, static_cast<float>(composerScreenInfo.height))) {
        return false;
    }
    if (ROSEN_NE(filterParams.targetWidth, filterParams.realWidth) ||
        ROSEN_NE(filterParams.targetHeight, filterParams.realHeight * filterParams.paramN / filterParams.paramA)) {
        return false;
    }
    return true;
}

bool RSMultiDisplayPostProcessingUtils::Prepare(RSPaintFilterCanvas& canvas,
    RSMultiDisplayInterpolationFilterParams& filterParams, const ComposerScreenInfo& composerScreenInfo)
{
    if (!filterParams.filterOn) {
        return false;
    }
    filterParams.targetWidth = composerScreenInfo.phyWidth;
    filterParams.targetHeight = composerScreenInfo.phyHeight;
    if (!CheckFilterParamsIsValid(filterParams, composerScreenInfo)) {
        RS_LOGE("%{public}s: check filter params failed", __func__);
        return false;
    }
    RS_OPTIONAL_TRACE_NAME("RSMultiDisplayPostProcessingUtils::Prepare");
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        RS_LOGE("%{public}s: canvas surface is null", __func__);
        return false;
    }
    int32_t width = static_cast<int32_t>(composerScreenInfo.width);
    int32_t height = static_cast<int32_t>(composerScreenInfo.height);
    std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(width, height);
    if (offscreenSurface == nullptr) {
        RS_LOGE("%{public}s: make offscreen surface failed, [%{public}d x %{public}d]", __func__, width, height);
        return false;
    }

    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface.get());
    offscreenCanvas->CopyConfigurationToOffscreenCanvas(canvas);
    canvas.SaveEnv();
    canvas.ReplaceMainScreenData(offscreenSurface, offscreenCanvas);
    canvas.SavePCanvasList();
    canvas.RemoveAll();
    canvas.AddCanvas(offscreenCanvas.get());

    return true;
}

void RSMultiDisplayPostProcessingUtils::Process(RSPaintFilterCanvas& canvas,
    RSMultiDisplayInterpolationFilterParams& filterParams, const bool needPostProcessing)
{
    if (!needPostProcessing) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME("RSMultiDisplayPostProcessingUtils::Process");
    auto surface = canvas.GetSurface();
    std::shared_ptr<Drawing::Image> imageSnapshot = nullptr;
    if (surface) {
        imageSnapshot = surface->GetImageSnapshot();
    } else {
        RS_LOGD("%{public}s: canvas surface is null", __func__);
    }

    canvas.RestorePCanvasList();
    canvas.SwapBackMainScreenData();
    canvas.RestoreEnv();
    canvas.ResetClip();
    canvas.ResetMatrix();

    if (imageSnapshot == nullptr || imageSnapshot->GetWidth() <= 0 || imageSnapshot->GetHeight() <= 0) {
        RS_LOGE("%{public}s: invalid image snapshot", __func__);
        return;
    }
    auto filter = std::make_shared<RSMultiDisplayInterpolationFilter>(
        filterParams.paramA, filterParams.paramN, filterParams.realWidth, filterParams.realHeight);
    auto dstRect = Drawing::Rect(0.0f, 0.0f, filterParams.targetWidth, filterParams.targetHeight);
    filter->DrawInterpolation(canvas, imageSnapshot, dstRect);
}
} // namespace OHOS::Rosen
