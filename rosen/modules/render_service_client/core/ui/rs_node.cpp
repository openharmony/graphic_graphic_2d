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

#include "ui/rs_node.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

#include "animation/rs_animation.h"
#include "animation/rs_animation_manager_map.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "command/rs_node_command.h"
#include "common/rs_color.h"
#include "common/rs_obj_geometry.h"
#include "modifier/rs_modifier.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "transaction/rs_transaction_proxy.h"
#include "animation/rs_ui_animation_manager.h"
#include "modifier/rs_property_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
bool IsPathAnimatableModifier(const RSModifierType& type)
{
    if (type == RSModifierType::BOUNDS || type == RSModifierType::FRAME || type == RSModifierType::TRANSLATE) {
        return true;
    }
    return false;
}
}

RSNode::RSNode(bool isRenderServiceNode)
    : RSBaseNode(isRenderServiceNode), stagingPropertiesExtractor_(GetId())
{
    UpdateImplicitAnimator();
}

RSNode::RSNode(bool isRenderServiceNode, NodeId id)
    : RSBaseNode(isRenderServiceNode, id), stagingPropertiesExtractor_(id)
{
    UpdateImplicitAnimator();
}

RSNode::~RSNode()
{
    FallbackAnimationsToRoot();
    ClearAllModifiers();
}

void RSNode::OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::function<void()>& finishCallback)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return;
    }

    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, finishCallback);
}

std::vector<std::shared_ptr<RSAnimation>> RSNode::CloseImplicitAnimation()
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to close implicit animation, implicit animator is null!");
        return {};
    }

    return implicitAnimator->CloseImplicitAnimation();
}

void RSNode::AddKeyFrame(
    float fraction, const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to add keyframe, implicit animator is null!");
        return;
    }

    implicitAnimator->BeginImplicitKeyFrameAnimation(fraction, timingCurve);
    propertyCallback();
    implicitAnimator->EndImplicitKeyFrameAnimation();
}

void RSNode::AddKeyFrame(float fraction, const PropertyCallback& propertyCallback)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to add keyframe, implicit animator is null!");
        return;
    }

    implicitAnimator->BeginImplicitKeyFrameAnimation(fraction);
    propertyCallback();
    implicitAnimator->EndImplicitKeyFrameAnimation();
}

std::vector<std::shared_ptr<RSAnimation>> RSNode::Animate(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback,
    const std::function<void()>& finishCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }

    if (RSImplicitAnimatorMap::Instance().GetAnimator(gettid()) == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, implicit animator is null!");
        return {};
    }

    OpenImplicitAnimation(timingProtocol, timingCurve, finishCallback);
    propertyCallback();
    return CloseImplicitAnimation();
}

void RSNode::FallbackAnimationsToRoot()
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

void RSNode::AddAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    animations_.emplace(animation->GetId(), animation);
    animatingPropertyNum_[animation->GetPropertyId()]++;
}

void RSNode::RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    animatingPropertyNum_[animation->GetPropertyId()]--;
    if (animatingPropertyNum_[animation->GetPropertyId()] == 0) {
        animation->SetPropertyOnAllAnimationFinish();
    }
    animations_.erase(animation->GetId());
}

void RSNode::FinishAnimationByProperty(const PropertyId& id)
{
    for (const auto& [animationId, animation] : animations_) {
        if (animation->GetPropertyId() == id) {
            animation->Finish();
        }
    }
}

const RSModifierExtractor& RSNode::GetStagingProperties() const
{
    return stagingPropertiesExtractor_;
}

void RSNode::AddAnimation(const std::shared_ptr<RSAnimation>& animation)
{
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to add animation, animation is null!");
        return;
    }

    auto animationId = animation->GetId();
    if (animations_.find(animationId) != animations_.end()) {
        ROSEN_LOGE("Failed to add animation, animation already exists!");
        return;
    }

    if (animation->GetDuration() <= 0) {
        FinishAnimationByProperty(animation->GetPropertyId());
    }

    AddAnimationInner(animation);
    animation->StartInner(std::static_pointer_cast<RSNode>(shared_from_this()));
}

void RSNode::RemoveAllAnimations()
{
    for (const auto& [id, animation] : animations_) {
        RemoveAnimation(animation);
    }
}

void RSNode::RemoveAnimation(const std::shared_ptr<RSAnimation>& animation)
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

void RSNode::SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption)
{
    motionPathOption_ = motionPathOption;
    UpdateModifierMotionPathOption();
}

const std::shared_ptr<RSMotionPathOption> RSNode::GetMotionPathOption() const
{
    return motionPathOption_;
}

bool RSNode::HasPropertyAnimation(const PropertyId& id)
{
    auto it = animatingPropertyNum_.find(id);
    return it != animatingPropertyNum_.end() && it->second > 0;
}

#define SET_ANIMATABLE_MODIFIER(propertyName, T, value, propertyType, defaultValue)                       \
    do {                                                                                                  \
        auto iter = propertyModifiers_.find(RSModifierType::propertyType);                                \
        if (iter != propertyModifiers_.end()) {                                                           \
            std::static_pointer_cast<RSAnimatableProperty<T>>(iter->second->GetProperty())->Set(value);   \
            break;                                                                                        \
        }                                                                                                 \
        auto property = std::make_shared<RSAnimatableProperty<T>>(defaultValue);                          \
        std::shared_ptr<RSModifier> modifier = std::make_shared<RS##propertyName##Modifier>(property);    \
        modifiers_.emplace(modifier->GetPropertyId(), modifier);                                          \
        propertyModifiers_.emplace(RSModifierType::propertyType, modifier);                               \
        modifier->AttachToNode(std::static_pointer_cast<RSNode>(shared_from_this()));                     \
        if (motionPathOption_ != nullptr && IsPathAnimatableModifier(RSModifierType::propertyType)) {     \
            modifier->SetMotionPathOption(motionPathOption_);                                             \
        }                                                                                                 \
        std::unique_ptr<RSCommand> cmd = std::make_unique<RSAddModifier>(                                 \
            GetId(), modifier->CreateRenderModifier());                                                   \
        auto transactionProxy = RSTransactionProxy::GetInstance();                                        \
        if (transactionProxy != nullptr) {                                                                \
            transactionProxy->AddCommand(cmd, IsRenderServiceNode(), GetFollowType(), GetId());           \
            if (NeedForcedSendToRemote()) {                                                               \
                std::unique_ptr<RSCommand> cmdForRemote =                                                 \
                    std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());           \
                transactionProxy->AddCommand(cmdForRemote, true, GetFollowType(), GetId());               \
            }                                                                                             \
            if (NeedSendExtraCommand()) {                                                                 \
                std::unique_ptr<RSCommand> extraCommand =                                                 \
                    std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());           \
                transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), GetId()); \
            }                                                                                             \
        }                                                                                                 \
        property->Set(value);                                                                             \
    } while (0)

#define SET_NONANIMATABLE_MODIFIER(propertyName, T, value, propertyType, defaultValue)                    \
    do {                                                                                                  \
        auto iter = propertyModifiers_.find(RSModifierType::propertyType);                                \
        if (iter != propertyModifiers_.end()) {                                                           \
            std::static_pointer_cast<RSProperty<T>>(iter->second->GetProperty())->Set(value);             \
            break;                                                                                        \
        }                                                                                                 \
        auto property = std::make_shared<RSProperty<T>>(value);                                           \
        std::shared_ptr<RSModifier> modifier = std::make_shared<RS##propertyName##Modifier>(property);    \
        modifiers_.emplace(modifier->GetPropertyId(), modifier);                                          \
        propertyModifiers_.emplace(RSModifierType::propertyType, modifier);                               \
        modifier->AttachToNode(std::static_pointer_cast<RSNode>(shared_from_this()));                     \
        std::unique_ptr<RSCommand> cmd = std::make_unique<RSAddModifier>(                                 \
            GetId(), modifier->CreateRenderModifier());                                                   \
        auto transactionProxy = RSTransactionProxy::GetInstance();                                        \
        if (transactionProxy != nullptr) {                                                                \
            transactionProxy->AddCommand(cmd, IsRenderServiceNode(), GetFollowType(), GetId());           \
            if (NeedForcedSendToRemote()) {                                                               \
                std::unique_ptr<RSCommand> cmdForRemote =                                                 \
                    std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());           \
                transactionProxy->AddCommand(cmdForRemote, true, GetFollowType(), GetId());               \
            }                                                                                             \
            if (NeedSendExtraCommand()) {                                                                 \
                std::unique_ptr<RSCommand> extraCommand =                                                 \
                    std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());           \
                transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), GetId()); \
            }                                                                                              \
        }                                                                                                 \
    } while (0)

// alpha
void RSNode::SetAlpha(float alpha)
{
    SET_ANIMATABLE_MODIFIER(Alpha, float, alpha, ALPHA, 1.f);
}

void RSNode::SetAlphaOffscreen(bool alphaOffscreen)
{
    SET_NONANIMATABLE_MODIFIER(AlphaOffscreen, bool, alphaOffscreen, ALPHA_OFFSCREEN, true);
}

// Bounds
void RSNode::SetBounds(const Vector4f& bounds)
{
    SET_ANIMATABLE_MODIFIER(Bounds, Vector4f, bounds, BOUNDS, Vector4f(0.f));
    OnBoundsSizeChanged();
}

void RSNode::SetBounds(float positionX, float positionY, float width, float height)
{
    SetBounds({ positionX, positionY, width, height });
}

void RSNode::SetBoundsWidth(float width)
{
    auto iter = propertyModifiers_.find(RSModifierType::BOUNDS);
    if (iter == propertyModifiers_.end()) {
        SetBounds(0.f, 0.f, width, 0.f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto bounds = property->Get();
    bounds.z_ = width;
    property->Set(bounds);
    OnBoundsSizeChanged();
}

void RSNode::SetBoundsHeight(float height)
{
    auto iter = propertyModifiers_.find(RSModifierType::BOUNDS);
    if (iter == propertyModifiers_.end()) {
        SetBounds(0.f, 0.f, 0.f, height);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto bounds = property->Get();
    bounds.w_ = height;
    property->Set(bounds);
    OnBoundsSizeChanged();
}

// Frame
void RSNode::SetFrame(const Vector4f& bounds)
{
    SET_ANIMATABLE_MODIFIER(Frame, Vector4f, bounds, FRAME, Vector4f(0.f));
}

void RSNode::SetFrame(float positionX, float positionY, float width, float height)
{
    SetFrame({ positionX, positionY, width, height });
}

void RSNode::SetFramePositionX(float positionX)
{
    auto iter = propertyModifiers_.find(RSModifierType::FRAME);
    if (iter == propertyModifiers_.end()) {
        SetFrame(positionX, 0.f, 0.f, 0.f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto frame = property->Get();
    frame.x_ = positionX;
    property->Set(frame);
}

void RSNode::SetFramePositionY(float positionY)
{
    auto iter = propertyModifiers_.find(RSModifierType::FRAME);
    if (iter == propertyModifiers_.end()) {
        SetFrame(0.f, positionY, 0.f, 0.f);
        return;
    }
    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto frame = property->Get();
    frame.y_ = positionY;
    property->Set(frame);
}

void RSNode::SetPositionZ(float positionZ)
{
    SET_ANIMATABLE_MODIFIER(PositionZ, float, positionZ, POSITION_Z, 0.f);
}

// pivot
void RSNode::SetPivot(const Vector2f& pivot)
{
    SET_ANIMATABLE_MODIFIER(Pivot, Vector2f, pivot, PIVOT, Vector2f(0.5f, 0.5f));
}

void RSNode::SetPivot(float pivotX, float pivotY)
{
    SetPivot({ pivotX, pivotY });
}

void RSNode::SetPivotX(float pivotX)
{
    auto iter = propertyModifiers_.find(RSModifierType::PIVOT);
    if (iter == propertyModifiers_.end()) {
        SetPivot(pivotX, 0.5f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto pivot = property->Get();
    pivot.x_ = pivotX;
    property->Set(pivot);
}

void RSNode::SetPivotY(float pivotY)
{
    auto iter = propertyModifiers_.find(RSModifierType::PIVOT);
    if (iter == propertyModifiers_.end()) {
        SetPivot(0.5f, pivotY);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto pivot = property->Get();
    pivot.y_ = pivotY;
    property->Set(pivot);
}

void RSNode::SetCornerRadius(float cornerRadius)
{
    SetCornerRadius(Vector4f(cornerRadius));
}

void RSNode::SetCornerRadius(const Vector4f& cornerRadius)
{
    SET_ANIMATABLE_MODIFIER(CornerRadius, Vector4f, cornerRadius, CORNER_RADIUS, Vector4f(0.f));
}

// transform
void RSNode::SetRotation(const Quaternion& quaternion)
{
    SET_ANIMATABLE_MODIFIER(Quaternion, Quaternion, quaternion, QUATERNION, Quaternion());
}

void RSNode::SetRotation(float degree)
{
    SET_ANIMATABLE_MODIFIER(Rotation, float, degree, ROTATION, 0.f);
}

void RSNode::SetRotation(float degreeX, float degreeY, float degreeZ)
{
    SetRotationX(degreeX);
    SetRotationY(degreeY);
    SetRotation(degreeZ);
}

void RSNode::SetRotationX(float degree)
{
    SET_ANIMATABLE_MODIFIER(RotationX, float, degree, ROTATION_X, 0.f);
}

void RSNode::SetRotationY(float degree)
{
    SET_ANIMATABLE_MODIFIER(RotationY, float, degree, ROTATION_Y, 0.f);
}

void RSNode::SetTranslate(const Vector2f& translate)
{
    SET_ANIMATABLE_MODIFIER(Translate, Vector2f, translate, TRANSLATE, Vector2f(0.f, 0.f));
}

void RSNode::SetTranslate(float translateX, float translateY, float translateZ)
{
    SetTranslate({ translateX, translateY });
    SetTranslateZ(translateZ);
}
void RSNode::SetTranslateX(float translate)
{
    auto iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
    if (iter == propertyModifiers_.end()) {
        SetTranslate({ translate, 0.f });
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto trans = property->Get();
    trans.x_ = translate;
    property->Set(trans);
}

void RSNode::SetTranslateY(float translate)
{
    auto iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
    if (iter == propertyModifiers_.end()) {
        SetTranslate({ 0.f, translate });
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto trans = property->Get();
    trans.y_ = translate;
    property->Set(trans);
}

void RSNode::SetTranslateZ(float translate)
{
    SET_ANIMATABLE_MODIFIER(TranslateZ, float, translate, TRANSLATE_Z, 0.f);
}

void RSNode::SetScale(float scale)
{
    SetScale({ scale, scale });
}

void RSNode::SetScale(float scaleX, float scaleY)
{
    SetScale({ scaleX, scaleY });
}

void RSNode::SetScale(const Vector2f& scale)
{
    SET_ANIMATABLE_MODIFIER(Scale, Vector2f, scale, SCALE, Vector2f(1.f, 1.f));
}

void RSNode::SetScaleX(float scaleX)
{
    auto iter = propertyModifiers_.find(RSModifierType::SCALE);
    if (iter == propertyModifiers_.end()) {
        SetScale(scaleX, 1.f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto scale = property->Get();
    scale.x_ = scaleX;
    property->Set(scale);
}

void RSNode::SetScaleY(float scaleY)
{
    auto iter = propertyModifiers_.find(RSModifierType::SCALE);
    if (iter == propertyModifiers_.end()) {
        SetScale(1.f, scaleY);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto scale = property->Get();
    scale.y_ = scaleY;
    property->Set(scale);
}

// foreground
void RSNode::SetForegroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SET_ANIMATABLE_MODIFIER(ForegroundColor, Color, color, FOREGROUND_COLOR, RgbPalette::Transparent());
}

void RSNode::SetBackgroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SET_ANIMATABLE_MODIFIER(BackgroundColor, Color, color, BACKGROUND_COLOR, RgbPalette::Transparent());
}

void RSNode::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
    SET_NONANIMATABLE_MODIFIER(BackgroundShader, std::shared_ptr<RSShader>, shader, BACKGROUND_SHADER, nullptr);
}

// background
void RSNode::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    SET_NONANIMATABLE_MODIFIER(BgImage, std::shared_ptr<RSImage>, image, BG_IMAGE, nullptr);
}

void RSNode::SetBgImageSize(float width, float height)
{
    SetBgImageWidth(width);
    SetBgImageHeight(height);
}

void RSNode::SetBgImageWidth(float width)
{
    SET_ANIMATABLE_MODIFIER(BgImageWidth, float, width, BG_IMAGE_WIDTH, 0.f);
}

void RSNode::SetBgImageHeight(float height)
{
    SET_ANIMATABLE_MODIFIER(BgImageHeight, float, height, BG_IMAGE_HEIGHT, 0.f);
}

void RSNode::SetBgImagePosition(float positionX, float positionY)
{
    SetBgImagePositionX(positionX);
    SetBgImagePositionY(positionY);
}

void RSNode::SetBgImagePositionX(float positionX)
{
    SET_ANIMATABLE_MODIFIER(BgImagePositionX, float, positionX, BG_IMAGE_POSITION_X, 0.f);
}

void RSNode::SetBgImagePositionY(float positionY)
{
    SET_ANIMATABLE_MODIFIER(BgImagePositionY, float, positionY, BG_IMAGE_POSITION_Y, 0.f);
}

// border
void RSNode::SetBorderColor(uint32_t colorValue)
{
    SetBorderColor(colorValue, colorValue, colorValue, colorValue);
}

void RSNode::SetBorderColor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    Vector4<Color> color(Color::FromArgbInt(left), Color::FromArgbInt(top),
                         Color::FromArgbInt(right), Color::FromArgbInt(bottom));
    SET_ANIMATABLE_MODIFIER(
        BorderColor, Vector4<Color>, color, BORDER_COLOR, Vector4<Color>(RgbPalette::Transparent()));
}

void RSNode::SetBorderColor(const Vector4<Color>& color)
{
    SET_ANIMATABLE_MODIFIER(
        BorderColor, Vector4<Color>, color, BORDER_COLOR, Vector4<Color>(RgbPalette::Transparent()));
}

void RSNode::SetBorderWidth(float width)
{
    SetBorderWidth(width, width, width, width);
}

void RSNode::SetBorderWidth(float left, float top, float right, float bottom)
{
    Vector4f width(left, top, right, bottom);
    SET_ANIMATABLE_MODIFIER(BorderWidth, Vector4f, width, BORDER_WIDTH, Vector4f(0.f));
}

void RSNode::SetBorderWidth(const Vector4f& width)
{
    SET_ANIMATABLE_MODIFIER(BorderWidth, Vector4f, width, BORDER_WIDTH, Vector4f(0.f));
}

void RSNode::SetBorderStyle(uint32_t styleValue)
{
    SetBorderStyle(styleValue, styleValue, styleValue, styleValue);
}

void RSNode::SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    Vector4<uint32_t> style(left, top, right, bottom);
    SET_NONANIMATABLE_MODIFIER(
        BorderStyle, Vector4<uint32_t>, style, BORDER_STYLE, Vector4<uint32_t>(BorderStyle::NONE));
}

void RSNode::SetBorderStyle(const Vector4<BorderStyle>& style)
{
    Vector4<uint32_t> styles(static_cast<uint32_t>(style.x_), static_cast<uint32_t>(style.y_),
                             static_cast<uint32_t>(style.z_), static_cast<uint32_t>(style.w_));
    SET_NONANIMATABLE_MODIFIER(
        BorderStyle, Vector4<uint32_t>, styles, BORDER_STYLE, Vector4<uint32_t>(BorderStyle::NONE));
}

void RSNode::SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter)
{
    SET_ANIMATABLE_MODIFIER(BackgroundFilter, std::shared_ptr<RSFilter>, backgroundFilter, BACKGROUND_FILTER, nullptr);
}

void RSNode::SetFilter(const std::shared_ptr<RSFilter>& filter)
{
    SET_ANIMATABLE_MODIFIER(Filter, std::shared_ptr<RSFilter>, filter, FILTER, nullptr);
}

void RSNode::SetCompositingFilter(const std::shared_ptr<RSFilter>& compositingFilter) {}

void RSNode::SetShadowColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SET_ANIMATABLE_MODIFIER(ShadowColor, Color, color, SHADOW_COLOR, Color::FromArgbInt(DEFAULT_SPOT_COLOR));
}

void RSNode::SetShadowOffset(float offsetX, float offsetY)
{
    SetShadowOffsetX(offsetX);
    SetShadowOffsetY(offsetY);
}

void RSNode::SetShadowOffsetX(float offsetX)
{
    SET_ANIMATABLE_MODIFIER(ShadowOffsetX, float, offsetX, SHADOW_OFFSET_X, DEFAULT_SHADOW_OFFSET_X);
}

void RSNode::SetShadowOffsetY(float offsetY)
{
    SET_ANIMATABLE_MODIFIER(ShadowOffsetY, float, offsetY, SHADOW_OFFSET_Y, DEFAULT_SHADOW_OFFSET_Y);
}

void RSNode::SetShadowAlpha(float alpha)
{
    SET_ANIMATABLE_MODIFIER(ShadowAlpha, float, alpha, SHADOW_ALPHA, 0.f);
}

void RSNode::SetShadowElevation(float elevation)
{
    SET_ANIMATABLE_MODIFIER(ShadowElevation, float, elevation, SHADOW_ELEVATION, 0.f);
}

void RSNode::SetShadowRadius(float radius)
{
    SET_ANIMATABLE_MODIFIER(ShadowRadius, float, radius, SHADOW_RADIUS, DEFAULT_SHADOW_RADIUS);
}

void RSNode::SetShadowPath(const std::shared_ptr<RSPath>& shadowPath)
{
    SET_NONANIMATABLE_MODIFIER(ShadowPath, std::shared_ptr<RSPath>, shadowPath, SHADOW_PATH, nullptr);
}

void RSNode::SetFrameGravity(Gravity gravity)
{
    SET_NONANIMATABLE_MODIFIER(FrameGravity, Gravity, gravity, FRAME_GRAVITY, Gravity::DEFAULT);
}

void RSNode::SetClipBounds(const std::shared_ptr<RSPath>& path)
{
    SET_NONANIMATABLE_MODIFIER(ClipBounds, std::shared_ptr<RSPath>, path, CLIP_BOUNDS, nullptr);
}

void RSNode::SetClipToBounds(bool clipToBounds)
{
    SET_NONANIMATABLE_MODIFIER(ClipToBounds, bool, clipToBounds, CLIP_TO_BOUNDS, false);
}

void RSNode::SetClipToFrame(bool clipToFrame)
{
    SET_NONANIMATABLE_MODIFIER(ClipToFrame, bool, clipToFrame, CLIP_TO_FRAME, false);
}

void RSNode::SetVisible(bool visible)
{
    SET_NONANIMATABLE_MODIFIER(Visible, bool, visible, VISIBLE, true);
}

void RSNode::SetMask(const std::shared_ptr<RSMask>& mask)
{
    SET_NONANIMATABLE_MODIFIER(Mask, std::shared_ptr<RSMask>, mask, MASK, nullptr);
}

void RSNode::NotifyTransition(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
{
    // temporary fix for multithread issue in implicit animator
    UpdateImplicitAnimator();
    if (implicitAnimator_ == nullptr) {
        ROSEN_LOGE("Failed to notify transition, implicit animator is null!");
        return;
    }

    if (!implicitAnimator_->NeedImplicitAnimation()) {
        return;
    }

    implicitAnimator_->BeginImplicitTransition(effect);
    implicitAnimator_->CreateImplicitTransition(*this, isTransitionIn);
    implicitAnimator_->EndImplicitTransition();
}

void RSNode::OnAddChildren()
{
    if (transitionEffect_ != nullptr) {
        NotifyTransition(transitionEffect_, true);
    }
}

void RSNode::OnRemoveChildren()
{
    if (transitionEffect_ != nullptr) {
        NotifyTransition(transitionEffect_, false);
    }
}

void RSNode::AnimationFinish(AnimationId animationId)
{
    auto animationItr = animations_.find(animationId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("Failed to find animation[%" PRIu64 "]!", animationId);
        return;
    }

    auto animation = animationItr->second;
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to finish animation[%" PRIu64 "], animation is null!", animationId);
        return;
    }

    animation->CallFinishCallback();
    RemoveAnimationInner(animation);
}

void RSNode::SetPaintOrder(bool drawContentLast)
{
    drawContentLast_ = drawContentLast;
}

void RSNode::ClearAllModifiers()
{
    if (animationManager_ == nullptr) {
        animationManager_ = RSAnimationManagerMap::Instance()->GetAnimationManager(gettid());
    }
    for (auto& [id, modifier] : modifiers_) {
        modifier->DetachFromNode();
        animationManager_->RemoveProperty(id);
    }
}

void RSNode::AddModifier(const std::shared_ptr<RSModifier>& modifier)
{
    if (!modifier || modifiers_.count(modifier->GetPropertyId())) {
        return;
    }
    if (motionPathOption_ != nullptr && IsPathAnimatableModifier(modifier->GetModifierType())) {
        modifier->SetMotionPathOption(motionPathOption_);
    }
    modifiers_.insert({ modifier->GetPropertyId(), modifier });
    modifier->AttachToNode(std::static_pointer_cast<RSNode>(shared_from_this()));
    std::unique_ptr<RSCommand> command = std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        if (NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> cmdForRemote =
                std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
            transactionProxy->AddCommand(cmdForRemote, true, GetFollowType(), GetId());
        }
        if (NeedSendExtraCommand()) {
            std::unique_ptr<RSCommand> extraCommand =
                std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
            transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), GetId());
        }
    }
}

void RSNode::RemoveModifier(const std::shared_ptr<RSModifier>& modifier)
{
    if (!modifier) {
        return;
    }
    auto iter = modifiers_.find(modifier->GetPropertyId());
    if (iter == modifiers_.end()) {
        return;
    }

    modifiers_.erase(iter);
    modifier->DetachFromNode();
    if (animationManager_ == nullptr) {
        animationManager_ = RSAnimationManagerMap::Instance()->GetAnimationManager(gettid());
    }
    animationManager_->RemoveProperty(modifier->GetPropertyId());
    std::unique_ptr<RSCommand> command = std::make_unique<RSRemoveModifier>(GetId(), modifier->GetPropertyId());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        if (NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> cmdForRemote =
                std::make_unique<RSRemoveModifier>(GetId(), modifier->GetPropertyId());
            transactionProxy->AddCommand(cmdForRemote, true, GetFollowType(), GetId());
        }
        if (NeedSendExtraCommand()) {
            std::unique_ptr<RSCommand> extraCommand =
                std::make_unique<RSRemoveModifier>(GetId(), modifier->GetPropertyId());
            transactionProxy->AddCommand(extraCommand, !IsRenderServiceNode(), GetFollowType(), GetId());
        }
    }
}

const std::shared_ptr<RSModifier> RSNode::GetModifier(const PropertyId& propertyId)
{
    auto iter = modifiers_.find(propertyId);
    if (iter != modifiers_.end()) {
        return iter->second;
    }

    return {};
}

void RSNode::UpdateModifierMotionPathOption()
{
    for (auto& [type, modifier] : propertyModifiers_) {
        if (IsPathAnimatableModifier(type)) {
            modifier->SetMotionPathOption(motionPathOption_);
        }
    }
    for (auto& [id, modifier] : modifiers_) {
        if (IsPathAnimatableModifier(modifier->GetModifierType())) {
            modifier->SetMotionPathOption(motionPathOption_);
        }
    }
}

void RSNode::UpdateExtendedModifier(const PropertyId& id)
{
    auto modifier = GetModifier(id);
    if (modifier != nullptr) {
        modifier->UpdateToRender();
    }
}

std::string RSNode::DumpNode(int depth) const
{
    std::stringstream ss;
    ss << RSBaseNode::DumpNode(depth);
    if (!animations_.empty()) {
        ss << " animation:" << std::to_string(animations_.size());
    }
    ss << " " << GetStagingProperties().Dump();
    return ss.str();
}

void RSNode::UpdateImplicitAnimator()
{
    auto tid = gettid();
    if (tid == implicitAnimatorTid_) {
        return;
    }
    implicitAnimatorTid_ = tid;
    implicitAnimator_ = RSImplicitAnimatorMap::Instance().GetAnimator(tid);
}
} // namespace Rosen
} // namespace OHOS
