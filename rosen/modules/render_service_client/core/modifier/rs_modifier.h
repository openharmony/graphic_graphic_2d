/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "modifier/rs_modifier_base.h"

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSBoundsModifier : public RSModifier<RSAnimatableProperty<Vector4f>> {
public:
    explicit RSBoundsModifier(const std::shared_ptr<RSAnimatableProperty<Vector4f>>& property);
    virtual ~RSBoundsModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBoundsSizeModifier : public RSModifier<RSAnimatableProperty<Vector2f>> {
public:
    explicit RSBoundsSizeModifier(const std::shared_ptr<RSAnimatableProperty<Vector2f>>& property);
    virtual ~RSBoundsSizeModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBoundsPositionModifier : public RSModifier<RSAnimatableProperty<Vector2f>> {
public:
    explicit RSBoundsPositionModifier(const std::shared_ptr<RSAnimatableProperty<Vector2f>>& property);
    virtual ~RSBoundsPositionModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSFrameModifier : public RSModifier<RSAnimatableProperty<Vector4f>> {
public:
    explicit RSFrameModifier(const std::shared_ptr<RSAnimatableProperty<Vector4f>>& property);
    virtual ~RSFrameModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSPositionZModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSPositionZModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSPositionZModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSPivotModifier : public RSModifier<RSAnimatableProperty<Vector2f>> {
public:
    explicit RSPivotModifier(const std::shared_ptr<RSAnimatableProperty<Vector2f>>& property);
    virtual ~RSPivotModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSQuaternionModifier : public RSModifier<RSAnimatableProperty<Quaternion>> {
public:
    explicit RSQuaternionModifier(const std::shared_ptr<RSAnimatableProperty<Quaternion>>& property);
    virtual ~RSQuaternionModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSRotationModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSRotationModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSRotationModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSRotationXModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSRotationXModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSRotationXModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSRotationYModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSRotationYModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSRotationYModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSScaleModifier : public RSModifier<RSAnimatableProperty<Vector2f>> {
public:
    explicit RSScaleModifier(const std::shared_ptr<RSAnimatableProperty<Vector2f>>& property);
    virtual ~RSScaleModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSTranslateModifier : public RSModifier<RSAnimatableProperty<Vector2f>> {
public:
    explicit RSTranslateModifier(const std::shared_ptr<RSAnimatableProperty<Vector2f>>& property);
    virtual ~RSTranslateModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSTranslateZModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSTranslateZModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSTranslateZModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSCornerRadiusModifier : public RSModifier<RSAnimatableProperty<Vector4f>> {
public:
    explicit RSCornerRadiusModifier(const std::shared_ptr<RSAnimatableProperty<Vector4f>>& property);
    virtual ~RSCornerRadiusModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSAlphaModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSAlphaModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSAlphaModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSForegroundColorModifier : public RSModifier<RSAnimatableProperty<Color>> {
public:
    explicit RSForegroundColorModifier(const std::shared_ptr<RSAnimatableProperty<Color>>& property);
    virtual ~RSForegroundColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBackgroundColorModifier : public RSModifier<RSAnimatableProperty<Color>> {
public:
    explicit RSBackgroundColorModifier(const std::shared_ptr<RSAnimatableProperty<Color>>& property);
    virtual ~RSBackgroundColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSSurfaceBgColorModifier : public RSModifier<RSAnimatableProperty<Color>> {
public:
    explicit RSSurfaceBgColorModifier(const std::shared_ptr<RSAnimatableProperty<Color>>& property);
    virtual ~RSSurfaceBgColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBackgroundShaderModifier :
    public RSModifier<RSProperty<std::shared_ptr<RSShader>>> {
public:
    explicit RSBackgroundShaderModifier(
        const std::shared_ptr<RSProperty<std::shared_ptr<RSShader>>>& property);
    virtual ~RSBackgroundShaderModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImageModifier : public RSModifier<RSProperty<std::shared_ptr<RSImage>>> {
public:
    explicit RSBgImageModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSImage>>>& property);
    virtual ~RSBgImageModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImageWidthModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSBgImageWidthModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSBgImageWidthModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImageHeightModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSBgImageHeightModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSBgImageHeightModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImagePositionXModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSBgImagePositionXModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSBgImagePositionXModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImagePositionYModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSBgImagePositionYModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSBgImagePositionYModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBorderColorModifier : public RSModifier<RSAnimatableProperty<Vector4<Color>>> {
public:
    explicit RSBorderColorModifier(const std::shared_ptr<RSAnimatableProperty<Vector4<Color>>>& property);
    virtual ~RSBorderColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBorderWidthModifier : public RSModifier<RSAnimatableProperty<Vector4f>> {
public:
    explicit RSBorderWidthModifier(const std::shared_ptr<RSAnimatableProperty<Vector4f>>& property);
    virtual ~RSBorderWidthModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBorderStyleModifier : public RSModifier<RSProperty<Vector4<uint32_t>>> {
public:
    explicit RSBorderStyleModifier(const std::shared_ptr<RSProperty<Vector4<uint32_t>>>& property);
    virtual ~RSBorderStyleModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSFilterModifier : public RSModifier<RSAnimatableProperty<std::shared_ptr<RSFilter>>> {
public:
    explicit RSFilterModifier(const std::shared_ptr<RSAnimatableProperty<std::shared_ptr<RSFilter>>>& property);
    virtual ~RSFilterModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBackgroundFilterModifier :
    public RSModifier<RSAnimatableProperty<std::shared_ptr<RSFilter>>> {
public:
    explicit RSBackgroundFilterModifier(
        const std::shared_ptr<RSAnimatableProperty<std::shared_ptr<RSFilter>>>& property);
    virtual ~RSBackgroundFilterModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSFrameGravityModifier : public RSModifier<RSProperty<Gravity>> {
public:
    explicit RSFrameGravityModifier(const std::shared_ptr<RSProperty<Gravity>>& property);
    virtual ~RSFrameGravityModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSClipBoundsModifier :
    public RSModifier<RSProperty<std::shared_ptr<RSPath>>> {
public:
    explicit RSClipBoundsModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSPath>>>& property);
    virtual ~RSClipBoundsModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSClipToBoundsModifier : public RSModifier<RSProperty<bool>> {
public:
    explicit RSClipToBoundsModifier(const std::shared_ptr<RSProperty<bool>>& property);
    virtual ~RSClipToBoundsModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSClipToFrameModifier : public RSModifier<RSProperty<bool>> {
public:
    explicit RSClipToFrameModifier(const std::shared_ptr<RSProperty<bool>>& property);
    virtual ~RSClipToFrameModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSVisibleModifier : public RSModifier<RSProperty<bool>> {
public:
    explicit RSVisibleModifier(const std::shared_ptr<RSProperty<bool>>& property);
    virtual ~RSVisibleModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowColorModifier : public RSModifier<RSAnimatableProperty<Color>> {
public:
    explicit RSShadowColorModifier(const std::shared_ptr<RSAnimatableProperty<Color>>& property);
    virtual ~RSShadowColorModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowOffsetXModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSShadowOffsetXModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSShadowOffsetXModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowOffsetYModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSShadowOffsetYModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSShadowOffsetYModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowAlphaModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSShadowAlphaModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSShadowAlphaModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowElevationModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSShadowElevationModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSShadowElevationModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowRadiusModifier : public RSModifier<RSAnimatableProperty<float>> {
public:
    explicit RSShadowRadiusModifier(const std::shared_ptr<RSAnimatableProperty<float>>& property);
    virtual ~RSShadowRadiusModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowPathModifier :
    public RSModifier<RSProperty<std::shared_ptr<RSPath>>> {
public:
    explicit RSShadowPathModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSPath>>>& property);
    virtual ~RSShadowPathModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSMaskModifier : public RSModifier<RSProperty<std::shared_ptr<RSMask>>> {
public:
    explicit RSMaskModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSMask>>>& property);
    virtual ~RSMaskModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H
