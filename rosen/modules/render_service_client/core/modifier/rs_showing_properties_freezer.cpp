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
#include "pipeline/rs_node_map.h"
#include "ui/rs_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSShowingPropertiesFreezer::RSShowingPropertiesFreezer(NodeId id) : id_(id) {}

template<typename T, RSModifierType Type>
std::optional<T> RSShowingPropertiesFreezer::GetPropertyImpl() const
{
    auto node = RSNodeMap::Instance().GetNode<RSNode>(id_);
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

std::optional<Vector4f> RSShowingPropertiesFreezer::GetBounds() const
{
    return GetPropertyImpl<Vector4f, RSModifierType::BOUNDS>();
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetFrame() const
{
    return GetPropertyImpl<Vector4f, RSModifierType::FRAME>();
}

std::optional<float> RSShowingPropertiesFreezer::GetPositionZ() const
{
    return GetPropertyImpl<float, RSModifierType::POSITION_Z>();
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetPivot() const
{
    return GetPropertyImpl<Vector2f, RSModifierType::PIVOT>();
}

std::optional<float> RSShowingPropertiesFreezer::GetPivotZ() const
{
    return GetPropertyImpl<float, RSModifierType::PIVOT_Z>();
}

std::optional<Quaternion> RSShowingPropertiesFreezer::GetQuaternion() const
{
    return GetPropertyImpl<Quaternion, RSModifierType::QUATERNION>();
}

std::optional<float> RSShowingPropertiesFreezer::GetRotation() const
{
    return GetPropertyImpl<float, RSModifierType::ROTATION>();
}

std::optional<float> RSShowingPropertiesFreezer::GetRotationX() const
{
    return GetPropertyImpl<float, RSModifierType::ROTATION_X>();
}

std::optional<float> RSShowingPropertiesFreezer::GetRotationY() const
{
    return GetPropertyImpl<float, RSModifierType::ROTATION_Y>();
}

std::optional<float> RSShowingPropertiesFreezer::GetCameraDistance() const
{
    return GetPropertyImpl<float, RSModifierType::CAMERA_DISTANCE>();
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetTranslate() const
{
    return GetPropertyImpl<Vector2f, RSModifierType::TRANSLATE>();
}

std::optional<float> RSShowingPropertiesFreezer::GetTranslateZ() const
{
    return GetPropertyImpl<float, RSModifierType::TRANSLATE_Z>();
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetScale() const
{
    return GetPropertyImpl<Vector2f, RSModifierType::SCALE>();
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetSkew() const
{
    return GetPropertyImpl<Vector2f, RSModifierType::SKEW>();
}

std::optional<float> RSShowingPropertiesFreezer::GetAlpha() const
{
    return GetPropertyImpl<float, RSModifierType::ALPHA>();
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetCornerRadius() const
{
    return GetPropertyImpl<Vector4f, RSModifierType::CORNER_RADIUS>();
}

std::optional<Color> RSShowingPropertiesFreezer::GetForegroundColor() const
{
    return GetPropertyImpl<Color, RSModifierType::FOREGROUND_COLOR>();
}

std::optional<Color> RSShowingPropertiesFreezer::GetBackgroundColor() const
{
    return GetPropertyImpl<Color, RSModifierType::BACKGROUND_COLOR>();
}

std::optional<Color> RSShowingPropertiesFreezer::GetSurfaceBgColor() const
{
    return GetPropertyImpl<Color, RSModifierType::SURFACE_BG_COLOR>();
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImageWidth() const
{
    return GetPropertyImpl<float, RSModifierType::BG_IMAGE_WIDTH>();
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImageHeight() const
{
    return GetPropertyImpl<float, RSModifierType::BG_IMAGE_HEIGHT>();
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImagePositionX() const
{
    return GetPropertyImpl<float, RSModifierType::BG_IMAGE_POSITION_X>();
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImagePositionY() const
{
    return GetPropertyImpl<float, RSModifierType::BG_IMAGE_POSITION_Y>();
}

std::optional<Vector4<Color>> RSShowingPropertiesFreezer::GetBorderColor() const
{
    return GetPropertyImpl<Vector4<Color>, RSModifierType::BORDER_COLOR>();
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetBorderWidth() const
{
    return GetPropertyImpl<Vector4f, RSModifierType::BORDER_WIDTH>();
}

std::optional<std::shared_ptr<RSFilter>> RSShowingPropertiesFreezer::GetBackgroundFilter() const
{
    return GetPropertyImpl<std::shared_ptr<RSFilter>, RSModifierType::BACKGROUND_FILTER>();
}

std::optional<std::shared_ptr<RSFilter>> RSShowingPropertiesFreezer::GetFilter() const
{
    return GetPropertyImpl<std::shared_ptr<RSFilter>, RSModifierType::FILTER>();
}

std::optional<Color> RSShowingPropertiesFreezer::GetShadowColor() const
{
    return GetPropertyImpl<Color, RSModifierType::SHADOW_COLOR>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowOffsetX() const
{
    return GetPropertyImpl<float, RSModifierType::SHADOW_OFFSET_X>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowOffsetY() const
{
    return GetPropertyImpl<float, RSModifierType::SHADOW_OFFSET_Y>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowAlpha() const
{
    return GetPropertyImpl<float, RSModifierType::SHADOW_ALPHA>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowElevation() const
{
    return GetPropertyImpl<float, RSModifierType::SHADOW_ELEVATION>();
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowRadius() const
{
    return GetPropertyImpl<float, RSModifierType::SHADOW_RADIUS>();
}

std::optional<float> RSShowingPropertiesFreezer::GetSpherizeDegree() const
{
    return GetPropertyImpl<float, RSModifierType::SPHERIZE>();
}

std::optional<float> RSShowingPropertiesFreezer::GetLightUpEffectDegree() const
{
    return GetPropertyImpl<float, RSModifierType::LIGHT_UP_EFFECT>();
}
} // namespace Rosen
} // namespace OHOS
