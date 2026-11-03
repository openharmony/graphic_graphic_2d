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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_ATLAS_INFO_H
#define RENDER_SERVICE_BASE_COMMON_RS_ATLAS_INFO_H

#include <cmath>
#include <cinttypes>

#include "common/rs_rect.h"

namespace OHOS {
namespace Media {
class PixelMap;
} // namespace Media
}

namespace OHOS::Rosen {

// AtlasInfo: atlas frame animation parameter struct
// Context fields (mode/rows/cols/frameWidth/frameHeight/padding/totalFrame/pixelMap)
// carry through from `this` in operators.
// Animated field (frameIndex) is the only field participating in arithmetic.
// Context fields are read by the Drawable and passed to GE shader as uniforms.
// Precondition: start and end values must share identical context fields.
struct AtlasInfo {
    // ===== Context fields (carry-through, not animated) =====
    int32_t mode = 0;
    int32_t rows = 0;
    int32_t cols = 0;
    float frameWidth = 0.0f;    // single frame width in pixels (e.g. 360.0)
    float frameHeight = 0.0f;   // single frame height in pixels (e.g. 360.0)
    float padding = 0.0f;       // padding between frames in pixels (e.g. 2.0)
    int32_t totalFrame = 0;
    std::shared_ptr<Media::PixelMap> pixelMap; // atlas image (required), passed once via IPC

    // ===== Animated field (only field participating in arithmetic) =====
    float frameIndex = 0.0f;

    // RSAnimatableArithmetic operators — satisfy RSAnimatableProperty compile-time constraint.
    // Actual interpolation is done by RSRenderAtlasInfoAnimation::OnAnimate,
    // NOT through the standard RSRenderCurveAnimation operator chain.
    // Operators are only used for change detection (operator==).
    AtlasInfo operator+(const AtlasInfo& other) const
    {
        return { mode, rows, cols, frameWidth, frameHeight, padding, totalFrame, pixelMap,
                 frameIndex + other.frameIndex };
    }

    AtlasInfo operator-(const AtlasInfo& other) const
    {
        return { mode, rows, cols, frameWidth, frameHeight, padding, totalFrame, pixelMap,
                 frameIndex - other.frameIndex };
    }

    AtlasInfo operator*(float f) const
    {
        return { mode, rows, cols, frameWidth, frameHeight, padding, totalFrame, pixelMap,
                 frameIndex * f };
    }

    bool operator==(const AtlasInfo& other) const
    {
        return mode == other.mode && rows == other.rows && cols == other.cols &&
               std::abs(frameWidth - other.frameWidth) < 0.001f &&
               std::abs(frameHeight - other.frameHeight) < 0.001f &&
               std::abs(padding - other.padding) < 0.001f &&
               totalFrame == other.totalFrame &&
               pixelMap == other.pixelMap &&
               std::abs(frameIndex - other.frameIndex) < 0.001f;
    }
};

} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_COMMON_RS_ATLAS_INFO_H
