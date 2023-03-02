/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_MODIFIER_H

#include "modifier/rs_modifier.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSBoundsModifier : public RSGeometryModifier {
public:
    explicit RSBoundsModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBoundsModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBoundsSizeModifier : public RSGeometryModifier {
public:
    explicit RSBoundsSizeModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBoundsSizeModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBoundsPositionModifier : public RSGeometryModifier {
public:
    explicit RSBoundsPositionModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBoundsPositionModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSFrameModifier : public RSGeometryModifier {
public:
    explicit RSFrameModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSFrameModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSPositionZModifier : public RSGeometryModifier {
public:
    explicit RSPositionZModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSPositionZModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSPivotModifier : public RSGeometryModifier {
public:
    explicit RSPivotModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSPivotModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSQuaternionModifier : public RSGeometryModifier {
public:
    explicit RSQuaternionModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSQuaternionModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSRotationModifier : public RSGeometryModifier {
public:
    explicit RSRotationModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSRotationModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSRotationXModifier : public RSGeometryModifier {
public:
    explicit RSRotationXModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSRotationXModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSRotationYModifier : public RSGeometryModifier {
public:
    explicit RSRotationYModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSRotationYModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSScaleModifier : public RSGeometryModifier {
public:
    explicit RSScaleModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSScaleModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSTranslateModifier : public RSGeometryModifier {
public:
    explicit RSTranslateModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSTranslateModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSTranslateZModifier : public RSGeometryModifier {
public:
    explicit RSTranslateZModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSTranslateZModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSCornerRadiusModifier : public RSAppearanceModifier {
public:
    explicit RSCornerRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSCornerRadiusModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSAlphaModifier : public RSAppearanceModifier {
public:
    explicit RSAlphaModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSAlphaModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSAlphaOffscreenModifier : public RSAppearanceModifier {
public:
    explicit RSAlphaOffscreenModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSAlphaOffscreenModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSForegroundColorModifier : public RSForegroundModifier {
public:
    explicit RSForegroundColorModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSForegroundColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBackgroundColorModifier : public RSBackgroundModifier {
public:
    explicit RSBackgroundColorModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBackgroundColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBackgroundShaderModifier : public RSBackgroundModifier {
public:
    explicit RSBackgroundShaderModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBackgroundShaderModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImageModifier : public RSBackgroundModifier {
public:
    explicit RSBgImageModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImageModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImageWidthModifier : public RSBackgroundModifier {
public:
    explicit RSBgImageWidthModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImageWidthModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImageHeightModifier : public RSBackgroundModifier {
public:
    explicit RSBgImageHeightModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImageHeightModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImagePositionXModifier : public RSBackgroundModifier {
public:
    explicit RSBgImagePositionXModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImagePositionXModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImagePositionYModifier : public RSBackgroundModifier {
public:
    explicit RSBgImagePositionYModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImagePositionYModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBorderColorModifier : public RSForegroundModifier {
public:
    explicit RSBorderColorModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBorderColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBorderWidthModifier : public RSForegroundModifier {
public:
    explicit RSBorderWidthModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBorderWidthModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBorderStyleModifier : public RSForegroundModifier {
public:
    explicit RSBorderStyleModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBorderStyleModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSFilterModifier : public RSForegroundModifier {
public:
    explicit RSFilterModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSFilterModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSLightUpEffectModifier : public RSForegroundModifier {
public:
    explicit RSLightUpEffectModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSLightUpEffectModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBackgroundFilterModifier : public RSBackgroundModifier {
public:
    explicit RSBackgroundFilterModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBackgroundFilterModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSFrameGravityModifier : public RSAppearanceModifier {
public:
    explicit RSFrameGravityModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSFrameGravityModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSClipBoundsModifier : public RSAppearanceModifier {
public:
    explicit RSClipBoundsModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSClipBoundsModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSClipToBoundsModifier : public RSAppearanceModifier {
public:
    explicit RSClipToBoundsModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSClipToBoundsModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSClipToFrameModifier : public RSAppearanceModifier {
public:
    explicit RSClipToFrameModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSClipToFrameModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSVisibleModifier : public RSAppearanceModifier {
public:
    explicit RSVisibleModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSVisibleModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSSpherizeModifier : public RSAppearanceModifier {
public:
    explicit RSSpherizeModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSSpherizeModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowColorModifier : public RSBackgroundModifier {
public:
    explicit RSShadowColorModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowOffsetXModifier : public RSBackgroundModifier {
public:
    explicit RSShadowOffsetXModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowOffsetXModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowOffsetYModifier : public RSBackgroundModifier {
public:
    explicit RSShadowOffsetYModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowOffsetYModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowAlphaModifier : public RSBackgroundModifier {
public:
    explicit RSShadowAlphaModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowAlphaModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowElevationModifier : public RSBackgroundModifier {
public:
    explicit RSShadowElevationModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowElevationModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowRadiusModifier : public RSBackgroundModifier {
public:
    explicit RSShadowRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowRadiusModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowPathModifier : public RSBackgroundModifier {
public:
    explicit RSShadowPathModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowPathModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowMaskModifier : public RSBackgroundModifier {
public:
    explicit RSShadowMaskModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowMaskModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSMaskModifier : public RSForegroundModifier {
public:
    explicit RSMaskModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSMaskModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_MODIFIER_H
