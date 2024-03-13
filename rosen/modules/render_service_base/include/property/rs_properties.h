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
#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier_type.h"
#include "property/rs_properties_def.h"
#include "property/rs_color_picker_cache_task.h"
#include "render/rs_aibar_filter.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_shadow.h"

#include "property/rs_filter_cache_manager.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;
class RSObjAbsGeometry;
class RSB_EXPORT RSProperties final {
friend class RSBackgroundImageDrawable;
friend class RSBackgroundFilterDrawable;
friend class RSShadowDrawable;
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
#ifdef DDGR_ENABLE_FEATURE_OPINC
    bool GetOpincPropDirty() const;
#endif

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

    void SetAlpha(float alpha);
    float GetAlpha() const;
    void SetAlphaOffscreen(bool alphaOffscreen);
    bool GetAlphaOffscreen() const;

    void SetSublayerTransform(const std::optional<Matrix3f>& sublayerTransform);
    const std::optional<Matrix3f>& GetSublayerTransform() const;

    bool GetUseShadowBatching() const;
    void SetUseShadowBatching(bool useShadowBatching);
    bool GetNeedSkipShadow() const;
    void SetNeedSkipShadow(bool needSkipShadow);

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

    // filter properties
    void SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter);
    void SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para);
    void SetDynamicLightUpRate(const std::optional<float>& rate);
    void SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree);
    void SetFilter(const std::shared_ptr<RSFilter>& filter);
    const std::shared_ptr<RSFilter>& GetBackgroundFilter() const;
    const std::shared_ptr<RSLinearGradientBlurPara>& GetLinearGradientBlurPara() const;
    void IfLinearGradientBlurInvalid();
    const std::shared_ptr<RSFilter>& GetFilter() const;
    bool NeedFilter() const;
    void SetGreyCoef(const std::optional<Vector2f>& greyCoef);
    const std::optional<Vector2f>& GetGreyCoef() const;

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
    const std::optional<Vector4f>& GetPixelStretch() const;
    void SetPixelStretchPercent(const std::optional<Vector4f>& stretchPercent);
    const std::optional<Vector4f>& GetPixelStretchPercent() const;
    void SetAiInvert(const std::optional<Vector4f>& aiInvert);
    const std::optional<Vector4f>& GetAiInvert() const;
    void SetSystemBarEffect(bool systemBarEffect);
    bool GetSystemBarEffect() const;
    RectI GetPixelStretchDirtyRect() const;

    const std::shared_ptr<RSObjAbsGeometry>& GetBoundsGeometry() const;
    const std::shared_ptr<RSObjGeometry>& GetFrameGeometry() const;
    bool UpdateGeometry(const RSProperties* parent, bool dirtyFlag, const std::optional<Drawing::Point>& offset,
        const std::optional<Drawing::Rect>& clipRect);
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

    // Image effect properties
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
    void SetColorBlend(const std::optional<Color>& colorBlend);
    const std::optional<Color>& GetColorBlend() const;

    const std::shared_ptr<Drawing::ColorFilter>& GetColorFilter() const;

    void SetLightIntensity(float lightIntensity);
    void SetLightPosition(const Vector4f& lightPosition);
    void SetIlluminatedBorderWidth(float illuminatedBorderWidth);
    void SetIlluminatedType(int illuminatedType);
    void SetBloom(float bloomIntensity);
    float GetLightIntensity() const;
    Vector4f GetLightPosition() const;
    float GetIlluminatedBorderWidth() const;
    int GetIlluminatedType() const;
    float GetBloom() const;
    void CalculateAbsLightPosition();
    const std::shared_ptr<RSLightSource>& GetLightSource() const;
    const std::shared_ptr<RSIlluminated>& GetIlluminated() const;

    void SetUseEffect(bool useEffect);
    bool GetUseEffect() const;

    void SetColorBlendMode(int colorBlendMode);
    int GetColorBlendMode() const;
    void SetColorBlendApplyType(int colorBlendApplyType);
    int GetColorBlendApplyType() const;

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    const std::unique_ptr<RSFilterCacheManager>& GetFilterCacheManager(bool isForeground) const;
    const std::shared_ptr<RSColorPickerCacheTask>& GetColorPickerCacheTaskShadow() const;
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
    void ResetProperty(const ModifierDirtyTypes& dirtyTypes);
    void SetDirty();
    void ResetDirty();
    bool IsDirty() const;

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
    bool isDrawn_ = false;
    bool alphaNeedApply_ = false;
    bool systemBarEffect_ = false;
#ifdef DDGR_ENABLE_FEATURE_OPINC
    bool isOpincPropDirty_ = false;
#endif

    bool hasBounds_ = false;
    bool useEffect_ = false;
    bool useShadowBatching_ = false;
    bool needSkipShadow_ = false;

    int colorBlendMode_ = 0;
    int colorBlendApplyType_ = 0;

    Gravity frameGravity_ = Gravity::DEFAULT;

    std::shared_ptr<RectF> drawRegion_ = nullptr;

    float alpha_ = 1.f;
    bool alphaOffscreen_ = false;

    std::shared_ptr<RSObjAbsGeometry> boundsGeo_;
    std::shared_ptr<RSObjGeometry> frameGeo_;

    std::shared_ptr<RSLightSource> lightSourcePtr_ = nullptr;
    std::shared_ptr<RSIlluminated> illuminatedPtr_ = nullptr;

    std::shared_ptr<RSFilter> backgroundFilter_ = nullptr;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
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

    std::weak_ptr<RSRenderNode> backref_;

    std::optional<Vector4f> pixelStretch_;
    std::optional<Vector4f> pixelStretchPercent_;
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
    std::optional<Color> colorBlend_;
    std::optional<RectI> lastRect_;
    std::optional<Vector2f> greyCoef_;

    // OnApplyModifiers hooks
    void CheckEmptyBounds();
    void GenerateColorFilter();
    void CalculatePixelStretch();
    void CalculateFrameOffset();
    void CheckGreyCoef();
    void ApplyGreyCoef();

    // partial update
    bool colorFilterNeedUpdate_ = false;
    bool pixelStretchNeedUpdate_ = false;
    bool filterNeedUpdate_ = false;
    bool greyCoefNeedUpdate_ = false;
    float frameOffsetX_ = 0.f;
    float frameOffsetY_ = 0.f;
    bool needFilter_ = false;
    RRect rrect_ = RRect{};

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
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H
