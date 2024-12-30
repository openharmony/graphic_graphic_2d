/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_APPEARANCE_RS_FOREGROUND_FILTER_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_APPEARANCE_RS_FOREGROUND_FILTER_RENDER_MODIFIER_H

#include "modifier_ng/rs_render_modifier_ng.h"
// TODO: remove it
#include "render/rs_shadow.h"

namespace OHOS::Rosen {
class RSMotionBlurFilter;
class RSForegroundEffectFilter;
class RSSpherizeEffectFilter;
class RSFlyOutShaderFilter;
class RSAttractionEffectFilter;
class RSColorfulShadowFilter;
class RSDistortionFilter;

namespace ModifierNG {
class RSB_EXPORT RSBeginForegroundFilterDrawable : public RSDrawable {
public:
    RSBeginForegroundFilterDrawable() = default;
    ~RSBeginForegroundFilterDrawable() override = default;
    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;
    void Purge() override {};
    void SetBoundsRect(const RectF& boundsRect);

private:
    void OnSync() override;
    RectF boundsRect_;
    RectF stagingBoundsRect_;
};

class RSB_EXPORT RSForegroundFilterRenderModifier : public RSRenderModifier {
public:
    RSForegroundFilterRenderModifier() = default;
    ~RSForegroundFilterRenderModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::FOREGROUND_FILTER;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;
    std::shared_ptr<RSBeginForegroundFilterDrawable> GetBeginForegroundFilterDrawable();

    void ResetProperties(RSProperties& properties) override;

private:
    static const LegacyPropertyApplierMap LegacyPropertyApplierMap_;
    const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const override
    {
        return LegacyPropertyApplierMap_;
    }

    static const bool IS_UNI_RENDER;
    std::shared_ptr<RSMotionBlurFilter> GenerateMotionBlurFilterIfValid();
    std::shared_ptr<RSForegroundEffectFilter> GenerateForegroundEffectFilterIfValid();
    std::shared_ptr<RSSpherizeEffectFilter> GenerateSphereEffectFilterIfValid();
    std::shared_ptr<RSFlyOutShaderFilter> GenerateFlyOutShaderFilterIfValid();
    std::shared_ptr<RSAttractionEffectFilter> GenerateAttractionEffectFilterIfValid(const RSModifierContext& context);
    std::shared_ptr<RSColorfulShadowFilter> GenerateColorfulShadowFilterIfValid(const RSModifierContext& context);
    std::shared_ptr<RSDistortionFilter> GenerateDistortionFilterIfValid();
    bool OnApply(RSModifierContext& context) override;
    void OnSync() override;

    std::shared_ptr<OHOS::Rosen::RSFilter> filter_;
    std::shared_ptr<OHOS::Rosen::RSFilter> stagingFilter_;
    std::shared_ptr<RSBeginForegroundFilterDrawable> beginForegroundFilterDrawable_;

    // TODO: remove it
    std::optional<RSShadow> shadow_;
    RectI attractionEffectCurrentDirtyRegion_ = { 0, 0, 0, 0 };
    float GetShadowElevation() const;
    float GetShadowRadius() const;
    std::shared_ptr<RSPath> GetShadowPath() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
    bool GetShadowIsFilled() const;
};
} // namespace ModifierNG
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_APPEARANCE_RS_FOREGROUND_FILTER_RENDER_MODIFIER_H
