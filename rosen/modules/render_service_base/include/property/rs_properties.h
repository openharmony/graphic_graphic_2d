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

#include "animation/rs_particle_noise_field.h"
#include "animation/rs_render_particle.h"
#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector4.h"
#include "property/rs_properties_def.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_filter_cache_manager.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_magnifier_para.h"
#include "render/rs_mask.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_shadow.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;
class RSObjAbsGeometry;
class RSNGRenderFilterBase;
class ParticleRippleFields;
class ParticleVelocityFields;
struct ColorPickerParam;
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
    Vector4f GetBounds() const
    {
        return { boundsGeo_->GetX(), boundsGeo_->GetY(), boundsGeo_->GetWidth(), boundsGeo_->GetHeight() };
    }
    Vector2f GetBoundsSize() const;
    float GetBoundsWidth() const
    {
        return boundsGeo_->GetWidth();
    }
    float GetBoundsHeight() const
    {
        return boundsGeo_->GetHeight();
    }
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
    float GetFrameWidth() const
    {
        return frameGeo_.GetWidth();
    }
    float GetFrameHeight() const
    {
        return frameGeo_.GetHeight();
    }
    Vector2f GetFramePosition() const;
    float GetFramePositionX() const;
    float GetFramePositionY() const;
    float GetFrameOffsetX() const
    {
        return frameOffsetX_;
    }
    float GetFrameOffsetY() const
    {
        return frameOffsetY_;
    }

    void SetSandBox(const std::optional<Vector2f>& parentPosition);
    std::optional<Vector2f> GetSandBox() const
    {
        return sandbox_ ? sandbox_->position_ : std::nullopt;
    }
    void ResetSandBox();
    void UpdateSandBoxMatrix(const std::optional<Drawing::Matrix>& rootMatrix);
    std::optional<Drawing::Matrix> GetSandBoxMatrix() const;

    void SetPositionZ(float positionZ);
    float GetPositionZ() const
    {
        return boundsGeo_->GetZ();
    }
    void SetPositionZApplicableCamera3D(bool isApplicable);
    bool GetPositionZApplicableCamera3D() const;

    void SetPivot(Vector2f pivot);
    void SetPivotX(float pivotX);
    void SetPivotY(float pivotY);
    void SetPivotZ(float pivotZ);
    Vector2f GetPivot() const
    {
        return { boundsGeo_->GetPivotX(), boundsGeo_->GetPivotY() };
    }
    float GetPivotX() const;
    float GetPivotY() const;
    float GetPivotZ() const;

    void SetCornerRadius(const Vector4f& cornerRadius);
    const Vector4f& GetCornerRadius() const;
    void SetCornerApplyType(int type);
    int GetCornerApplyType() const
    {
        return cornerApplyType_;
    }
    bool NeedCornerOptimization() const;

    void SetQuaternion(Quaternion quaternion);
    void SetRotation(float degree);
    void SetRotationX(float degree);
    void SetRotationY(float degree);
    void SetCameraDistance(float cameraDistance);
    Quaternion GetQuaternion() const
    {
        return boundsGeo_->GetQuaternion();
    }
    float GetRotation() const
    {
        return boundsGeo_->GetRotation();
    }
    float GetRotationX() const
    {
        return boundsGeo_->GetRotationX();
    }
    float GetRotationY() const
    {
        return boundsGeo_->GetRotationY();
    }
    float GetCameraDistance() const;

    void SetTranslate(Vector2f translate);
    void SetTranslateX(float translate);
    void SetTranslateY(float translate);
    void SetTranslateZ(float translate);
    Vector2f GetTranslate() const
    {
        return Vector2f(GetTranslateX(), GetTranslateY());
    }
    float GetTranslateX() const
    {
        return boundsGeo_->GetTranslateX();
    }
    float GetTranslateY() const
    {
        return boundsGeo_->GetTranslateY();
    }
    float GetTranslateZ() const
    {
        return boundsGeo_->GetTranslateZ();
    }

    void SetScale(Vector2f scale);
    void SetScaleX(float sx);
    void SetScaleY(float sy);
    void SetScaleZ(float sz);
    Vector2f GetScale() const
    {
        return { boundsGeo_->GetScaleX(), boundsGeo_->GetScaleY() };
    }
    float GetScaleX() const
    {
        return boundsGeo_->GetScaleX();
    }
    float GetScaleY() const
    {
        return boundsGeo_->GetScaleY();
    }
    float GetScaleZ() const
    {
        return boundsGeo_->GetScaleZ();
    }

    void SetSkew(const Vector3f& skew);
    void SetSkewX(float skewX);
    void SetSkewY(float skewY);
    void SetSkewZ(float skewZ);
    Vector3f GetSkew() const
    {
        return { boundsGeo_->GetSkewX(), boundsGeo_->GetSkewY(), boundsGeo_->GetSkewZ() };
    }
    float GetSkewX() const;
    float GetSkewY() const;
    float GetSkewZ() const;

    void SetPersp(const Vector4f& persp);
    void SetPerspX(float perspX);
    void SetPerspY(float perspY);
    void SetPerspZ(float perspZ);
    void SetPerspW(float perspW);
    Vector4f GetPersp() const
    {
        return { boundsGeo_->GetPerspX(), boundsGeo_->GetPerspY(), boundsGeo_->GetPerspZ(), boundsGeo_->GetPerspW() };
    }
    float GetPerspX() const;
    float GetPerspY() const;
    float GetPerspZ() const;
    float GetPerspW() const;

    void SetAlpha(float alpha);
    float GetAlpha() const
    {
        return alpha_;
    }
    void SetAlphaOffscreen(bool alphaOffscreen);
    bool GetAlphaOffscreen() const;

    void SetLocalMagnificationCap(bool localMagnificationCap);

    void SetSublayerTransform(const std::optional<Matrix3f>& sublayerTransform);
    const std::optional<Matrix3f>& GetSublayerTransform() const;

    inline bool GetUseShadowBatching() const
    {
        if (effect_) {
            return effect_->useShadowBatching_;
        }
        return false;
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

    inline bool GetNeedForceSubmit() const
    {
        return needForceSubmit_;
    }

    void SetHDRBrightnessFactor(float factor);
    float GetHDRBrightnessFactor() const
    {
        return hdrBrightnessFactor_;
    }
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
    const Color& GetBackgroundColor() const
    {
        return decoration_ ? decoration_->backgroundColor_ : RgbPalette::Transparent();
    }
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
    const std::shared_ptr<RSBorder>& GetOutline() const
    {
        return outline_;
    }

    void SetForegroundEffectRadius(const float foregroundEffectRadius);
    float GetForegroundEffectRadius() const;
    bool IsForegroundEffectRadiusValid() const;
    void SetForegroundEffectDirty(bool dirty);
    bool GetForegroundEffectDirty() const;
    void SetForegroundFilterCache(const std::shared_ptr<RSFilter>& foregroundFilterCache);
    const std::shared_ptr<RSFilter>& GetForegroundFilterCache() const
    {
        return foregroundFilterCache_;
    }

    // filter properties
    void SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para);
    void SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para);
    void SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para);
    void SetParticleRippleFields(const std::shared_ptr<ParticleRippleFields>& para);
    void SetParticleVelocityFields(const std::shared_ptr<ParticleVelocityFields>& para);
    void SetDynamicLightUpRate(const std::optional<float>& rate);
    void SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree);
    void SetDynamicDimDegree(const std::optional<float>& DimDegree);

    void SetBackgroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetBackgroundNGFilter() const;

    void SetForegroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetForegroundNGFilter() const;
    void SetBackgroundNGShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    std::shared_ptr<RSNGRenderShaderBase> GetBackgroundNGShader() const;
    void SetForegroundShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    std::shared_ptr<RSNGRenderShaderBase> GetForegroundShader() const;
    void InternalSetSDFShape(const std::shared_ptr<RSNGRenderShapeBase>& shape);
    void SetSDFShape(const std::shared_ptr<RSNGRenderShapeBase>& shape);
    std::shared_ptr<RSNGRenderShapeBase> GetSDFShape() const;
    void SetMaterialNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetMaterialNGFilter() const;
    void SetCompositingNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetCompositingNGFilter() const;

    // setter and getter of color picker related properties
    void SetColorPickerPlaceholder(int placeholder);
    void SetColorPickerStrategy(int strategy);
    void SetColorPickerInterval(int interval);
    void SetColorPickerNotifyThreshold(int packedThresholds); // packed: lower 16 bits = dark, upper 16 bits = light
    void SetColorPickerRect(const Vector4f& rect); // [left, top, right, bottom]
    std::shared_ptr<ColorPickerParam> GetColorPicker() const;

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
    void SetFgBrightnessHdr(const bool enableHdr);
    bool GetFgBrightnessHdr() const;
    void SetFgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params);
    std::optional<RSDynamicBrightnessPara> GetFgBrightnessParams() const;
    bool GetFgBrightnessEnableEDR() const;

    void SetShadowBlenderParams(const std::optional<RSShadowBlenderPara>& params);
    std::optional<RSShadowBlenderPara> GetShadowBlenderParams() const;

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
    const std::optional<float>& GetDistortionK() const
    {
        static const std::optional<float> defaultValue = std::nullopt;
        if (effect_) {
            return effect_->distortionK_;
        }
        return defaultValue;
    }

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
    const std::shared_ptr<RSFilter>& GetBackgroundFilter() const
    {
        return backgroundFilter_;
    }
    const std::shared_ptr<RSLinearGradientBlurPara>& GetLinearGradientBlurPara() const;
    const std::vector<std::shared_ptr<EmitterUpdater>>& GetEmitterUpdater() const;
    const std::shared_ptr<ParticleNoiseFields>& GetParticleNoiseFields() const;
    const std::shared_ptr<ParticleRippleFields>& GetParticleRippleFields() const;
    const std::shared_ptr<ParticleVelocityFields>& GetParticleVelocityFields() const;
    void IfLinearGradientBlurInvalid();
    const std::shared_ptr<RSFilter>& GetFilter() const
    {
        return filter_;
    }
    const std::shared_ptr<RSFilter>& GetMaterialFilter() const
    {
        static const std::shared_ptr<RSFilter> defaultValue = nullptr;
        if (effect_) {
            return effect_->materialFilter_;
        }
        return defaultValue;
    }
    const std::shared_ptr<MotionBlurParam>& GetMotionBlurPara() const;
    const std::shared_ptr<RSMagnifierParams>& GetMagnifierPara() const;
    bool DisableHWCForFilter() const;
    bool NeedFilter() const;
    bool NeedHwcFilter() const;
    bool NeedSkipSubtreeParallel() const;
    void SetGreyCoef(const std::optional<Vector2f>& greyCoef);
    const std::optional<Vector2f>& GetGreyCoef() const;
    bool IsGreyCoefNeedUpdate() const
    {
        if (effect_) {
            return effect_->greyCoefNeedUpdate_;
        }
        return false;
    }
    const std::shared_ptr<RSFilter>& GetForegroundFilter() const
    {
        return foregroundFilter_;
    }
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
    bool IsForegroundMaterialFilterValid() const;
    bool IsBackgroundLightBlurFilterValid() const;
    bool IsForegroundLightBlurFilterValid() const;

    // shadow properties
    void SetShadowColor(Color color);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowElevation(float radius);
    void SetShadowRadius(float radius);
    void SetShadowPath(std::shared_ptr<RSPath> shadowPath);
    void SetShadowMask(int shadowMask);
    void SetShadowIsFilled(bool shadowIsFilled);
    void SetShadowColorStrategy(int shadowColorStrategy);
    const Color& GetShadowColor() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
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
    Gravity GetFrameGravity() const
    {
        return frameGravity_;
    }

    void SetDrawRegion(const std::shared_ptr<RectF>& rect);
    std::shared_ptr<RectF> GetDrawRegion() const
    {
        return drawRegion_;
    }

    void SetClipRRect(RRect clipRRect);
    RRect GetClipRRect() const
    {
        return clipRRect_ ? *clipRRect_ : RRect();
    }
    bool GetClipToRRect() const
    {
        return clipRRect_.has_value() && !clipRRect_->rect_.IsEmpty();
    }
    void SetClipBounds(const std::shared_ptr<RSPath>& path);
    const std::shared_ptr<RSPath>& GetClipBounds() const
    {
        return clipPath_;
    }
    void SetClipToBounds(bool clipToBounds);
    bool GetClipToBounds() const
    {
        return clipToBounds_;
    }
    void SetClipToFrame(bool clipToFrame);
    bool GetClipToFrame() const
    {
        return clipToFrame_;
    }
    void SetVisible(bool visible);
    bool GetVisible() const
    {
        return visible_;
    }
    std::string Dump() const;

    void SetMask(const std::shared_ptr<RSMask>& mask);
    std::shared_ptr<RSMask> GetMask() const;

    // Pixel Stretch
    void SetPixelStretch(const std::optional<Vector4f>& stretchSize);
    inline const std::optional<Vector4f>& GetPixelStretch() const
    {
        static const std::optional<Vector4f> defaultValue = std::nullopt;
        if (effect_) {
            return effect_->pixelStretch_;
        }
        return defaultValue;
    }

    void SetPixelStretchPercent(const std::optional<Vector4f>& stretchPercent);
    inline const std::optional<Vector4f>& GetPixelStretchPercent() const
    {
        static const std::optional<Vector4f> defaultValue = std::nullopt;
        if (effect_) {
            return effect_->pixelStretchPercent_;
        }
        return defaultValue;
    }

    void SetPixelStretchTileMode(int stretchTileMode);
    int GetPixelStretchTileMode() const;

    void SetAiInvert(const std::optional<Vector4f>& aiInvert);
    const std::optional<Vector4f>& GetAiInvert() const;
    void SetSystemBarEffect(bool systemBarEffect);
    bool GetSystemBarEffect() const;
    RectI GetPixelStretchDirtyRect() const;

    const std::shared_ptr<RSObjAbsGeometry>& GetBoundsGeometry() const
    {
        return boundsGeo_;
    }
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

    bool IsAttractionValid() const
    {
        if (effect_) {
            return effect_->isAttractionValid_;
        }
        return false;
    }
    void SetAttractionFraction(float fraction);
    void SetAttractionDstPoint(Vector2f dstPoint);
    float GetAttractionFraction() const;
    Vector2f GetAttractionDstPoint() const;
    void CreateAttractionEffectFilter();
    RectI GetAttractionEffectCurrentDirtyRegion() const
    {
        if (effect_) {
            return effect_->attractionEffectCurrentDirtyRegion_;
        }
        return {0, 0, 0, 0};
    }
    void SetLightUpEffect(float lightUpEffectDegree);
    float GetLightUpEffect() const;
    bool IsLightUpEffectValid() const;
    bool IsDynamicLightUpValid() const;
    bool IsDynamicDimValid() const;
    bool IsFgBrightnessValid() const
    {
        const auto& fgBrightnessParams = GetFgBrightnessParams();
        return fgBrightnessParams.has_value() && fgBrightnessParams->IsValid();
    }

    bool IsBgBrightnessValid() const
    {
        const auto& bgBrightnessParams = GetBgBrightnessParams();
        return bgBrightnessParams.has_value() && bgBrightnessParams->IsValid();
    }
    bool IsShadowBlenderValid() const;
    bool IsWaterRippleValid() const;
    bool IsFlyOutValid() const;
    bool IsDistortionKValid() const;
    void SetDistortionDirty(bool distortionEffectDirty);
    bool GetDistortionDirty() const;
    bool GetMagnifierDirty() const;
    std::string GetFgBrightnessDescription() const;
    std::string GetBgBrightnessDescription() const;
    std::string GetShadowBlenderDescription() const;

    // Image effect properties
    void SetGrayScale(const std::optional<float>& grayScale);
    inline const std::optional<float>& GetGrayScale() const
    {
        static const std::optional<float> defaultValue = std::nullopt;
        if (effect_) {
            return effect_->grayScale_;
        }
        return defaultValue;
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
    bool GetColorAdaptive() const;
    void SetAdaptive(bool value);

    const std::shared_ptr<Drawing::ColorFilter>& GetColorFilter() const
    {
        static const std::shared_ptr<Drawing::ColorFilter> defaultValue = nullptr;
        if (effect_) {
            return effect_->colorFilter_;
        }
        return defaultValue;
    }

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
        const auto& illuminatedPtr = GetIlluminated();
        return illuminatedPtr ? illuminatedPtr->GetIlluminatedBorderWidth() : 0.f;
    }

    int GetIlluminatedType() const;
    inline float GetBloom() const
    {
        const auto& illuminatedPtr = GetIlluminated();
        return illuminatedPtr ? illuminatedPtr->GetBloomIntensity() : 0.f;
    }

    inline const std::shared_ptr<RSLightSource>& GetLightSource() const
    {
        static const std::shared_ptr<RSLightSource> defaultValue = nullptr;
        if (effect_) {
            return effect_->lightSourcePtr_;
        }
        return defaultValue;
    }
    inline const std::shared_ptr<RSIlluminated>& GetIlluminated() const
    {
        static const std::shared_ptr<RSIlluminated> defaultValue = nullptr;
        if (effect_) {
            return effect_->illuminatedPtr_;
        }
        return defaultValue;
    }

    bool HasHarmonium() const;

    void SetUseEffect(bool useEffect);
    bool GetUseEffect() const;
    void SetUseEffectType(int useEffectType);
    int GetUseEffectType() const;
    void SetNeedDrawBehindWindow(bool needDrawBehindWindow);
    bool GetNeedDrawBehindWindow() const
    {
        if (effect_) {
            return effect_->needDrawBehindWindow_;
        }
        return false;
    }

    void SetUseUnion(bool useUnion);
    bool GetUseUnion() const;
    void SetUnionSpacing(float spacing);
    float GetUnionSpacing() const;

    void SetColorBlendMode(int colorBlendMode);
    int GetColorBlendMode() const
    {
        if (effect_) {
            return effect_->colorBlendMode_;
        }
        return 0;
    }
    bool IsColorBlendModeValid() const
    {
        const auto& colorBlendMode = GetColorBlendMode();
        return colorBlendMode != static_cast<int>(RSColorBlendMode::SRC_OVER) &&
               colorBlendMode != static_cast<int>(RSColorBlendMode::NONE);
    }
    bool IsColorBlendModeNone() const
    {
        return GetColorBlendMode() == static_cast<int>(RSColorBlendMode::NONE);
    }

    void SetColorBlendApplyType(int colorBlendApplyType);
    int GetColorBlendApplyType() const
    {
        if (effect_) {
            return effect_->colorBlendApplyType_;
        }
        return 0;
    }
    bool IsColorBlendApplyTypeOffscreen() const
    {
        return GetColorBlendApplyType() != static_cast<int>(RSColorBlendApplyType::FAST);
    }

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
    RRect GetRRectForSDF() const;

private:
    struct CommonEffectParams {
        bool isAttractionValid_ = false;
        float attractFraction_ = 0.f;
        Vector2f attractDstPoint_ = { 0.f, 0.f };
        RectI attractionEffectCurrentDirtyRegion_ = { 0, 0, 0, 0 };
        std::shared_ptr<MotionBlurParam> motionBlurPara_ = nullptr;
        float waterRippleProgress_ = 0.0f;
        std::optional<RSWaterRipplePara> waterRippleParams_ = std::nullopt;
        bool isSpherizeValid_ = false;
        /**
         * @brief If true, would adapt foreground to contrast background color.
         */
        bool colorAdaptive_ = false;
        float spherizeDegree_ = 0.f;
        bool bgBlurDisableSystemAdaptation = true;
        bool fgBlurDisableSystemAdaptation = true;
        bool useEffect_ = false;
        bool haveEffectRegion_ = false;
        std::shared_ptr<ParticleNoiseFields> particleNoiseFields_ = nullptr;
        std::vector<std::shared_ptr<EmitterUpdater>> emitterUpdater_;
        RSRenderParticleVector particles_;
        float lightUpEffectDegree_ = 1.0f;
        std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
        bool greyCoefNeedUpdate_ = false;
        std::optional<Vector2f> greyCoef_;
        float flyOutDegree_ = 0.0f;
        std::optional<RSFlyOutPara> flyOutParams_ = std::nullopt;
        std::shared_ptr<RSMagnifierParams> magnifierPara_ = nullptr;
        std::optional<float> dynamicLightUpRate_;
        std::optional<float> dynamicLightUpDegree_;
        std::optional<float> dynamicDimDegree_;
        bool needDrawBehindWindow_ = false;
        int useEffectType_ = 0;
        std::optional<RSShadowBlenderPara> shadowBlenderParams_;
        std::optional<std::vector<float>> complexShaderParam_;
        int pixelStretchTileMode_ = 0;
        std::optional<Vector4f> pixelStretch_;
        std::optional<Vector4f> pixelStretchPercent_;
        std::shared_ptr<RSMask> mask_ = nullptr;
        std::optional<float> grayScale_;
        std::optional<float> brightness_;
        std::optional<float> contrast_;
        std::optional<float> saturate_;
        std::optional<float> sepia_;
        std::optional<float> invert_;
        std::optional<float> hueRotate_;
        bool distortionEffectDirty_ = false;
        std::optional<float> distortionK_ = std::nullopt;
        std::shared_ptr<Drawing::ColorFilter> colorFilter_ = nullptr;
        bool systemBarEffect_ = false;
        std::optional<Vector4f> aiInvert_;
        std::shared_ptr<RSLightSource> lightSourcePtr_ = nullptr;
        std::shared_ptr<RSIlluminated> illuminatedPtr_ = nullptr;
        bool alwaysSnapshot_ = false;
        bool useShadowBatching_ = false;
        std::optional<RSShadow> shadow_;
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
        Color backgroundMaskColor_ = RSColor();
        Color foregroundMaskColor_ = RSColor();
        std::optional<RSDynamicBrightnessPara> fgBrightnessParams_;
        float foregroundEffectRadius_ = 0.f;
        int colorBlendMode_ = 0;
        int colorBlendApplyType_ = 0;
        std::optional<Color> colorBlend_;
        std::shared_ptr<RSNGRenderFilterBase> bgNGRenderFilter_ = nullptr; // for background render
        std::shared_ptr<RSNGRenderFilterBase> fgNGRenderFilter_ = nullptr; // for foreground render
        std::shared_ptr<RSNGRenderFilterBase> mtNGRenderFilter_ = nullptr; // for material filter render
        std::shared_ptr<RSNGRenderFilterBase> cgNGRenderFilter_ = nullptr; // for compositing render
        std::shared_ptr<RSNGRenderShaderBase> bgNGRenderShader_ = nullptr;
        std::shared_ptr<RSNGRenderShaderBase> fgRenderShader_ = nullptr;
        std::shared_ptr<RSFilter> materialFilter_ = nullptr;
    };
    inline float DecreasePrecision(float value)
    {
        // preserve two digital precision when calculating hash, this can reuse filterCache as much as possible.
        return 0.01 * round(value * 100);
    }
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
    void GenerateMaterialFilter();
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
    void ComposeNGRenderFilter(
        std::shared_ptr<RSFilter>& originFilter, std::shared_ptr<RSNGRenderFilterBase> filter);

    bool NeedClip() const;
    bool NeedBlurFuzed();
    bool NeedLightBlur(bool disableSystemAdaptation);

    void GenerateRRect();
    RectI GetDirtyRect() const;
    // added for update dirty region dfx
    RectI GetDirtyRect(RectI& drawRegion) const;

    std::unique_ptr<CommonEffectParams> effect_;

    /**
     * Use this only when you need to ensure CommonEffectParams exists for modification.
     * Creates the object with memory allocation if it doesn't exist.
     * For conditional modifications within this class, prefer USING WITH_EFFECT macro
     * or GetXXXMember func to avoid unnecessary memory allocation.
     * Examples:
     * ✅ GetEffect().requiredMember = value;        // Must exist, value is meaningful
     * ❌ GetEffect().optionalMem = nullptr;         // May create unnecessarily
     * ❌ GetEffect().optionalMem = std::nullopt;    // May create unnecessarily
     * ❌ GetEffect().optionalMem.reset();           // May create unnecessarily
     * ✅ WITH_EFFECT(optionalMem = nullptr);        // Safe, no allocation
     * ✅ WITH_EFFECT(optionalMem = std::nullopt);   // Safe, no allocation
     * ✅ WITH_EFFECT(optionalMem.reset());          // Safe, no allocation
     */
    CommonEffectParams& GetEffect()
    {
        if (effect_ == nullptr) {
            effect_ = std::make_unique<CommonEffectParams>();
        }
        return *effect_;
    }

    bool isDirty_ = false;
    bool geoDirty_ = false;
    bool contentDirty_ = false;
    bool subTreeAllDirty_ = false;
    bool curIsDirty_ = false;
    bool curGeoDirty_ = false;
    bool curContentDirty_ = false;
    bool curSubTreeAllDirty_ = false;
    bool hasBounds_ = false;
    bool clipToBounds_ = false;
    bool clipToFrame_ = false;
    // partial update
    bool colorFilterNeedUpdate_ = false;
    bool pixelStretchNeedUpdate_ = false;
    bool bgShaderNeedUpdate_ = false;
    bool filterNeedUpdate_ = false;
    bool visible_ = true;
    bool isDrawn_ = false;
    bool foregroundEffectDirty_ = false;
    bool needFilter_ = false;
    bool needHwcFilter_ = false;
    bool needForceSubmit_ = false;
    bool hasHarmonium_ = false;
    bool useUnion_ = false;
    bool alphaOffscreen_ = false;
    std::optional<RRect> clipRRect_;
    bool alphaNeedApply_ = false;
    bool needSkipShadow_ = false;
    bool localMagnificationCap_ = false;
    float hdrBrightnessFactor_ = 1.0f; // for displayNode
    float canvasNodeHDRBrightnessFactor_ = 1.0f; // for canvasNode
    float frameOffsetX_ = 0.f;
    float frameOffsetY_ = 0.f;
    float alpha_ = 1.f;
    float unionSpacing_ = 0.f;
    Gravity frameGravity_ = Gravity::DEFAULT;
    float hdrUIBrightness_ = 1.0f;
    std::shared_ptr<ColorPickerParam> colorPicker_;
    // filter property
    std::shared_ptr<RSObjAbsGeometry> boundsGeo_;
    std::shared_ptr<RSNGRenderShapeBase> renderSDFShape_ = nullptr;
    std::shared_ptr<RectF> drawRegion_ = nullptr;
    std::shared_ptr<ParticleRippleFields> particleRippleFields_ = nullptr;
    std::shared_ptr<ParticleVelocityFields> particleVelocityFields_ = nullptr;
    std::shared_ptr<RSBorder> border_ = nullptr;
    std::shared_ptr<RSBorder> outline_ = nullptr;
    std::shared_ptr<RSPath> clipPath_ = nullptr;
    std::shared_ptr<RSFilter> backgroundFilter_ = nullptr;
    std::shared_ptr<RSFilter> filter_ = nullptr;
    std::shared_ptr<RSFilter> foregroundFilter_ = nullptr;
    std::shared_ptr<RSFilter> foregroundFilterCache_ = nullptr;
    std::weak_ptr<RSRenderNode> backref_;
    std::unique_ptr<Sandbox> sandbox_ = nullptr;
    RRect rrect_ = RRect{};
    RSObjGeometry frameGeo_;
    std::optional<Vector4f> cornerRadius_;
    std::optional<RSDynamicBrightnessPara> bgBrightnessParams_;
    int cornerApplyType_ = 0;

    std::optional<Decoration> decoration_;
    std::optional<Matrix3f> sublayerTransform_;

    std::optional<RectI> lastRect_;

    //Hisysevent params and funcs for GenerateBack/ForegroundFilter and UpdateForegroundFilter
    std::bitset<3> hasReportedServerXXFilterCascade_ = 0b000;
    void StatBackgroundFilter();
    void StatCompositingFilter();
    void StatForegroundFilter();
    
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
