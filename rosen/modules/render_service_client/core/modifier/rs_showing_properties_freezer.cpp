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
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BOUNDS, ModifierNG::RSPropertyType::BOUNDS>();
#else
    return GetPropertyImpl<Vector4f, RSModifierType::BOUNDS>();
#endif
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetFrame() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::FRAME, ModifierNG::RSPropertyType::FRAME>();
#else
    return GetPropertyImpl<Vector4f, RSModifierType::FRAME>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetPositionZ() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::POSITION_Z>();
#else
    return GetPropertyImpl<float, RSModifierType::POSITION_Z>();
#endif
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetPivot() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector2f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::PIVOT>();
#else
    return GetPropertyImpl<Vector2f, RSModifierType::PIVOT>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetPivotZ() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::PIVOT_Z>();
#else
    return GetPropertyImpl<float, RSModifierType::PIVOT_Z>();
#endif
}

std::optional<Quaternion> RSShowingPropertiesFreezer::GetQuaternion() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Quaternion, ModifierNG::RSModifierType::TRANSFORM,
        ModifierNG::RSPropertyType::QUATERNION>();
#else
    return GetPropertyImpl<Quaternion, RSModifierType::QUATERNION>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetRotation() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::ROTATION>();
#else
    return GetPropertyImpl<float, RSModifierType::ROTATION>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetRotationX() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::ROTATION_X>();
#else
    return GetPropertyImpl<float, RSModifierType::ROTATION_X>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetRotationY() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::ROTATION_Y>();
#else
    return GetPropertyImpl<float, RSModifierType::ROTATION_Y>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetCameraDistance() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM,
        ModifierNG::RSPropertyType::CAMERA_DISTANCE>();
#else
    return GetPropertyImpl<float, RSModifierType::CAMERA_DISTANCE>();
#endif
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetTranslate() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector2f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::TRANSLATE>();
#else
    return GetPropertyImpl<Vector2f, RSModifierType::TRANSLATE>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetTranslateZ() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::TRANSLATE_Z>();
#else
    return GetPropertyImpl<float, RSModifierType::TRANSLATE_Z>();
#endif
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetScale() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector2f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::SCALE>();
#else
    return GetPropertyImpl<Vector2f, RSModifierType::SCALE>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetScaleZ() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::SCALE_Z>();
#else
    return GetPropertyImpl<float, RSModifierType::SCALE_Z>();
#endif
}

std::optional<Vector3f> RSShowingPropertiesFreezer::GetSkew() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector3f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::SKEW>();
#else
    return GetPropertyImpl<Vector3f, RSModifierType::SKEW>();
#endif
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetPersp() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::TRANSFORM, ModifierNG::RSPropertyType::PERSP>();
#else
    return GetPropertyImpl<Vector4f, RSModifierType::PERSP>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetAlpha() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::ALPHA, ModifierNG::RSPropertyType::ALPHA>();
#else
    return GetPropertyImpl<float, RSModifierType::ALPHA>();
#endif
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetCornerRadius() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::CLIP_TO_BOUNDS,
        ModifierNG::RSPropertyType::CORNER_RADIUS>();
#else
    return GetPropertyImpl<Vector4f, RSModifierType::CORNER_RADIUS>();
#endif
}

std::optional<Color> RSShowingPropertiesFreezer::GetForegroundColor() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Color, ModifierNG::RSModifierType::FOREGROUND_COLOR,
        ModifierNG::RSPropertyType::FOREGROUND_COLOR>();
#else
    return GetPropertyImpl<Color, RSModifierType::FOREGROUND_COLOR>();
#endif
}

std::optional<Color> RSShowingPropertiesFreezer::GetBackgroundColor() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Color, ModifierNG::RSModifierType::BACKGROUND_COLOR,
        ModifierNG::RSPropertyType::BACKGROUND_COLOR>();
#else
    return GetPropertyImpl<Color, RSModifierType::BACKGROUND_COLOR>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImageWidth() const
{
#if defined(MODIFIER_NG)
    auto rect = GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BACKGROUND_IMAGE,
        ModifierNG::RSPropertyType::BG_IMAGE_RECT>();
    if (!rect.has_value()) {
        return std::nullopt;
    }
    return rect.value()[2]; // 2 index of width
#else
    return GetPropertyImpl<float, RSModifierType::BG_IMAGE_WIDTH>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImageHeight() const
{
#if defined(MODIFIER_NG)
    auto rect = GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BACKGROUND_IMAGE,
        ModifierNG::RSPropertyType::BG_IMAGE_RECT>();
    if (!rect.has_value()) {
        return std::nullopt;
    }
    return rect.value()[3]; // 3 index of height
#else
    return GetPropertyImpl<float, RSModifierType::BG_IMAGE_HEIGHT>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImagePositionX() const
{
#if defined(MODIFIER_NG)
    auto rect = GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BACKGROUND_IMAGE,
        ModifierNG::RSPropertyType::BG_IMAGE_RECT>();
    if (!rect.has_value()) {
        return std::nullopt;
    }
    return rect.value()[0]; // 0 index of position x
#else
    return GetPropertyImpl<float, RSModifierType::BG_IMAGE_POSITION_X>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImagePositionY() const
{
#if defined(MODIFIER_NG)
    auto rect = GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BACKGROUND_IMAGE,
        ModifierNG::RSPropertyType::BG_IMAGE_RECT>();
    if (!rect.has_value()) {
        return std::nullopt;
    }
    return rect.value()[1]; // 1 index of position y
#else
    return GetPropertyImpl<float, RSModifierType::BG_IMAGE_POSITION_Y>();
#endif
}

std::optional<Vector4<Color>> RSShowingPropertiesFreezer::GetBorderColor() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector4<Color>, ModifierNG::RSModifierType::BORDER,
        ModifierNG::RSPropertyType::BORDER_COLOR>();
#else
    return GetPropertyImpl<Vector4<Color>, RSModifierType::BORDER_COLOR>();
#endif
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetBorderWidth() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector4f, ModifierNG::RSModifierType::BORDER, ModifierNG::RSPropertyType::BORDER_WIDTH>();
#else
    return GetPropertyImpl<Vector4f, RSModifierType::BORDER_WIDTH>();
#endif
}

std::optional<Color> RSShowingPropertiesFreezer::GetShadowColor() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Color, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_COLOR>();
#else
    return GetPropertyImpl<Color, RSModifierType::SHADOW_COLOR>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowOffsetX() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_OFFSET_X>();
#else
    return GetPropertyImpl<float, RSModifierType::SHADOW_OFFSET_X>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowOffsetY() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_OFFSET_Y>();
#else
    return GetPropertyImpl<float, RSModifierType::SHADOW_OFFSET_Y>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowAlpha() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_ALPHA>();
#else
    return GetPropertyImpl<float, RSModifierType::SHADOW_ALPHA>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowElevation() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_ELEVATION>();
#else
    return GetPropertyImpl<float, RSModifierType::SHADOW_ELEVATION>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowRadius() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::SHADOW, ModifierNG::RSPropertyType::SHADOW_RADIUS>();
#else
    return GetPropertyImpl<float, RSModifierType::SHADOW_RADIUS>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetSpherizeDegree() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::FOREGROUND_FILTER,
        ModifierNG::RSPropertyType::SPHERIZE>();
#else
    return GetPropertyImpl<float, RSModifierType::SPHERIZE>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetHDRUIBrightness() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::HDR_BRIGHTNESS,
        ModifierNG::RSPropertyType::HDR_UI_BRIGHTNESS>();
#else
    return GetPropertyImpl<float, RSModifierType::HDR_UI_BRIGHTNESS>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetAttractionFractionValue() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::FOREGROUND_FILTER,
        ModifierNG::RSPropertyType::ATTRACTION_FRACTION>();
#else
    return GetPropertyImpl<float, RSModifierType::ATTRACTION_FRACTION>();
#endif
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetAttractionDstPointValue() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<Vector2f, ModifierNG::RSModifierType::FOREGROUND_FILTER,
        ModifierNG::RSPropertyType::ATTRACTION_DSTPOINT>();
#else
    return GetPropertyImpl<Vector2f, RSModifierType::ATTRACTION_DSTPOINT>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetLightUpEffectDegree() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::COMPOSITING_FILTER,
        ModifierNG::RSPropertyType::LIGHT_UP_EFFECT_DEGREE>();
#else
    return GetPropertyImpl<float, RSModifierType::LIGHT_UP_EFFECT>();
#endif
}

std::optional<float> RSShowingPropertiesFreezer::GetDynamicDimDegree() const
{
#if defined(MODIFIER_NG)
    return GetPropertyImplNG<float, ModifierNG::RSModifierType::COMPOSITING_FILTER,
        ModifierNG::RSPropertyType::DYNAMIC_DIM_DEGREE>();
#else
    return GetPropertyImpl<float, RSModifierType::DYNAMIC_DIM_DEGREE>();
#endif
}
} // namespace Rosen
} // namespace OHOS
