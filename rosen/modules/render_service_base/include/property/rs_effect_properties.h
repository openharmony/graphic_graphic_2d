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
class RSShader;

struct FilterEffectParams {
    // material filter
    std::shared_ptr<RSFilter> materialFilter_ = nullptr;
    std::shared_ptr<RSNGRenderFilterBase> mtNGRenderFilter_ = nullptr; // for material filter render

    // background filter
    std::shared_ptr<RSFilter> backgroundFilter_ = nullptr;
    std::shared_ptr<RSNGRenderFilterBase> bgNGRenderFilter_ = nullptr; // for background render

    std::unique_ptr<RSBackgroundBlurPara> backgroundBlurPara_ = nullptr;
    bool bgBlurDisableSystemAdaptation = true;
    bool alwaysSnapshot_ = false;
    bool systemBarEffect_ = false;
    float waterRippleProgress_ = 0.0f;
    std::optional<RSWaterRipplePara> waterRippleParams_ = std::nullopt;

    // compositing filter
    std::shared_ptr<RSFilter> compositingFilter_ = nullptr;
    std::shared_ptr<RSNGRenderFilterBase> cgNGRenderFilter_ = nullptr; // for compositing render

    std::unique_ptr<RSCompositingBlurPara> compositingBlurPara_ = nullptr;
    bool ctBlurDisableSystemAdaptation = true;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;

    // foreground filter
    std::shared_ptr<RSFilter> foregroundFilter_ = nullptr;
    std::shared_ptr<RSFilter> foregroundFilterCache_ = nullptr;
    std::shared_ptr<RSNGRenderFilterBase> fgNGRenderFilter_ = nullptr; // for foreground render

    float foregroundEffectRadius_ = 0.f;
    bool foregroundEffectDirty_ = false;
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
    std::optional<Color> colorBlend_;

    float lightUpEffectDegree_ = 1.0f;
    std::optional<float> dynamicDimDegree_;

    // pixel stretch
    std::unique_ptr<RSPixelStretchPara> pixelStretchPara_ = nullptr;

    // common
    bool greyCoefNeedUpdate_ = false;
    std::optional<Vector2f> greyCoef_;

    static bool blurAdaptiveAdjustEnabled_;
};

struct ShaderEffectParams {
    std::shared_ptr<ParticleNoiseFields> particleNoiseFields_ = nullptr;
    std::shared_ptr<ParticleRippleFields> particleRippleFields_ = nullptr;
    std::shared_ptr<ParticleVelocityFields> particleVelocityFields_ = nullptr;
    std::shared_ptr<ParticleFieldCollection> particleFields_ = nullptr;
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
    std::optional<RSDynamicBrightnessPara> bgBrightnessParams_;
    int colorBlendMode_ = 0;
    int colorBlendApplyType_ = 0;
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
    int uniModeUC_ = 0; // 1 GravityPull Mode, 0 SmoothUnion.
    float gravityPullStrength_ = 0.0f;
    float gravityHotZone_ = 0.0f;
    bool isGravityPullModeCenter_ = false; // true, current node is gravity pull center
};

struct DepthEffectParams {
    std::shared_ptr<RSImage> depthImage_ = nullptr;
    std::optional<DepthCameraPara> depthCameraPara_ = std::nullopt;
    std::optional<DepthLightPara> depthLightPara_ = std::nullopt;
    std::optional<Matrix3f> depthImageMatrix_ = std::nullopt;
    std::optional<SpatialEffectVariantPara> spatialEffectVariantPara_ = std::nullopt;
    std::optional<std::vector<Drawing::Point>> spatialEffectDstPoints_ = std::nullopt;
};

// Geometry and render-mode inputs for foreground filter generation, provided by RSProperties.
struct ForegroundFilterParams {
    bool needGenFgFilterCache = false;
    float hdrUIBrightness = 1.0f;
    Vector3f rotationAngle;
    float canvasWidth = 0.f;     // bounds rect width, used by attraction effect
    float canvasHeight = 0.f;    // bounds rect height, used by attraction effect
    float windowLeftPoint = 0.f; // frame position x, used by attraction effect
    float windowTopPoint = 0.f;  // frame position y, used by attraction effect
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

    // backgroundFilter_
    void SetBackgroundFilter(const std::shared_ptr<RSFilter>& filter);
    const std::shared_ptr<RSFilter>& GetBackgroundFilter() const;

    // foregroundFilter_
    void SetForegroundFilter(const std::shared_ptr<RSFilter>& filter);
    const std::shared_ptr<RSFilter>& GetForegroundFilter() const;

    // foregroundFilterCache_
    void SetForegroundFilterCache(const std::shared_ptr<RSFilter>& cache);
    const std::shared_ptr<RSFilter>& GetForegroundFilterCache() const;

    // compositingFilter_
    void SetCompositingFilter(const std::shared_ptr<RSFilter>& filter);
    const std::shared_ptr<RSFilter>& GetCompositingFilter() const;

    // blur adaptive adjustment (blurAdaptiveAdjustEnabled_)
    bool NeedBlurFuzed() const;
    bool NeedLightBlur(bool disableSystemAdaptation) const;

    static std::shared_ptr<Drawing::ColorFilter> GetMaterialColorFilter(float sat, float brightness);
    std::shared_ptr<RSFilter> GenerateLightBlurFilter(float radius);
    std::shared_ptr<RSFilter> GenerateMaterialLightBlurFilter(std::shared_ptr<Drawing::ColorFilter> colorFilter,
        uint32_t hash, float radius, int colorMode, const RSColor& color);

    // background filter generation, results are stored in filterEffect_
    void GenerateBackgroundBlurFilter();
    bool GenerateBackgroundMaterialBlurFilter();
    void GenerateBackgroundMaterialFuzedBlurFilter();
    void GenerateAIBarFilter();
    void GenerateWaterRippleFilter();
    void GenerateAlwaysSnapshotFilter();

    // compositing filter generation, results are stored in filterEffect_
    void GenerateCompositingBlurFilter();
    bool GenerateCompositingMaterialBlurFilter();
    void GenerateCompositingMaterialFuzedBlurFilter();
    void GenerateLinearGradientBlurFilter(float width, float height);

    // top-level generation orchestration, geometry inputs are provided by RSProperties
    void ComposeNGRenderFilter(std::shared_ptr<RSFilter>& originFilter, std::shared_ptr<RSNGRenderFilterBase> filter,
        const Vector3f& rotationAngle);
    // return true when FASTAVERAGE blur color mode is normalized to AVERAGE during generation,
    // the caller (RSProperties) shall set dirty flags to keep legacy behavior
    bool GenerateBackgroundFilter(const Vector3f& rotationAngle);
    bool GenerateCompositingFilter(const Vector3f& rotationAngle, float frameWidth, float frameHeight);
    void GenerateForegroundFilter(const ForegroundFilterParams& params);

    // foreground filter factories, geometry and render-mode inputs are provided by RSProperties
    void CreateFlyOutShaderFilter();
    void CreateSphereEffectFilter(bool needGenFgFilterCache);
    void CreateHDRUIBrightnessFilter(float hdrUIBrightness, bool needGenFgFilterCache);
    void CreateAttractionEffectFilter(float canvasWidth, float canvasHeight,
        float windowLeftPoint, float windowTopPoint);

    // background shader update, decoration-owned shader and geometry inputs are provided by RSProperties
    void UpdateBackgroundShader(const std::shared_ptr<RSShader>& bgShader, float bgShaderProgress,
        const RectF& boundsRect, const Vector3f& rotationAngle, float cornerRadius);

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

    // compositingBlurPara_
    void SetCompositingBlurRadius(float radius);
    float GetCompositingBlurRadius() const;
    bool IsCompositingBlurRadiusValid() const;
    void SetCompositingBlurSaturation(float saturation);
    float GetCompositingBlurSaturation() const;
    bool IsCompositingBlurSaturationValid() const;
    void SetCompositingBlurBrightness(float brightness);
    float GetCompositingBlurBrightness() const;
    bool IsCompositingBlurBrightnessValid() const;
    void SetCompositingBlurMaskColor(Color maskColor);
    const Color& GetCompositingBlurMaskColor() const;
    bool IsCompositingBlurMaskColorValid() const;
    void SetCompositingBlurColorMode(int colorMode);
    int GetCompositingBlurColorMode() const;
    void SetCompositingBlurRadiusX(float radiusX);
    float GetCompositingBlurRadiusX() const;
    bool IsCompositingBlurRadiusXValid() const;
    void SetCompositingBlurRadiusY(float radiusY);
    float GetCompositingBlurRadiusY() const;
    bool IsCompositingBlurRadiusYValid() const;
    bool IsForegroundMaterialFilterValid() const;

    // ctBlurDisableSystemAdaptation
    void SetCtBlurDisableSystemAdaptation(bool disableSystemAdaptation);
    bool GetCtBlurDisableSystemAdaptation() const;

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

    // foregroundEffectDirty_
    void SetForegroundEffectDirty(bool dirty);
    bool GetForegroundEffectDirty() const;

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
    void SetShadowClipType(ClipType clipType);
    // shadow getters, return legacy defaults when shadow_ is not set
    bool GetShadowDisableSDFBlur() const;
    const Color& GetShadowColor() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
    float GetShadowElevation() const;
    float GetShadowRadius() const;
    std::shared_ptr<RSPath> GetShadowPath() const;
    int GetShadowMask() const;
    bool GetShadowIsFilled() const;
    int GetShadowColorStrategy() const;
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

    // colorBlend_
    void SetColorBlend(const std::optional<Color>& colorBlend);
    const std::optional<Color>& GetColorBlend() const;

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

    // converts and validates pixel stretch params, returns true if stretch is valid and node should be drawn
    bool CalculatePixelStretch(float boundsWidth, float boundsHeight);
    RectI GetPixelStretchDirtyRect(const RectI& dirtyRect) const;

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

    // particleRippleFields_
    void SetParticleRippleFields(const std::shared_ptr<ParticleRippleFields>& para);
    const std::shared_ptr<ParticleRippleFields>& GetParticleRippleFields() const;

    // particleVelocityFields_
    void SetParticleVelocityFields(const std::shared_ptr<ParticleVelocityFields>& para);
    const std::shared_ptr<ParticleVelocityFields>& GetParticleVelocityFields() const;

    // particleFields_
    void SetParticleFields(const std::shared_ptr<ParticleFieldCollection>& para);
    const std::shared_ptr<ParticleFieldCollection>& GetParticleFields() const;

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
    Vector4f GetFgBrightnessRates() const;
    float GetFgBrightnessSaturation() const;
    Vector4f GetFgBrightnessPosCoeff() const;
    Vector4f GetFgBrightnessNegCoeff() const;
    float GetFgBrightnessFract() const;
    bool GetFgBrightnessHdr() const;

    // bgBrightnessParams_
    void SetBgBrightnessRates(const Vector4f& rates);
    void SetBgBrightnessSaturation(const float& saturation);
    void SetBgBrightnessPosCoeff(const Vector4f& coeff);
    void SetBgBrightnessNegCoeff(const Vector4f& coeff);
    void SetBgBrightnessFract(const float& fraction);
    void SetBgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params);
    std::optional<RSDynamicBrightnessPara> GetBgBrightnessParams() const;
    std::string GetBgBrightnessDescription() const;
    Vector4f GetBgBrightnessRates() const;
    float GetBgBrightnessSaturation() const;
    Vector4f GetBgBrightnessPosCoeff() const;
    Vector4f GetBgBrightnessNegCoeff() const;
    float GetBgBrightnessFract() const;
    bool IsBgBrightnessValid() const;

    // colorBlendMode_
    void SetColorBlendMode(int colorBlendMode);
    int GetColorBlendMode() const;
    bool IsColorBlendModeValid() const;
    bool IsColorBlendModeNone() const;

    // colorBlendApplyType_
    void SetColorBlendApplyType(int colorBlendApplyType);
    int GetColorBlendApplyType() const;
    bool IsColorBlendApplyTypeOffscreen() const;

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

    // uniModeUC_
    void SetSDFUnionMode(int uniModeUC);
    int GetSDFUnionMode() const;

    // gravityPullStrength_
    void SetGravityPullStrength(float gravityPullStrength);
    float GetGravityPullStrength() const;

    // gravityHotZone_
    void SetGravityHotZone(float hotZone);
    float GetGravityHotZone() const;

    // isGravityPullModeCenter_
    void SetGravityPullCenterFlag(bool isGravityPullModeCenter);
    bool GetGravityPullCenterFlag() const;

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

    // generation helpers split from the top-level generation functions
    void GenerateBackgroundFuzedBlurFilter();
    void GenerateCompositingFuzedBlurFilter();
    void ComposeBackgroundMaterialBlurFilter(std::shared_ptr<Drawing::ColorFilter> colorFilter,
        std::shared_ptr<Drawing::ImageFilter> blurColorFilter, uint32_t hash);
    void ComposeCompositingMaterialBlurFilter(std::shared_ptr<Drawing::ColorFilter> colorFilter,
        std::shared_ptr<Drawing::ImageFilter> blurColorFilter, uint32_t hash);
    // handles motion blur, foreground effect, spherize, fly-out and attraction branches,
    // returns true when one of them generated the foreground filter
    bool GenerateForegroundEffectFilter(const ForegroundFilterParams& params);

    std::unique_ptr<FilterEffectParams> filterEffect_ = nullptr;
    std::unique_ptr<ShaderEffectParams> shaderEffect_ = nullptr;
    std::unique_ptr<BlenderEffectParams> blenderEffect_ = nullptr;
    std::unique_ptr<NodesEffectParams> nodesEffect_ = nullptr;
    std::unique_ptr<DepthEffectParams> depthEffect_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_EFFECT_PROPERTIES_H
