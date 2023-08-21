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

#include <optional>
#include <tuple>
#include <vector>

#include "include/effects/SkColorMatrix.h"

#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "animation/rs_render_particle.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier_type.h"
#include "property/rs_properties_def.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_shadow.h"

#ifndef USE_ROSEN_DRAWING
#include "property/rs_filter_cache_manager.h"
#endif

namespace OHOS {
namespace Rosen {
class RSRenderNode;
class RSObjAbsGeometry;
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
#ifndef USE_ROSEN_DRAWING
    void UpdateSandBoxMatrix(const std::optional<SkMatrix>& rootMatrix);
    std::optional<SkMatrix> GetSandBoxMatrix() const;
#else
    void UpdateSandBoxMatrix(const std::optional<Drawing::Matrix>& rootMatrix);
    std::optional<Drawing::Matrix> GetSandBoxMatrix() const;
#endif

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

    void SetCornerRadius(Vector4f cornerRadius);
    Vector4f GetCornerRadius() const;

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

    void SetAlpha(float alpha);
    float GetAlpha() const;
    void SetAlphaOffscreen(bool alphaOffscreen);
    bool GetAlphaOffscreen() const;

    void SetSublayerTransform(const std::optional<Matrix3f>& sublayerTransform);
    const std::optional<Matrix3f>& GetSublayerTransform() const;

    // particle properties
    void SetParticles(const std::vector<std::shared_ptr<RSRenderParticle>>& particles);
    std::vector<std::shared_ptr<RSRenderParticle>> GetParticles() const;

    // foreground properties
    void SetForegroundColor(Color color);
    Color GetForegroundColor() const;

    // background properties
    void SetBackgroundColor(Color color);
    Color GetBackgroundColor() const;
    void SetBackgroundShader(std::shared_ptr<RSShader> shader);
    std::shared_ptr<RSShader> GetBackgroundShader() const;
    void SetBgImage(std::shared_ptr<RSImage> image);
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

    // filter properties
    void SetBackgroundFilter(std::shared_ptr<RSFilter> backgroundFilter);
    void SetLinearGradientBlurPara(std::shared_ptr<RSLinearGradientBlurPara> para);
    void SetDynamicLightUpRate(const std::optional<float>& rate);
    void SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree);
    void SetFilter(std::shared_ptr<RSFilter> filter);
    const std::shared_ptr<RSFilter>& GetBackgroundFilter() const;
    const std::shared_ptr<RSLinearGradientBlurPara>& GetLinearGradientBlurPara() const;
    const std::shared_ptr<RSFilter>& GetFilter() const;
    bool NeedFilter() const;

    // shadow properties
    void SetShadowColor(Color color);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowAlpha(float alpha);
    void SetShadowElevation(float radius);
    void SetShadowRadius(float radius);
    void SetShadowPath(std::shared_ptr<RSPath> shadowPath);
    void SetShadowMask(bool shadowMask);
    Color GetShadowColor() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
    float GetShadowAlpha() const;
    float GetShadowElevation() const;
    float GetShadowRadius() const;
    const std::optional<float>& GetDynamicLightUpRate() const;
    const std::optional<float>& GetDynamicLightUpDegree() const;
    std::shared_ptr<RSPath> GetShadowPath() const;
    bool GetShadowMask() const;
    bool IsShadowValid() const;

    void SetFrameGravity(Gravity gravity);
    Gravity GetFrameGravity() const;

    void SetDrawRegion(std::shared_ptr<RectF> rect);
    std::shared_ptr<RectF> GetDrawRegion() const;

    void SetClipRRect(RRect clipRRect);
    RRect GetClipRRect() const;
    bool GetClipToRRect() const;
    void SetClipBounds(std::shared_ptr<RSPath> path);
    std::shared_ptr<RSPath> GetClipBounds() const;
    void SetClipToBounds(bool clipToBounds);
    bool GetClipToBounds() const;
    void SetClipToFrame(bool clipToFrame);
    bool GetClipToFrame() const;

    void SetVisible(bool visible);
    bool GetVisible() const;
    std::string Dump() const;

    void SetMask(std::shared_ptr<RSMask> mask);
    std::shared_ptr<RSMask> GetMask() const;

    // Pixel Stretch
    void SetPixelStretch(const std::optional<Vector4f>& stretchSize);
    const std::optional<Vector4f>& GetPixelStretch() const;
    void SetPixelStretchPercent(const std::optional<Vector4f>& stretchPercent);
    const std::optional<Vector4f>& GetPixelStretchPercent() const;
    RectI GetPixelStretchDirtyRect() const;

    const std::shared_ptr<RSObjAbsGeometry>& GetBoundsGeometry() const;
    const std::shared_ptr<RSObjGeometry>& GetFrameGeometry() const;
#ifndef USE_ROSEN_DRAWING
    bool UpdateGeometry(const RSProperties* parent, bool dirtyFlag, const std::optional<SkPoint>& offset,
        const std::optional<SkRect>& clipRect);
#else
    bool UpdateGeometry(const RSProperties* parent, bool dirtyFlag, const std::optional<Drawing::Point>& offset,
        const std::optional<Drawing::Rect>& clipRect);
#endif
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

#ifndef USE_ROSEN_DRAWING
    const sk_sp<SkColorFilter>& GetColorFilter() const;
#else
    const std::shared_ptr<Drawing::ColorFilter>& GetColorFilter() const;
#endif

    void SetUseEffect(bool useEffect);
    bool GetUseEffect() const;

#ifndef USE_ROSEN_DRAWING
    const std::unique_ptr<RSFilterCacheManager>& GetFilterCacheManager(bool isForeground) const;
#endif

    void OnApplyModifiers();

private:
    void ResetProperty(const std::unordered_set<RSModifierType>& dirtyTypes);
    void SetDirty();
    void ResetDirty();
    bool IsDirty() const;

    bool NeedClip() const;

    RectF GetFrameRect() const;
    RectF GetBgImageRect() const;
    RRect GetRRect() const;
    RRect GetInnerRRect() const;
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

    bool hasBounds_ = false;
    bool useEffect_ = false;

    Gravity frameGravity_ = Gravity::DEFAULT;

    std::shared_ptr<RectF> drawRegion_ = nullptr;

    float alpha_ = 1.f;
    bool alphaOffscreen_ = false;

    std::shared_ptr<RSObjAbsGeometry> boundsGeo_;
    std::shared_ptr<RSObjGeometry> frameGeo_;

    std::shared_ptr<RSFilter> backgroundFilter_ = nullptr;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
    std::shared_ptr<RSBorder> border_ = nullptr;
    std::shared_ptr<RSPath> clipPath_ = nullptr;
    std::optional<Vector4f> cornerRadius_;
    std::optional<Decoration> decoration_;
    std::shared_ptr<RSFilter> filter_ = nullptr;
    std::shared_ptr<RSMask> mask_ = nullptr;
    std::optional<RSShadow> shadow_;
    std::optional<Matrix3f> sublayerTransform_;
    float spherizeDegree_ = 0.f;
    float lightUpEffectDegree_ = 1.0f;

    std::weak_ptr<RSRenderNode> backref_;

    std::optional<Vector4f> pixelStretch_;
    std::optional<Vector4f> pixelStretchPercent_;
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

    // OnApplyModifiers hooks
    void CheckEmptyBounds();
    void GenerateColorFilter();
    void CalculatePixelStretch();
    void CalculateFrameOffset();

    // partial update
    bool colorFilterNeedUpdate_ = false;
    bool pixelStretchNeedUpdate_ = false;
    bool filterManagerNeedUpdate_ = false;
    float frameOffsetX_ = 0.f;
    float frameOffsetY_ = 0.f;

    std::vector<std::shared_ptr<RSRenderParticle>> particles_;
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkColorFilter> colorFilter_ = nullptr;
#else
    std::shared_ptr<Drawing::ColorFilter> colorFilter_ = nullptr;
#endif

#ifndef USE_ROSEN_DRAWING
    void CreateFilterCacheManagerIfNeed();
    std::unique_ptr<RSFilterCacheManager> backgroundFilterCacheManager_;
    std::unique_ptr<RSFilterCacheManager> foregroundFilterCacheManager_;
#endif

    std::unique_ptr<Sandbox> sandbox_ = nullptr;

    static const bool FilterCacheEnabled;

    friend class RSCanvasDrawingRenderNode;
    friend class RSCanvasRenderNode;
    friend class RSPropertiesPainter;
    friend class RSRenderNode;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H
