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

#include "ui/rs_node.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

#include "rs_trace.h"
#include "sandbox_utils.h"

#include "animation/rs_animation.h"
#include "animation/rs_animation_group.h"
#include "animation/rs_animation_callback.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "animation/rs_render_particle_animation.h"
#include "command/rs_base_node_command.h"
#include "command/rs_node_command.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_geometry.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_canvas_drawing_node.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_frame_rate_policy.h"
#include "ui/rs_node.h"
#include "ui/rs_proxy_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []() -> int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif

namespace OHOS {
namespace Rosen {
namespace {
static bool g_isUniRenderEnabled = false;
static const std::unordered_map<RSUINodeType, std::string> RSUINodeTypeStrs = {
    {RSUINodeType::UNKNOW,              "UNKNOW"},
    {RSUINodeType::DISPLAY_NODE,        "DisplayNode"},
    {RSUINodeType::RS_NODE,             "RsNode"},
    {RSUINodeType::SURFACE_NODE,        "SurfaceNode"},
    {RSUINodeType::PROXY_NODE,          "ProxyNode"},
    {RSUINodeType::CANVAS_NODE,         "CanvasNode"},
    {RSUINodeType::ROOT_NODE,           "RootNode"},
    {RSUINodeType::EFFECT_NODE,         "EffectNode"},
    {RSUINodeType::CANVAS_DRAWING_NODE, "CanvasDrawingNode"},
};
std::once_flag flag_;
bool IsPathAnimatableModifier(const RSModifierType& type)
{
    if (type == RSModifierType::BOUNDS || type == RSModifierType::FRAME || type == RSModifierType::TRANSLATE) {
        return true;
    }
    return false;
}
}

RSNode::RSNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode)
    : isRenderServiceNode_(isRenderServiceNode), isTextureExportNode_(isTextureExportNode),
    id_(id), stagingPropertiesExtractor_(this), showingPropertiesFreezer_(id)
{
    InitUniRenderEnabled();
    if (g_isUniRenderEnabled && isTextureExportNode) {
        std::call_once(flag_, []() {
            auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
            auto transactionProxy = RSTransactionProxy::GetInstance();
            if (transactionProxy != nullptr) {
                transactionProxy->SetRenderThreadClient(renderThreadClient);
            }
        });
    }
    UpdateImplicitAnimator();
}

RSNode::RSNode(bool isRenderServiceNode, bool isTextureExportNode)
    : RSNode(isRenderServiceNode, GenerateId(), isTextureExportNode) {}

RSNode::~RSNode()
{
    FallbackAnimationsToRoot();
    ClearAllModifiers();

    // break current (ui) parent-child relationship.
    // render nodes will check if its child is expired and remove it, no need to manually remove it here.
    if (auto parentPtr = RSNodeMap::Instance().GetNode(parent_)) {
        parentPtr->RemoveChildById(id_);
    }
    // unregister node from node map
    RSNodeMap::MutableInstance().UnregisterNode(id_);

    // tell RT/RS to destroy related render node
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr || skipDestroyCommandInDestructor_) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(id_);
    transactionProxy->AddCommand(command, IsRenderServiceNode());
}

void RSNode::OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::function<void()>& finishCallback)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return;
    }

    std::shared_ptr<AnimationFinishCallback> animationFinishCallback;
    if (finishCallback != nullptr) {
        animationFinishCallback =
            std::make_shared<AnimationFinishCallback>(finishCallback, timingProtocol.GetFinishCallbackType());
    }
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(animationFinishCallback));
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

void RSNode::SetFrameNodeInfo(int32_t id, std::string tag)
{
    frameNodeId_ = id;
    frameNodeTag_ = tag;
}

int32_t RSNode::GetFrameNodeId()
{
    return frameNodeId_;
}

std::string RSNode::GetFrameNodeTag()
{
    return frameNodeTag_;
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

void RSNode::AddDurationKeyFrame(
    int duration, const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to add keyframe, implicit animator is null!");
        return;
    }

    implicitAnimator->BeginImplicitDurationKeyFrameAnimation(duration, timingCurve);
    propertyCallback();
    implicitAnimator->EndImplicitDurationKeyFrameAnimation();
}

bool RSNode::IsImplicitAnimationOpen()
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    return implicitAnimator && implicitAnimator->NeedImplicitAnimation();
}

std::vector<std::shared_ptr<RSAnimation>> RSNode::Animate(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback,
    const std::function<void()>& finishCallback, const std::function<void()>& repeatCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }

    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return {};
    }
    std::shared_ptr<AnimationFinishCallback> animationFinishCallback;
    if (finishCallback != nullptr) {
        animationFinishCallback =
            std::make_shared<AnimationFinishCallback>(finishCallback, timingProtocol.GetFinishCallbackType());
    }
    std::shared_ptr<AnimationRepeatCallback> animationRepeatCallback;
    if (repeatCallback != nullptr) {
        animationRepeatCallback = std::make_shared<AnimationRepeatCallback>(repeatCallback);
    }
    implicitAnimator->OpenImplicitAnimation(
        timingProtocol, timingCurve, std::move(animationFinishCallback), std::move(animationRepeatCallback));
    propertyCallback();
    return implicitAnimator->CloseImplicitAnimation();
}

std::vector<std::shared_ptr<RSAnimation>> RSNode::AnimateWithCurrentOptions(
    const PropertyCallback& propertyCallback, const std::function<void()>& finishCallback, bool timingSensitive)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }
    if (finishCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, finish callback is null!");
        propertyCallback();
        return {};
    }

    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return {};
    }
    auto finishCallbackType =
        timingSensitive ? FinishCallbackType::TIME_SENSITIVE : FinishCallbackType::TIME_INSENSITIVE;
    // re-use the current options and replace the finish callback
    auto animationFinishCallback = std::make_shared<AnimationFinishCallback>(finishCallback, finishCallbackType);
    implicitAnimator->OpenImplicitAnimation(std::move(animationFinishCallback));
    propertyCallback();
    return implicitAnimator->CloseImplicitAnimation();
}

std::vector<std::shared_ptr<RSAnimation>> RSNode::AnimateWithCurrentCallback(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
    const PropertyCallback& propertyCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }

    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return {};
    }
    // re-use the current finish callback and replace the options
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve);
    propertyCallback();
    return implicitAnimator->CloseImplicitAnimation();
}

void RSNode::ExecuteWithoutAnimation(
    const PropertyCallback& callback, std::shared_ptr<RSImplicitAnimator> implicitAnimator)
{
    if (callback == nullptr) {
        ROSEN_LOGE("Failed to execute without animation, property callback is null!");
        return;
    }
    if (implicitAnimator == nullptr) {
        implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    }
    if (implicitAnimator == nullptr) {
        callback();
    } else {
        implicitAnimator->ExecuteWithoutAnimation(callback);
    }
}

void RSNode::FallbackAnimationsToRoot()
{
    auto target = RSNodeMap::Instance().GetAnimationFallbackNode();

    if (target == nullptr) {
        ROSEN_LOGE("Failed to move animation to root, root node is null!");
        return;
    }
    for (auto& [unused, animation] : animations_) {
        if (animation && animation->GetRepeatCount() == -1) {
            continue;
        }
        std::unique_lock<std::mutex> lock(animationMutex_);
        target->AddAnimationInner(std::move(animation));
    }
    std::unique_lock<std::mutex> lock(animationMutex_);
    animations_.clear();
}

void RSNode::AddAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    animations_.emplace(animation->GetId(), animation);
    animatingPropertyNum_[animation->GetPropertyId()]++;
}

void RSNode::RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    std::unique_lock<std::mutex> lock(animationMutex_);
    if (auto it = animatingPropertyNum_.find(animation->GetPropertyId()); it != animatingPropertyNum_.end()) {
        it->second--;
        if (it->second == 0) {
            animatingPropertyNum_.erase(it);
            animation->SetPropertyOnAllAnimationFinish();
        }
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

void RSNode::CancelAnimationByProperty(const PropertyId& id, const bool needForceSync)
{
    animatingPropertyNum_.erase(id);
    std::vector<std::shared_ptr<RSAnimation>> toBeRemoved;
    {
        std::unique_lock<std::mutex> lock(animationMutex_, std::defer_lock);
        if (!lock.try_lock()) {
            // The Arkui component has logic to cancel animation within the callback of another animation. However, this
            // approach may cause a deadlock. Although it is a dirty workaround, it currently works as intended.
            FinishAnimationByProperty(id);
            return;
        }
        EraseIf(animations_, [id, &toBeRemoved](const auto& pair) {
            if (pair.second && (pair.second->GetPropertyId() == id)) {
                toBeRemoved.emplace_back(pair.second);
                return true;
            }
            return false;
        });
    }
    // Destroy the cancelled animations outside the lock, since destroying them may trigger OnFinish callbacks, and
    // callbacks may add/remove other animations, doing this with the lock would cause a deadlock.
    toBeRemoved.clear();

    if (needForceSync) {
        // Avoid animation on current property not cancelled in RS
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            ROSEN_LOGE("RSNode::CancelAnimationByProperty, failed to get RSTransactionProxy!");
            return;
        }

        std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCancel>(id_, id);
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
        if (NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSAnimationCancel>(id_, id);
            transactionProxy->AddCommand(commandForRemote, true, GetFollowType(), id_);
        }
    }
}

const RSModifierExtractor& RSNode::GetStagingProperties() const
{
    return stagingPropertiesExtractor_;
}

const RSShowingPropertiesFreezer& RSNode::GetShowingProperties() const
{
    return showingPropertiesFreezer_;
}

void RSNode::AddAnimation(const std::shared_ptr<RSAnimation>& animation)
{
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to add animation, animation is null!");
        return;
    }

    auto animationId = animation->GetId();
    {
        std::unique_lock<std::mutex> lock(animationMutex_);
        if (animations_.find(animationId) != animations_.end()) {
            ROSEN_LOGE("Failed to add animation, animation already exists!");
            return;
        }
    }

    // Note: Animation cancellation logic is now handled by RSImplicitAnimator. The code below might cause Spring
    // Animations with a zero duration to not inherit velocity correctly, an issue slated for future resolution.
    // This code is retained to ensure backward compatibility with specific arkui component animations.
    if (animation->GetDuration() <= 0 && id_ != 0) {
        FinishAnimationByProperty(animation->GetPropertyId());
    }

    {
        std::unique_lock<std::mutex> lock(animationMutex_);
        AddAnimationInner(animation);
    }
    animation->StartInner(shared_from_this());
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
    std::unique_lock<std::mutex> lock(animationMutex_);
    auto it = animatingPropertyNum_.find(id);
    return it != animatingPropertyNum_.end() && it->second > 0;
}

template<typename ModifierName, typename PropertyName, typename T>
void RSNode::SetProperty(RSModifierType modifierType, T value)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    auto iter = propertyModifiers_.find(modifierType);
    if (iter != propertyModifiers_.end()) {
        auto property = std::static_pointer_cast<PropertyName>(iter->second->GetProperty());
        if (property == nullptr) {
            ROSEN_LOGE("RSNode::SetProperty: failed to set property, property is null!");
            return;
        }
        property->Set(value);
        return;
    }
    auto property = std::make_shared<PropertyName>(value);
    auto propertyModifier = std::make_shared<ModifierName>(property);
    propertyModifiers_.emplace(modifierType, propertyModifier);
    AddModifier(propertyModifier);
}

// alpha
void RSNode::SetAlpha(float alpha)
{
    SetProperty<RSAlphaModifier, RSAnimatableProperty<float>>(RSModifierType::ALPHA, alpha);
}

void RSNode::SetAlphaOffscreen(bool alphaOffscreen)
{
    SetProperty<RSAlphaOffscreenModifier, RSProperty<bool>>(RSModifierType::ALPHA_OFFSCREEN, alphaOffscreen);
}

// Bounds
void RSNode::SetBounds(const Vector4f& bounds)
{
    SetProperty<RSBoundsModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::BOUNDS, bounds);
    OnBoundsSizeChanged();
}

void RSNode::SetBounds(float positionX, float positionY, float width, float height)
{
    SetBounds({ positionX, positionY, width, height });
}

void RSNode::SetBoundsWidth(float width)
{
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::BOUNDS);
        if (iter == propertyModifiers_.end()) {
            SetBounds(0.f, 0.f, width, 0.f);
            return;
        }
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
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::BOUNDS);
        if (iter == propertyModifiers_.end()) {
            SetBounds(0.f, 0.f, 0.f, height);
            return;
        }
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
    SetProperty<RSFrameModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::FRAME, bounds);
}

void RSNode::SetFrame(float positionX, float positionY, float width, float height)
{
    SetFrame({ positionX, positionY, width, height });
}

void RSNode::SetFramePositionX(float positionX)
{
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::FRAME);
        if (iter == propertyModifiers_.end()) {
            SetFrame(positionX, 0.f, 0.f, 0.f);
            return;
        }
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
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::FRAME);
        if (iter == propertyModifiers_.end()) {
            SetFrame(0.f, positionY, 0.f, 0.f);
            return;
        }
    }
    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto frame = property->Get();
    frame.y_ = positionY;
    property->Set(frame);
}

void RSNode::SetSandBox(std::optional<Vector2f> parentPosition)
{
    if (!parentPosition.has_value()) {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        auto iter = propertyModifiers_.find(RSModifierType::SANDBOX);
        if (iter != propertyModifiers_.end()) {
            RemoveModifier(iter->second);
            propertyModifiers_.erase(iter);
        }
        return;
    }
    SetProperty<RSSandBoxModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::SANDBOX, parentPosition.value());
}

void RSNode::SetPositionZ(float positionZ)
{
    SetProperty<RSPositionZModifier, RSAnimatableProperty<float>>(RSModifierType::POSITION_Z, positionZ);
}

// pivot
void RSNode::SetPivot(const Vector2f& pivot)
{
    SetProperty<RSPivotModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::PIVOT, pivot);
}

void RSNode::SetPivot(float pivotX, float pivotY)
{
    SetPivot({ pivotX, pivotY });
}

void RSNode::SetPivotX(float pivotX)
{
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::PIVOT);
        if (iter == propertyModifiers_.end()) {
            SetPivot(pivotX, 0.5f);
            return;
        }
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
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::PIVOT);
        if (iter == propertyModifiers_.end()) {
            SetPivot(0.5f, pivotY);
            return;
        }
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto pivot = property->Get();
    pivot.y_ = pivotY;
    property->Set(pivot);
}

void RSNode::SetPivotZ(const float pivotZ)
{
    SetProperty<RSPivotZModifier, RSAnimatableProperty<float>>(RSModifierType::PIVOT_Z, pivotZ);
}

void RSNode::SetCornerRadius(float cornerRadius)
{
    SetCornerRadius(Vector4f(cornerRadius));
}

void RSNode::SetCornerRadius(const Vector4f& cornerRadius)
{
    SetProperty<RSCornerRadiusModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::CORNER_RADIUS, cornerRadius);
}

// transform
void RSNode::SetRotation(const Quaternion& quaternion)
{
    SetProperty<RSQuaternionModifier, RSAnimatableProperty<Quaternion>>(RSModifierType::QUATERNION, quaternion);
}

void RSNode::SetRotation(float degree)
{
    SetProperty<RSRotationModifier, RSAnimatableProperty<float>>(RSModifierType::ROTATION, degree);
}

void RSNode::SetRotation(float degreeX, float degreeY, float degreeZ)
{
    SetRotationX(degreeX);
    SetRotationY(degreeY);
    SetRotation(degreeZ);
}

void RSNode::SetRotationX(float degree)
{
    SetProperty<RSRotationXModifier, RSAnimatableProperty<float>>(RSModifierType::ROTATION_X, degree);
}

void RSNode::SetRotationY(float degree)
{
    SetProperty<RSRotationYModifier, RSAnimatableProperty<float>>(RSModifierType::ROTATION_Y, degree);
}

void RSNode::SetCameraDistance(float cameraDistance)
{
    SetProperty<RSCameraDistanceModifier, RSAnimatableProperty<float>>(RSModifierType::CAMERA_DISTANCE, cameraDistance);
}

void RSNode::SetTranslate(const Vector2f& translate)
{
    SetProperty<RSTranslateModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::TRANSLATE, translate);
}

void RSNode::SetTranslate(float translateX, float translateY, float translateZ)
{
    SetTranslate({ translateX, translateY });
    SetTranslateZ(translateZ);
}
void RSNode::SetTranslateX(float translate)
{
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
        if (iter == propertyModifiers_.end()) {
            SetTranslate({ translate, 0.f });
            return;
        }
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
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
        if (iter == propertyModifiers_.end()) {
            SetTranslate({ 0.f, translate });
            return;
        }
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
    SetProperty<RSTranslateZModifier, RSAnimatableProperty<float>>(RSModifierType::TRANSLATE_Z, translate);
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
    SetProperty<RSScaleModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::SCALE, scale);
}

void RSNode::SetScaleX(float scaleX)
{
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::SCALE);
        if (iter == propertyModifiers_.end()) {
            SetScale(scaleX, 1.f);
            return;
        }
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
    std::map<RSModifierType, std::shared_ptr<RSModifier>>::iterator iter;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        iter = propertyModifiers_.find(RSModifierType::SCALE);
        if (iter == propertyModifiers_.end()) {
            SetScale(1.f, scaleY);
            return;
        }
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto scale = property->Get();
    scale.y_ = scaleY;
    property->Set(scale);
}

void RSNode::SetSkew(float skew)
{
    SetSkew({ skew, skew });
}

void RSNode::SetSkew(float skewX, float skewY)
{
    SetSkew({ skewX, skewY });
}

void RSNode::SetSkew(const Vector2f& skew)
{
    SetProperty<RSSkewModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::SKEW, skew);
}

void RSNode::SetSkewX(float skewX)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    auto iter = propertyModifiers_.find(RSModifierType::SKEW);
    if (iter == propertyModifiers_.end()) {
        SetSkew(skewX, 0.f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto skew = property->Get();
    skew.x_ = skewX;
    property->Set(skew);
}

void RSNode::SetSkewY(float skewY)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    auto iter = propertyModifiers_.find(RSModifierType::SKEW);
    if (iter == propertyModifiers_.end()) {
        SetSkew(0.f, skewY);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto skew = property->Get();
    skew.y_ = skewY;
    property->Set(skew);
}

// Set the foreground color of the control
void RSNode::SetEnvForegroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SetProperty<RSEnvForegroundColorModifier, RSAnimatableProperty<Color>>(RSModifierType::ENV_FOREGROUND_COLOR, color);
}

// Set the foreground color strategy of the control
void RSNode::SetEnvForegroundColorStrategy(ForegroundColorStrategyType strategyType)
{
    SetProperty<RSEnvForegroundColorStrategyModifier,
        RSProperty<ForegroundColorStrategyType>>(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, strategyType);
}

// Set ParticleParams
void RSNode::SetParticleParams(std::vector<ParticleParams>& particleParams, const std::function<void()>& finishCallback)
{
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams;
    for (size_t i = 0; i < particleParams.size(); i++) {
        particlesRenderParams.push_back(particleParams[i].SetParamsToRenderParticle());
    }
    SetParticleDrawRegion(particleParams);
    auto property = std::make_shared<RSPropertyBase>();
    auto propertyId = property->GetId();
    auto uiAnimation = std::make_shared<RSAnimationGroup>();
    auto animationId = uiAnimation->GetId();
    AddAnimation(uiAnimation);
    if (finishCallback != nullptr) {
        uiAnimation->SetFinishCallback(std::make_shared<AnimationFinishCallback>(finishCallback));
    }
    auto animation =
        std::make_shared<RSRenderParticleAnimation>(animationId, propertyId, std::move(particlesRenderParams));

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCreateParticle>(GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        if (NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> cmdForRemote =
                std::make_unique<RSAnimationCreateParticle>(GetId(), animation);
            transactionProxy->AddCommand(cmdForRemote, true, GetFollowType(), GetId());
        }
    }
}

void RSNode::SetParticleDrawRegion(std::vector<ParticleParams>& particleParams)
{
    Vector4f bounds = GetStagingProperties().GetBounds();
    float left = 0.f;
    float top = 0.f;
    float right = bounds.z_;
    float bottom = bounds.w_;
    for (size_t i = 0; i < particleParams.size(); i++) {
        auto particleType = particleParams[i].emitterConfig_.type_;
        auto position = particleParams[i].emitterConfig_.position_;
        auto emitSize = particleParams[i].emitterConfig_.emitSize_;
        float scaleMax = particleParams[i].scale_.val_.end_;
        if (particleType == ParticleType::POINTS) {
            auto radius = particleParams[i].emitterConfig_.radius_;
            auto radiusMax = radius * scaleMax;
            left = std::min(left - radiusMax, position.x_ - radiusMax);
            top = std::min(top - radiusMax, position.y_ - radiusMax);
            right = std::max(right + radiusMax + radiusMax, position.x_ + emitSize.x_ + radiusMax + radiusMax);
            bottom = std::max(bottom + radiusMax + radiusMax, position.y_ + emitSize.y_ + radiusMax + radiusMax);
        } else {
            float imageSizeWidth = 0.f;
            float imageSizeHeight = 0.f;
            auto image = particleParams[i].emitterConfig_.image_;
            auto imageSize = particleParams[i].emitterConfig_.imageSize_;
            if (image == nullptr)
                continue;
            auto pixelMap = image->GetPixelMap();
            if (pixelMap != nullptr) {
                imageSizeWidth = std::max(imageSize.x_, static_cast<float>(pixelMap->GetWidth()));
                imageSizeHeight = std::max(imageSize.y_, static_cast<float>(pixelMap->GetHeight()));
            }
            float imageSizeWidthMax = imageSizeWidth * scaleMax;
            float imageSizeHeightMax = imageSizeHeight * scaleMax;
            left = std::min(left - imageSizeWidthMax, position.x_ - imageSizeWidthMax);
            top = std::min(top - imageSizeHeightMax, position.y_ - imageSizeHeightMax);
            right = std::max(right + imageSizeWidthMax + imageSizeWidthMax,
                position.x_ + emitSize.x_ + imageSizeWidthMax + imageSizeWidthMax);
            bottom = std::max(bottom + imageSizeHeightMax + imageSizeHeightMax,
                position.y_ + emitSize.y_ + imageSizeHeightMax + imageSizeHeightMax);
        }
        std::shared_ptr<RectF> overlayRect = std::make_shared<RectF>(left, top, right, bottom);
        SetDrawRegion(overlayRect);
    }
}

// Update Particle Emitter Position and Size
void RSNode::SetEmitterUpdater(const std::shared_ptr<EmitterUpdater>& para)
{
    SetProperty<RSEmitterUpdaterModifier, RSProperty<std::shared_ptr<EmitterUpdater>>>(
        RSModifierType::PARTICLE_EMITTER_UPDATER, para);
}

// foreground
void RSNode::SetForegroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SetProperty<RSForegroundColorModifier, RSAnimatableProperty<Color>>(RSModifierType::FOREGROUND_COLOR, color);
}

void RSNode::SetBackgroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SetProperty<RSBackgroundColorModifier, RSAnimatableProperty<Color>>(RSModifierType::BACKGROUND_COLOR, color);
}

void RSNode::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
    SetProperty<RSBackgroundShaderModifier, RSProperty<std::shared_ptr<RSShader>>>(
        RSModifierType::BACKGROUND_SHADER, shader);
}

// background
void RSNode::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    image->SetNodeId(GetId());
    SetProperty<RSBgImageModifier, RSProperty<std::shared_ptr<RSImage>>>(RSModifierType::BG_IMAGE, image);
}

void RSNode::SetBgImageInnerRect(const Vector4f& rect)
{
    SetProperty<RSBgImageInnerRectModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::BG_IMAGE_INNER_RECT, rect);
}

void RSNode::SetBgImageSize(float width, float height)
{
    SetBgImageWidth(width);
    SetBgImageHeight(height);
}

void RSNode::SetBgImageWidth(float width)
{
    SetProperty<RSBgImageWidthModifier, RSAnimatableProperty<float>>(RSModifierType::BG_IMAGE_WIDTH, width);
}

void RSNode::SetBgImageHeight(float height)
{
    SetProperty<RSBgImageHeightModifier, RSAnimatableProperty<float>>(RSModifierType::BG_IMAGE_HEIGHT, height);
}

void RSNode::SetBgImagePosition(float positionX, float positionY)
{
    SetBgImagePositionX(positionX);
    SetBgImagePositionY(positionY);
}

void RSNode::SetBgImagePositionX(float positionX)
{
    SetProperty<RSBgImagePositionXModifier, RSAnimatableProperty<float>>(
        RSModifierType::BG_IMAGE_POSITION_X, positionX);
}

void RSNode::SetBgImagePositionY(float positionY)
{
    SetProperty<RSBgImagePositionYModifier, RSAnimatableProperty<float>>(
        RSModifierType::BG_IMAGE_POSITION_Y, positionY);
}

// set inner border color
void RSNode::SetBorderColor(uint32_t colorValue)
{
    SetBorderColor(colorValue, colorValue, colorValue, colorValue);
}

// set inner border color
void RSNode::SetBorderColor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    Vector4<Color> color(Color::FromArgbInt(left), Color::FromArgbInt(top),
                         Color::FromArgbInt(right), Color::FromArgbInt(bottom));
    SetBorderColor(color);
}

// set inner border color
void RSNode::SetBorderColor(const Vector4<Color>& color)
{
    SetProperty<RSBorderColorModifier, RSAnimatableProperty<Vector4<Color>>>(RSModifierType::BORDER_COLOR, color);
}

// set inner border width
void RSNode::SetBorderWidth(float width)
{
    SetBorderWidth(width, width, width, width);
}

// set inner border width
void RSNode::SetBorderWidth(float left, float top, float right, float bottom)
{
    Vector4f width(left, top, right, bottom);
    SetBorderWidth(width);
}

// set inner border width
void RSNode::SetBorderWidth(const Vector4f& width)
{
    SetProperty<RSBorderWidthModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::BORDER_WIDTH, width);
}

// set inner border style
void RSNode::SetBorderStyle(uint32_t styleValue)
{
    SetBorderStyle(styleValue, styleValue, styleValue, styleValue);
}

// set inner border style
void RSNode::SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    Vector4<BorderStyle> style(static_cast<BorderStyle>(left), static_cast<BorderStyle>(top),
                               static_cast<BorderStyle>(right), static_cast<BorderStyle>(bottom));
    SetBorderStyle(style);
}

// set inner border style
void RSNode::SetBorderStyle(const Vector4<BorderStyle>& style)
{
    Vector4<uint32_t> styles(static_cast<uint32_t>(style.x_), static_cast<uint32_t>(style.y_),
                             static_cast<uint32_t>(style.z_), static_cast<uint32_t>(style.w_));
    SetProperty<RSBorderStyleModifier, RSProperty<Vector4<uint32_t>>>(RSModifierType::BORDER_STYLE, styles);
}

void RSNode::SetOuterBorderColor(const Vector4<Color>& color)
{
    SetOutlineColor(color);
}

void RSNode::SetOuterBorderWidth(const Vector4f& width)
{
    SetOutlineWidth(width);
}

void RSNode::SetOuterBorderStyle(const Vector4<BorderStyle>& style)
{
    SetOutlineStyle(style);
}

void RSNode::SetOuterBorderRadius(const Vector4f& radius)
{
    SetOutlineRadius(radius);
}

void RSNode::SetOutlineColor(const Vector4<Color>& color)
{
    SetProperty<RSOutlineColorModifier, RSAnimatableProperty<Vector4<Color>>>(
        RSModifierType::OUTLINE_COLOR, color);
}

void RSNode::SetOutlineWidth(const Vector4f& width)
{
    SetProperty<RSOutlineWidthModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::OUTLINE_WIDTH, width);
}

void RSNode::SetOutlineStyle(const Vector4<BorderStyle>& style)
{
    Vector4<uint32_t> styles(static_cast<uint32_t>(style.x_), static_cast<uint32_t>(style.y_),
                             static_cast<uint32_t>(style.z_), static_cast<uint32_t>(style.w_));
    SetProperty<RSOutlineStyleModifier, RSProperty<Vector4<uint32_t>>>(
        RSModifierType::OUTLINE_STYLE, styles);
}

void RSNode::SetOutlineRadius(const Vector4f& radius)
{
    SetProperty<RSOutlineRadiusModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::OUTLINE_RADIUS, radius);
}

void RSNode::SetForegroundEffectRadius(const float blurRadius)
{
    SetProperty<RSForegroundEffectRadiusModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_EFFECT_RADIUS, blurRadius);
}

void RSNode::SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter)
{
    SetProperty<RSBackgroundFilterModifier, RSAnimatableProperty<std::shared_ptr<RSFilter>>>(
        RSModifierType::BACKGROUND_FILTER, backgroundFilter);
}

void RSNode::SetFilter(const std::shared_ptr<RSFilter>& filter)
{
    SetProperty<RSFilterModifier, RSAnimatableProperty<std::shared_ptr<RSFilter>>>(RSModifierType::FILTER, filter);
}

void RSNode::SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para)
{
    SetProperty<RSLinearGradientBlurParaModifier, RSProperty<std::shared_ptr<RSLinearGradientBlurPara>>>
                                                                    (RSModifierType::LINEAR_GRADIENT_BLUR_PARA, para);
}

void RSNode::SetDynamicLightUpRate(const float rate)
{
    SetProperty<RSDynamicLightUpRateModifier, RSAnimatableProperty<float>>(RSModifierType::DYNAMIC_LIGHT_UP_RATE, rate);
}

void RSNode::SetDynamicLightUpDegree(const float lightUpDegree)
{
    SetProperty<RSDynamicLightUpDegreeModifier,
        RSAnimatableProperty<float>>(RSModifierType::DYNAMIC_LIGHT_UP_DEGREE, lightUpDegree);
}

void RSNode::SetDynamicDimDegree(const float dimDegree)
{
    SetProperty<RSDynamicDimDegreeModifier,
        RSAnimatableProperty<float>>(RSModifierType::DYNAMIC_DIM_DEGREE, dimDegree);
}

void RSNode::SetGreyCoef(const Vector2f greyCoef)
{
    SetProperty<RSGreyCoefModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::GREY_COEF, greyCoef);
}

void RSNode::SetCompositingFilter(const std::shared_ptr<RSFilter>& compositingFilter) {}

void RSNode::SetShadowColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SetProperty<RSShadowColorModifier, RSAnimatableProperty<Color>>(RSModifierType::SHADOW_COLOR, color);
}

void RSNode::SetShadowOffset(float offsetX, float offsetY)
{
    SetShadowOffsetX(offsetX);
    SetShadowOffsetY(offsetY);
}

void RSNode::SetShadowOffsetX(float offsetX)
{
    SetProperty<RSShadowOffsetXModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_OFFSET_X, offsetX);
}

void RSNode::SetShadowOffsetY(float offsetY)
{
    SetProperty<RSShadowOffsetYModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_OFFSET_Y, offsetY);
}

void RSNode::SetShadowAlpha(float alpha)
{
    SetProperty<RSShadowAlphaModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_ALPHA, alpha);
}

void RSNode::SetShadowElevation(float elevation)
{
    SetProperty<RSShadowRadiusModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_RADIUS, 0);
    SetProperty<RSShadowElevationModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_ELEVATION, elevation);
}

void RSNode::SetShadowRadius(float radius)
{
    SetProperty<RSShadowElevationModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_ELEVATION, 0);
    SetProperty<RSShadowRadiusModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_RADIUS, radius);
}

void RSNode::SetShadowPath(const std::shared_ptr<RSPath>& shadowPath)
{
    SetProperty<RSShadowPathModifier, RSProperty<std::shared_ptr<RSPath>>>(RSModifierType::SHADOW_PATH, shadowPath);
}

void RSNode::SetShadowMask(bool shadowMask)
{
    SetProperty<RSShadowMaskModifier, RSProperty<bool>>(RSModifierType::SHADOW_MASK, shadowMask);
}

void RSNode::SetShadowIsFilled(bool shadowIsFilled)
{
    SetProperty<RSShadowIsFilledModifier, RSProperty<bool>>(RSModifierType::SHADOW_IS_FILLED, shadowIsFilled);
}

void RSNode::SetShadowColorStrategy(int shadowColorStrategy)
{
    SetProperty<RSShadowColorStrategyModifier, RSProperty<int>>(
        RSModifierType::SHADOW_COLOR_STRATEGY, shadowColorStrategy);
}

void RSNode::SetFrameGravity(Gravity gravity)
{
    ROSEN_LOGD("RSNode::SetFrameGravity, gravity = %{public}d", gravity);
    SetProperty<RSFrameGravityModifier, RSProperty<Gravity>>(RSModifierType::FRAME_GRAVITY, gravity);
}

void RSNode::SetClipRRect(const Vector4f& clipRect, const Vector4f& clipRadius)
{
    SetProperty<RSClipRRectModifier, RSAnimatableProperty<RRect>>(
        RSModifierType::CLIP_RRECT, RRect(clipRect, clipRadius));
}

void RSNode::SetClipBounds(const std::shared_ptr<RSPath>& path)
{
    SetProperty<RSClipBoundsModifier, RSProperty<std::shared_ptr<RSPath>>>(RSModifierType::CLIP_BOUNDS, path);
}

void RSNode::SetClipToBounds(bool clipToBounds)
{
    SetProperty<RSClipToBoundsModifier, RSProperty<bool>>(RSModifierType::CLIP_TO_BOUNDS, clipToBounds);
}

void RSNode::SetClipToFrame(bool clipToFrame)
{
    SetProperty<RSClipToFrameModifier, RSProperty<bool>>(RSModifierType::CLIP_TO_FRAME, clipToFrame);
}

void RSNode::SetVisible(bool visible)
{
    // kick off transition only if it's on tree(has valid parent) and visibility is changed.
    if (transitionEffect_ != nullptr && GetParent() != nullptr && visible != GetStagingProperties().GetVisible()) {
        NotifyTransition(transitionEffect_, visible);
    }

    SetProperty<RSVisibleModifier, RSProperty<bool>>(RSModifierType::VISIBLE, visible);
}

void RSNode::SetMask(const std::shared_ptr<RSMask>& mask)
{
    SetProperty<RSMaskModifier, RSProperty<std::shared_ptr<RSMask>>>(RSModifierType::MASK, mask);
}

void RSNode::SetUseEffect(bool useEffect)
{
    SetProperty<RSUseEffectModifier, RSProperty<bool>>(RSModifierType::USE_EFFECT, useEffect);
}

void RSNode::SetUseShadowBatching(bool useShadowBatching)
{
    SetProperty<RSUseShadowBatchingModifier, RSProperty<bool>>(RSModifierType::USE_SHADOW_BATCHING, useShadowBatching);
}

void RSNode::SetColorBlendMode(RSColorBlendMode colorBlendMode)
{
    SetProperty<RSColorBlendModeModifier, RSProperty<int>>(
        RSModifierType::COLOR_BLEND_MODE, static_cast<int>(colorBlendMode));
}

void RSNode::SetColorBlendApplyType(RSColorBlendApplyType colorBlendApplyType)
{
    SetProperty<RSColorBlendApplyTypeModifier, RSProperty<int>>(
        RSModifierType::COLOR_BLEND_APPLY_TYPE, static_cast<int>(colorBlendApplyType));
}

void RSNode::SetPixelStretch(const Vector4f& stretchSize)
{
    SetProperty<RSPixelStretchModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::PIXEL_STRETCH, stretchSize);
}

void RSNode::SetPixelStretchPercent(const Vector4f& stretchPercent)
{
    SetProperty<RSPixelStretchPercentModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::PIXEL_STRETCH_PERCENT,
        stretchPercent);
}

void RSNode::SetFreeze(bool isFreeze)
{
    ROSEN_LOGE("SetFreeze only support RSSurfaceNode and RSCanvasNode in uniRender");
}

void RSNode::SetNodeName(const std::string& nodeName)
{
    if (nodeName_ != nodeName) {
        nodeName_ = nodeName;
        std::unique_ptr<RSCommand> command = std::make_unique<RSSetNodeName>(GetId(), nodeName_);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode());
        }
    }
}

void RSNode::SetTakeSurfaceForUIFlag()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetTakeSurfaceForUIFlag>(GetId());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::SetSpherizeDegree(float spherizeDegree)
{
    SetProperty<RSSpherizeModifier, RSAnimatableProperty<float>>(RSModifierType::SPHERIZE, spherizeDegree);
}

void RSNode::SetLightUpEffectDegree(float LightUpEffectDegree)
{
    SetProperty<RSLightUpEffectModifier, RSAnimatableProperty<float>>(
        RSModifierType::LIGHT_UP_EFFECT, LightUpEffectDegree);
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

    auto& customEffects = isTransitionIn ? effect->customTransitionInEffects_ : effect->customTransitionOutEffects_;
    // temporary close the implicit animation
    ExecuteWithoutAnimation(
        [&customEffects] {
            for (auto& customEffect : customEffects) {
                customEffect->Active();
            }
        },
        implicitAnimator_);

    implicitAnimator_->BeginImplicitTransition(effect, isTransitionIn);
    for (auto& customEffect : customEffects) {
        customEffect->Identity();
    }
    implicitAnimator_->CreateImplicitTransition(*this);
    implicitAnimator_->EndImplicitTransition();
}

void RSNode::OnAddChildren()
{
    // kick off transition only if it's visible.
    if (transitionEffect_ != nullptr && GetStagingProperties().GetVisible()) {
        NotifyTransition(transitionEffect_, true);
    }
}

void RSNode::OnRemoveChildren()
{
    // kick off transition only if it's visible.
    if (transitionEffect_ != nullptr && GetStagingProperties().GetVisible()) {
        NotifyTransition(transitionEffect_, false);
    }
}

bool RSNode::AnimationCallback(AnimationId animationId, AnimationCallbackEvent event)
{
    std::shared_ptr<RSAnimation> animation = nullptr;
    {
        std::unique_lock<std::mutex> lock(animationMutex_);
        auto animationItr = animations_.find(animationId);
        if (animationItr == animations_.end()) {
            ROSEN_LOGE("Failed to find animation[%{public}" PRIu64 "]!", animationId);
            return false;
        }
        animation = animationItr->second;
    }

    if (animation == nullptr) {
        ROSEN_LOGE("Failed to callback animation[%{public}" PRIu64 "], animation is null!", animationId);
        return false;
    }
    if (event == FINISHED) {
        RemoveAnimationInner(animation);
        animation->CallFinishCallback();
        return true;
    } else if (event == REPEAT_FINISHED) {
        animation->CallRepeatCallback();
        return true;
    } else if (event == LOGICALLY_FINISHED) {
        animation->CallLogicallyFinishCallback();
        return true;
    }
    ROSEN_LOGE("Failed to callback animation event[%{public}d], event is null!", event);
    return false;
}

void RSNode::SetPaintOrder(bool drawContentLast)
{
    drawContentLast_ = drawContentLast;
}

void RSNode::MarkDrivenRender(bool flag)
{
    if (drivenFlag_ != flag) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSMarkDrivenRender>(GetId(), flag);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode());
        }
        drivenFlag_ = flag;
    }
}

void RSNode::MarkDrivenRenderItemIndex(int index)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkDrivenRenderItemIndex>(GetId(), index);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::MarkDrivenRenderFramePaintState(bool flag)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSMarkDrivenRenderFramePaintState>(GetId(), flag);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::MarkContentChanged(bool isChanged)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkContentChanged>(GetId(), isChanged);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::ClearAllModifiers()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);    
    for (auto [id, modifier] : modifiers_) {
        if (modifier) {
            modifier->DetachFromNode();
        }
    }
    modifiers_.clear();
    propertyModifiers_.clear();
    for (int i = 0; i < (uint16_t)RSModifierType::MAX_RS_MODIFIER_TYPE; ++i) {
        modifiersTypeMap_[i] = nullptr;
    }
}

void RSNode::AddModifier(const std::shared_ptr<RSModifier> modifier)
{
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        if (!modifier || modifiers_.count(modifier->GetPropertyId())) {
            return;
        }
        if (motionPathOption_ != nullptr && IsPathAnimatableModifier(modifier->GetModifierType())) {
            modifier->SetMotionPathOption(motionPathOption_);
        }
        auto rsnode = std::static_pointer_cast<RSNode>(shared_from_this());
        modifier->AttachToNode(rsnode);
        modifiers_.emplace(modifier->GetPropertyId(), modifier);
        modifiersTypeMap_[(int16_t)modifier->GetModifierType()] = modifier;
    }
    if (modifier->GetModifierType() == RSModifierType::NODE_MODIFIER) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        if (NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> cmdForRemote =
                std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
            transactionProxy->AddCommand(cmdForRemote, true, GetFollowType(), GetId());
        }
    }
}

void RSNode::DoFlushModifier()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    if (modifiers_.empty()) {
        return;
    }
    for (const auto& [_, modifier] : modifiers_) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        }
    }
}

void RSNode::RemoveModifier(const std::shared_ptr<RSModifier> modifier)
{
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        if (!modifier) {
            return;
        }
        auto iter = modifiers_.find(modifier->GetPropertyId());
        if (iter == modifiers_.end()) {
            return;
        }
        auto deleteType = modifier->GetModifierType();
        bool isExist = false;
        for (auto [id, value] : modifiers_) {
            if (value && value->GetModifierType() == deleteType) {
                modifiersTypeMap_[(int16_t)deleteType] = value;
                isExist = true;
                break;
            }
        }
        modifiers_.erase(iter);
        if (isExist) {
            modifiersTypeMap_[(int16_t)deleteType] = nullptr;
        }
    }
    modifier->DetachFromNode();
    std::unique_ptr<RSCommand> command = std::make_unique<RSRemoveModifier>(GetId(), modifier->GetPropertyId());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        if (NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> cmdForRemote =
                std::make_unique<RSRemoveModifier>(GetId(), modifier->GetPropertyId());
            transactionProxy->AddCommand(cmdForRemote, true, GetFollowType(), GetId());
        }
    }
}

const std::shared_ptr<RSModifier> RSNode::GetModifier(const PropertyId& propertyId)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    auto iter = modifiers_.find(propertyId);
    if (iter != modifiers_.end()) {
        return iter->second;
    }

    return {};
}

void RSNode::UpdateModifierMotionPathOption()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
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

void RSNode::UpdateImplicitAnimator()
{
    auto tid = gettid();
    if (tid == implicitAnimatorTid_) {
        return;
    }
    implicitAnimatorTid_ = tid;
    implicitAnimator_ = RSImplicitAnimatorMap::Instance().GetAnimator(tid);
}

std::vector<PropertyId> RSNode::GetModifierIds() const
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    std::vector<PropertyId> ids;
    for (const auto& [id, _] : modifiers_) {
        ids.push_back(id);
    }
    return ids;
}

void RSNode::MarkAllExtendModifierDirty()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    if (extendModifierIsDirty_) {
        return;
    }

    extendModifierIsDirty_ = true;
    for (auto& [id, modifier] : modifiers_) {
        if (modifier->GetModifierType() < RSModifierType::CUSTOM) {
            continue;
        }
        modifier->SetDirty(true);
    }
}

void RSNode::ResetExtendModifierDirty()
{
    extendModifierIsDirty_ = false;
}

void RSNode::SetIsCustomTextType(bool isCustomTextType)
{
    isCustomTextType_ = isCustomTextType;
}

bool RSNode::GetIsCustomTextType()
{
    return isCustomTextType_;
}

void RSNode::SetDrawRegion(std::shared_ptr<RectF> rect)
{
    if (drawRegion_ != rect) {
        drawRegion_ = rect;
        std::unique_ptr<RSCommand> command = std::make_unique<RSSetDrawRegion>(GetId(), rect);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        }
    }
}

void RSNode::RegisterTransitionPair(NodeId inNodeId, NodeId outNodeId)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSRegisterGeometryTransitionNodePair>(inNodeId, outNodeId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSNode::UnregisterTransitionPair(NodeId inNodeId, NodeId outNodeId)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSUnregisterGeometryTransitionNodePair>(inNodeId, outNodeId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSNode::MarkNodeGroup(bool isNodeGroup, bool isForced, bool includeProperty)
{
    if (isNodeGroup_ == isNodeGroup) {
        return;
    }
    isNodeGroup_ = isNodeGroup;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkNodeGroup>(GetId(), isNodeGroup, isForced,
        includeProperty);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer)
{
    if (isNodeSingleFrameComposer_ != isNodeSingleFrameComposer) {
        isNodeSingleFrameComposer_ = isNodeSingleFrameComposer;
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSMarkNodeSingleFrameComposer>(GetId(), isNodeSingleFrameComposer, GetRealPid());
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode());
        }
    }
}

void RSNode::SetGrayScale(float grayScale)
{
    SetProperty<RSGrayScaleModifier, RSAnimatableProperty<float>>(RSModifierType::GRAY_SCALE, grayScale);
}

void RSNode::SetLightIntensity(float lightIntensity)
{
    SetProperty<RSLightIntensityModifier, RSAnimatableProperty<float>>(RSModifierType::LIGHT_INTENSITY, lightIntensity);
}

void RSNode::SetLightPosition(float positionX, float positionY, float positionZ)
{
    SetLightPosition(Vector4f(positionX, positionY, positionZ, 0.f));
}

void RSNode::SetLightPosition(const Vector4f& lightPosition)
{
    SetProperty<RSLightPositionModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::LIGHT_POSITION, lightPosition);
}

void RSNode::SetIlluminatedBorderWidth(float illuminatedBorderWidth)
{
    SetProperty<RSIlluminatedBorderWidthModifier, RSAnimatableProperty<float>>(
        RSModifierType::ILLUMINATED_BORDER_WIDTH, illuminatedBorderWidth);
}

void RSNode::SetIlluminatedType(uint32_t illuminatedType)
{
    SetProperty<RSIlluminatedTypeModifier, RSProperty<int>>(
        RSModifierType::ILLUMINATED_TYPE, illuminatedType);
}

void RSNode::SetBloom(float bloomIntensity)
{
    SetProperty<RSBloomModifier, RSAnimatableProperty<float>>(RSModifierType::BLOOM, bloomIntensity);
}

void RSNode::SetBrightness(float brightness)
{
    SetProperty<RSBrightnessModifier, RSAnimatableProperty<float>>(RSModifierType::BRIGHTNESS, brightness);
}

void RSNode::SetContrast(float contrast)
{
    SetProperty<RSContrastModifier, RSAnimatableProperty<float>>(RSModifierType::CONTRAST, contrast);
}

void RSNode::SetSaturate(float saturate)
{
    SetProperty<RSSaturateModifier, RSAnimatableProperty<float>>(RSModifierType::SATURATE, saturate);
}

void RSNode::SetSepia(float sepia)
{
    SetProperty<RSSepiaModifier, RSAnimatableProperty<float>>(RSModifierType::SEPIA, sepia);
}

void RSNode::SetInvert(float invert)
{
    SetProperty<RSInvertModifier, RSAnimatableProperty<float>>(RSModifierType::INVERT, invert);
}

void RSNode::SetAiInvert(const Vector4f& aiInvert)
{
    SetProperty<RSAiInvertModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::AIINVERT, aiInvert);
}

void RSNode::SetSystemBarEffect()
{
    SetProperty<RSSystemBarEffectModifier, RSProperty<bool>>(RSModifierType::SYSTEMBAREFFECT, true);
}

void RSNode::SetHueRotate(float hueRotate)
{
    SetProperty<RSHueRotateModifier, RSAnimatableProperty<float>>(RSModifierType::HUE_ROTATE, hueRotate);
}

void RSNode::SetColorBlend(uint32_t colorValue)
{
    auto colorBlend = Color::FromArgbInt(colorValue);
    SetProperty<RSColorBlendModifier, RSAnimatableProperty<Color>>(RSModifierType::COLOR_BLEND, colorBlend);
}

int32_t RSNode::CalcExpectedFrameRate(const std::string& scene, float speed)
{
    auto preferredFps = RSFrameRatePolicy::GetInstance()->GetPreferredFps(scene, speed);
    return preferredFps;
}

void RSNode::SetOutOfParent(OutOfParentType outOfParent)
{
    if (outOfParent != outOfParent_) {
        outOfParent_ = outOfParent;

        std::unique_ptr<RSCommand> command = std::make_unique<RSSetOutOfParent>(GetId(), outOfParent);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode());
        }
    }
}

NodeId RSNode::GenerateId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 1; // surfaceNode is seted correctly during boot when currentId is 1

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Node Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((NodeId)pid_ << 32) | currentId;
}

void RSNode::InitUniRenderEnabled()
{
    static bool inited = false;
    if (!inited) {
        inited = true;
        g_isUniRenderEnabled = RSSystemProperties::GetUniRenderEnabled();
        ROSEN_LOGD("RSNode::InitUniRenderEnabled:%{public}d", g_isUniRenderEnabled);
    }
}


// RSNode::~RSNode()
// {

// }

bool RSNode::IsUniRenderEnabled() const
{
    return g_isUniRenderEnabled;
}

bool RSNode::IsRenderServiceNode() const
{
    return (g_isUniRenderEnabled || isRenderServiceNode_) && (!isTextureExportNode_);
}

void RSNode::AddChild(SharedPtr child, int index)
{
    if (child == nullptr) {
        ROSEN_LOGE("RSNode::AddChild, child is nullptr");
        return;
    }
    if (child->parent_ == id_) {
        ROSEN_LOGD("RSNode::AddChild, child already exist");
        return;
    }
    if (child->GetType() == RSUINodeType::DISPLAY_NODE) {
        // Disallow to add display node as child.
        return;
    }
    NodeId childId = child->GetId();
    if (child->parent_ != 0 && !child->isTextureExportNode_) {
        child->RemoveFromTree();
    }

    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }
    child->SetParent(id_);
    if (isTextureExportNode_) {
        child->SyncTextureExport(isTextureExportNode_);
    }
    child->OnAddChildren();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(id_, childId, index);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    if (GetType() == RSUINodeType::SURFACE_NODE) {
        ROSEN_LOGI("RSNode::AddChild, NodeId: %{public}" PRIu64 ", ChildId: %{public}" PRIu64, id_, childId);
        RS_TRACE_NAME_FMT("RSNode::AddChild, NodeId: %" PRIu64 ", ChildId: %" PRIu64, id_, childId);
    }
}

void RSNode::MoveChild(SharedPtr child, int index)
{
    if (child == nullptr || child->parent_ != id_) {
        ROSEN_LOGD("RSNode::MoveChild, not valid child");
        return;
    }
    NodeId childId = child->GetId();
    auto itr = std::find(children_.begin(), children_.end(), childId);
    if (itr == children_.end()) {
        ROSEN_LOGD("RSNode::MoveChild, not child");
        return;
    }
    children_.erase(itr);
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeMoveChild>(id_, childId, index);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
}

void RSNode::RemoveChild(SharedPtr child)
{
    if (child == nullptr || child->parent_ != id_) {
        ROSEN_LOGI("RSNode::RemoveChild, child is nullptr");
        return;
    }
    NodeId childId = child->GetId();
    RemoveChildById(childId);
    child->OnRemoveChildren();
    child->SetParent(0);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveChild>(id_, childId);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
}

void RSNode::RemoveChildByNodeId(NodeId childId)
{
    if (auto childPtr = RSNodeMap::Instance().GetNode(childId)) {
        RemoveChild(childPtr);
    } else {
        ROSEN_LOGE("RSNode::RemoveChildByNodeId, childId not found");
    }
}

void RSNode::AddCrossParentChild(SharedPtr child, int index)
{
    // AddCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // so this child will not remove from the old parent.
    if (child == nullptr) {
        ROSEN_LOGE("RSNode::AddCrossScreenChild, child is nullptr");
        return;
    }
    if (!this->IsInstanceOf<RSDisplayNode>()) {
        ROSEN_LOGE("RSNode::AddCrossScreenChild, only displayNode support AddCrossScreenChild");
        return;
    }
    NodeId childId = child->GetId();

    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }
    child->SetParent(id_);
    child->OnAddChildren();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddCrossParentChild>(id_, childId, index);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
}

void RSNode::RemoveCrossParentChild(SharedPtr child, NodeId newParentId)
{
    // RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // set the newParentId to rebuild the parent-child relationship.
    if (child == nullptr) {
        ROSEN_LOGI("RSNode::RemoveCrossScreenChild, child is nullptr");
        return;
    }
    if (!this->IsInstanceOf<RSDisplayNode>()) {
        ROSEN_LOGE("RSNode::RemoveCrossScreenChild, only displayNode support RemoveCrossScreenChild");
        return;
    }
    NodeId childId = child->GetId();
    RemoveChildById(childId);
    child->OnRemoveChildren();
    child->SetParent(newParentId);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveCrossParentChild>(id_, childId, newParentId);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
}

void RSNode::RemoveChildById(NodeId childId)
{
    auto itr = std::find(children_.begin(), children_.end(), childId);
    if (itr != children_.end()) {
        children_.erase(itr);
    }
}

void RSNode::RemoveFromTree()
{
    if (auto parentPtr = RSNodeMap::Instance().GetNode(parent_)) {
        parentPtr->RemoveChildById(GetId());
        OnRemoveChildren();
        SetParent(0);
    }
    // always send Remove-From-Tree command
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using own GetHierarchyCommandNodeId(), not GetId()
    auto nodeId = GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveFromTree>(nodeId);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), nodeId);
}

void RSNode::ClearChildren()
{
    for (auto child : children_) {
        if (auto childPtr = RSNodeMap::Instance().GetNode(child)) {
            childPtr->SetParent(0);
        }
    }
    children_.clear();

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    // construct command using own GetHierarchyCommandNodeId(), not GetId()
    auto nodeId = GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(nodeId);
    transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), nodeId);
}

void RSNode::SetTextureExport(bool isTextureExportNode)
{
    if (isTextureExportNode == isTextureExportNode_) {
        return;
    }
    isTextureExportNode_ = isTextureExportNode;
    if (!isTextureExportNode_) {
        DoFlushModifier();
        return;
    }
    CreateTextureExportRenderNodeInRT();
    DoFlushModifier();
}

void RSNode::SyncTextureExport(bool isTextureExportNode)
{
    if (isTextureExportNode == isTextureExportNode_) {
        return;
    }
    SetTextureExport(isTextureExportNode);
    for (uint32_t index = 0; index < children_.size(); index++) {
        if (auto childPtr = RSNodeMap::Instance().GetNode(children_[index])) {
            childPtr->SyncTextureExport(isTextureExportNode);
            if (auto transactionProxy = RSTransactionProxy::GetInstance()) {
                std::unique_ptr<RSCommand> command =
                    std::make_unique<RSBaseNodeAddChild>(id_, childPtr->GetHierarchyCommandNodeId(), index);
                transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
            }
        }
    }
}

const std::optional<NodeId> RSNode::GetChildIdByIndex(int index) const
{
    int childrenTotal = static_cast<int>(children_.size());
    if (childrenTotal <= 0 || index < -1 || index >= childrenTotal) {
        return std::nullopt;
    }
    if (index == -1) {
        index = childrenTotal - 1;
    }
    return children_.at(index);
}

void RSNode::SetParent(NodeId parentId)
{
    parent_ = parentId;
}

RSNode::SharedPtr RSNode::GetParent()
{
    return RSNodeMap::Instance().GetNode(parent_);
}

std::string RSNode::DumpNode(int depth) const
{
    std::stringstream ss;
    auto it = RSUINodeTypeStrs.find(GetType());
    if (it == RSUINodeTypeStrs.end()) {
        return "";
    }
    ss << it->second << "[" << std::to_string(id_) << "] child[";
    for (auto child : children_) {
        ss << std::to_string(child) << " ";
    }
    ss << "]";

    if (!animations_.empty()) {
        ss << " animation:" << std::to_string(animations_.size());
    }
    ss << " " << GetStagingProperties().Dump();
    return ss.str();
}

bool RSNode::IsInstanceOf(RSUINodeType type) const
{
    auto targetType = static_cast<uint32_t>(type);
    auto instanceType = static_cast<uint32_t>(GetType());
    // use bitmask to check whether the instance is a subclass of the target type
    return (instanceType & targetType) == targetType;
}

template<typename T>
bool RSNode::IsInstanceOf() const
{
    return IsInstanceOf(T::Type);
}

// explicit instantiation with all render node types
template bool RSNode::IsInstanceOf<RSDisplayNode>() const;
template bool RSNode::IsInstanceOf<RSSurfaceNode>() const;
template bool RSNode::IsInstanceOf<RSProxyNode>() const;
template bool RSNode::IsInstanceOf<RSCanvasNode>() const;
template bool RSNode::IsInstanceOf<RSRootNode>() const;
template bool RSNode::IsInstanceOf<RSCanvasDrawingNode>() const;

void RSNode::SetInstanceId(int32_t instanceId)
{
    instanceId_ = instanceId;
    RSNodeMap::MutableInstance().RegisterNodeInstanceId(id_, instanceId_);
}

} // namespace Rosen
} // namespace OHOS
