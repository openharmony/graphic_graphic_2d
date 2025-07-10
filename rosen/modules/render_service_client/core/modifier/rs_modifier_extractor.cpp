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

#include "modifier/rs_modifier_type.h"
#include "modifier/rs_property_modifier.h"
#include "modifier_ng/rs_modifier_ng.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_def.h"
#include "ui/rs_node.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {
RSModifierExtractor::RSModifierExtractor(NodeId id, std::shared_ptr<RSUIContext> rsUIContext)
    : id_(id), rsUIContext_(rsUIContext)
{}
constexpr uint32_t DEBUG_MODIFIER_SIZE = 20;
#if defined(MODIFIER_NG)
#define GET_PROPERTY_FROM_MODIFIERS_NG(T, modifierType, propertyType, defaultValue, operator)          \
    do {                                                                                               \
        auto node = rsUIContext_.lock() ? rsUIContext_.lock()->GetNodeMap().GetNode<RSNode>(id_)       \
                                        : RSNodeMap::Instance().GetNode<RSNode>(id_);                  \
        if (!node) {                                                                                   \
            return defaultValue;                                                                       \
        }                                                                                              \
        std::unique_lock<std::recursive_mutex> lock(node->GetPropertyMutex());                         \
        T value = defaultValue;                                                                        \
        if (node->modifiersNG_.size() > DEBUG_MODIFIER_SIZE) {                                         \
            ROSEN_LOGD("RSModifierExtractor modifier size is %{public}zu", node->modifiersNG_.size()); \
        }                                                                                              \
        for (auto& [_, modifier] : node->modifiersNG_) {                                               \
            if (modifier->GetType() == ModifierNG::RSModifierType::modifierType) {                     \
                std::shared_ptr<RSProperty<T>> property = std::static_pointer_cast<RSProperty<T>>(     \
                    modifier->GetProperty(ModifierNG::RSPropertyType::propertyType));                  \
                if (property == nullptr) {                                                             \
                    continue;                                                                          \
                }                                                                                      \
                value operator property->Get();                                                        \
            }                                                                                          \
        }                                                                                              \
        return value;                                                                                  \
    } while (0)
#else
#define GET_PROPERTY_FROM_MODIFIERS(T, propertyType, defaultValue, operator)                                        \
    do {                                                                                                            \
        auto rsUIContextPtr = rsUIContext_.lock();                                                                  \
        auto node = rsUIContextPtr ? rsUIContextPtr->GetNodeMap().GetNode<RSNode>(id_)                              \
                                   : RSNodeMap::Instance().GetNode<RSNode>(id_);                                    \
        if (!node) {                                                                                                \
            return defaultValue;                                                                                    \
        }                                                                                                           \
        std::unique_lock<std::recursive_mutex> lock(node->GetPropertyMutex());                                      \
        T value = defaultValue;                                                                                     \
        if (node->modifiers_.size() > DEBUG_MODIFIER_SIZE) {                                                        \
            RS_LOGE_LIMIT(                                                                                          \
                __func__, __line__, "RSModifierExtractor modifier size is %{public}zu", node->modifiers_.size());   \
        }                                                                                                           \
        for (auto& [_, modifier] : node->modifiers_) {                                                              \
            if (modifier->GetModifierType() == RSModifierType::propertyType) {                                      \
                value operator std::static_pointer_cast<RSProperty<T>>(modifier->GetProperty())->Get();             \
            }                                                                                                       \
        }                                                                                                           \
        return value;                                                                                               \
    } while (0)

#define GET_PROPERTY_FROM_MODIFIERS_EQRETURN(T, propertyType, defaultValue, operator)                               \
    do {                                                                                                            \
        auto rsUIContextPtr = rsUIContext_.lock();                                                                  \
        auto node = rsUIContextPtr ? rsUIContextPtr->GetNodeMap().GetNode<RSNode>(id_)                              \
                                   : RSNodeMap::Instance().GetNode<RSNode>(id_);                                    \
        if (!node) {                                                                                                \
            return defaultValue;                                                                                    \
        }                                                                                                           \
        std::unique_lock<std::recursive_mutex> lock(node->GetPropertyMutex());                                      \
        auto typeIter = node->modifiersTypeMap_.find((int16_t)RSModifierType::propertyType);                        \
        if (typeIter != node->modifiersTypeMap_.end()) {                                                            \
            auto modifier = typeIter->second;                                                                       \
            return std::static_pointer_cast<RSProperty<T>>(modifier->GetProperty())->Get();                         \
        } else {                                                                                                    \
            return defaultValue;                                                                                    \
        }                                                                                                           \
    } while (0)
#endif

Vector4f RSModifierExtractor::GetBounds() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BOUNDS, BOUNDS, Vector4f(), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, BOUNDS, Vector4f(), =);
#endif
}

Vector4f RSModifierExtractor::GetFrame() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, FRAME, FRAME, Vector4f(), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, FRAME, Vector4f(), =);
#endif
}

float RSModifierExtractor::GetPositionZ() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, POSITION_Z, 0.f, +=);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, POSITION_Z, 0.f, +=);
#endif
}

Vector2f RSModifierExtractor::GetPivot() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector2f, TRANSFORM, PIVOT, Vector2f(0.5f, 0.5f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector2f, PIVOT, Vector2f(0.5f, 0.5f), =);
#endif
}

float RSModifierExtractor::GetPivotZ() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, PIVOT_Z, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, PIVOT_Z, 0.f, =);
#endif
}

Quaternion RSModifierExtractor::GetQuaternion() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Quaternion, TRANSFORM, QUATERNION, Quaternion(), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Quaternion, QUATERNION, Quaternion(), =);
#endif
}

float RSModifierExtractor::GetRotation() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, ROTATION, 0.f, +=);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, ROTATION, 0.f, +=);
#endif
}

float RSModifierExtractor::GetRotationX() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, ROTATION_X, 0.f, +=);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, ROTATION_X, 0.f, +=);
#endif
}

float RSModifierExtractor::GetRotationY() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, ROTATION_Y, 0.f, +=);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, ROTATION_Y, 0.f, +=);
#endif
}

float RSModifierExtractor::GetCameraDistance() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, CAMERA_DISTANCE, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, CAMERA_DISTANCE, 0.f, =);
#endif
}

Vector2f RSModifierExtractor::GetTranslate() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector2f, TRANSFORM, TRANSLATE, Vector2f(0.f, 0.0f), +=);
#else
    GET_PROPERTY_FROM_MODIFIERS(Vector2f, TRANSLATE, Vector2f(0.f, 0.f), +=);
#endif
}

float RSModifierExtractor::GetTranslateZ() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, TRANSLATE_Z, 0.f, +=);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, TRANSLATE_Z, 0.f, +=);
#endif
}

Vector2f RSModifierExtractor::GetScale() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector2f, TRANSFORM, SCALE, Vector2f(1.f, 1.f), *=);
#else
    GET_PROPERTY_FROM_MODIFIERS(Vector2f, SCALE, Vector2f(1.f, 1.f), *=);
#endif
}

float RSModifierExtractor::GetScaleZ() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, SCALE_Z, 1.f, *=);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, SCALE_Z, 1.f, *=);
#endif
}

Vector3f RSModifierExtractor::GetSkew() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector3f, TRANSFORM, SKEW, Vector3f(0.f, 0.f, 0.f), +=);
#else
    GET_PROPERTY_FROM_MODIFIERS(Vector3f, SKEW, Vector3f(0.f, 0.f, 0.f), +=);
#endif
}

Vector4f RSModifierExtractor::GetPersp() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, TRANSFORM, PERSP, Vector4f(0.f, 0.f, 0.f, 1.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS(Vector4f, PERSP, Vector4f(0.f, 0.f, 0.f, 1.f), =);
#endif
}

float RSModifierExtractor::GetAlpha() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, ALPHA, ALPHA, 1.f, *=);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, ALPHA, 1.f, *=);
#endif
}

bool RSModifierExtractor::GetAlphaOffscreen() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, ALPHA, ALPHA_OFFSCREEN, true, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(bool, ALPHA_OFFSCREEN, true, =);
#endif
}

Vector4f RSModifierExtractor::GetCornerRadius() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, CLIP_TO_BOUNDS, CORNER_RADIUS, Vector4f(), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, CORNER_RADIUS, Vector4f(), =);
#endif
}

Color RSModifierExtractor::GetForegroundColor() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, FOREGROUND_COLOR, FOREGROUND_COLOR, RgbPalette::Transparent(), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Color, FOREGROUND_COLOR, RgbPalette::Transparent(), =);
#endif
}

Color RSModifierExtractor::GetBackgroundColor() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, BACKGROUND_COLOR, BACKGROUND_COLOR, RgbPalette::Transparent(), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Color, BACKGROUND_COLOR, RgbPalette::Transparent(), =);
#endif
}

std::shared_ptr<RSShader> RSModifierExtractor::GetBackgroundShader() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSShader>, BACKGROUND_SHADER, BACKGROUND_SHADER, nullptr, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(std::shared_ptr<RSShader>, BACKGROUND_SHADER, nullptr, =);
#endif
}

std::shared_ptr<RSImage> RSModifierExtractor::GetBgImage() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSImage>, BACKGROUND_IMAGE, BG_IMAGE, nullptr, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(std::shared_ptr<RSImage>, BG_IMAGE, nullptr, =);
#endif
}

#if defined(MODIFIER_NG)
Vector4f RSModifierExtractor::GetBgImageDstRect() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BACKGROUND_IMAGE, BG_IMAGE_RECT, Vector4f(), =);
}
#endif

float RSModifierExtractor::GetBgImageWidth() const
{
#if defined(MODIFIER_NG)
    Vector4f imageRect = GetBgImageDstRect();
    return imageRect[2]; // 2 index of width
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, BG_IMAGE_WIDTH, 0.f, =);
#endif
}

float RSModifierExtractor::GetBgImageHeight() const
{
#if defined(MODIFIER_NG)
    Vector4f imageRect = GetBgImageDstRect();
    return imageRect[3]; // 3 index of height
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, BG_IMAGE_HEIGHT, 0.f, =);
#endif
}

float RSModifierExtractor::GetBgImagePositionX() const
{
#if defined(MODIFIER_NG)
    Vector4f imageRect = GetBgImageDstRect();
    return imageRect[0]; // 0 index of position x
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, BG_IMAGE_POSITION_X, 0.f, =);
#endif
}

float RSModifierExtractor::GetBgImagePositionY() const
{
#if defined(MODIFIER_NG)
    Vector4f imageRect = GetBgImageDstRect();
    return imageRect[1]; // 1 index of position y
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, BG_IMAGE_POSITION_Y, 0.f, =);
#endif
}

Vector4<Color> RSModifierExtractor::GetBorderColor() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4<Color>, BORDER, BORDER_COLOR, Vector4<Color>(RgbPalette::Transparent()), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4<Color>, BORDER_COLOR, Vector4<Color>(RgbPalette::Transparent()), =);
#endif
}

Vector4f RSModifierExtractor::GetBorderWidth() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BORDER, BORDER_WIDTH, Vector4f(0.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, BORDER_WIDTH, Vector4f(0.f), =);
#endif
}

Vector4<uint32_t> RSModifierExtractor::GetBorderStyle() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(
        Vector4<uint32_t>, BORDER, BORDER_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE)), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(
        Vector4<uint32_t>, BORDER_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE)), =);
#endif
}

Vector4f RSModifierExtractor::GetBorderDashWidth() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BORDER, BORDER_DASH_WIDTH, Vector4f(0.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, BORDER_DASH_WIDTH, Vector4f(0.f), =);
#endif
}

Vector4f RSModifierExtractor::GetBorderDashGap() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BORDER, BORDER_DASH_GAP, Vector4f(0.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, BORDER_DASH_GAP, Vector4f(0.f), =);
#endif
}

Vector4<Color> RSModifierExtractor::GetOutlineColor() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(
        Vector4<Color>, OUTLINE, OUTLINE_COLOR, Vector4<Color>(RgbPalette::Transparent()), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4<Color>, OUTLINE_COLOR, Vector4<Color>(RgbPalette::Transparent()), =);
#endif
}

Vector4f RSModifierExtractor::GetOutlineWidth() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, OUTLINE, OUTLINE_WIDTH, Vector4f(0.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, OUTLINE_WIDTH, Vector4f(0.f), =);
#endif
}

Vector4<uint32_t> RSModifierExtractor::GetOutlineStyle() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(
        Vector4<uint32_t>, OUTLINE, OUTLINE_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE)), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(
        Vector4<uint32_t>, OUTLINE_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE)), =);
#endif
}

Vector4f RSModifierExtractor::GetOutlineDashWidth() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, OUTLINE, OUTLINE_DASH_WIDTH, Vector4f(0.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, OUTLINE_DASH_WIDTH, Vector4f(0.f), =);
#endif
}

Vector4f RSModifierExtractor::GetOutlineDashGap() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, OUTLINE, OUTLINE_DASH_GAP, Vector4f(0.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, OUTLINE_DASH_GAP, Vector4f(0.f), =);
#endif
}

Vector4f RSModifierExtractor::GetOutlineRadius() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, OUTLINE, OUTLINE_RADIUS, Vector4f(0.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, OUTLINE_RADIUS, Vector4f(0.f), =);
#endif
}

float RSModifierExtractor::GetForegroundEffectRadius() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, FOREGROUND_FILTER, FOREGROUND_EFFECT_RADIUS, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, FOREGROUND_EFFECT_RADIUS, 0.f, =);
#endif
}

Color RSModifierExtractor::GetShadowColor() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, SHADOW, SHADOW_COLOR, Color::FromArgbInt(DEFAULT_SPOT_COLOR), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Color, SHADOW_COLOR, Color::FromArgbInt(DEFAULT_SPOT_COLOR), =);
#endif
}

float RSModifierExtractor::GetShadowOffsetX() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_OFFSET_X, DEFAULT_SHADOW_OFFSET_X, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, SHADOW_OFFSET_X, DEFAULT_SHADOW_OFFSET_X, =);
#endif
}

float RSModifierExtractor::GetShadowOffsetY() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_OFFSET_Y, DEFAULT_SHADOW_OFFSET_Y, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, SHADOW_OFFSET_Y, DEFAULT_SHADOW_OFFSET_Y, =);
#endif
}

float RSModifierExtractor::GetShadowAlpha() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_ALPHA, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, SHADOW_ALPHA, 0.f, =);
#endif
}

float RSModifierExtractor::GetShadowElevation() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_ELEVATION, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, SHADOW_ELEVATION, 0.f, =);
#endif
}

float RSModifierExtractor::GetShadowRadius() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_RADIUS, DEFAULT_SHADOW_RADIUS, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, SHADOW_RADIUS, DEFAULT_SHADOW_RADIUS, =);
#endif
}

std::shared_ptr<RSPath> RSModifierExtractor::GetShadowPath() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSPath>, SHADOW, SHADOW_PATH, nullptr, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(std::shared_ptr<RSPath>, SHADOW_PATH, nullptr, =);
#endif
}

int RSModifierExtractor::GetShadowMask() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(int, SHADOW, SHADOW_MASK, SHADOW_MASK_STRATEGY::MASK_NONE, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(int, SHADOW_MASK, SHADOW_MASK_STRATEGY::MASK_NONE, =);
#endif
}

bool RSModifierExtractor::GetShadowIsFilled() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, SHADOW, SHADOW_IS_FILLED, false, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(bool, SHADOW_IS_FILLED, false, =);
#endif
}

int RSModifierExtractor::GetShadowColorStrategy() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(int, SHADOW, SHADOW_COLOR_STRATEGY, SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(int, SHADOW_COLOR_STRATEGY, SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE, =);
#endif
}

Gravity RSModifierExtractor::GetFrameGravity() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Gravity, CLIP_TO_FRAME, FRAME_GRAVITY, Gravity::DEFAULT, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Gravity, FRAME_GRAVITY, Gravity::DEFAULT, =);
#endif
}

std::shared_ptr<RSPath> RSModifierExtractor::GetClipBounds() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSPath>, CLIP_TO_BOUNDS, CLIP_BOUNDS, nullptr, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(std::shared_ptr<RSPath>, CLIP_BOUNDS, nullptr, =);
#endif
}

bool RSModifierExtractor::GetClipToBounds() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, CLIP_TO_BOUNDS, CLIP_TO_BOUNDS, false, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(bool, CLIP_TO_BOUNDS, false, =);
#endif
}

bool RSModifierExtractor::GetClipToFrame() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, CLIP_TO_FRAME, CLIP_TO_FRAME, false, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(bool, CLIP_TO_FRAME, false, =);
#endif
}

bool RSModifierExtractor::GetVisible() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, VISIBILITY, VISIBLE, true, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(bool, VISIBLE, true, =);
#endif
}

std::shared_ptr<RSMask> RSModifierExtractor::GetMask() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSMask>, MASK, MASK, nullptr, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(std::shared_ptr<RSMask>, MASK, nullptr, =);
#endif
}

float RSModifierExtractor::GetSpherizeDegree() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, FOREGROUND_FILTER, SPHERIZE, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, SPHERIZE, 0.f, =);
#endif
}

float RSModifierExtractor::GetHDRUIBrightness() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, HDR_BRIGHTNESS, HDR_UI_BRIGHTNESS, 1.0f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, HDR_UI_BRIGHTNESS, 1.0f, =);
#endif
}

float RSModifierExtractor::GetAttractionFractionValue() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, FOREGROUND_FILTER, ATTRACTION_FRACTION, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, ATTRACTION_FRACTION, 0.f, =);
#endif
}

Vector2f RSModifierExtractor::GetAttractionDstPointValue() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector2f, FOREGROUND_FILTER, ATTRACTION_DSTPOINT, Vector2f(0.f, 0.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS(Vector2f, ATTRACTION_DSTPOINT, Vector2f(0.f, 0.f), =);
#endif
}

float RSModifierExtractor::GetHDRBrightnessFactor() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, HDR_BRIGHTNESS, HDR_BRIGHTNESS_FACTOR, 1.0f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, HDR_BRIGHTNESS_FACTOR, 1.0f, =);
#endif
}

float RSModifierExtractor::GetLightUpEffectDegree() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, LIGHT_UP_EFFECT_DEGREE, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, LIGHT_UP_EFFECT, 0.f, =);
#endif
}

float RSModifierExtractor::GetDynamicDimDegree() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, DYNAMIC_DIM_DEGREE, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, DYNAMIC_DIM_DEGREE, 0.f, =);
#endif
}

float RSModifierExtractor::GetBackgroundBlurRadius() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_RADIUS, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, BACKGROUND_BLUR_RADIUS, 0.f, =);
#endif
}

float RSModifierExtractor::GetBackgroundBlurSaturation() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_SATURATION, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, BACKGROUND_BLUR_SATURATION, 0.f, =);
#endif
}

float RSModifierExtractor::GetBackgroundBlurBrightness() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_BRIGHTNESS, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, BACKGROUND_BLUR_BRIGHTNESS, 0.f, =);
#endif
}

Color RSModifierExtractor::GetBackgroundBlurMaskColor() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, BACKGROUND_FILTER, BACKGROUND_BLUR_MASK_COLOR, RSColor(), =);
#else
    GET_PROPERTY_FROM_MODIFIERS(Color, BACKGROUND_BLUR_MASK_COLOR, RSColor(), =);
#endif
}

int RSModifierExtractor::GetBackgroundBlurColorMode() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(int, BACKGROUND_FILTER, BACKGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(int, BACKGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT, =);
#endif
}

float RSModifierExtractor::GetBackgroundBlurRadiusX() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_RADIUS_X, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, BACKGROUND_BLUR_RADIUS_X, 0.f, =);
#endif
}

float RSModifierExtractor::GetBackgroundBlurRadiusY() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_RADIUS_Y, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, BACKGROUND_BLUR_RADIUS_Y, 0.f, =);
#endif
}

bool RSModifierExtractor::GetBgBlurDisableSystemAdaptation() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, BACKGROUND_FILTER, BG_BLUR_DISABLE_SYSTEM_ADAPTATION, true, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(bool, BG_BLUR_DISABLE_SYSTEM_ADAPTATION, true, =);
#endif
}

bool RSModifierExtractor::GetAlwaysSnapshot() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, BACKGROUND_FILTER, ALWAYS_SNAPSHOT, false, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(bool, ALWAYS_SNAPSHOT, false, =);
#endif
}

float RSModifierExtractor::GetForegroundBlurRadius() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_RADIUS, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, FOREGROUND_BLUR_RADIUS, 0.f, =);
#endif
}

float RSModifierExtractor::GetForegroundBlurSaturation() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_SATURATION, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, FOREGROUND_BLUR_SATURATION, 0.f, =);
#endif
}

float RSModifierExtractor::GetForegroundBlurBrightness() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_BRIGHTNESS, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, FOREGROUND_BLUR_BRIGHTNESS, 0.f, =);
#endif
}

Color RSModifierExtractor::GetForegroundBlurMaskColor() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, COMPOSITING_FILTER, FOREGROUND_BLUR_MASK_COLOR, RSColor(), =);
#else
    GET_PROPERTY_FROM_MODIFIERS(Color, FOREGROUND_BLUR_MASK_COLOR, RSColor(), =);
#endif
}

int RSModifierExtractor::GetForegroundBlurColorMode() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(int, COMPOSITING_FILTER, FOREGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(int, FOREGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT, =);
#endif
}

float RSModifierExtractor::GetForegroundBlurRadiusX() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_RADIUS_X, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, FOREGROUND_BLUR_RADIUS_X, 0.f, =);
#endif
}

float RSModifierExtractor::GetForegroundBlurRadiusY() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_RADIUS_Y, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(float, FOREGROUND_BLUR_RADIUS_Y, 0.f, =);
#endif
}

bool RSModifierExtractor::GetFgBlurDisableSystemAdaptation() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, COMPOSITING_FILTER, FG_BLUR_DISABLE_SYSTEM_ADAPTATION, true, =);
#else
    GET_PROPERTY_FROM_MODIFIERS(bool, FG_BLUR_DISABLE_SYSTEM_ADAPTATION, true, =);
#endif
}

float RSModifierExtractor::GetLightIntensity() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, POINT_LIGHT, LIGHT_INTENSITY, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, LIGHT_INTENSITY, 0.f, =);
#endif
}

Vector4f RSModifierExtractor::GetLightPosition() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, POINT_LIGHT, LIGHT_POSITION, Vector4f(0.f), =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(Vector4f, LIGHT_POSITION, Vector4f(0.f), =);
#endif
}

float RSModifierExtractor::GetIlluminatedBorderWidth() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, POINT_LIGHT, ILLUMINATED_BORDER_WIDTH, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, ILLUMINATED_BORDER_WIDTH, 0.f, =);
#endif
}

int RSModifierExtractor::GetIlluminatedType() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(int, POINT_LIGHT, ILLUMINATED_TYPE, 0, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(int, ILLUMINATED_TYPE, 0, =);
#endif
}

float RSModifierExtractor::GetBloom() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(float, POINT_LIGHT, BLOOM, 0.f, =);
#else
    GET_PROPERTY_FROM_MODIFIERS_EQRETURN(float, BLOOM, 0.f, =);
#endif
}

Color RSModifierExtractor::GetLightColor() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, POINT_LIGHT, LIGHT_COLOR, RgbPalette::White(), =);
#else
    GET_PROPERTY_FROM_MODIFIERS(Color, LIGHT_COLOR, RgbPalette::White(), =);
#endif
}

int RSModifierExtractor::GetColorBlendMode() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(int, BLENDER, COLOR_BLEND_MODE, static_cast<int>(RSColorBlendMode::NONE), =);
#else
    GET_PROPERTY_FROM_MODIFIERS(int, COLOR_BLEND_MODE, static_cast<int>(RSColorBlendMode::NONE), =);
#endif
}

int RSModifierExtractor::GetColorBlendApplyType() const
{
#if defined(MODIFIER_NG)
    GET_PROPERTY_FROM_MODIFIERS_NG(
        int, BLENDER, COLOR_BLEND_APPLY_TYPE, static_cast<int>(RSColorBlendApplyType::FAST), =);
#else
    GET_PROPERTY_FROM_MODIFIERS(int, COLOR_BLEND_APPLY_TYPE, static_cast<int>(RSColorBlendApplyType::FAST), =);
#endif
}

std::string RSModifierExtractor::Dump() const
{
    std::string dumpInfo;
    char buffer[UINT8_MAX] = { 0 };
    if (auto rsUIContextPtr = rsUIContext_.lock()) {
        if (sprintf_s(buffer, UINT8_MAX, "modifierExtractor rsUIContext token is %llu",
                      rsUIContextPtr ? rsUIContextPtr->GetToken() : 0) != -1) {
            dumpInfo.append(buffer);
        }
        if (rsUIContextPtr->GetNodeMap().GetNode<RSNode>(id_) != nullptr) {
            sprintf_s(buffer, UINT8_MAX, "node Is %d ", 1);
        } else {
            sprintf_s(buffer, UINT8_MAX, "node Is %d ", 0);
        }
        dumpInfo.append(buffer);
    }

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

    // BackgroundFilter
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for BackgroundFilter, ret=" + std::to_string(ret);
    }
    float radius = GetBackgroundBlurRadius();
    if (ROSEN_GNE(radius, 0.0f)) {
        float saturation = GetBackgroundBlurSaturation();
        float brightness = GetBackgroundBlurBrightness();
        Color maskColor = GetBackgroundBlurMaskColor();
        int colorMode = GetBackgroundBlurColorMode();
        if (sprintf_s(buffer, UINT8_MAX, ", BackgroundFilter[radius: %.2f, saturation: %.2f, brightness: %.2f, "
            "maskColor: %08X, colorMode: %d]", radius, saturation, brightness, maskColor.AsArgbInt(),
            colorMode) != -1) {
            dumpInfo.append(buffer);
        }
    }

    if (!GetVisible()) {
        dumpInfo.append(", IsVisible[false]");
    }
    return dumpInfo;
}
} // namespace Rosen
} // namespace OHOS
