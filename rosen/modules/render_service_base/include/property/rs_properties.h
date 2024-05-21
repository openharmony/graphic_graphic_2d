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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H

#include <bitset>
#include <optional>
#include <tuple>
#include <vector>

#include "animation/rs_render_particle.h"
#include "animation/rs_particle_noise_field.h"
#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "effect/runtime_blender_builder.h"
#include "modifier/rs_modifier_type.h"
#include "property/rs_properties_def.h"
#include "property/rs_color_picker_cache_task.h"
#include "render/rs_aibar_filter.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_shadow.h"

#include "property/rs_filter_cache_manager.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;
class RSObjAbsGeometry;
namespace DrawableV2 {
class RSBackgroundImageDrawable;
class RSBackgroundFilterDrawable;
class RSShadowDrawable;
class RSFilterDrawable;
}
class RSB_EXPORT RSProperties final {
public:
    RSProperties();
    RSProperties(const RSProperties&) = delete;
    RSProperties(const RSProperties&&) = delete;
    RSProperties& operator=(const RSProperties&) = delete;
    RSProperties& operator=(const RSProperties&&) = delete;
    virtual ~RSProperties();

    // geometry properties
    void SetBounds(Vector4f bounds);
    void SetBoundsSize(Vector2f size);
    void SetBoundsWidth(float width);
    void SetBoundsHeight(float height);
    void SetBoundsPosition(Vector2f position);
    void SetBoundsPositionX(float positionX);
    void SetBoundsPositionY(float positionY);
    Vector4f GetBounds() const;
    Vector2f GetBoundsSize() const;
    float GetBoundsWidth() const;
    float GetBoundsHeight() const;
    Vector2f GetBoundsPosition() const;
    float GetBoundsPositionX() const;
    float GetBoundsPositionY() const;

    void SetFrame(Vector4f frame);
    void SetFrameSize(Vector2f size);
    void SetFrameWidth(float width);
    void SetFrameHeight(float height);
    void SetFramePosition(Vector2f position);
    void SetFramePositionX(float positionX);
    void SetFramePositionY(float positionY);
    Vector4f GetFrame() const;
    Vector2f GetFrameSize() const;
    float GetFrameWidth() const;
    float GetFrameHeight() const;
    Vector2f GetFramePosition() const;
    float GetFramePositionX() const;
    float GetFramePositionY() const;
    float GetFrameOffsetX() const;
    float GetFrameOffsetY() const;

    void SetSandBox(const std::optional<Vector2f>& parentPosition);
    std::optional<Vector2f> GetSandBox() const;
    void ResetSandBox();
    void UpdateSandBoxMatrix(const std::optional<Drawing::Matrix>& rootMatrix);
    std::optional<Drawing::Matrix> GetSandBoxMatrix() const;

    void SetPositionZ(float positionZ);
    float GetPositionZ() const;

    void SetPivot(Vector2f pivot);
    void SetPivotX(float pivotX);
    void SetPivotY(float pivotY);
    void SetPivotZ(float pivotZ);
    Vector2f GetPivot() const;
    float GetPivotX() const;
    float GetPivotY() const;
    float GetPivotZ() const;

    void SetCornerRadius(const Vector4f& cornerRadius);
    const Vector4f& GetCornerRadius() const;

    void SetQuaternion(Quaternion quaternion);
    void SetRotation(float degree);
    void SetRotationX(float degree);
    void SetRotationY(float degree);
    void SetCameraDistance(float cameraDistance);
    Quaternion GetQuaternion() const;
    float GetRotation() const;
    float GetRotationX() const;
    float GetRotationY() const;
    float GetCameraDistance() const;

    void SetTranslate(Vector2f translate);
    void SetTranslateX(float translate);
    void SetTranslateY(float translate);
    void SetTranslateZ(float translate);
    Vector2f GetTranslate() const;
    float GetTranslateX() const;
    float GetTranslateY() const;
    float GetTranslateZ() const;

    void SetScale(Vector2f scale);
    void SetScaleX(float sx);
    void SetScaleY(float sy);
    Vector2f GetScale() const;
    float GetScaleX() const;
    float GetScaleY() const;

    void SetSkew(Vector2f skew);
    void SetSkewX(float skewX);
    void SetSkewY(float skewY);
    Vector2f GetSkew() const;
    float GetSkewX() const;
    float GetSkewY() const;

    void SetPersp(Vector2f persp);
    void SetPerspX(float perspX);
    void SetPerspY(float perspY);
    Vector2f GetPersp() const;
    float GetPerspX() const;
    float GetPerspY() const;

    void SetAlpha(float alpha);
    float GetAlpha() const;
    void SetAlphaOffscreen(bool alphaOffscreen);
    bool GetAlphaOffscreen() const;

    void SetSublayerTransform(const std::optional<Matrix3f>& sublayerTransform);
    const std::optional<Matrix3f>& GetSublayerTransform() const;

    inline bool GetUseShadowBatching() const
    {
        return useShadowBatching_;
    }

    void SetUseShadowBatching(bool useShadowBatching);
    inline bool GetNeedSkipShadow() const
    {
        return needSkipShadow_;
    }

    inline void SetNeedSkipShadow(bool needSkipShadow)
    {
        needSkipShadow_ = needSkipShadow;
    }

    // particle properties
    void SetParticles(const RSRenderParticleVector& particles);
    const RSRenderParticleVector& GetParticles() const;

    // foreground properties
    void SetForegroundColor(Color color);
    Color GetForegroundColor() const;

    // background properties
    void SetBackgroundColor(Color color);
    const Color& GetBackgroundColor() const;
    void SetBackgroundShader(const std::shared_ptr<RSShader>& shader);
    std::shared_ptr<RSShader> GetBackgroundShader() const;
    void SetBgImage(const std::shared_ptr<RSImage>& image);
    std::shared_ptr<RSImage> GetBgImage() const;
    void SetBgImageInnerRect(const Vector4f& rect);
    Vector4f GetBgImageInnerRect() const;
    void SetBgImageWidth(float width);
    void SetBgImageHeight(float height);
    void SetBgImagePositionX(float positionX);
    void SetBgImagePositionY(float positionY);
    float GetBgImageWidth() const;
    float GetBgImageHeight() const;
    float GetBgImagePositionX() const;
    float GetBgImagePositionY() const;

    // border properties
    void SetBorderColor(Vector4<Color> color);
    void SetBorderWidth(Vector4f width);
    void SetBorderStyle(Vector4<uint32_t> style);
    Vector4<Color> GetBorderColor() const;
    Vector4f GetBorderWidth() const;
    Vector4<uint32_t> GetBorderStyle() const;
    const std::shared_ptr<RSBorder>& GetBorder() const;
    void SetOutlineColor(Vector4<Color> color);
    void SetOutlineWidth(Vector4f width);
    void SetOutlineStyle(Vector4<uint32_t> style);
    void SetOutlineRadius(Vector4f radius);
    Vector4<Color> GetOutlineColor() const;
    Vector4f GetOutlineWidth() const;
    Vector4<uint32_t> GetOutlineStyle() const;
    Vector4f GetOutlineRadius() const;
    const std::shared_ptr<RSBorder>& GetOutline() const;
    bool GetBorderColorIsTransparent() const;

    void SetForegroundEffectRadius(const float foregroundEffectRadius);
    float GetForegroundEffectRadius() const;
    bool IsForegroundEffectRadiusValid() const;

    // filter properties
    void SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter);
    void SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para);
    void SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para);
    void SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para);
    void SetDynamicLightUpRate(const std::optional<float>& rate);
    void SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree);
    void SetDynamicDimDegree(const std::optional<float>& DimDegree);

    void SetFgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params);
    std::optional<RSDynamicBrightnessPara> GetFgBrightnessParams() const;
    void SetFgBrightnessFract(float fraction);
    float GetFgBrightnessFract() const;

    void SetBgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params);
    std::optional<RSDynamicBrightnessPara> GetBgBrightnessParams() const;
    void SetBgBrightnessFract(float fraction);
    float GetBgBrightnessFract() const;

    void SetFilter(const std::shared_ptr<RSFilter>& filter);
    void SetMotionBlurPara(const std::shared_ptr<MotionBlurParam>& para);
    const std::shared_ptr<RSFilter>& GetBackgroundFilter() const;
    const std::shared_ptr<RSLinearGradientBlurPara>& GetLinearGradientBlurPara() const;
    const std::vector<std::shared_ptr<EmitterUpdater>>& GetEmitterUpdater() const;
    const std::shared_ptr<ParticleNoiseFields>& GetParticleNoiseFields() const;
    void IfLinearGradientBlurInvalid();
    const std::shared_ptr<RSFilter>& GetFilter() const;
    const std::shared_ptr<MotionBlurParam>& GetMotionBlurPara() const;
    bool NeedFilter() const;
    void SetGreyCoef(const std::optional<Vector2f>& greyCoef);
    const std::optional<Vector2f>& GetGreyCoef() const;
    const std::shared_ptr<RSFilter>& GetForegroundFilter() const;
    void SetForegroundFilter(const std::shared_ptr<RSFilter>& foregroundFilter);

    void SetBackgroundBlurRadius(float backgroundBlurRadius);
    float GetBackgroundBlurRadius() const;
    bool IsBackgroundBlurRadiusValid() const;

    void SetBackgroundBlurSaturation(float backgroundBlurSaturation);
    float GetBackgroundBlurSaturation() const;
    bool IsBackgroundBlurSaturationValid() const;

    void SetBackgroundBlurBrightness(float backgroundBlurBrightness);
    float GetBackgroundBlurBrightness() const;
    bool IsBackgroundBlurBrightnessValid() const;

    void SetBackgroundBlurMaskColor(Color backgroundMaskColor);
    const Color& GetBackgroundBlurMaskColor() const;
    bool IsBackgroundBlurMaskColorValid() const;

    void SetBackgroundBlurColorMode(int backgroundColorMode);
    int GetBackgroundBlurColorMode() const;

    void SetBackgroundBlurRadiusX(float backgroundBlurRadiusX);
    float GetBackgroundBlurRadiusX() const;
    bool IsBackgroundBlurRadiusXValid() const;

    void SetBackgroundBlurRadiusY(float backgroundBlurRadiusY);
    float GetBackgroundBlurRadiusY() const;
    bool IsBackgroundBlurRadiusYValid() const;

    void SetForegroundBlurRadius(float ForegroundBlurRadius);
    float GetForegroundBlurRadius() const;
    bool IsForegroundBlurRadiusValid() const;

    void SetForegroundBlurSaturation(float ForegroundBlurSaturation);
    float GetForegroundBlurSaturation() const;
    bool IsForegroundBlurSaturationValid() const;

    void SetForegroundBlurBrightness(float ForegroundBlurBrightness);
    float GetForegroundBlurBrightness() const;
    bool IsForegroundBlurBrightnessValid() const;

    void SetForegroundBlurMaskColor(Color ForegroundMaskColor);
    const Color& GetForegroundBlurMaskColor() const;
    bool IsForegroundBlurMaskColorValid() const;

    void SetForegroundBlurColorMode(int ForegroundColorMode);
    int GetForegroundBlurColorMode() const;

    void SetForegroundBlurRadiusX(float foregroundBlurRadiusX);
    float GetForegroundBlurRadiusX() const;
    bool IsForegroundBlurRadiusXValid() const;

    void SetForegroundBlurRadiusY(float foregroundBlurRadiusY);
    float GetForegroundBlurRadiusY() const;
    bool IsForegroundBlurRadiusYValid() const;

    bool IsBackgroundMaterialFilterValid() const;
    bool IsForegroundMaterialFilterVaild() const;
    
    // shadow properties
    void SetShadowColor(Color color);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowAlpha(float alpha);
    void SetShadowElevation(float radius);
    void SetShadowRadius(float radius);
    void SetShadowPath(std::shared_ptr<RSPath> shadowPath);
    void SetShadowMask(bool shadowMask);
    void SetShadowIsFilled(bool shadowIsFilled);
    void SetShadowColorStrategy(int shadowColorStrategy);
    const Color& GetShadowColor() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
    float GetShadowAlpha() const;
    float GetShadowElevation() const;
    float GetShadowRadius() const;
    const std::optional<float>& GetDynamicLightUpRate() const;
    const std::optional<float>& GetDynamicLightUpDegree() const;
    const std::optional<float>& GetDynamicDimDegree() const;
    std::shared_ptr<RSPath> GetShadowPath() const;
    bool GetShadowMask() const;
    bool GetShadowIsFilled() const;
    int GetShadowColorStrategy() const;
    const std::optional<RSShadow>& GetShadow() const;
    bool IsShadowValid() const;

    void SetFrameGravity(Gravity gravity);
    Gravity GetFrameGravity() const;

    void SetDrawRegion(const std::shared_ptr<RectF>& rect);
    std::shared_ptr<RectF> GetDrawRegion() const;

    void SetClipRRect(RRect clipRRect);
    RRect GetClipRRect() const;
    bool GetClipToRRect() const;
    void SetClipBounds(const std::shared_ptr<RSPath>& path);
    const std::shared_ptr<RSPath>& GetClipBounds() const;
    void SetClipToBounds(bool clipToBounds);
    bool GetClipToBounds() const;
    void SetClipToFrame(bool clipToFrame);
    bool GetClipToFrame() const;

    void SetVisible(bool visible);
    bool GetVisible() const;
    std::string Dump() const;

    void SetMask(const std::shared_ptr<RSMask>& mask);
    std::shared_ptr<RSMask> GetMask() const;

    // Pixel Stretch
    void SetPixelStretch(const std::optional<Vector4f>& stretchSize);
    inline const std::optional<Vector4f>& GetPixelStretch() const
    {
        return pixelStretch_;
    }

    void SetPixelStretchPercent(const std::optional<Vector4f>& stretchPercent);
    inline const std::optional<Vector4f>& GetPixelStretchPercent() const
    {
        return pixelStretchPercent_;
    }

    void SetPixelStretchTileMode(int stretchTileMode);
    int GetPixelStretchTileMode() const;

    void SetAiInvert(const std::optional<Vector4f>& aiInvert);
    const std::optional<Vector4f>& GetAiInvert() const;
    void SetSystemBarEffect(bool systemBarEffect);
    bool GetSystemBarEffect() const;
    RectI GetPixelStretchDirtyRect() const;

    const std::shared_ptr<RSObjAbsGeometry>& GetBoundsGeometry() const;
    const std::shared_ptr<RSObjGeometry>& GetFrameGeometry() const;
    bool UpdateGeometry(const RSProperties* parent, bool dirtyFlag, const std::optional<Drawing::Point>& offset);
    bool UpdateGeometryByParent(const Drawing::Matrix* parentMatrix, const std::optional<Drawing::Point>& offset);
    RectF GetLocalBoundsAndFramesRect() const;
    RectF GetBoundsRect() const;

    bool IsGeoDirty() const;
    bool IsContentDirty() const;

    void SetSpherize(float spherizeDegree);
    float GetSpherize() const;
    bool IsSpherizeValid() const;

    void SetLightUpEffect(float lightUpEffectDegree);
    float GetLightUpEffect() const;
    bool IsLightUpEffectValid() const;
    bool IsDynamicLightUpValid() const;
    bool IsDynamicDimValid() const;
    bool IsFgBrightnessValid() const;
    bool IsBgBrightnessValid() const;

    // Image effect properties
    void SetGrayScale(const std::optional<float>& grayScale);
    inline const std::optional<float>& GetGrayScale() const
    {
        return grayScale_;
    }

    void SetBrightness(const std::optional<float>& brightness);
    const std::optional<float>& GetBrightness() const;
    void SetContrast(const std::optional<float>& contrast);
    const std::optional<float>& GetContrast() const;
    void SetSaturate(const std::optional<float>& saturate);
    const std::optional<float>& GetSaturate() const;
    void SetSepia(const std::optional<float>& sepia);
    const std::optional<float>& GetSepia() const;
    void SetInvert(const std::optional<float>& invert);
    const std::optional<float>& GetInvert() const;
    void SetHueRotate(const std::optional<float>& hueRotate);
    const std::optional<float>& GetHueRotate() const;
    void SetColorBlend(const std::optional<Color>& colorBlend);
    const std::optional<Color>& GetColorBlend() const;

    const std::shared_ptr<Drawing::ColorFilter>& GetColorFilter() const;

    void SetLightIntensity(float lightIntensity);
    void SetLightColor(Color lightColor);
    void SetLightPosition(const Vector4f& lightPosition);
    void SetIlluminatedBorderWidth(float illuminatedBorderWidth);
    void SetIlluminatedType(int illuminatedType);
    void SetBloom(float bloomIntensity);
    float GetLightIntensity() const;
    Color GetLightColor() const;
    Vector4f GetLightPosition() const;
    inline float GetIlluminatedBorderWidth() const
    {
        return illuminatedPtr_ ? illuminatedPtr_->GetIlluminatedBorderWidth() : 0.f;
    }

    int GetIlluminatedType() const;
    inline float GetBloom() const
    {
        return illuminatedPtr_ ? illuminatedPtr_->GetBloomIntensity() : 0.f;
    }

    void CalculateAbsLightPosition();
    inline const std::shared_ptr<RSLightSource>& GetLightSource() const
    {
        return lightSourcePtr_;
    }
    inline const std::shared_ptr<RSIlluminated>& GetIlluminated() const
    {
        return illuminatedPtr_;
    }

    void SetUseEffect(bool useEffect);
    bool GetUseEffect() const;

    void SetColorBlendMode(int colorBlendMode);
    int GetColorBlendMode() const;
    void SetColorBlendApplyType(int colorBlendApplyType);
    int GetColorBlendApplyType() const;

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    const std::unique_ptr<RSFilterCacheManager>& GetFilterCacheManager(bool isForeground) const;
    std::shared_ptr<RSColorPickerCacheTask> GetColorPickerCacheTaskShadow() const;
    void ReleaseColorPickerTaskShadow() const;
    void ClearFilterCache();
#endif

    const RRect& GetRRect() const;
    RRect GetInnerRRect() const;
    RectF GetFrameRect() const;

    bool GetHaveEffectRegion() const;
    void SetHaveEffectRegion(bool hasEffectRegion);

    void OnApplyModifiers();

private:
    void ResetProperty(const std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)>& dirtyTypes);
    void SetDirty();
    void ResetDirty();
    bool IsDirty() const;
    void AccmulateDirtyStatus();
    void RecordCurDirtyStatus();

    // generate filter
    void GenerateBackgroundFilter();
    void GenerateForegroundFilter();
    void GenerateBackgroundBlurFilter();
    void GenerateForegroundBlurFilter();
    void GenerateBackgroundMaterialBlurFilter();
    void GenerateForegroundMaterialBlurFilter();
    std::shared_ptr<Drawing::ColorFilter> GetMaterialColorFilter(float sat, float brightness);
    void GenerateAIBarFilter();
    void GenerateLinearGradientBlurFilter();

    bool NeedClip() const;

    const RectF& GetBgImageRect() const;
    void GenerateRRect();
    RectI GetDirtyRect() const;
    // added for update dirty region dfx
    RectI GetDirtyRect(RectI& drawRegion) const;

    bool visible_ = true;
    bool clipToBounds_ = false;
    bool clipToFrame_ = false;
    bool isDirty_ = false;
    bool geoDirty_ = false;
    bool contentDirty_ = false;
    bool curIsDirty_ = false;
    bool curGeoDirty_ = false;
    bool curContentDirty_ = false;
    bool isDrawn_ = false;
    bool alphaNeedApply_ = false;
    bool systemBarEffect_ = false;

    bool hasBounds_ = false;
    bool useEffect_ = false;
    bool useShadowBatching_ = false;
    bool needSkipShadow_ = false;

    int colorBlendMode_ = 0;
    int colorBlendApplyType_ = 0;

    std::optional<RSDynamicBrightnessPara> fgBrightnessParams_ = std::nullopt;
    float fgBrightnessFract_ = 1.0f;
    std::optional<RSDynamicBrightnessPara> bgBrightnessParams_ = std::nullopt;
    float bgBrightnessFract_ = 1.0f;

    Gravity frameGravity_ = Gravity::DEFAULT;

    std::shared_ptr<RectF> drawRegion_ = nullptr;

    float alpha_ = 1.f;
    bool alphaOffscreen_ = false;

    std::shared_ptr<RSObjAbsGeometry> boundsGeo_;
    std::shared_ptr<RSObjGeometry> frameGeo_;

    std::shared_ptr<RSLightSource> lightSourcePtr_ = nullptr;
    std::shared_ptr<RSIlluminated> illuminatedPtr_ = nullptr;

    float foregroundEffectRadius_ = 0.f;
    std::shared_ptr<RSFilter> backgroundFilter_ = nullptr;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
    std::shared_ptr<MotionBlurParam> motionBlurPara_ = nullptr;
    std::vector<std::shared_ptr<EmitterUpdater>> emitterUpdater_;
    std::shared_ptr<ParticleNoiseFields> particleNoiseFields_ = nullptr;
    std::shared_ptr<RSBorder> border_ = nullptr;
    std::shared_ptr<RSBorder> outline_ = nullptr;
    std::shared_ptr<RSPath> clipPath_ = nullptr;
    std::optional<Vector4f> cornerRadius_;
    std::optional<Decoration> decoration_;
    std::shared_ptr<RSFilter> filter_ = nullptr;
    std::shared_ptr<RSMask> mask_ = nullptr;
    std::optional<RSShadow> shadow_;
    std::optional<Matrix3f> sublayerTransform_;
    float spherizeDegree_ = 0.f;
    bool isSpherizeValid_ = false;
    float lightUpEffectDegree_ = 1.0f;
    std::shared_ptr<RSFilter> foregroundFilter_ = nullptr; // view content filter

    // filter property
    float backgroundBlurRadius_ = 0.f;
    float backgroundBlurSaturation_ = 1.f;
    float backgroundBlurBrightness_ = 1.f;
    Color backgroundMaskColor_ = RSColor();
    int backgroundColorMode_ = BLUR_COLOR_MODE::DEFAULT;
    float backgroundBlurRadiusX_ = 0.f;
    float backgroundBlurRadiusY_ = 0.f;

    float foregroundBlurRadius_ = 0.f;
    float foregroundBlurSaturation_ = 1.f;
    float foregroundBlurBrightness_ = 1.f;
    Color foregroundMaskColor_ = RSColor();
    int foregroundColorMode_ = BLUR_COLOR_MODE::DEFAULT;
    float foregroundBlurRadiusX_ = 0.f;
    float foregroundBlurRadiusY_ = 0.f;
    
    std::weak_ptr<RSRenderNode> backref_;

    std::optional<Vector4f> pixelStretch_;
    std::optional<Vector4f> pixelStretchPercent_;
    int pixelStretchTileMode_ = 0;

    std::optional<Vector4f> aiInvert_;
    std::optional<RRect> clipRRect_;

    std::optional<float> grayScale_;
    std::optional<float> brightness_;
    std::optional<float> contrast_;
    std::optional<float> saturate_;
    std::optional<float> sepia_;
    std::optional<float> invert_;
    std::optional<float> hueRotate_;
    std::optional<float> dynamicLightUpRate_;
    std::optional<float> dynamicLightUpDegree_;
    std::optional<float> dynamicDimDegree_;
    std::optional<Color> colorBlend_;
    std::optional<RectI> lastRect_;
    std::optional<Vector2f> greyCoef_;

    // OnApplyModifiers hooks
    void CheckEmptyBounds();
    void GenerateColorFilter();
    void CalculatePixelStretch();
    void CalculateFrameOffset();
    void CheckGreyCoef();

    // partial update
    bool colorFilterNeedUpdate_ = false;
    bool pixelStretchNeedUpdate_ = false;
    bool filterNeedUpdate_ = false;
    bool greyCoefNeedUpdate_ = false;
    float frameOffsetX_ = 0.f;
    float frameOffsetY_ = 0.f;
    bool needFilter_ = false;
    RRect rrect_ = RRect{};
    Drawing::Matrix prevAbsMatrix_;

    RSRenderParticleVector particles_;
    std::shared_ptr<Drawing::ColorFilter> colorFilter_ = nullptr;
    bool haveEffectRegion_ = false;

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    void CreateFilterCacheManagerIfNeed();
    std::unique_ptr<RSFilterCacheManager> backgroundFilterCacheManager_;
    std::unique_ptr<RSFilterCacheManager> foregroundFilterCacheManager_;
    static const bool FilterCacheEnabled;
#endif

    std::unique_ptr<Sandbox> sandbox_ = nullptr;

    friend class RSBackgroundImageDrawable;
    friend class RSCanvasRenderNode;
    friend class RSColorfulShadowDrawable;
    friend class RSEffectDataGenerateDrawable;
    friend class RSModifierDrawable;
    friend class RSPropertiesPainter;
    friend class RSRenderNode;
    friend class RSEffectRenderNode;
    friend class RSPropertyDrawableUtils;

    friend class DrawableV2::RSBackgroundImageDrawable;
    friend class DrawableV2::RSBackgroundFilterDrawable;
    friend class DrawableV2::RSShadowDrawable;
    friend class DrawableV2::RSFilterDrawable;
#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H
