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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_EFFECT_PROPERTIES_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_EFFECT_PROPERTIES_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "animation/rs_particle_noise_field.h"
#include "animation/rs_render_particle.h"
#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "effect/color_filter.h"
#include "property/rs_properties_def.h"
#include "property/rs_spatial_effect_def.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_shadow.h"
#include "utils/point.h"

namespace OHOS {
namespace Rosen {
class RSNGRenderFilterBase;
class RSNGRenderShaderBase;
class RSPath;

struct FilterEffectParams {
    // material filter
    std::shared_ptr<RSFilter> materialFilter_ = nullptr;
    std::shared_ptr<RSNGRenderFilterBase> mtNGRenderFilter_ = nullptr; // for material filter render

    // background filter
    std::shared_ptr<RSNGRenderFilterBase> bgNGRenderFilter_ = nullptr; // for background render

    std::unique_ptr<RSBackgroundBlurPara> backgroundBlurPara_ = nullptr;
    bool bgBlurDisableSystemAdaptation = true;
    bool alwaysSnapshot_ = false;
    bool systemBarEffect_ = false;
    float waterRippleProgress_ = 0.0f;
    std::optional<RSWaterRipplePara> waterRippleParams_ = std::nullopt;

    // compositing filter
    std::shared_ptr<RSNGRenderFilterBase> cgNGRenderFilter_ = nullptr; // for compositing render

    std::unique_ptr<RSForegroundBlurPara> foregroundBlurPara_ = nullptr;
    bool fgBlurDisableSystemAdaptation = true;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;

    // foreground filter
    std::shared_ptr<RSNGRenderFilterBase> fgNGRenderFilter_ = nullptr; // for foreground render

    float foregroundEffectRadius_ = 0.f;
    std::shared_ptr<MotionBlurParam> motionBlurPara_ = nullptr;
    std::unique_ptr<RSDistortionPara> distortionPara_ = nullptr;
    bool isAttractionValid_ = false;
    float attractFraction_ = 0.f;
    Vector2f attractDstPoint_ = { 0.f, 0.f };
    RectI attractionEffectCurrentDirtyRegion_ = { 0, 0, 0, 0 };
    bool colorAdaptive_ = false; // @brief If true, would adapt foreground to contrast background color.
    float spherizeDegree_ = 0.f;
    float flyOutDegree_ = 0.0f;
    std::optional<RSFlyOutPara> flyOutParams_ = std::nullopt;
    std::optional<RSShadow> shadow_;

    // color filter
    std::shared_ptr<Drawing::ColorFilter> colorFilter_ = nullptr;
    std::optional<float> grayScale_;
    std::optional<float> brightness_;
    std::optional<float> contrast_;
    std::optional<float> saturate_;
    std::optional<float> sepia_;
    std::optional<float> invert_;
    std::optional<float> hueRotate_;
    std::optional<Vector4f> aiInvert_;

    float lightUpEffectDegree_ = 1.0f;
    std::optional<float> dynamicDimDegree_;

    // pixel stretch
    std::unique_ptr<RSPixelStretchPara> pixelStretchPara_ = nullptr;

    // common
    bool greyCoefNeedUpdate_ = false;
    std::optional<Vector2f> greyCoef_;
};

struct ShaderEffectParams {
    std::shared_ptr<ParticleNoiseFields> particleNoiseFields_ = nullptr;
    std::vector<std::shared_ptr<EmitterUpdater>> emitterUpdater_;
    RSRenderParticleVector particles_;
    std::optional<std::vector<float>> complexShaderParam_;
    std::shared_ptr<RSLightSource> lightSourcePtr_ = nullptr;
    std::shared_ptr<RSIlluminated> illuminatedPtr_ = nullptr;
    std::shared_ptr<RSNGRenderShaderBase> bgNGRenderShader_ = nullptr;
    std::shared_ptr<RSNGRenderShaderBase> fgRenderShader_ = nullptr;
    std::shared_ptr<RSNGRenderShaderBase> coRenderShader_ = nullptr; // for coverage shader
    std::shared_ptr<RSNGRenderShaderBase> mtRenderShader_ = nullptr; // for material shader
    std::shared_ptr<RSNGRenderShaderBase> olRenderShader_ = nullptr; // for overlay shader
};

struct BlenderEffectParams {
    std::optional<RSShadowBlenderPara> shadowBlenderParams_;
    std::optional<RSHdrDarkenBlenderPara> hdrDarkenBlenderParams_;
    std::unique_ptr<RSColorfulBrightnessBlenderPara> colorfulBrightnessBlenderParams_ = nullptr;
    std::optional<RSDynamicBrightnessPara> fgBrightnessParams_;
    int colorBlendMode_ = 0;
    int colorBlendApplyType_ = 0;
    std::optional<Color> colorBlend_;
    std::unique_ptr<RSDynamicLightUpPara> dynamicLightUpPara_ = nullptr;
};

struct NodesEffectParams {
    bool haveEffectRegion_ = false;
    bool useEffect_ = false;
    int useEffectType_ = 0;

    bool needDrawBehindWindow_ = false;

    bool useShadowBatching_ = false;

    bool useUnion_ = false;
    float unionSpacing_ = 0.f;
};

struct DepthEffectParams {
    std::shared_ptr<RSImage> depthImage_ = nullptr;
    std::optional<DepthCameraPara> depthCameraPara_ = std::nullopt;
    std::optional<DepthLightPara> depthLightPara_ = std::nullopt;
    std::optional<Matrix3f> depthImageMatrix_ = std::nullopt;
    std::optional<SpatialEffectVariantPara> spatialEffectVariantPara_ = std::nullopt;
    std::optional<std::vector<Drawing::Point>> spatialEffectDstPoints_ = std::nullopt;
};

class RSB_EXPORT RSEffectProperties final {
public:
    RSEffectProperties() = default;
    ~RSEffectProperties() = default;
    RSEffectProperties(const RSEffectProperties&) = delete;
    RSEffectProperties(RSEffectProperties&&) = delete;
    RSEffectProperties& operator=(const RSEffectProperties&) = delete;
    RSEffectProperties& operator=(RSEffectProperties&&) = delete;

    const FilterEffectParams* GetFilterEffect() const
    {
        return filterEffect_.get();
    }
    const ShaderEffectParams* GetShaderEffect() const
    {
        return shaderEffect_.get();
    }
    const BlenderEffectParams* GetBlenderEffect() const
    {
        return blenderEffect_.get();
    }
    const NodesEffectParams* GetNodesEffect() const
    {
        return nodesEffect_.get();
    }
    const DepthEffectParams* GetDepthEffect() const
    {
        return depthEffect_.get();
    }

    /**
     * @brief Non-const overloads create the object with memory allocation if it doesn't exist.
     * Use them only when the params must exist for modification; for conditional reads prefer
     * the const overloads to avoid unnecessary memory allocation.
     */
    FilterEffectParams& GetFilterEffect()
    {
        if (filterEffect_ == nullptr) {
            filterEffect_ = std::make_unique<FilterEffectParams>();
        }
        return *filterEffect_;
    }
    ShaderEffectParams& GetShaderEffect()
    {
        if (shaderEffect_ == nullptr) {
            shaderEffect_ = std::make_unique<ShaderEffectParams>();
        }
        return *shaderEffect_;
    }
    BlenderEffectParams& GetBlenderEffect()
    {
        if (blenderEffect_ == nullptr) {
            blenderEffect_ = std::make_unique<BlenderEffectParams>();
        }
        return *blenderEffect_;
    }
    NodesEffectParams& GetNodesEffect()
    {
        if (nodesEffect_ == nullptr) {
            nodesEffect_ = std::make_unique<NodesEffectParams>();
        }
        return *nodesEffect_;
    }
    DepthEffectParams& GetDepthEffect()
    {
        if (depthEffect_ == nullptr) {
            depthEffect_ = std::make_unique<DepthEffectParams>();
        }
        return *depthEffect_;
    }

    // filter effect params
    // materialFilter_
    const std::shared_ptr<RSFilter>& GetMaterialFilter() const;
    void SetMaterialFilter(const std::shared_ptr<RSFilter>& filter);
    void GenerateMaterialFilter();

    // mtNGRenderFilter_
    void SetMaterialNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filter);
    std::shared_ptr<RSNGRenderFilterBase> GetMaterialNGFilter() const;

    // bgNGRenderFilter_
    void SetBackgroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filter);
    std::shared_ptr<RSNGRenderFilterBase> GetBackgroundNGFilter() const;

    // backgroundBlurPara_
    void SetBackgroundBlurRadius(float radius);
    float GetBackgroundBlurRadius() const;
    bool IsBackgroundBlurRadiusValid() const;
    void SetBackgroundBlurSaturation(float saturation);
    float GetBackgroundBlurSaturation() const;
    bool IsBackgroundBlurSaturationValid() const;
    void SetBackgroundBlurBrightness(float brightness);
    float GetBackgroundBlurBrightness() const;
    bool IsBackgroundBlurBrightnessValid() const;
    void SetBackgroundBlurMaskColor(Color maskColor);
    const Color& GetBackgroundBlurMaskColor() const;
    bool IsBackgroundBlurMaskColorValid() const;
    void SetBackgroundBlurColorMode(int colorMode);
    int GetBackgroundBlurColorMode() const;
    void SetBackgroundBlurRadiusX(float radiusX);
    float GetBackgroundBlurRadiusX() const;
    bool IsBackgroundBlurRadiusXValid() const;
    void SetBackgroundBlurRadiusY(float radiusY);
    float GetBackgroundBlurRadiusY() const;
    bool IsBackgroundBlurRadiusYValid() const;
    bool IsBackgroundMaterialFilterValid() const;

    // bgBlurDisableSystemAdaptation
    void SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation);
    bool GetBgBlurDisableSystemAdaptation() const;

    // alwaysSnapshot_
    void SetAlwaysSnapshot(bool enable);
    bool GetAlwaysSnapshot() const;

    // systemBarEffect_
    void SetSystemBarEffect(bool systemBarEffect);
    bool GetSystemBarEffect() const;

    // waterRippleProgress_
    void SetWaterRippleProgress(const float& progress);
    float GetWaterRippleProgress() const;

    // waterRippleParams_
    void SetWaterRippleParams(const std::optional<RSWaterRipplePara>& params);
    std::optional<RSWaterRipplePara> GetWaterRippleParams() const;
    bool IsWaterRippleValid() const;

    // cgNGRenderFilter_
    void SetCompositingNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filter);
    std::shared_ptr<RSNGRenderFilterBase> GetCompositingNGFilter() const;

    // foregroundBlurPara_
    void SetForegroundBlurRadius(float radius);
    float GetForegroundBlurRadius() const;
    bool IsForegroundBlurRadiusValid() const;
    void SetForegroundBlurSaturation(float saturation);
    float GetForegroundBlurSaturation() const;
    bool IsForegroundBlurSaturationValid() const;
    void SetForegroundBlurBrightness(float brightness);
    float GetForegroundBlurBrightness() const;
    bool IsForegroundBlurBrightnessValid() const;
    void SetForegroundBlurMaskColor(Color maskColor);
    const Color& GetForegroundBlurMaskColor() const;
    bool IsForegroundBlurMaskColorValid() const;
    void SetForegroundBlurColorMode(int colorMode);
    int GetForegroundBlurColorMode() const;
    void SetForegroundBlurRadiusX(float radiusX);
    float GetForegroundBlurRadiusX() const;
    bool IsForegroundBlurRadiusXValid() const;
    void SetForegroundBlurRadiusY(float radiusY);
    float GetForegroundBlurRadiusY() const;
    bool IsForegroundBlurRadiusYValid() const;
    bool IsForegroundMaterialFilterValid() const;

    // fgBlurDisableSystemAdaptation
    void SetFgBlurDisableSystemAdaptation(bool disableSystemAdaptation);
    bool GetFgBlurDisableSystemAdaptation() const;

    // linearGradientBlurPara_
    void SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para);
    const std::shared_ptr<RSLinearGradientBlurPara>& GetLinearGradientBlurPara() const;
    void IfLinearGradientBlurInvalid();

    // fgNGRenderFilter_
    void SetForegroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& filter);
    std::shared_ptr<RSNGRenderFilterBase> GetForegroundNGFilter() const;

    // foregroundEffectRadius_
    void SetForegroundEffectRadius(float foregroundEffectRadius);
    float GetForegroundEffectRadius() const;
    bool IsForegroundEffectRadiusValid() const;

    // motionBlurPara_
    void SetMotionBlurPara(const std::shared_ptr<MotionBlurParam>& para);
    const std::shared_ptr<MotionBlurParam>& GetMotionBlurPara() const;

    // distortionPara_
    void SetDistortionK(const std::optional<float>& distortionK);
    std::optional<float> GetDistortionK() const;
    void SetDistortionDirty(bool distortionEffectDirty);
    bool GetDistortionDirty() const;
    bool IsDistortionKValid() const;

    // isAttractionValid_ + attractFraction_
    bool SetAttractionFraction(float fraction); // returns whether attraction is valid
    float GetAttractionFraction() const;
    bool IsAttractionValid() const;

    // attractDstPoint_
    void SetAttractionDstPoint(Vector2f dstPoint);
    Vector2f GetAttractionDstPoint() const;

    // attractionEffectCurrentDirtyRegion_
    RectI GetAttractionEffectCurrentDirtyRegion() const;
    void SetAttractionEffectCurrentDirtyRegion(const RectI& dirtyRegion);

    // colorAdaptive_
    bool GetColorAdaptive() const;
    void SetColorAdaptive(bool value);

    // spherizeDegree_
    void SetSpherize(float spherizeDegree);
    float GetSpherize() const;
    bool IsSpherizeValid() const;

    // flyOutDegree_
    void SetFlyOutDegree(const float& degree);
    float GetFlyOutDegree() const;

    // flyOutParams_
    void SetFlyOutParams(const std::optional<RSFlyOutPara>& params);
    std::optional<RSFlyOutPara> GetFlyOutParams() const;
    bool IsFlyOutValid() const;

    // shadow_
    void SetShadowColor(Color color);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowElevation(float elevation);
    void SetShadowRadius(float radius);
    void SetShadowPath(std::shared_ptr<RSPath> shadowPath);
    void SetShadowMask(int shadowMask);
    void SetShadowIsFilled(bool shadowIsFilled);
    void SetShadowColorStrategy(int shadowColorStrategy);
    void SetShadowDisableSDFBlur(bool disable);
    const std::optional<RSShadow>& GetShadow() const;
    bool IsShadowMaskValid() const;
    bool IsShadowValid() const;

    // colorFilter_
    const std::shared_ptr<Drawing::ColorFilter>& GetColorFilter() const;
    bool GenerateColorFilter(); // returns true if a color filter was generated from color params

    // grayScale_
    void SetGrayScale(const std::optional<float>& grayScale);
    const std::optional<float>& GetGrayScale() const;

    // brightness_
    void SetBrightness(const std::optional<float>& brightness);
    const std::optional<float>& GetBrightness() const;

    // contrast_
    void SetContrast(const std::optional<float>& contrast);
    const std::optional<float>& GetContrast() const;

    // saturate_
    void SetSaturate(const std::optional<float>& saturate);
    const std::optional<float>& GetSaturate() const;

    // sepia_
    void SetSepia(const std::optional<float>& sepia);
    const std::optional<float>& GetSepia() const;

    // invert_
    void SetInvert(const std::optional<float>& invert);
    const std::optional<float>& GetInvert() const;

    // hueRotate_
    void SetHueRotate(const std::optional<float>& hueRotate);
    const std::optional<float>& GetHueRotate() const;

    // aiInvert_
    void SetAiInvert(const std::optional<Vector4f>& aiInvert);
    const std::optional<Vector4f>& GetAiInvert() const;

    // lightUpEffectDegree_
    void SetLightUpEffect(float lightUpEffectDegree);
    float GetLightUpEffect() const;
    bool IsLightUpEffectValid() const;

    // dynamicDimDegree_
    void SetDynamicDimDegree(const std::optional<float>& dimDegree);
    const std::optional<float>& GetDynamicDimDegree() const;
    bool IsDynamicDimValid() const;

    // pixelStretchPara_
    void SetPixelStretch(const std::optional<Vector4f>& stretchSize);
    void SetPixelStretchPercent(const std::optional<Vector4f>& stretchPercent);
    void SetPixelStretchTileMode(int tileMode);
    int GetPixelStretchTileMode() const;
    Vector4f GetPixelStretch() const;
    Vector4f GetPixelStretchPercent() const;
    // converts pixel stretch percent to size with the given bounds, needs existing stretch para
    void ConvertPixelStretchPercentToSize(float width, float height);
    void ResetPixelStretchPara();

    // greyCoefNeedUpdate_
    bool IsGreyCoefNeedUpdate() const;
    void SetGreyCoefNeedUpdate(bool needUpdate);

    // greyCoef_
    void SetGreyCoef(const std::optional<Vector2f>& greyCoef);
    const std::optional<Vector2f>& GetGreyCoef() const;
    void CheckGreyCoef();

    // shader effect params
    // particleNoiseFields_
    void SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para);
    const std::shared_ptr<ParticleNoiseFields>& GetParticleNoiseFields() const;

    // emitterUpdater_
    void SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para);
    const std::vector<std::shared_ptr<EmitterUpdater>>& GetEmitterUpdater() const;

    // particles_
    void SetParticles(const RSRenderParticleVector& particles);
    const RSRenderParticleVector& GetParticles() const;

    // complexShaderParam_
    void SetComplexShaderParam(const std::vector<float>& param);
    std::optional<std::vector<float>> GetComplexShaderParam() const;

    // lightSourcePtr_
    const std::shared_ptr<RSLightSource>& GetLightSource() const;
    bool SetLightIntensity(float lightIntensity);
    void SetLightColor(Color lightColor);
    void SetLightPosition(const Vector4f& lightPosition);

    // illuminatedPtr_
    const std::shared_ptr<RSIlluminated>& GetIlluminated() const;
    void SetIlluminatedBorderWidth(float illuminatedBorderWidth);
    void SetIlluminatedType(int illuminatedType);
    void SetBloom(float bloomIntensity);

    // bgNGRenderShader_
    void SetBackgroundNGShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    std::shared_ptr<RSNGRenderShaderBase> GetBackgroundNGShader() const;

    // fgRenderShader_
    void SetForegroundShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    std::shared_ptr<RSNGRenderShaderBase> GetForegroundShader() const;

    // coRenderShader_
    void SetCoverageNGShader(const std::shared_ptr<RSNGRenderShaderBase>& coverageShader);
    std::shared_ptr<RSNGRenderShaderBase> GetCoverageNGShader() const;

    // mtRenderShader_
    void SetMaterialShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    std::shared_ptr<RSNGRenderShaderBase> GetMaterialShader() const;

    // olRenderShader_
    void SetOverlayNGShader(const std::shared_ptr<RSNGRenderShaderBase>& overlayShader);
    std::shared_ptr<RSNGRenderShaderBase> GetOverlayNGShader() const;

    // blender effect params
    // shadowBlenderParams_
    void SetShadowBlenderParams(const std::optional<RSShadowBlenderPara>& params);
    std::optional<RSShadowBlenderPara> GetShadowBlenderParams() const;
    bool IsShadowBlenderValid() const;
    std::string GetShadowBlenderDescription() const;

    // hdrDarkenBlenderParams_
    void SetHdrDarkenBlenderParams(const std::optional<RSHdrDarkenBlenderPara>& params);
    std::optional<RSHdrDarkenBlenderPara> GetHdrDarkenBlenderParams() const;
    bool IsHdrDarkenBlenderValid() const;
    std::string GetHdrDarkenBlenderDescription() const;

    // colorfulBrightnessBlenderParams_
    void SetColorfulBrightnessBlenderParams(std::unique_ptr<RSColorfulBrightnessBlenderPara> params);
    const RSColorfulBrightnessBlenderPara* GetColorfulBrightnessBlenderParams() const;
    bool IsColorfulBrightnessBlenderValid() const;
    std::string GetColorfulBrightnessBlenderDescription() const;

    template <typename T>
    void SetColorfulBrightnessBlenderMember(T RSColorfulBrightnessBlenderPara::* member, const T& val)
    {
        auto& blender = GetBlenderEffect();
        if (!blender.colorfulBrightnessBlenderParams_) {
            blender.colorfulBrightnessBlenderParams_ = std::make_unique<RSColorfulBrightnessBlenderPara>();
        }
        (*blender.colorfulBrightnessBlenderParams_).*member = val;
    }

    // fgBrightnessParams_
    void SetFgBrightnessRates(const Vector4f& rates);
    void SetFgBrightnessSaturation(const float& saturation);
    void SetFgBrightnessPosCoeff(const Vector4f& coeff);
    void SetFgBrightnessNegCoeff(const Vector4f& coeff);
    void SetFgBrightnessFract(const float& fraction);
    void SetFgBrightnessHdr(bool enableHdr);
    void SetFgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params);
    std::optional<RSDynamicBrightnessPara> GetFgBrightnessParams() const;
    bool IsFgBrightnessValid() const;
    bool GetFgBrightnessEnableEDR() const;
    std::string GetFgBrightnessDescription() const;

    // colorBlendMode_
    void SetColorBlendMode(int colorBlendMode);
    int GetColorBlendMode() const;

    // colorBlendApplyType_
    void SetColorBlendApplyType(int colorBlendApplyType);
    int GetColorBlendApplyType() const;

    // colorBlend_
    void SetColorBlend(const std::optional<Color>& colorBlend);
    const std::optional<Color>& GetColorBlend() const;

    // dynamicLightUpPara_
    void SetDynamicLightUpRate(const std::optional<float>& rate);
    void SetDynamicLightUpDegree(const std::optional<float>& degree);
    float GetDynamicLightUpRate() const;
    float GetDynamicLightUpDegree() const;
    bool IsDynamicLightUpValid() const;

    // nodes effect params
    // haveEffectRegion_
    void SetHaveEffectRegion(bool haveEffectRegion);
    bool GetHaveEffectRegion() const;

    // useEffect_
    void SetUseEffect(bool useEffect);
    bool GetUseEffect() const;

    // useEffectType_
    void SetUseEffectType(int useEffectType);
    int GetUseEffectType() const;

    // needDrawBehindWindow_
    void SetNeedDrawBehindWindow(bool needDrawBehindWindow);
    bool GetNeedDrawBehindWindow() const;

    // useShadowBatching_
    void SetUseShadowBatching(bool useShadowBatching);
    bool GetUseShadowBatching() const;

    // useUnion_
    void SetUseUnion(bool useUnion);
    bool GetUseUnion() const;

    // unionSpacing_
    void SetUnionSpacing(float spacing);
    float GetUnionSpacing() const;

    // depth and spatial effect params
    // depthImage_
    void SetDepthImage(const std::shared_ptr<RSImage>& depthImage);
    std::shared_ptr<RSImage> GetDepthImage() const;

    // depthCameraPara_
    void SetDepthCameraPara(const DepthCameraPara& depthCameraPara);
    std::optional<DepthCameraPara> GetDepthCameraPara() const;

    // depthLightPara_
    void SetDepthLightPara(const DepthLightPara& depthLightPara);
    std::optional<DepthLightPara> GetDepthLightPara() const;

    // depthImageMatrix_
    void SetDepthImageMatrix(const Matrix3f& imageMatrix);
    std::optional<Matrix3f> GetDepthImageMatrix() const;

    // spatialEffectVariantPara_
    void SetSpatialEffectDepth(float depth);
    void SetSpatialEffectLeftTop(const Vector3f& leftTop);
    void SetSpatialEffectRightTop(const Vector3f& rightTop);
    void SetSpatialEffectLeftBottom(const Vector3f& leftBottom);
    void SetSpatialEffectRightBottom(const Vector3f& rightBottom);
    void SetSpatialEffectOcclusionWeight(float occlusionWeight);
    void SetDepthEffectPara(const std::optional<DepthEffectPara>& depthEffectPara);
    std::optional<DepthEffectPara> GetDepthEffectPara() const;
    void SetSpatialEffectPara(const std::optional<SpatialEffectPara>& spatialEffectPara);
    std::optional<SpatialEffectPara> GetSpatialEffectPara() const;
    void SetSpatialEffectMode(int mode);
    void SetSpatialEffectVariantPara(const std::optional<SpatialEffectVariantPara>& spatialEffectVariantPara);
    std::optional<SpatialEffectVariantPara> GetSpatialEffectVariantPara() const;
    bool GetSpatialEffectOcclusionEnabled() const;
    void SetSpatialEffectXyzCornerPoints(const SpatialEffectPara::CornerPositions& xyzCornerPoints);

    // spatialEffectDstPoints_
    void SetSpatialEffectDstPoints(const std::optional<std::vector<Drawing::Point>>& dstPoints);
    std::optional<std::vector<Drawing::Point>> GetSpatialEffectDstPoints() const;

private:
    void GenerateColorFilterByGrayScale();
    void GenerateColorFilterByBrightness();
    void GenerateColorFilterByContrast();
    void GenerateColorFilterBySaturate();
    void GenerateColorFilterBySepia();
    void GenerateColorFilterByInvert();
    void GenerateColorFilterByHueRotate();
    void GenerateColorFilterByColorBlend();

    std::unique_ptr<FilterEffectParams> filterEffect_ = nullptr;
    std::unique_ptr<ShaderEffectParams> shaderEffect_ = nullptr;
    std::unique_ptr<BlenderEffectParams> blenderEffect_ = nullptr;
    std::unique_ptr<NodesEffectParams> nodesEffect_ = nullptr;
    std::unique_ptr<DepthEffectParams> depthEffect_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_EFFECT_PROPERTIES_H
