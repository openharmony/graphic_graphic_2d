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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H

#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
class RSPropertyDrawableUtils {
public:
    static Drawing::RoundRect RRect2DrawingRRect(const RRect& rr);
    static Drawing::Rect Rect2DrawingRect(const RectF& r);
    static RRect GetRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool& isOutline);
    static RRect GetInnerRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool& isOutline);
    static bool PickColor(Drawing::Canvas& canvas, const std::shared_ptr<RSColorPickerCacheTask>& colorPickerTask,
        Drawing::Path& drPath, Drawing::Matrix& matrix, RSColor& colorPicked);
    static void GetDarkColor(RSColor& color);
    static void DrawFilter(Drawing::Canvas* canvas, const std::shared_ptr<RSFilter>& rsFilter,
        const bool isForegroundFilter);
    static void DrawColorFilter(Drawing::Canvas* canvas, const std::shared_ptr<Drawing::ColorFilter>& colorFilter);
    static void DrawLightUpEffect(Drawing::Canvas* canvas, const float lightUpEffectDegree);
    static std::shared_ptr<Drawing::ShaderEffect> MakeBinarizationShader(float low, float high,
        float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static void DrawBinarization(Drawing::Canvas* canvas, const std::optional<Vector4f>& aiInvert);
    static void DrawPixelStretch(Drawing::Canvas* canvas, const std::optional<Vector4f>& pixelStretch,
        const RectF& boundsRect, const bool boundsGeoValid);
    static Drawing::Path CreateShadowPath(Drawing::Canvas& canvas, bool shadowIsFilled,
        const std::shared_ptr<RSPath> shadowPath, const std::shared_ptr<RSPath>& clipBounds, const RRect& rrect);

private:
    static std::shared_ptr<Drawing::RuntimeEffect> binarizationShaderEffect_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H
