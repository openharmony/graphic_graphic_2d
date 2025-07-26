/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "modifier/rs_showing_properties_freezer.h"

#include <securec.h>

#include "modifier/rs_modifier_type.h"
#include "modifier/rs_property_modifier.h"
#include "modifier_ng/rs_modifier_ng.h"
#include "pipeline/rs_node_map.h"
#include "ui/rs_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSShowingPropertiesFreezer::RSShowingPropertiesFreezer(NodeId id, std::shared_ptr<RSUIContext> rsUIContext)
    : id_(id), rsUIContext_(rsUIContext)
{}

template<typename T, RSModifierType Type>
std::optional<T> RSShowingPropertiesFreezer::GetPropertyImpl() const
{
    auto rsUIContextPtr = rsUIContext_.lock();
    auto node = rsUIContextPtr ? rsUIContextPtr->GetNodeMap().GetNode<RSNode>(id_)
                               : RSNodeMap::Instance().GetNode<RSNode>(id_);
    if (node == nullptr) {
        return std::nullopt;
    }
    std::unique_lock<std::recursive_mutex> lock(node->propertyMutex_);
    auto iter = node->propertyModifiers_.find(Type);
    if (iter == node->propertyModifiers_.end()) {
        ROSEN_LOGE(
            "RSShowingPropertiesFreezer::GetPropertyImpl Type %{public}d failed, no such property!",
            static_cast<int>(Type));
        return std::nullopt;
    }
    auto property = std::static_pointer_cast<RSAnimatableProperty<T>>(iter->second->GetProperty());
    if (property == nullptr) {
        ROSEN_LOGE(
            "RSShowingPropertiesFreezer::GetPropertyImpl Type %{public}d failed, property is null!",
            static_cast<int>(Type));
        return std::nullopt;
    }
    bool success = property->GetShowingValueAndCancelAnimation();
    if (!success) {
        ROSEN_LOGE("RSShowingPropertiesFreezer::GetPropertyImpl Type %{public}d failed, cancel animation failed!",
            static_cast<int>(Type));
        return std::nullopt;
    }
    return property->Get();
}

template<typename T, ModifierNG::RSModifierType ModifierType, ModifierNG::RSPropertyType PropertyType>
std::optional<T> RSShowingPropertiesFreezer::GetPropertyImplNG() const
{
    auto context = rsUIContext_.lock();
    auto node = context ? context->GetNodeMap().GetNode<RSNode>(id_) : RSNodeMap::Instance().GetNode<RSNode>(id_);
    if (!node) {
        return std::nullopt;
    }
    std::unique_lock<std::recursive_mutex> lock(node->propertyMutex_);
    auto& modifier = node->modifiersNGCreatedBySetter_[static_cast<uint16_t>(ModifierType)];
    if (!modifier) {
        ROSEN_LOGE("RSShowingPropertiesFreezer::GetPropertyImplNG Type %{public}d failed, modifierNG is null!",
            static_cast<int>(ModifierType));
        return std::nullopt;
    }
    auto property = std::static_pointer_cast<RSAnimatableProperty<T>>(modifier->GetProperty(PropertyType));
    if (!property) {
        ROSEN_LOGE("RSShowingPropertiesFreezer::GetPropertyImplNG Type %{public}d failed, no such modifierNG!",
            static_cast<int>(ModifierType));
        return std::nullopt;
    }
    bool success = property->GetShowingValueAndCancelAnimation();
    if (!success) {
        ROSEN_LOGE("RSShowingPropertiesFreezer::GetPropertyImplNG Type %{public}d failed, cancel animation failed!",
            static_cast<int>(PropertyType));
        return std::nullopt;
    }
    return property->Get();
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetBounds() const
{
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BOUNDS, ModifierNG::RSPropertyType::BOUNDS>();
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetFrame() const
{
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::FRAME, ModifierNG::RSPropertyType::FRAME>();
}

std::optional<float> RSShowingPropertiesFreezer::GetPositionZ() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::POSITION_Z>();
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetPivot() const
{
    return GetPropertyImplNG<Vector2f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::PIVOT>();
}

std::optional<float> RSShowingPropertiesFreezer::GetPivotZ() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::PIVOT_Z>();
}

std::optional<Quaternion> RSShowingPropertiesFreezer::GetQuaternion() const
{
    return GetPropertyImplNG<Quaternion, ModifierNG::RSModifierType::TRANSFORM,
        ModifierNG::RSPropertyType::QUATERNION>();
}

std::optional<float> RSShowingPropertiesFreezer::GetRotation() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::ROTATION>();
}

std::optional<float> RSShowingPropertiesFreezer::GetRotationX() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::ROTATION_X>();
}

std::optional<float> RSShowingPropertiesFreezer::GetRotationY() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::ROTATION_Y>();
}

std::optional<float> RSShowingPropertiesFreezer::GetCameraDistance() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM,
        ModifierNG::RSPropertyType::CAMERA_DISTANCE>();
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetTranslate() const
{
    return GetPropertyImplNG<Vector2f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::TRANSLATE>();
}

std::optional<float> RSShowingPropertiesFreezer::GetTranslateZ() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::TRANSLATE_Z>();
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetScale() const
{
    return GetPropertyImplNG<Vector2f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::SCALE>();
}

std::optional<float> RSShowingPropertiesFreezer::GetScaleZ() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::SCALE_Z>();
}

std::optional<Vector3f> RSShowingPropertiesFreezer::GetSkew() const
{
    return GetPropertyImplNG<Vector3f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::SKEW>();
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetPersp() const
{
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::PERSP>();
}

std::optional<float> RSShowingPropertiesFreezer::GetAlpha() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::ALPHA, ModifierNG::RSPropertyType::ALPHA>();
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetCornerRadius() const
{
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::CLIP_TO_BOUNDS,
        ModifierNG::RSPropertyType::CORNER_RADIUS>();
}

std::optional<Color> RSShowingPropertiesFreezer::GetForegroundColor() const
{
    return GetPropertyImplNG<Color, ModifierNG::RSModifierType::FOREGROUND_COLOR,
        ModifierNG::RSPropertyType::FOREGROUND_COLOR>();
}

std::optional<Color> RSShowingPropertiesFreezer::GetBackgroundColor() const
{
    return GetPropertyImplNG<Color, ModifierNG::RSModifierType::BACKGROUND_COLOR,
        ModifierNG::RSPropertyType::BACKGROUND_COLOR>();
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImageWidth() const
{
    auto rect = GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BACKGROUND_IMAGE,
        ModifierNG::RSPropertyType::BG_IMAGE_RECT>();
    if (!rect.has_value()) {
        return std::nullopt;
    }
    return rect.value()[2]; // 2 index of width
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImageHeight() const
{
    auto rect = GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BACKGROUND_IMAGE,
        ModifierNG::RSPropertyType::BG_IMAGE_RECT>();
    if (!rect.has_value()) {
        return std::nullopt;
    }
    return rect.value()[3]; // 3 index of height
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImagePositionX() const
{
    auto rect = GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BACKGROUND_IMAGE,
        ModifierNG::RSPropertyType::BG_IMAGE_RECT>();
    if (!rect.has_value()) {
        return std::nullopt;
    }
    return rect.value()[0]; // 0 index of position x
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImagePositionY() const
{
    auto rect = GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BACKGROUND_IMAGE,
        ModifierNG::RSPropertyType::BG_IMAGE_RECT>();
    if (!rect.has_value()) {
        return std::nullopt;
    }
    return rect.value()[1]; // 1 index of position y
}

std::optional<Vector4<Color>> RSShowingPropertiesFreezer::GetBorderColor() const
{
    return GetPropertyImplNG<Vector4<Color>, ModifierNG::RSModifierType::BORDER,
        ModifierNG::RSPropertyType::BORDER_COLOR>();
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetBorderWidth() const
{
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BORDER, ModifierNG::RSPropertyType::BORDER_WIDTH>();
}

std::optional<Color> RSShowingPropertiesFreezer::GetShadowColor() const
{
    return GetPropertyImplNG<Color, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_COLOR>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowOffsetX() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_OFFSET_X>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowOffsetY() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_OFFSET_Y>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowAlpha() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_ALPHA>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowElevation() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_ELEVATION>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowRadius() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_RADIUS>();
}

std::optional<float> RSShowingPropertiesFreezer::GetSpherizeDegree() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::FOREGROUND_FILTER,
        ModifierNG::RSPropertyType::SPHERIZE>();
}

std::optional<float> RSShowingPropertiesFreezer::GetHDRUIBrightness() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::HDR_BRIGHTNESS,
        ModifierNG::RSPropertyType::HDR_UI_BRIGHTNESS>();
}

std::optional<float> RSShowingPropertiesFreezer::GetAttractionFractionValue() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::FOREGROUND_FILTER,
        ModifierNG::RSPropertyType::ATTRACTION_FRACTION>();
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetAttractionDstPointValue() const
{
    return GetPropertyImplNG<Vector2f, ModifierNG::RSModifierType::FOREGROUND_FILTER,
        ModifierNG::RSPropertyType::ATTRACTION_DSTPOINT>();
}

std::optional<float> RSShowingPropertiesFreezer::GetLightUpEffectDegree() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::COMPOSITING_FILTER,
        ModifierNG::RSPropertyType::LIGHT_UP_EFFECT_DEGREE>();
}

std::optional<float> RSShowingPropertiesFreezer::GetDynamicDimDegree() const
{
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::COMPOSITING_FILTER,
        ModifierNG::RSPropertyType::DYNAMIC_DIM_DEGREE>();
}
} // namespace Rosen
} // namespace OHOS
