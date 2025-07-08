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

#include "sandbox_utils.h"
#include "ui_effect/property/include/rs_ui_filter.h"

#include "command/rs_node_command.h"
#include "modifier/rs_property.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "platform/common/rs_log.h"
#include "ui/rs_node.h"

namespace OHOS::Rosen::ModifierNG {
constexpr int PID_SHIFT = 32;

static const std::unordered_map<RSPropertyType, ThresholdType> g_propertyTypeToThresholdTypeMap = {
    { RSPropertyType::INVALID, ThresholdType::DEFAULT },
    { RSPropertyType::BOUNDS, ThresholdType::LAYOUT },
    { RSPropertyType::FRAME, ThresholdType::LAYOUT },
    { RSPropertyType::POSITION_Z, ThresholdType::LAYOUT },
    { RSPropertyType::POSITION_Z_APPLICABLE_CAMERA3D, ThresholdType::ZERO },
    { RSPropertyType::PIVOT, ThresholdType::FINE },
    { RSPropertyType::PIVOT_Z, ThresholdType::FINE },
    { RSPropertyType::QUATERNION, ThresholdType::FINE },
    { RSPropertyType::ROTATION, ThresholdType::FINE },
    { RSPropertyType::ROTATION_X, ThresholdType::FINE },
    { RSPropertyType::ROTATION_Y, ThresholdType::FINE },
    { RSPropertyType::CAMERA_DISTANCE, ThresholdType::COARSE },
    { RSPropertyType::SCALE, ThresholdType::FINE },
    { RSPropertyType::SCALE_Z, ThresholdType::FINE },
    { RSPropertyType::SKEW, ThresholdType::FINE },
    { RSPropertyType::PERSP, ThresholdType::FINE },
    { RSPropertyType::TRANSLATE, ThresholdType::LAYOUT },
    { RSPropertyType::TRANSLATE_Z, ThresholdType::LAYOUT },
    { RSPropertyType::SUBLAYER_TRANSFORM, ThresholdType::DEFAULT },
    { RSPropertyType::CORNER_RADIUS, ThresholdType::MEDIUM },
    { RSPropertyType::ALPHA, ThresholdType::COARSE },
    { RSPropertyType::ALPHA_OFFSCREEN, ThresholdType::ZERO },
    { RSPropertyType::FOREGROUND_COLOR, ThresholdType::COLOR },
    { RSPropertyType::BACKGROUND_COLOR, ThresholdType::COLOR },
    { RSPropertyType::BACKGROUND_SHADER, ThresholdType::ZERO },
    { RSPropertyType::BACKGROUND_SHADER_PROGRESS, ThresholdType::FINE },
    { RSPropertyType::BG_IMAGE, ThresholdType::ZERO },
    { RSPropertyType::BG_IMAGE_INNER_RECT, ThresholdType::LAYOUT },
    { RSPropertyType::BG_IMAGE_WIDTH, ThresholdType::LAYOUT },
    { RSPropertyType::BG_IMAGE_HEIGHT, ThresholdType::LAYOUT },
    { RSPropertyType::BG_IMAGE_POSITION_X, ThresholdType::LAYOUT },
    { RSPropertyType::BG_IMAGE_POSITION_Y, ThresholdType::LAYOUT },
    { RSPropertyType::BG_IMAGE_RECT, ThresholdType::DEFAULT },
    { RSPropertyType::BORDER_COLOR, ThresholdType::COLOR },
    { RSPropertyType::BORDER_WIDTH, ThresholdType::LAYOUT },
    { RSPropertyType::BORDER_STYLE, ThresholdType::ZERO },
    { RSPropertyType::BORDER_DASH_WIDTH, ThresholdType::ZERO },
    { RSPropertyType::BORDER_DASH_GAP, ThresholdType::ZERO },
    { RSPropertyType::BACKGROUND_FILTER, ThresholdType::DEFAULT },
    { RSPropertyType::LINEAR_GRADIENT_BLUR_PARA, ThresholdType::ZERO },
    { RSPropertyType::DYNAMIC_LIGHT_UP_RATE, ThresholdType::COARSE },
    { RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE, ThresholdType::COARSE },
    { RSPropertyType::FG_BRIGHTNESS_RATES, ThresholdType::COARSE },
    { RSPropertyType::FG_BRIGHTNESS_SATURATION, ThresholdType::COARSE },
    { RSPropertyType::FG_BRIGHTNESS_POSCOEFF, ThresholdType::COARSE },
    { RSPropertyType::FG_BRIGHTNESS_NEGCOEFF, ThresholdType::COARSE },
    { RSPropertyType::FG_BRIGHTNESS_FRACTION, ThresholdType::COARSE },
    { RSPropertyType::FG_BRIGHTNESS_HDR, ThresholdType::ZERO },
    { RSPropertyType::BG_BRIGHTNESS_RATES, ThresholdType::COARSE },
    { RSPropertyType::BG_BRIGHTNESS_SATURATION, ThresholdType::COARSE },
    { RSPropertyType::BG_BRIGHTNESS_POSCOEFF, ThresholdType::COARSE },
    { RSPropertyType::BG_BRIGHTNESS_NEGCOEFF, ThresholdType::COARSE },
    { RSPropertyType::BG_BRIGHTNESS_FRACTION, ThresholdType::COARSE },
    { RSPropertyType::FRAME_GRAVITY, ThresholdType::ZERO },
    { RSPropertyType::CLIP_RRECT, ThresholdType::MEDIUM },
    { RSPropertyType::CLIP_BOUNDS, ThresholdType::ZERO },
    { RSPropertyType::CLIP_TO_BOUNDS, ThresholdType::ZERO },
    { RSPropertyType::CLIP_TO_FRAME, ThresholdType::ZERO },
    { RSPropertyType::VISIBLE, ThresholdType::ZERO },
    { RSPropertyType::SHADOW_COLOR, ThresholdType::COLOR },
    { RSPropertyType::SHADOW_OFFSET_X, ThresholdType::LAYOUT },
    { RSPropertyType::SHADOW_OFFSET_Y, ThresholdType::LAYOUT },
    { RSPropertyType::SHADOW_ALPHA, ThresholdType::COARSE },
    { RSPropertyType::SHADOW_ELEVATION, ThresholdType::COARSE },
    { RSPropertyType::SHADOW_RADIUS, ThresholdType::LAYOUT },
    { RSPropertyType::SHADOW_PATH, ThresholdType::ZERO },
    { RSPropertyType::SHADOW_MASK, ThresholdType::ZERO },
    { RSPropertyType::SHADOW_COLOR_STRATEGY, ThresholdType::ZERO },
    { RSPropertyType::MASK, ThresholdType::ZERO },
    { RSPropertyType::SPHERIZE, ThresholdType::MEDIUM },
    { RSPropertyType::LIGHT_UP_EFFECT_DEGREE, ThresholdType::COARSE },
    { RSPropertyType::PIXEL_STRETCH_SIZE, ThresholdType::COARSE },
    { RSPropertyType::PIXEL_STRETCH_PERCENT, ThresholdType::FINE },
    { RSPropertyType::PIXEL_STRETCH_TILE_MODE, ThresholdType::ZERO },
    { RSPropertyType::USE_EFFECT, ThresholdType::ZERO },
    { RSPropertyType::USE_EFFECT_TYPE, ThresholdType::ZERO },
    { RSPropertyType::COLOR_BLEND_MODE, ThresholdType::ZERO },
    { RSPropertyType::COLOR_BLEND_APPLY_TYPE, ThresholdType::ZERO },
    { RSPropertyType::SANDBOX, ThresholdType::LAYOUT },
    { RSPropertyType::GRAY_SCALE, ThresholdType::COARSE },
    { RSPropertyType::BRIGHTNESS, ThresholdType::COARSE },
    { RSPropertyType::CONTRAST, ThresholdType::COARSE },
    { RSPropertyType::SATURATE, ThresholdType::COARSE },
    { RSPropertyType::SEPIA, ThresholdType::COARSE },
    { RSPropertyType::INVERT, ThresholdType::COARSE },
    { RSPropertyType::AIINVERT, ThresholdType::COARSE },
    { RSPropertyType::SYSTEMBAREFFECT, ThresholdType::ZERO },
    { RSPropertyType::WATER_RIPPLE_PROGRESS, ThresholdType::COARSE },
    { RSPropertyType::WATER_RIPPLE_PARAMS, ThresholdType::ZERO },
    { RSPropertyType::HUE_ROTATE, ThresholdType::COARSE },
    { RSPropertyType::COLOR_BLEND, ThresholdType::COLOR },
    { RSPropertyType::PARTICLE, ThresholdType::ZERO },
    { RSPropertyType::SHADOW_IS_FILLED, ThresholdType::ZERO },
    { RSPropertyType::OUTLINE_COLOR, ThresholdType::COLOR },
    { RSPropertyType::OUTLINE_WIDTH, ThresholdType::LAYOUT },
    { RSPropertyType::OUTLINE_STYLE, ThresholdType::ZERO },
    { RSPropertyType::OUTLINE_DASH_WIDTH, ThresholdType::ZERO },
    { RSPropertyType::OUTLINE_DASH_GAP, ThresholdType::ZERO },
    { RSPropertyType::OUTLINE_RADIUS, ThresholdType::MEDIUM },
    { RSPropertyType::GREY_COEF, ThresholdType::COARSE },
    { RSPropertyType::LIGHT_INTENSITY, ThresholdType::COARSE },
    { RSPropertyType::LIGHT_COLOR, ThresholdType::COARSE },
    { RSPropertyType::LIGHT_POSITION, ThresholdType::LAYOUT },
    { RSPropertyType::ILLUMINATED_BORDER_WIDTH, ThresholdType::COARSE },
    { RSPropertyType::ILLUMINATED_TYPE, ThresholdType::ZERO },
    { RSPropertyType::BLOOM, ThresholdType::COARSE },
    { RSPropertyType::FOREGROUND_EFFECT_RADIUS, ThresholdType::COARSE },
    { RSPropertyType::USE_SHADOW_BATCHING, ThresholdType::ZERO },
    { RSPropertyType::MOTION_BLUR_PARA, ThresholdType::ZERO },
    { RSPropertyType::PARTICLE_EMITTER_UPDATER, ThresholdType::ZERO },
    { RSPropertyType::PARTICLE_NOISE_FIELD, ThresholdType::ZERO },
    { RSPropertyType::FLY_OUT_DEGREE, ThresholdType::COARSE },
    { RSPropertyType::FLY_OUT_PARAMS, ThresholdType::ZERO },
    { RSPropertyType::DISTORTION_K, ThresholdType::COARSE },
    { RSPropertyType::DYNAMIC_DIM_DEGREE, ThresholdType::COARSE },
    { RSPropertyType::MAGNIFIER_PARA, ThresholdType::ZERO },
    { RSPropertyType::BACKGROUND_BLUR_RADIUS, ThresholdType::COARSE },
    { RSPropertyType::BACKGROUND_BLUR_SATURATION, ThresholdType::COARSE },
    { RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS, ThresholdType::COARSE },
    { RSPropertyType::BACKGROUND_BLUR_MASK_COLOR, ThresholdType::COLOR },
    { RSPropertyType::BACKGROUND_BLUR_COLOR_MODE, ThresholdType::ZERO },
    { RSPropertyType::BACKGROUND_BLUR_RADIUS_X, ThresholdType::COARSE },
    { RSPropertyType::BACKGROUND_BLUR_RADIUS_Y, ThresholdType::COARSE },
    { RSPropertyType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION, ThresholdType::ZERO },
    { RSPropertyType::ALWAYS_SNAPSHOT, ThresholdType::ZERO },
    { RSPropertyType::FOREGROUND_BLUR_RADIUS, ThresholdType::COARSE },
    { RSPropertyType::FOREGROUND_BLUR_SATURATION, ThresholdType::COARSE },
    { RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS, ThresholdType::COARSE },
    { RSPropertyType::FOREGROUND_BLUR_MASK_COLOR, ThresholdType::COLOR },
    { RSPropertyType::FOREGROUND_BLUR_COLOR_MODE, ThresholdType::ZERO },
    { RSPropertyType::FOREGROUND_BLUR_RADIUS_X, ThresholdType::COARSE },
    { RSPropertyType::FOREGROUND_BLUR_RADIUS_Y, ThresholdType::COARSE },
    { RSPropertyType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION, ThresholdType::ZERO },
    { RSPropertyType::ATTRACTION_FRACTION, ThresholdType::MEDIUM },
    { RSPropertyType::ATTRACTION_DSTPOINT, ThresholdType::MEDIUM },
    { RSPropertyType::CUSTOM, ThresholdType::DEFAULT },
    { RSPropertyType::TRANSITION_STYLE, ThresholdType::DEFAULT },
    { RSPropertyType::BACKGROUND_STYLE, ThresholdType::DEFAULT },
    { RSPropertyType::CONTENT_STYLE, ThresholdType::DEFAULT },
    { RSPropertyType::FOREGROUND_STYLE, ThresholdType::DEFAULT },
    { RSPropertyType::OVERLAY_STYLE, ThresholdType::DEFAULT },
    { RSPropertyType::NODE_MODIFIER, ThresholdType::DEFAULT },
    { RSPropertyType::ENV_FOREGROUND_COLOR, ThresholdType::DEFAULT },
    { RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY, ThresholdType::DEFAULT },
    { RSPropertyType::CUSTOM_CLIP_TO_FRAME, ThresholdType::DEFAULT },
    { RSPropertyType::HDR_BRIGHTNESS, ThresholdType::DEFAULT },
    { RSPropertyType::HDR_UI_BRIGHTNESS, ThresholdType::COARSE },
    { RSPropertyType::HDR_BRIGHTNESS_FACTOR, ThresholdType::COARSE },
    { RSPropertyType::BACKGROUND_UI_FILTER, ThresholdType::DEFAULT },
    { RSPropertyType::FOREGROUND_UI_FILTER, ThresholdType::DEFAULT },
    { RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS, ThresholdType::COARSE },
    { RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION, ThresholdType::COARSE },
    { RSPropertyType::BEHIND_WINDOW_FILTER_BRIGHTNESS, ThresholdType::COARSE },
    { RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR, ThresholdType::COARSE },
    { RSPropertyType::CHILDREN, ThresholdType::DEFAULT }
};

ModifierId RSModifier::GenerateModifierId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 1;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        RS_LOGE("Modifier Id overflow");
    }

    return ((ModifierId)pid_ << PID_SHIFT) | currentId;
}

void RSModifier::AttachProperty(const std::shared_ptr<RSPropertyBase>& property)
{
    if (property == nullptr) {
        RS_LOGE("Failed to attach property, property is null.");
        return;
    }
    property->SetIsCustom(IsCustom());
    property->AttachModifier(shared_from_this());
    if (node_.expired()) {
        return;
    }
    property->target_ = node_;
    if (IsCustom()) {
        property->MarkCustomModifierDirty();
    }
}

void RSModifier::AttachProperty(RSPropertyType type, std::shared_ptr<RSPropertyBase> property)
{
    if (property == nullptr) {
        RS_LOGE("Failed to attach property with type %{public}d, property is null!", static_cast<int32_t>(type));
        return;
    }

    std::shared_ptr<RSRenderPropertyBase> renderProperty = nullptr;
    bool isUIFilter = (type == RSPropertyType::FOREGROUND_UI_FILTER || type == RSPropertyType::BACKGROUND_UI_FILTER);
    if (isUIFilter) {
        auto id = property->GetId();
        auto filterProperty = std::static_pointer_cast<RSProperty<std::shared_ptr<RSUIFilter>>>(property);
        if (filterProperty) {
            auto uiFilter = filterProperty->Get();
            renderProperty = uiFilter->CreateRenderProperty(id);
            if (!renderProperty) {
                return;
            }
        }
    } else {
        renderProperty = property->GetRenderProperty();
    }

    property->SetPropertyTypeNG(type);
    // replace existing property if any
    properties_[type] = property;
    SetPropertyThresholdType(type, property);

    auto node = node_.lock();
    if (!node) {
        // not attached yet
        return;
    }
    auto shouldSetOption = node->motionPathOption_ != nullptr && property->IsPathAnimatable();
    if (shouldSetOption) {
        property->SetMotionPathOption(node->motionPathOption_);
    }
    property->Attach(*node, weak_from_this());
    MarkNodeDirty();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSModifierNGAttachProperty>(node->GetId(), id_, GetType(), type, renderProperty);
    node->AddCommand(command, node->IsRenderServiceNode(), node->GetFollowType(), node->GetId());
    if (node->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> cmdForRemote =
            std::make_unique<RSModifierNGAttachProperty>(node->GetId(), id_, GetType(), type, renderProperty);
        node->AddCommand(cmdForRemote, true, node->GetFollowType(), node->GetId());
    }
}

void RSModifier::DetachProperty(RSPropertyType type)
{
    auto it = properties_.find(type);
    if (it == properties_.end()) {
        return;
    }
    auto property = it->second;
    properties_.erase(it);
    // actually do the detach
    property->Detach();
    auto node = node_.lock();
    if (!node) {
        // not attached yet
        return;
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSModifierNGDetachProperty>(node->GetId(), id_, GetType(), type);
    node->AddCommand(command, node->IsRenderServiceNode());
}

void RSModifier::SetPropertyThresholdType(RSPropertyType type, std::shared_ptr<RSPropertyBase> property)
{
    if (!g_propertyTypeToThresholdTypeMap.count(type)) {
        RS_LOGE("RSPropertyType is not exist! type: %{public}d", static_cast<int32_t>(type));
        return;
    }
    if (g_propertyTypeToThresholdTypeMap.at(type) != ThresholdType::DEFAULT) {
        property->SetThresholdType(g_propertyTypeToThresholdTypeMap.at(type));
    }
}

void RSModifier::OnAttach(RSNode& node)
{
    node_ = node.weak_from_this();
    if (properties_.empty()) {
        return;
    }
    auto weakPtr = weak_from_this();
    for (auto& [_, property] : properties_) {
        auto shouldSetOption = node.motionPathOption_ != nullptr && property->IsPathAnimatable();
        if (shouldSetOption) {
            property->SetMotionPathOption(node.motionPathOption_);
        }
        property->Attach(node, weakPtr);
    }
    MarkNodeDirty();
}

void RSModifier::OnDetach()
{
    node_.reset();
    for (auto& [_, property] : properties_) {
        property->Detach();
    }
    MarkNodeDirty();
}

void RSModifier::SetDirty(bool isDirty, const std::shared_ptr<RSModifierManager>& modifierManager)
{
#if defined(MODIFIER_NG)
    if (isDirty_ == isDirty) {
        // not changed
        return;
    }
    isDirty_ = isDirty;

    if (!isDirty_) {
        // not dirty
        return;
    }

    if (modifierManager == nullptr) {
        RS_LOGE("multi-instance, Modifier manager is null while mark modifier dirty Id: %{public}" PRIu64 "!", GetId());
        return;
    }
    modifierManager->AddModifier(shared_from_this());
#endif
}

std::shared_ptr<RSRenderModifier> RSModifier::CreateRenderModifier()
{
    const auto& constructor = RSRenderModifier::ConstructorLUT_[static_cast<uint16_t>(GetType())];
    if (constructor == nullptr) {
        return nullptr;
    }
    auto rawPointer = constructor();
    if (rawPointer == nullptr) {
        return nullptr;
    }
    std::shared_ptr<RSRenderModifier> renderModifier(
        rawPointer, [](RSRenderModifier* rawPointer) { delete rawPointer; });
    renderModifier->id_ = id_;
    for (auto& [type, property] : properties_) {
        if (property == nullptr) {
            RS_LOGE("RSModifier::CreateRenderModifier property is null! type: %{public}d", static_cast<int32_t>(type));
            continue;
        }
        renderModifier->properties_.emplace(type, property->GetRenderProperty());
    }
    // this function is only called before marshalling, no need to actually attach properties
    return renderModifier;
}
} // namespace OHOS::Rosen::ModifierNG
