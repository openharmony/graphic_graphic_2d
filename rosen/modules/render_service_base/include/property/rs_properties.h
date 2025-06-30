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
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector4.h"
#include "effect/runtime_blender_builder.h"
#include "modifier/rs_modifier_type.h"
#include "property/rs_properties_def.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_magnifier_para.h"
#include "render/rs_mask.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_particles_drawable.h"
#include "render/rs_path.h"
#include "render/rs_render_filter.h"
#include "render/rs_shader.h"
#include "render/rs_shadow.h"
#include "render/rs_attraction_effect_filter.h"

#include "render/rs_filter_cache_manager.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;
class RSObjAbsGeometry;
class RSNGRenderFilterBase;
namespace DrawableV2 {
class RSBackgroundImageDrawable;
class RSBackgroundFilterDrawable;
class RSShadowDrawable;
#ifdef RS_ENABLE_GPU
class RSFilterDrawable;
#endif
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
    void SetPositionZApplicableCamera3D(bool isApplicable);
    bool GetPositionZApplicableCamera3D() const;

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
    void SetScaleZ(float sz);
    Vector2f GetScale() const;
    float GetScaleX() const;
    float GetScaleY() const;
    float GetScaleZ() const;

    void SetSkew(const Vector3f& skew);
    void SetSkewX(float skewX);
    void SetSkewY(float skewY);
    void SetSkewZ(float skewZ);
    Vector3f GetSkew() const;
    float GetSkewX() const;
    float GetSkewY() const;
    float GetSkewZ() const;

    void SetPersp(const Vector4f& persp);
    void SetPerspX(float perspX);
    void SetPerspY(float perspY);
    void SetPerspZ(float perspZ);
    void SetPerspW(float perspW);
    Vector4f GetPersp() const;
    float GetPerspX() const;
    float GetPerspY() const;
    float GetPerspZ() const;
    float GetPerspW() const;

    void SetAlpha(float alpha);
    float GetAlpha() const;
    void SetAlphaOffscreen(bool alphaOffscreen);
    bool GetAlphaOffscreen() const;

    void SetLocalMagnificationCap(bool localMagnificationCap);

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

    void SetHDRBrightnessFactor(float factor);
    float GetHDRBrightnessFactor() const;
    void SetCanvasNodeHDRBrightnessFactor(float factor);
    float GetCanvasNodeHDRBrightnessFactor() const;

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
    void SetBackgroundShaderProgress(const float& progress);
    float GetBackgroundShaderProgress() const;

    void SetComplexShaderParam(const std::vector<float>& param);
    std::optional<std::vector<float>> GetComplexShaderParam() const;

    void SetBgImage(const std::shared_ptr<RSImage>& image);
    std::shared_ptr<RSImage> GetBgImage() const;
    void SetBgImageInnerRect(const Vector4f& rect);
    Vector4f GetBgImageInnerRect() const;
    void SetBgImageDstRect(const Vector4f& rect);
    const Vector4f GetBgImageDstRect();
    const RectF& GetBgImageRect() const;
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
    void SetBorderDashWidth(const Vector4f& dashWidth);
    void SetBorderDashGap(const Vector4f& dashGap);
    Vector4<Color> GetBorderColor() const;
    Vector4f GetBorderWidth() const;
    Vector4<uint32_t> GetBorderStyle() const;
    Vector4f GetBorderDashWidth() const;
    Vector4f GetBorderDashGap() const;
    const std::shared_ptr<RSBorder>& GetBorder() const;
    void SetOutlineColor(Vector4<Color> color);
    void SetOutlineWidth(Vector4f width);
    void SetOutlineStyle(Vector4<uint32_t> style);
    void SetOutlineDashWidth(const Vector4f& dashWidth);
    void SetOutlineDashGap(const Vector4f& dashGap);
    void SetOutlineRadius(Vector4f radius);
    Vector4<Color> GetOutlineColor() const;
    Vector4f GetOutlineWidth() const;
    Vector4<uint32_t> GetOutlineStyle() const;
    Vector4f GetOutlineDashWidth() const;
    Vector4f GetOutlineDashGap() const;
    Vector4f GetOutlineRadius() const;
    const std::shared_ptr<RSBorder>& GetOutline() const;

    void SetForegroundEffectRadius(const float foregroundEffectRadius);
    float GetForegroundEffectRadius() const;
    bool IsForegroundEffectRadiusValid() const;
    void SetForegroundEffectDirty(bool dirty);
    bool GetForegroundEffectDirty() const;
    void SetForegroundFilterCache(const std::shared_ptr<RSFilter>& foregroundFilterCache);
    const std::shared_ptr<RSFilter>& GetForegroundFilterCache() const;

    // filter properties
    void SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para);
    void SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para);
    void SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para);
    void SetDynamicLightUpRate(const std::optional<float>& rate);
    void SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree);
    void SetDynamicDimDegree(const std::optional<float>& DimDegree);

    void SetBackgroundUIFilter(const std::shared_ptr<RSRenderFilter>& renderFilter);
    std::shared_ptr<RSRenderFilter> GetBackgroundUIFilter() const;
    void SetForegroundUIFilter(const std::shared_ptr<RSRenderFilter>& renderFilter);
    std::shared_ptr<RSRenderFilter> GetForegroundUIFilter() const;

    void SetBackgroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetBackgroundNGFilter() const;

    void SetForegroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetForegroundNGFilter() const;

    void SetFgBrightnessRates(const Vector4f& rates);
    Vector4f GetFgBrightnessRates() const;
    void SetFgBrightnessSaturation(const float& saturation);
    float GetFgBrightnessSaturation() const;
    void SetFgBrightnessPosCoeff(const Vector4f& coeff);
    Vector4f GetFgBrightnessPosCoeff() const;
    void SetFgBrightnessNegCoeff(const Vector4f& coeff);
    Vector4f GetFgBrightnessNegCoeff() const;
    void SetFgBrightnessFract(const float& fraction);
    float GetFgBrightnessFract() const;
    void SetFgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params);
    std::optional<RSDynamicBrightnessPara> GetFgBrightnessParams() const;

    void SetWaterRippleParams(const std::optional<RSWaterRipplePara>& params);
    std::optional<RSWaterRipplePara> GetWaterRippleParams() const;
    void SetWaterRippleProgress(const float& progress);
    float GetWaterRippleProgress() const;

    void SetFlyOutParams(const std::optional<RSFlyOutPara>& params);
    std::optional<RSFlyOutPara> GetFlyOutParams() const;
    void SetFlyOutDegree(const float& degree);
    float GetFlyOutDegree() const;
    void CreateFlyOutShaderFilter();

    void SetDistortionK(const std::optional<float>& distortionK);
    const std::optional<float>& GetDistortionK() const;

    void SetBgBrightnessRates(const Vector4f& rates);
    Vector4f GetBgBrightnessRates() const;
    void SetBgBrightnessSaturation(const float& saturation);
    float GetBgBrightnessSaturation() const;
    void SetBgBrightnessPosCoeff(const Vector4f& coeff);
    Vector4f GetBgBrightnessPosCoeff() const;
    void SetBgBrightnessNegCoeff(const Vector4f& coeff);
    Vector4f GetBgBrightnessNegCoeff() const;
    void SetBgBrightnessFract(const float& fraction);
    float GetBgBrightnessFract() const;
    void SetBgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params);
    std::optional<RSDynamicBrightnessPara> GetBgBrightnessParams() const;

    void SetMotionBlurPara(const std::shared_ptr<MotionBlurParam>& para);
    void SetMagnifierParams(const std::shared_ptr<RSMagnifierParams>& para);
    const std::shared_ptr<RSFilter>& GetBackgroundFilter() const;
    const std::shared_ptr<RSLinearGradientBlurPara>& GetLinearGradientBlurPara() const;
    const std::vector<std::shared_ptr<EmitterUpdater>>& GetEmitterUpdater() const;
    const std::shared_ptr<ParticleNoiseFields>& GetParticleNoiseFields() const;
    void IfLinearGradientBlurInvalid();
    const std::shared_ptr<RSFilter>& GetFilter() const;
    const std::shared_ptr<MotionBlurParam>& GetMotionBlurPara() const;
    const std::shared_ptr<RSMagnifierParams>& GetMagnifierPara() const;
    bool NeedFilter() const;
    bool NeedHwcFilter() const;
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

    void SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation);
    bool GetBgBlurDisableSystemAdaptation() const;

    void SetAlwaysSnapshot(bool enable);
    bool GetAlwaysSnapshot() const;

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

    void SetFgBlurDisableSystemAdaptation(bool disableSystemAdaptation);
    bool GetFgBlurDisableSystemAdaptation() const;

    bool IsBackgroundMaterialFilterValid() const;
    bool IsForegroundMaterialFilterVaild() const;
    bool IsBackgroundLightBlurFilterValid() const;
    bool IsForegroundLightBlurFilterValid() const;

    // shadow properties
    void SetShadowColor(Color color);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowAlpha(float alpha);
    void SetShadowElevation(float radius);
    void SetShadowRadius(float radius);
    void SetShadowPath(std::shared_ptr<RSPath> shadowPath);
    void SetShadowMask(int shadowMask);
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
    int GetShadowMask() const;
    bool IsShadowMaskValid() const;
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
    const RSObjGeometry& GetFrameGeometry() const;
    bool UpdateGeometry(const RSProperties* parent, bool dirtyFlag, const std::optional<Drawing::Point>& offset);
    bool UpdateGeometryByParent(const Drawing::Matrix* parentMatrix, const std::optional<Drawing::Point>& offset);
    RectF GetLocalBoundsAndFramesRect() const;
    RectF GetBoundsRect() const;

    bool IsGeoDirty() const;
    bool IsCurGeoDirty() const;
    bool IsContentDirty() const;
    bool IsSubTreeAllDirty() const;

    void SetSpherize(float spherizeDegree);
    float GetSpherize() const;
    bool IsSpherizeValid() const;
    void CreateSphereEffectFilter();

    void SetHDRUIBrightness(float hdrUIBrightness);
    float GetHDRUIBrightness() const;
    bool IsHDRUIBrightnessValid() const;
    void CreateHDRUIBrightnessFilter();

    bool IsAttractionValid() const;
    void SetAttractionFraction(float fraction);
    void SetAttractionDstPoint(Vector2f dstPoint);
    float GetAttractionFraction() const;
    Vector2f GetAttractionDstPoint() const;
    void CreateAttractionEffectFilter();
    void CreateColorfulShadowFilter();
    RectI GetAttractionEffectCurrentDirtyRegion() const;
    void SetLightUpEffect(float lightUpEffectDegree);
    float GetLightUpEffect() const;
    bool IsLightUpEffectValid() const;
    bool IsDynamicLightUpValid() const;
    bool IsDynamicDimValid() const;
    bool IsFgBrightnessValid() const;
    bool IsBgBrightnessValid() const;
    bool IsWaterRippleValid() const;
    bool IsFlyOutValid() const;
    bool IsDistortionKValid() const;
    void SetDistortionDirty(bool distortionEffectDirty);
    bool GetDistortionDirty() const;
    std::string GetFgBrightnessDescription() const;
    std::string GetBgBrightnessDescription() const;

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
    void SetUseEffectType(int useEffectType);
    int GetUseEffectType() const;
    void SetNeedDrawBehindWindow(bool needDrawBehindWindow);
    bool GetNeedDrawBehindWindow() const;

    void SetEnableHDREffect(bool useHDREffect);
    bool GetEnableHDREffect() const;

    void SetColorBlendMode(int colorBlendMode);
    int GetColorBlendMode() const;
    bool IsColorBlendModeValid() const;
    bool IsColorBlendModeNone() const;
    void SetColorBlendApplyType(int colorBlendApplyType);
    int GetColorBlendApplyType() const;
    bool IsColorBlendApplyTypeOffscreen() const;

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    const std::unique_ptr<RSFilterCacheManager>& GetFilterCacheManager(bool isForeground) const;
    void ClearFilterCache();
#endif

    const RRect& GetRRect() const;
    RRect GetInnerRRect() const;
    RectF GetFrameRect() const;

    bool GetHaveEffectRegion() const;
    void SetHaveEffectRegion(bool hasEffectRegion);

    void OnApplyModifiers();

    void ResetBorder(bool isOutline);

    static void SetFilterCacheEnabledByCCM(bool isCCMFilterCacheEnable);
    static void SetBlurAdaptiveAdjustEnabledByCCM(bool isCCMBlurAdaptiveAdjustEnabled);

private:
    inline float DecreasePrecision(float value)
    {
        // preserve two digital precision when calculating hash, this can reuse filterCache as much as possible.
        return 0.01 * round(value * 100);
    }
    void ResetProperty(const std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)>& dirtyTypes);
    void SetDirty();
    void ResetDirty();
    bool IsDirty() const;
    void AccumulateDirtyStatus();
    void RecordCurDirtyStatus();

    // generate filter
    std::shared_ptr<RSFilter> GenerateLightBlurFilter(float radius);
    std::shared_ptr<RSFilter> GenerateMaterialLightBlurFilter(
        std::shared_ptr<Drawing::ColorFilter> colorFilter, uint32_t hash, float radius,
        int colorMode, const RSColor& color);
    void GenerateBackgroundFilter();
    void GenerateForegroundFilter();
    void GenerateBackgroundMaskFilter();
    void GenerateBackgroundBlurFilter();
    void GenerateForegroundBlurFilter();
    void GenerateBackgroundMaterialBlurFilter();
    void GenerateForegroundMaterialBlurFilter();
    void GenerateBackgroundMaterialFuzedBlurFilter();
    void GenerateCompositingMaterialFuzedBlurFilter();
    std::shared_ptr<Drawing::ColorFilter> GetMaterialColorFilter(float sat, float brightness);
    void GenerateAIBarFilter();
    void GenerateAlwaysSnapshotFilter();
    void GenerateWaterRippleFilter();
    void GenerateLinearGradientBlurFilter();
    void GenerateMagnifierFilter();
    void GenerateRenderFilter();
    void GenerateDisplacementDistortFilter();
    void GenerateRenderFilterColorGradient();
    void GenerateSoundWaveFilter();
    void GenerateRenderFilterEdgeLight();
    void GenerateBezierWarpFilter();
    void GenerateRenderFilterDispersion();
    void GenerateForegroundRenderFilter();
    void GenerateContentLightFilter();
    void ComposeNGRenderFilter(
        std::shared_ptr<RSFilter>& originFilter, std::shared_ptr<RSNGRenderFilterBase> filter);

    bool NeedClip() const;
    bool NeedBlurFuzed();
    bool NeedLightBlur(bool disableSystemAdaptation);

    void GenerateRRect();
    RectI GetDirtyRect() const;
    // added for update dirty region dfx
    RectI GetDirtyRect(RectI& drawRegion) const;

    bool isDirty_ = false;
    bool geoDirty_ = false;
    bool contentDirty_ = false;
    bool subTreeAllDirty_ = false;
    bool curIsDirty_ = false;
    bool curGeoDirty_ = false;
    bool curContentDirty_ = false;
    bool curSubTreeAllDirty_ = false;
    bool hasBounds_ = false;
    bool isSpherizeValid_ = false;
    bool clipToBounds_ = false;
    bool clipToFrame_ = false;
    // partial update
    bool colorFilterNeedUpdate_ = false;
    bool pixelStretchNeedUpdate_ = false;
    bool bgShaderNeedUpdate_ = false;
    bool filterNeedUpdate_ = false;
    bool greyCoefNeedUpdate_ = false;
    bool visible_ = true;
    bool isDrawn_ = false;
    bool foregroundEffectDirty_ = false;
    bool needFilter_ = false;
    bool needHwcFilter_ = false;
    bool useEffect_ = false;
    bool enableHDREffect_ = false;
    bool needDrawBehindWindow_ = false;
    bool alphaOffscreen_ = false;
    bool alphaNeedApply_ = false;
    bool systemBarEffect_ = false;
    bool useShadowBatching_ = false;
    bool needSkipShadow_ = false;
    bool distortionEffectDirty_ = false;
    bool haveEffectRegion_ = false;
    bool isAttractionValid_ = false;
    bool bgBlurDisableSystemAdaptation = true;
    bool fgBlurDisableSystemAdaptation = true;
    bool alwaysSnapshot_ = false;
    bool localMagnificationCap_ = false;
    float hdrBrightnessFactor_ = 1.0f; // for displayNode
    float canvasNodeHDRBrightnessFactor_ = 1.0f; // for canvasNode
    float frameOffsetX_ = 0.f;
    float frameOffsetY_ = 0.f;
    float alpha_ = 1.f;
    int useEffectType_ = 0;
    Gravity frameGravity_ = Gravity::DEFAULT;
    int colorBlendMode_ = 0;
    int colorBlendApplyType_ = 0;
    float waterRippleProgress_ = 0.0f;
    float flyOutDegree_ = 0.0f;
    float foregroundEffectRadius_ = 0.f;
    float attractFraction_ = 0.f;
    float spherizeDegree_ = 0.f;
    float hdrUIBrightness_ = 1.0f;
    float lightUpEffectDegree_ = 1.0f;
    // filter property
    float backgroundBlurRadius_ = 0.f;
    float backgroundBlurSaturation_ = 1.f;
    float backgroundBlurBrightness_ = 1.f;
    int backgroundColorMode_ = BLUR_COLOR_MODE::DEFAULT;
    float backgroundBlurRadiusX_ = 0.f;
    float backgroundBlurRadiusY_ = 0.f;
    float foregroundBlurRadius_ = 0.f;
    float foregroundBlurSaturation_ = 1.f;
    float foregroundBlurBrightness_ = 1.f;
    int foregroundColorMode_ = BLUR_COLOR_MODE::DEFAULT;
    float foregroundBlurRadiusX_ = 0.f;
    float foregroundBlurRadiusY_ = 0.f;
    int pixelStretchTileMode_ = 0;
    std::shared_ptr<RSObjAbsGeometry> boundsGeo_;
    std::shared_ptr<RSFilter> foregroundFilter_ = nullptr; // view content filter
    std::shared_ptr<RSFilter> foregroundFilterCache_ = nullptr; // view content filter via cache
    std::shared_ptr<RSRenderFilter> foregroundRenderFilter_ = nullptr;
    std::shared_ptr<RSNGRenderFilterBase> bgNGRenderFilter_ = nullptr;
    std::shared_ptr<RSNGRenderFilterBase> fgNGRenderFilter_ = nullptr;
    std::shared_ptr<RSFilter> backgroundFilter_ = nullptr;
    std::shared_ptr<RSRenderFilter> backgroundRenderFilter_ = nullptr;
    std::shared_ptr<RSFilter> filter_ = nullptr;
    std::shared_ptr<RectF> drawRegion_ = nullptr;
    std::shared_ptr<RSLightSource> lightSourcePtr_ = nullptr;
    std::shared_ptr<RSIlluminated> illuminatedPtr_ = nullptr;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
    std::shared_ptr<MotionBlurParam> motionBlurPara_ = nullptr;
    std::shared_ptr<RSMagnifierParams> magnifierPara_ = nullptr;
    std::shared_ptr<ParticleNoiseFields> particleNoiseFields_ = nullptr;
    std::shared_ptr<RSBorder> border_ = nullptr;
    std::shared_ptr<RSBorder> outline_ = nullptr;
    std::shared_ptr<RSPath> clipPath_ = nullptr;
    std::shared_ptr<RSMask> mask_ = nullptr;
    std::shared_ptr<Drawing::ColorFilter> colorFilter_ = nullptr;
    std::weak_ptr<RSRenderNode> backref_;
    std::unique_ptr<Sandbox> sandbox_ = nullptr;
    Vector2f attractDstPoint_ = {0.f, 0.f};
    RectI attractionEffectCurrentDirtyRegion_ = {0, 0, 0, 0};
    RRect rrect_ = RRect{};
    RSObjGeometry frameGeo_;
    std::optional<Vector4f> pixelStretch_;
    std::optional<Vector4f> pixelStretchPercent_;
    std::optional<Vector4f> cornerRadius_;
    std::optional<RSShadow> shadow_;

    std::shared_ptr<Drawing::RuntimeEffect> shaderEffect_ = nullptr;
    std::optional<std::vector<float>> complexShaderParam_;

    std::optional<RSWaterRipplePara> waterRippleParams_ = std::nullopt;
    std::optional<RSFlyOutPara> flyOutParams_ = std::nullopt;
    std::optional<float> distortionK_ = std::nullopt;
    std::optional<RSDynamicBrightnessPara> fgBrightnessParams_;
    std::optional<RSDynamicBrightnessPara> bgBrightnessParams_;
    std::vector<std::shared_ptr<EmitterUpdater>> emitterUpdater_;
    std::optional<Decoration> decoration_;
    std::optional<Matrix3f> sublayerTransform_;
    Color backgroundMaskColor_ = RSColor();
    Color foregroundMaskColor_ = RSColor();

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

    void UpdateFilter();
    void UpdateForegroundFilter();
    void UpdateBackgroundShader();

    Drawing::Matrix prevAbsMatrix_;
    RSRenderParticleVector particles_;

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    void CreateFilterCacheManagerIfNeed();
    std::unique_ptr<RSFilterCacheManager> backgroundFilterCacheManager_;
    std::unique_ptr<RSFilterCacheManager> foregroundFilterCacheManager_;
    static bool filterCacheEnabled_;
#endif
    static bool blurAdaptiveAdjustEnabled_;
    static const bool IS_UNI_RENDER;
    static const bool FOREGROUND_FILTER_ENABLED;

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
#ifdef RS_ENABLE_GPU
    friend class DrawableV2::RSFilterDrawable;
#endif
#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H
