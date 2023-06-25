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

#define SHOWING_PROPERTIES_FREEZER(T, propertyType, defaultValue)                                           \
    do {                                                                                                    \
        bool success = false;                                                                               \
        T value = defaultValue;                                                                             \
        auto node = RSNodeMap::Instance().GetNode<RSNode>(id_);                                             \
        if (node == nullptr) {                                                                              \
            return { value, success };                                                                      \
        }                                                                                                   \
        auto iter = node->propertyModifiers_.find(RSModifierType::propertyType);                            \
        if (iter != node->propertyModifiers_.end()) {                                                       \
            auto property = std::static_pointer_cast<RSAnimatableProperty<T>>(iter->second->GetProperty()); \
            if (property == nullptr) {                                                                      \
                ROSEN_LOGE("RSShowingPropertiesFreezer::Get##propertyType## failed, property is null!");    \
            } else {                                                                                        \
                success = property->GetShowingValueAndCancelAnimation(node->IsRenderServiceNode());         \
                if (success) {                                                                              \
                    node->CancelAnimationByProperty(property->GetId());                                     \
                    value = property->Get();                                                                \
                }                                                                                           \
            }                                                                                               \
        } else {                                                                                            \
            ROSEN_LOGE("RSShowingPropertiesFreezer::Ge##propertyType## failed, modifier is not exist");     \
        }                                                                                                   \
        return { value, success };                                                                          \
    } while (0)

std::tuple<Vector4f, bool> RSShowingPropertiesFreezer::GetBounds() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4f, BOUNDS, Vector4f(0.0f));
}

std::tuple<Vector4f, bool> RSShowingPropertiesFreezer::GetFrame() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4f, FRAME, Vector4f(0.0f));
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetPositionZ() const
{
    SHOWING_PROPERTIES_FREEZER(float, POSITION_Z, 0.0f);
}

std::tuple<Vector2f, bool> RSShowingPropertiesFreezer::GetPivot() const
{
    SHOWING_PROPERTIES_FREEZER(Vector2f, PIVOT, Vector2f(0.5f, 0.5f));
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetPivotZ() const
{
    SHOWING_PROPERTIES_FREEZER(float, PIVOT_Z, 0.0f);
}

std::tuple<Quaternion, bool> RSShowingPropertiesFreezer::GetQuaternion() const
{
    SHOWING_PROPERTIES_FREEZER(Quaternion, QUATERNION, Quaternion());
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetRotation() const
{
    SHOWING_PROPERTIES_FREEZER(float, ROTATION, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetRotationX() const
{
    SHOWING_PROPERTIES_FREEZER(float, ROTATION_X, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetRotationY() const
{
    SHOWING_PROPERTIES_FREEZER(float, ROTATION_Y, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetCameraDistance() const
{
    SHOWING_PROPERTIES_FREEZER(float, CAMERA_DISTANCE, 0.0f);
}

std::tuple<Vector2f, bool> RSShowingPropertiesFreezer::GetTranslate() const
{
    SHOWING_PROPERTIES_FREEZER(Vector2f, TRANSLATE, Vector2f(0.0f, 0.0f));
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetTranslateZ() const
{
    SHOWING_PROPERTIES_FREEZER(float, TRANSLATE_Z, 0.0f);
}

std::tuple<Vector2f, bool> RSShowingPropertiesFreezer::GetScale() const
{
    SHOWING_PROPERTIES_FREEZER(Vector2f, SCALE, Vector2f(1.0f, 1.0f));
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetAlpha() const
{
    SHOWING_PROPERTIES_FREEZER(float, ALPHA, 1.0f);
}

std::tuple<Vector4f, bool> RSShowingPropertiesFreezer::GetCornerRadius() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4f, CORNER_RADIUS, Vector4f(0.0f));
}

std::tuple<Color, bool> RSShowingPropertiesFreezer::GetForegroundColor() const
{
    SHOWING_PROPERTIES_FREEZER(Color, FOREGROUND_COLOR, RgbPalette::Transparent());
}

std::tuple<Color, bool> RSShowingPropertiesFreezer::GetBackgroundColor() const
{
    SHOWING_PROPERTIES_FREEZER(Color, BACKGROUND_COLOR, RgbPalette::Transparent());
}

std::tuple<Color, bool> RSShowingPropertiesFreezer::GetSurfaceBgColor() const
{
    SHOWING_PROPERTIES_FREEZER(Color, SURFACE_BG_COLOR, RgbPalette::Transparent());
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetBgImageWidth() const
{
    SHOWING_PROPERTIES_FREEZER(float, BG_IMAGE_WIDTH, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetBgImageHeight() const
{
    SHOWING_PROPERTIES_FREEZER(float, BG_IMAGE_HEIGHT, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetBgImagePositionX() const
{
    SHOWING_PROPERTIES_FREEZER(float, BG_IMAGE_POSITION_X, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetBgImagePositionY() const
{
    SHOWING_PROPERTIES_FREEZER(float, BG_IMAGE_POSITION_Y, 0.0f);
}

std::tuple<Vector4<Color>, bool> RSShowingPropertiesFreezer::GetBorderColor() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4<Color>, BORDER_COLOR, Vector4<Color>(RgbPalette::Transparent()));
}

std::tuple<Vector4f, bool> RSShowingPropertiesFreezer::GetBorderWidth() const
{
    SHOWING_PROPERTIES_FREEZER(Vector4f, BORDER_WIDTH, Vector4f(0.0f));
}

std::tuple<std::shared_ptr<RSFilter>, bool> RSShowingPropertiesFreezer::GetBackgroundFilter() const
{
    SHOWING_PROPERTIES_FREEZER(std::shared_ptr<RSFilter>, BACKGROUND_FILTER, nullptr);
}

std::tuple<std::shared_ptr<RSFilter>, bool> RSShowingPropertiesFreezer::GetFilter() const
{
    SHOWING_PROPERTIES_FREEZER(std::shared_ptr<RSFilter>, FILTER, nullptr);
}

std::tuple<Color, bool> RSShowingPropertiesFreezer::GetShadowColor() const
{
    SHOWING_PROPERTIES_FREEZER(Color, SHADOW_COLOR, Color::FromArgbInt(DEFAULT_SPOT_COLOR));
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetShadowOffsetX() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_OFFSET_X, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetShadowOffsetY() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_OFFSET_Y, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetShadowAlpha() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_ALPHA, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetShadowElevation() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_ELEVATION, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetShadowRadius() const
{
    SHOWING_PROPERTIES_FREEZER(float, SHADOW_RADIUS, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetSpherizeDegree() const
{
    SHOWING_PROPERTIES_FREEZER(float, SPHERIZE, 0.0f);
}

std::tuple<float, bool> RSShowingPropertiesFreezer::GetLightUpEffectDegree() const
{
    SHOWING_PROPERTIES_FREEZER(float, LIGHT_UP_EFFECT, 0.0f);
}
} // namespace Rosen
} // namespace OHOS
