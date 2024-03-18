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

void DrawingCanvasUtils::DrawPixelMapRect(Drawing::Canvas* canvas, std::shared_ptr<Media::PixelMap> pixelMap,
    const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling)
{
#ifdef OHOS_PLATFORM
    if (canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        canvas_->DrawPixelMapRect(pixelMap, src, dst, sampling);
        return;
    }
    std::shared_ptr<Drawing::Image> image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    canvas->DrawImageRect(*image, src, dst, sampling);
#endif
}