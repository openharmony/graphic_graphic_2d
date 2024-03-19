/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RECORDING_CANVAS_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RECORDING_CANVAS_H

#include "recording/recording_canvas.h"
#include "pipeline/rs_draw_cmd.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RSB_EXPORT ExtendRecordingCanvas : public Drawing::RecordingCanvas {
public:
    ExtendRecordingCanvas(int width, int weight, bool addDrawOpImmediate = true);
    ~ExtendRecordingCanvas() override = default;
    void DrawImageWithParm(const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Data>& data,
        const Drawing::AdaptiveImageInfo& rsImageInfo, const Drawing::SamplingOptions& sampling);
    void DrawPixelMapWithParm(const std::shared_ptr<Media::PixelMap>& pixelMap,
        const Drawing::AdaptiveImageInfo& rsImageInfo, const Drawing::SamplingOptions& sampling);
    void DrawImageNineWithPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap, const Drawing::RectI& center,
        const Drawing::Rect& dst, Drawing::FilterMode filter, const Drawing::Brush* brush);
    void DrawPixelMapRect(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& src,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling,
        Drawing::SrcRectConstraint constraint = Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    void DrawDrawFunc(Drawing::RecordingCanvas::DrawFunc&& drawFunc);
#ifdef ROSEN_OHOS
    void DrawSurfaceBuffer(const DrawingSurfaceBufferInfo& surfaceBufferInfo);
#endif
private:
    template<typename T, typename... Args>
    void AddDrawOpImmediate(Args&&... args);
    template<typename T, typename... Args>
    void AddDrawOpDeferred(Args&&... args);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RECORDING_CANVAS_H
