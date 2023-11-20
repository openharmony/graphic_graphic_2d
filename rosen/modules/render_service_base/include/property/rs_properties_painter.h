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
#endif

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
enum class FilterType {
    BACKGROUND_FILTER,
    FOREGROUND_FILTER,
};
class RSB_EXPORT RSPropertiesPainter {
public:
    static void SetBgAntiAlias(bool forceBgAntiAlias);
    static bool GetBgAntiAlias();
    static void DrawBackground(const RSProperties& properties, RSPaintFilterCanvas& canvas, bool isAntiAlias = true);
    static void GetShadowDirtyRect(RectI& dirtyShadow, const RSProperties& properties,
        const RRect* rrect = nullptr, bool isAbsCoordinate = true);
    static void DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect = nullptr);
    static int GetAndResetBlurCnt();
    static RSColor PickColor(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& skPath,
        SkMatrix& matrix, SkIRect& deviceClipBounds);
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

#ifndef USE_ROSEN_DRAWING
    static void Clip(SkCanvas& canvas, RectF rect, bool isAntiAlias = true);
    static void DrawBorder(const RSProperties& properties, SkCanvas& canvas);
    static void DrawLight(const RSProperties& properties, SkCanvas& canvas);
    static void DrawFrame(const RSProperties& properties, RSPaintFilterCanvas& canvas, DrawCmdListPtr& drawCmdList);
    static void DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas, FilterType filterType,
        const std::optional<SkRect>& rect = std::nullopt, const std::shared_ptr<RSFilter>& externalFilter = nullptr);
    static void DrawLinearGradientBlurFilter(const RSProperties& properties,
        RSPaintFilterCanvas& canvas, const std::optional<SkRect>& rect = std::nullopt);
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
    // EffectView and useEffect
    static void DrawBackgroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas, const SkRect& rect);
    static sk_sp<SkShader> MakeDynamicLightUpShader(
        float dynamicLightUpRate, float dynamicLightUpDeg, sk_sp<SkShader> imageShader);
#else // USE_ROSEN_DRAWING
    static void Clip(Drawing::Canvas& canvas, RectF rect, bool isAntiAlias = true);
    static void DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawFrame(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::DrawCmdListPtr& drawCmdList);
    static void DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas, FilterType filterType,
        const std::optional<Drawing::Rect>& rect = std::nullopt,
        const std::shared_ptr<RSFilter>& externalFilter = nullptr);
    static void DrawLinearGradientBlurFilter(const RSProperties& properties,
        RSPaintFilterCanvas& canvas, const std::optional<Drawing::Rect>& rect = std::nullopt);
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
    // EffectView and useEffect
    static void DrawBackgroundEffect(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, const Drawing::Rect& rect);
    static std::shared_ptr<Drawing::ShaderEffect> MakeDynamicLightUpShader(
        float dynamicLightUpRate, float dynamicLightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader);
#endif // USE_ROSEN_DRAWING

    static const bool BLUR_ENABLED;

private:
    static void ApplyBackgroundEffectFallback(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias);
    static RRect GetRRectForDrawingBorder(const RRect& rr, const std::shared_ptr<RSBorder>& border,
                                          const Vector4f& outset, const bool& isFirstLayerBorder);
    static RRect GetInnerRRectForDrawingBorder(const RSProperties& properties,
                                               const std::shared_ptr<RSBorder>& border,
                                               const Vector4f& innerOutset, const bool& isFirstLayerBorder);
#ifndef USE_ROSEN_DRAWING
    static void DrawColorfulShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& path);
    static void DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& path);
    static void DrawContentLight(const RSProperties& properties, SkCanvas& canvas,
        std::shared_ptr<SkRuntimeShaderBuilder> lightBuilder, SkPaint& paint, float lightIntensity);
    static void DrawBorderLight(const RSProperties& properties, SkCanvas& canvas,
        std::shared_ptr<SkRuntimeShaderBuilder> lightBuilder, SkPaint& paint, float lightIntensity);
    static bool GetGradientDirectionPoints(SkPoint (&pts)[2],
                                const SkRect& clipBounds, GradientDirection direction);
    static sk_sp<SkShader> MakeAlphaGradientShader(const SkRect& clipBounds,
                                const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias);
    static sk_sp<SkShader> MakeHorizontalMeanBlurShader(float radiusIn,
                                            sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader);
    static sk_sp<SkShader> MakeVerticalMeanBlurShader(float radiusIn,
                                            sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader);
    static sk_sp<SkShader> MakeLightUpEffectShader(float lightUpDeg, sk_sp<SkShader> imageShader);
    static sk_sp<SkShader> MakeBinarizationShader(float low, float high, float threshold, sk_sp<SkShader> imageShader);
    static void DrawHorizontalLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
        float radius, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding);
    static void DrawVerticalLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
        float radius, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding);
    static uint8_t CalcDirectionBias(const SkMatrix& mat);
    static void DrawLinearGradientBlur(SkSurface* surface, RSPaintFilterCanvas& canvas,
        const std::shared_ptr<RSLinearGradientBlurPara>& para, sk_sp<SkShader> alphaGradientShader,
        const SkIRect& clipIPadding);
    static void DrawMaskLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
        std::shared_ptr<RSSkiaFilter>& blurFilter, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding);
    static sk_sp<SkShader> MakeMeanBlurShader(sk_sp<SkShader> srcImageShader,
        sk_sp<SkShader> blurImageShader, sk_sp<SkShader> gradientShader);
    static sk_sp<SkShader> MakeGreyAdjustmentShader(const float coef1, const float coef2, sk_sp<SkShader> imageShader);
    static void DrawGreyAdjustment(const RSProperties& properties, RSPaintFilterCanvas& canvas);

    static void DrawBorderBase(const RSProperties& properties, SkCanvas& canvas,
                               const std::shared_ptr<RSBorder>& border, Vector4f& outset,
                               Vector4f& innerOutset, const bool isFirstLayerBorder);
    static const std::shared_ptr<SkRuntimeShaderBuilder>& GetPhongShaderBuilder();
#else // USE_ROSEN_DRAWING
    static void DrawColorfulShadowInner(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
    static void DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
    static bool GetGradientDirectionPoints(
        Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GradientDirection direction);
    static std::shared_ptr<Drawing::ShaderEffect> MakeAlphaGradientShader(const Drawing::Rect& clipBounds,
        const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias);
    static std::shared_ptr<Drawing::ShaderEffect> MakeHorizontalMeanBlurShader(float radiusIn,
        std::shared_ptr<Drawing::ShaderEffect> shader, std::shared_ptr<Drawing::ShaderEffect> gradientShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeVerticalMeanBlurShader(float radiusIn,
        std::shared_ptr<Drawing::ShaderEffect> shader, std::shared_ptr<Drawing::ShaderEffect> gradientShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeLightUpEffectShader(
        float lightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static void DrawHorizontalLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
        float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding);
    static void DrawVerticalLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
        float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding);
    static uint8_t CalcDirectionBias(const Drawing::Matrix& mat);
    static void DrawLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
        const std::shared_ptr<RSLinearGradientBlurPara>& para,
        std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding);
    static void DrawMaskLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
        std::shared_ptr<RSDrawingFilter>& blurFilter, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
        const Drawing::RectI& clipIPadding);
    static std::shared_ptr<Drawing::ShaderEffect> MakeMeanBlurShader(
        std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
        std::shared_ptr<Drawing::ShaderEffect> gradientShader);
#endif // USE_ROSEN_DRAWING
    inline static int g_blurCnt = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
