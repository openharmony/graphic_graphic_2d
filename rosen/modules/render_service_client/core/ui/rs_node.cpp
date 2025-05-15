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
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector4.h"
#include "feature/hyper_graphic_manager/rs_frame_rate_policy.h"
#include "modifier/rs_modifier.h"
#include "modifier/rs_modifier_manager_map.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "render/rs_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_blur_filter.h"
#include "render/rs_path.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_canvas_drawing_node.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_effect_node.h"
#include "ui/rs_proxy_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"

#ifdef RS_ENABLE_VK
#include "modifier_render_thread/rs_modifiers_draw.h"
#endif

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

RSNode::RSNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext,
    bool isOnTheTree)
    : isRenderServiceNode_(isRenderServiceNode), isTextureExportNode_(isTextureExportNode), id_(id),
      rsUIContext_(rsUIContext), stagingPropertiesExtractor_(id, rsUIContext),
      showingPropertiesFreezer_(id, rsUIContext), isOnTheTree_(isOnTheTree)
{
    InitUniRenderEnabled();

    if (rsUIContext_.lock() != nullptr) {
        auto transaction = rsUIContext_.lock()->GetRSTransaction();
        if (transaction != nullptr && g_isUniRenderEnabled && isTextureExportNode) {
            std::call_once(flag_, [transaction]() {
                auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
                transaction->SetRenderThreadClient(renderThreadClient);
            });
        }
        hasCreateRenderNodeInRT_ = isTextureExportNode;
        hasCreateRenderNodeInRS_ = !hasCreateRenderNodeInRT_;
        return;
    }
    if (g_isUniRenderEnabled && isTextureExportNode) {
        std::call_once(flag_, []() {
            auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
            auto transactionProxy = RSTransactionProxy::GetInstance();
            if (transactionProxy != nullptr) {
                transactionProxy->SetRenderThreadClient(renderThreadClient);
            }
        });
    }
    hasCreateRenderNodeInRT_ = isTextureExportNode;
    hasCreateRenderNodeInRS_ = !hasCreateRenderNodeInRT_;
}

RSNode::RSNode(bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext,
    bool isOnTheTree)
    : RSNode(isRenderServiceNode, GenerateId(), isTextureExportNode, rsUIContext, isOnTheTree) {}

RSNode::~RSNode()
{
    if (!FallbackAnimationsToContext()) {
        FallbackAnimationsToRoot();
    }
    ClearAllModifiers();
#ifdef RS_ENABLE_VK
    RSModifiersDraw::EraseOffTreeNode(instanceId_, id_);
#endif

    // break current (ui) parent-child relationship.
    // render nodes will check if its child is expired and remove it, no need to manually remove it here.
    SharedPtr parentPtr;
    auto rsUIContext = rsUIContext_.lock();
    if (rsUIContext != nullptr) {
        parentPtr = rsUIContext->GetNodeMap().GetNode(parent_);
        // tell RT/RS to destroy related render node
        rsUIContext->GetMutableNodeMap().UnregisterNode(id_);
        auto transaction = rsUIContext->GetRSTransaction();
        if (transaction == nullptr || skipDestroyCommandInDestructor_) {
            return;
        }
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(id_);
        transaction->AddCommand(command, IsRenderServiceNode());
        if ((IsRenderServiceNode() && hasCreateRenderNodeInRT_) ||
            (!IsRenderServiceNode() && hasCreateRenderNodeInRS_)) {
            command = std::make_unique<RSBaseNodeDestroy>(id_);
            transaction->AddCommand(command, !IsRenderServiceNode());
        }
    } else {
        parentPtr = RSNodeMap::Instance().GetNode(parent_);
        RSNodeMap::MutableInstance().UnregisterNode(id_);
        // tell RT/RS to destroy related render node
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr || skipDestroyCommandInDestructor_) {
            return;
        }
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(id_);
        transactionProxy->AddCommand(command, IsRenderServiceNode());
        if ((IsRenderServiceNode() && hasCreateRenderNodeInRT_) ||
            (!IsRenderServiceNode() && hasCreateRenderNodeInRS_)) {
            command = std::make_unique<RSBaseNodeDestroy>(id_);
            transactionProxy->AddCommand(command, !IsRenderServiceNode());
        }
    }
    if (parentPtr) {
        parentPtr->RemoveChildById(id_);
    }
}

std::shared_ptr<RSTransactionHandler> RSNode::GetRSTransaction() const
{
    auto rsUIContext = rsUIContext_.lock();
    if (!rsUIContext) {
        return nullptr;
    }
    return rsUIContext->GetRSTransaction();
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

void RSNode::OpenImplicitAnimation(const std::shared_ptr<RSUIContext> rsUIContext,
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
    const std::function<void()>& finishCallback)
{
    auto implicitAnimator =
        rsUIContext ? rsUIContext->GetRSImplicitAnimator() : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
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

std::vector<std::shared_ptr<RSAnimation>> RSNode::CloseImplicitAnimation(const std::shared_ptr<RSUIContext> rsUIContext)
{
    auto implicitAnimator =
        rsUIContext ? rsUIContext->GetRSImplicitAnimator() : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("multi-instance Failed to close implicit animation, implicit animator is null!");
        return {};
    }

    return implicitAnimator->CloseImplicitAnimation();
}

bool RSNode::CloseImplicitCancelAnimation()
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to close implicit animation for cancel, implicit animator is null!");
        return false;
    }

    return implicitAnimator->CloseImplicitCancelAnimation();
}

bool RSNode::CloseImplicitCancelAnimation(const std::shared_ptr<RSUIContext> rsUIContext)
{
    auto implicitAnimator =
        rsUIContext ? rsUIContext->GetRSImplicitAnimator() : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("multi-instance Failed to close implicit animation for cancel, implicit animator is null!");
        return false;
    }

    return implicitAnimator->CloseImplicitCancelAnimation();
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

void RSNode::AddKeyFrame(const std::shared_ptr<RSUIContext> rsUIContext,
    float fraction, const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback)
{
    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator()
                                        : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
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

void RSNode::AddKeyFrame(const std::shared_ptr<RSUIContext> rsUIContext,
    float fraction, const PropertyCallback& propertyCallback)
{
    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator()
                                        : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
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

void RSNode::AddDurationKeyFrame(const std::shared_ptr<RSUIContext> rsUIContext,
    int duration, const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback)
{
    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator()
                                        : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
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

bool RSNode::IsImplicitAnimationOpen(const std::shared_ptr<RSUIContext> rsUIContext)
{
    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator()
                                        : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
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

std::vector<std::shared_ptr<RSAnimation>> RSNode::Animate(const std::shared_ptr<RSUIContext> rsUIContext,
    const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback,
    const std::function<void()>& finishCallback, const std::function<void()>& repeatCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }

    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator()
                                        : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
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
        propertyCallback();
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

std::vector<std::shared_ptr<RSAnimation>> RSNode::AnimateWithCurrentOptions(
    const std::shared_ptr<RSUIContext> rsUIContext, const PropertyCallback& propertyCallback,
    const std::function<void()>& finishCallback, bool timingSensitive)
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

    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator()
                                        : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        propertyCallback();
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

std::vector<std::shared_ptr<RSAnimation>> RSNode::AnimateWithCurrentCallback(
    const std::shared_ptr<RSUIContext> rsUIContext,
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
    const PropertyCallback& propertyCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }

    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator()
                                        : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
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
    const PropertyCallback& callback, const std::shared_ptr<RSUIContext> rsUIContext,
    std::shared_ptr<RSImplicitAnimator> implicitAnimator)
{
    if (callback == nullptr) {
        ROSEN_LOGE("Failed to execute without animation, property callback is null!");
        return;
    }
    if (implicitAnimator == nullptr) {
        implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator()
                                       : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    }
    if (implicitAnimator == nullptr) {
        callback();
    } else {
        implicitAnimator->ExecuteWithoutAnimation(callback);
    }
}

bool RSNode::FallbackAnimationsToContext()
{
    auto rsUIContext = rsUIContext_.lock();
    if (rsUIContext == nullptr) {
        return false;
    }
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
    for (auto& [animationId, animation] : animations_) {
        if (animation && animation->GetRepeatCount() == -1) {
            continue;
        }
        rsUIContext->AddAnimationInner(std::move(animation));
    }
    animations_.clear();
    return true;
}

void RSNode::FallbackAnimationsToRoot()
{
    auto target = RSNodeMap::Instance().GetAnimationFallbackNode(); // delete
    if (target == nullptr) {
        ROSEN_LOGE("Failed to move animation to root, root node is null!");
        return;
    }
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
    for (auto& [animationId, animation] : animations_) {
        if (animation && animation->GetRepeatCount() == -1) {
            continue;
        }
        RSNodeMap::MutableInstance().RegisterAnimationInstanceId(animationId, id_, instanceId_); // delete
        target->AddAnimationInner(std::move(animation));
    }
    animations_.clear();
}

void RSNode::AddAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
    animations_.emplace(animation->GetId(), animation);
    animatingPropertyNum_[animation->GetPropertyId()]++;
    SetDrawNode();
}

void RSNode::RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
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
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
    for (const auto& [animationId, animation] : animations_) {
        if (animation->GetPropertyId() == id) {
            animation->Finish();
        }
    }
}

void RSNode::CancelAnimationByProperty(const PropertyId& id, const bool needForceSync)
{
    std::vector<std::shared_ptr<RSAnimation>> toBeRemoved;
    {
        std::unique_lock<std::recursive_mutex> lock(animationMutex_);
        animatingPropertyNum_.erase(id);
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
        std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCancel>(id_, id);
        AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
        if (NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote = std::make_unique<RSAnimationCancel>(id_, id);
            AddCommand(commandForRemote, true, GetFollowType(), id_);
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

void RSNode::AddAnimation(const std::shared_ptr<RSAnimation>& animation, bool isStartAnimation)
{
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to add animation, animation is null!");
        return;
    }

    auto animationId = animation->GetId();
    {
        std::unique_lock<std::recursive_mutex> lock(animationMutex_);
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

    AddAnimationInner(animation);

    animation->StartInner(shared_from_this());
    if (!isStartAnimation) {
        animation->Pause();
    }
}

void RSNode::RemoveAllAnimations()
{
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
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

    {
        std::unique_lock<std::recursive_mutex> lock(animationMutex_);
        if (animations_.find(animation->GetId()) == animations_.end()) {
            ROSEN_LOGE("Failed to remove animation, animation not exists!");
            return;
        }
    }
    animation->Finish();
}

void RSNode::SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption)
{
    motionPathOption_ = motionPathOption;
    UpdateModifierMotionPathOption();
}

void RSNode::SetMagnifierParams(const std::shared_ptr<RSMagnifierParams>& para)
{
    SetProperty<RSMagnifierParamsModifier, RSProperty<std::shared_ptr<RSMagnifierParams>>>(
        RSModifierType::MAGNIFIER_PARA, para);
}

const std::shared_ptr<RSMotionPathOption> RSNode::GetMotionPathOption() const
{
    return motionPathOption_;
}

bool RSNode::HasPropertyAnimation(const PropertyId& id)
{
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
    auto it = animatingPropertyNum_.find(id);
    return it != animatingPropertyNum_.end() && it->second > 0;
}

std::vector<AnimationId> RSNode::GetAnimationByPropertyId(const PropertyId& id)
{
    std::unique_lock<std::recursive_mutex> lock(animationMutex_);
    std::vector<AnimationId> animations;
    for (auto& [animateId, animation] : animations_) {
        if (animation->GetPropertyId() == id) {
            animations.push_back(animateId);
        }
    }
    return animations;
}

bool RSNode::IsGeometryDirty() const
{
    return dirtyType_ & static_cast<uint32_t>(NodeDirtyType::GEOMETRY);
}

bool RSNode::IsAppearanceDirty() const
{
    return dirtyType_ & static_cast<uint32_t>(NodeDirtyType::APPEARANCE);
}

void RSNode::MarkDirty(NodeDirtyType type, bool isDirty)
{
    if (isDirty) {
        dirtyType_ |= static_cast<uint32_t>(type);
    } else {
        dirtyType_ &= ~static_cast<uint32_t>(type);
    }
}

float RSNode::GetGlobalPositionX() const
{
    return globalPositionX_;
}

float RSNode::GetGlobalPositionY() const
{
    return globalPositionY_;
}

std::shared_ptr<RSObjAbsGeometry> RSNode::GetLocalGeometry() const
{
    return localGeometry_;
}

std::shared_ptr<RSObjAbsGeometry> RSNode::GetGlobalGeometry() const
{
    return globalGeometry_;
}

void RSNode::UpdateLocalGeometry()
{
    if (!IsGeometryDirty()) {
        return;
    }
    localGeometry_ = std::make_shared<RSObjAbsGeometry>();
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    for (const auto& [_, modifier] : modifiers_) {
        if (modifier->GetPropertyModifierType() == RSPropertyModifierType::GEOMETRY) {
            modifier->Apply(localGeometry_);
        }
    }
}

void RSNode::UpdateGlobalGeometry(const std::shared_ptr<RSObjAbsGeometry>& parentGlobalGeometry)
{
    if (parentGlobalGeometry == nullptr || localGeometry_ == nullptr) {
        return;
    }
    if (globalGeometry_ == nullptr) {
        globalGeometry_ = std::make_shared<RSObjAbsGeometry>();
    }
    *globalGeometry_ = *localGeometry_;
    globalGeometry_->UpdateMatrix(&parentGlobalGeometry->GetAbsMatrix(), std::nullopt);

    float parentGlobalPositionX = 0.f;
    float parentGlobalPositionY = 0.f;
    auto parent = GetParent();
    if (parent) {
        parentGlobalPositionX = parent->globalPositionX_;
        parentGlobalPositionY = parent->globalPositionY_;
    }
    globalPositionX_ = parentGlobalPositionX + localGeometry_->GetX();
    globalPositionY_ = parentGlobalPositionY + localGeometry_->GetY();
}

template<typename ModifierName, typename PropertyName, typename T>
void RSNode::SetProperty(RSModifierType modifierType, T value)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
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
    if (alpha < 1) {
        SetDrawNode();
    }
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
    if (bounds.x_ != 0 || bounds.y_ != 0) {
        SetDrawNode();
    }
}

void RSNode::SetBounds(float positionX, float positionY, float width, float height)
{
    SetBounds({ positionX, positionY, width, height });
}

void RSNode::SetBoundsWidth(float width)
{
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::BOUNDS);
        if (iter == propertyModifiers_.end()) {
            SetBounds(0.f, 0.f, width, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    }

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
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::BOUNDS);
        if (iter == propertyModifiers_.end()) {
            SetBounds(0.f, 0.f, 0.f, height);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    }

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
    if (bounds.x_ != 0 || bounds.y_ != 0) {
        SetDrawNode();
    }
}

void RSNode::SetFrame(float positionX, float positionY, float width, float height)
{
    SetFrame({ positionX, positionY, width, height });
}

void RSNode::SetFramePositionX(float positionX)
{
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::FRAME);
        if (iter == propertyModifiers_.end()) {
            SetFrame(positionX, 0.f, 0.f, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    }

    if (property == nullptr) {
        return;
    }
    auto frame = property->Get();
    frame.x_ = positionX;
    property->Set(frame);
    SetDrawNode();
}

void RSNode::SetFramePositionY(float positionY)
{
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::FRAME);
        if (iter == propertyModifiers_.end()) {
            SetFrame(0.f, positionY, 0.f, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    }

    if (property == nullptr) {
        return;
    }
    auto frame = property->Get();
    frame.y_ = positionY;
    property->Set(frame);
    SetDrawNode();
}

void RSNode::SetSandBox(std::optional<Vector2f> parentPosition)
{
    if (!parentPosition.has_value()) {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
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
    if (drawNodeChangeCallback_) {
        drawNodeChangeCallback_(shared_from_this(), true);
    }
    SetProperty<RSPositionZModifier, RSAnimatableProperty<float>>(RSModifierType::POSITION_Z, positionZ);
}

void RSNode::SetPositionZApplicableCamera3D(bool isApplicable)
{
    SetProperty<RSPositionZApplicableCamera3DModifier, RSProperty<bool>>(
        RSModifierType::POSITION_Z_APPLICABLE_CAMERA3D, isApplicable);
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
    std::shared_ptr<RSAnimatableProperty<Vector2f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::PIVOT);
        if (iter == propertyModifiers_.end()) {
            SetPivot(pivotX, 0.5f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    }

    if (property == nullptr) {
        return;
    }
    auto pivot = property->Get();
    pivot.x_ = pivotX;
    property->Set(pivot);
}

void RSNode::SetPivotY(float pivotY)
{
    std::shared_ptr<RSAnimatableProperty<Vector2f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::PIVOT);
        if (iter == propertyModifiers_.end()) {
            SetPivot(0.5f, pivotY);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    }

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
    std::shared_ptr<RSAnimatableProperty<Vector2f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
        if (iter == propertyModifiers_.end()) {
            SetTranslate({ translate, 0.f });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    }

    if (property == nullptr) {
        return;
    }
    auto trans = property->Get();
    trans.x_ = translate;
    property->Set(trans);
}

void RSNode::SetTranslateY(float translate)
{
    std::shared_ptr<RSAnimatableProperty<Vector2f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
        if (iter == propertyModifiers_.end()) {
            SetTranslate({ 0.f, translate });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    }

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
    std::shared_ptr<RSAnimatableProperty<Vector2f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::SCALE);
        if (iter == propertyModifiers_.end()) {
            SetScale(scaleX, 1.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    }

    if (property == nullptr) {
        return;
    }
    auto scale = property->Get();
    scale.x_ = scaleX;
    property->Set(scale);
}

void RSNode::SetScaleY(float scaleY)
{
    std::shared_ptr<RSAnimatableProperty<Vector2f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto iter = propertyModifiers_.find(RSModifierType::SCALE);
        if (iter == propertyModifiers_.end()) {
            SetScale(1.f, scaleY);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    }

    if (property == nullptr) {
        return;
    }
    auto scale = property->Get();
    scale.y_ = scaleY;
    property->Set(scale);
}

void RSNode::SetScaleZ(const float& scaleZ)
{
    SetProperty<RSScaleZModifier, RSAnimatableProperty<float>>(RSModifierType::SCALE_Z, scaleZ);
}

void RSNode::SetSkew(float skew)
{
    SetSkew({ skew, skew, skew });
}

void RSNode::SetSkew(float skewX, float skewY)
{
    SetSkew({ skewX, skewY, 0.f });
}

void RSNode::SetSkew(float skewX, float skewY, float skewZ)
{
    SetSkew({ skewX, skewY, skewZ });
}

void RSNode::SetSkew(const Vector3f& skew)
{
    SetProperty<RSSkewModifier, RSAnimatableProperty<Vector3f>>(RSModifierType::SKEW, skew);
}

void RSNode::SetSkewX(float skewX)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto iter = propertyModifiers_.find(RSModifierType::SKEW);
    if (iter == propertyModifiers_.end()) {
        SetSkew(skewX, 0.f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(iter->second->GetProperty());
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
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto iter = propertyModifiers_.find(RSModifierType::SKEW);
    if (iter == propertyModifiers_.end()) {
        SetSkew(0.f, skewY);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto skew = property->Get();
    skew.y_ = skewY;
    property->Set(skew);
}

void RSNode::SetRSUIContext(std::shared_ptr<RSUIContext> rsUIContext)
{
    if (rsUIContext == nullptr) {
        return;
    }
    auto rsContext = rsUIContext_.lock();
    if ((rsContext != nullptr) && (rsContext == rsUIContext)) {
        return;
    }

    RSModifierExtractor rsModifierExtractor(id_, rsUIContext);
    stagingPropertiesExtractor_ = rsModifierExtractor;
    RSShowingPropertiesFreezer showingPropertiesFreezer(id_, rsUIContext);
    showingPropertiesFreezer_ = showingPropertiesFreezer;
    // step1 register node to new nodeMap
    RegisterNodeMap();

    // if have old rsContext, should remove nodeId from old nodeMap and travel child
    if (rsContext != nullptr) {
        // step2 remove node from old context
        rsContext->GetMutableNodeMap().UnregisterNode(id_);
        // sync child
        for (uint32_t index = 0; index < children_.size(); index++) {
            if (auto childPtr = rsContext->GetNodeMap().GetNode(children_[index])) {
                childPtr->SetRSUIContext(rsUIContext);
            }
        }
    }
    // step3 sign
    rsUIContext_ = rsUIContext;
}

void RSNode::SetSkewZ(float skewZ)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto iter = propertyModifiers_.find(RSModifierType::SKEW);
    if (iter == propertyModifiers_.end()) {
        SetSkew(0.f, 0.f, skewZ);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto skew = property->Get();
    skew.z_ = skewZ;
    property->Set(skew);
}

void RSNode::SetPersp(float persp)
{
    SetPersp({ persp, persp, 0.f, 1.f });
}

void RSNode::SetPersp(float perspX, float perspY)
{
    SetPersp({ perspX, perspY, 0.f, 1.f });
}

void RSNode::SetPersp(float perspX, float perspY, float perspZ, float perspW)
{
    SetPersp({ perspX, perspY, perspZ, perspW });
}

void RSNode::SetPersp(const Vector4f& persp)
{
    SetProperty<RSPerspModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::PERSP, persp);
}

void RSNode::SetPerspX(float perspX)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto iter = propertyModifiers_.find(RSModifierType::PERSP);
    if (iter == propertyModifiers_.end()) {
        SetPersp({perspX, 0.f, 0.0f, 1.0f});
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto persp = property->Get();
    persp.x_ = perspX;
    property->Set(persp);
}

void RSNode::SetPerspY(float perspY)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto iter = propertyModifiers_.find(RSModifierType::PERSP);
    if (iter == propertyModifiers_.end()) {
        SetPersp({0.f, perspY, 0.f, 1.f});
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto persp = property->Get();
    persp.y_ = perspY;
    property->Set(persp);
}

void RSNode::SetPerspZ(float perspZ)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    auto iter = propertyModifiers_.find(RSModifierType::PERSP);
    if (iter == propertyModifiers_.end()) {
        SetPersp({0.f, 0.f, perspZ, 1.f});
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto persp = property->Get();
    persp.z_ = perspZ;
    property->Set(persp);
}

void RSNode::SetPerspW(float perspW)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    auto iter = propertyModifiers_.find(RSModifierType::PERSP);
    if (iter == propertyModifiers_.end()) {
        SetPersp({0.f, 0.f, 0.f, perspW});
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto persp = property->Get();
    persp.w_ = perspW;
    property->Set(persp);
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
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
    if (NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> cmdForRemote = std::make_unique<RSAnimationCreateParticle>(GetId(), animation);
        AddCommand(cmdForRemote, true, GetFollowType(), GetId());
    }
}

void RSNode::SetParticleDrawRegion(std::vector<ParticleParams>& particleParams)
{
    Vector4f bounds = GetStagingProperties().GetBounds();
    float boundsRight = bounds.x_ + bounds.z_;
    float boundsBottom = bounds.y_ + bounds.w_;
    size_t emitterCount = particleParams.size();
    std::vector<float> left(emitterCount);
    std::vector<float> top(emitterCount);
    std::vector<float> right(emitterCount);
    std::vector<float> bottom(emitterCount);
    for (size_t i = 0; i < emitterCount; i++) {
        auto particleType = particleParams[i].emitterConfig_.type_;
        auto position = particleParams[i].emitterConfig_.position_;
        auto emitSize = particleParams[i].emitterConfig_.emitSize_;
        float scaleMax = particleParams[i].scale_.val_.end_;
        if (particleType == ParticleType::POINTS) {
            auto diameMax = particleParams[i].emitterConfig_.radius_ * 2 * scaleMax; // diameter = 2 * radius
            left[i] = std::min(bounds.x_ - diameMax, bounds.x_ + position.x_ - diameMax);
            top[i] = std::min(bounds.y_ - diameMax, bounds.y_ + position.y_ - diameMax);
            right[i] = std::max(boundsRight + diameMax + diameMax, position.x_ + emitSize.x_ + diameMax + diameMax);
            bottom[i] = std::max(boundsBottom + diameMax + diameMax, position.y_ + emitSize.y_ + diameMax + diameMax);
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
            left[i] = std::min(bounds.x_ - imageSizeWidthMax, bounds.x_ + position.x_ - imageSizeWidthMax);
            top[i] = std::min(bounds.y_ - imageSizeHeightMax, bounds.y_ + position.y_ - imageSizeHeightMax);
            right[i] = std::max(boundsRight + imageSizeWidthMax + imageSizeWidthMax,
                position.x_ + emitSize.x_ + imageSizeWidthMax + imageSizeWidthMax);
            bottom[i] = std::max(boundsBottom + imageSizeHeightMax + imageSizeHeightMax,
                position.y_ + emitSize.y_ + imageSizeHeightMax + imageSizeHeightMax);
        }
    }
    if (emitterCount != 0) {
        float l = *std::min_element(left.begin(), left.end());
        float t = *std::min_element(top.begin(), top.end());
        boundsRight = *std::max_element(right.begin(), right.end());
        boundsBottom = *std::max_element(bottom.begin(), bottom.end());
        SetDrawRegion(std::make_shared<RectF>(l - bounds.x_, t - bounds.y_, boundsRight - l, boundsBottom - t));
    }
}

// Update Particle Emitter
void RSNode::SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para)
{
    SetProperty<RSEmitterUpdaterModifier, RSProperty<std::vector<std::shared_ptr<EmitterUpdater>>>>(
        RSModifierType::PARTICLE_EMITTER_UPDATER, para);
}

// Set Particle Noise Field
void RSNode::SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para)
{
    SetProperty<RSParticleNoiseFieldsModifier, RSProperty<std::shared_ptr<ParticleNoiseFields>>>(
        RSModifierType::PARTICLE_NOISE_FIELD, para);
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
    if (color.GetAlpha() > 0) {
        SetDrawNode();
    }
}

void RSNode::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
    SetProperty<RSBackgroundShaderModifier, RSProperty<std::shared_ptr<RSShader>>>(
        RSModifierType::BACKGROUND_SHADER, shader);
}

void RSNode::SetBackgroundShaderProgress(const float& progress)
{
    SetProperty<RSBackgroundShaderProgressModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_SHADER_PROGRESS, progress);
}

// background
void RSNode::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    if (image) {
        image->SetNodeId(GetId());
    }
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

// set dash width for border
void RSNode::SetBorderDashWidth(const Vector4f& dashWidth)
{
    SetProperty<RSBorderDashWidthModifier, RSProperty<Vector4f>>(
        RSModifierType::BORDER_DASH_WIDTH, dashWidth);
}

// set dash gap for border
void RSNode::SetBorderDashGap(const Vector4f& dashGap)
{
    SetProperty<RSBorderDashGapModifier, RSProperty<Vector4f>>(
        RSModifierType::BORDER_DASH_GAP, dashGap);
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

void RSNode::SetOutlineDashWidth(const Vector4f& dashWidth)
{
    SetProperty<RSOutlineDashWidthModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::OUTLINE_DASH_WIDTH, dashWidth);
}

void RSNode::SetOutlineDashGap(const Vector4f& dashGap)
{
    SetProperty<RSOutlineDashGapModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::OUTLINE_DASH_GAP, dashGap);
}

void RSNode::SetOutlineRadius(const Vector4f& radius)
{
    SetProperty<RSOutlineRadiusModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::OUTLINE_RADIUS, radius);
}

void RSNode::SetUIBackgroundFilter(const OHOS::Rosen::Filter* backgroundFilter)
{
    if (backgroundFilter == nullptr) {
        ROSEN_LOGE("Failed to set backgroundFilter, backgroundFilter is null!");
        return;
    }
    // To do: generate composed filter here. Now we just set background blur in v1.0.
    auto filterParas = backgroundFilter->GetAllPara();
    for (const auto& filterPara : filterParas) {
        if (filterPara->GetParaType() == FilterPara::BLUR) {
            auto filterBlurPara = std::static_pointer_cast<FilterBlurPara>(filterPara);
            auto blurRadius = filterBlurPara->GetRadius();
            SetBackgroundBlurRadiusX(blurRadius);
            SetBackgroundBlurRadiusY(blurRadius);
        }
        if (filterPara->GetParaType() == FilterPara::WATER_RIPPLE) {
            auto waterRipplePara = std::static_pointer_cast<WaterRipplePara>(filterPara);
            auto waveCount = waterRipplePara->GetWaveCount();
            auto rippleCenterX = waterRipplePara->GetRippleCenterX();
            auto rippleCenterY = waterRipplePara->GetRippleCenterY();
            auto progress = waterRipplePara->GetProgress();
            auto rippleMode = waterRipplePara->GetRippleMode();
            RSWaterRipplePara params = {
                waveCount,
                rippleCenterX,
                rippleCenterY,
                rippleMode
            };
            SetWaterRippleParams(params, progress);
        }
    }
}

void RSNode::SetUICompositingFilter(const OHOS::Rosen::Filter* compositingFilter)
{
    if (compositingFilter == nullptr) {
        ROSEN_LOGE("Failed to set compositingFilter, compositingFilter is null!");
        return;
    }
    // To do: generate composed filter here. Now we just set compositing blur in v1.0.
    auto filterParas = compositingFilter->GetAllPara();
    for (const auto& filterPara : filterParas) {
        if (filterPara->GetParaType() == FilterPara::BLUR) {
            auto filterBlurPara = std::static_pointer_cast<FilterBlurPara>(filterPara);
            auto blurRadius = filterBlurPara->GetRadius();
            SetForegroundBlurRadiusX(blurRadius);
            SetForegroundBlurRadiusY(blurRadius);
        }
        if (filterPara->GetParaType() == FilterPara::PIXEL_STRETCH) {
            auto pixelStretchPara = std::static_pointer_cast<PixelStretchPara>(filterPara);
            auto stretchPercent = pixelStretchPara->GetStretchPercent();
            SetPixelStretchPercent(stretchPercent, pixelStretchPara->GetTileMode());
        }
        if (filterPara->GetParaType() == FilterPara::RADIUS_GRADIENT_BLUR) {
            auto radiusGradientBlurPara = std::static_pointer_cast<RadiusGradientBlurPara>(filterPara);
            auto rsLinearGradientBlurPara = std::make_shared<RSLinearGradientBlurPara>(
                radiusGradientBlurPara->GetBlurRadius(),
                radiusGradientBlurPara->GetFractionStops(),
                radiusGradientBlurPara->GetDirection());
            rsLinearGradientBlurPara->isRadiusGradient_ = true;
            SetLinearGradientBlurPara(rsLinearGradientBlurPara);
        }
    }
}

void RSNode::SetUIForegroundFilter(const OHOS::Rosen::Filter* foregroundFilter)
{
    if (foregroundFilter == nullptr) {
        ROSEN_LOGE("Failed to set foregroundFilter, foregroundFilter is null!");
        return;
    }
    // To do: generate composed filter here. Now we just set pixel stretch in v1.0.
    auto filterParas = foregroundFilter->GetAllPara();
    for (const auto& filterPara : filterParas) {
        if (filterPara->GetParaType() == FilterPara::BLUR) {
            auto filterBlurPara = std::static_pointer_cast<FilterBlurPara>(filterPara);
            auto blurRadius = filterBlurPara->GetRadius();
            SetForegroundEffectRadius(blurRadius);
        }
        if (filterPara->GetParaType() == FilterPara::FLY_OUT) {
            auto flyOutPara = std::static_pointer_cast<FlyOutPara>(filterPara);
            auto flyMode = flyOutPara->GetFlyMode();
            auto degree = flyOutPara->GetDegree();
            RSFlyOutPara rs_fly_out_param = {
                flyMode,
            };
            SetFlyOutParams(rs_fly_out_param, degree);
        }
        if (filterPara->GetParaType() == FilterPara::DISTORT) {
            auto distortPara = std::static_pointer_cast<DistortPara>(filterPara);
            auto distortionK = distortPara->GetDistortionK();
            SetDistortionK(distortionK);
        }
    }
}

void RSNode::SetVisualEffect(const VisualEffect* visualEffect)
{
    if (visualEffect == nullptr) {
        ROSEN_LOGE("Failed to set visualEffect, visualEffect is null!");
        return;
    }
    // To do: generate composed visual effect here. Now we just set background brightness in v1.0.
    auto visualEffectParas = visualEffect->GetAllPara();
    for (const auto& visualEffectPara : visualEffectParas) {
        if (visualEffectPara->GetParaType() != VisualEffectPara::BACKGROUND_COLOR_EFFECT) {
            continue;
        }
        auto backgroundColorEffectPara = std::static_pointer_cast<BackgroundColorEffectPara>(visualEffectPara);
        auto blender = backgroundColorEffectPara->GetBlender();
        auto brightnessBlender = std::static_pointer_cast<BrightnessBlender>(blender);
        if (brightnessBlender == nullptr) {
            continue;
        }
        auto fraction = brightnessBlender->GetFraction();
        SetBgBrightnessFract(fraction);
        SetBgBrightnessParams({ brightnessBlender->GetLinearRate(), brightnessBlender->GetDegree(),
            brightnessBlender->GetCubicRate(), brightnessBlender->GetQuadRate(), brightnessBlender->GetSaturation(),
            { brightnessBlender->GetPositiveCoeff().data_[0], brightnessBlender->GetPositiveCoeff().data_[1],
                brightnessBlender->GetPositiveCoeff().data_[2] },
            { brightnessBlender->GetNegativeCoeff().data_[0], brightnessBlender->GetNegativeCoeff().data_[1],
                brightnessBlender->GetNegativeCoeff().data_[2] } });
    }
}

void RSNode::SetBlender(const Blender* blender)
{
    if (blender == nullptr) {
        ROSEN_LOGE("RSNode::SetBlender: blender is null!");
        return;
    }

    if (Blender::BRIGHTNESS_BLENDER == blender->GetBlenderType()) {
        auto brightnessBlender = static_cast<const BrightnessBlender*>(blender);
        if (brightnessBlender != nullptr) {
            SetFgBrightnessFract(brightnessBlender->GetFraction());
            SetFgBrightnessParams({ brightnessBlender->GetLinearRate(), brightnessBlender->GetDegree(),
                brightnessBlender->GetCubicRate(), brightnessBlender->GetQuadRate(), brightnessBlender->GetSaturation(),
                { brightnessBlender->GetPositiveCoeff().x_, brightnessBlender->GetPositiveCoeff().y_,
                    brightnessBlender->GetPositiveCoeff().z_ },
                { brightnessBlender->GetNegativeCoeff().x_, brightnessBlender->GetNegativeCoeff().y_,
                    brightnessBlender->GetNegativeCoeff().z_ } });
        }
    }
}

void RSNode::SetForegroundEffectRadius(const float blurRadius)
{
    SetProperty<RSForegroundEffectRadiusModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_EFFECT_RADIUS, blurRadius);
}

void RSNode::SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter)
{
    if (backgroundFilter == nullptr) {
        SetBackgroundBlurRadius(0.f);
        SetBackgroundBlurSaturation(1.f);
        SetBackgroundBlurBrightness(1.f);
        SetBackgroundBlurMaskColor(RSColor());
        SetBackgroundBlurColorMode(BLUR_COLOR_MODE::DEFAULT);
        SetBackgroundBlurRadiusX(0.f);
        SetBackgroundBlurRadiusY(0.f);
    } else if (backgroundFilter->GetFilterType() == RSFilter::MATERIAL) {
        auto materialFilter = std::static_pointer_cast<RSMaterialFilter>(backgroundFilter);
        float Radius = materialFilter->GetRadius();
        float Saturation = materialFilter->GetSaturation();
        float Brightness = materialFilter->GetBrightness();
        Color MaskColor = materialFilter->GetMaskColor();
        int ColorMode = materialFilter->GetColorMode();
        bool disableSystemAdaptation = materialFilter->GetDisableSystemAdaptation();
        SetBackgroundBlurRadius(Radius);
        SetBackgroundBlurSaturation(Saturation);
        SetBackgroundBlurBrightness(Brightness);
        SetBackgroundBlurMaskColor(MaskColor);
        SetBackgroundBlurColorMode(ColorMode);
        SetBgBlurDisableSystemAdaptation(disableSystemAdaptation);
    } else if (backgroundFilter->GetFilterType() == RSFilter::BLUR) {
        auto blurFilter = std::static_pointer_cast<RSBlurFilter>(backgroundFilter);
        float blurRadiusX = blurFilter->GetBlurRadiusX();
        float blurRadiusY = blurFilter->GetBlurRadiusY();
        bool disableSystemAdaptation = blurFilter->GetDisableSystemAdaptation();
        SetBackgroundBlurRadiusX(blurRadiusX);
        SetBackgroundBlurRadiusY(blurRadiusY);
        SetBgBlurDisableSystemAdaptation(disableSystemAdaptation);
    }
}

void RSNode::SetFilter(const std::shared_ptr<RSFilter>& filter)
{
    if (filter == nullptr) {
        SetForegroundBlurRadius(0.f);
        SetForegroundBlurSaturation(1.f);
        SetForegroundBlurBrightness(1.f);
        SetForegroundBlurMaskColor(RSColor());
        SetForegroundBlurColorMode(BLUR_COLOR_MODE::DEFAULT);
        SetForegroundBlurRadiusX(0.f);
        SetForegroundBlurRadiusY(0.f);
    } else if (filter->GetFilterType() == RSFilter::MATERIAL) {
        auto materialFilter = std::static_pointer_cast<RSMaterialFilter>(filter);
        float Radius = materialFilter->GetRadius();
        float Saturation = materialFilter->GetSaturation();
        float Brightness = materialFilter->GetBrightness();
        Color MaskColor = materialFilter->GetMaskColor();
        int ColorMode = materialFilter->GetColorMode();
        bool disableSystemAdaptation = materialFilter->GetDisableSystemAdaptation();
        SetForegroundBlurRadius(Radius);
        SetForegroundBlurSaturation(Saturation);
        SetForegroundBlurBrightness(Brightness);
        SetForegroundBlurMaskColor(MaskColor);
        SetForegroundBlurColorMode(ColorMode);
        SetFgBlurDisableSystemAdaptation(disableSystemAdaptation);
    } else if (filter->GetFilterType() == RSFilter::BLUR) {
        auto blurFilter = std::static_pointer_cast<RSBlurFilter>(filter);
        float blurRadiusX = blurFilter->GetBlurRadiusX();
        float blurRadiusY = blurFilter->GetBlurRadiusY();
        bool disableSystemAdaptation = blurFilter->GetDisableSystemAdaptation();
        SetForegroundBlurRadiusX(blurRadiusX);
        SetForegroundBlurRadiusY(blurRadiusY);
        SetFgBlurDisableSystemAdaptation(disableSystemAdaptation);
    }
}

void RSNode::SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para)
{
    SetProperty<RSLinearGradientBlurParaModifier, RSProperty<std::shared_ptr<RSLinearGradientBlurPara>>>(
        RSModifierType::LINEAR_GRADIENT_BLUR_PARA, para);
}

void RSNode::SetMotionBlurPara(const float radius, const Vector2f& anchor)
{
    Vector2f anchor1 = {anchor[0], anchor[1]};
    std::shared_ptr<MotionBlurParam> para = std::make_shared<MotionBlurParam>(radius, anchor1);
    SetProperty<RSMotionBlurParaModifier, RSProperty<std::shared_ptr<MotionBlurParam>>>(
        RSModifierType::MOTION_BLUR_PARA, para);
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

void RSNode::SetFgBrightnessParams(const RSDynamicBrightnessPara& params)
{
    // Compatible with original interfaces
    SetFgBrightnessRates(params.rates_);
    SetFgBrightnessSaturation(params.saturation_);
    SetFgBrightnessPosCoeff(params.posCoeff_);
    SetFgBrightnessNegCoeff(params.negCoeff_);
}

void RSNode::SetFgBrightnessRates(const Vector4f& rates)
{
    SetProperty<RSFgBrightnessRatesModifier,
        RSAnimatableProperty<Vector4f>>(RSModifierType::FG_BRIGHTNESS_RATES, rates);
}

void RSNode::SetFgBrightnessSaturation(const float& saturation)
{
    SetProperty<RSFgBrightnessSaturationModifier,
        RSAnimatableProperty<float>>(RSModifierType::FG_BRIGHTNESS_SATURATION, saturation);
}

void RSNode::SetFgBrightnessPosCoeff(const Vector4f& coeff)
{
    SetProperty<RSFgBrightnessPosCoeffModifier,
        RSAnimatableProperty<Vector4f>>(RSModifierType::FG_BRIGHTNESS_POSCOEFF, coeff);
}

void RSNode::SetFgBrightnessNegCoeff(const Vector4f& coeff)
{
    SetProperty<RSFgBrightnessNegCoeffModifier,
        RSAnimatableProperty<Vector4f>>(RSModifierType::FG_BRIGHTNESS_NEGCOEFF, coeff);
}

void RSNode::SetFgBrightnessFract(const float& fract)
{
    SetProperty<RSFgBrightnessFractModifier,
        RSAnimatableProperty<float>>(RSModifierType::FG_BRIGHTNESS_FRACTION, fract);
}

void RSNode::SetBgBrightnessParams(const RSDynamicBrightnessPara& params)
{
    ROSEN_LOGE("LJQDEBUG: params.saturation_ %{public}f", params.saturation_);
    // Compatible with original interfaces
    SetBgBrightnessRates(params.rates_);
    SetBgBrightnessSaturation(params.saturation_);
    SetBgBrightnessPosCoeff(params.posCoeff_);
    SetBgBrightnessNegCoeff(params.negCoeff_);
}

void RSNode::SetBgBrightnessRates(const Vector4f& rates)
{
    SetProperty<RSBgBrightnessRatesModifier,
        RSAnimatableProperty<Vector4f>>(RSModifierType::BG_BRIGHTNESS_RATES, rates);
}

void RSNode::SetBgBrightnessSaturation(const float& saturation)
{
    SetProperty<RSBgBrightnessSaturationModifier,
        RSAnimatableProperty<float>>(RSModifierType::BG_BRIGHTNESS_SATURATION, saturation);
}

void RSNode::SetBgBrightnessPosCoeff(const Vector4f& coeff)
{
    SetProperty<RSBgBrightnessPosCoeffModifier,
        RSAnimatableProperty<Vector4f>>(RSModifierType::BG_BRIGHTNESS_POSCOEFF, coeff);
}

void RSNode::SetBgBrightnessNegCoeff(const Vector4f& coeff)
{
    SetProperty<RSBgBrightnessNegCoeffModifier,
        RSAnimatableProperty<Vector4f>>(RSModifierType::BG_BRIGHTNESS_NEGCOEFF, coeff);
}

void RSNode::SetBgBrightnessFract(const float& fract)
{
    SetProperty<RSBgBrightnessFractModifier,
        RSAnimatableProperty<float>>(RSModifierType::BG_BRIGHTNESS_FRACTION, fract);
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
    SetClipRRect(std::make_shared<RRect>(clipRect, clipRadius));
}

void RSNode::SetClipRRect(const std::shared_ptr<RRect>& rrect)
{
    SetProperty<RSClipRRectModifier, RSAnimatableProperty<RRect>>(
        RSModifierType::CLIP_RRECT, rrect ? *rrect : RRect());
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

void RSNode::SetCustomClipToFrame(const Vector4f& clipRect)
{
    SetProperty<RSCustomClipToFrameModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::CUSTOM_CLIP_TO_FRAME, clipRect);
}

void RSNode::SetHDRBrightness(const float& hdrBrightness)
{
    SetProperty<RSHDRBrightnessModifier, RSAnimatableProperty<float>>(
        RSModifierType::HDR_BRIGHTNESS, hdrBrightness);
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

void RSNode::SetUseEffectType(UseEffectType useEffectType)
{
    SetProperty<RSUseEffectTypeModifier, RSProperty<int>>(
        RSModifierType::USE_EFFECT_TYPE, static_cast<int>(useEffectType));
}

void RSNode::SetAlwaysSnapshot(bool enable)
{
    SetProperty<RSAlwaysSnapshotModifier, RSProperty<bool>>(
        RSModifierType::ALWAYS_SNAPSHOT, static_cast<bool>(enable));
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

void RSNode::SetPixelStretch(const Vector4f& stretchSize, Drawing::TileMode stretchTileMode)
{
    SetProperty<RSPixelStretchModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::PIXEL_STRETCH, stretchSize);
    SetProperty<RSPixelStretchTileModeModifier, RSProperty<int>>(
        RSModifierType::PIXEL_STRETCH_TILE_MODE, static_cast<int>(stretchTileMode));
}

void RSNode::SetPixelStretchPercent(const Vector4f& stretchPercent, Drawing::TileMode stretchTileMode)
{
    SetProperty<RSPixelStretchPercentModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::PIXEL_STRETCH_PERCENT,
        stretchPercent);
    SetProperty<RSPixelStretchTileModeModifier, RSProperty<int>>(
        RSModifierType::PIXEL_STRETCH_TILE_MODE, static_cast<int>(stretchTileMode));
}

void RSNode::SetWaterRippleParams(const RSWaterRipplePara& params, float progress)
{
    SetProperty<RSWaterRippleParamsModifier,
        RSProperty<RSWaterRipplePara>>(RSModifierType::WATER_RIPPLE_PARAMS, params);
    SetProperty<RSWaterRippleProgressModifier,
        RSAnimatableProperty<float>>(RSModifierType::WATER_RIPPLE_PROGRESS, progress);
}

void RSNode::SetFlyOutParams(const RSFlyOutPara& params, float degree)
{
    SetProperty<RSFlyOutParamsModifier,
        RSProperty<RSFlyOutPara>>(RSModifierType::FLY_OUT_PARAMS, params);
    SetProperty<RSFlyOutDegreeModifier,
        RSAnimatableProperty<float>>(RSModifierType::FLY_OUT_DEGREE, degree);
}

void RSNode::SetDistortionK(const float distortionK)
{
    SetProperty<RSDistortionKModifier, RSAnimatableProperty<float>>(RSModifierType::DISTORTION_K, distortionK);
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
        AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::SetTakeSurfaceForUIFlag()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetTakeSurfaceForUIFlag>(GetId());
    auto transaction = GetRSTransaction();
    if (transaction != nullptr) {
        transaction->AddCommand(command, IsRenderServiceNode());
        transaction->FlushImplicitTransaction();
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode());
            transactionProxy->FlushImplicitTransaction();
        }
    }
}

void RSNode::SetSpherizeDegree(float spherizeDegree)
{
    SetProperty<RSSpherizeModifier, RSAnimatableProperty<float>>(RSModifierType::SPHERIZE, spherizeDegree);
}

void RSNode::SetAttractionEffect(float fraction, const Vector2f& destinationPoint)
{
    SetAttractionEffectFraction(fraction);
    SetAttractionEffectDstPoint(destinationPoint);
}

void RSNode::SetAttractionEffectFraction(float fraction)
{
    SetProperty<RSAttractionFractionModifier, RSAnimatableProperty<float>>(RSModifierType::ATTRACTION_FRACTION,
        fraction);
}

void RSNode::SetAttractionEffectDstPoint(Vector2f destinationPoint)
{
    SetProperty<RSAttractionDstPointModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::ATTRACTION_DSTPOINT,
        destinationPoint);
}

void RSNode::SetLightUpEffectDegree(float LightUpEffectDegree)
{
    SetProperty<RSLightUpEffectModifier, RSAnimatableProperty<float>>(
        RSModifierType::LIGHT_UP_EFFECT, LightUpEffectDegree);
}

void RSNode::NotifyTransition(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
{
    auto rsUIContext = rsUIContext_.lock();
    auto implicitAnimator = rsUIContext ? rsUIContext->GetRSImplicitAnimator()
                                        : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to notify transition, implicit animator is null!");
        return;
    }

    if (!implicitAnimator->NeedImplicitAnimation()) {
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
        rsUIContext, implicitAnimator);

    implicitAnimator->BeginImplicitTransition(effect, isTransitionIn);
    for (auto& customEffect : customEffects) {
        customEffect->Identity();
    }
    implicitAnimator->CreateImplicitTransition(*this);
    implicitAnimator->EndImplicitTransition();
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

void RSNode::SetBackgroundBlurRadius(float radius)
{
    SetProperty<RSBackgroundBlurRadiusModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_RADIUS, radius);
}

void RSNode::SetBackgroundBlurSaturation(float saturation)
{
    SetProperty<RSBackgroundBlurSaturationModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_SATURATION, saturation);
}

void RSNode::SetBackgroundBlurBrightness(float brightness)
{
    SetProperty<RSBackgroundBlurBrightnessModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_BRIGHTNESS, brightness);
}

void RSNode::SetBackgroundBlurMaskColor(Color maskColor)
{
    SetProperty<RSBackgroundBlurMaskColorModifier, RSAnimatableProperty<Color>>(
        RSModifierType::BACKGROUND_BLUR_MASK_COLOR, maskColor);
}

void RSNode::SetBackgroundBlurColorMode(int colorMode)
{
    SetProperty<RSBackgroundBlurColorModeModifier, RSProperty<int>>(
        RSModifierType::BACKGROUND_BLUR_COLOR_MODE, colorMode);
}

void RSNode::SetBackgroundBlurRadiusX(float blurRadiusX)
{
    SetProperty<RSBackgroundBlurRadiusXModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_RADIUS_X, blurRadiusX);
}

void RSNode::SetBackgroundBlurRadiusY(float blurRadiusY)
{
    SetProperty<RSBackgroundBlurRadiusYModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_RADIUS_Y, blurRadiusY);
}

void RSNode::SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
    SetProperty<RSBgBlurDisableSystemAdaptationModifier, RSProperty<bool>>(
        RSModifierType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION, disableSystemAdaptation);
}

void RSNode::SetForegroundBlurRadius(float radius)
{
    SetProperty<RSForegroundBlurRadiusModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_RADIUS, radius);
}

void RSNode::SetForegroundBlurSaturation(float saturation)
{
    SetProperty<RSForegroundBlurSaturationModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_SATURATION, saturation);
}

void RSNode::SetForegroundBlurBrightness(float brightness)
{
    SetProperty<RSForegroundBlurBrightnessModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_BRIGHTNESS, brightness);
}

void RSNode::SetForegroundBlurMaskColor(Color maskColor)
{
    SetProperty<RSForegroundBlurMaskColorModifier, RSAnimatableProperty<Color>>(
        RSModifierType::FOREGROUND_BLUR_MASK_COLOR, maskColor);
}

void RSNode::SetForegroundBlurColorMode(int colorMode)
{
    SetProperty<RSForegroundBlurColorModeModifier, RSProperty<int>>(
        RSModifierType::FOREGROUND_BLUR_COLOR_MODE, colorMode);
}

void RSNode::SetForegroundBlurRadiusX(float blurRadiusX)
{
    SetProperty<RSForegroundBlurRadiusXModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_RADIUS_X, blurRadiusX);
}

void RSNode::SetForegroundBlurRadiusY(float blurRadiusY)
{
    SetProperty<RSForegroundBlurRadiusYModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_RADIUS_Y, blurRadiusY);
}

void RSNode::SetFgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
    SetProperty<RSFgBlurDisableSystemAdaptationModifier, RSProperty<bool>>(
        RSModifierType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION, disableSystemAdaptation);
}

bool RSNode::AnimationCallback(AnimationId animationId, AnimationCallbackEvent event)
{
    std::shared_ptr<RSAnimation> animation = nullptr;
    {
        std::unique_lock<std::recursive_mutex> lock(animationMutex_);
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

void RSNode::ClearAllModifiers()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    for (auto [id, modifier] : modifiers_) {
        if (modifier) {
            modifier->DetachFromNode();
        }
    }
    modifiers_.clear();
    propertyModifiers_.clear();
    modifiersTypeMap_.clear();
}

void RSNode::AddModifier(const std::shared_ptr<RSModifier> modifier)
{
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        if (!modifier || modifiers_.count(modifier->GetPropertyId())) {
            return;
        }
        if (motionPathOption_ != nullptr && IsPathAnimatableModifier(modifier->GetModifierType())) {
            modifier->SetMotionPathOption(motionPathOption_);
        }
        auto rsnode = std::static_pointer_cast<RSNode>(shared_from_this());
        modifier->AttachToNode(rsnode);
        modifiers_.emplace(modifier->GetPropertyId(), modifier);
        modifiersTypeMap_.emplace((int16_t)modifier->GetModifierType(), modifier);
    }
    if (modifier->GetModifierType() == RSModifierType::NODE_MODIFIER) {
        return;
    }
    if (modifier->GetModifierType() > RSModifierType::FRAME &&
        modifier->GetModifierType() != RSModifierType::BACKGROUND_COLOR &&
        modifier->GetModifierType() != RSModifierType::ALPHA &&
        modifier->GetModifierType() != RSModifierType::CORNER_RADIUS) {
        SetDrawNode();
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
    if (NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> cmdForRemote =
            std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
        AddCommand(cmdForRemote, true, GetFollowType(), GetId());
    }
    ROSEN_LOGD("RSNode::add modifier, node id: %{public}" PRIu64 ", type: %{public}s",
            GetId(), modifier->GetModifierTypeString().c_str());
}

void RSNode::DoFlushModifier()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (modifiers_.empty()) {
        return;
    }
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> removeAllModifiersCommand = std::make_unique<RSRemoveAllModifiers>(GetId());
    AddCommand(removeAllModifiersCommand, IsRenderServiceNode(), GetFollowType(), GetId());
    for (const auto& [_, modifier] : modifiers_) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
        AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        ROSEN_LOGD("RSNode::flush modifier, node id: %{public}" PRIu64 ", type: %{public}s",
            GetId(), modifier->GetModifierTypeString().c_str());
    }
}

void RSNode::RemoveModifier(const std::shared_ptr<RSModifier> modifier)
{
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        if (!modifier) {
            return;
        }
        auto iter = modifiers_.find(modifier->GetPropertyId());
        if (iter == modifiers_.end()) {
            return;
        }
        auto deleteType = modifier->GetModifierType();
        bool isExist = false;
        modifiers_.erase(iter);
        for (auto [id, value] : modifiers_) {
            if (value && value->GetModifierType() == deleteType) {
                modifiersTypeMap_.emplace((int16_t)deleteType, value);
                isExist = true;
                break;
            }
        }
        if (!isExist) {
            modifiersTypeMap_.erase((int16_t)deleteType);
        }
        modifier->DetachFromNode();
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSRemoveModifier>(GetId(), modifier->GetPropertyId());
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
    if (NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> cmdForRemote =
            std::make_unique<RSRemoveModifier>(GetId(), modifier->GetPropertyId());
        AddCommand(cmdForRemote, true, GetFollowType(), GetId());
    }
    ROSEN_LOGD("RSNode::remove modifier, node id: %{public}" PRIu64 ", type: %{public}s", GetId(),
        modifier->GetModifierTypeString().c_str());
}

const std::shared_ptr<RSModifier> RSNode::GetModifier(const PropertyId& propertyId)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN_VALUE(CheckMultiThreadAccess(__func__), nullptr);
    auto iter = modifiers_.find(propertyId);
    if (iter != modifiers_.end()) {
        return iter->second;
    }

    return {};
}

void RSNode::UpdateModifierMotionPathOption()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
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

bool RSNode::CheckMultiThreadAccess(const std::string& func) const
{
    if (isSkipCheckInMultiInstance_) {
        return true;
    }
    auto rsContext = rsUIContext_.lock();
    // Node toDo
    if (rsContext == nullptr) {
        return true;
    }
#ifdef ROSEN_OHOS
    thread_local auto tid = gettid();
    if ((tid != ExtractTid(rsContext->GetToken()))) {
        ROSEN_LOGE("RSNode::CheckMultiThreadAccess nodeId is %{public}" PRIu64 ", func:%{public}s is not "
                   "correspond tid is "
                   "%{public}d context "
                   "tid is %{public}d"
                   "nodeType is %{public}d",
            GetId(),
            func.c_str(),
            tid,
            ExtractTid(rsContext->GetToken()),
            GetType());
        return false;
    }
#endif
    return true;
}

void RSNode::SetSkipCheckInMultiInstance(bool isSkipCheckInMultiInstance)
{
    isSkipCheckInMultiInstance_ = isSkipCheckInMultiInstance;
}

std::vector<PropertyId> RSNode::GetModifierIds() const
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    std::vector<PropertyId> ids;
    CHECK_FALSE_RETURN_VALUE(CheckMultiThreadAccess(__func__), ids);
    for (const auto& [id, _] : modifiers_) {
        ids.push_back(id);
    }
    return ids;
}

void RSNode::MarkAllExtendModifierDirty()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (extendModifierIsDirty_) {
        return;
    }

    auto rsUIContext = rsUIContext_.lock();
    auto modifierManager = rsUIContext ? rsUIContext->GetRSModifierManager() :
        RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    extendModifierIsDirty_ = true;
    for (auto& [id, modifier] : modifiers_) {
        if (modifier->GetModifierType() < RSModifierType::CUSTOM) {
            continue;
        }
        modifier->SetDirty(true, modifierManager);
    }
}

void RSNode::ResetExtendModifierDirty()
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    extendModifierIsDirty_ = false;
}

void RSNode::SetIsCustomTextType(bool isCustomTextType)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    isCustomTextType_ = isCustomTextType;
}

bool RSNode::GetIsCustomTextType()
{
    return isCustomTextType_;
}

void RSNode::SetIsCustomTypeface(bool isCustomTypeface)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    isCustomTypeface_ = isCustomTypeface;
}

bool RSNode::GetIsCustomTypeface()
{
    return isCustomTypeface_;
}

void RSNode::SetDrawRegion(std::shared_ptr<RectF> rect)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (drawRegion_ != rect) {
        drawRegion_ = rect;
        std::unique_ptr<RSCommand> command = std::make_unique<RSSetDrawRegion>(GetId(), rect);
        AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
    }
}

void RSNode::RegisterTransitionPair(NodeId inNodeId, NodeId outNodeId, const bool isInSameWindow)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSRegisterGeometryTransitionNodePair>(inNodeId, outNodeId, isInSameWindow);
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

void RSNode::RegisterTransitionPair(const std::shared_ptr<RSUIContext> rsUIContext, NodeId inNodeId, NodeId outNodeId,
    const bool isInSameWindow)
{
    if (rsUIContext == nullptr) {
        ROSEN_LOGE("RSNode::RegisterTransitionPair, rsUIContext is nullptr");
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSRegisterGeometryTransitionNodePair>(inNodeId, outNodeId,
        isInSameWindow);
    auto transaction = rsUIContext->GetRSTransaction();
    if (transaction != nullptr) {
        transaction->AddCommand(command, true);
    }
}

void RSNode::UnregisterTransitionPair(const std::shared_ptr<RSUIContext> rsUIContext, NodeId inNodeId, NodeId outNodeId)
{
    if (rsUIContext == nullptr) {
        ROSEN_LOGE("RSNode::UnregisterTransitionPair, rsUIContext is nullptr");
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSUnregisterGeometryTransitionNodePair>(inNodeId, outNodeId);
    auto transaction = rsUIContext->GetRSTransaction();
    if (transaction != nullptr) {
        transaction->AddCommand(command, true);
    }
}

void RSNode::MarkNodeGroup(bool isNodeGroup, bool isForced, bool includeProperty)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (isNodeGroup_ == isNodeGroup) {
        return;
    }
    if (!isForced && !RSSystemProperties::GetNodeGroupGroupedByUIEnabled()) {
        return;
    }
    isNodeGroup_ = isNodeGroup;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkNodeGroup>(GetId(), isNodeGroup, isForced,
        includeProperty);
    AddCommand(command, IsRenderServiceNode());
    if (isNodeGroup_) {
        SetDrawNode();
        if (GetParent()) {
            GetParent()->SetDrawNode();
        }
    }
}

void RSNode::MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (isNodeSingleFrameComposer_ != isNodeSingleFrameComposer) {
        isNodeSingleFrameComposer_ = isNodeSingleFrameComposer;
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSMarkNodeSingleFrameComposer>(GetId(), isNodeSingleFrameComposer, GetRealPid());
        AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::MarkSuggestOpincNode(bool isOpincNode, bool isNeedCalculate)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (isSuggestOpincNode_ == isOpincNode) {
        return;
    }
    isSuggestOpincNode_ = isOpincNode;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkSuggestOpincNode>(GetId(),
        isOpincNode, isNeedCalculate);
    AddCommand(command, IsRenderServiceNode());
    if (isSuggestOpincNode_) {
        SetDrawNode();
        if (GetParent()) {
            GetParent()->SetDrawNode();
        }
    }
}

void RSNode::MarkUifirstNode(bool isUifirstNode)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (isUifirstNode_ == isUifirstNode) {
        return;
    }
    isUifirstNode_ = isUifirstNode;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkUifirstNode>(GetId(), isUifirstNode);
    AddCommand(command, IsRenderServiceNode());
}
 
void RSNode::MarkUifirstNode(bool isForceFlag, bool isUifirstEnable)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (isForceFlag == isForceFlag_ && isUifirstEnable_ == isUifirstEnable) {
        return;
    }
    isForceFlag_ = isForceFlag;
    isUifirstEnable_ = isUifirstEnable;
    std::unique_ptr<RSCommand> command = std::make_unique<RSForceUifirstNode>(GetId(), isForceFlag, isUifirstEnable);
    AddCommand(command, IsRenderServiceNode());
}

void RSNode::SetDrawNode()
{
    if (isDrawNode_) {
        return;
    }
    isDrawNode_ = true;
    if (drawNodeChangeCallback_) {
        drawNodeChangeCallback_(shared_from_this(), false);
    }
}

bool RSNode::GetIsDrawn()
{
    return isDrawNode_;
}

void RSNode::SetUIFirstSwitch(RSUIFirstSwitch uiFirstSwitch)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (uiFirstSwitch_ == uiFirstSwitch) {
        return;
    }
    uiFirstSwitch_ = uiFirstSwitch;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetUIFirstSwitch>(GetId(), uiFirstSwitch);
    AddCommand(command, IsRenderServiceNode());
}

void RSNode::SetGrayScale(float grayScale)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSGrayScaleModifier, RSAnimatableProperty<float>>(RSModifierType::GRAY_SCALE, grayScale);
}

void RSNode::SetLightIntensity(float lightIntensity)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSLightIntensityModifier, RSAnimatableProperty<float>>(RSModifierType::LIGHT_INTENSITY, lightIntensity);
}

void RSNode::SetLightColor(uint32_t lightColorValue)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto lightColor = Color::FromArgbInt(lightColorValue);
    SetProperty<RSLightColorModifier, RSAnimatableProperty<Color>>(RSModifierType::LIGHT_COLOR, lightColor);
}

void RSNode::SetLightPosition(float positionX, float positionY, float positionZ)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetLightPosition(Vector4f(positionX, positionY, positionZ, 0.f));
}

void RSNode::SetLightPosition(const Vector4f& lightPosition)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSLightPositionModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::LIGHT_POSITION, lightPosition);
}

void RSNode::SetIlluminatedBorderWidth(float illuminatedBorderWidth)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSIlluminatedBorderWidthModifier, RSAnimatableProperty<float>>(
        RSModifierType::ILLUMINATED_BORDER_WIDTH, illuminatedBorderWidth);
}

void RSNode::SetIlluminatedType(uint32_t illuminatedType)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSIlluminatedTypeModifier, RSProperty<int>>(
        RSModifierType::ILLUMINATED_TYPE, illuminatedType);
}

void RSNode::SetBloom(float bloomIntensity)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSBloomModifier, RSAnimatableProperty<float>>(RSModifierType::BLOOM, bloomIntensity);
}

void RSNode::SetBrightness(float brightness)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSBrightnessModifier, RSAnimatableProperty<float>>(RSModifierType::BRIGHTNESS, brightness);
}

void RSNode::SetContrast(float contrast)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSContrastModifier, RSAnimatableProperty<float>>(RSModifierType::CONTRAST, contrast);
}

void RSNode::SetSaturate(float saturate)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSSaturateModifier, RSAnimatableProperty<float>>(RSModifierType::SATURATE, saturate);
}

void RSNode::SetSepia(float sepia)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSSepiaModifier, RSAnimatableProperty<float>>(RSModifierType::SEPIA, sepia);
}

void RSNode::SetInvert(float invert)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSInvertModifier, RSAnimatableProperty<float>>(RSModifierType::INVERT, invert);
}

void RSNode::SetAiInvert(const Vector4f& aiInvert)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSAiInvertModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::AIINVERT, aiInvert);
}

void RSNode::SetSystemBarEffect()
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSSystemBarEffectModifier, RSProperty<bool>>(RSModifierType::SYSTEMBAREFFECT, true);
}

void RSNode::SetHueRotate(float hueRotate)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetProperty<RSHueRotateModifier, RSAnimatableProperty<float>>(RSModifierType::HUE_ROTATE, hueRotate);
}

void RSNode::SetColorBlend(uint32_t colorValue)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
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
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (outOfParent != outOfParent_) {
        outOfParent_ = outOfParent;

        std::unique_ptr<RSCommand> command = std::make_unique<RSSetOutOfParent>(GetId(), outOfParent);
        AddCommand(command, IsRenderServiceNode());
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

void RSNode::SetIsOnTheTree(bool flag)
{
    if (isOnTheTree_ == flag && isOnTheTreeInit_ == true) {
        return;
    }
    isOnTheTreeInit_ = true;
    isOnTheTree_ = flag;
#ifdef RS_ENABLE_VK
    if (!flag) {
        RSModifiersDraw::InsertOffTreeNode(instanceId_, id_);
    } else {
        RSModifiersDraw::EraseOffTreeNode(instanceId_, id_);
    }
#endif
    for (const auto& childId : children_) {
        auto childPtr = RSNodeMap::Instance().GetNode(childId);
        if (childPtr == nullptr) {
            continue;
        }
        childPtr->SetIsOnTheTree(flag);
    }
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
    if (frameNodeId_ < 0) {
        child->SetDrawNode();
    }
    NodeId childId = child->GetId();
    if (child->parent_ != 0) {
        child->RemoveFromTree();
    }

    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }
    child->SetParent(id_);
    if (isTextureExportNode_ != child->isTextureExportNode_) {
        child->SyncTextureExport(isTextureExportNode_);
    }
    child->OnAddChildren();
    child->MarkDirty(NodeDirtyType::APPEARANCE, true);
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(id_, childId, index);

    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    if (child->GetType() == RSUINodeType::SURFACE_NODE) {
        auto surfaceNode = RSBaseNode::ReinterpretCast<RSSurfaceNode>(child);
        ROSEN_LOGI("RSNode::AddChild, Id: %{public}" PRIu64 ", SurfaceNode:[Id: %{public}" PRIu64 ", name: %{public}s]",
            id_, childId, surfaceNode->GetName().c_str());
        RS_TRACE_NAME_FMT("RSNode::AddChild, Id: %" PRIu64 ", SurfaceNode:[Id: %" PRIu64 ", name: %s]",
            id_, childId, surfaceNode->GetName().c_str());
    }
    child->SetIsOnTheTree(isOnTheTree_);
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
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeMoveChild>(id_, childId, index);

    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    child->SetIsOnTheTree(isOnTheTree_);
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
    child->MarkDirty(NodeDirtyType::APPEARANCE, true);
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveChild>(id_, childId);
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);

    if (child->GetType() == RSUINodeType::SURFACE_NODE) {
        auto surfaceNode = RSBaseNode::ReinterpretCast<RSSurfaceNode>(child);
        ROSEN_LOGI("RSNode::RemoveChild, Id: %{public}" PRIu64 ", SurfaceNode:[Id: %{public}" PRIu64 ", "
            "name: %{public}s]", id_, childId, surfaceNode->GetName().c_str());
        RS_TRACE_NAME_FMT("RSNode::RemoveChild, Id: %" PRIu64 ", SurfaceNode:[Id: %" PRIu64 ", name: %s]",
            id_, childId, surfaceNode->GetName().c_str());
    }
    child->SetIsOnTheTree(false);
}

void RSNode::RemoveChildByNodeId(NodeId childId)
{
    SharedPtr childPtr = rsUIContext_.lock() ? rsUIContext_.lock()->GetNodeMap().GetNode(childId)
                                             : RSNodeMap::Instance().GetNode(childId);
    if (childPtr) {
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
    child->MarkDirty(NodeDirtyType::APPEARANCE, true);
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddCrossParentChild>(id_, childId, index);

    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    child->SetIsOnTheTree(isOnTheTree_);
}

void RSNode::RemoveCrossParentChild(SharedPtr child, NodeId newParentId)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
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
    child->MarkDirty(NodeDirtyType::APPEARANCE, true);

    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveCrossParentChild>(id_, childId, newParentId);
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
}

void RSNode::SetIsCrossNode(bool isCrossNode)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        ROSEN_LOGE("transactionProxy is null, SetIsCrossNode failed !");
        return;
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSBaseNodeSetIsCrossNode>(GetId(), isCrossNode);
    transactionProxy->AddCommand(command);
}

void RSNode::AddCrossScreenChild(SharedPtr child, int index, bool autoClearCloneNode)
{
    if (child == nullptr) {
        ROSEN_LOGE("RSNode::AddCrossScreenChild, child is nullptr");
        return;
    }
    if (!this->IsInstanceOf<RSDisplayNode>()) {
        ROSEN_LOGE("RSNode::AddCrossScreenChild, only displayNode support AddCrossScreenChild");
        return;
    }

    if (!child->IsInstanceOf<RSSurfaceNode>()) {
        ROSEN_LOGE("RSNode::AddCrossScreenChild, child shoult be RSSurfaceNode");
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    NodeId childId = child->GetHierarchyCommandNodeId();
    // Generate an id on the client and create a clone node on the server based on the id.
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddCrossScreenChild>(id_, childId,
        GenerateId(), index, autoClearCloneNode);
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
}

void RSNode::RemoveCrossScreenChild(SharedPtr child)
{
    if (child == nullptr) {
        ROSEN_LOGE("RSNode::RemoveCrossScreenChild, child is nullptr");
        return;
    }
    if (!this->IsInstanceOf<RSDisplayNode>()) {
        ROSEN_LOGE("RSNode::RemoveCrossScreenChild, only displayNode support RemoveCrossScreenChild");
        return;
    }

    if (!child->IsInstanceOf<RSSurfaceNode>()) {
        ROSEN_LOGE("RSNode::RemoveCrossScreenChild, child shoult be RSSurfaceNode");
        return;
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    NodeId childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveCrossScreenChild>(id_, childId);
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
}

void RSNode::RemoveChildById(NodeId childId)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto itr = std::find(children_.begin(), children_.end(), childId);
    if (itr != children_.end()) {
        children_.erase(itr);
    }
}

void RSNode::RemoveFromTree()
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    MarkDirty(NodeDirtyType::APPEARANCE, true);
    auto parentPtr = rsUIContext_.lock() ? rsUIContext_.lock()->GetNodeMap().GetNode(parent_)
                                         : RSNodeMap::Instance().GetNode(parent_);
    if (parentPtr) {
        parentPtr->RemoveChildById(GetId());
        OnRemoveChildren();
        SetParent(0);
    }
    // construct command using own GetHierarchyCommandNodeId(), not GetId()
    auto nodeId = GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveFromTree>(nodeId);
    // always send Remove-From-Tree command
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), nodeId);
    SetIsOnTheTree(false);
}

void RSNode::ClearChildren()
{
    for (auto child : children_) {
        auto childPtr = rsUIContext_.lock() ? rsUIContext_.lock()->GetNodeMap().GetNode(child)
                                            : RSNodeMap::Instance().GetNode(child);
        if (childPtr) {
            childPtr->SetIsOnTheTree(false);
            childPtr->SetParent(0);
            childPtr->MarkDirty(NodeDirtyType::APPEARANCE, true);
        }
    }
    children_.clear();
    // construct command using own GetHierarchyCommandNodeId(), not GetId()
    auto nodeId = GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(nodeId);
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), nodeId);
}

void RSNode::SetExportTypeChangedCallback(ExportTypeChangedCallback callback)
{
    exportTypeChangedCallback_ = callback;
}

void RSNode::SetTextureExport(bool isTextureExportNode)
{
    if (isTextureExportNode == isTextureExportNode_) {
        return;
    }
    isTextureExportNode_ = isTextureExportNode;
    if (!IsUniRenderEnabled()) {
        return;
    }
    if (exportTypeChangedCallback_) {
        exportTypeChangedCallback_(isTextureExportNode);
    }
    if ((isTextureExportNode_ && !hasCreateRenderNodeInRT_) ||
        (!isTextureExportNode_ && !hasCreateRenderNodeInRS_)) {
        CreateRenderNodeForTextureExportSwitch();
    }
    DoFlushModifier();
}

void RSNode::SyncTextureExport(bool isTextureExportNode)
{
    if (isTextureExportNode == isTextureExportNode_) {
        return;
    }
    SetTextureExport(isTextureExportNode);
    for (uint32_t index = 0; index < children_.size(); index++) {
        auto childPtr = rsUIContext_.lock() ? rsUIContext_.lock()->GetNodeMap().GetNode(children_[index])
                                            : RSNodeMap::Instance().GetNode(children_[index]);
        if (childPtr) {
            childPtr->SyncTextureExport(isTextureExportNode);
            std::unique_ptr<RSCommand> command =
                std::make_unique<RSBaseNodeAddChild>(id_, childPtr->GetHierarchyCommandNodeId(), index);
            AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
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
    return rsUIContext_.lock() ? rsUIContext_.lock()->GetNodeMap().GetNode(parent_)
                               : RSNodeMap::Instance().GetNode(parent_);
}

void RSNode::DumpTree(int depth, std::string& out) const
{
    for (int i = 0; i < depth; i++) {
        out += "  ";
    }
    out += "| ";
    Dump(out);
    for (auto childId : children_) {
        auto child = rsUIContext_.lock() ? rsUIContext_.lock()->GetNodeMap().GetNode(childId)
                                         : RSNodeMap::Instance().GetNode(childId);
        if (child) {
            out += "\n";
            child->DumpTree(depth + 1, out);
        }
    }
}

void RSNode::Dump(std::string& out) const
{
    auto iter = RSUINodeTypeStrs.find(GetType());
    out += (iter != RSUINodeTypeStrs.end() ? iter->second : "RSNode");
    out += "[" + std::to_string(id_);
    out += "], parent[" + std::to_string(parent_);
    out += "], instanceId[" + std::to_string(instanceId_);
    if (auto node = ReinterpretCastTo<RSSurfaceNode>()) {
        out += "], name[" + node->GetName();
    } else if (!nodeName_.empty()) {
        out += "], nodeName[" + nodeName_;
    }
    out += "], frameNodeId[" + std::to_string(frameNodeId_);
    out += "], frameNodeTag[" + frameNodeTag_;
    out += "], extendModifierIsDirty[";
    out += extendModifierIsDirty_ ? "true" : "false";
    out += "], isNodeGroup[";
    out += isNodeGroup_ ? "true" : "false";
    out += "], isSingleFrameComposer[";
    out += isNodeSingleFrameComposer_ ? "true" : "false";
    out += "], isSuggestOpincNode[";
    out += isSuggestOpincNode_ ? "true" : "false";
    out += "], isUifirstNode[";
    out += isUifirstNode_ ? "true" : "false";
    out += "], drawRegion[";
    if (drawRegion_) {
        out += "x:" + std::to_string(drawRegion_->GetLeft());
        out += " y:" + std::to_string(drawRegion_->GetTop());
        out += " width:" + std::to_string(drawRegion_->GetWidth());
        out += " height:" + std::to_string(drawRegion_->GetHeight());
    } else {
        out += "null";
    }
    out += "], outOfParent[" + std::to_string(static_cast<int>(outOfParent_));
#ifdef RS_ENABLE_VK
    out += "], hybridRenderCanvas[";
    out += hybridRenderCanvas_ ? "true" : "false";
#endif
    out += "], animations[";
    for (const auto& [id, anim] : animations_) {
        out += "{id:" + std::to_string(id);
        out += " propId:" + std::to_string(anim->GetPropertyId());
        out += "} ";
    }
    if (!animations_.empty()) {
        out.pop_back();
    }
    out += "]";
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
    for (const auto& [animationId, animation] : animations_) {
        if (animation) {
            ss << " animationInfo:" << animation->DumpAnimation();
        }
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
template bool RSNode::IsInstanceOf<RSEffectNode>() const;

void RSNode::SetInstanceId(int32_t instanceId)
{
    instanceId_ = instanceId;
    auto rsUIContext = rsUIContext_.lock();
    // use client multi don’t need
    if (rsUIContext == nullptr) {
        RSNodeMap::MutableInstance().RegisterNodeInstanceId(id_, instanceId_);
    }
}

bool RSNode::AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand,
    FollowType followType, NodeId nodeId) const
{
    auto transaction = GetRSTransaction();
    if (transaction != nullptr) {
        transaction->AddCommand(command, isRenderServiceCommand, followType, nodeId);
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (!transactionProxy) {
            RS_LOGE("transactionProxy is nullptr");
            return false;
        }
        transactionProxy->AddCommand(command, isRenderServiceCommand, followType, nodeId);
    }
    return true;
}

DrawNodeChangeCallback RSNode::drawNodeChangeCallback_ = nullptr;
void RSNode::SetDrawNodeChangeCallback(DrawNodeChangeCallback callback)
{
    if (drawNodeChangeCallback_) {
        return;
    }
    drawNodeChangeCallback_ = callback;
}

} // namespace Rosen
} // namespace OHOS
