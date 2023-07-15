/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ADAPTIVE_IMAGE_HELPER_H
#define ADAPTIVE_IMAGE_HELPER_H

#include "draw/canvas.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class ImageFit {
    FILL,
    CONTAIN,
    COVER,
    FIT_WIDTH,
    FIT_HEIGHT,
    NONE,
    SCALE_DOWN,
    TOP_LEFT,
};

enum class ImageRepeat {
    NO_REPEAT = 0,
    REPEAT_X,
    REPEAT_Y,
    REPEAT,
};

struct AdaptiveImageInfo {
    int32_t fitNum = 0;
    int32_t repeatNum = 0;
    Point radius[4];
    double scale = 0.0;
    uint32_t uniqueId = 0;
    int32_t width = 0;
    int32_t height = 0;
};

struct BoundaryRect {
    int32_t minX = 0;
    int32_t minY = 0;
    int32_t maxX = 0;
    int32_t maxY = 0;
};

class AdaptiveImageHelper {
public:
    AdaptiveImageHelper() = default;
    ~AdaptiveImageHelper() = default;

    static void DrawImage(Canvas& canvas, const Rect& rect, const std::shared_ptr<Image>& image,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling);
    static void DrawImage(Canvas& canvas, const Rect& rect, const std::shared_ptr<Data>& data,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling);
    static void DrawPixelMap(Canvas& canvas, const Rect& rect, const std::shared_ptr<Media::PixelMap>& pixelMap,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling);

private:
    static Rect GetDstRect(const AdaptiveImageInfo& rsImageInfo, const float srcRectWidth, const float srcRectHeight,
        const float frameRectWidth, const float frameRectHeight);
    static void ApplyCanvasClip(Canvas& canvas, const Rect& rect, const AdaptiveImageInfo& rsImageInfo,
        const float srcRectWidth, const float srcRectHeight);
    static void GetRectCropMultiple(
        const Rect& rect, const int32_t repeatNum, const Rect& dstRect, BoundaryRect& boundaryRect);
    static void DrawImageRepeatRect(Canvas& canvas, const Rect& rect, const std::shared_ptr<Image>& image,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling);
    static void DrawImageRepeatRect(Canvas& canvas, const Rect& rect, const std::shared_ptr<Data>& data,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling);
    static void DrawPixelMapRepeatRect(Canvas& canvas, const Rect& rect,
        const std::shared_ptr<Media::PixelMap>& pixelMap, const AdaptiveImageInfo& rsImageInfo,
        const SamplingOptions& smapling);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
