/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "modifier/rs_modifier_extractor.h"

#include <securec.h>

#include "modifier/rs_property_modifier.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_node_map.h"
#include "property/rs_properties_def.h"
#include "ui/rs_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSModifierExtractor::RSModifierExtractor(NodeId id) : id_(id) {}
constexpr uint32_t DEBUG_MODIFIER_SIZE = 20;
#define GET_PROPERTY_FROM_MODIFIERS(T, propertyType, defaultValue, operator)                                        \
    do {                                                                                                            \
        auto node = RSNodeMap::Instance().GetNode<RSNode>(id_);                                                     \
        if (!node) {                                                                                                \
            return defaultValue;                                                                                    \
        }                                                                                                           \
        std::unique_lock<std::recursive_mutex> lock(node->GetPropertyMutex());                                      \
        auto iter = node->propertyModifiers_.find(RSModifierType::propertyType);                                    \
        if (iter != node->propertyModifiers_.end()) {                                                               \
            if (!iter->second || !iter->second->GetProperty()) {                                                    \
                return defaultValue;                                                                                \
            }                                                                                                       \
            return std::static_pointer_cast<RSProperty<T>>(iter->second->GetProperty())->Get();                     \
        }                                                                                                           \
        T value = defaultValue;                                                                                     \
        if (node->modifiers_.size() > DEBUG_MODIFIER_SIZE) {                                                        \
            ROSEN_LOGD("RSModifierExtractor modifier size is %{public}zu", node->modifiers_.size());                \
        }                                                                                                           \
        for (auto& [_, modifier] : node->modifiers_) {                                                              \
            if (modifier->GetModifierType() == RSModifierType::propertyType) {                                      \
                value operator std::static_pointer_cast<RSProperty<T>>(modifier->GetProperty())->Get();             \
            }                                                                                                       \
        }                                                                                                           \
        return value;                                                                                               \
    } while (0)

Vector4f RSModifierExtractor::GetBounds() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector4f, BOUNDS, Vector4f(), =);
}

Vector4f RSModifierExtractor::GetFrame() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector4f, FRAME, Vector4f(), =);
}

float RSModifierExtractor::GetPositionZ() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, POSITION_Z, 0.f, +=);
}

Vector2f RSModifierExtractor::GetPivot() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector2f, PIVOT, Vector2f(0.5f, 0.5f), =);
}

float RSModifierExtractor::GetPivotZ() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, PIVOT_Z, 0.f, =);
}

Quaternion RSModifierExtractor::GetQuaternion() const
{
    GET_PROPERTY_FROM_MODIFIERS(Quaternion, QUATERNION, Quaternion(), =);
}

float RSModifierExtractor::GetRotation() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, ROTATION, 0.f, +=);
}

float RSModifierExtractor::GetRotationX() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, ROTATION_X, 0.f, +=);
}

float RSModifierExtractor::GetRotationY() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, ROTATION_Y, 0.f, +=);
}

float RSModifierExtractor::GetCameraDistance() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, CAMERA_DISTANCE, 0.f, =);
}

Vector2f RSModifierExtractor::GetTranslate() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector2f, TRANSLATE, Vector2f(0.f, 0.f), +=);
}

float RSModifierExtractor::GetTranslateZ() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, TRANSLATE_Z, 0.f, +=);
}

Vector2f RSModifierExtractor::GetScale() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector2f, SCALE, Vector2f(1.f, 1.f), *=);
}

float RSModifierExtractor::GetAlpha() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, ALPHA, 1.f, *=);
}

bool RSModifierExtractor::GetAlphaOffscreen() const
{
    GET_PROPERTY_FROM_MODIFIERS(bool, ALPHA_OFFSCREEN, true, =);
}

Vector4f RSModifierExtractor::GetCornerRadius() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector4f, CORNER_RADIUS, Vector4f(), =);
}

Color RSModifierExtractor::GetForegroundColor() const
{
    GET_PROPERTY_FROM_MODIFIERS(Color, FOREGROUND_COLOR, RgbPalette::Transparent(), =);
}

Color RSModifierExtractor::GetBackgroundColor() const
{
    GET_PROPERTY_FROM_MODIFIERS(Color, BACKGROUND_COLOR, RgbPalette::Transparent(), =);
}

Color RSModifierExtractor::GetSurfaceBgColor() const
{
    GET_PROPERTY_FROM_MODIFIERS(Color, SURFACE_BG_COLOR, RgbPalette::Transparent(), =);
}

std::shared_ptr<RSShader> RSModifierExtractor::GetBackgroundShader() const
{
    GET_PROPERTY_FROM_MODIFIERS(std::shared_ptr<RSShader>, BACKGROUND_SHADER, nullptr, =);
}

std::shared_ptr<RSImage> RSModifierExtractor::GetBgImage() const
{
    GET_PROPERTY_FROM_MODIFIERS(std::shared_ptr<RSImage>, BG_IMAGE, nullptr, =);
}

float RSModifierExtractor::GetBgImageWidth() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, BG_IMAGE_WIDTH, 0.f, =);
}

float RSModifierExtractor::GetBgImageHeight() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, BG_IMAGE_HEIGHT, 0.f, =);
}

float RSModifierExtractor::GetBgImagePositionX() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, BG_IMAGE_POSITION_X, 0.f, =);
}

float RSModifierExtractor::GetBgImagePositionY() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, BG_IMAGE_POSITION_Y, 0.f, =);
}

Vector4<Color> RSModifierExtractor::GetBorderColor() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector4<Color>, BORDER_COLOR, Vector4<Color>(RgbPalette::Transparent()), =);
}

Vector4f RSModifierExtractor::GetBorderWidth() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector4f, BORDER_WIDTH, Vector4f(0.f), =);
}

Vector4<uint32_t> RSModifierExtractor::GetBorderStyle() const
{
    GET_PROPERTY_FROM_MODIFIERS(
        Vector4<uint32_t>, BORDER_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE)), =);
}

Vector4<Color> RSModifierExtractor::GetOutlineColor() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector4<Color>, OUTLINE_COLOR, Vector4<Color>(RgbPalette::Transparent()), =);
}

Vector4f RSModifierExtractor::GetOutlineWidth() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector4f, OUTLINE_WIDTH, Vector4f(0.f), =);
}

Vector4<uint32_t> RSModifierExtractor::GetOutlineStyle() const
{
    GET_PROPERTY_FROM_MODIFIERS(
        Vector4<uint32_t>, OUTLINE_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE)), =);
}

Vector4f RSModifierExtractor::GetOutlineRadius() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector4f, OUTLINE_RADIUS, Vector4f(0.f), =);
}

std::shared_ptr<RSFilter> RSModifierExtractor::GetBackgroundFilter() const
{
    GET_PROPERTY_FROM_MODIFIERS(std::shared_ptr<RSFilter>, BACKGROUND_FILTER, nullptr, =);
}

std::shared_ptr<RSFilter> RSModifierExtractor::GetFilter() const
{
    GET_PROPERTY_FROM_MODIFIERS(std::shared_ptr<RSFilter>, FILTER, nullptr, =);
}

Color RSModifierExtractor::GetShadowColor() const
{
    GET_PROPERTY_FROM_MODIFIERS(Color, SHADOW_COLOR, Color::FromArgbInt(DEFAULT_SPOT_COLOR), =);
}

float RSModifierExtractor::GetShadowOffsetX() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, SHADOW_OFFSET_X, DEFAULT_SHADOW_OFFSET_X, =);
}

float RSModifierExtractor::GetShadowOffsetY() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, SHADOW_OFFSET_Y, DEFAULT_SHADOW_OFFSET_Y, =);
}

float RSModifierExtractor::GetShadowAlpha() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, SHADOW_ALPHA, 0.f, =);
}

float RSModifierExtractor::GetShadowElevation() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, SHADOW_ELEVATION, 0.f, =);
}

float RSModifierExtractor::GetShadowRadius() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, SHADOW_RADIUS, DEFAULT_SHADOW_RADIUS, =);
}

std::shared_ptr<RSPath> RSModifierExtractor::GetShadowPath() const
{
    GET_PROPERTY_FROM_MODIFIERS(std::shared_ptr<RSPath>, SHADOW_PATH, nullptr, =);
}

bool RSModifierExtractor::GetShadowMask() const
{
    GET_PROPERTY_FROM_MODIFIERS(bool, SHADOW_MASK, false, =);
}

bool RSModifierExtractor::GetShadowIsFilled() const
{
    GET_PROPERTY_FROM_MODIFIERS(bool, SHADOW_IS_FILLED, false, =);
}

int RSModifierExtractor::GetShadowColorStrategy() const
{
    GET_PROPERTY_FROM_MODIFIERS(int, SHADOW_COLOR_STRATEGY, SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE, =);
}

Gravity RSModifierExtractor::GetFrameGravity() const
{
    GET_PROPERTY_FROM_MODIFIERS(Gravity, FRAME_GRAVITY, Gravity::DEFAULT, =);
}

std::shared_ptr<RSPath> RSModifierExtractor::GetClipBounds() const
{
    GET_PROPERTY_FROM_MODIFIERS(std::shared_ptr<RSPath>, CLIP_BOUNDS, nullptr, =);
}

bool RSModifierExtractor::GetClipToBounds() const
{
    GET_PROPERTY_FROM_MODIFIERS(bool, CLIP_TO_BOUNDS, false, =);
}

bool RSModifierExtractor::GetClipToFrame() const
{
    GET_PROPERTY_FROM_MODIFIERS(bool, CLIP_TO_FRAME, false, =);
}

bool RSModifierExtractor::GetVisible() const
{
    GET_PROPERTY_FROM_MODIFIERS(bool, VISIBLE, true, =);
}

std::shared_ptr<RSMask> RSModifierExtractor::GetMask() const
{
    GET_PROPERTY_FROM_MODIFIERS(std::shared_ptr<RSMask>, MASK, nullptr, =);
}

float RSModifierExtractor::GetSpherizeDegree() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, SPHERIZE, 0.f, =);
}

float RSModifierExtractor::GetLightUpEffectDegree() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, LIGHT_UP_EFFECT, 0.f, =);
}

float RSModifierExtractor::GetLightIntensity() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, LIGHT_INTENSITY, 0.f, =);
}

Vector4f RSModifierExtractor::GetLightPosition() const
{
    GET_PROPERTY_FROM_MODIFIERS(Vector4f, LIGHT_POSITION, Vector4f(0.f), =);
}

float RSModifierExtractor::GetIlluminatedBorderWidth() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, ILLUMINATED_BORDER_WIDTH, 0.f, =);
}

int RSModifierExtractor::GetIlluminatedType() const
{
    GET_PROPERTY_FROM_MODIFIERS(int, ILLUMINATED_TYPE, 0, =);
}

float RSModifierExtractor::GetBloom() const
{
    GET_PROPERTY_FROM_MODIFIERS(float, BLOOM, 0.f, =);
}

std::string RSModifierExtractor::Dump() const
{
    std::string dumpInfo;
    char buffer[UINT8_MAX] = { 0 };
    auto bounds = GetBounds();
    auto frame = GetFrame();
    if (sprintf_s(buffer, UINT8_MAX, "Bounds[%.1f %.1f %.1f %.1f] Frame[%.1f %.1f %.1f %.1f]",
        bounds.x_, bounds.y_, bounds.z_, bounds.w_, frame.x_, frame.y_, frame.z_, frame.w_) != -1) {
        dumpInfo.append(buffer);
    }

    auto ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for BackgroundColor, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetBackgroundColor(), RgbPalette::Transparent()) &&
        sprintf_s(buffer, UINT8_MAX, ", BackgroundColor[#%08X]", GetBackgroundColor().AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Alpha, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetAlpha(), 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Alpha[%.1f]", GetAlpha()) != -1) {
        dumpInfo.append(buffer);
    }

    if (!GetVisible()) {
        dumpInfo.append(", IsVisible[false]");
    }
    return dumpInfo;
}
} // namespace Rosen
} // namespace OHOS
