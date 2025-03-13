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

#include "drawing_canvas_utils.h"

#include <memory>

#include "pipeline/rs_recording_canvas.h"
#include "pixel_map.h"
#include "render/rs_pixel_map_util.h"

using namespace OHOS;
using namespace Rosen;

OH_Drawing_ErrorCode DrawingCanvasUtils::DrawPixelMapNine(Drawing::Canvas* canvas,
    std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* center, const Drawing::Rect* dst,
    const Drawing::FilterMode mode)
{
#ifdef OHOS_PLATFORM
    if (!canvas || !pixelMap || !dst) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Drawing::RectI centerRectI;
    if (center) {
        centerRectI = Drawing::RectI(Drawing::DrawingFloatSaturate2Int(center->GetLeft()),
            Drawing::DrawingFloatSaturate2Int(center->GetTop()),
            Drawing::DrawingFloatSaturate2Int(center->GetRight()),
            Drawing::DrawingFloatSaturate2Int(center->GetBottom()));
    } else {
        centerRectI = Drawing::RectI(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight());
    }
    if (canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        canvas_->DrawPixelMapNine(pixelMap, centerRectI, *dst, mode);
        return OH_DRAWING_SUCCESS;
    }
    std::shared_ptr<Drawing::Image> image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    if (!image) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    canvas->DrawImageNine(image.get(), centerRectI, *dst, mode);
#endif
    return OH_DRAWING_SUCCESS;
}

void DrawingCanvasUtils::DrawPixelMapRect(Drawing::Canvas* canvas, std::shared_ptr<Media::PixelMap> pixelMap,
    const Drawing::Rect* src, const Drawing::Rect* dst, const Drawing::SamplingOptions* sampling)
{
#ifdef OHOS_PLATFORM
    if (!canvas || !pixelMap || !dst) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        canvas_->DrawPixelMapRect(pixelMap,
            src ? *src : Drawing::Rect(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight()),
            *dst, sampling ? *sampling : Drawing::SamplingOptions());
        return;
    }
    std::shared_ptr<Drawing::Image> image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    if (!image) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawImageRect(*image, src ? *src : Drawing::Rect(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight()),
        *dst, sampling ? *sampling : Drawing::SamplingOptions());
#endif
}