/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "animation/rs_animatable_property.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_property_accessors.h"
#include "animation/rs_transition_effect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties.h"
#include "render/rs_path.h"
#include "render/rs_mask.h"
#include "ui/rs_base_node.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
using PropertyCallback = std::function<void()>;
class RSAnimation;
class RSCommand;
class RSImplicitAnimParam;
class RSBasePropertyAccessors;

class RS_EXPORT RSNode : public RSBaseNode {
public:
    using WeakPtr = std::weak_ptr<RSNode>;
    using SharedPtr = std::shared_ptr<RSNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::RS_NODE;

    virtual ~RSNode();
    std::string DumpNode(int depth) const override;

    static std::vector<std::shared_ptr<RSAnimation>> Animate(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback,
        const std::function<void()>& finshCallback = nullptr);
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

    const RSProperties& GetStagingProperties() const;

    virtual void SetBounds(const Vector4f& bounds);
    virtual void SetBounds(float positionX, float positionY, float width, float height);
    virtual void SetBoundsSize(const Vector2f& size);
    virtual void SetBoundsSize(float width, float height);
    virtual void SetBoundsWidth(float width);
    virtual void SetBoundsHeight(float height);
    void SetBoundsPosition(const Vector2f& boundsPosition);
    void SetBoundsPosition(float positionX, float positionY);
    void SetBoundsPositionX(float positionX);
    void SetBoundsPositionY(float positionY);

    void SetFrame(const Vector4f& frame);
    void SetFrame(float positionX, float positionY, float width, float height);
    void SetFrameSize(const Vector2f& size);
    void SetFrameSize(float width, float height);
    void SetFrameWidth(float width);
    void SetFrameHeight(float height);
    void SetFramePosition(const Vector2f& framePosition);
    void SetFramePosition(float positionX, float positionY);
    void SetFramePositionX(float positionX);
    void SetFramePositionY(float positionY);

    void SetPositionZ(float positionZ);

    void SetPivot(const Vector2f& pivot);
    void SetPivot(float pivotX, float pivotY);
    void SetPivotX(float pivotX);
    void SetPivotY(float pivotY);

    void SetCornerRadius(float cornerRadius);

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
    void SetScaleX(float scale);
    void SetScaleY(float scale);

    void SetAlpha(float alpha);

    void SetForegroundColor(uint32_t colorValue);
    void SetBackgroundColor(uint32_t colorValue);
    void SetBackgroundShader(std::shared_ptr<RSShader> shader);

    void SetBgImage(std::shared_ptr<RSImage> image);
    void SetBgImageSize(float width, float height);
    void SetBgImageWidth(float width);
    void SetBgImageHeight(float height);
    void SetBgImagePosition(float positionX, float positionY);
    void SetBgImagePositionX(float positionX);
    void SetBgImagePositionY(float positionY);

    void SetBorderColor(uint32_t colorValue);
    void SetBorderWidth(float width);
    void SetBorderStyle(uint32_t styleValue);

    void SetSublayerTransform(Matrix3f sublayerTransform);

    void SetBackgroundFilter(std::shared_ptr<RSFilter> backgroundFilter);
    void SetFilter(std::shared_ptr<RSFilter> filter);
    void SetCompositingFilter(std::shared_ptr<RSFilter> compositingFilter);

    void SetShadowColor(uint32_t colorValue);
    void SetShadowOffset(float offsetX, float offsetY);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowAlpha(float alpha);
    void SetShadowElevation(float elevation);
    void SetShadowRadius(float radius);
    void SetShadowPath(std::shared_ptr<RSPath> shadowpath);

    void SetFrameGravity(Gravity gravity);

    void SetClipBounds(std::shared_ptr<RSPath> clipToBounds);
    void SetClipToBounds(bool clipToBounds);
    void SetClipToFrame(bool clipToFrame);

    void SetVisible(bool visible);
    void SetPaintOrder(bool drawContentLast);
    void SetMask(std::shared_ptr<RSMask> mask);

    void SetTransitionEffect(const std::shared_ptr<const RSTransitionEffect>& effect)
    {
        transitionEffect_ = effect;
    }

    RSUINodeType GetType() const override
    {
        return RSUINodeType::RS_NODE;
    }

protected:
    RSNode(bool isRenderServiceNode);
    RSNode(const RSNode&) = delete;
    RSNode(const RSNode&&) = delete;
    RSNode& operator=(const RSNode&) = delete;
    RSNode& operator=(const RSNode&&) = delete;

    bool drawContentLast_ = false;

    void OnAddChildren() override;
    void OnRemoveChildren() override;
    void AnimationFinish(long long animationId);

    virtual bool NeedForcedSendToRemote() const
    {
        return false;
    }

private:
    bool HasPropertyAnimation(const RSAnimatableProperty& property);
    void FallbackAnimationsToRoot();
    void AddAnimationInner(const std::shared_ptr<RSAnimation>& animation);
    void RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation);
    void FinishAnimationByProperty(const RSAnimatableProperty& property);

    std::map<AnimationId, std::shared_ptr<RSAnimation>> animations_;
    std::map<RSAnimatableProperty, int> animatingPropertyNum_;
    std::shared_ptr<RSMotionPathOption> motionPathOption_;

    std::shared_ptr<const RSTransitionEffect> transitionEffect_ = nullptr;

    RSProperties stagingProperties_;

    friend class RSAnimation;
    template<typename T>
    friend class RSCurveAnimation;
    template<typename T>
    friend class RSKeyframeAnimation;
    template<typename T>
    friend class RSPropertyAnimation;
    friend class RSPathAnimation;
    friend class RSTransition;
    friend class RSUIDirector;
    friend class RSImplicitAnimator;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
