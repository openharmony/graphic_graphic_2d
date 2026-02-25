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

#include "effect/runtime_blender_builder.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {

/*
Feature:
    NodeGroup recommendation:ROUND_STATIC_CAST_INT
    HardwareComposer recommendation: clip hole-ROUND_IN, draw-ROUND_OUT
    RSUniRenderComposerAdapter: ROUND_IN
Dirty Region and Occlusion:
    Dirty Region: ROUND_OUT
    Occlusion: ROUND_IN
Roration and Drag: ROUND_OFF
SnapShot and Draw:
    SnapShot:ROUND_IN
    Draw：ROUND_OUT
Default:ROUND_BUTT(ROUND_OUT)
 */
enum class RoundingStrategyType : uint8_t {
    ROUND_IN = 0,
    ROUND_OUT,
    ROUND_OFF,
    ROUND_STATIC_CAST_INT,
    ROUND_BUTT,
};
namespace Drawing {
class GEVisualEffectContainer;
}

class RSPropertyDrawableUtils {
public:
    static Drawing::RoundRect RRect2DrawingRRect(const RRect& rr);
    static Drawing::Rect Rect2DrawingRect(const RectF& r);
    static RectI DrawingRectI2RectI(const Drawing::RectI& r);
    static RRect GetRRectForDrawingBorder(
        const RSProperties& properties, const std::shared_ptr<RSBorder>& border, const bool& isOutline);
    static RRect GetInnerRRectForDrawingBorder(
        const RSProperties& properties, const std::shared_ptr<RSBorder>& border, const bool& isOutline);
    /**
     * @brief Check if the background is light or dark using extracted color,
     * and update the adaptive frosted glass params.
     */
    static void ApplyAdaptiveFrostedGlassParams(
        Drawing::Canvas* canvas, const std::shared_ptr<RSDrawingFilter>& filter);
    static Color GetColorForShadowSyn(Drawing::Canvas* canvas, Drawing::Path& path, const Color& color,
        const int& colorStrategy);
    static std::shared_ptr<Drawing::Image> GetShadowRegionImage(Drawing::Canvas* canvas,
        Drawing::Path& drPath, Drawing::Matrix& matrix);
    static bool PickColorSyn(Drawing::Canvas* canvas, Drawing::Path& drPath, Drawing::Matrix& matrix,
        RSColor& colorPicked, const int& colorStrategy);
    static bool PickColor(std::shared_ptr<Drawing::GPUContext> context, std::shared_ptr<Drawing::Image> image,
        Drawing::ColorQuad& colorPicked, void* waitSemaphore = nullptr);
    // Note: waitSemaphore uses void* instead of VkSemaphore to avoid preprocessor macro guards in
    // function signatures. This keeps the API consistent across all build configurations. The
    // implementation reinterpret_casts to VkSemaphore when RS_ENABLE_VK is defined.
    static std::shared_ptr<Drawing::Image> GpuScaleImage(std::shared_ptr<Drawing::GPUContext> context,
        std::shared_ptr<Drawing::Image> image, void* waitSemaphore = nullptr);
    static void GetDarkColor(RSColor& color);
    static void BeginForegroundFilter(RSPaintFilterCanvas& canvas, const RectF& bounds);
    static void DrawForegroundFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSFilter>& rsFilter);
    static void DrawFilter(Drawing::Canvas* canvas, const std::shared_ptr<RSFilter>& rsFilter,
        const std::unique_ptr<RSFilterCacheManager>& cacheManager, NodeId id, const bool isForegroundFilter,
        const std::optional<Drawing::RectI>& snapshotRect = std::nullopt,
        const std::optional<Drawing::RectI>& drawRect = std::nullopt);
    static void DrawBackgroundEffect(RSPaintFilterCanvas* canvas, const std::shared_ptr<RSFilter>& rsFilter,
        const std::unique_ptr<RSFilterCacheManager>& cacheManager,
        Drawing::RectI& bounds, bool behindWindow = false);
    static void DrawColorFilter(Drawing::Canvas* canvas, const std::shared_ptr<Drawing::ColorFilter>& colorFilter);
    static void DrawLightUpEffect(Drawing::Canvas* canvas, const float lightUpEffectDegree);
    static std::shared_ptr<Drawing::ShaderEffect> MakeLightUpShader(float lightUpDeg,
        std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static void DrawDynamicDim(Drawing::Canvas* canvas, const float dynamicDimDegree);
    static std::shared_ptr<Drawing::ShaderEffect> MakeDynamicDimShader(float dynamicDimDeg,
        std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeBinarizationShader(float low, float high, float thresholdLow,
        float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::RuntimeBlenderBuilder> MakeDynamicBrightnessBuilder();
    static std::shared_ptr<Drawing::RuntimeBlenderBuilder> MakeDynamicBrightnessLinearBuilder();
    static std::shared_ptr<Drawing::Blender> MakeDynamicBrightnessBlender(const RSDynamicBrightnessPara& params);
    static std::shared_ptr<Drawing::Blender> MakeShadowBlender(const RSShadowBlenderPara& params);
    static void DrawBinarization(Drawing::Canvas* canvas, const std::optional<Vector4f>& aiInvert);
    static void DrawPixelStretch(Drawing::Canvas* canvas, const std::optional<Vector4f>& pixelStretch,
        const RectF& boundsRect, const bool boundsGeoValid, const Drawing::TileMode pixelStretchTileMode);
    static Drawing::Path CreateShadowPath(const std::shared_ptr<RSPath> rsPath,
        const std::shared_ptr<RSPath>& clipBounds, const RRect& rrect);
    static void DrawShadow(Drawing::Canvas* canvas, Drawing::Path& path, const float& offsetX, const float& offsetY,
        const float& elevation, const bool& isFilled, Color spotColor);
    static void DrawShadowMaskFilter(Drawing::Canvas* canvas, Drawing::Path& path, const float& offsetX,
        const float& offsetY, const float& radius, const bool& isFilled, Color spotColor, bool disableSDFBlur);
    static void DrawUseEffect(RSPaintFilterCanvas* canvas, UseEffectType useEffectType);

    static bool IsDangerousBlendMode(int blendMode, int blendApplyType);
    static void BeginBlender(RSPaintFilterCanvas& canvas, std::shared_ptr<Drawing::Blender> blender,
        int blendModeApplyType, bool isDangerous);
    static void EndBlender(RSPaintFilterCanvas& canvas, int blendModeApplyType);

    static Color CalculateInvertColor(const Color& backgroundColor);
    static Color GetInvertBackgroundColor(RSPaintFilterCanvas& canvas, bool needClipToBounds,
        const Vector4f& boundsRect, const Color& backgroundColor);
    RSB_EXPORT static int GetAndResetBlurCnt();
    static bool GetGravityMatrix(const Gravity& gravity, const Drawing::Rect& rect, const float& w, const float& h,
        Drawing::Matrix& mat);
    static bool RSFilterSetPixelStretch(const RSProperties& property, const std::shared_ptr<RSFilter>& filter);
    static void RSFilterRemovePixelStretch(const std::shared_ptr<RSFilter>& filter);
    static void DrawFilterWithDRM(Drawing::Canvas* canvas, bool isDark);
    static void DrawColorUsingSDFWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect, bool isDark,
        const std::shared_ptr<Drawing::GEVisualEffectContainer>& filterGEContainer, const std::string& filterTag,
        const std::string& shapeTag);

    static std::shared_ptr<RSFilter> GenerateBehindWindowFilter(float radius, float saturation, float brightness,
        RSColor maskColor);

    static bool IsBlurFilterType(const RSFilter::FilterType& filterType);

    static float GetBlurFilterRadius(const std::shared_ptr<RSFilter>& rsFilter);

    RSB_EXPORT static Drawing::RectI GetRectByStrategy(
        const Drawing::Rect& rect, RoundingStrategyType roundingStrategy);
    RSB_EXPORT static Drawing::RectI GetAbsRectByStrategy(const Drawing::Surface* surface,
        const Drawing::Matrix& totalMatrix, const Drawing::Rect& relativeRect, RoundingStrategyType roundingStrategy);
    RSB_EXPORT static std::tuple<Drawing::RectI, Drawing::RectI> GetAbsRectByStrategyForImage(
        const Drawing::Surface* surface, const Drawing::Matrix& totalMatrix, const Drawing::Rect& relativeRect);
    static void ApplySDFShapeToFilter(const RSProperties& properties,
        const std::shared_ptr<RSDrawingFilter>& drawingFilter, NodeId nodeId);

private:
    static std::shared_ptr<Drawing::ColorFilter> GenerateMaterialColorFilter(float sat, float brt);
    inline static void ClipVisibleRect(RSPaintFilterCanvas* canvas);
    static std::shared_ptr<Drawing::RuntimeEffect> binarizationShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicDimShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicBrightnessBlenderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicBrightnessLinearBlenderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> lightUpShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> shadowBlenderEffect_;
    inline static std::atomic<int> g_blurCnt = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H
