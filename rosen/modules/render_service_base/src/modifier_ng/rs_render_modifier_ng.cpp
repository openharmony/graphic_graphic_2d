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

#include "modifier_ng/rs_render_modifier_ng.h"

#include "recording/draw_cmd_list.h"
#include "rs_trace.h"

#include "modifier/rs_render_property.h"
#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "modifier_ng/appearance/rs_background_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_behind_window_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_color_picker_render_modifier.h"
#include "modifier_ng/appearance/rs_blend_render_modifier.h"
#include "modifier_ng/appearance/rs_border_render_modifier.h"
#include "modifier_ng/appearance/rs_compositing_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_dynamic_light_up_render_modifier.h"
#include "modifier_ng/appearance/rs_foreground_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_hdr_brightness_render_modifier.h"
#include "modifier_ng/appearance/rs_mask_render_modifier.h"
#include "modifier_ng/appearance/rs_material_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_outline_render_modifier.h"
#include "modifier_ng/appearance/rs_particle_effect_render_modifier.h"
#include "modifier_ng/appearance/rs_pixel_stretch_render_modifier.h"
#include "modifier_ng/appearance/rs_point_light_render_modifier.h"
#include "modifier_ng/appearance/rs_shadow_render_modifier.h"
#include "modifier_ng/appearance/rs_use_effect_render_modifier.h"
#include "modifier_ng/appearance/rs_visibility_render_modifier.h"
#include "modifier_ng/background/rs_background_color_render_modifier.h"
#include "modifier_ng/background/rs_background_image_render_modifier.h"
#include "modifier_ng/background/rs_background_ng_shader_render_modifier.h"
#include "modifier_ng/background/rs_background_shader_render_modifier.h"
#include "modifier_ng/foreground/rs_env_foreground_color_render_modifier.h"
#include "modifier_ng/foreground/rs_foreground_color_render_modifier.h"
#include "modifier_ng/foreground/rs_foreground_shader_render_modifier.h"
#include "modifier_ng/geometry/rs_bounds_clip_render_modifier.h"
#include "modifier_ng/geometry/rs_bounds_render_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_render_modifier.h"
#include "modifier_ng/geometry/rs_frame_render_modifier.h"
#include "modifier_ng/geometry/rs_transform_render_modifier.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen::ModifierNG {
// RSRenderModifier ==========================================================================
const RSRenderModifier::LegacyPropertyApplierMap RSRenderModifier::emptyLegacyPropertyApplierMap_;

static const std::unordered_map<RSModifierType, RSRenderModifier::ResetFunc> g_resetFuncLUT_ = {
    { RSModifierType::TRANSFORM,                RSTransformRenderModifier::ResetProperties },
    { RSModifierType::ALPHA,                    RSAlphaRenderModifier::ResetProperties },
    { RSModifierType::FOREGROUND_COLOR,         RSForegroundColorRenderModifier::ResetProperties },
    { RSModifierType::BACKGROUND_COLOR,         RSBackgroundColorRenderModifier::ResetProperties },
    { RSModifierType::BACKGROUND_SHADER,        RSBackgroundShaderRenderModifier::ResetProperties },
    { RSModifierType::BACKGROUND_IMAGE,         RSBackgroundImageRenderModifier::ResetProperties },
    { RSModifierType::BORDER,                   RSBorderRenderModifier::ResetProperties },
    { RSModifierType::OUTLINE,                  RSOutlineRenderModifier::ResetProperties },
    { RSModifierType::CLIP_TO_BOUNDS,           RSBoundsClipRenderModifier::ResetProperties },
    { RSModifierType::CLIP_TO_FRAME,            RSFrameClipRenderModifier::ResetProperties },
    { RSModifierType::VISIBILITY,               RSVisibilityRenderModifier::ResetProperties },
    { RSModifierType::DYNAMIC_LIGHT_UP,         RSDynamicLightUpRenderModifier::ResetProperties },
    { RSModifierType::SHADOW,                   RSShadowRenderModifier::ResetProperties },
    { RSModifierType::MASK,                     RSMaskRenderModifier::ResetProperties },
    { RSModifierType::PIXEL_STRETCH,            RSPixelStretchRenderModifier::ResetProperties },
    { RSModifierType::USE_EFFECT,               RSUseEffectRenderModifier::ResetProperties },
    { RSModifierType::BLENDER,                  RSBlendRenderModifier::ResetProperties },
    { RSModifierType::POINT_LIGHT,              RSPointLightRenderModifier::ResetProperties },
    { RSModifierType::PARTICLE_EFFECT,          RSParticleEffectRenderModifier::ResetProperties },
    { RSModifierType::COMPOSITING_FILTER,       RSCompositingFilterRenderModifier::ResetProperties },
    { RSModifierType::BACKGROUND_FILTER,        RSBackgroundFilterRenderModifier::ResetProperties },
    { RSModifierType::FOREGROUND_FILTER,        RSForegroundFilterRenderModifier::ResetProperties },
    { RSModifierType::BACKGROUND_NG_SHADER,     RSBackgroundNGShaderRenderModifier::ResetProperties },
    { RSModifierType::FOREGROUND_SHADER,        RSForegroundShaderRenderModifier::ResetProperties },
    { RSModifierType::COLOR_PICKER,             RSColorPickerRenderModifier::ResetProperties },
    { RSModifierType::BOUNDS,                   RSBoundsRenderModifier::ResetProperties },
    { RSModifierType::MATERIAL_FILTER,          RSMaterialFilterRenderModifier::ResetProperties },
};

std::array<RSRenderModifier::Constructor, MODIFIER_TYPE_COUNT> RSRenderModifier::ConstructorLUT_ = {
    nullptr,                                                                                     // INVALID
    [] { return std::make_shared<RSBoundsRenderModifier>(); },                                   // BOUNDS
    [] { return std::make_shared<RSFrameRenderModifier>(); },                                    // FRAME
    [] { return std::make_shared<RSTransformRenderModifier>(); },                                // TRANSFORM
    [] { return std::make_shared<RSAlphaRenderModifier>(); },                                    // ALPHA
    [] { return std::make_shared<RSForegroundColorRenderModifier>(); },                          // FOREGROUND_COLOR
    [] { return std::make_shared<RSBackgroundColorRenderModifier>(); },                          // BACKGROUND_COLOR
    [] { return std::make_shared<RSBackgroundShaderRenderModifier>(); },                         // BACKGROUND_SHADER
    [] { return std::make_shared<RSBackgroundImageRenderModifier>(); },                          // BACKGROUND_IMAGE
    [] { return std::make_shared<RSBorderRenderModifier>(); },                                   // BORDER
    [] { return std::make_shared<RSOutlineRenderModifier>(); },                                  // OUTLINE
    [] { return std::make_shared<RSBoundsClipRenderModifier>(); },                               // CLIP_TO_BOUNDS
    [] { return std::make_shared<RSFrameClipRenderModifier>(); },                                // CLIP_TO_FRAME
    [] { return std::make_shared<RSVisibilityRenderModifier>(); },                               // VISIBILITY
    [] { return std::make_shared<RSDynamicLightUpRenderModifier>(); },                           // DYNAMIC_LIGHT_UP
    [] { return std::make_shared<RSShadowRenderModifier>(); },                                   // SHADOW
    [] { return std::make_shared<RSMaskRenderModifier>(); },                                     // MASK
    [] { return std::make_shared<RSPixelStretchRenderModifier>(); },                             // PIXEL_STRETCH
    [] { return std::make_shared<RSUseEffectRenderModifier>(); },                                // USE_EFFECT
    [] { return std::make_shared<RSBlendRenderModifier>(); },                                    // BLENDER
    [] { return std::make_shared<RSPointLightRenderModifier>(); },                               // POINT_LIGHT
    [] { return std::make_shared<RSParticleEffectRenderModifier>(); },                           // PARTICLE_EFFECT
    [] { return std::make_shared<RSCompositingFilterRenderModifier>(); },                        // COMPOSITING_FILTER
    [] { return std::make_shared<RSBackgroundFilterRenderModifier>(); },                         // BACKGROUND_FILTER
    [] { return std::make_shared<RSForegroundFilterRenderModifier>(); },                         // FOREGROUND_FILTER
    [] { return std::make_shared<RSCustomRenderModifier<RSModifierType::TRANSITION_STYLE>>(); }, // TRANSITION_STYLE
    [] { return std::make_shared<RSCustomRenderModifier<RSModifierType::BACKGROUND_STYLE>>(); }, // BACKGROUND_STYLE
    [] { return std::make_shared<RSCustomRenderModifier<RSModifierType::CONTENT_STYLE>>(); },    // CONTENT_STYLE
    [] { return std::make_shared<RSCustomRenderModifier<RSModifierType::FOREGROUND_STYLE>>(); }, // FOREGROUND_STYLE
    [] { return std::make_shared<RSCustomRenderModifier<RSModifierType::OVERLAY_STYLE>>(); },    // OVERLAY_STYLE
    nullptr,                                                                                     // NODE_MODIFIER
    [] { return std::make_shared<RSEnvForegroundColorRenderModifier>(); },                       // ENV_FOREGROUND_COLOR
    [] { return std::make_shared<RSHDRBrightnessRenderModifier>(); },                            // HDR_BRIGHTNESS
    [] { return std::make_shared<RSBehindWindowFilterRenderModifier>(); },                       // BEHIND_WINDOW_FILTER
    [] { return std::make_shared<RSBackgroundNGShaderRenderModifier>(); },                       // BACKGROUND_NG_SHADER
    [] { return std::make_shared<RSForegroundShaderRenderModifier>(); },                         // FOREGROUND_SHADER
    [] { return std::make_shared<RSMaterialFilterRenderModifier>(); },                           // MATERIAL_FILTER
    [] { return std::make_shared<RSColorPickerRenderModifier>(); },                              // COLOR_PICKER
    nullptr,                                                                                     // CHILDREN
};

const std::unordered_map<RSModifierType, RSRenderModifier::ResetFunc>& RSRenderModifier::GetResetFuncMap()
{
    return g_resetFuncLUT_;
}

void RSRenderModifier::AttachProperty(RSPropertyType type, const std::shared_ptr<RSRenderPropertyBase>& property)
{
    auto [it, isInserted] = properties_.try_emplace(type, property);
    if (!isInserted) {
        return;
    }
    if (auto node = target_.lock()) {
        property->Attach(*node, weak_from_this());
        property->UpdatePropertyUnitNG(type);
        node->SetDirty();
        node->AddDirtyType(GetType());
    }
    dirty_ = true;
}

void RSRenderModifier::DetachProperty(RSPropertyType type)
{
    auto it = properties_.find(type);
    if (it == properties_.end()) {
        return;
    }
    if (auto node = target_.lock()) {
        if (it->second) {
            it->second->Detach();
        }
        node->SetDirty();
        node->AddDirtyType(GetType());
    }
    properties_.erase(it);
    dirty_ = true;
}

void RSRenderModifier::ApplyLegacyProperty(RSProperties& properties)
{
    const auto& map = GetLegacyPropertyApplierMap();
    for (auto& [type, property] : properties_) {
        auto it = map.find(type);
        if (it == map.end()) {
            RS_LOGD("apply legacy property failed, type: %{public}d", static_cast<int>(type));
            continue;
        }
        if (it->second != nullptr) {
            it->second(properties, *property);
        }
    }
    dirty_ = false;
}

bool RSRenderModifier::IsAttached() const
{
    return !target_.expired();
}

void RSRenderModifier::OnAttachModifier(RSRenderNode& node)
{
    node.SetDirty();
    node.AddDirtyType(GetType());
    target_ = node.weak_from_this();
    for (auto& [type, property] : properties_) {
        property->Attach(node, weak_from_this());
        property->UpdatePropertyUnitNG(type);
    }
    dirty_ = true;
}

void RSRenderModifier::OnDetachModifier()
{
    auto node = target_.lock();
    target_.reset();
    if (node == nullptr) {
        return;
    }
    for (auto& [type, property] : properties_) {
        property->Detach();
    }
    node->SetDirty();
    node->AddDirtyType(GetType());
    dirty_ = true;
}

void RSRenderModifier::SetDirty()
{
    dirty_ = true;
    if (auto node = target_.lock()) {
        node->SetDirty();
        node->AddDirtyType(GetType());
        OnSetDirty();
    }
}

void RSRenderModifier::OnSetDirty()
{
    if (auto node = target_.lock()) {
        node->MarkNonGeometryChanged();
    }
}

bool RSRenderModifier::Marshalling(Parcel& parcel) const
{
    auto ret = RSMarshallingHelper::Marshalling(parcel, GetType());
    if (ret) {
        ret = RSMarshallingHelper::Marshalling(parcel, id_);
    }
    if (ret) {
        ret = DeduplicationMarshalling(parcel);
    }
    if (ret) {
        ret = RSMarshallingHelper::Marshalling(parcel, properties_);
    }
    return ret;
}

std::shared_ptr<RSRenderModifier> RSRenderModifier::Unmarshalling(Parcel& parcel)
{
    RSModifierType type = RSModifierType::MAX;
    if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
        return nullptr;
    }
    if (type >= RSModifierType::MAX) {
        RS_LOGE("%{public}s: type[%{public}u] out of limit.", __func__, static_cast<uint16_t>(type));
        return nullptr;
    }
    auto constructor = ConstructorLUT_[static_cast<uint16_t>(type)];
    if (constructor == nullptr) {
        return nullptr;
    }
    auto ret = constructor();
    if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, ret->id_)) {
        return nullptr;
    }
    if (!ret->DeduplicationUnmarshalling(parcel)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, ret->properties_)) {
        return nullptr;
    }
    for (auto& [type, property] : ret->properties_) {
        // Attach property to RSRenderModifier
        ret->AttachProperty(type, property);
    }
    return ret;
}

// only use in dump without consideration of time performance
RSPropertyType RSRenderModifier::FindPropertyType(const std::shared_ptr<RSRenderPropertyBase> target) const
{
    for (auto& [type, property] : properties_) {
        if (property == target) {
            return type;
        }
    }
    return RSPropertyType::INVALID;
}

// RSCustomRenderModifier ===========================================================================
template<RSModifierType T>
void RSCustomRenderModifier<T>::Apply(RSPaintFilterCanvas* canvas, RSProperties& properties)
{
    auto propertyType = ModifierTypeConvertor::GetPropertyType(GetType());
    if (HasProperty(propertyType) && canvas) {
        auto cmds = Getter<Drawing::DrawCmdListPtr>(propertyType, nullptr);
        RSPropertiesPainter::DrawFrame(properties, *canvas, cmds);
    }
}

template<RSModifierType T>
void RSCustomRenderModifier<T>::OnSetDirty()
{
    if (auto node = target_.lock()) {
        node->MarkNonGeometryChanged();
        node->SetContentDirty();
    }
}

template class RSCustomRenderModifier<RSModifierType::TRANSITION_STYLE>;
template class RSCustomRenderModifier<RSModifierType::BACKGROUND_STYLE>;
template class RSCustomRenderModifier<RSModifierType::CONTENT_STYLE>;
template class RSCustomRenderModifier<RSModifierType::FOREGROUND_STYLE>;
template class RSCustomRenderModifier<RSModifierType::OVERLAY_STYLE>;
} // namespace OHOS::Rosen::ModifierNG
