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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H

#include <unordered_map>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_transition_effect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier_extractor.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "ui/rs_base_node.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
using DrawFunc = std::function<void(std::shared_ptr<SkCanvas>)>;
using PropertyCallback = std::function<void()>;
class RSAnimation;
class RSCommand;
class RSImplicitAnimParam;
class RSImplicitAnimator;
class RSUIAnimationManager;
class RSModifier;

class RSC_EXPORT RSNode : public RSBaseNode {
public:
    using WeakPtr = std::weak_ptr<RSNode>;
    using SharedPtr = std::shared_ptr<RSNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::RS_NODE;
    RSUINodeType GetType() const override
    {
        return Type;
    }

    ~RSNode() override;
    std::string DumpNode(int depth) const override;

    static std::vector<std::shared_ptr<RSAnimation>> Animate(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback,
        const std::function<void()>& finishCallback = nullptr);
    static void OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const std::function<void()>& finishCallback = nullptr);
    static std::vector<std::shared_ptr<RSAnimation>> CloseImplicitAnimation();
    static void AddKeyFrame(
        float fraction, const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback);
    static void AddKeyFrame(float fraction, const PropertyCallback& callback);

    void NotifyTransition(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn);

    void AddAnimation(const std::shared_ptr<RSAnimation>& animation);
    void RemoveAllAnimations();
    void RemoveAnimation(const std::shared_ptr<RSAnimation>& animation);
    void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption);
    const std::shared_ptr<RSMotionPathOption> GetMotionPathOption() const;

    virtual void DrawOnNode(RSModifierType type, DrawFunc func) {} // [PLANNING]: support SurfaceNode

    const RSModifierExtractor& GetStagingProperties() const;

    template<typename ModifierName, typename PropertyName, typename T>
    void SetProperty(RSModifierType modifierType, T value);

    virtual void SetBounds(const Vector4f& bounds);
    virtual void SetBounds(float positionX, float positionY, float width, float height);
    virtual void SetBoundsWidth(float width);
    virtual void SetBoundsHeight(float height);

    virtual void SetFrame(const Vector4f& frame);
    virtual void SetFrame(float positionX, float positionY, float width, float height);
    virtual void SetFramePositionX(float positionX);
    virtual void SetFramePositionY(float positionY);

    void SetPositionZ(float positionZ);

    void SetPivot(const Vector2f& pivot);
    void SetPivot(float pivotX, float pivotY);
    void SetPivotX(float pivotX);
    void SetPivotY(float pivotY);

    void SetCornerRadius(float cornerRadius);
    void SetCornerRadius(const Vector4f& cornerRadius);

    void SetRotation(const Quaternion& quaternion);
    void SetRotation(float degreeX, float degreeY, float degreeZ);
    void SetRotation(float degree);
    void SetRotationX(float degree);
    void SetRotationY(float degree);

    void SetTranslate(const Vector2f& translate);
    void SetTranslate(float translateX, float translateY, float translateZ);
    void SetTranslateX(float translate);
    void SetTranslateY(float translate);
    void SetTranslateZ(float translate);

    void SetScale(float scale);
    void SetScale(float scaleX, float scaleY);
    void SetScale(const Vector2f& scale);
    void SetScaleX(float scaleX);
    void SetScaleY(float scaleY);

    void SetAlpha(float alpha);
    void SetAlphaOffscreen(bool alphaOffscreen);

    void SetForegroundColor(uint32_t colorValue);
    void SetBackgroundColor(uint32_t colorValue);
    void SetBackgroundShader(const std::shared_ptr<RSShader>& shader);

    void SetBgImage(const std::shared_ptr<RSImage>& image);
    void SetBgImageSize(float width, float height);
    void SetBgImageWidth(float width);
    void SetBgImageHeight(float height);
    void SetBgImagePosition(float positionX, float positionY);
    void SetBgImagePositionX(float positionX);
    void SetBgImagePositionY(float positionY);

    void SetBorderColor(uint32_t colorValue);
    void SetBorderColor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
    void SetBorderColor(const Vector4<Color>& color);
    void SetBorderWidth(float width);
    void SetBorderWidth(float left, float top, float right, float bottom);
    void SetBorderWidth(const Vector4f& width);
    void SetBorderStyle(uint32_t styleValue);
    void SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
    void SetBorderStyle(const Vector4<BorderStyle>& style);

    void SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter);
    void SetFilter(const std::shared_ptr<RSFilter>& filter);
    void SetCompositingFilter(const std::shared_ptr<RSFilter>& compositingFilter);

    void SetShadowColor(uint32_t colorValue);
    void SetShadowOffset(float offsetX, float offsetY);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowAlpha(float alpha);
    void SetShadowElevation(float elevation);
    void SetShadowRadius(float radius);
    void SetShadowPath(const std::shared_ptr<RSPath>& shadowPath);

    void SetFrameGravity(Gravity gravity);

    void SetClipBounds(const std::shared_ptr<RSPath>& clipToBounds);
    void SetClipToBounds(bool clipToBounds);
    void SetClipToFrame(bool clipToFrame);

    void SetVisible(bool visible);
    void SetMask(const std::shared_ptr<RSMask>& mask);

    void SetPaintOrder(bool drawContentLast);

    void SetTransitionEffect(const std::shared_ptr<const RSTransitionEffect>& effect)
    {
        transitionEffect_ = effect;
    }

    void AddModifier(const std::shared_ptr<RSModifier> modifier);
    void RemoveModifier(const std::shared_ptr<RSModifier> modifier);

protected:
    explicit RSNode(bool isRenderServiceNode);
    explicit RSNode(bool isRenderServiceNode, NodeId id);
    RSNode(const RSNode&) = delete;
    RSNode(const RSNode&&) = delete;
    RSNode& operator=(const RSNode&) = delete;
    RSNode& operator=(const RSNode&&) = delete;

    bool drawContentLast_ = false;

    void OnAddChildren() override;
    void OnRemoveChildren() override;

    virtual bool NeedForcedSendToRemote() const
    {
        return false;
    }

    std::vector<PropertyId> GetModifierIds() const;

private:
    bool AnimationFinish(AnimationId animationId);
    bool HasPropertyAnimation(const PropertyId& id);
    void FallbackAnimationsToRoot();
    void AddAnimationInner(const std::shared_ptr<RSAnimation>& animation);
    void RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation);
    void FinishAnimationByProperty(const PropertyId& id);
    const std::shared_ptr<RSModifier> GetModifier(const PropertyId& propertyId);
    virtual void OnBoundsSizeChanged() const {};
    void UpdateModifierMotionPathOption();
    void UpdateExtendedModifier(const std::weak_ptr<RSModifier>& modifier);

    // Planning: refactor RSUIAnimationManager and remove this method
    void ClearAllModifiers();

    std::unordered_map<AnimationId, std::shared_ptr<RSAnimation>> animations_;
    std::unordered_map<PropertyId, uint32_t> animatingPropertyNum_;
    std::unordered_map<PropertyId, std::shared_ptr<RSModifier>> modifiers_;
    std::unordered_map<RSModifierType, std::shared_ptr<RSModifier>> propertyModifiers_;
    std::shared_ptr<RSMotionPathOption> motionPathOption_;

    void UpdateImplicitAnimator();
    pid_t implicitAnimatorTid_ = 0;
    std::shared_ptr<RSImplicitAnimator> implicitAnimator_;
    std::shared_ptr<const RSTransitionEffect> transitionEffect_;

    RSModifierExtractor stagingPropertiesExtractor_;

    friend class RSAnimation;
    friend class RSCurveAnimation;
    friend class RSKeyframeAnimation;
    friend class RSPropertyAnimation;
    friend class RSSpringAnimation;
    template<typename T>
    friend class RSProperty;
    template<typename T>
    friend class RSAnimatableProperty;
    friend class RSPathAnimation;
    friend class RSExtendedModifier;
    friend class RSTransition;
    friend class RSUIDirector;
    friend class RSImplicitAnimator;
    friend class RSModifierExtractor;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
