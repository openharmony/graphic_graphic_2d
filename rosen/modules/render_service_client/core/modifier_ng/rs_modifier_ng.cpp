/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_ng/rs_modifier_ng.h"

#include "command/rs_node_command.h"
#include "modifier/rs_property.h"
#include "modifier_ng/appearance/rs_alpha_modifier.h"
#include "modifier_ng/appearance/rs_blend_modifier.h"
#include "modifier_ng/appearance/rs_env_foreground_color_modifier.h"
#include "modifier_ng/appearance/rs_foreground_filter_modifier.h"
#include "modifier_ng/geometry/rs_bounds_clip_modifier.h"
#include "modifier_ng/geometry/rs_bounds_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_modifier.h"
#include "modifier_ng/geometry/rs_frame_modifier.h"
#include "modifier_ng/rs_appearance_modifier.h"
#include "modifier_ng/rs_background_modifier.h"
#include "modifier_ng/rs_content_modifier.h"
#include "modifier_ng/rs_foreground_modifier.h"
#include "modifier_ng/rs_overlay_modifier.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "modifier_ng/rs_transition_modifier.h"
#include "platform/common/rs_log.h"
#include "ui/rs_node.h"

namespace OHOS::Rosen::ModifierNG {
std::array<RSModifier::Constructor, ModifierNG::MODIFIER_TYPE_COUNT>
    RSModifier::ConstructorLUT_ = {
        nullptr,                                            // INVALID
        nullptr,                                            // SAVE_ALL

        [] { return new RSTransformModifier(); },           // TRANSFORM
        [] { return new RSAlphaModifier(); },               // ALPHA
        nullptr,                                            // VISIBILITY

        // Bounds Geometry
        nullptr,                                            // MASK
        [] { return new RSTransitionStyleModifier(); },     // TRANSITION
        [] { return new RSEnvForegroundColorModifier(); },  // ENV_FOREGROUND_COLOR
        nullptr,                                            // SHADOW
        nullptr,                                            // BEGIN_FOREGROUND_FILTER
        nullptr,                                            // OUTLINE

        // BG properties in Bounds Clip
        nullptr,                                            // BG_SAVE_BOUNDS
        [] { return new RSBoundsModifier(); },              // BOUNDS
        [] { return new RSBoundsClipModifier(); },          // CLIP_TO_BOUNDS
        [] { return new RSBlendModifier(); },               // BLENDER
        [] { return new RSBackgroundColorModifier(); },     // BACKGROUND_COLOR
        [] { return new RSBackgroundShaderModifier(); },    // BACKGROUND_SHADER
        [] { return new RSBackgroundImageModifier(); },     // BACKGROUND_IMAGE
        nullptr,                                            // BEHIND_WINDOW_FILTER
        [] { return new RSBackgroundFilterModifier(); },    // BACKGROUND_FILTER
        nullptr,                                            // USE_EFFECT
        [] { return new RSBackgroundStyleModifier(); },     // BACKGROUND_STYLE
        nullptr,                                            // DYNAMIC_LIGHT_UP
        nullptr,                                            // BG_RESTORE_BOUNDS

        // Frame Geometry
        nullptr,                                            // SAVE_FRAME
        [] { return new RSFrameModifier(); },               // FRAME
        [] { return new RSFrameClipModifier(); },           // CLIP_TO_FRAME
        nullptr,                                            // CUSTOM_CLIP_TO_FRAME
        [] { return new RSContentStyleModifier(); },        // CONTENT_STYLE
        nullptr,                                            // CHILDREN
        nullptr,                                            // NODE_MODIFIER
        [] { return new RSForegroundStyleModifier(); },     // FOREGROUND_STYLE
        nullptr,                                            // RESTORE_FRAME

        // FG properties in Bounds clip
        nullptr,                                            // FG_SAVE_BOUNDS
        nullptr,                                            // FG_CLIP_TO_BOUNDS
        nullptr,                                            // BINARIZATION
        nullptr,                                            // COLOR_FILTER
        nullptr,                                            // LIGHT_UP_EFFECT
        nullptr,                                            // DYNAMIC_DIM
        [] { return new RSCompositingFilterModifier(); },   // COMPOSITING_FILTER
        nullptr,                                            // FOREGROUND_COLOR
        nullptr,                                            // FG_RESTORE_BOUNDS

        // No clip (unless ClipToBounds is set)
        nullptr,                                            // POINT_LIGHT
        [] { return new RSBorderModifier(); },              // BORDER
        [] { return new RSOverlayStyleModifier(); },        // OVERLAY_STYLE
        [] { return new RSParticleEffectModifier(); },      // PARTICLE_EFFECT
        nullptr,                                            // PIXEL_STRETCH

        // Restore state
        nullptr,                                            // RESTORE_BLENDER
        [] { return new RSForegroundFilterModifier(); },    // FOREGROUND_FILTER
        nullptr,                                            // RESTORE_ALL
    };

void RSModifier::AttachProperty(ModifierNG::RSPropertyType type, std::shared_ptr<RSPropertyBase> property)
{
    if (property == nullptr) {
        ROSEN_LOGE("Failed to attach property with type %d, property is null!", static_cast<int32_t>(type));
        return;
    }
    // replace existing property if any
    // TODO: maybe we should do this in a additive way?
    properties_[type] = property;
    // actually do the detach
    // property->AttachModifier(shared_from_this());

    auto node = node_.lock();
    if (!node) {
        // not attached yet
        return;
    }
    property->target_ = node_;

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSModifierNGAttachProperty>(
        node->GetId(), id_, GetType(), type, property->GetRenderProperty());
    transactionProxy->AddCommand(command, node->IsUniRenderEnabled());
}

void RSModifier::DetachProperty(ModifierNG::RSPropertyType type)
{
    auto it = properties_.find(type);
    if (it == properties_.end()) {
        return;
    }
    auto property = it->second;
    properties_.erase(it);
    // actually do the detach
    // property->DetachModifier(shared_from_this());
    property->target_.reset();

    auto node = node_.lock();
    if (!node) {
        // not attached yet
        return;
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSModifierNGDetachProperty>(node->GetId(), id_, GetType(), type);
    transactionProxy->AddCommand(command, node->IsUniRenderEnabled());
}

void RSModifier::OnAttach(RSNode& node)
{
    node_ = node.weak_from_this();
    for (auto& [_, property] : properties_) {
        property->target_ = node_;
    }
}

void RSModifier::OnDetach()
{
    node_.reset();
    for (auto& [_, property] : properties_) {
        property->target_.reset();
    }
}

void RSModifier::SetDirty(bool isDirty)
{
    // custom animation related logic
}

std::shared_ptr<RSRenderModifier> RSModifier::CreateRenderModifier()
{
    const auto& constructor = RSRenderModifier::ConstructorLUT_[static_cast<uint8_t>(GetType())];
    if (constructor == nullptr) {
        return nullptr;
    }
    auto rawPointer = constructor();
    if (rawPointer == nullptr) {
        return nullptr;
    }
    std::shared_ptr<RSRenderModifier> renderModifier(
        rawPointer, [](RSRenderModifier* rawPointer) { delete rawPointer; });
    // renderModifier.reset(rawPointer);
    renderModifier->id_ = id_;
    for (auto& [type, property] : properties_) {
        if (property == nullptr) {
            ROSEN_LOGE("RSModifier::CreateRenderModifier property is null! type: %d", static_cast<int32_t>(type));
            continue;
        }
        renderModifier->properties_.emplace(type, property->GetRenderProperty());
    }
    // this function is only called before marshalling, no need to actually attach properties
    return renderModifier;
}
} // namespace OHOS::Rosen::ModifierNG
