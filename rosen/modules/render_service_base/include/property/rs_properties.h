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
#include "property/rs_effect_properties.h"
#include "property/rs_properties_def.h"
#include "property/rs_spatial_effect_def.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_filter_cache_manager.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
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
class ParticleFieldCollection;
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
namespace Drawing {
struct GECameraIntrinsics;
struct GECameraExtrinsics;
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
    void SetBorderSDFShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    Vector4<Color> GetBorderColor() const;
    Vector4f GetBorderWidth() const;
    Vector4<uint32_t> GetBorderStyle() const;
    Vector4f GetBorderDashWidth() const;
    Vector4f GetBorderDashGap() const;
    std::shared_ptr<RSNGRenderShaderBase> GetBorderSDFShader() const;
    const std::shared_ptr<RSBorder>& GetBorder() const;
    void SetOutlineColor(Vector4<Color> color);
    void SetOutlineWidth(Vector4f width);
    void SetOutlineStyle(Vector4<uint32_t> style);
    void SetOutlineDashWidth(const Vector4f& dashWidth);
    void SetOutlineDashGap(const Vector4f& dashGap);
    void SetOutlineRadius(Vector4f radius);
    void SetOutlineSDFShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    Vector4<Color> GetOutlineColor() const;
    Vector4f GetOutlineWidth() const;
    Vector4<uint32_t> GetOutlineStyle() const;
    Vector4f GetOutlineDashWidth() const;
    Vector4f GetOutlineDashGap() const;
    Vector4f GetOutlineRadius() const;
    std::shared_ptr<RSNGRenderShaderBase> GetOutlineSDFShader() const;
    const std::shared_ptr<RSBorder>& GetOutline() const
    {
        return outline_;
    }

    void InternalSetSDFShape(const std::shared_ptr<RSNGRenderShapeBase>& shape);
    void SetSDFShape(const std::shared_ptr<RSNGRenderShapeBase>& shape);
    std::shared_ptr<RSNGRenderShapeBase> GetSDFShape() const;
    bool IsSDFDistortShape() const;

    // setter and getter of color picker related properties
    void SetColorPickerPlaceholder(int placeholder);
    void SetColorPickerStrategy(int strategy);
    void SetColorPickerInterval(int interval);
    void SetColorPickerNotifyThreshold(int packedThresholds); // packed: lower 16 bits = dark, upper 16 bits = light
    void SetColorPickerRect(const Vector4f& rect); // [left, top, right, bottom]
    void SetLastContrastColorScheme(ContrastColorScheme colorScheme);
    std::shared_ptr<ColorPickerParam> GetColorPicker() const;

    bool DisableHWCForFilter() const;
    bool NeedClipHoleForRenderGroup() const;
    bool NeedFilter() const;
    bool NeedDisabledPartialRender() const;
    bool NeedHwcFilter() const;
    bool NeedSkipSubtreeParallel() const;

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
        return clipRRect_ && !clipRRect_->rect_.IsEmpty();
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
    void SetDoubleSidedEnabled(bool isDoubleSided);
    bool GetDoubleSidedEnabled() const;
    void SetVisible(bool visible);
    bool GetVisible() const
    {
        return visible_;
    }
    std::string Dump() const;

    void SetMask(const std::shared_ptr<RSMask>& mask);
    std::shared_ptr<RSMask> GetMask() const;

    const std::shared_ptr<RSObjAbsGeometry>& GetBoundsGeometry() const
    {
        return boundsGeo_;
    }
    const RSObjGeometry& GetFrameGeometry() const;
    bool UpdateGeometry(const RSProperties* parent, bool dirtyFlag, const std::optional<Drawing::Point>& offset);
    bool UpdateGeometryByParent(const Drawing::Matrix* parentMatrix, const std::optional<Drawing::Point>& offset);
    RectF GetLocalBoundsAndFramesRect() const;
    RectF GetBoundsRect() const;
    NodeId GetRenderNodeId() const;

    bool IsDirty() const;
    bool IsGeoDirty() const;
    bool IsCurGeoDirty() const;
    bool IsContentDirty() const;
    bool IsSubTreeAllDirty() const;

    void SetHDRUIBrightness(float hdrUIBrightness);
    float GetHDRUIBrightness() const;
    bool IsHDRUIBrightnessValid() const;

    void SetHDRColorHeadroom(float headroom);
    float GetHDRColorHeadroom() const;
    float GetHDRColorMaxHeadroom() const;
    bool HDRColorHeadroomEnabled() const;
    void UpdateHDRColorMaxHeadroom(float hdrColorHeadroom, float backgroundColorHeadroom);

    RRect GetRRect() const;
    RRect GetInnerRRect() const;
    RectF GetFrameRect() const;

    void ResetBorder(bool isOutline);

    RRect GetRRectForSDF() const;
    void SetGeoDirty();

    // ---------------------------- RSEffectProperties ---------------------------- //
    bool HasHarmonium() const;
    bool HasSpatialGlassEffect() const;
    bool HasGlassEffect() const;

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    const std::unique_ptr<RSFilterCacheManager>& GetFilterCacheManager(bool isForeground) const;
    void ClearFilterCache();
#endif

    static void SetFilterCacheEnabledByCCM(bool isCCMFilterCacheEnable);

    // ---------------------------- filter effect delegations ---------------------------- //
    // material filter
    const std::shared_ptr<RSFilter>& GetMaterialFilter() const;

    void SetMaterialNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetMaterialNGFilter() const;

    // background filter
    void SetBackgroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetBackgroundNGFilter() const;
    const std::shared_ptr<RSFilter>& GetBackgroundFilter() const
    {
        return GetEffectProperties().GetBackgroundFilter();
    }

    void SetBackgroundBlurRadius(float backgroundBlurRadius);
    float GetBackgroundBlurRadius() const;
    void SetBackgroundBlurSaturation(float backgroundBlurSaturation);
    float GetBackgroundBlurSaturation() const;
    void SetBackgroundBlurBrightness(float backgroundBlurBrightness);
    float GetBackgroundBlurBrightness() const;
    void SetBackgroundBlurMaskColor(Color backgroundMaskColor);
    const Color& GetBackgroundBlurMaskColor() const;
    void SetBackgroundBlurColorMode(int backgroundColorMode);
    int GetBackgroundBlurColorMode() const;
    void SetBackgroundBlurRadiusX(float backgroundBlurRadiusX);
    float GetBackgroundBlurRadiusX() const;
    void SetBackgroundBlurRadiusY(float backgroundBlurRadiusY);
    float GetBackgroundBlurRadiusY() const;

    void SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation);
    bool GetBgBlurDisableSystemAdaptation() const;

    void SetAlwaysSnapshot(bool enable);
    bool GetAlwaysSnapshot() const;

    void SetSystemBarEffect(bool systemBarEffect);
    bool GetSystemBarEffect() const;

    void SetWaterRippleProgress(const float& progress);
    float GetWaterRippleProgress() const;

    void SetWaterRippleParams(const std::optional<RSWaterRipplePara>& params);
    std::optional<RSWaterRipplePara> GetWaterRippleParams() const;
    bool IsWaterRippleValid() const;

    // compositing filter
    void SetCompositingNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetCompositingNGFilter() const;

    const std::shared_ptr<RSFilter>& GetCompositingFilter() const
    {
        return GetEffectProperties().GetCompositingFilter();
    }

    void SetCompositingBlurRadius(float compositingBlurRadius);
    float GetCompositingBlurRadius() const;
    void SetCompositingBlurSaturation(float compositingBlurSaturation);
    float GetCompositingBlurSaturation() const;
    void SetCompositingBlurBrightness(float compositingBlurBrightness);
    float GetCompositingBlurBrightness() const;
    void SetCompositingBlurMaskColor(Color compositingMaskColor);
    const Color& GetCompositingBlurMaskColor() const;
    void SetCompositingBlurColorMode(int compositingColorMode);
    int GetCompositingBlurColorMode() const;
    void SetCompositingBlurRadiusX(float compositingBlurRadiusX);
    float GetCompositingBlurRadiusX() const;
    void SetCompositingBlurRadiusY(float compositingBlurRadiusY);
    float GetCompositingBlurRadiusY() const;

    void SetCtBlurDisableSystemAdaptation(bool disableSystemAdaptation);
    bool GetCtBlurDisableSystemAdaptation() const;

    void SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para);
    const std::shared_ptr<RSLinearGradientBlurPara>& GetLinearGradientBlurPara() const;
    void IfLinearGradientBlurInvalid();

    // foreground filter
    void SetForegroundNGFilter(const std::shared_ptr<RSNGRenderFilterBase>& renderFilter);
    std::shared_ptr<RSNGRenderFilterBase> GetForegroundNGFilter() const;

    void SetForegroundFilter(const std::shared_ptr<RSFilter>& foregroundFilter);
    const std::shared_ptr<RSFilter>& GetForegroundFilter() const
    {
        return GetEffectProperties().GetForegroundFilter();
    }

    void SetForegroundFilterCache(const std::shared_ptr<RSFilter>& foregroundFilterCache);
    const std::shared_ptr<RSFilter>& GetForegroundFilterCache() const
    {
        return GetEffectProperties().GetForegroundFilterCache();
    }

    void SetForegroundEffectRadius(const float foregroundEffectRadius);
    float GetForegroundEffectRadius() const;

    void SetForegroundEffectDirty(bool dirty);
    bool GetForegroundEffectDirty() const;

    void SetMotionBlurPara(const std::shared_ptr<MotionBlurParam>& para);
    const std::shared_ptr<MotionBlurParam>& GetMotionBlurPara() const;

    void SetDistortionK(const std::optional<float>& distortionK);
    std::optional<float> GetDistortionK() const
    {
        return GetEffectProperties().GetDistortionK();
    }
    void SetDistortionDirty(bool distortionEffectDirty);
    bool GetDistortionDirty() const;
    bool IsDistortionKValid() const;

    void SetAttractionFraction(float fraction);
    float GetAttractionFraction() const;
    bool IsAttractionValid() const
    {
        return GetEffectProperties().IsAttractionValid();
    }

    void SetAttractionDstPoint(Vector2f dstPoint);
    Vector2f GetAttractionDstPoint() const;

    RectI GetAttractionEffectCurrentDirtyRegion() const
    {
        return GetEffectProperties().GetAttractionEffectCurrentDirtyRegion();
    }

    bool GetColorAdaptive() const;
    void SetAdaptive(bool value);

    void SetSpherize(float spherizeDegree);
    float GetSpherize() const;
    bool IsSpherizeValid() const;

    void SetFlyOutDegree(const float& degree);
    float GetFlyOutDegree() const;

    void SetFlyOutParams(const std::optional<RSFlyOutPara>& params);
    std::optional<RSFlyOutPara> GetFlyOutParams() const;
    bool IsFlyOutValid() const;

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
    void SetShadowDisableSDFBlur(bool disable);
    const std::optional<RSShadow>& GetShadow() const;
    bool IsShadowMaskValid() const;
    bool IsShadowValid() const;
    const Color& GetShadowColor() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
    float GetShadowElevation() const;
    float GetShadowRadius() const;
    std::shared_ptr<RSPath> GetShadowPath() const;
    int GetShadowMask() const;
    bool GetShadowIsFilled() const;
    int GetShadowColorStrategy() const;
    bool GetShadowDisableSDFBlur() const;

    const std::shared_ptr<Drawing::ColorFilter>& GetColorFilter() const;

    // color filter
    void SetGrayScale(const std::optional<float>& grayScale);
    const std::optional<float>& GetGrayScale() const;

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

    void SetAiInvert(const std::optional<Vector4f>& aiInvert);
    const std::optional<Vector4f>& GetAiInvert() const;

    void SetColorBlend(const std::optional<Color>& colorBlend);
    const std::optional<Color>& GetColorBlend() const;

    void SetLightUpEffect(float lightUpEffectDegree);
    float GetLightUpEffect() const;
    bool IsLightUpEffectValid() const;

    void SetDynamicDimDegree(const std::optional<float>& DimDegree);
    const std::optional<float>& GetDynamicDimDegree() const;
    bool IsDynamicDimValid() const;

    // Pixel Stretch
    void SetPixelStretch(const std::optional<Vector4f>& stretchSize);
    void SetPixelStretchPercent(const std::optional<Vector4f>& stretchPercent);
    void SetPixelStretchTileMode(int tileMode);
    int GetPixelStretchTileMode() const;
    inline const Vector4f GetPixelStretch() const
    {
        return GetEffectProperties().GetPixelStretch();
    }
    inline const Vector4f GetPixelStretchPercent() const
    {
        return GetEffectProperties().GetPixelStretchPercent();
    }

    RectI GetPixelStretchDirtyRect() const;

    bool IsGreyCoefNeedUpdate() const
    {
        return GetEffectProperties().IsGreyCoefNeedUpdate();
    }

    // common
    void SetGreyCoef(const std::optional<Vector2f>& greyCoef);
    const std::optional<Vector2f>& GetGreyCoef() const;
    void OnApplyModifiers();
    static void SetBlurAdaptiveAdjustEnabledByCCM(bool isCCMBlurAdaptiveAdjustEnabled);

    // ---------------------------- shader effect delegations ---------------------------- //
    void SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para);
    const std::shared_ptr<ParticleNoiseFields>& GetParticleNoiseFields() const;

    void SetParticleRippleFields(const std::shared_ptr<ParticleRippleFields>& para);
    const std::shared_ptr<ParticleRippleFields>& GetParticleRippleFields() const;

    void SetParticleVelocityFields(const std::shared_ptr<ParticleVelocityFields>& para);
    const std::shared_ptr<ParticleVelocityFields>& GetParticleVelocityFields() const;

    void SetParticleFields(const std::shared_ptr<ParticleFieldCollection>& para);
    const std::shared_ptr<ParticleFieldCollection>& GetParticleFields() const;

    void SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para);
    const std::vector<std::shared_ptr<EmitterUpdater>>& GetEmitterUpdater() const;

    // particle properties
    void SetParticles(const RSRenderParticleVector& particles);
    const RSRenderParticleVector& GetParticles() const;

    void SetComplexShaderParam(const std::vector<float>& param);
    std::optional<std::vector<float>> GetComplexShaderParam() const;

    inline const std::shared_ptr<RSLightSource>& GetLightSource() const
    {
        return GetEffectProperties().GetLightSource();
    }
    void SetLightIntensity(float lightIntensity);
    void SetLightColor(Color lightColor);
    void SetLightPosition(const Vector4f& lightPosition);
    float GetLightIntensity() const;
    Color GetLightColor() const;
    Vector4f GetLightPosition() const;

    inline const std::shared_ptr<RSIlluminated>& GetIlluminated() const
    {
        return GetEffectProperties().GetIlluminated();
    }
    void SetIlluminatedBorderWidth(float illuminatedBorderWidth);
    void SetIlluminatedType(int illuminatedType);
    void SetBloom(float bloomIntensity);
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

    void SetBackgroundNGShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    std::shared_ptr<RSNGRenderShaderBase> GetBackgroundNGShader() const;

    void SetForegroundShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    std::shared_ptr<RSNGRenderShaderBase> GetForegroundShader() const;

    void SetCoverageNGShader(const std::shared_ptr<RSNGRenderShaderBase>& coverageShader);
    std::shared_ptr<RSNGRenderShaderBase> GetCoverageNGShader() const;

    void SetMaterialShader(const std::shared_ptr<RSNGRenderShaderBase>& renderShader);
    std::shared_ptr<RSNGRenderShaderBase> GetMaterialShader() const;

    void SetOverlayNGShader(const std::shared_ptr<RSNGRenderShaderBase>& overlayShader);
    std::shared_ptr<RSNGRenderShaderBase> GetOverlayNGShader() const;

    // ---------------------------- blender effect delegations ---------------------------- //
    void SetShadowBlenderParams(const std::optional<RSShadowBlenderPara>& params);
    std::optional<RSShadowBlenderPara> GetShadowBlenderParams() const;
    bool IsShadowBlenderValid() const;
    std::string GetShadowBlenderDescription() const;

    void SetHdrDarkenBlenderParams(const std::optional<RSHdrDarkenBlenderPara>& params);
    std::optional<RSHdrDarkenBlenderPara> GetHdrDarkenBlenderParams() const;
    bool IsHdrDarkenBlenderValid() const;
    std::string GetHdrDarkenBlenderDescription() const;

    void SetColorfulBrightnessBlenderParams(std::unique_ptr<RSColorfulBrightnessBlenderPara> params);
    const RSColorfulBrightnessBlenderPara* GetColorfulBrightnessBlenderParams() const;
    bool IsColorfulBrightnessBlenderValid() const;
    std::string GetColorfulBrightnessBlenderDescription() const;
    template <typename T>
    void SetColorfulBrightnessBlenderMember(T RSColorfulBrightnessBlenderPara::* member, const T& val)
    {
        GetEffectProperties().SetColorfulBrightnessBlenderMember(member, val);
        isDrawn_ = true;
        SetDirty();
        contentDirty_ = true;
    }

    void SetFgBrightnessRates(const Vector4f& rates);
    void SetFgBrightnessSaturation(const float& saturation);
    void SetFgBrightnessPosCoeff(const Vector4f& coeff);
    void SetFgBrightnessNegCoeff(const Vector4f& coeff);
    void SetFgBrightnessFract(const float& fraction);
    void SetFgBrightnessHdr(const bool enableHdr);
    void SetFgBrightnessParams(const std::optional<RSDynamicBrightnessPara>& params);
    std::optional<RSDynamicBrightnessPara> GetFgBrightnessParams() const;
    bool IsFgBrightnessValid() const
    {
        return GetEffectProperties().IsFgBrightnessValid();
    }
    bool GetFgBrightnessEnableEDR() const;
    std::string GetFgBrightnessDescription() const;
    Vector4f GetFgBrightnessRates() const;
    float GetFgBrightnessSaturation() const;
    Vector4f GetFgBrightnessPosCoeff() const;
    Vector4f GetFgBrightnessNegCoeff() const;
    float GetFgBrightnessFract() const;
    bool GetFgBrightnessHdr() const;
    bool IsFgBlenderEffectValid() const;

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
    bool IsBgBrightnessValid() const
    {
        return GetEffectProperties().IsBgBrightnessValid();
    }

    void SetColorBlendMode(int colorBlendMode);
    int GetColorBlendMode() const
    {
        return GetEffectProperties().GetColorBlendMode();
    }
    bool IsColorBlendModeValid() const
    {
        return GetEffectProperties().IsColorBlendModeValid();
    }
    bool IsColorBlendModeNone() const
    {
        return GetEffectProperties().IsColorBlendModeNone();
    }

    void SetColorBlendApplyType(int colorBlendApplyType);
    int GetColorBlendApplyType() const
    {
        return GetEffectProperties().GetColorBlendApplyType();
    }
    bool IsColorBlendApplyTypeOffscreen() const
    {
        return GetEffectProperties().IsColorBlendApplyTypeOffscreen();
    }

    void SetDynamicLightUpRate(const std::optional<float>& rate);
    void SetDynamicLightUpDegree(const std::optional<float>& degree);
    float GetDynamicLightUpRate() const;
    float GetDynamicLightUpDegree() const;
    bool IsDynamicLightUpValid() const;

    // ---------------------------- nodes effect delegations ---------------------------- //
    void SetHaveEffectRegion(bool hasEffectRegion);
    bool GetHaveEffectRegion() const;

    void SetUseEffect(bool useEffect);
    bool GetUseEffect() const;

    void SetUseEffectType(int useEffectType);
    int GetUseEffectType() const;

    void SetNeedDrawBehindWindow(bool needDrawBehindWindow);
    bool GetNeedDrawBehindWindow() const
    {
        return GetEffectProperties().GetNeedDrawBehindWindow();
    }

    void SetUseShadowBatching(bool useShadowBatching);
    inline bool GetUseShadowBatching() const
    {
        return GetEffectProperties().GetUseShadowBatching();
    }

    void SetUseUnion(bool useUnion);
    bool GetUseUnion() const;

    void SetUnionSpacing(float spacing);
    float GetUnionSpacing() const;

    void SetSDFUnionMode(int uniModeUC);
    int GetSDFUnionMode() const;

    void SetGravityPullStrength(float gravityPullStrength);
    float GetGravityPullStrength() const;

    void SetGravityHotZone(float hotZone);
    float GetGravityHotZone() const;

    void SetGravityPullCenterFlag(bool isGravityPullModeCenter);
    bool GetGravityPullCenterFlag() const;

    // ---------------------------- depth and spatial effect delegations ---------------------------- //
    void SetDepthImage(const std::shared_ptr<RSImage>& depthImage);
    std::shared_ptr<RSImage> GetDepthImage() const;
    void SetDepthCameraPara(const DepthCameraPara& depthCameraPara);
    std::optional<DepthCameraPara> GetDepthCameraPara() const;
    void SetDepthLightPara(const DepthLightPara& depthLightPara);
    std::optional<DepthLightPara> GetDepthLightPara() const;
    void SetDepthImageMatrix(const Matrix3f& imageMatrix);
    std::optional<Matrix3f> GetDepthImageMatrix() const;

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
    /**
     * @brief Parameters used for computing spatial effect matrices.
     *
     * @param rectSelf      Bounds of the current node.
     * @param depNodeRect   Bounds of the depth node.
     * @param cornerPoints  Target 3D positions after SpatialEffect transformation (World Space).
     */
    struct SpatialEffectMatrixParams {
        RectF rectSelf;
        RectF depNodeRect;
        std::array<Vector3f, 4> cornerPoints;
        SpatialEffectMode spatialEffectMode = SpatialEffectMode::WORLD_XYZ_MODE;
    };

    struct SpatialEffectPerspectiveResults {
        Drawing::Matrix transformMatrix;
        std::array<Drawing::Point, 4> dstPoints;
    };

    void ApplySpatialEffectMatrix();
    static SpatialEffectPerspectiveResults CalculateSpatialEffectMatrix(const SpatialEffectMatrixParams& params,
        const Drawing::GECameraIntrinsics& intrinsics, const Drawing::GECameraExtrinsics& extrinsics);
    static bool CalculateDstPointsByWorldXYZ(SpatialEffectPerspectiveResults& ret,
        const SpatialEffectMatrixParams& params, const Drawing::GECameraIntrinsics& intrinsics,
        const Drawing::GECameraExtrinsics& extrinsics);
    static void CalculateDstPointsByNdcXY(SpatialEffectPerspectiveResults& ret,
        const SpatialEffectMatrixParams& params);
    SpatialEffectPara::CornerPositions CalculateWorldXYZ(const std::array<Vector3f, 4>& uvzPoints,
        const Drawing::GECameraIntrinsics& intrinsics, const Drawing::GECameraExtrinsics& extrinsics);

    void SetSpatialEffectDstPoints(const std::optional<std::vector<Drawing::Point>>& dstPoints);
    std::optional<std::vector<Drawing::Point>> GetSpatialEffectDstPoints() const;
private:
    void SetDirty();
    void ResetDirty();
    void AccumulateDirtyStatus();
    void RecordCurDirtyStatus();

    // generate filter
    void GenerateBackgroundFilter();
    void GenerateCompositingFilter();

    bool NeedClip() const;

    RectI GetDirtyRect() const;
    // added for update dirty region dfx
    RectI GetDirtyRect(RectI& drawRegion) const;

    RSEffectProperties& GetEffectProperties()
    {
        if (effectProperties_ == nullptr) {
            effectProperties_ = std::make_unique<RSEffectProperties>();
        }
        return *effectProperties_;
    }
    const RSEffectProperties& GetEffectProperties() const
    {
        if (effectProperties_ == nullptr) {
            effectProperties_ = std::make_unique<RSEffectProperties>();
        }
        return *effectProperties_;
    }
    mutable std::unique_ptr<RSEffectProperties> effectProperties_;

    std::shared_ptr<RSMask> mask_ = nullptr;

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
    bool isDoubleSided_ = true;
    // partial update
    bool colorFilterNeedUpdate_ = false;
    bool pixelStretchNeedUpdate_ = false;
    bool bgShaderNeedUpdate_ = false;
    bool filterNeedUpdate_ = false;
    bool visible_ = true;
    bool isDrawn_ = false;
    bool needFilter_ = false;
    bool needHwcFilter_ = false;
    bool needForceSubmit_ = false;
    bool hasHarmonium_ = false;
    bool hasSpatialGlassEffect_ = false;
    bool hasGlassEffect_ = false;
    bool alphaOffscreen_ = false;
    std::unique_ptr<RRect> clipRRect_;
    bool alphaNeedApply_ = false;
    bool localMagnificationCap_ = false;
    float hdrBrightnessFactor_ = 1.0f; // for displayNode
    float canvasNodeHDRBrightnessFactor_ = 1.0f; // for canvasNode
    float frameOffsetX_ = 0.f;
    float frameOffsetY_ = 0.f;
    float alpha_ = 1.f;
    Gravity frameGravity_ = Gravity::DEFAULT;
    float hdrUIBrightness_ = 1.0f;
    float hdrColorHeadroom_ = 1.0f;
    float hdrColorMaxHeadroom_ = 1.0f;
    std::shared_ptr<ColorPickerParam> colorPicker_;
    // filter property
    std::shared_ptr<RSObjAbsGeometry> boundsGeo_;
    std::shared_ptr<RSNGRenderShapeBase> renderSDFShape_ = nullptr;
    std::shared_ptr<RectF> drawRegion_ = nullptr;
    std::shared_ptr<RSBorder> border_ = nullptr;
    std::shared_ptr<RSBorder> outline_ = nullptr;
    std::shared_ptr<RSPath> clipPath_ = nullptr;
    std::weak_ptr<RSRenderNode> backref_;
    std::unique_ptr<Sandbox> sandbox_ = nullptr;
    RSObjGeometry frameGeo_;
    std::optional<Vector4f> cornerRadius_;
    int cornerApplyType_ = 0;

    std::unique_ptr<Decoration> decoration_;
    std::optional<Matrix3f> sublayerTransform_;

    std::optional<RectI> lastRect_;

    //Hisysevent params and funcs for GenerateBack/ForegroundFilter and GenerateForegroundFilter
    std::bitset<3> hasReportedServerXXFilterCascade_ = 0b000;
    void StatBackgroundFilter();
    void StatCompositingFilter();
    void StatForegroundFilter();

    // OnApplyModifiers hooks
    void CheckEmptyBounds();
    void GenerateColorFilter();
    void CalculatePixelStretch();
    void CalculateFrameOffset();

    void UpdateFilter();
    void GenerateForegroundFilter();
    void UpdateBackgroundShader();

    void ProcessSpatialEffectDstPoints(const SpatialEffectPerspectiveResults& perspectiveResults,
        const RectF& depNodeRect, const Drawing::Matrix& absMatrixDep,
        const std::shared_ptr<RSRenderNode>& renderNode);

    Drawing::Matrix prevAbsMatrix_;

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    void CreateFilterCacheManagerIfNeed();
    std::unique_ptr<RSFilterCacheManager> backgroundFilterCacheManager_;
    std::unique_ptr<RSFilterCacheManager> foregroundFilterCacheManager_;
    static bool filterCacheEnabled_;
#endif
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
