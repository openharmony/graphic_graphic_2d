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

namespace OHOS {
namespace Rosen {
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
    static void DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect = nullptr);
    static int GetAndResetBlurCnt();
    static void GetOutlineDirtyRect(RectI& dirtyOutline,
        const RSProperties& properties, const bool isAbsCoordinate = true);
    static bool PickColor(const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& drPath,
    Drawing::Matrix& matrix, Drawing::RectI& deviceClipBounds, RSColor& colorPicked);
    static void GetDarkColor(RSColor& color);
    // todo: doublecheck if calculate correct
    static void GetPixelStretchDirtyRect(RectI& dirtyPixelStretch,
        const RSProperties& properties, const bool isAbsCoordinate = true);
    static void DrawPixelStretch(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawForegroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void ApplyBackgroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    // Foreground Color filter
    static void DrawColorFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    static void DrawBinarizationShader(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawLightUpEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawDynamicLightUp(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawDynamicDim(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawParticle(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    // BlendMode
    static bool IsDangerousBlendMode(int blendMode, int blendApplyType);

    static void Clip(Drawing::Canvas& canvas, RectF rect, bool isAntiAlias = true);
    static void DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawOutline(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawLight(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawFrame(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::DrawCmdListPtr& drawCmdList);
    static void DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas, FilterType filterType,
        const std::optional<Drawing::Rect>& rect = std::nullopt,
        const std::shared_ptr<RSFilter>& externalFilter = nullptr);
    static void DrawForegroundFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawForegroundColor(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawMask(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawMask(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Rect maskBounds);
    static bool GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, Drawing::Matrix& mat);
    static Drawing::RoundRect RRect2DrawingRRect(const RRect& rr);
    static Drawing::Rect Rect2DrawingRect(const RectF& r);
    static Drawing::ColorQuad CalcAverageColor(std::shared_ptr<Drawing::Image> imageSnapshot);
    static void DrawSpherize(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        const std::shared_ptr<Drawing::Surface>& spherizeSurface);
    static std::shared_ptr<Drawing::Blender> MakeDynamicLightUpBlender(
        float dynamicLightUpRate, float dynamicLightUpDeg);
    static std::shared_ptr<Drawing::Image> DrawGreyAdjustment(
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Vector2f& greyCoef);

    // EffectView and useEffect
    static void DrawBackgroundImageAsEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawBackgroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    static const bool BLUR_ENABLED;

private:
    static void ApplyBackgroundEffectFallback(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static RRect GetRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool isOutline);
    static RRect GetInnerRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool isOutline);
    static void ClipVisibleCanvas(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawColorfulShadowInner(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
    static void DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
    static void DrawLightInner(const RSProperties& properties, Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        const std::vector<std::pair<std::shared_ptr<RSLightSource>, Vector4f>>& lightSourcesAndPosMap,
        const std::shared_ptr<RSObjAbsGeometry>& geoPtr);
    static void DrawContentLight(const RSProperties& properties, Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush,
        const float lightIntensityArray[]);
    static void DrawBorderLight(const RSProperties& properties, Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen,
        const float lightIntensityArray[]);
    static std::shared_ptr<Drawing::ShaderEffect> MakeLightUpEffectShader(
        float lightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeDynamicDimShader(
        float dynamicDimDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeBinarizationShader(float low, float high,
        float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::RuntimeEffect> MakeGreyAdjustmentEffect();

    static void DrawBorderBase(const RSProperties& properties, Drawing::Canvas& canvas,
        const std::shared_ptr<RSBorder>& border, const bool isOutline);
    static const std::shared_ptr<Drawing::RuntimeShaderBuilder>& GetPhongShaderBuilder();

    static std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> binarizationShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> lightUpEffectShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicLightUpBlenderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicDimShaderEffect_;
    inline static int g_blurCnt = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
