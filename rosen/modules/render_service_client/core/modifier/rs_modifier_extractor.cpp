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

Vector4f RSModifierExtractor::GetBounds() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BOUNDS, BOUNDS, Vector4f(), =);
}

Vector4f RSModifierExtractor::GetFrame() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, FRAME, FRAME, Vector4f(), =);
}

float RSModifierExtractor::GetPositionZ() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, POSITION_Z, 0.f, +=);
}

Vector2f RSModifierExtractor::GetPivot() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector2f, TRANSFORM, PIVOT, Vector2f(0.5f, 0.5f), =);
}

float RSModifierExtractor::GetPivotZ() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, PIVOT_Z, 0.f, =);
}

Quaternion RSModifierExtractor::GetQuaternion() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Quaternion, TRANSFORM, QUATERNION, Quaternion(), =);
}

float RSModifierExtractor::GetRotation() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, ROTATION, 0.f, +=);
}

float RSModifierExtractor::GetRotationX() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, ROTATION_X, 0.f, +=);
}

float RSModifierExtractor::GetRotationY() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, ROTATION_Y, 0.f, +=);
}

float RSModifierExtractor::GetCameraDistance() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, CAMERA_DISTANCE, 0.f, =);
}

Vector2f RSModifierExtractor::GetTranslate() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector2f, TRANSFORM, TRANSLATE, Vector2f(0.f, 0.0f), +=);
}

float RSModifierExtractor::GetTranslateZ() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, TRANSLATE_Z, 0.f, +=);
}

Vector2f RSModifierExtractor::GetScale() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector2f, TRANSFORM, SCALE, Vector2f(1.f, 1.f), *=);
}

float RSModifierExtractor::GetScaleZ() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, TRANSFORM, SCALE_Z, 1.f, *=);
}

Vector3f RSModifierExtractor::GetSkew() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector3f, TRANSFORM, SKEW, Vector3f(0.f, 0.f, 0.f), +=);
}

Vector4f RSModifierExtractor::GetPersp() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, TRANSFORM, PERSP, Vector4f(0.f, 0.f, 0.f, 1.f), =);
}

float RSModifierExtractor::GetAlpha() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, ALPHA, ALPHA, 1.f, *=);
}

bool RSModifierExtractor::GetAlphaOffscreen() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, ALPHA, ALPHA_OFFSCREEN, true, =);
}

Vector4f RSModifierExtractor::GetCornerRadius() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, CLIP_TO_BOUNDS, CORNER_RADIUS, Vector4f(), =);
}

Color RSModifierExtractor::GetForegroundColor() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, FOREGROUND_COLOR, FOREGROUND_COLOR, RgbPalette::Transparent(), =);
}

Color RSModifierExtractor::GetBackgroundColor() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, BACKGROUND_COLOR, BACKGROUND_COLOR, RgbPalette::Transparent(), =);
}

std::shared_ptr<RSShader> RSModifierExtractor::GetBackgroundShader() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSShader>, BACKGROUND_SHADER, BACKGROUND_SHADER, nullptr, =);
}

std::shared_ptr<RSImage> RSModifierExtractor::GetBgImage() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSImage>, BACKGROUND_IMAGE, BG_IMAGE, nullptr, =);
}

Vector4f RSModifierExtractor::GetBgImageDstRect() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BACKGROUND_IMAGE, BG_IMAGE_RECT, Vector4f(), =);
}

float RSModifierExtractor::GetBgImageWidth() const
{
    Vector4f imageRect = GetBgImageDstRect();
    return imageRect[2]; // 2 index of width
}

float RSModifierExtractor::GetBgImageHeight() const
{
    Vector4f imageRect = GetBgImageDstRect();
    return imageRect[3]; // 3 index of height
}

float RSModifierExtractor::GetBgImagePositionX() const
{
    Vector4f imageRect = GetBgImageDstRect();
    return imageRect[0]; // 0 index of position x
}

float RSModifierExtractor::GetBgImagePositionY() const
{
    Vector4f imageRect = GetBgImageDstRect();
    return imageRect[1]; // 1 index of position y
}

Vector4<Color> RSModifierExtractor::GetBorderColor() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4<Color>, BORDER, BORDER_COLOR, Vector4<Color>(RgbPalette::Transparent()), =);
}

Vector4f RSModifierExtractor::GetBorderWidth() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BORDER, BORDER_WIDTH, Vector4f(0.f), =);
}

Vector4<uint32_t> RSModifierExtractor::GetBorderStyle() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(
        Vector4<uint32_t>, BORDER, BORDER_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::SOLID)), =);
}

Vector4f RSModifierExtractor::GetBorderDashWidth() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BORDER, BORDER_DASH_WIDTH, Vector4f(0.f), =);
}

Vector4f RSModifierExtractor::GetBorderDashGap() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, BORDER, BORDER_DASH_GAP, Vector4f(0.f), =);
}

Vector4<Color> RSModifierExtractor::GetOutlineColor() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(
        Vector4<Color>, OUTLINE, OUTLINE_COLOR, Vector4<Color>(RgbPalette::Transparent()), =);
}

Vector4f RSModifierExtractor::GetOutlineWidth() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, OUTLINE, OUTLINE_WIDTH, Vector4f(0.f), =);
}

Vector4<uint32_t> RSModifierExtractor::GetOutlineStyle() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(
        Vector4<uint32_t>, OUTLINE, OUTLINE_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE)), =);
}

Vector4f RSModifierExtractor::GetOutlineDashWidth() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, OUTLINE, OUTLINE_DASH_WIDTH, Vector4f(0.f), =);
}

Vector4f RSModifierExtractor::GetOutlineDashGap() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, OUTLINE, OUTLINE_DASH_GAP, Vector4f(0.f), =);
}

Vector4f RSModifierExtractor::GetOutlineRadius() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, OUTLINE, OUTLINE_RADIUS, Vector4f(0.f), =);
}

float RSModifierExtractor::GetForegroundEffectRadius() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, FOREGROUND_FILTER, FOREGROUND_EFFECT_RADIUS, 0.f, =);
}

Color RSModifierExtractor::GetShadowColor() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, SHADOW, SHADOW_COLOR, Color::FromArgbInt(DEFAULT_SPOT_COLOR), =);
}

float RSModifierExtractor::GetShadowOffsetX() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_OFFSET_X, DEFAULT_SHADOW_OFFSET_X, =);
}

float RSModifierExtractor::GetShadowOffsetY() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_OFFSET_Y, DEFAULT_SHADOW_OFFSET_Y, =);
}

float RSModifierExtractor::GetShadowAlpha() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_ALPHA, 1.f, =);
}

float RSModifierExtractor::GetShadowElevation() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_ELEVATION, 0.f, =);
}

float RSModifierExtractor::GetShadowRadius() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, SHADOW, SHADOW_RADIUS, DEFAULT_SHADOW_RADIUS, =);
}

std::shared_ptr<RSPath> RSModifierExtractor::GetShadowPath() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSPath>, SHADOW, SHADOW_PATH, nullptr, =);
}

int RSModifierExtractor::GetShadowMask() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(int, SHADOW, SHADOW_MASK, SHADOW_MASK_STRATEGY::MASK_NONE, =);
}

bool RSModifierExtractor::GetShadowIsFilled() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, SHADOW, SHADOW_IS_FILLED, false, =);
}

int RSModifierExtractor::GetShadowColorStrategy() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(int, SHADOW, SHADOW_COLOR_STRATEGY, SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE, =);
}

Gravity RSModifierExtractor::GetFrameGravity() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Gravity, CLIP_TO_FRAME, FRAME_GRAVITY, Gravity::DEFAULT, =);
}

std::shared_ptr<RSPath> RSModifierExtractor::GetClipBounds() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSPath>, CLIP_TO_BOUNDS, CLIP_BOUNDS, nullptr, =);
}

bool RSModifierExtractor::GetClipToBounds() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, CLIP_TO_BOUNDS, CLIP_TO_BOUNDS, false, =);
}

bool RSModifierExtractor::GetClipToFrame() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, CLIP_TO_FRAME, CLIP_TO_FRAME, false, =);
}

bool RSModifierExtractor::GetVisible() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, VISIBILITY, VISIBLE, true, =);
}

std::shared_ptr<RSMask> RSModifierExtractor::GetMask() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(std::shared_ptr<RSMask>, MASK, MASK, nullptr, =);
}

float RSModifierExtractor::GetSpherizeDegree() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, FOREGROUND_FILTER, SPHERIZE, 0.f, =);
}

float RSModifierExtractor::GetHDRUIBrightness() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, HDR_BRIGHTNESS, HDR_UI_BRIGHTNESS, 1.0f, =);
}

float RSModifierExtractor::GetAttractionFractionValue() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, FOREGROUND_FILTER, ATTRACTION_FRACTION, 0.f, =);
}

Vector2f RSModifierExtractor::GetAttractionDstPointValue() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector2f, FOREGROUND_FILTER, ATTRACTION_DSTPOINT, Vector2f(0.f, 0.f), =);
}

float RSModifierExtractor::GetHDRBrightnessFactor() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, HDR_BRIGHTNESS, HDR_BRIGHTNESS_FACTOR, 1.0f, =);
}

float RSModifierExtractor::GetLightUpEffectDegree() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, LIGHT_UP_EFFECT_DEGREE, 0.f, =);
}

float RSModifierExtractor::GetDynamicDimDegree() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, DYNAMIC_DIM_DEGREE, 0.f, =);
}

float RSModifierExtractor::GetBackgroundBlurRadius() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_RADIUS, 0.f, =);
}

float RSModifierExtractor::GetBackgroundBlurSaturation() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_SATURATION, 0.f, =);
}

float RSModifierExtractor::GetBackgroundBlurBrightness() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_BRIGHTNESS, 0.f, =);
}

Color RSModifierExtractor::GetBackgroundBlurMaskColor() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, BACKGROUND_FILTER, BACKGROUND_BLUR_MASK_COLOR, RSColor(), =);
}

int RSModifierExtractor::GetBackgroundBlurColorMode() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(int, BACKGROUND_FILTER, BACKGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT, =);
}

float RSModifierExtractor::GetBackgroundBlurRadiusX() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_RADIUS_X, 0.f, =);
}

float RSModifierExtractor::GetBackgroundBlurRadiusY() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, BACKGROUND_FILTER, BACKGROUND_BLUR_RADIUS_Y, 0.f, =);
}

bool RSModifierExtractor::GetBgBlurDisableSystemAdaptation() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, BACKGROUND_FILTER, BG_BLUR_DISABLE_SYSTEM_ADAPTATION, true, =);
}

bool RSModifierExtractor::GetAlwaysSnapshot() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, BACKGROUND_FILTER, ALWAYS_SNAPSHOT, false, =);
}

float RSModifierExtractor::GetForegroundBlurRadius() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_RADIUS, 0.f, =);
}

float RSModifierExtractor::GetForegroundBlurSaturation() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_SATURATION, 0.f, =);
}

float RSModifierExtractor::GetForegroundBlurBrightness() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_BRIGHTNESS, 0.f, =);
}

Color RSModifierExtractor::GetForegroundBlurMaskColor() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, COMPOSITING_FILTER, FOREGROUND_BLUR_MASK_COLOR, RSColor(), =);
}

int RSModifierExtractor::GetForegroundBlurColorMode() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(int, COMPOSITING_FILTER, FOREGROUND_BLUR_COLOR_MODE, BLUR_COLOR_MODE::DEFAULT, =);
}

float RSModifierExtractor::GetForegroundBlurRadiusX() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_RADIUS_X, 0.f, =);
}

float RSModifierExtractor::GetForegroundBlurRadiusY() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, COMPOSITING_FILTER, FOREGROUND_BLUR_RADIUS_Y, 0.f, =);
}

bool RSModifierExtractor::GetFgBlurDisableSystemAdaptation() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(bool, COMPOSITING_FILTER, FG_BLUR_DISABLE_SYSTEM_ADAPTATION, true, =);
}

float RSModifierExtractor::GetLightIntensity() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, POINT_LIGHT, LIGHT_INTENSITY, 0.f, =);
}

Vector4f RSModifierExtractor::GetLightPosition() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Vector4f, POINT_LIGHT, LIGHT_POSITION, Vector4f(0.f), =);
}

float RSModifierExtractor::GetIlluminatedBorderWidth() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, POINT_LIGHT, ILLUMINATED_BORDER_WIDTH, 0.f, =);
}

int RSModifierExtractor::GetIlluminatedType() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(int, POINT_LIGHT, ILLUMINATED_TYPE, 0, =);
}

float RSModifierExtractor::GetBloom() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(float, POINT_LIGHT, BLOOM, 0.f, =);
}

Color RSModifierExtractor::GetLightColor() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(Color, POINT_LIGHT, LIGHT_COLOR, RgbPalette::White(), =);
}

int RSModifierExtractor::GetColorBlendMode() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(int, BLENDER, COLOR_BLEND_MODE, static_cast<int>(RSColorBlendMode::NONE), =);
}

int RSModifierExtractor::GetColorBlendApplyType() const
{
    GET_PROPERTY_FROM_MODIFIERS_NG(
        int, BLENDER, COLOR_BLEND_APPLY_TYPE, static_cast<int>(RSColorBlendApplyType::FAST), =);
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
