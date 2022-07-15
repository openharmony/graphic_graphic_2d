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
class RS_EXPORT RSBoundsModifier : public RSAnimatableModifier<Vector4f> {
public:
    explicit RSBoundsModifier(const std::shared_ptr<RSProperty<Vector4f>> property);
    virtual ~RSBoundsModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBoundsSizeModifier : public RSAnimatableModifier<Vector2f> {
public:
    explicit RSBoundsSizeModifier(const std::shared_ptr<RSProperty<Vector2f>> property);
    virtual ~RSBoundsSizeModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBoundsPositionModifier : public RSAnimatableModifier<Vector2f> {
public:
    explicit RSBoundsPositionModifier(const std::shared_ptr<RSProperty<Vector2f>> property);
    virtual ~RSBoundsPositionModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSFrameModifier : public RSAnimatableModifier<Vector4f> {
public:
    explicit RSFrameModifier(const std::shared_ptr<RSProperty<Vector4f>> property);
    virtual ~RSFrameModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSPositionZModifier : public RSAnimatableModifier<float> {
public:
    explicit RSPositionZModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSPositionZModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSPivotModifier : public RSAnimatableModifier<Vector2f> {
public:
    explicit RSPivotModifier(const std::shared_ptr<RSProperty<Vector2f>> property);
    virtual ~RSPivotModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSQuaternionModifier : public RSAnimatableModifier<Quaternion> {
public:
    explicit RSQuaternionModifier(const std::shared_ptr<RSProperty<Quaternion>> property);
    virtual ~RSQuaternionModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSRotationModifier : public RSAnimatableModifier<float> {
public:
    explicit RSRotationModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSRotationModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSRotationXModifier : public RSAnimatableModifier<float> {
public:
    explicit RSRotationXModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSRotationXModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSRotationYModifier : public RSAnimatableModifier<float> {
public:
    explicit RSRotationYModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSRotationYModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSScaleModifier : public RSAnimatableModifier<Vector2f> {
public:
    explicit RSScaleModifier(const std::shared_ptr<RSProperty<Vector2f>> property);
    virtual ~RSScaleModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSTranslateModifier : public RSAnimatableModifier<Vector2f> {
public:
    explicit RSTranslateModifier(const std::shared_ptr<RSProperty<Vector2f>> property);
    virtual ~RSTranslateModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSTranslateZModifier : public RSAnimatableModifier<float> {
public:
    explicit RSTranslateZModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSTranslateZModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSCornerRadiusModifier : public RSAnimatableModifier<Vector4f> {
public:
    explicit RSCornerRadiusModifier(const std::shared_ptr<RSProperty<Vector4f>> property);
    virtual ~RSCornerRadiusModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSAlphaModifier : public RSAnimatableModifier<float> {
public:
    explicit RSAlphaModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSAlphaModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSForegroundColorModifier : public RSAnimatableModifier<Color> {
public:
    explicit RSForegroundColorModifier(const std::shared_ptr<RSProperty<Color>> property);
    virtual ~RSForegroundColorModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBackgroundColorModifier : public RSAnimatableModifier<Color> {
public:
    explicit RSBackgroundColorModifier(const std::shared_ptr<RSProperty<Color>> property);
    virtual ~RSBackgroundColorModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSSurfaceBgColorModifier : public RSAnimatableModifier<Color> {
public:
    explicit RSSurfaceBgColorModifier(const std::shared_ptr<RSProperty<Color>> property);
    virtual ~RSSurfaceBgColorModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBackgroundShaderModifier : public RSAnimatableModifier<std::shared_ptr<RSShader>> {
public:
    explicit RSBackgroundShaderModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSShader>>> property);
    virtual ~RSBackgroundShaderModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImageModifier : public RSAnimatableModifier<std::shared_ptr<RSImage>> {
public:
    explicit RSBgImageModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSImage>>> property);
    virtual ~RSBgImageModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImageWidthModifier : public RSAnimatableModifier<float> {
public:
    explicit RSBgImageWidthModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSBgImageWidthModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImageHeightModifier : public RSAnimatableModifier<float> {
public:
    explicit RSBgImageHeightModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSBgImageHeightModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImagePositionXModifier : public RSAnimatableModifier<float> {
public:
    explicit RSBgImagePositionXModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSBgImagePositionXModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBgImagePositionYModifier : public RSAnimatableModifier<float> {
public:
    explicit RSBgImagePositionYModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSBgImagePositionYModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBorderColorModifier : public RSAnimatableModifier<Vector4<Color>> {
public:
    explicit RSBorderColorModifier(const std::shared_ptr<RSProperty<Vector4<Color>>> property);
    virtual ~RSBorderColorModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBorderWidthModifier : public RSAnimatableModifier<Vector4f> {
public:
    explicit RSBorderWidthModifier(const std::shared_ptr<RSProperty<Vector4f>> property);
    virtual ~RSBorderWidthModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBorderStyleModifier : public RSAnimatableModifier<Vector4<uint32_t>> {
public:
    explicit RSBorderStyleModifier(const std::shared_ptr<RSProperty<Vector4<uint32_t>>> property);
    virtual ~RSBorderStyleModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSFilterModifier : public RSAnimatableModifier<std::shared_ptr<RSFilter>> {
public:
    explicit RSFilterModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSFilter>>> property);
    virtual ~RSFilterModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSBackgroundFilterModifier : public RSAnimatableModifier<std::shared_ptr<RSFilter>> {
public:
    explicit RSBackgroundFilterModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSFilter>>> property);
    virtual ~RSBackgroundFilterModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSFrameGravityModifier : public RSAnimatableModifier<Gravity> {
public:
    explicit RSFrameGravityModifier(const std::shared_ptr<RSProperty<Gravity>> property);
    virtual ~RSFrameGravityModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSClipBoundsModifier : public RSAnimatableModifier<std::shared_ptr<RSPath>> {
public:
    explicit RSClipBoundsModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSPath>>> property);
    virtual ~RSClipBoundsModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSClipToBoundsModifier : public RSAnimatableModifier<bool> {
public:
    explicit RSClipToBoundsModifier(const std::shared_ptr<RSProperty<bool>> property);
    virtual ~RSClipToBoundsModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSClipToFrameModifier : public RSAnimatableModifier<bool> {
public:
    explicit RSClipToFrameModifier(const std::shared_ptr<RSProperty<bool>> property);
    virtual ~RSClipToFrameModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSVisibleModifier : public RSAnimatableModifier<bool> {
public:
    explicit RSVisibleModifier(const std::shared_ptr<RSProperty<bool>> property);
    virtual ~RSVisibleModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowColorModifier : public RSAnimatableModifier<Color> {
public:
    explicit RSShadowColorModifier(const std::shared_ptr<RSProperty<Color>> property);
    virtual ~RSShadowColorModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowOffsetXModifier : public RSAnimatableModifier<float> {
public:
    explicit RSShadowOffsetXModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSShadowOffsetXModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowOffsetYModifier : public RSAnimatableModifier<float> {
public:
    explicit RSShadowOffsetYModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSShadowOffsetYModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowAlphaModifier : public RSAnimatableModifier<float> {
public:
    explicit RSShadowAlphaModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSShadowAlphaModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowElevationModifier : public RSAnimatableModifier<float> {
public:
    explicit RSShadowElevationModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSShadowElevationModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowRadiusModifier : public RSAnimatableModifier<float> {
public:
    explicit RSShadowRadiusModifier(const std::shared_ptr<RSProperty<float>> property);
    virtual ~RSShadowRadiusModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSShadowPathModifier : public RSAnimatableModifier<std::shared_ptr<RSPath>> {
public:
    explicit RSShadowPathModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSPath>>> property);
    virtual ~RSShadowPathModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSMaskModifier : public RSAnimatableModifier<std::shared_ptr<RSMask>> {
public:
    explicit RSMaskModifier(const std::shared_ptr<RSProperty<std::shared_ptr<RSMask>>> property);
    virtual ~RSMaskModifier() = default;
    RSModifierType GetModifierType() const override;
protected:
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H
