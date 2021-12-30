/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ui/rs_property_node.h"

#include <algorithm>
#include <string>

#include "animation/rs_animation.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_implicit_animator.h"
#include "command/rs_property_node_command.h"
#include "common/rs_color.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {


RSPropertyNode::RSPropertyNode() : stagingProperties_(false, GetId()), showingProperties_(false, GetId())
{
}

RSPropertyNode::~RSPropertyNode()
{
    FallbackAnimationsToRoot();
}

void RSPropertyNode::OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::function<void()>& finishCallback)
{
    RSImplicitAnimator::Instance().OpenImplicitAnimation(timingProtocol, timingCurve, finishCallback);
}

std::vector<std::shared_ptr<RSAnimation>> RSPropertyNode::CloseImplicitAnimation()
{
    return RSImplicitAnimator::Instance().CloseImplicitAnimation();
}

void RSPropertyNode::AddKeyFrame(
    float fraction, const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add keyframe animation, property callback is null!");
        return;
    }

    RSImplicitAnimator::Instance().BeginImplicitKeyFrameAnimation(fraction, timingCurve);
    propertyCallback();
    RSImplicitAnimator::Instance().EndImplicitKeyFrameAnimation();
}

void RSPropertyNode::AddKeyFrame(float fraction, const PropertyCallback& propertyCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add keyframe animation, property callback is null!");
        return;
    }

    RSImplicitAnimator::Instance().BeginImplicitKeyFrameAnimation(fraction);
    propertyCallback();
    RSImplicitAnimator::Instance().EndImplicitKeyFrameAnimation();
}

std::vector<std::shared_ptr<RSAnimation>> RSPropertyNode::Animate(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback,
    const std::function<void()>& finshCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }
    OpenImplicitAnimation(timingProtocol, timingCurve, finshCallback);
    propertyCallback();
    return CloseImplicitAnimation();
}

void RSPropertyNode::FallbackAnimationsToRoot()
{
    auto target = RSNodeMap::Instance().GetAnimationFallbackNode();

    if (target == nullptr) {
        ROSEN_LOGE("Failed to move animation to root, root node is null!");
        return;
    }
    for (const auto& [animationId, animation] : animations_) {
        target->AddAnimationInner(animation);
    }
}

void RSPropertyNode::AddAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    animations_[animation->GetId()] = animation;
    animatingPropertyNum_[animation->GetProperty()]++;
}

void RSPropertyNode::RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    auto animationItr = animations_.find(animation->GetId());
    animations_.erase(animationItr);
    animatingPropertyNum_[animation->GetProperty()]--;
}

void RSPropertyNode::FinishAnimationByProperty(const RSAnimatableProperty& property)
{
    for (const auto& [animationId, animation] : animations_) {
        if (animation->GetProperty() == property) {
            animation->Finish();
        }
    }
}

const RSProperties& RSPropertyNode::GetStagingProperties() const
{
    return stagingProperties_;
}

const RSProperties& RSPropertyNode::GetShowingProperties() const
{
    return showingProperties_;
}

void RSPropertyNode::AddAnimation(const std::shared_ptr<RSAnimation>& animation)
{
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to add animation, animation is null!");
        return;
    }

    auto animationId = animation->GetId();
    if (animations_.find(animationId) != animations_.end()) {
        ROSEN_LOGE("Failed to remove animation, animation already exists!");
        return;
    }

    auto target = std::static_pointer_cast<RSPropertyNode>(RSNodeMap::Instance().GetNode(GetId()).lock());
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start animation, target is null!");
        return;
    }

    if (animation->GetDuration() <= 0) {
        FinishAnimationByProperty(animation->GetProperty());
    }

    AddAnimationInner(animation);
    animation->StartInner(target);
}

void RSPropertyNode::RemoveAllAnimations()
{
    for (const auto& [id, animation] : animations_) {
        RemoveAnimation(animation);
    }
}

void RSPropertyNode::RemoveAnimation(const std::shared_ptr<RSAnimation>& animation)
{
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to remove animation, animation is null!");
        return;
    }

    if (animations_.find(animation->GetId()) == animations_.end()) {
        ROSEN_LOGE("Failed to remove animation, animation not exists!");
        return;
    }

    animation->Finish();
}

void RSPropertyNode::SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption)
{
    motionPathOption_ = motionPathOption;
}

const std::shared_ptr<RSMotionPathOption> RSPropertyNode::GetMotionPathOption() const
{
    return motionPathOption_;
}

bool RSPropertyNode::HasPropertyAnimation(const RSAnimatableProperty& property)
{
    auto numIt = animatingPropertyNum_.find(property);
    if (numIt != animatingPropertyNum_.end() && numIt->second > 0) {
        return true;
    }

    return false;
}

void RSPropertyNode::SetBounds(float positionX, float positionY, float width, float height)
{
    SetBoundsPosition({ positionX, positionY });
    SetBoundsSize(width, height);
}

void RSPropertyNode::SetBoundsSize(float width, float height)
{
    SetBoundsWidth(width);
    SetBoundsHeight(height);
}

#define PROPERTY_SETTER_CHECKED(memberName, value, property, needAnimation)                       \
    if (RSImplicitAnimator::Instance().NeedImplicitAnimaton() && needAnimation) {                 \
        RSImplicitAnimator::Instance().CreateImplicitAnimation(                                   \
            *this, property, stagingProperties_.Get##memberName(), value);                        \
    } else if (HasPropertyAnimation(property)) {                                                  \
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNode##memberName##Delta>( \
            GetId(), value - stagingProperties_.Get##memberName());                               \
        RSTransactionProxy::GetInstance().AddCommand(command);                                    \
        stagingProperties_.Set##memberName(value, false);                                         \
    } else {                                                                                      \
        stagingProperties_.Set##memberName(value);                                                \
    }

#define PROPERTY_SETTER(memberName, value, property) PROPERTY_SETTER_CHECKED(memberName, value, property, true)

void RSPropertyNode::SetBoundsWidth(float width)
{
    float oldWidth = stagingProperties_.GetBoundsWidth();
    if (!ROSEN_EQ(width, oldWidth)) {
        PROPERTY_SETTER_CHECKED(BoundsWidth, width, RSAnimatableProperty::BOUNDS_WIDTH, !isinf(oldWidth));
    }
}

void RSPropertyNode::SetBoundsHeight(float height)
{
    float oldHeight = stagingProperties_.GetBoundsHeight();
    if (!ROSEN_EQ(height, oldHeight)) {
        PROPERTY_SETTER_CHECKED(BoundsHeight, height, RSAnimatableProperty::BOUNDS_HEIGHT, !isinf(oldHeight));
    }
}

void RSPropertyNode::SetBoundsPosition(const Vector2f& boundsPosition)
{
    Vector2f oldPosition = stagingProperties_.GetBoundsPosition();
    if (boundsPosition != oldPosition) {
        PROPERTY_SETTER_CHECKED(BoundsPosition, boundsPosition, RSAnimatableProperty::BOUNDS_POSITION,
            !isinf(oldPosition.data_[0]) && !isinf(oldPosition.data_[1]));
    }
}

void RSPropertyNode::SetBoundsPositionX(float positionX)
{
    float oldPositionX = stagingProperties_.GetBoundsPositionX();
    if (!ROSEN_EQ(positionX, oldPositionX)) {
        PROPERTY_SETTER_CHECKED(
            BoundsPositionX, positionX, RSAnimatableProperty::BOUNDS_POSITION_X, !isinf(oldPositionX));
    }
}

void RSPropertyNode::SetBoundsPositionY(float positionY)
{
    float oldPositionY = stagingProperties_.GetBoundsPositionY();
    if (!ROSEN_EQ(positionY, oldPositionY)) {
        PROPERTY_SETTER_CHECKED(
            BoundsPositionY, positionY, RSAnimatableProperty::BOUNDS_POSITION_Y, !isinf(oldPositionY));
    }
}

void RSPropertyNode::SetFrame(float positionX, float positionY, float width, float height)
{
    SetFramePosition({ positionX, positionY });
    SetFrameSize(width, height);
}

void RSPropertyNode::SetFrameSize(float width, float height)
{
    SetFrameWidth(width);
    SetFrameHeight(height);
}

void RSPropertyNode::SetFrameWidth(float width)
{
    float oldWidth = stagingProperties_.GetFrameWidth();
    if (!ROSEN_EQ(width, oldWidth)) {
        PROPERTY_SETTER_CHECKED(FrameWidth, width, RSAnimatableProperty::FRAME_WIDTH, !isinf(oldWidth));
    }
}

void RSPropertyNode::SetFrameHeight(float height)
{
    float oldHeight = stagingProperties_.GetFrameHeight();
    if (!ROSEN_EQ(height, oldHeight)) {
        PROPERTY_SETTER_CHECKED(FrameHeight, height, RSAnimatableProperty::FRAME_HEIGHT, !isinf(oldHeight));
    }
}

void RSPropertyNode::SetFramePosition(const Vector2f& framePosition)
{
    Vector2f oldPosition = stagingProperties_.GetFramePosition();
    if (framePosition != oldPosition) {
        PROPERTY_SETTER_CHECKED(FramePosition, framePosition, RSAnimatableProperty::FRAME_POSITION,
            !isinf(oldPosition.data_[0]) && !isinf(oldPosition.data_[1]));
    }
}

void RSPropertyNode::SetFramePositionX(float positionX)
{
    float oldPositionX = stagingProperties_.GetFramePositionX();
    if (!ROSEN_EQ(positionX, oldPositionX)) {
        PROPERTY_SETTER_CHECKED(
            FramePositionX, positionX, RSAnimatableProperty::FRAME_POSITION_X, !isinf(oldPositionX));
    }
}

void RSPropertyNode::SetFramePositionY(float positionY)
{
    float oldPositionY = stagingProperties_.GetFramePositionY();
    if (!ROSEN_EQ(positionY, oldPositionY)) {
        PROPERTY_SETTER_CHECKED(
            FramePositionY, positionY, RSAnimatableProperty::FRAME_POSITION_Y, !isinf(oldPositionY));
    }
}

void RSPropertyNode::SetPositionZ(float positionZ)
{
    if (!ROSEN_EQ(positionZ, stagingProperties_.GetPositionZ())) {
        PROPERTY_SETTER(PositionZ, positionZ, RSAnimatableProperty::POSITION_Z);
    }
}

void RSPropertyNode::SetPivot(float pivotX, float pivotY)
{
    SetPivotX(pivotX);
    SetPivotY(pivotY);
}

void RSPropertyNode::SetPivotX(float pivotX)
{
    if (!ROSEN_EQ(pivotX, stagingProperties_.GetPivotX())) {
        PROPERTY_SETTER(PivotX, pivotX, RSAnimatableProperty::PIVOT_X);
    }
}

void RSPropertyNode::SetPivotY(float pivotY)
{
    if (!ROSEN_EQ(pivotY, stagingProperties_.GetPivotY())) {
        PROPERTY_SETTER(PivotY, pivotY, RSAnimatableProperty::PIVOT_Y);
    }
}

void RSPropertyNode::SetCornerRadius(float cornerRadius)
{
    if (!ROSEN_EQ(cornerRadius, stagingProperties_.GetCornerRadius())) {
        PROPERTY_SETTER(CornerRadius, cornerRadius, RSAnimatableProperty::CORNER_RADIUS);
    }
}

void RSPropertyNode::SetRotation(const Vector4f& quaternion)
{
    if (quaternion != stagingProperties_.GetQuaternion()) {
        PROPERTY_SETTER(Quaternion, quaternion, RSAnimatableProperty::ROTATION_3D);
    }
}

void RSPropertyNode::SetRotation(float degree)
{
    if (!ROSEN_EQ(degree, stagingProperties_.GetRotation())) {
        PROPERTY_SETTER(Rotation, degree, RSAnimatableProperty::ROTATION);
    }
}

void RSPropertyNode::SetRotation(float degreeX, float degreeY, float degreeZ)
{
    SetRotationX(degreeX);
    SetRotationY(degreeY);
    SetRotation(degreeZ);
}

void RSPropertyNode::SetRotationX(float degree)
{
    if (!ROSEN_EQ(degree, stagingProperties_.GetRotationX())) {
        PROPERTY_SETTER(RotationX, degree, RSAnimatableProperty::ROTATION_X);
    }
}

void RSPropertyNode::SetRotationY(float degree)
{
    if (!ROSEN_EQ(degree, stagingProperties_.GetRotationY())) {
        PROPERTY_SETTER(RotationY, degree, RSAnimatableProperty::ROTATION_Y);
    }
}

void RSPropertyNode::SetTranslate(const Vector2f& translate)
{
    if (translate != stagingProperties_.GetTranslate()) {
        PROPERTY_SETTER(Translate, translate, RSAnimatableProperty::TRANSLATE);
    }
}

void RSPropertyNode::SetTranslate(float translateX, float translateY, float translateZ)
{
    SetTranslate({ translateX, translateY });
    SetTranslateZ(translateZ);
}

void RSPropertyNode::SetTranslateX(float translate)
{
    if (!ROSEN_EQ(translate, stagingProperties_.GetTranslateX())) {
        PROPERTY_SETTER(TranslateX, translate, RSAnimatableProperty::TRANSLATE_X);
    }
}

void RSPropertyNode::SetTranslateY(float translate)
{
    if (!ROSEN_EQ(translate, stagingProperties_.GetTranslateY())) {
        PROPERTY_SETTER(TranslateY, translate, RSAnimatableProperty::TRANSLATE_Y);
    }
}

void RSPropertyNode::SetTranslateZ(float translate)
{
    if (!ROSEN_EQ(translate, stagingProperties_.GetTranslateZ())) {
        PROPERTY_SETTER(TranslateZ, translate, RSAnimatableProperty::TRANSLATE_Z);
    }
}

void RSPropertyNode::SetScale(float scale)
{
    SetScaleX(scale);
    SetScaleY(scale);
}

void RSPropertyNode::SetScale(float scaleX, float scaleY)
{
    SetScaleX(scaleX);
    SetScaleY(scaleY);
}

void RSPropertyNode::SetScaleX(float scale)
{
    if (!ROSEN_EQ(scale, stagingProperties_.GetScaleX())) {
        PROPERTY_SETTER(ScaleX, scale, RSAnimatableProperty::SCALE_X);
    }
}

void RSPropertyNode::SetScaleY(float scale)
{
    if (!ROSEN_EQ(scale, stagingProperties_.GetScaleY())) {
        PROPERTY_SETTER(ScaleY, scale, RSAnimatableProperty::SCALE_Y);
    }
}

void RSPropertyNode::SetAlpha(float alpha)
{
    if (!ROSEN_EQ(alpha, stagingProperties_.GetAlpha())) {
        PROPERTY_SETTER(Alpha, alpha, RSAnimatableProperty::ALPHA);
    }
}

void RSPropertyNode::SetForegroundColor(uint32_t colorValue)
{
    Color color = Color::FromArgbInt(colorValue);
    if (color != stagingProperties_.GetForegroundColor()) {
        PROPERTY_SETTER(ForegroundColor, color, RSAnimatableProperty::FOREGROUND_COLOR);
    }
}

void RSPropertyNode::SetBackgroundColor(uint32_t colorValue)
{
    Color color = Color::FromArgbInt(colorValue);
    if (color != stagingProperties_.GetBackgroundColor()) {
        PROPERTY_SETTER(BackgroundColor, color, RSAnimatableProperty::BACKGROUND_COLOR);
    }
}

void RSPropertyNode::SetBackgroundShader(std::shared_ptr<RSShader> shader)
{
    if (shader != stagingProperties_.GetBackgroundShader()) {
        stagingProperties_.SetBackgroundShader(shader, true);
    }
}

void RSPropertyNode::SetBgImage(std::shared_ptr<RSImage> image)
{
    if (image != stagingProperties_.GetBgImage()) {
        stagingProperties_.SetBgImage(image, true);
    }
}

void RSPropertyNode::SetBgImageSize(float width, float height)
{
    SetBgImageWidth(width);
    SetBgImageHeight(height);
}

void RSPropertyNode::SetBgImageWidth(float width)
{
    if (!ROSEN_EQ(width, stagingProperties_.GetBgImageWidth())) {
        PROPERTY_SETTER(BgImageWidth, width, RSAnimatableProperty::BGIMAGE_WIDTH);
    }
}

void RSPropertyNode::SetBgImageHeight(float height)
{
    if (!ROSEN_EQ(height, stagingProperties_.GetBgImageHeight())) {
        PROPERTY_SETTER(BgImageHeight, height, RSAnimatableProperty::BGIMAGE_HEIGHT);
    }
}

void RSPropertyNode::SetBgImagePosition(float positionX, float positionY)
{
    SetBgImagePositionX(positionX);
    SetBgImagePositionY(positionY);
}

void RSPropertyNode::SetBgImagePositionX(float positionX)
{
    if (!ROSEN_EQ(positionX, stagingProperties_.GetBgImagePositionX())) {
        PROPERTY_SETTER(BgImagePositionX, positionX, RSAnimatableProperty::BGIMAGE_POSITION_X);
    }
}

void RSPropertyNode::SetBgImagePositionY(float positionY)
{
    if (!ROSEN_EQ(positionY, stagingProperties_.GetBgImagePositionY())) {
        PROPERTY_SETTER(BgImagePositionY, positionY, RSAnimatableProperty::BGIMAGE_POSITION_Y);
    }
}

void RSPropertyNode::SetBorderColor(uint32_t colorValue)
{
    Color color = Color::FromArgbInt(colorValue);
    if (color != stagingProperties_.GetBorderColor()) {
        PROPERTY_SETTER(BorderColor, color, RSAnimatableProperty::BORDER_COLOR);
    }
}

void RSPropertyNode::SetBorderWidth(float width)
{
    if (!ROSEN_EQ(width, stagingProperties_.GetBorderWidth())) {
        PROPERTY_SETTER(BorderWidth, width, RSAnimatableProperty::BORDER_WIDTH);
    }
}

void RSPropertyNode::SetBorderStyle(uint32_t styleValue)
{
    BorderStyle style = static_cast<BorderStyle>(styleValue);
    if (style != stagingProperties_.GetBorderStyle()) {
        stagingProperties_.SetBorderStyle(style, true);
    }
}

void RSPropertyNode::SetSublayerTransform(Matrix3f sublayerTransform)
{
    if (sublayerTransform != stagingProperties_.GetSublayerTransform()) {
        PROPERTY_SETTER(SublayerTransform, sublayerTransform, RSAnimatableProperty::SUB_LAYER_TRANSFORM);
    }
}

void RSPropertyNode::SetBackgroundFilter(std::shared_ptr<RSFilter> backgroundFilter)
{
    if (backgroundFilter != stagingProperties_.GetBackgroundFilter()) {
        PROPERTY_SETTER(BackgroundFilter, backgroundFilter, RSAnimatableProperty::BACKGROUND_FILTER);
    }
}

void RSPropertyNode::SetFilter(std::shared_ptr<RSFilter> filter)
{
    if (filter != stagingProperties_.GetFilter()) {
        PROPERTY_SETTER(Filter, filter, RSAnimatableProperty::FILTER);
    }
}

void RSPropertyNode::SetCompositingFilter(std::shared_ptr<RSFilter> compositingFilter) {}

void RSPropertyNode::SetShadowColor(uint32_t colorValue)
{
    Color color = Color::FromArgbInt(colorValue);
    if (color != stagingProperties_.GetShadowColor()) {
        PROPERTY_SETTER(ShadowColor, color, RSAnimatableProperty::SHADOW_COLOR);
    }
}

void RSPropertyNode::SetShadowOffset(float offsetX, float offsetY)
{
    SetShadowOffsetX(offsetX);
    SetShadowOffsetY(offsetY);
}

void RSPropertyNode::SetShadowOffsetX(float offsetX)
{
    if (!ROSEN_EQ(offsetX, stagingProperties_.GetShadowOffsetX())) {
        PROPERTY_SETTER(ShadowOffsetX, offsetX, RSAnimatableProperty::SHADOW_OFFSET_X);
    }
}

void RSPropertyNode::SetShadowOffsetY(float offsetY)
{
    if (!ROSEN_EQ(offsetY, stagingProperties_.GetShadowOffsetY())) {
        PROPERTY_SETTER(ShadowOffsetY, offsetY, RSAnimatableProperty::SHADOW_OFFSET_Y);
    }
}

void RSPropertyNode::SetShadowAlpha(float alpha)
{
    if (!ROSEN_EQ(alpha, stagingProperties_.GetShadowAlpha())) {
        PROPERTY_SETTER(ShadowAlpha, alpha, RSAnimatableProperty::SHADOW_ALPHA);
    }
}

void RSPropertyNode::SetShadowElevation(float elevation)
{
    if (!ROSEN_EQ(elevation, stagingProperties_.GetShadowElevation())) {
        PROPERTY_SETTER(ShadowElevation, elevation, RSAnimatableProperty::SHADOW_ELEVATION);
    }
}

void RSPropertyNode::SetShadowRadius(float radius)
{
    if (!ROSEN_EQ(radius, stagingProperties_.GetShadowRadius())) {
        PROPERTY_SETTER(ShadowRadius, radius, RSAnimatableProperty::SHADOW_RADIUS);
    }
}

void RSPropertyNode::SetShadowPath(std::shared_ptr<RSPath> shadowpath)
{
    if (shadowpath != stagingProperties_.GetShadowPath()) {
        stagingProperties_.SetShadowPath(shadowpath);
    }
}

void RSPropertyNode::SetFrameGravity(Gravity gravity)
{
    if (gravity != stagingProperties_.GetFrameGravity()) {
        stagingProperties_.SetFrameGravity(gravity);
    }
}

void RSPropertyNode::SetClipBounds(std::shared_ptr<RSPath> path)
{
    if (path != stagingProperties_.GetClipBounds()) {
        stagingProperties_.SetClipBounds(path);
    }
}

void RSPropertyNode::SetClipToBounds(bool clipToBounds)
{
    if (clipToBounds != stagingProperties_.GetClipToBounds()) {
        stagingProperties_.SetClipToBounds(clipToBounds);
    }
}

void RSPropertyNode::SetClipToFrame(bool clipToFrame)
{
    if (clipToFrame != stagingProperties_.GetClipToFrame()) {
        stagingProperties_.SetClipToFrame(clipToFrame);
    }
}

void RSPropertyNode::SetVisible(bool visible)
{
    if (visible != stagingProperties_.GetVisible()) {
        auto type = visible ? RSTransitionEffectType::FADE_IN : RSTransitionEffectType::FADE_OUT;
        NotifyTransition({RSTransitionEffect(type)}, GetId());
        stagingProperties_.SetVisible(visible);
    }
}

void RSPropertyNode::NotifyTransition(const std::vector<RSTransitionEffect> effects, NodeId nodeId)
{
    if (!RSImplicitAnimator::Instance().NeedImplicitAnimaton() || effects.empty()) {
        return;
    }
    auto node = std::static_pointer_cast<RSPropertyNode>(RSNodeMap::Instance().GetNode(nodeId).lock());
    if (node == nullptr) {
        return;
    }
    for (auto effect : effects) {
        if (effect.GetType() == RSTransitionEffectType::UNDEFINED) {
            continue;
        }
        RSImplicitAnimator::Instance().BeginImplicitTransition(effect);
        RSImplicitAnimator::Instance().CreateImplicitTransition(*(node.get()));
        RSImplicitAnimator::Instance().EndImplicitTransition();
    }
}

void RSPropertyNode::OnAddChildren()
{
    NotifyTransition({RSTransitionEffect(RSTransitionEffectType::FADE_IN)}, GetId());
}

void RSPropertyNode::OnRemoveChildren()
{
    NotifyTransition({RSTransitionEffect(RSTransitionEffectType::FADE_OUT)}, GetId());
}

void RSPropertyNode::AnimationFinish(long long animationId)
{
    auto animationItr = animations_.find(animationId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("Failed to find animation[%llu]!", animationId);
        return;
    }

    auto animation = animationItr->second;
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to finish animation[%llu], animation is null!", animationId);
        return;
    }

    animation->CallFinishCallback();
    RemoveAnimationInner(animation);
}
} // namespace Rosen
} // namespace OHOS
