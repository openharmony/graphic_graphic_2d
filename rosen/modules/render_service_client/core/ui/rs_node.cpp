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
#include <vector>

#include "feature/hyper_graphic_manager/rs_frame_rate_policy.h"
#include "rs_trace.h"
#include "sandbox_utils.h"
#include "ui_effect/mask/include/ripple_mask_para.h"
#include "ui_effect/property/include/rs_ui_bezier_warp_filter.h"
#include "ui_effect/property/include/rs_ui_content_light_filter.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/property/include/rs_ui_dispersion_filter.h"
#include "ui_effect/property/include/rs_ui_displacement_distort_filter.h"
#include "ui_effect/property/include/rs_ui_edge_light_filter.h"
#include "ui_effect/property/include/rs_ui_filter.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

#include "animation/rs_animation.h"
#include "animation/rs_animation_callback.h"
#include "animation/rs_animation_group.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "animation/rs_render_particle_animation.h"
#include "command/rs_base_node_command.h"
#include "command/rs_node_command.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "common/rs_vector4.h"
#include "feature/composite_layer/rs_composite_layer_utils.h"
#include "modifier/rs_modifier_manager_map.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"
#include "modifier_ng/appearance/rs_alpha_modifier.h"
#include "modifier_ng/appearance/rs_background_filter_modifier.h"
#include "modifier_ng/appearance/rs_blend_modifier.h"
#include "modifier_ng/appearance/rs_border_modifier.h"
#include "modifier_ng/appearance/rs_compositing_filter_modifier.h"
#include "modifier_ng/appearance/rs_dynamic_light_up_modifier.h"
#include "modifier_ng/appearance/rs_foreground_filter_modifier.h"
#include "modifier_ng/appearance/rs_hdr_brightness_modifier.h"
#include "modifier_ng/appearance/rs_mask_modifier.h"
#include "modifier_ng/appearance/rs_outline_modifier.h"
#include "modifier_ng/appearance/rs_particle_effect_modifier.h"
#include "modifier_ng/appearance/rs_pixel_stretch_modifier.h"
#include "modifier_ng/appearance/rs_point_light_modifier.h"
#include "modifier_ng/appearance/rs_shadow_modifier.h"
#include "modifier_ng/appearance/rs_use_effect_modifier.h"
#include "modifier_ng/appearance/rs_visibility_modifier.h"
#include "modifier_ng/background/rs_background_color_modifier.h"
#include "modifier_ng/background/rs_background_image_modifier.h"
#include "modifier_ng/background/rs_background_ng_shader_modifier.h"
#include "modifier_ng/background/rs_background_shader_modifier.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "modifier_ng/foreground/rs_env_foreground_color_modifier.h"
#include "modifier_ng/foreground/rs_foreground_color_modifier.h"
#include "modifier_ng/foreground/rs_foreground_shader_modifier.h"
#include "modifier_ng/geometry/rs_bounds_clip_modifier.h"
#include "modifier_ng/geometry/rs_bounds_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_modifier.h"
#include "modifier_ng/geometry/rs_frame_modifier.h"
#include "modifier_ng/geometry/rs_transform_modifier.h"
#include "modifier_ng/rs_modifier_ng.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "render/rs_blur_filter.h"
#include "render/rs_border_light_shader.h"
#include "render/rs_filter.h"
#include "render/rs_material_filter.h"
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
#include "ui/rs_ui_patten_vec.h"

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
#include <sys/syscall.h>
#include <sys/types.h>
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

#ifndef MODIFIER_NG
bool IsPathAnimatableModifier(const RSModifierType& type)
{
    if (type == RSModifierType::BOUNDS || type == RSModifierType::FRAME || type == RSModifierType::TRANSLATE) {
        return true;
    }
    return false;
}
#endif

enum HdrEffectType : uint32_t {
    HDR_EFFECT_NONE = 0,
    HDR_EFFECT_FILTER = 1,
    HDR_EFFECT_BRIGHTNESS_BLENDER = 2,
};
} // namespace

RSNode::RSNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext,
    bool isOnTheTree)
    : isRenderServiceNode_(isRenderServiceNode), isTextureExportNode_(isTextureExportNode), id_(id),
      rsUIContext_(rsUIContext), stagingPropertiesExtractor_(id, rsUIContext),
      showingPropertiesFreezer_(id, rsUIContext), isOnTheTree_(isOnTheTree)
{
    InitUniRenderEnabled();
    if (auto rsUIContextPtr = rsUIContext_.lock()) {
        auto transaction = rsUIContextPtr->GetRSTransaction();
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
    if (RSSystemProperties::GetHybridRenderEnabled()) {
        RSModifiersDraw::EraseDrawRegions(id_);
    }
#endif

    // break current (ui) parent-child relationship.
    // render nodes will check if its child is expired and remove it, no need to manually remove it here.
    SharedPtr parentPtr = parent_.lock();
    auto rsUIContext = rsUIContext_.lock();
    if (rsUIContext != nullptr) {
        // tell RT/RS to destroy related render node
        rsUIContext->GetMutableNodeMap().UnregisterNode(id_);
        auto transaction = rsUIContext->GetRSTransaction();
        if (transaction == nullptr || skipDestroyCommandInDestructor_) {
            RS_LOGD("RSNode::~RSNode stop destroy proxy:%{public}d, skip: %{public}d", transaction == nullptr,
                skipDestroyCommandInDestructor_);
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
        RSNodeMap::MutableInstance().UnregisterNode(id_);
        // tell RT/RS to destroy related render node
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr || skipDestroyCommandInDestructor_) {
            RS_LOGD("RSNode::~RSNode stop destroy proxy:%{public}d, skip: %{public}d", transactionProxy == nullptr,
                skipDestroyCommandInDestructor_);
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
        parentPtr->children_.erase(std::remove_if(parentPtr->children_.begin(),
                                                  parentPtr->children_.end(),
                                                  [](const auto& child) { return child.expired(); }),
                                   parentPtr->children_.end());
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

    return implicitAnimator->CloseImplicitCancelAnimation() == CancelAnimationStatus::SUCCESS ? true : false;
}

bool RSNode::CloseImplicitCancelAnimation(const std::shared_ptr<RSUIContext> rsUIContext)
{
    auto implicitAnimator =
        rsUIContext ? rsUIContext->GetRSImplicitAnimator() : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("multi-instance Failed to close implicit animation for cancel, implicit animator is null!");
        return false;
    }

    return implicitAnimator->CloseImplicitCancelAnimation() == CancelAnimationStatus::SUCCESS ? true : false;
}

CancelAnimationStatus RSNode::CloseImplicitCancelAnimationReturnStatus(const std::shared_ptr<RSUIContext> rsUIContext)
{
    auto implicitAnimator =
        rsUIContext ? rsUIContext->GetRSImplicitAnimator() : RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("multi-instance Failed to close implicit animation for cancel, implicit animator is null!");
        return CancelAnimationStatus::NULL_ANIMATOR;
    }

    return implicitAnimator->CloseImplicitCancelAnimation();
}

void RSNode::SetFrameNodeInfo(int32_t id, std::string tag)
{
    frameNodeId_ = id;
    frameNodeTag_ = tag;
    MarkRepaintBoundary(tag);
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
    SetDrawNodeType(DrawNodeType::DrawPropertyType);
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier, &ModifierNG::RSBackgroundFilterModifier::SetMagnifierParams>(
        para);
#else
    SetProperty<RSMagnifierParamsModifier, RSProperty<std::shared_ptr<RSMagnifierParams>>>(
        RSModifierType::MAGNIFIER_PARA, para);
#endif
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
#if defined(MODIFIER_NG)
    for (const auto& [_, modifierNG] : modifiersNG_) {
        if (modifierNG->GetType() == ModifierNG::RSModifierType::BOUNDS) {
            auto boundsModifierNG = std::static_pointer_cast<ModifierNG::RSBoundsModifier>(modifierNG);
            boundsModifierNG->ApplyGeometry(localGeometry_);
        }
        if (modifierNG->GetType() == ModifierNG::RSModifierType::TRANSFORM) {
            auto transformModifierNG = std::static_pointer_cast<ModifierNG::RSTransformModifier>(modifierNG);
            transformModifierNG->ApplyGeometry(localGeometry_);
        }
    }
#else
    for (const auto& [_, modifier] : modifiers_) {
        if (modifier->GetPropertyModifierType() == RSPropertyModifierType::GEOMETRY) {
            modifier->Apply(localGeometry_);
        }
    }
#endif
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

bool RSNode::isNeedCallbackNodeChange_ = true;
void RSNode::SetNeedCallbackNodeChange(bool needCallback)
{
    isNeedCallbackNodeChange_ = needCallback;
}

// Notifies UI observer about page node modifications.
void RSNode::NotifyPageNodeChanged()
{
    if (isNeedCallbackNodeChange_ && propertyNodeChangeCallback_) {
        propertyNodeChangeCallback_();
    }
}

#if defined(MODIFIER_NG)
template<typename ModifierType, auto Setter, typename T>
void RSNode::SetPropertyNG(T value)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    auto type = static_cast<uint16_t>(ModifierType::Type);
    auto& modifier = modifiersNGCreatedBySetter_[type];
    // Create corresponding modifier if not exist
    if (modifier == nullptr) {
        modifier = std::make_shared<ModifierType>();
        (*std::static_pointer_cast<ModifierType>(modifier).*Setter)(value);
        modifiersNGCreatedBySetter_[type] = modifier;
        AddModifier(modifier);
    } else {
        (*std::static_pointer_cast<ModifierType>(modifier).*Setter)(value);
        NotifyPageNodeChanged();
    }
}

template<typename ModifierType, auto Setter, typename T>
void RSNode::SetPropertyNG(T value, bool animatable)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    auto type = static_cast<uint16_t>(ModifierType::Type);
    auto& modifier = modifiersNGCreatedBySetter_[type];
    // Create corresponding modifier if not exist
    if (modifier == nullptr) {
        modifier = std::make_shared<ModifierType>();
        (*std::static_pointer_cast<ModifierType>(modifier).*Setter)(value, animatable);
        modifiersNGCreatedBySetter_[type] = modifier;
        AddModifier(modifier);
    } else {
        (*std::static_pointer_cast<ModifierType>(modifier).*Setter)(value, animatable);
    }
}

template<typename ModifierType, auto Setter, typename T>
void RSNode::SetUIFilterPropertyNG(T value)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    auto type = static_cast<uint16_t>(ModifierType::Type);
    auto& modifier = modifiersNGCreatedBySetter_[type];
    // Create corresponding modifier if not exist
    if (modifier == nullptr) {
        modifier = std::make_shared<ModifierType>();
        modifiersNGCreatedBySetter_[type] = modifier;
        AddModifier(modifier);
    }
    (*std::static_pointer_cast<ModifierType>(modifier).*Setter)(value);
}
#else
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
        NotifyPageNodeChanged();
        return;
    }
    auto property = std::make_shared<PropertyName>(value);
    auto propertyModifier = std::make_shared<ModifierName>(property);
    propertyModifiers_.emplace(modifierType, propertyModifier);
    AddModifier(propertyModifier);
}
#endif

// alpha
void RSNode::SetAlpha(float alpha)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSAlphaModifier, &ModifierNG::RSAlphaModifier::SetAlpha>(alpha);
#else
    SetProperty<RSAlphaModifier, RSAnimatableProperty<float>>(RSModifierType::ALPHA, alpha);
#endif
    if (alpha < 1) {
        SetDrawNode();
        SetDrawNodeType(DrawNodeType::DrawPropertyType);
    }
}

void RSNode::SetAlphaOffscreen(bool alphaOffscreen)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSAlphaModifier, &ModifierNG::RSAlphaModifier::SetAlphaOffscreen>(alphaOffscreen);
#else
    SetProperty<RSAlphaOffscreenModifier, RSProperty<bool>>(RSModifierType::ALPHA_OFFSCREEN, alphaOffscreen);
#endif
}

// Bounds
void RSNode::SetBounds(const Vector4f& bounds)
{
    if (auto surfaceNode = ReinterpretCastTo<RSSurfaceNode>()) {
        auto compositeLayerUtils = surfaceNode->GetCompositeLayerUtils();
        if (compositeLayerUtils) {
            compositeLayerUtils->UpdateVirtualNodeBounds(bounds);
        }
    }
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBoundsModifier, &ModifierNG::RSBoundsModifier::SetBounds>(bounds);
#else
    SetProperty<RSBoundsModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::BOUNDS, bounds);
#endif
    OnBoundsSizeChanged();
    if (bounds.x_ != 0 || bounds.y_ != 0) {
        SetDrawNode();
        SetDrawNodeType(DrawNodeType::MergeableType);
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
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::BOUNDS)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::BOUNDS)) {
            SetBounds(0.f, 0.f, width, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::BOUNDS));
#else
        auto iter = propertyModifiers_.find(RSModifierType::BOUNDS);
        if (iter == propertyModifiers_.end()) {
            SetBounds(0.f, 0.f, width, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
#endif
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
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::BOUNDS)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::BOUNDS)) {
            SetBounds(0.f, 0.f, 0.f, height);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::BOUNDS));
#else
        auto iter = propertyModifiers_.find(RSModifierType::BOUNDS);
        if (iter == propertyModifiers_.end()) {
            SetBounds(0.f, 0.f, 0.f, height);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSFrameModifier, &ModifierNG::RSFrameModifier::SetFrame>(bounds);
#else
    SetProperty<RSFrameModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::FRAME, bounds);
#endif
    if (bounds.x_ != 0 || bounds.y_ != 0) {
        SetDrawNode();
        SetDrawNodeType(DrawNodeType::MergeableType);
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
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::FRAME)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::FRAME)) {
            SetFrame(positionX, 0.f, 0.f, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::FRAME));
#else
        auto iter = propertyModifiers_.find(RSModifierType::FRAME);
        if (iter == propertyModifiers_.end()) {
            SetFrame(positionX, 0.f, 0.f, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
#endif
    }

    if (property == nullptr) {
        return;
    }
    auto frame = property->Get();
    frame.x_ = positionX;
    property->Set(frame);
    SetDrawNode();
    SetDrawNodeType(DrawNodeType::MergeableType);
}

void RSNode::SetFramePositionY(float positionY)
{
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::FRAME)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::FRAME)) {
            SetFrame(0.f, positionY, 0.f, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::FRAME));
#else
        auto iter = propertyModifiers_.find(RSModifierType::FRAME);
        if (iter == propertyModifiers_.end()) {
            SetFrame(0.f, positionY, 0.f, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
#endif
    }

    if (property == nullptr) {
        return;
    }
    auto frame = property->Get();
    frame.y_ = positionY;
    property->Set(frame);
    SetDrawNode();
    SetDrawNodeType(DrawNodeType::MergeableType);
}

void RSNode::SetSandBox(std::optional<Vector2f> parentPosition)
{
    if (!parentPosition.has_value()) {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier != nullptr) {
            modifier->DetachProperty(ModifierNG::RSPropertyType::SANDBOX);
        }
        return;
    }
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetSandBox>(
        parentPosition.value());
#else
        auto iter = propertyModifiers_.find(RSModifierType::SANDBOX);
        if (iter != propertyModifiers_.end()) {
            RemoveModifier(iter->second);
            propertyModifiers_.erase(iter);
        }
        return;
    }
    SetProperty<RSSandBoxModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::SANDBOX, parentPosition.value());
#endif
}

void RSNode::SetPositionZ(float positionZ)
{
    if (drawNodeChangeCallback_) {
        drawNodeChangeCallback_(shared_from_this(), true);
    }
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetPositionZ>(positionZ);
#else
    SetProperty<RSPositionZModifier, RSAnimatableProperty<float>>(RSModifierType::POSITION_Z, positionZ);
#endif
}

void RSNode::SetPositionZApplicableCamera3D(bool isApplicable)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetPositionZApplicableCamera3D>(
        isApplicable);
#else
    SetProperty<RSPositionZApplicableCamera3DModifier, RSProperty<bool>>(
        RSModifierType::POSITION_Z_APPLICABLE_CAMERA3D, isApplicable);
#endif
}

// pivot
void RSNode::SetPivot(const Vector2f& pivot)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetPivot>(pivot, true);
#else
    SetProperty<RSPivotModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::PIVOT, pivot);
#endif
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
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::PIVOT)) {
            SetPivot(pivotX, 0.5f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::PIVOT));
#else
        auto iter = propertyModifiers_.find(RSModifierType::PIVOT);
        if (iter == propertyModifiers_.end()) {
            SetPivot(pivotX, 0.5f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
#endif
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
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::PIVOT)) {
            SetPivot(0.5f, pivotY);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::PIVOT));
#else
        auto iter = propertyModifiers_.find(RSModifierType::PIVOT);
        if (iter == propertyModifiers_.end()) {
            SetPivot(0.5f, pivotY);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetPivotZ>(pivotZ);
#else
    SetProperty<RSPivotZModifier, RSAnimatableProperty<float>>(RSModifierType::PIVOT_Z, pivotZ);
#endif
}

void RSNode::SetCornerRadius(float cornerRadius)
{
    SetCornerRadius(Vector4f(cornerRadius));
}

void RSNode::SetCornerRadius(const Vector4f& cornerRadius)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBoundsClipModifier, &ModifierNG::RSBoundsClipModifier::SetCornerRadius>(cornerRadius);
#else
    SetProperty<RSCornerRadiusModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::CORNER_RADIUS, cornerRadius);
#endif
}

// transform
void RSNode::SetRotation(const Quaternion& quaternion)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetQuaternion>(quaternion);
#else
    SetProperty<RSQuaternionModifier, RSAnimatableProperty<Quaternion>>(RSModifierType::QUATERNION, quaternion);
#endif
}

void RSNode::SetRotation(float degree)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetRotation>(degree);
#else
    SetProperty<RSRotationModifier, RSAnimatableProperty<float>>(RSModifierType::ROTATION, degree);
#endif
}

void RSNode::SetRotation(float degreeX, float degreeY, float degreeZ)
{
    SetRotationX(degreeX);
    SetRotationY(degreeY);
    SetRotation(degreeZ);
}

void RSNode::SetRotationX(float degree)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetRotationX>(degree);
#else
    SetProperty<RSRotationXModifier, RSAnimatableProperty<float>>(RSModifierType::ROTATION_X, degree);
#endif
}

void RSNode::SetRotationY(float degree)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetRotationY>(degree);
#else
    SetProperty<RSRotationYModifier, RSAnimatableProperty<float>>(RSModifierType::ROTATION_Y, degree);
#endif
}

void RSNode::SetCameraDistance(float cameraDistance)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetCameraDistance>(cameraDistance);
#else
    SetProperty<RSCameraDistanceModifier, RSAnimatableProperty<float>>(RSModifierType::CAMERA_DISTANCE, cameraDistance);
#endif
}

void RSNode::SetTranslate(const Vector2f& translate)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetTranslate>(translate);
#else
    SetProperty<RSTranslateModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::TRANSLATE, translate);
#endif
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
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::TRANSLATE)) {
            SetTranslate({ translate, 0.f });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::TRANSLATE));
#else
        auto iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
        if (iter == propertyModifiers_.end()) {
            SetTranslate({ translate, 0.f });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
#endif
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
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::TRANSLATE)) {
            SetTranslate({ 0.f, translate });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::TRANSLATE));
#else
        auto iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
        if (iter == propertyModifiers_.end()) {
            SetTranslate({ 0.f, translate });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetTranslateZ>(translate);
#else
    SetProperty<RSTranslateZModifier, RSAnimatableProperty<float>>(RSModifierType::TRANSLATE_Z, translate);
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetScale>(scale);
#else
    SetProperty<RSScaleModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::SCALE, scale);
#endif
}

void RSNode::SetScaleX(float scaleX)
{
    std::shared_ptr<RSAnimatableProperty<Vector2f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::SCALE)) {
            SetScale(scaleX, 1.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::SCALE));
#else
        auto iter = propertyModifiers_.find(RSModifierType::SCALE);
        if (iter == propertyModifiers_.end()) {
            SetScale(scaleX, 1.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
#endif
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
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::SCALE)) {
            SetScale(1.f, scaleY);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::SCALE));
#else
        auto iter = propertyModifiers_.find(RSModifierType::SCALE);
        if (iter == propertyModifiers_.end()) {
            SetScale(1.f, scaleY);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
#endif
    }

    if (property == nullptr) {
        return;
    }
    auto scale = property->Get();
    scale.y_ = scaleY;
    property->Set(scale);
}

void RSNode::SetScaleZ(float scaleZ)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetScaleZ>(scaleZ);
#else
    SetProperty<RSScaleZModifier, RSAnimatableProperty<float>>(RSModifierType::SCALE_Z, scaleZ);
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetSkew>(skew);
#else
    SetProperty<RSSkewModifier, RSAnimatableProperty<Vector3f>>(RSModifierType::SKEW, skew);
#endif
}

void RSNode::SetSkewX(float skewX)
{
    std::shared_ptr<RSAnimatableProperty<Vector3f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::SKEW)) {
            SetSkew(skewX, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::SKEW));
#else
        auto iter = propertyModifiers_.find(RSModifierType::SKEW);
        if (iter == propertyModifiers_.end()) {
            SetSkew(skewX, 0.f);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(iter->second->GetProperty());
#endif
    }

    if (property == nullptr) {
        return;
    }
    auto skew = property->Get();
    skew.x_ = skewX;
    property->Set(skew);
}

void RSNode::SetSkewY(float skewY)
{
    std::shared_ptr<RSAnimatableProperty<Vector3f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::SKEW)) {
            SetSkew(0.f, skewY);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::SKEW));
#else
        auto iter = propertyModifiers_.find(RSModifierType::SKEW);
        if (iter == propertyModifiers_.end()) {
            SetSkew(0.f, skewY);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(iter->second->GetProperty());
#endif
    }
    if (property == nullptr) {
        return;
    }
    auto skew = property->Get();
    skew.y_ = skewY;
    property->Set(skew);
}

void RSNode::SetSkewZ(float skewZ)
{
    std::shared_ptr<RSAnimatableProperty<Vector3f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::SKEW)) {
            SetSkew(0.f, 0.f, skewZ);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::SKEW));
#else
        auto iter = propertyModifiers_.find(RSModifierType::SKEW);
        if (iter == propertyModifiers_.end()) {
            SetSkew(0.f, 0.f, skewZ);
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector3f>>(iter->second->GetProperty());
#endif
    }

    if (property == nullptr) {
        return;
    }
    auto skew = property->Get();
    skew.z_ = skewZ;
    property->Set(skew);
}

void RSNode::SetRSUIContext(std::shared_ptr<RSUIContext> rsUIContext)
{
    if (rsUIContext == nullptr) {
        return;
    }
    auto preUIContext = rsUIContext_.lock();
    if ((preUIContext != nullptr) && (preUIContext == rsUIContext)) {
        return;
    }

    // if have old rsContext, should remove nodeId from old nodeMap and travel child
    if (preUIContext != nullptr) {
        // step1 remove node from old context
        preUIContext->GetMutableNodeMap().UnregisterNode(id_);
        // sync child
        for (uint32_t index = 0; index < children_.size(); index++) {
            if (auto childPtr = children_[index].lock()) {
                childPtr->SetRSUIContext(rsUIContext);
            }
        }
    }

    RSModifierExtractor rsModifierExtractor(id_, rsUIContext);
    stagingPropertiesExtractor_ = rsModifierExtractor;
    RSShowingPropertiesFreezer showingPropertiesFreezer(id_, rsUIContext);
    showingPropertiesFreezer_ = showingPropertiesFreezer;

    // step2 sign
    rsUIContext_ = rsUIContext;
    // step3 register node to new nodeMap and move the command to the new RSUIContext
    RegisterNodeMap();
    if (preUIContext != nullptr) {
        auto preTransaction = preUIContext->GetRSTransaction();
        auto curTransaction = rsUIContext->GetRSTransaction();
        if (preTransaction && curTransaction) {
            preTransaction->MoveCommandByNodeId(curTransaction, id_);
        }
    }
    SetUIContextToken();
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetPersp>(persp);
#else
    SetProperty<RSPerspModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::PERSP, persp);
#endif
}

void RSNode::SetPerspX(float perspX)
{
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::PERSP)) {
            SetPersp({ perspX, 0.f, 0.0f, 1.0f });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::PERSP));
#else
        auto iter = propertyModifiers_.find(RSModifierType::PERSP);
        if (iter == propertyModifiers_.end()) {
            SetPersp({ perspX, 0.f, 0.0f, 1.0f });
            return;
        }

        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
#endif
    }

    if (property == nullptr) {
        return;
    }
    auto persp = property->Get();
    persp.x_ = perspX;
    property->Set(persp);
}

void RSNode::SetPerspY(float perspY)
{
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::PERSP)) {
            SetPersp({ 0.f, perspY, 0.f, 1.f });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::PERSP));
#else
        auto iter = propertyModifiers_.find(RSModifierType::PERSP);
        if (iter == propertyModifiers_.end()) {
            SetPersp({ 0.f, perspY, 0.f, 1.f });
            return;
        }

        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
#endif
    }
    if (property == nullptr) {
        return;
    }
    auto persp = property->Get();
    persp.y_ = perspY;
    property->Set(persp);
}

void RSNode::SetPerspZ(float perspZ)
{
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::PERSP)) {
            SetPersp({ 0.f, 0.f, perspZ, 1.f });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::PERSP));
#else
        auto iter = propertyModifiers_.find(RSModifierType::PERSP);
        if (iter == propertyModifiers_.end()) {
            SetPersp({ 0.f, 0.f, perspZ, 1.f });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
#endif
    }
    if (property == nullptr) {
        return;
    }
    auto persp = property->Get();
    persp.z_ = perspZ;
    property->Set(persp);
}

void RSNode::SetPerspW(float perspW)
{
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
        auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::PERSP)) {
            SetPersp({ 0.f, 0.f, 0.f, perspW });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::PERSP));
#else
        auto iter = propertyModifiers_.find(RSModifierType::PERSP);
        if (iter == propertyModifiers_.end()) {
            SetPersp({ 0.f, 0.f, 0.f, perspW });
            return;
        }
        property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
#endif
    }
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSEnvForegroundColorModifier,
        &ModifierNG::RSEnvForegroundColorModifier::SetEnvForegroundColor>(color);
#else
    SetProperty<RSEnvForegroundColorModifier, RSAnimatableProperty<Color>>(RSModifierType::ENV_FOREGROUND_COLOR, color);
#endif
}

// Set the foreground color strategy of the control
void RSNode::SetEnvForegroundColorStrategy(ForegroundColorStrategyType strategyType)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSEnvForegroundColorModifier,
        &ModifierNG::RSEnvForegroundColorModifier::SetEnvForegroundColorStrategy>(strategyType);
#else
    SetProperty<RSEnvForegroundColorStrategyModifier, RSProperty<ForegroundColorStrategyType>>(
        RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, strategyType);
#endif
}

// Set ParticleParams
void RSNode::SetParticleParams(std::vector<ParticleParams>& particleParams, const std::function<void()>& finishCallback)
{
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams;
    for (size_t i = 0; i < particleParams.size(); i++) {
        particlesRenderParams.push_back(particleParams[i].SetParamsToRenderParticle());
    }

    SetParticleDrawRegion(particleParams);
    auto property = std::make_shared<RSProperty<int>>();
    auto propertyId = property->GetId();
    auto uiAnimation = std::make_shared<RSAnimationGroup>();
    auto animationId = uiAnimation->GetId();
    AddAnimation(uiAnimation);
    if (finishCallback != nullptr) {
        uiAnimation->SetFinishCallback(std::make_shared<AnimationFinishCallback>(finishCallback));
    }
    auto animation =
        std::make_shared<RSRenderParticleAnimation>(animationId, propertyId, std::move(particlesRenderParams));
#if defined(MODIFIER_NG)
    ModifierId modifierId = ModifierNG::RSModifier::GenerateModifierId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCreateParticleNG>(GetId(), modifierId, animation);
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
    if (NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> cmdForRemote =
            std::make_unique<RSAnimationCreateParticleNG>(GetId(), modifierId, animation);
        AddCommand(cmdForRemote, true, GetFollowType(), GetId());
#else
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCreateParticle>(GetId(), animation);
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
    if (NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> cmdForRemote = std::make_unique<RSAnimationCreateParticle>(GetId(), animation);
        AddCommand(cmdForRemote, true, GetFollowType(), GetId());
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSParticleEffectModifier, &ModifierNG::RSParticleEffectModifier::SetEmitterUpdater>(para);
#else
    SetProperty<RSEmitterUpdaterModifier, RSProperty<std::vector<std::shared_ptr<EmitterUpdater>>>>(
        RSModifierType::PARTICLE_EMITTER_UPDATER, para);
#endif
}

// Set Particle Noise Field
void RSNode::SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSParticleEffectModifier, &ModifierNG::RSParticleEffectModifier::SetParticleNoiseFields>(
        para);
#else
    SetProperty<RSParticleNoiseFieldsModifier, RSProperty<std::shared_ptr<ParticleNoiseFields>>>(
        RSModifierType::PARTICLE_NOISE_FIELD, para);
#endif
}

// foreground
void RSNode::SetForegroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSForegroundColorModifier, &ModifierNG::RSForegroundColorModifier::SetForegroundColor>(
        color);
#else
    SetProperty<RSForegroundColorModifier, RSAnimatableProperty<Color>>(RSModifierType::FOREGROUND_COLOR, color);
#endif
}

void RSNode::SetBackgroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SetBackgroundColor(color);
}

void RSNode::SetBackgroundColor(RSColor& color)
{
    RSColor colorInP3 = color;
    colorInP3.ConvertToP3ColorSpace();
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundColorModifier, &ModifierNG::RSBackgroundColorModifier::SetBackgroundColor>(
        colorInP3);
#else
    SetProperty<RSBackgroundColorModifier, RSAnimatableProperty<Color>>(RSModifierType::BACKGROUND_COLOR, colorInP3);
#endif
    if (colorInP3.GetAlpha() > 0) {
        SetDrawNode();
        SetDrawNodeType(DrawNodeType::DrawPropertyType);
    }
}

void RSNode::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundShaderModifier, &ModifierNG::RSBackgroundShaderModifier::SetBackgroundShader>(
        shader);
#else
    SetProperty<RSBackgroundShaderModifier, RSProperty<std::shared_ptr<RSShader>>>(
        RSModifierType::BACKGROUND_SHADER, shader);
#endif
}

void RSNode::SetBackgroundShaderProgress(const float& progress)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundShaderModifier,
        &ModifierNG::RSBackgroundShaderModifier::SetBackgroundShaderProgress>(progress);
#else
    SetProperty<RSBackgroundShaderProgressModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_SHADER_PROGRESS, progress);
#endif
}

// background
void RSNode::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    if (image) {
        image->SetNodeId(GetId());
    }
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundImageModifier, &ModifierNG::RSBackgroundImageModifier::SetBgImage>(image);
#else
    SetProperty<RSBgImageModifier, RSProperty<std::shared_ptr<RSImage>>>(RSModifierType::BG_IMAGE, image);
#endif
}

void RSNode::SetBgImageInnerRect(const Vector4f& rect)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundImageModifier, &ModifierNG::RSBackgroundImageModifier::SetBgImageInnerRect>(
        rect);
#else
    SetProperty<RSBgImageInnerRectModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::BG_IMAGE_INNER_RECT, rect);
#endif
}

void RSNode::SetBgImageSize(float width, float height)
{
    SetBgImageWidth(width);
    SetBgImageHeight(height);
}

void RSNode::SetBgImageWidth(float width)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundImageModifier, &ModifierNG::RSBackgroundImageModifier::SetBgImageWidth>(
        width);
#else
    SetProperty<RSBgImageWidthModifier, RSAnimatableProperty<float>>(RSModifierType::BG_IMAGE_WIDTH, width);
#endif
}

void RSNode::SetBgImageHeight(float height)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundImageModifier, &ModifierNG::RSBackgroundImageModifier::SetBgImageHeight>(
        height);
#else
    SetProperty<RSBgImageHeightModifier, RSAnimatableProperty<float>>(RSModifierType::BG_IMAGE_HEIGHT, height);
#endif
}

void RSNode::SetBgImagePosition(float positionX, float positionY)
{
    SetBgImagePositionX(positionX);
    SetBgImagePositionY(positionY);
}

void RSNode::SetBgImagePositionX(float positionX)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundImageModifier, &ModifierNG::RSBackgroundImageModifier::SetBgImagePositionX>(
        positionX);
#else
    SetProperty<RSBgImagePositionXModifier, RSAnimatableProperty<float>>(
        RSModifierType::BG_IMAGE_POSITION_X, positionX);
#endif
}

void RSNode::SetBgImagePositionY(float positionY)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundImageModifier, &ModifierNG::RSBackgroundImageModifier::SetBgImagePositionY>(
        positionY);
#else
    SetProperty<RSBgImagePositionYModifier, RSAnimatableProperty<float>>(
        RSModifierType::BG_IMAGE_POSITION_Y, positionY);
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBorderModifier, &ModifierNG::RSBorderModifier::SetBorderColor>(color);
#else
    SetProperty<RSBorderColorModifier, RSAnimatableProperty<Vector4<Color>>>(RSModifierType::BORDER_COLOR, color);
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBorderModifier, &ModifierNG::RSBorderModifier::SetBorderWidth>(width);
#else
    SetProperty<RSBorderWidthModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::BORDER_WIDTH, width);
#endif
}

// set inner border style
void RSNode::SetBorderStyle(uint32_t styleValue)
{
    SetBorderStyle(styleValue, styleValue, styleValue, styleValue);
}

// set inner border style
void RSNode::SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
#if defined(MODIFIER_NG)
    Vector4<uint32_t> style(left, top, right, bottom);
    SetPropertyNG<ModifierNG::RSBorderModifier, &ModifierNG::RSBorderModifier::SetBorderStyle>(style);
#else
    Vector4<BorderStyle> style(static_cast<BorderStyle>(left), static_cast<BorderStyle>(top),
        static_cast<BorderStyle>(right), static_cast<BorderStyle>(bottom));
    SetBorderStyle(style);
#endif
}

// set inner border style
void RSNode::SetBorderStyle(const Vector4<BorderStyle>& style)
{
    Vector4<uint32_t> borderStyle(static_cast<uint32_t>(style.x_), static_cast<uint32_t>(style.y_),
        static_cast<uint32_t>(style.z_), static_cast<uint32_t>(style.w_));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBorderModifier, &ModifierNG::RSBorderModifier::SetBorderStyle>(borderStyle);
#else
    SetProperty<RSBorderStyleModifier, RSProperty<Vector4<uint32_t>>>(RSModifierType::BORDER_STYLE, borderStyle);
#endif
}

// set dash width for border
void RSNode::SetBorderDashWidth(const Vector4f& dashWidth)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBorderModifier, &ModifierNG::RSBorderModifier::SetBorderDashWidth>(dashWidth);
#else
    SetProperty<RSBorderDashWidthModifier, RSProperty<Vector4f>>(RSModifierType::BORDER_DASH_WIDTH, dashWidth);
#endif
}

// set dash gap for border
void RSNode::SetBorderDashGap(const Vector4f& dashGap)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBorderModifier, &ModifierNG::RSBorderModifier::SetBorderDashGap>(dashGap);
#else
    SetProperty<RSBorderDashGapModifier, RSProperty<Vector4f>>(RSModifierType::BORDER_DASH_GAP, dashGap);
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSOutlineModifier, &ModifierNG::RSOutlineModifier::SetOutlineColor>(color);
#else
    SetProperty<RSOutlineColorModifier, RSAnimatableProperty<Vector4<Color>>>(RSModifierType::OUTLINE_COLOR, color);
#endif
}

void RSNode::SetOutlineWidth(const Vector4f& width)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSOutlineModifier, &ModifierNG::RSOutlineModifier::SetOutlineWidth>(width);
#else
    SetProperty<RSOutlineWidthModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::OUTLINE_WIDTH, width);
#endif
}

void RSNode::SetOutlineStyle(const Vector4<BorderStyle>& style)
{
    Vector4<uint32_t> styles(static_cast<uint32_t>(style.x_), static_cast<uint32_t>(style.y_),
        static_cast<uint32_t>(style.z_), static_cast<uint32_t>(style.w_));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSOutlineModifier, &ModifierNG::RSOutlineModifier::SetOutlineStyle>(styles);
#else
    SetProperty<RSOutlineStyleModifier, RSProperty<Vector4<uint32_t>>>(RSModifierType::OUTLINE_STYLE, styles);
#endif
}

void RSNode::SetOutlineDashWidth(const Vector4f& dashWidth)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSOutlineModifier, &ModifierNG::RSOutlineModifier::SetOutlineDashWidth>(dashWidth);
#else
    SetProperty<RSOutlineDashWidthModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::OUTLINE_DASH_WIDTH, dashWidth);
#endif
}

void RSNode::SetOutlineDashGap(const Vector4f& dashGap)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSOutlineModifier, &ModifierNG::RSOutlineModifier::SetOutlineDashGap>(dashGap);
#else
    SetProperty<RSOutlineDashGapModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::OUTLINE_DASH_GAP, dashGap);
#endif
}

void RSNode::SetOutlineRadius(const Vector4f& radius)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSOutlineModifier, &ModifierNG::RSOutlineModifier::SetOutlineRadius>(radius);
#else
    SetProperty<RSOutlineRadiusModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::OUTLINE_RADIUS, radius);
#endif
}

void RSNode::SetUIBackgroundFilter(const OHOS::Rosen::Filter* backgroundFilter)
{
    if (backgroundFilter == nullptr) {
        ROSEN_LOGE("Failed to set backgroundFilter, backgroundFilter is null!");
        return;
    }
    // planning: remove RSUIFilter and generate composed filter as RSNGFilterBase
    std::shared_ptr<RSNGFilterBase> headFilter = nullptr;
    std::shared_ptr<RSUIFilter> uiFilter = std::make_shared<RSUIFilter>();
    auto filterParas = backgroundFilter->GetAllPara();
    for (auto it = filterParas.begin(); it != filterParas.end(); ++it) {
        auto filterPara = *it;
        if (filterPara == nullptr) {
            continue;
        }
        if (auto curFilter = RSNGFilterBase::Create(filterPara)) {
            if (headFilter) {
                headFilter->Append(curFilter);
            } else {
                headFilter = curFilter; // init headFilter
            }
            continue;
        }
        switch (filterPara->GetParaType()) {
            case FilterPara::BLUR : {
                auto filterBlurPara = std::static_pointer_cast<FilterBlurPara>(filterPara);
                auto blurRadius = filterBlurPara->GetRadius();
                SetBackgroundBlurRadiusX(blurRadius);
                SetBackgroundBlurRadiusY(blurRadius);
                break;
            }
            case FilterPara::WATER_RIPPLE : {
                auto waterRipplePara = std::static_pointer_cast<WaterRipplePara>(filterPara);
                auto waveCount = waterRipplePara->GetWaveCount();
                auto rippleCenterX = waterRipplePara->GetRippleCenterX();
                auto rippleCenterY = waterRipplePara->GetRippleCenterY();
                auto progress = waterRipplePara->GetProgress();
                auto rippleMode = waterRipplePara->GetRippleMode();
                RSWaterRipplePara params { waveCount, rippleCenterX, rippleCenterY, rippleMode };
                SetWaterRippleParams(params, progress);
                break;
            }
            case FilterPara::DISPLACEMENT_DISTORT : {
                auto distortProperty = std::make_shared<RSUIDispDistortFilterPara>();
                auto filterDistortPara = std::static_pointer_cast<DisplacementDistortPara>(filterPara);
                distortProperty->SetDisplacementDistort(filterDistortPara);
                uiFilter->Insert(distortProperty);
                break;
            }
            case FilterPara::COLOR_GRADIENT : {
                auto filterColorGradientPara = std::static_pointer_cast<ColorGradientPara>(filterPara);
                auto colorGradientProperty = std::make_shared<RSUIColorGradientFilterPara>();
                colorGradientProperty->SetColorGradient(filterColorGradientPara);
                uiFilter->Insert(colorGradientProperty);
                break;
            }
            case FilterPara::EDGE_LIGHT: {
                auto edgeLightProperty = std::make_shared<RSUIEdgeLightFilterPara>();
                auto filterEdgeLightPara = std::static_pointer_cast<EdgeLightPara>(filterPara);
                edgeLightProperty->SetEdgeLight(filterEdgeLightPara);
                uiFilter->Insert(edgeLightProperty);
                break;
            }
            case FilterPara::DISPERSION: {
                auto dispersionProperty = std::make_shared<RSUIDispersionFilterPara>();
                auto filterDispersionPara = std::static_pointer_cast<DispersionPara>(filterPara);
                dispersionProperty->SetDispersion(filterDispersionPara);
                uiFilter->Insert(dispersionProperty);
                break;
            }
            default:
                break;
        }
    }
    if (!uiFilter->GetAllTypes().empty()) {
        SetBackgroundUIFilter(uiFilter);
    }
    SetBackgroundNGFilter(headFilter);
}

void RSNode::SetBackgroundUIFilter(const std::shared_ptr<RSUIFilter> backgroundFilter)
{
    if (!backgroundFilter) {
        ROSEN_LOGE("RSNode::SetBackgroundUIFilter background RSUIFilter is nullptr");
        return;
    }

#if defined(MODIFIER_NG)
    SetEnableHDREffect(HdrEffectType::HDR_EFFECT_FILTER, backgroundFilter->GetHdrEffectEnable());
    SetUIFilterPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetUIFilter>(backgroundFilter);
#else
    bool shouldAdd = true;
    std::shared_ptr<RSUIFilter> oldProperty = nullptr;
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto iter = propertyModifiers_.find(RSModifierType::BACKGROUND_UI_FILTER);
    if (iter != propertyModifiers_.end() && iter->second != nullptr) {
        auto oldRsPro = std::static_pointer_cast<RSProperty<std::shared_ptr<RSUIFilter>>>(
            iter->second->GetProperty());
        if (oldRsPro) {
            oldProperty = oldRsPro->Get();
        }
        if (oldProperty && backgroundFilter->IsStructureSame(oldProperty)) {
            shouldAdd = false;
        }
    }
    SetEnableHDREffect(HdrEffectType::HDR_EFFECT_FILTER, backgroundFilter->GetHdrEffectEnable());

    if (shouldAdd) {
        auto rsProperty = std::make_shared<RSProperty<std::shared_ptr<RSUIFilter>>>(backgroundFilter);
        auto propertyModifier = std::make_shared<RSBackgroundUIFilterModifier>(rsProperty);
        propertyModifiers_.emplace(RSModifierType::BACKGROUND_UI_FILTER, propertyModifier);
        AddModifier(propertyModifier);
    } else {
        oldProperty->SetValue(backgroundFilter);
    }
#endif
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
    // To do: generate composed filter here. Now we just set foreground blur in v1.0.
    std::shared_ptr<RSNGFilterBase> headFilter = nullptr;
    std::shared_ptr<RSUIFilter> uiFilter = std::make_shared<RSUIFilter>();
    auto& filterParas = foregroundFilter->GetAllPara();
    for (const auto& filterPara : filterParas) {
        if (filterPara == nullptr) {
            continue;
        }
        if (auto curFilter = RSNGFilterBase::Create(filterPara)) {
            if (headFilter) {
                headFilter->Append(curFilter);
            } else {
                headFilter = curFilter; // init headFilter
            }
            continue;
        }
        if (filterPara->GetParaType() == FilterPara::BLUR) {
            auto filterBlurPara = std::static_pointer_cast<FilterBlurPara>(filterPara);
            auto blurRadius = filterBlurPara->GetRadius();
            SetForegroundEffectRadius(blurRadius);
        }
        if (filterPara->GetParaType() == FilterPara::FLY_OUT) {
            auto flyOutPara = std::static_pointer_cast<FlyOutPara>(filterPara);
            auto flyMode = flyOutPara->GetFlyMode();
            auto degree = flyOutPara->GetDegree();
            RSFlyOutPara rs_fly_out_param = { flyMode };
            SetFlyOutParams(rs_fly_out_param, degree);
        }
        if (filterPara->GetParaType() == FilterPara::DISTORT) {
            auto distortPara = std::static_pointer_cast<DistortPara>(filterPara);
            auto distortionK = distortPara->GetDistortionK();
            SetDistortionK(distortionK);
        }
        if (filterPara->GetParaType() == FilterPara::BEZIER_WARP) {
            auto bezierWarpProperty = std::make_shared<RSUIBezierWarpFilterPara>();
            auto bezierWarpPara = std::static_pointer_cast<BezierWarpPara>(filterPara);
            bezierWarpProperty->SetBezierWarp(bezierWarpPara);
            uiFilter->Insert(bezierWarpProperty);
        }
        if (filterPara->GetParaType() == FilterPara::HDR_BRIGHTNESS_RATIO) {
            auto hdrBrightnessRatioPara = std::static_pointer_cast<HDRBrightnessRatioPara>(filterPara);
            auto brightnessRatio = hdrBrightnessRatioPara->GetBrightnessRatio();
            SetHDRUIBrightness(brightnessRatio);
        }
        if (filterPara->GetParaType() == FilterPara::CONTENT_LIGHT) {
            auto contentLightProperty = std::make_shared<RSUIContentLightFilterPara>();
            auto contentLightPara = std::static_pointer_cast<ContentLightPara>(filterPara);
            contentLightProperty->SetContentLight(contentLightPara);
            uiFilter->Insert(contentLightProperty);
        }
    }
    if (!uiFilter->GetAllTypes().empty()) {
        SetForegroundUIFilter(uiFilter);
    }
    SetForegroundNGFilter(headFilter);
}

void RSNode::SetForegroundUIFilter(const std::shared_ptr<RSUIFilter> foregroundFilter)
{
    if (foregroundFilter == nullptr) {
        ROSEN_LOGE("RSNode::SetForegroundUIFilter foregroundFilter is nullptr");
        return;
    }

#if defined(MODIFIER_NG)
    SetUIFilterPropertyNG<ModifierNG::RSForegroundFilterModifier,
        &ModifierNG::RSForegroundFilterModifier::SetUIFilter>(foregroundFilter);
#else
    bool shouldAdd = true;
    std::shared_ptr<RSUIFilter> oldProperty = nullptr;
    auto iter = propertyModifiers_.find(RSModifierType::FOREGROUND_UI_FILTER);
    if (iter != propertyModifiers_.end() && iter->second != nullptr) {
        auto oldRsPro = std::static_pointer_cast<RSProperty<std::shared_ptr<RSUIFilter>>>(
            iter->second->GetProperty());
        if (oldRsPro) {
            oldProperty = oldRsPro->Get();
        }
        if (oldProperty && foregroundFilter->IsStructureSame(oldProperty)) {
            shouldAdd = false;
            oldProperty->SetValue(foregroundFilter);
            return;
        }
    }

    if (shouldAdd) {
        auto rsProperty = std::make_shared<RSProperty<std::shared_ptr<RSUIFilter>>>(foregroundFilter);
        auto propertyModifier = std::make_shared<RSForegroundUIFilterModifier>(rsProperty);
        propertyModifiers_.emplace(RSModifierType::FOREGROUND_UI_FILTER, propertyModifier);
        AddModifier(propertyModifier);
    }
#endif
}

void RSNode::SetHDRUIBrightness(float hdrUIBrightness)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSHDRBrightnessModifier, &ModifierNG::RSHDRBrightnessModifier::SetHDRUIBrightness>(
        hdrUIBrightness);
#else
    SetProperty<RSHDRUIBrightnessModifier, RSAnimatableProperty<float>>(
        RSModifierType::HDR_UI_BRIGHTNESS, hdrUIBrightness);
#endif
}

void RSNode::SetVisualEffect(const VisualEffect* visualEffect)
{
    if (visualEffect == nullptr) {
        ROSEN_LOGE("Failed to set visualEffect, visualEffect is null!");
        return;
    }
    // To do: generate composed visual effect here. Now we just set background brightness in v1.0.
    auto visualEffectParas = visualEffect->GetAllPara();
    bool hasHdrBrightnessBlender = false;
    for (const auto& visualEffectPara : visualEffectParas) {
        if (visualEffectPara == nullptr) {
            continue;
        }
        if (visualEffectPara->GetParaType() == VisualEffectPara::BORDER_LIGHT_EFFECT) {
            SetBorderLightShader(visualEffectPara);
        }
        if (visualEffectPara->GetParaType() != VisualEffectPara::BACKGROUND_COLOR_EFFECT) {
            continue;
        }
        auto backgroundColorEffectPara = std::static_pointer_cast<BackgroundColorEffectPara>(visualEffectPara);
        auto blender = backgroundColorEffectPara->GetBlender();
        auto brightnessBlender = std::static_pointer_cast<BrightnessBlender>(blender);
        if (brightnessBlender == nullptr) {
            continue;
        }
        if (brightnessBlender->GetHdr() && ROSEN_GNE(brightnessBlender->GetFraction(), 0.0f)) {
            hasHdrBrightnessBlender = true;
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
    // Update blender hdr status
    if (hasHdrBrightnessBlender) {
        SetEnableHDREffect(HdrEffectType::HDR_EFFECT_BRIGHTNESS_BLENDER, true);
    }
}

void RSNode::SetBorderLightShader(std::shared_ptr<VisualEffectPara> visualEffectPara)
{
    if (visualEffectPara == nullptr) {
        ROSEN_LOGE("RSNode::SetBorderLightShader: visualEffectPara is null!");
        return;
    }
    auto borderLightEffectPara = std::static_pointer_cast<BorderLightEffectPara>(visualEffectPara);
    Vector3f rotationAngle;
    float cornerRadius = 1.0f;
    RSBorderLightParams borderLightParam = {
        borderLightEffectPara->GetLightPosition(),
        borderLightEffectPara->GetLightColor(),
        borderLightEffectPara->GetLightIntensity(),
        borderLightEffectPara->GetLightWidth(),
        rotationAngle,
        cornerRadius
    };
    auto borderLightShader = std::make_shared<RSBorderLightShader>();
    borderLightShader->SetRSBorderLightParams(borderLightParam);
    SetBackgroundShader(borderLightShader);
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
                    brightnessBlender->GetNegativeCoeff().z_ }});
            if (brightnessBlender->GetHdr()) {
                SetFgBrightnessHdr(brightnessBlender->GetHdr());
                SetEnableHDREffect(HdrEffectType::HDR_EFFECT_BRIGHTNESS_BLENDER, true);
            }
        }
    }
}

void RSNode::SetForegroundEffectRadius(const float blurRadius)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSForegroundFilterModifier,
        &ModifierNG::RSForegroundFilterModifier::SetForegroundEffectRadius>(blurRadius);
#else
    SetProperty<RSForegroundEffectRadiusModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_EFFECT_RADIUS, blurRadius);
#endif
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

void RSNode::SetBackgroundNGFilter(const std::shared_ptr<RSNGFilterBase>& backgroundFilter)
{
#if defined(MODIFIER_NG)
    if (!backgroundFilter) {
        ROSEN_LOGW("RSNode::SetBackgroundNGFilter background filter is nullptr");
        auto& modifier =
            modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::BACKGROUND_FILTER)];
        if (modifier != nullptr) {
            modifier->DetachProperty(ModifierNG::RSPropertyType::BACKGROUND_NG_FILTER);
        }
        return;
    }
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetNGFilterBase>(backgroundFilter);
#endif
}

void RSNode::SetForegroundNGFilter(const std::shared_ptr<RSNGFilterBase>& foregroundFilter)
{
#if defined(MODIFIER_NG)
    if (!foregroundFilter) {
        ROSEN_LOGW("RSNode::SetForegroundNGFilter background filter is nullptr");
        auto& modifier =
            modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::FOREGROUND_FILTER)];
        if (modifier != nullptr) {
            modifier->DetachProperty(ModifierNG::RSPropertyType::FOREGROUND_NG_FILTER);
        }
        return;
    }
    SetPropertyNG<ModifierNG::RSForegroundFilterModifier,
        &ModifierNG::RSForegroundFilterModifier::SetNGFilterBase>(foregroundFilter);
#endif
}

void RSNode::SetBackgroundNGShader(const std::shared_ptr<RSNGShaderBase>& backgroundShader)
{
#if defined(MODIFIER_NG)
    if (!backgroundShader) {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto& modifier =
            modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::BACKGROUND_NG_SHADER)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::BACKGROUND_NG_SHADER)) {
            return;
        }
        modifier->DetachProperty(ModifierNG::RSPropertyType::BACKGROUND_NG_SHADER);
        return;
    }
    SetPropertyNG<ModifierNG::RSBackgroundNGShaderModifier,
        &ModifierNG::RSBackgroundNGShaderModifier::SetBackgroundNGShader>(backgroundShader);
#else
    // origin
#endif
}

void RSNode::SetForegroundShader(const std::shared_ptr<RSNGShaderBase>& foregroundShader)
{
#if defined(MODIFIER_NG)
    if (!foregroundShader) {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        auto& modifier =
            modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierNG::RSModifierType::FOREGROUND_SHADER)];
        if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::FOREGROUND_SHADER)) {
            return;
        }
        modifier->DetachProperty(ModifierNG::RSPropertyType::FOREGROUND_SHADER);
        return;
    }
    SetPropertyNG<ModifierNG::RSForegroundShaderModifier,
        &ModifierNG::RSForegroundShaderModifier::SetForegroundShader>(foregroundShader);
#else
    // origin
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetLinearGradientBlurPara>(para);
#else
    SetProperty<RSLinearGradientBlurParaModifier, RSProperty<std::shared_ptr<RSLinearGradientBlurPara>>>(
        RSModifierType::LINEAR_GRADIENT_BLUR_PARA, para);
#endif
}

void RSNode::SetMotionBlurPara(const float radius, const Vector2f& anchor)
{
    Vector2f anchor1 = { anchor[0], anchor[1] };
    std::shared_ptr<MotionBlurParam> para = std::make_shared<MotionBlurParam>(radius, anchor1);
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSForegroundFilterModifier, &ModifierNG::RSForegroundFilterModifier::SetMotionBlurParam>(
        para);
#else
    SetProperty<RSMotionBlurParaModifier, RSProperty<std::shared_ptr<MotionBlurParam>>>(
        RSModifierType::MOTION_BLUR_PARA, para);
#endif
}

void RSNode::SetDynamicLightUpRate(const float rate)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSDynamicLightUpModifier, &ModifierNG::RSDynamicLightUpModifier::SetDynamicLightUpRate>(
        rate);
#else
    SetProperty<RSDynamicLightUpRateModifier, RSAnimatableProperty<float>>(RSModifierType::DYNAMIC_LIGHT_UP_RATE, rate);
#endif
}

void RSNode::SetDynamicLightUpDegree(const float lightUpDegree)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSDynamicLightUpModifier, &ModifierNG::RSDynamicLightUpModifier::SetDynamicLightUpDegree>(
        lightUpDegree);
#else
    SetProperty<RSDynamicLightUpDegreeModifier, RSAnimatableProperty<float>>(
        RSModifierType::DYNAMIC_LIGHT_UP_DEGREE, lightUpDegree);
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBlendModifier, &ModifierNG::RSBlendModifier::SetFgBrightnessRates>(rates);
#else
    SetProperty<RSFgBrightnessRatesModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::FG_BRIGHTNESS_RATES, rates);
#endif
}

void RSNode::SetFgBrightnessSaturation(const float& saturation)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBlendModifier, &ModifierNG::RSBlendModifier::SetFgBrightnessSaturation>(saturation);
#else
    SetProperty<RSFgBrightnessSaturationModifier, RSAnimatableProperty<float>>(
        RSModifierType::FG_BRIGHTNESS_SATURATION, saturation);
#endif
}

void RSNode::SetFgBrightnessPosCoeff(const Vector4f& coeff)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBlendModifier, &ModifierNG::RSBlendModifier::SetFgBrightnessPosCoeff>(coeff);
#else
    SetProperty<RSFgBrightnessPosCoeffModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::FG_BRIGHTNESS_POSCOEFF, coeff);
#endif
}

void RSNode::SetFgBrightnessNegCoeff(const Vector4f& coeff)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBlendModifier, &ModifierNG::RSBlendModifier::SetFgBrightnessNegCoeff>(coeff);
#else
    SetProperty<RSFgBrightnessNegCoeffModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::FG_BRIGHTNESS_NEGCOEFF, coeff);
#endif
}

void RSNode::SetFgBrightnessFract(const float& fract)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBlendModifier, &ModifierNG::RSBlendModifier::SetFgBrightnessFract>(fract);
#else
    SetProperty<RSFgBrightnessFractModifier, RSAnimatableProperty<float>>(
        RSModifierType::FG_BRIGHTNESS_FRACTION, fract);
#endif
}

void RSNode::SetFgBrightnessHdr(const bool hdr)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBlendModifier, &ModifierNG::RSBlendModifier::SetFgBrightnessHdr>(hdr);
#else
    SetProperty<RSFgBrightnessHdrModifier, RSProperty<bool>>(RSModifierType::FG_BRIGHTNESS_HDR, hdr);
#endif
}

void RSNode::SetBgBrightnessParams(const RSDynamicBrightnessPara& params)
{
    // Compatible with original interfaces
    SetBgBrightnessRates(params.rates_);
    SetBgBrightnessSaturation(params.saturation_);
    SetBgBrightnessPosCoeff(params.posCoeff_);
    SetBgBrightnessNegCoeff(params.negCoeff_);
}

void RSNode::SetBgBrightnessRates(const Vector4f& rates)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundColorModifier, &ModifierNG::RSBackgroundColorModifier::SetBgBrightnessRates>(
        rates);
#else
    SetProperty<RSBgBrightnessRatesModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::BG_BRIGHTNESS_RATES, rates);
#endif
}

void RSNode::SetBgBrightnessSaturation(const float& saturation)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundColorModifier,
        &ModifierNG::RSBackgroundColorModifier::SetBgBrightnessSaturation>(saturation);
#else
    SetProperty<RSBgBrightnessSaturationModifier, RSAnimatableProperty<float>>(
        RSModifierType::BG_BRIGHTNESS_SATURATION, saturation);
#endif
}

void RSNode::SetBgBrightnessPosCoeff(const Vector4f& coeff)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundColorModifier,
        &ModifierNG::RSBackgroundColorModifier::SetBgBrightnessPosCoeff>(coeff);
#else
    SetProperty<RSBgBrightnessPosCoeffModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::BG_BRIGHTNESS_POSCOEFF, coeff);
#endif
}

void RSNode::SetBgBrightnessNegCoeff(const Vector4f& coeff)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundColorModifier,
        &ModifierNG::RSBackgroundColorModifier::SetBgBrightnessNegCoeff>(coeff);
#else
    SetProperty<RSBgBrightnessNegCoeffModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::BG_BRIGHTNESS_NEGCOEFF, coeff);
#endif
}

void RSNode::SetBgBrightnessFract(const float& fract)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundColorModifier, &ModifierNG::RSBackgroundColorModifier::SetBgBrightnessFract>(
        fract);
#else
    SetProperty<RSBgBrightnessFractModifier, RSAnimatableProperty<float>>(
        RSModifierType::BG_BRIGHTNESS_FRACTION, fract);
#endif
}

void RSNode::SetGreyCoef(const Vector2f greyCoef)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier, &ModifierNG::RSBackgroundFilterModifier::SetGreyCoef>(
        greyCoef);
#else
    SetProperty<RSGreyCoefModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::GREY_COEF, greyCoef);
#endif
}

void RSNode::SetCompositingFilter(const std::shared_ptr<RSFilter>& compositingFilter) {}

void RSNode::SetShadowColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowColor>(color);
#else
    SetProperty<RSShadowColorModifier, RSAnimatableProperty<Color>>(RSModifierType::SHADOW_COLOR, color);
#endif
}

void RSNode::SetShadowOffset(float offsetX, float offsetY)
{
    SetShadowOffsetX(offsetX);
    SetShadowOffsetY(offsetY);
}

void RSNode::SetShadowOffsetX(float offsetX)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowOffsetX>(offsetX);
#else
    SetProperty<RSShadowOffsetXModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_OFFSET_X, offsetX);
#endif
}

void RSNode::SetShadowOffsetY(float offsetY)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowOffsetY>(offsetY);
#else
    SetProperty<RSShadowOffsetYModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_OFFSET_Y, offsetY);
#endif
}

void RSNode::SetShadowAlpha(float alpha)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowAlpha>(alpha);
#else
    SetProperty<RSShadowAlphaModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_ALPHA, alpha);
#endif
}

void RSNode::SetShadowElevation(float elevation)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowRadius>(0);
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowElevation>(elevation);
#else
    SetProperty<RSShadowRadiusModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_RADIUS, 0);
    SetProperty<RSShadowElevationModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_ELEVATION, elevation);
#endif
}

void RSNode::SetShadowRadius(float radius)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowElevation>(0);
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowRadius>(radius);
#else
    SetProperty<RSShadowElevationModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_ELEVATION, 0);
    SetProperty<RSShadowRadiusModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_RADIUS, radius);
#endif
}

void RSNode::SetShadowPath(const std::shared_ptr<RSPath>& shadowPath)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowPath>(shadowPath);
#else
    SetProperty<RSShadowPathModifier, RSProperty<std::shared_ptr<RSPath>>>(RSModifierType::SHADOW_PATH, shadowPath);
#endif
}

void RSNode::SetShadowMask(bool shadowMask)
{
    SetShadowMaskStrategy(shadowMask ? SHADOW_MASK_STRATEGY::MASK_BLUR : SHADOW_MASK_STRATEGY::MASK_NONE);
}

void RSNode::SetShadowMaskStrategy(SHADOW_MASK_STRATEGY strategy)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowMask>(strategy);
#else
    SetProperty<RSShadowMaskModifier, RSProperty<int>>(RSModifierType::SHADOW_MASK, strategy);
#endif
}

void RSNode::SetShadowIsFilled(bool shadowIsFilled)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowIsFilled>(shadowIsFilled);
#else
    SetProperty<RSShadowIsFilledModifier, RSProperty<bool>>(RSModifierType::SHADOW_IS_FILLED, shadowIsFilled);
#endif
}

void RSNode::SetShadowColorStrategy(int shadowColorStrategy)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowColorStrategy>(
        shadowColorStrategy);
#else
    SetProperty<RSShadowColorStrategyModifier, RSProperty<int>>(
        RSModifierType::SHADOW_COLOR_STRATEGY, shadowColorStrategy);
#endif
}

void RSNode::SetFrameGravity(Gravity gravity)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSFrameClipModifier, &ModifierNG::RSFrameClipModifier::SetFrameGravity>(gravity);
#else
    SetProperty<RSFrameGravityModifier, RSProperty<Gravity>>(RSModifierType::FRAME_GRAVITY, gravity);
#endif
}

void RSNode::SetClipRRect(const Vector4f& clipRect, const Vector4f& clipRadius)
{
    SetClipRRect(std::make_shared<RRect>(clipRect, clipRadius));
}

void RSNode::SetClipRRect(const std::shared_ptr<RRect>& rrect)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBoundsClipModifier, &ModifierNG::RSBoundsClipModifier::SetClipRRect>(rrect);
#else
    SetProperty<RSClipRRectModifier, RSAnimatableProperty<RRect>>(RSModifierType::CLIP_RRECT, rrect ? *rrect : RRect());
#endif
}

void RSNode::SetClipBounds(const std::shared_ptr<RSPath>& path)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBoundsClipModifier, &ModifierNG::RSBoundsClipModifier::SetClipBounds>(path);
#else
    SetProperty<RSClipBoundsModifier, RSProperty<std::shared_ptr<RSPath>>>(RSModifierType::CLIP_BOUNDS, path);
#endif
}

void RSNode::SetClipToBounds(bool clipToBounds)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBoundsClipModifier, &ModifierNG::RSBoundsClipModifier::SetClipToBounds>(clipToBounds);
#else
    SetProperty<RSClipToBoundsModifier, RSProperty<bool>>(RSModifierType::CLIP_TO_BOUNDS, clipToBounds);
#endif
}

void RSNode::SetClipToFrame(bool clipToFrame)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSFrameClipModifier, &ModifierNG::RSFrameClipModifier::SetClipToFrame>(clipToFrame);
#else
    SetProperty<RSClipToFrameModifier, RSProperty<bool>>(RSModifierType::CLIP_TO_FRAME, clipToFrame);
#endif
}

void RSNode::SetCustomClipToFrame(const Vector4f& clipRect)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSFrameClipModifier, &ModifierNG::RSFrameClipModifier::SetCustomClipToFrame>(clipRect);
#else
    SetProperty<RSCustomClipToFrameModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::CUSTOM_CLIP_TO_FRAME, clipRect);
#endif
}

void RSNode::SetHDRBrightness(const float& hdrBrightness)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSHDRBrightnessModifier, &ModifierNG::RSHDRBrightnessModifier::SetHDRBrightness>(
        hdrBrightness);
#else
    SetProperty<RSHDRBrightnessModifier, RSAnimatableProperty<float>>(RSModifierType::HDR_BRIGHTNESS, hdrBrightness);
#endif
}

void RSNode::SetHDRBrightnessFactor(float factor)
{
    if (!IsInstanceOf<RSDisplayNode>()) {
        ROSEN_LOGE("SetHDRBrightnessFactor only can be used by RSDisplayNode");
        return;
    }
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSHDRBrightnessModifier, &ModifierNG::RSHDRBrightnessModifier::SetHDRBrightnessFactor>(
        factor);
#else
    SetProperty<RSHDRBrightnessFactorModifier, RSAnimatableProperty<float>>(
        RSModifierType::HDR_BRIGHTNESS_FACTOR, factor);
#endif
}

void RSNode::SetVisible(bool visible)
{
    // kick off transition only if it's on tree(has valid parent) and visibility is changed.
    if (transitionEffect_ != nullptr && GetParent() != nullptr && visible != GetStagingProperties().GetVisible()) {
        NotifyTransition(transitionEffect_, visible);
    }

#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSVisibilityModifier, &ModifierNG::RSVisibilityModifier::SetVisible>(visible);
#else
    SetProperty<RSVisibleModifier, RSProperty<bool>>(RSModifierType::VISIBLE, visible);
#endif
}

void RSNode::SetMask(const std::shared_ptr<RSMask>& mask)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSMaskModifier, &ModifierNG::RSMaskModifier::SetMask>(mask);
#else
    SetProperty<RSMaskModifier, RSProperty<std::shared_ptr<RSMask>>>(RSModifierType::MASK, mask);
#endif
}

void RSNode::SetUseEffect(bool useEffect)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSUseEffectModifier, &ModifierNG::RSUseEffectModifier::SetUseEffect>(useEffect);
#else
    SetProperty<RSUseEffectModifier, RSProperty<bool>>(RSModifierType::USE_EFFECT, useEffect);
#endif
}

void RSNode::SetUseEffectType(UseEffectType useEffectType)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSUseEffectModifier, &ModifierNG::RSUseEffectModifier::SetUseEffectType>(useEffectType);
#else
    SetProperty<RSUseEffectTypeModifier, RSProperty<int>>(
        RSModifierType::USE_EFFECT_TYPE, static_cast<int>(useEffectType));
#endif
}

void RSNode::SetAlwaysSnapshot(bool enable)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier, &ModifierNG::RSBackgroundFilterModifier::SetAlwaysSnapshot>(
        enable);
#else
    SetProperty<RSAlwaysSnapshotModifier, RSProperty<bool>>(RSModifierType::ALWAYS_SNAPSHOT, enable);
#endif
}

void RSNode::SetEnableHDREffect(uint32_t type, bool enableHdrEffect)
{
    bool old = hdrEffectType_ > HdrEffectType::HDR_EFFECT_NONE;
    if (enableHdrEffect) {
        hdrEffectType_ |= type;
    } else {
        hdrEffectType_ &= ~type;
    }
    bool enabled = hdrEffectType_ > HdrEffectType::HDR_EFFECT_NONE;
    if (old == enabled) {
        return;
    }
    ROSEN_LOGD("RSNode::SetEnableHDREffect hdrEffectType=%{public}d", static_cast<int>(hdrEffectType_));
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetEnableHDREffect>(GetId(), enabled);
    AddCommand(command, IsRenderServiceNode());
}

void RSNode::SetUseShadowBatching(bool useShadowBatching)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetUseShadowBatching>(useShadowBatching);
#else
    SetProperty<RSUseShadowBatchingModifier, RSProperty<bool>>(RSModifierType::USE_SHADOW_BATCHING, useShadowBatching);
#endif
}

void RSNode::SetColorBlendMode(RSColorBlendMode colorBlendMode)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBlendModifier, &ModifierNG::RSBlendModifier::SetColorBlendMode>(colorBlendMode);
#else
    SetProperty<RSColorBlendModeModifier, RSProperty<int>>(
        RSModifierType::COLOR_BLEND_MODE, static_cast<int>(colorBlendMode));
#endif
}

void RSNode::SetColorBlendApplyType(RSColorBlendApplyType colorBlendApplyType)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBlendModifier, &ModifierNG::RSBlendModifier::SetColorBlendApplyType>(
        colorBlendApplyType);
#else
    SetProperty<RSColorBlendApplyTypeModifier, RSProperty<int>>(
        RSModifierType::COLOR_BLEND_APPLY_TYPE, static_cast<int>(colorBlendApplyType));
#endif
}

void RSNode::SetPixelStretch(const Vector4f& stretchSize, Drawing::TileMode stretchTileMode)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSPixelStretchModifier, &ModifierNG::RSPixelStretchModifier::SetPixelStretchSize>(
        stretchSize);
    SetPropertyNG<ModifierNG::RSPixelStretchModifier, &ModifierNG::RSPixelStretchModifier::SetPixelStretchTileMode>(
        static_cast<int>(stretchTileMode));
#else
    SetProperty<RSPixelStretchModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::PIXEL_STRETCH, stretchSize);
    SetProperty<RSPixelStretchTileModeModifier, RSProperty<int>>(
        RSModifierType::PIXEL_STRETCH_TILE_MODE, static_cast<int>(stretchTileMode));
#endif
}

void RSNode::SetPixelStretchPercent(const Vector4f& stretchPercent, Drawing::TileMode stretchTileMode)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSPixelStretchModifier, &ModifierNG::RSPixelStretchModifier::SetPixelStretchPercent>(
        stretchPercent);
    SetPropertyNG<ModifierNG::RSPixelStretchModifier, &ModifierNG::RSPixelStretchModifier::SetPixelStretchTileMode>(
        static_cast<int>(stretchTileMode));
#else
    SetProperty<RSPixelStretchPercentModifier, RSAnimatableProperty<Vector4f>>(
        RSModifierType::PIXEL_STRETCH_PERCENT, stretchPercent);
    SetProperty<RSPixelStretchTileModeModifier, RSProperty<int>>(
        RSModifierType::PIXEL_STRETCH_TILE_MODE, static_cast<int>(stretchTileMode));
#endif
}

void RSNode::SetWaterRippleParams(const RSWaterRipplePara& params, float progress)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetWaterRippleParams>(params);
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetWaterRippleProgress>(progress);
#else
    SetProperty<RSWaterRippleParamsModifier, RSProperty<RSWaterRipplePara>>(
        RSModifierType::WATER_RIPPLE_PARAMS, params);
    SetProperty<RSWaterRippleProgressModifier, RSAnimatableProperty<float>>(
        RSModifierType::WATER_RIPPLE_PROGRESS, progress);
#endif
}

void RSNode::SetFlyOutParams(const RSFlyOutPara& params, float degree)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSForegroundFilterModifier, &ModifierNG::RSForegroundFilterModifier::SetFlyOutParams>(
        params);
    SetPropertyNG<ModifierNG::RSForegroundFilterModifier, &ModifierNG::RSForegroundFilterModifier::SetFlyOutDegree>(
        degree);
#else
    SetProperty<RSFlyOutParamsModifier, RSProperty<RSFlyOutPara>>(RSModifierType::FLY_OUT_PARAMS, params);
    SetProperty<RSFlyOutDegreeModifier, RSAnimatableProperty<float>>(RSModifierType::FLY_OUT_DEGREE, degree);
#endif
}

void RSNode::SetDistortionK(const float distortionK)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSForegroundFilterModifier, &ModifierNG::RSForegroundFilterModifier::SetDistortionK>(
        distortionK);
#else
    SetProperty<RSDistortionKModifier, RSAnimatableProperty<float>>(RSModifierType::DISTORTION_K, distortionK);
#endif
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
        ROSEN_LOGW("OffScreenIsSync SetTakeSurfaceForUIFlag AddCommand be processed. nodeId: [%{public}" PRIu64 "]"
            ", IsRenderServiceNode: [%{public}s]", GetId(), IsRenderServiceNode() ? "true" : "false");
        transaction->FlushImplicitTransaction();
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode());
            ROSEN_LOGW("OffScreenIsSync SetTakeSurfaceForUIFlag AddCommand be processed. nodeId:[%{public}" PRIu64 "]"
                ", IsRenderServiceNode: [%{public}s] (Proxy)", GetId(), IsRenderServiceNode() ? "true" : "false");
            transactionProxy->FlushImplicitTransaction();
        }
    }
}

void RSNode::SetSpherizeDegree(float spherizeDegree)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSForegroundFilterModifier, &ModifierNG::RSForegroundFilterModifier::SetSpherize>(
        spherizeDegree);
#else
    SetProperty<RSSpherizeModifier, RSAnimatableProperty<float>>(RSModifierType::SPHERIZE, spherizeDegree);
#endif
}

void RSNode::SetAttractionEffect(float fraction, const Vector2f& destinationPoint)
{
    SetAttractionEffectFraction(fraction);
    SetAttractionEffectDstPoint(destinationPoint);
}

void RSNode::SetAttractionEffectFraction(float fraction)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSForegroundFilterModifier,
        &ModifierNG::RSForegroundFilterModifier::SetAttractionFraction>(fraction);
#else
    SetProperty<RSAttractionFractionModifier, RSAnimatableProperty<float>>(
        RSModifierType::ATTRACTION_FRACTION, fraction);
#endif
}

void RSNode::SetAttractionEffectDstPoint(const Vector2f& destinationPoint)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSForegroundFilterModifier,
        &ModifierNG::RSForegroundFilterModifier::SetAttractionDstPoint>(destinationPoint);
#else
    SetProperty<RSAttractionDstPointModifier, RSAnimatableProperty<Vector2f>>(
        RSModifierType::ATTRACTION_DSTPOINT, destinationPoint);
#endif
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
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetBackgroundBlurRadius>(radius);
#else
    SetProperty<RSBackgroundBlurRadiusModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_RADIUS, radius);
#endif
}

void RSNode::SetBackgroundBlurSaturation(float saturation)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetBackgroundBlurSaturation>(saturation);
#else
    SetProperty<RSBackgroundBlurSaturationModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_SATURATION, saturation);
#endif
}

void RSNode::SetBackgroundBlurBrightness(float brightness)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetBackgroundBlurBrightness>(brightness);
#else
    SetProperty<RSBackgroundBlurBrightnessModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_BRIGHTNESS, brightness);
#endif
}

void RSNode::SetBackgroundBlurMaskColor(Color maskColor)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetBackgroundBlurMaskColor>(maskColor);
#else
    SetProperty<RSBackgroundBlurMaskColorModifier, RSAnimatableProperty<Color>>(
        RSModifierType::BACKGROUND_BLUR_MASK_COLOR, maskColor);
#endif
}

void RSNode::SetBackgroundBlurColorMode(int colorMode)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetBackgroundBlurColorMode>(colorMode);
#else
    SetProperty<RSBackgroundBlurColorModeModifier, RSProperty<int>>(
        RSModifierType::BACKGROUND_BLUR_COLOR_MODE, colorMode);
#endif
}

void RSNode::SetBackgroundBlurRadiusX(float blurRadiusX)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetBackgroundBlurRadiusX>(blurRadiusX);
#else
    SetProperty<RSBackgroundBlurRadiusXModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_RADIUS_X, blurRadiusX);
#endif
}

void RSNode::SetBackgroundBlurRadiusY(float blurRadiusY)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetBackgroundBlurRadiusY>(blurRadiusY);
#else
    SetProperty<RSBackgroundBlurRadiusYModifier, RSAnimatableProperty<float>>(
        RSModifierType::BACKGROUND_BLUR_RADIUS_Y, blurRadiusY);
#endif
}

void RSNode::SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier,
        &ModifierNG::RSBackgroundFilterModifier::SetBgBlurDisableSystemAdaptation>(disableSystemAdaptation);
#else
    SetProperty<RSBgBlurDisableSystemAdaptationModifier, RSProperty<bool>>(
        RSModifierType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION, disableSystemAdaptation);
#endif
}

void RSNode::SetForegroundBlurRadius(float radius)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetForegroundBlurRadius>(radius);
#else
    SetProperty<RSForegroundBlurRadiusModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_RADIUS, radius);
#endif
}

void RSNode::SetForegroundBlurSaturation(float saturation)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetForegroundBlurSaturation>(saturation);
#else
    SetProperty<RSForegroundBlurSaturationModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_SATURATION, saturation);
#endif
}

void RSNode::SetForegroundBlurBrightness(float brightness)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetForegroundBlurBrightness>(brightness);
#else
    SetProperty<RSForegroundBlurBrightnessModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_BRIGHTNESS, brightness);
#endif
}

void RSNode::SetForegroundBlurMaskColor(Color maskColor)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetForegroundBlurMaskColor>(maskColor);
#else
    SetProperty<RSForegroundBlurMaskColorModifier, RSAnimatableProperty<Color>>(
        RSModifierType::FOREGROUND_BLUR_MASK_COLOR, maskColor);
#endif
}

void RSNode::SetForegroundBlurColorMode(int colorMode)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetForegroundBlurColorMode>(colorMode);
#else
    SetProperty<RSForegroundBlurColorModeModifier, RSProperty<int>>(
        RSModifierType::FOREGROUND_BLUR_COLOR_MODE, colorMode);
#endif
}

void RSNode::SetForegroundBlurRadiusX(float blurRadiusX)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetForegroundBlurRadiusX>(blurRadiusX);
#else
    SetProperty<RSForegroundBlurRadiusXModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_RADIUS_X, blurRadiusX);
#endif
}

void RSNode::SetForegroundBlurRadiusY(float blurRadiusY)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetForegroundBlurRadiusY>(blurRadiusY);
#else
    SetProperty<RSForegroundBlurRadiusYModifier, RSAnimatableProperty<float>>(
        RSModifierType::FOREGROUND_BLUR_RADIUS_Y, blurRadiusY);
#endif
}

void RSNode::SetFgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetFgBlurDisableSystemAdaptation>(disableSystemAdaptation);
#else
    SetProperty<RSFgBlurDisableSystemAdaptationModifier, RSProperty<bool>>(
        RSModifierType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION, disableSystemAdaptation);
#endif
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
#if defined(MODIFIER_NG)
    for (auto [id, modifier] : modifiersNG_) {
        if (modifier) {
            modifier->OnDetach();
        }
    }
    modifiersNG_.clear();
#else
    for (auto [id, modifier] : modifiers_) {
        if (modifier) {
            modifier->DetachFromNode();
        }
    }
    modifiers_.clear();
#endif
    propertyModifiers_.clear();
    modifiersTypeMap_.clear();
    properties_.clear();
}

// Check if the modifierType is a special type.
void RSNode::CheckModifierType(RSModifierType modifierType)
{
    if (modifierType != RSModifierType::BOUNDS && modifierType != RSModifierType::FRAME &&
        modifierType != RSModifierType::BACKGROUND_COLOR && modifierType != RSModifierType::ALPHA) {
        SetDrawNode();
        SetDrawNodeType(DrawNodeType::DrawPropertyType);
    }
    if (modifierType == RSModifierType::TRANSLATE || modifierType == RSModifierType::SKEW ||
        modifierType == RSModifierType::PERSP || modifierType == RSModifierType::SCALE ||
        modifierType == RSModifierType::PIVOT || modifierType == RSModifierType::ROTATION ||
        modifierType == RSModifierType::ROTATION_X || modifierType == RSModifierType::ROTATION_Y ||
        modifierType == RSModifierType::QUATERNION) {
        SetDrawNodeType(DrawNodeType::GeometryPropertyType);
    }
}

void RSNode::DoFlushModifier()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
#if defined(MODIFIER_NG)
    if (!modifiersNG_.empty()) {
        std::unique_ptr<RSCommand> removeAllModifiersCommand = std::make_unique<RSRemoveAllModifiersNG>(GetId());
        AddCommand(removeAllModifiersCommand, IsRenderServiceNode(), GetFollowType(), GetId());
        for (const auto& [_, modifier] : modifiersNG_) {
            std::unique_ptr<RSCommand> command =
                std::make_unique<RSAddModifierNG>(GetId(), modifier->CreateRenderModifier());
            AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        }
    }
#else
    if (!modifiers_.empty()) {
        std::unique_ptr<RSCommand> removeAllModifiersCommand = std::make_unique<RSRemoveAllModifiers>(GetId());
        AddCommand(removeAllModifiersCommand, IsRenderServiceNode(), GetFollowType(), GetId());
        for (const auto& [_, modifier] : modifiers_) {
            std::unique_ptr<RSCommand> command =
                std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
            AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
            ROSEN_LOGD("RSNode::flush modifier, node id: %{public}" PRIu64 ", type: %{public}s", GetId(),
                modifier->GetModifierTypeString().c_str());
        }
    }
#endif
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

const std::shared_ptr<RSPropertyBase> RSNode::GetProperty(const PropertyId& propertyId)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN_VALUE(CheckMultiThreadAccess(__func__), nullptr);
    auto iter = properties_.find(propertyId);
    if (iter != properties_.end()) {
        return iter->second;
    }

    return {};
}

void RSNode::RegisterProperty(std::shared_ptr<RSPropertyBase> property)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    if (property) {
        properties_.emplace(property->GetId(), property);
    }
}

void RSNode::UnregisterProperty(const PropertyId& propertyId)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto iter = properties_.find(propertyId);
    if (iter != properties_.end()) {
        properties_.erase(iter);
    }
}

const std::shared_ptr<ModifierNG::RSModifier> RSNode::GetModifierByType(const ModifierNG::RSModifierType& type)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN_VALUE(CheckMultiThreadAccess(__func__), nullptr);
    for (auto [id, modifier] : modifiersNG_) {
        if (modifier && modifier->GetType() == type) {
            return modifier;
        }
    }
    return {};
}

void RSNode::UpdateModifierMotionPathOption()
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetMotionPathOptionToProperty(ModifierNG::RSModifierType::BOUNDS, ModifierNG::RSPropertyType::BOUNDS);
    SetMotionPathOptionToProperty(ModifierNG::RSModifierType::FRAME, ModifierNG::RSPropertyType::FRAME);
    SetMotionPathOptionToProperty(ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::TRANSLATE);
    for (const auto& [_, property] : properties_) {
        if (property->IsPathAnimatable()) {
            property->SetMotionPathOption(motionPathOption_);
        }
    }
#else
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
#endif
}

void RSNode::SetMotionPathOptionToProperty(
    const ModifierNG::RSModifierType& modifierType, const ModifierNG::RSPropertyType& propertyType)
{
    auto& property = GetPropertyByType(modifierType, propertyType);
    if (!property) {
        return;
    }
    property->SetMotionPathOption(motionPathOption_);
}

bool RSNode::CheckMultiThreadAccess(const std::string& func) const
{
    if (isSkipCheckInMultiInstance_) {
        return true;
    }
    auto rsContext = rsUIContext_.lock();
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

void RSNode::UpdateOcclusionCullingStatus(bool enable, NodeId keyOcclusionNodeId)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSUpdateOcclusionCullingStatus>(GetId(), enable, keyOcclusionNodeId);
    AddCommand(command, IsRenderServiceNode());
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
    auto modifierManager = rsUIContext ? rsUIContext->GetRSModifierManager()
                                       : RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    extendModifierIsDirty_ = true;
#if defined(MODIFIER_NG)
    for (auto& [id, modifier] : modifiersNG_) {
        if (modifier->GetType() < ModifierNG::RSModifierType::TRANSITION_STYLE) {
            continue;
        }
#else
    for (auto& [id, modifier] : modifiers_) {
        if (modifier->GetModifierType() < RSModifierType::CUSTOM) {
            continue;
        }
#endif
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
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetHybridRenderEnabled() && !drawRegion_->IsEmpty()) {
            RSModifiersDraw::AddDrawRegions(id_, drawRegion_);
        }
#endif
    }
}

void RSNode::SetNeedUseCmdlistDrawRegion(bool needUseCmdlistDrawRegion)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSetNeedUseCmdlistDrawRegion>(GetId(), needUseCmdlistDrawRegion);
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
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
        ROSEN_LOGD("RSNode::RegisterTransitionPair, rsUIContext is nullptr");
        RegisterTransitionPair(inNodeId, outNodeId, isInSameWindow);
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
        ROSEN_LOGD("RSNode::UnregisterTransitionPair, rsUIContext is nullptr");
        UnregisterTransitionPair(inNodeId, outNodeId);
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

void RSNode::MarkRepaintBoundary(const std::string& tag)
{
    bool isRepaintBoundary = CheckRbPatten(tag);
    if (isRepaintBoundary_ == isRepaintBoundary) {
        return;
    }
    isRepaintBoundary_ = isRepaintBoundary;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkRepaintBoundary>(id_, isRepaintBoundary_);
    AddCommand(command, IsRenderServiceNode());
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


/**
 * @brief Sets the drawing type of RSnode
 *
 * This function is used to set the corresponding draw type when
 * adding draw properties to RSnode, so that it is easy to identify
 * which draw nodes are really needed.
 *
 * @param nodeType The type of node that needs to be set.
 *
*/
void RSNode::SetDrawNodeType(DrawNodeType nodeType)
{
    // Assign values according to the priority rules
    if (nodeType <= drawNodeType_) {
        return;
    }
    drawNodeType_ = nodeType;
    if (RSSystemProperties::ViewDrawNodeType()) {
        SyncDrawNodeType(nodeType);
    }
}

DrawNodeType RSNode::GetDrawNodeType() const
{
    return drawNodeType_;
}

void RSNode::SyncDrawNodeType(DrawNodeType nodeType)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSSetDrawNodeType>(GetId(), nodeType);
    if (AddCommand(command, true)) {
        ROSEN_LOGD("RSNode::SyncDrawNodeType nodeType: %{public}d", nodeType);
    }
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

void RSNode::SetLightIntensity(float lightIntensity)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSPointLightModifier, &ModifierNG::RSPointLightModifier::SetLightIntensity>(
        lightIntensity);
#else
    SetProperty<RSLightIntensityModifier, RSAnimatableProperty<float>>(RSModifierType::LIGHT_INTENSITY, lightIntensity);
#endif
}

void RSNode::SetLightColor(uint32_t lightColorValue)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto lightColor = Color::FromArgbInt(lightColorValue);
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSPointLightModifier, &ModifierNG::RSPointLightModifier::SetLightColor>(lightColor);
#else
    SetProperty<RSLightColorModifier, RSAnimatableProperty<Color>>(RSModifierType::LIGHT_COLOR, lightColor);
#endif
}

void RSNode::SetLightPosition(float positionX, float positionY, float positionZ)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    SetLightPosition(Vector4f(positionX, positionY, positionZ, 0.f));
}

void RSNode::SetLightPosition(const Vector4f& lightPosition)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSPointLightModifier, &ModifierNG::RSPointLightModifier::SetLightPosition>(lightPosition);
#else
    SetProperty<RSLightPositionModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::LIGHT_POSITION, lightPosition);
#endif
}

void RSNode::SetIlluminatedBorderWidth(float illuminatedBorderWidth)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSPointLightModifier, &ModifierNG::RSPointLightModifier::SetIlluminatedBorderWidth>(
        illuminatedBorderWidth);
#else
    SetProperty<RSIlluminatedBorderWidthModifier, RSAnimatableProperty<float>>(
        RSModifierType::ILLUMINATED_BORDER_WIDTH, illuminatedBorderWidth);
#endif
}

void RSNode::SetIlluminatedType(uint32_t illuminatedType)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSPointLightModifier, &ModifierNG::RSPointLightModifier::SetIlluminatedType>(
        illuminatedType);
#else
    SetProperty<RSIlluminatedTypeModifier, RSProperty<int>>(RSModifierType::ILLUMINATED_TYPE, illuminatedType);
#endif
}

void RSNode::SetBloom(float bloomIntensity)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSPointLightModifier, &ModifierNG::RSPointLightModifier::SetBloom>(bloomIntensity);
#else
    SetProperty<RSBloomModifier, RSAnimatableProperty<float>>(RSModifierType::BLOOM, bloomIntensity);
#endif
}

void RSNode::SetAiInvert(const Vector4f& aiInvert)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier, &ModifierNG::RSCompositingFilterModifier::SetAiInvert>(
        aiInvert);
#else
    SetProperty<RSAiInvertModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::AIINVERT, aiInvert);
#endif
}

void RSNode::SetGrayScale(float grayScale)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier, &ModifierNG::RSCompositingFilterModifier::SetGrayScale>(
        grayScale);
#else
    SetProperty<RSGrayScaleModifier, RSAnimatableProperty<float>>(RSModifierType::GRAY_SCALE, grayScale);
#endif
}

void RSNode::SetBrightness(float brightness)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier, &ModifierNG::RSCompositingFilterModifier::SetBrightness>(
        brightness);
#else
    SetProperty<RSBrightnessModifier, RSAnimatableProperty<float>>(RSModifierType::BRIGHTNESS, brightness);
#endif
}

void RSNode::SetContrast(float contrast)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier, &ModifierNG::RSCompositingFilterModifier::SetContrast>(
        contrast);
#else
    SetProperty<RSContrastModifier, RSAnimatableProperty<float>>(RSModifierType::CONTRAST, contrast);
#endif
}

void RSNode::SetSaturate(float saturate)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier, &ModifierNG::RSCompositingFilterModifier::SetSaturate>(
        saturate);
#else
    SetProperty<RSSaturateModifier, RSAnimatableProperty<float>>(RSModifierType::SATURATE, saturate);
#endif
}

void RSNode::SetSepia(float sepia)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier, &ModifierNG::RSCompositingFilterModifier::SetSepia>(sepia);
#else
    SetProperty<RSSepiaModifier, RSAnimatableProperty<float>>(RSModifierType::SEPIA, sepia);
#endif
}

void RSNode::SetInvert(float invert)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier, &ModifierNG::RSCompositingFilterModifier::SetInvert>(invert);
#else
    SetProperty<RSInvertModifier, RSAnimatableProperty<float>>(RSModifierType::INVERT, invert);
#endif
}

void RSNode::SetHueRotate(float hueRotate)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier, &ModifierNG::RSCompositingFilterModifier::SetHueRotate>(
        hueRotate);
#else
    SetProperty<RSHueRotateModifier, RSAnimatableProperty<float>>(RSModifierType::HUE_ROTATE, hueRotate);
#endif
}

void RSNode::SetColorBlend(uint32_t colorValue)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto colorBlend = Color::FromArgbInt(colorValue);
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier, &ModifierNG::RSCompositingFilterModifier::SetColorBlend>(
        colorBlend);
#else
    SetProperty<RSColorBlendModifier, RSAnimatableProperty<Color>>(RSModifierType::COLOR_BLEND, colorBlend);
#endif
}

void RSNode::SetLightUpEffectDegree(float lightUpEffectDegree)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetLightUpEffectDegree>(lightUpEffectDegree);
#else
    SetProperty<RSLightUpEffectModifier, RSAnimatableProperty<float>>(
        RSModifierType::LIGHT_UP_EFFECT, lightUpEffectDegree);
#endif
}

void RSNode::SetDynamicDimDegree(const float dimDegree)
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSCompositingFilterModifier,
        &ModifierNG::RSCompositingFilterModifier::SetDynamicDimDegree>(dimDegree);
#else
    SetProperty<RSDynamicDimDegreeModifier, RSAnimatableProperty<float>>(RSModifierType::DYNAMIC_DIM_DEGREE, dimDegree);
#endif
}

void RSNode::SetSystemBarEffect()
{
#if defined(MODIFIER_NG)
    SetPropertyNG<ModifierNG::RSBackgroundFilterModifier, &ModifierNG::RSBackgroundFilterModifier::SetSystemBarEffect>(
        true);
#else
    SetProperty<RSSystemBarEffectModifier, RSProperty<bool>>(RSModifierType::SYSTEMBAREFFECT, true);
#endif
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
        isMultiInstanceOpen_ = RSSystemProperties::GetRSClientMultiInstanceEnabled();
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
    NotifyPageNodeChanged();
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
    for (auto child : children_) {
        auto childPtr = child.lock();
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
    if (!IsTextureExportNode() && child->IsTextureExportNode() && AddCompositeNodeChild(child, index)) {
        return;
    }
    if (child->parent_.lock().get() == this) {
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
    if (child->parent_.lock()) {
        child->RemoveFromTree();
    }

    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(child);
    } else {
        children_.insert(children_.begin() + index, child);
    }
    child->SetParent(weak_from_this());
    if (isTextureExportNode_ != child->isTextureExportNode_) {
        child->SyncTextureExport(isTextureExportNode_);
    }
    child->OnAddChildren();
    child->MarkDirty(NodeDirtyType::APPEARANCE, true);
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(id_, childId, index);

    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    if (child->GetRSUIContext() != GetRSUIContext()) {
        std::unique_ptr<RSCommand> child_command = std::make_unique<RSBaseNodeAddChild>(id_, childId, index);
        child->AddCommand(child_command, IsRenderServiceNode(), GetFollowType(), id_);
    }
    if (child->GetType() == RSUINodeType::SURFACE_NODE) {
        auto surfaceNode = RSBaseNode::ReinterpretCast<RSSurfaceNode>(child);
        ROSEN_LOGI("RSNode::AddChild, Id: %{public}" PRIu64 ", SurfaceNode:[Id: %{public}" PRIu64 ", name: %{public}s]",
            id_, childId, surfaceNode->GetName().c_str());
        RS_TRACE_NAME_FMT("RSNode::AddChild, Id: %" PRIu64 ", SurfaceNode:[Id: %" PRIu64 ", name: %s]",
            id_, childId, surfaceNode->GetName().c_str());
    }
    child->SetIsOnTheTree(isOnTheTree_);
}

bool RSNode::AddCompositeNodeChild(SharedPtr node, int index)
{
    if (!node) {
        return false;
    }
    auto surfaceNode = node->ReinterpretCastTo<RSSurfaceNode>();
    if (!surfaceNode) {
        return false;
    }
    auto compositeLayerUtils = surfaceNode->GetCompositeLayerUtils();
    if (compositeLayerUtils) {
        auto compositeNode = compositeLayerUtils->GetCompositeNode();
        if (compositeNode) {
            compositeNode->RemoveFromTree();
            RSBaseNode::AddChild(compositeNode, index);
            return true;
        }
    }
    return false;
}

void RSNode::MoveChild(SharedPtr child, int index)
{
    if (child == nullptr || child->parent_.lock().get() != this) {
        ROSEN_LOGD("RSNode::MoveChild, not valid child");
        return;
    }
    NodeId childId = child->GetId();
    auto itr = std::find_if(
        children_.begin(), children_.end(), [&](WeakPtr& ptr) -> bool { return ROSEN_EQ<RSNode>(ptr, child); });
    if (itr == children_.end()) {
        ROSEN_LOGD("RSNode::MoveChild, not child");
        return;
    }
    children_.erase(itr);
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(child);
    } else {
        children_.insert(children_.begin() + index, child);
    }
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeMoveChild>(id_, childId, index);

    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    child->SetIsOnTheTree(isOnTheTree_);
}

void RSNode::RemoveChild(SharedPtr child)
{
    if (child == nullptr || child->parent_.lock().get() != this) {
        ROSEN_LOGI("RSNode::RemoveChild, child is nullptr");
        return;
    }
    NodeId childId = child->GetId();
    RemoveChildByNode(child);
    child->OnRemoveChildren();
    child->parent_.reset();
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

void RSNode::RemoveChildByNodeSelf(WeakPtr child)
{
    SharedPtr childPtr = child.lock();
    if (childPtr) {
        RemoveChild(childPtr);
    } else {
        ROSEN_LOGE("RSNode::RemoveChildByNodeSelf, childId not found");
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
        children_.push_back(child);
    } else {
        children_.insert(children_.begin() + index, child);
    }
    child->SetParent(weak_from_this());
    child->OnAddChildren();
    child->MarkDirty(NodeDirtyType::APPEARANCE, true);
    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddCrossParentChild>(id_, childId, index);

    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    child->SetIsOnTheTree(isOnTheTree_);
}

void RSNode::RemoveCrossParentChild(SharedPtr child, SharedPtr newParent)
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
    RemoveChildByNode(child);
    child->OnRemoveChildren();
    child->SetParent(newParent);
    child->MarkDirty(NodeDirtyType::APPEARANCE, true);

    // construct command using child's GetHierarchyCommandNodeId(), not GetId()
    NodeId childId = child->GetHierarchyCommandNodeId();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSBaseNodeRemoveCrossParentChild>(id_, childId, newParent->GetId());
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
}

void RSNode::SetIsCrossNode(bool isCrossNode)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSBaseNodeSetIsCrossNode>(GetId(), isCrossNode);
    AddCommand(command);
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

void RSNode::RemoveChildByNode(SharedPtr child)
{
    if (child == nullptr) {
        RS_LOGE("RSNode::RemoveChildByNode %{public}" PRIu64 " failed:nullptr", GetId());
        return;
    }
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    auto itr = std::find_if(
        children_.begin(), children_.end(), [&](WeakPtr &ptr) -> bool {return ROSEN_EQ<RSNode>(ptr, child);});
    if (itr != children_.end()) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "RSNode::RemoveChildByNode parent:%" PRIu64 ", child:%" PRIu64 "", GetId(), child->GetId());
        children_.erase(itr);
    } else {
        RS_TRACE_NAME_FMT(
            "RSNode::RemoveChildByNode failed:%" PRIu64 " not children of %" PRIu64 "", child->GetId(), GetId());
    }
}

void RSNode::RemoveFromTree()
{
    CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
    RS_OPTIONAL_TRACE_NAME_FMT("RSNode::RemoveFromTree id:%" PRIu64 "", GetId());
    MarkDirty(NodeDirtyType::APPEARANCE, true);
    auto parentPtr = parent_.lock();
    if (parentPtr) {
        parentPtr->RemoveChildByNode(shared_from_this());
        OnRemoveChildren();
        parent_.reset();
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
    RS_OPTIONAL_TRACE_NAME_FMT("RSNode::ClearChildren id:%" PRIu64 "", GetId());
    for (auto child : children_) {
        auto childPtr = child.lock();
        if (childPtr) {
            childPtr->SetIsOnTheTree(false);
            childPtr->parent_.reset();
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
        auto childPtr = children_[index].lock();
        if (childPtr) {
            childPtr->SyncTextureExport(isTextureExportNode);
            std::unique_ptr<RSCommand> command =
                std::make_unique<RSBaseNodeAddChild>(id_, childPtr->GetHierarchyCommandNodeId(), index);
            AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
        }
    }
}

RSNode::SharedPtr RSNode::GetChildByIndex(int index) const
{
    int childrenTotal = static_cast<int>(children_.size());
    if (childrenTotal <= 0 || index < -1 || index >= childrenTotal) {
        return nullptr;
    }
    if (index == -1) {
        return children_.back().lock();
    }
    return children_.at(index).lock();
}

void RSNode::SetParent(WeakPtr parent)
{
    parent_ = parent;
}

RSNode::SharedPtr RSNode::GetParent()
{
    return parent_.lock();
}

void RSNode::DumpTree(int depth, std::string& out) const
{
    for (int i = 0; i < depth; i++) {
        out += "  ";
    }
    out += "| ";
    Dump(out);
    for (auto child : children_) {
        auto childPtr = child.lock();
        if (childPtr) {
            out += "\n";
            childPtr->DumpTree(depth + 1, out);
        }
    }
}

void RSNode::DumpModifiers(std::string& out) const
{
#if defined(MODIFIER_NG)
    const auto& modifiers = modifiersNG_;
#else
    const auto& modifiers = modifiers_;
#endif
    for (const auto& [id, modifier] : modifiers) {
        if (modifier == nullptr) {
            continue;
        }
        auto renderModifier = modifier->CreateRenderModifier();
        if (renderModifier == nullptr) {
            continue;
        }
#if defined(MODIFIER_NG)
        renderModifier->Dump(out, ",");
#else
        out += " " + modifier->GetModifierTypeString();
        out += ":";
        renderModifier->Dump(out);
#endif
    }
}

void RSNode::Dump(std::string& out) const
{
    auto iter = RSUINodeTypeStrs.find(GetType());
    auto rsUIContextPtr = rsUIContext_.lock();
    out += (iter != RSUINodeTypeStrs.end() ? iter->second : "RSNode");
    out += "[" + std::to_string(id_);
    out += "], parent[" + std::to_string(parent_.lock() ? parent_.lock()->GetId() : -1);
    out += "], instanceId[" + std::to_string(instanceId_);
    out += "], UIContext[" + (rsUIContextPtr ? std::to_string(rsUIContextPtr->GetToken()) : "null");
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
    out += "], hybridRenderCanvas[";
    out += hybridRenderCanvas_ ? "true" : "false";
    out += "], animations[";
    for (const auto& [id, anim] : animations_) {
        out += "{id:" + std::to_string(id);
        out += " propId:" + std::to_string(anim->GetPropertyId());
        out += "} ";
    }
    if (!animations_.empty()) {
        out.pop_back();
    }
    
    out += "], modifiers[";
    DumpModifiers(out);
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
        ss << std::to_string(child.lock() ? child.lock()->GetId() : -1) << " ";
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
    auto rsUIContextPtr = rsUIContext_.lock();
    ss << " token:" << (rsUIContextPtr ? std::to_string(rsUIContextPtr->GetToken()) : "null");
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

void RSNode::SetUIContextToken()
{
    if (GetRSUIContext()) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSetUIContextToken>(id_, GetRSUIContext()->GetToken());
        AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    }
}

DrawNodeChangeCallback RSNode::drawNodeChangeCallback_ = nullptr;
void RSNode::SetDrawNodeChangeCallback(DrawNodeChangeCallback callback)
{
    if (drawNodeChangeCallback_) {
        return;
    }
    drawNodeChangeCallback_ = callback;
}

PropertyNodeChangeCallback RSNode::propertyNodeChangeCallback_ = nullptr;

// Sets the callback function for property node change events.
void RSNode::SetPropertyNodeChangeCallback(PropertyNodeChangeCallback callback)
{
    if (propertyNodeChangeCallback_) {
        return;
    }
    propertyNodeChangeCallback_ = callback;
}

#if defined(MODIFIER_NG)
void RSNode::AddModifier(const std::shared_ptr<ModifierNG::RSModifier> modifier)
{
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        if (modifier == nullptr) {
            RS_LOGE("RSNode::AddModifier: null modifier, nodeId=%{public}" PRIu64, GetId());
            return;
        }
        if (modifiersNG_.count(modifier->GetId())) {
            return;
        }
        modifier->OnAttach(*this); // Attach properties of modifier here
        if (modifier->GetType() == ModifierNG::RSModifierType::NODE_MODIFIER) {
            return;
        }
        if (modifier->GetType() != ModifierNG::RSModifierType::BOUNDS &&
            modifier->GetType() != ModifierNG::RSModifierType::FRAME &&
            modifier->GetType() != ModifierNG::RSModifierType::BACKGROUND_COLOR &&
            modifier->GetType() != ModifierNG::RSModifierType::ALPHA) {
            SetDrawNode();
            SetDrawNodeType(DrawNodeType::DrawPropertyType);
            if (modifier->GetType() == ModifierNG::RSModifierType::TRANSFORM) {
                SetDrawNodeType(DrawNodeType::GeometryPropertyType);
            }
        }
        NotifyPageNodeChanged();
        modifiersNG_.emplace(modifier->GetId(), modifier);
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSAddModifierNG>(GetId(), modifier->CreateRenderModifier());
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
    if (NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> cmdForRemote =
            std::make_unique<RSAddModifierNG>(GetId(), modifier->CreateRenderModifier());
        AddCommand(cmdForRemote, true, GetFollowType(), GetId());
    }
}

void RSNode::RemoveModifier(const std::shared_ptr<ModifierNG::RSModifier> modifier)
{
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        CHECK_FALSE_RETURN(CheckMultiThreadAccess(__func__));
        if (modifier == nullptr || !modifiersNG_.count(modifier->GetId())) {
            RS_LOGE("RSNode::RemoveModifier: null modifier or modifier not exist.");
            return;
        }
        modifiersNG_.erase(modifier->GetId());
    }
    modifier->OnDetach(); // Detach properties of modifier here
    DetachUIFilterProperties(modifier);
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSRemoveModifierNG>(GetId(), modifier->GetType(), modifier->GetId());
    AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
    if (NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> cmdForRemote =
            std::make_unique<RSRemoveModifierNG>(GetId(), modifier->GetType(), modifier->GetId());
        AddCommand(cmdForRemote, true, GetFollowType(), GetId());
    }
}
#else
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
        modifier->AttachToNode(shared_from_this());
        modifiers_.emplace(modifier->GetPropertyId(), modifier);
        modifiersTypeMap_.emplace((int16_t)modifier->GetModifierType(), modifier);
    }
    if (modifier->GetModifierType() == RSModifierType::NODE_MODIFIER) {
        return;
    }
    NotifyPageNodeChanged();
    CheckModifierType(modifier->GetModifierType());
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
#endif

void RSNode::DetachUIFilterProperties(const std::shared_ptr<ModifierNG::RSModifier>& modifier)
{
    std::shared_ptr<RSProperty<std::shared_ptr<RSUIFilter>>> property = nullptr;
    if (modifier->GetType() == ModifierNG::RSModifierType::FOREGROUND_FILTER) {
        property = std::static_pointer_cast<RSProperty<std::shared_ptr<RSUIFilter>>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER));
    } else if (modifier->GetType() == ModifierNG::RSModifierType::BACKGROUND_FILTER) {
        property = std::static_pointer_cast<RSProperty<std::shared_ptr<RSUIFilter>>>(
            modifier->GetProperty(ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER));
    }
    if (!property) {
        return;
    }
    auto uiFilter = property->Get();
    if (!uiFilter) {
        return;
    }
    for (auto type : uiFilter->GetUIFilterTypes()) {
        auto paraGroup = uiFilter->GetUIFilterPara(type);
        if (!paraGroup) {
            continue;
        }
        for (auto& prop : paraGroup->GetLeafProperties()) {
            if (!prop) {
                continue;
            }
            prop->target_.reset();
            UnregisterProperty(prop->GetId());
        }
    }
}

const std::shared_ptr<RSPropertyBase> RSNode::GetPropertyById(const PropertyId& propertyId)
{
    std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
    CHECK_FALSE_RETURN_VALUE(CheckMultiThreadAccess(__func__), nullptr);
    auto iter = properties_.find(propertyId);
    if (iter != properties_.end()) {
        return iter->second;
    }
    return {};
}

const std::shared_ptr<RSPropertyBase> RSNode::GetPropertyByType(
    const ModifierNG::RSModifierType& modifierType, const ModifierNG::RSPropertyType& propertyType)
{
    auto& modifier = modifiersNGCreatedBySetter_[static_cast<uint16_t>(modifierType)];
    if (!modifier) {
        return {};
    }
    return modifier->GetProperty(propertyType);
}
} // namespace Rosen
} // namespace OHOS
