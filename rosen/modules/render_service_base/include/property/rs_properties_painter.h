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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H

#include "common/rs_macros.h"
#include "property/rs_properties.h"
#include "recording/draw_cmd_list.h"
#include "draw/surface.h"
#include "effect/runtime_shader_builder.h"
#include "effect/runtime_blender_builder.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int MAX_LIGHT_SOURCES = 12;
}
class RSPaintFilterCanvas;
class RSDrawingFilter;
class RSDrawingFilterOriginal;
enum class FilterType {
    BACKGROUND_FILTER,
    FOREGROUND_FILTER,
};
class RSB_EXPORT RSPropertiesPainter {
public:
    static void SetBgAntiAlias(bool forceBgAntiAlias);
    static bool GetBgAntiAlias();
    static void DrawBackground(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        bool isAntiAlias = true, bool isSurfaceView = false);
    static void GetShadowDirtyRect(RectI& dirtyShadow, const RSProperties& properties,
        const RRect* rrect = nullptr, bool isAbsCoordinate = true, bool radiusInclude = true);
    static void GetForegroundEffectDirtyRect(RectI& dirtyForegroundEffect,
        const RSProperties& properties, const bool isAbsCoordinate = true);
    static void GetDistortionEffectDirtyRect(RectI& dirtyDistortionEffect, const RSProperties& properties);
    static void DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect = nullptr);
    static int GetAndResetBlurCnt();
    static void GetOutlineDirtyRect(RectI& dirtyOutline,
        const RSProperties& properties, const bool isAbsCoordinate = true);
    // todo: doublecheck if calculate correct
    static void GetPixelStretchDirtyRect(RectI& dirtyPixelStretch,
        const RSProperties& properties, const bool isAbsCoordinate = true);

    // BlendMode
    static bool IsDangerousBlendMode(int blendMode, int blendApplyType);

    static void Clip(Drawing::Canvas& canvas, RectF rect, bool isAntiAlias = true);
    static void DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawOutline(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawFrame(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::DrawCmdListPtr& drawCmdList);
    static void DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas, FilterType filterType,
        const std::optional<Drawing::Rect>& rect = std::nullopt,
        const std::shared_ptr<RSFilter>& externalFilter = nullptr);
    static void DrawMask(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawMask(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Rect maskBounds);
    static bool GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, Drawing::Matrix& mat);
    static bool GetScalingModeMatrix(ScalingMode scalingMode, RectF bounds,
        float bufferWidth, float bufferHeight, Drawing::Matrix& scalingModeMatrix);
    static Drawing::RoundRect RRect2DrawingRRect(const RRect& rr);
    static Drawing::Rect Rect2DrawingRect(const RectF& r);
    static Drawing::ColorQuad CalcAverageColor(std::shared_ptr<Drawing::Image> imageSnapshot);
    static std::shared_ptr<Drawing::Image> DrawGreyAdjustment(
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Vector2f& greyCoef);

    // EffectView and useEffect
    static void DrawBackgroundImageAsEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawBackgroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    static const bool BLUR_ENABLED;

private:
    static RRect GetRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool isOutline);
    static RRect GetInnerRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool isOutline);

    static void DrawColorfulShadowInner(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
    static void DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
    static std::shared_ptr<Drawing::RuntimeEffect> MakeGreyAdjustmentEffect();

    static void DrawBorderBase(const RSProperties& properties, Drawing::Canvas& canvas,
        const std::shared_ptr<RSBorder>& border, const bool isOutline);

    static std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect_;
    inline static int g_blurCnt = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
