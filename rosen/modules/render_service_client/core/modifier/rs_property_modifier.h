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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H

#include "modifier/rs_modifier.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSBoundsModifier : public RSModifier {
public:
    explicit RSBoundsModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBoundsModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBoundsSizeModifier : public RSModifier {
public:
    explicit RSBoundsSizeModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBoundsSizeModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBoundsPositionModifier : public RSModifier {
public:
    explicit RSBoundsPositionModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBoundsPositionModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSFrameModifier : public RSModifier {
public:
    explicit RSFrameModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSFrameModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSPositionZModifier : public RSModifier {
public:
    explicit RSPositionZModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSPositionZModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSPivotModifier : public RSModifier {
public:
    explicit RSPivotModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSPivotModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSQuaternionModifier : public RSModifier {
public:
    explicit RSQuaternionModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSQuaternionModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSRotationModifier : public RSModifier {
public:
    explicit RSRotationModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSRotationModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSRotationXModifier : public RSModifier {
public:
    explicit RSRotationXModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSRotationXModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSRotationYModifier : public RSModifier {
public:
    explicit RSRotationYModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSRotationYModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSScaleModifier : public RSModifier {
public:
    explicit RSScaleModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSScaleModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSTranslateModifier : public RSModifier {
public:
    explicit RSTranslateModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSTranslateModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSTranslateZModifier : public RSModifier {
public:
    explicit RSTranslateZModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSTranslateZModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSCornerRadiusModifier : public RSModifier {
public:
    explicit RSCornerRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSCornerRadiusModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSAlphaModifier : public RSModifier {
public:
    explicit RSAlphaModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSAlphaModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSAlphaOffscreenModifier : public RSModifier {
public:
    explicit RSAlphaOffscreenModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSAlphaOffscreenModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSForegroundColorModifier : public RSModifier {
public:
    explicit RSForegroundColorModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSForegroundColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBackgroundColorModifier : public RSModifier {
public:
    explicit RSBackgroundColorModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBackgroundColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSSurfaceBgColorModifier : public RSModifier {
public:
    explicit RSSurfaceBgColorModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSSurfaceBgColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBackgroundShaderModifier : public RSModifier {
public:
    explicit RSBackgroundShaderModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBackgroundShaderModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImageModifier : public RSModifier {
public:
    explicit RSBgImageModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImageModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImageWidthModifier : public RSModifier {
public:
    explicit RSBgImageWidthModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImageWidthModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImageHeightModifier : public RSModifier {
public:
    explicit RSBgImageHeightModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImageHeightModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImagePositionXModifier : public RSModifier {
public:
    explicit RSBgImagePositionXModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImagePositionXModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImagePositionYModifier : public RSModifier {
public:
    explicit RSBgImagePositionYModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBgImagePositionYModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBorderColorModifier : public RSModifier {
public:
    explicit RSBorderColorModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBorderColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBorderWidthModifier : public RSModifier {
public:
    explicit RSBorderWidthModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBorderWidthModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBorderStyleModifier : public RSModifier {
public:
    explicit RSBorderStyleModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBorderStyleModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSFilterModifier : public RSModifier {
public:
    explicit RSFilterModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSFilterModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBackgroundFilterModifier : public RSModifier {
public:
    explicit RSBackgroundFilterModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBackgroundFilterModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSFrameGravityModifier : public RSModifier {
public:
    explicit RSFrameGravityModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSFrameGravityModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSClipBoundsModifier : public RSModifier {
public:
    explicit RSClipBoundsModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSClipBoundsModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSClipToBoundsModifier : public RSModifier {
public:
    explicit RSClipToBoundsModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSClipToBoundsModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSClipToFrameModifier : public RSModifier {
public:
    explicit RSClipToFrameModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSClipToFrameModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSVisibleModifier : public RSModifier {
public:
    explicit RSVisibleModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSVisibleModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowColorModifier : public RSModifier {
public:
    explicit RSShadowColorModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowOffsetXModifier : public RSModifier {
public:
    explicit RSShadowOffsetXModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowOffsetXModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowOffsetYModifier : public RSModifier {
public:
    explicit RSShadowOffsetYModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowOffsetYModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowAlphaModifier : public RSModifier {
public:
    explicit RSShadowAlphaModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowAlphaModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowElevationModifier : public RSModifier {
public:
    explicit RSShadowElevationModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowElevationModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowRadiusModifier : public RSModifier {
public:
    explicit RSShadowRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowRadiusModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSShadowPathModifier : public RSModifier {
public:
    explicit RSShadowPathModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSShadowPathModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSMaskModifier : public RSModifier {
public:
    explicit RSMaskModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSMaskModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H
