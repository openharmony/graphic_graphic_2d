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
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_draw_cmd_list.h"
#include "include/effects/SkRuntimeEffect.h"
#else
#include "recording/draw_cmd_list.h"
#include "draw/surface.h"
#include "effect/runtime_shader_builder.h"
#endif

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
#ifndef USE_ROSEN_DRAWING
class RSSkiaFilter;
#else
class RSDrawingFilter;
#endif
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
        const RSProperties& properties, const bool& isAbsCoordinate = true);
#ifndef USE_ROSEN_DRAWING
    static bool PickColor(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& skPath,
        SkMatrix& matrix, SkIRect& deviceClipBounds, RSColor& colorPicked);
#else
    static bool PickColor(const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& drPath,
    Drawing::Matrix& matrix, Drawing::RectI& deviceClipBounds, RSColor& colorPicked);
#endif
    static void GetDarkColor(RSColor& color);

    static void DrawPixelStretch(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawForegroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void ApplyBackgroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    // Foreground Color filter
    static void DrawColorFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    static void DrawBinarizationShader(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawLightUpEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawDynamicLightUp(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawParticle(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    static void BeginBlendMode(RSPaintFilterCanvas& canvas, const RSProperties& properties);
    static void EndBlendMode(RSPaintFilterCanvas& canvas, const RSProperties& properties);

#ifndef USE_ROSEN_DRAWING
    static void Clip(SkCanvas& canvas, RectF rect, bool isAntiAlias = true);
    static void DrawBorder(const RSProperties& properties, SkCanvas& canvas);
    static void DrawOutline(const RSProperties& properties, SkCanvas& canvas);
    static void DrawLight(const RSProperties& properties, SkCanvas& canvas);
    static void DrawFrame(const RSProperties& properties, RSPaintFilterCanvas& canvas, DrawCmdListPtr& drawCmdList);
    static void DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas, FilterType filterType,
        const std::optional<SkRect>& rect = std::nullopt, const std::shared_ptr<RSFilter>& externalFilter = nullptr);
    static void DrawForegroundColor(const RSProperties& properties, SkCanvas& canvas);
    static void DrawMask(const RSProperties& properties, SkCanvas& canvas);
    static void DrawMask(const RSProperties& properties, SkCanvas& canvas, SkRect maskBounds);
    static bool GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, SkMatrix& mat);
    static SkRRect RRect2SkRRect(const RRect& rr);
    static SkRect Rect2SkRect(const RectF& r);
    static SkColor CalcAverageColor(sk_sp<SkImage> imageSnapshot);
     // functions that are dedicated to driven render [start]
    static RectF GetCmdsClipRect(DrawCmdListPtr& cmds);
    static void DrawFrameForDriven(const RSProperties& properties, RSPaintFilterCanvas& canvas, DrawCmdListPtr& cmds);
    // functions that are dedicated to driven render [end]
    static void DrawSpherize(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        const sk_sp<SkSurface>& spherizeSurface);
    static sk_sp<SkBlender> MakeDynamicLightUpBlender(
        float dynamicLightUpRate, float dynamicLightUpDeg);
    static sk_sp<SkImage> DrawGreyAdjustment(SkCanvas& canvas, const sk_sp<SkImage>& image,
        const float greyCoef1, const float greyCoef2);
#else // USE_ROSEN_DRAWING
    static void Clip(Drawing::Canvas& canvas, RectF rect, bool isAntiAlias = true);
    static void DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawOutline(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawLight(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawFrame(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::DrawCmdListPtr& drawCmdList);
    static void DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas, FilterType filterType,
        const std::optional<Drawing::Rect>& rect = std::nullopt,
        const std::shared_ptr<RSFilter>& externalFilter = nullptr);
    static void DrawForegroundColor(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawMask(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawMask(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Rect maskBounds);
    static bool GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, Drawing::Matrix& mat);
    static Drawing::RoundRect RRect2DrawingRRect(const RRect& rr);
    static Drawing::Rect Rect2DrawingRect(const RectF& r);
    static Drawing::ColorQuad CalcAverageColor(std::shared_ptr<Drawing::Image> imageSnapshot);
    // functions that are dedicated to driven render [start]
    static RectF GetCmdsClipRect(Drawing::DrawCmdListPtr& cmds);
    static void DrawFrameForDriven(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        Drawing::DrawCmdListPtr& cmds);
    // functions that are dedicated to driven render [end]
    static void DrawSpherize(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        const std::shared_ptr<Drawing::Surface>& spherizeSurface);
    static std::shared_ptr<Drawing::Blender> MakeDynamicLightUpBlender(
        float dynamicLightUpRate, float dynamicLightUpDeg);
    static std::shared_ptr<Drawing::Image> DrawGreyAdjustment(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image, const float greyCoef1, const float greyCoef2);
#endif // USE_ROSEN_DRAWING

    // EffectView and useEffect
    static void DrawBackgroundImageAsEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawBackgroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    static const bool BLUR_ENABLED;

private:
    static void ApplyBackgroundEffectFallback(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static RRect GetRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool& isOutline);
    static RRect GetInnerRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool& isOutline);
    static void ClipVisibleCanvas(const RSProperties& properties, RSPaintFilterCanvas& canvas);
#ifndef USE_ROSEN_DRAWING
    static void DrawColorfulShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& path);
    static void DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& path);
    static void DrawLightInner(const RSProperties& properties, SkCanvas& canvas,
        std::shared_ptr<SkRuntimeShaderBuilder>& lightBuilder,
        const std::unordered_set<std::shared_ptr<RSLightSource>>& lightSources,
        const std::shared_ptr<RSObjAbsGeometry>& geoPtr);
    static void DrawContentLight(const RSProperties& properties, SkCanvas& canvas,
        std::shared_ptr<SkRuntimeShaderBuilder>& lightBuilder, SkPaint& paint, SkV4& lightIntensity);
    static void DrawBorderLight(const RSProperties& properties, SkCanvas& canvas,
        std::shared_ptr<SkRuntimeShaderBuilder>& lightBuilder, SkPaint& paint, SkV4& lightIntensity);
    static sk_sp<SkShader> MakeLightUpEffectShader(float lightUpDeg, sk_sp<SkShader> imageShader);
    static sk_sp<SkShader> MakeBinarizationShader(float low, float high,
        float thresholdLow, float thresholdHigh, sk_sp<SkShader> imageShader);
    static sk_sp<SkRuntimeEffect> MakeGreyAdjustmentEffect();

    static void DrawBorderBase(const RSProperties& properties, SkCanvas& canvas,
        const std::shared_ptr<RSBorder>& border, const bool& isOutline);
    static const std::shared_ptr<SkRuntimeShaderBuilder>& GetPhongShaderBuilder();

    static sk_sp<SkRuntimeEffect> greyAdjustEffect_;
#else // USE_ROSEN_DRAWING
    static void DrawColorfulShadowInner(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
    static void DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
    static void DrawLightInner(const RSProperties& properties, Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
        const std::unordered_set<std::shared_ptr<RSLightSource>>& lightSources,
        const std::shared_ptr<RSObjAbsGeometry>& geoPtr);
    static void DrawContentLight(const RSProperties& properties, Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Vector4f& lightIntensity);
    static void DrawBorderLight(const RSProperties& properties, Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen, Vector4f& lightIntensity);
    static std::shared_ptr<Drawing::ShaderEffect> MakeLightUpEffectShader(
        float lightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeBinarizationShader(float low, float high,
        float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::RuntimeEffect> MakeGreyAdjustmentEffect();

    static void DrawBorderBase(const RSProperties& properties, Drawing::Canvas& canvas,
        const std::shared_ptr<RSBorder>& border, const bool& isOutline);
    static const std::shared_ptr<Drawing::RuntimeShaderBuilder>& GetPhongShaderBuilder();

    static std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> binarizationShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> lightUpEffectShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicLightUpBlenderEffect_;
#endif // USE_ROSEN_DRAWING
    inline static int g_blurCnt = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
