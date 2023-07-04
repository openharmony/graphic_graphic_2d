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

namespace OHOS {
namespace Rosen {
RSShowingPropertiesFreezer::RSShowingPropertiesFreezer(NodeId id) : id_(id) {}

#define SHOWING_PROPERTIES_FREEZER(T, propertyType)                                                     \
    do {                                                                                                \
        auto node = RSNodeMap::Instance().GetNode<RSNode>(id_);                                         \
        if (node == nullptr) {                                                                          \
            return { std::nullopt };                                                                    \
        }                                                                                               \
        auto iter = node->propertyModifiers_.find(RSModifierType::propertyType);                        \
        if (iter == node->propertyModifiers_.end()) {                                                   \
            return { std::nullopt };                                                                    \
        }                                                                                               \
        auto property = std::static_pointer_cast<RSAnimatableProperty<T>>(iter->second->GetProperty()); \
        if (property == nullptr) {                                                                      \
            ROSEN_LOGE("RSShowingPropertiesFreezer::Get" #propertyType " failed, property is null!");   \
            return { std::nullopt };                                                                    \
        }                                                                                               \
        bool success = property->GetShowingValueAndCancelAnimation();                                   \
        if (!success) {                                                                                 \
            ROSEN_LOGE("RSShowingPropertiesFreezer::Get" #propertyType " failed!");                     \
            return { std::nullopt };                                                                    \
        }                                                                                               \
        return property->Get();                                                                         \
    } while (0)

std::optional<Vector4f> RSShowingPropertiesFreezer::GetBounds() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4f, BOUNDS);
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetFrame() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4f, FRAME);
}

std::optional<float> RSShowingPropertiesFreezer::GetPositionZ() const
{
    SHOWING_PROPERTIES_FREEZER(float, POSITION_Z);
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetPivot() const
{
    SHOWING_PROPERTIES_FREEZER(Vector2f, PIVOT);
}

std::optional<float> RSShowingPropertiesFreezer::GetPivotZ() const
{
    SHOWING_PROPERTIES_FREEZER(float, PIVOT_Z);
}

std::optional<Quaternion> RSShowingPropertiesFreezer::GetQuaternion() const
{
    SHOWING_PROPERTIES_FREEZER(Quaternion, QUATERNION);
}

std::optional<float> RSShowingPropertiesFreezer::GetRotation() const
{
    SHOWING_PROPERTIES_FREEZER(float, ROTATION);
}

std::optional<float> RSShowingPropertiesFreezer::GetRotationX() const
{
    SHOWING_PROPERTIES_FREEZER(float, ROTATION_X);
}

std::optional<float> RSShowingPropertiesFreezer::GetRotationY() const
{
    SHOWING_PROPERTIES_FREEZER(float, ROTATION_Y);
}

std::optional<float> RSShowingPropertiesFreezer::GetCameraDistance() const
{
    SHOWING_PROPERTIES_FREEZER(float, CAMERA_DISTANCE);
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetTranslate() const
{
    SHOWING_PROPERTIES_FREEZER(Vector2f, TRANSLATE);
}

std::optional<float> RSShowingPropertiesFreezer::GetTranslateZ() const
{
    SHOWING_PROPERTIES_FREEZER(float, TRANSLATE_Z);
}

std::optional<Vector2f> RSShowingPropertiesFreezer::GetScale() const
{
    SHOWING_PROPERTIES_FREEZER(Vector2f, SCALE);
}

std::optional<float> RSShowingPropertiesFreezer::GetAlpha() const
{
    SHOWING_PROPERTIES_FREEZER(float, ALPHA);
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetCornerRadius() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4f, CORNER_RADIUS);
}

std::optional<Color> RSShowingPropertiesFreezer::GetForegroundColor() const
{
    SHOWING_PROPERTIES_FREEZER(Color, FOREGROUND_COLOR);
}

std::optional<Color> RSShowingPropertiesFreezer::GetBackgroundColor() const
{
    SHOWING_PROPERTIES_FREEZER(Color, BACKGROUND_COLOR);
}

std::optional<Color> RSShowingPropertiesFreezer::GetSurfaceBgColor() const
{
    SHOWING_PROPERTIES_FREEZER(Color, SURFACE_BG_COLOR);
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImageWidth() const
{
    SHOWING_PROPERTIES_FREEZER(float, BG_IMAGE_WIDTH);
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImageHeight() const
{
    SHOWING_PROPERTIES_FREEZER(float, BG_IMAGE_HEIGHT);
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImagePositionX() const
{
    SHOWING_PROPERTIES_FREEZER(float, BG_IMAGE_POSITION_X);
}

std::optional<float> RSShowingPropertiesFreezer::GetBgImagePositionY() const
{
    SHOWING_PROPERTIES_FREEZER(float, BG_IMAGE_POSITION_Y);
}

std::optional<Vector4<Color>> RSShowingPropertiesFreezer::GetBorderColor() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4<Color>, BORDER_COLOR);
}

std::optional<Vector4f> RSShowingPropertiesFreezer::GetBorderWidth() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4f, BORDER_WIDTH);
}

std::optional<std::shared_ptr<RSFilter>> RSShowingPropertiesFreezer::GetBackgroundFilter() const
{
    SHOWING_PROPERTIES_FREEZER(std::shared_ptr<RSFilter>, BACKGROUND_FILTER);
}

std::optional<std::shared_ptr<RSFilter>> RSShowingPropertiesFreezer::GetFilter() const
{
    SHOWING_PROPERTIES_FREEZER(std::shared_ptr<RSFilter>, FILTER);
}

std::optional<Color> RSShowingPropertiesFreezer::GetShadowColor() const
{
    SHOWING_PROPERTIES_FREEZER(Color, SHADOW_COLOR);
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowOffsetX() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_OFFSET_X);
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowOffsetY() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_OFFSET_Y);
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowAlpha() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_ALPHA);
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowElevation() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_ELEVATION);
}

std::optional<float> RSShowingPropertiesFreezer::GetShadowRadius() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_RADIUS);
}

std::optional<float> RSShowingPropertiesFreezer::GetSpherizeDegree() const
{
    SHOWING_PROPERTIES_FREEZER(float, SPHERIZE);
}

std::optional<float> RSShowingPropertiesFreezer::GetLightUpEffectDegree() const
{
    SHOWING_PROPERTIES_FREEZER(float, LIGHT_UP_EFFECT);
}
} // namespace Rosen
} // namespace OHOS
