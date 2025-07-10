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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_RS_MODIFIER_NG_TYPE_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_RS_MODIFIER_NG_TYPE_H

#include <bitset>
#include <string>
#include <unordered_map>

namespace OHOS::Rosen::ModifierNG {
enum class RSModifierType : uint16_t {
    INVALID = 0,

    BOUNDS = 1,
    FRAME = 2,
    TRANSFORM = 3,
    ALPHA = 4,

    FOREGROUND_COLOR = 5,
    BACKGROUND_COLOR = 6,
    BACKGROUND_SHADER = 7,
    BACKGROUND_IMAGE = 8,

    BORDER = 9,
    OUTLINE = 10,
    CLIP_TO_BOUNDS = 11,
    CLIP_TO_FRAME = 12,
    VISIBILITY = 13,

    DYNAMIC_LIGHT_UP = 14,
    SHADOW = 15,
    MASK = 16,
    PIXEL_STRETCH = 17,
    USE_EFFECT = 18,
    BLENDER = 19,

    POINT_LIGHT = 20,
    PARTICLE_EFFECT = 21,
    COMPOSITING_FILTER = 22,
    BACKGROUND_FILTER = 23,
    FOREGROUND_FILTER = 24,

    TRANSITION_STYLE = 25,
    BACKGROUND_STYLE = 26,
    CONTENT_STYLE = 27,
    FOREGROUND_STYLE = 28,
    OVERLAY_STYLE = 29,
    NODE_MODIFIER = 30,

    ENV_FOREGROUND_COLOR = 31,
    HDR_BRIGHTNESS = 32,
    BEHIND_WINDOW_FILTER = 33,
    BACKGROUND_NG_SHADER = 34,
    FOREGROUND_SHADER = 35,

    CHILDREN, // PLACEHOLDER, no such modifier, but we need a dirty flag

    MAX = CHILDREN + 1,
};

enum class RSPropertyType : uint16_t {
#define X(name) name,
#include "modifier_ng/rs_property_ng_type.in"
#undef X
};

class ModifierTypeConvertor {
public:
    static RSPropertyType GetPropertyType(RSModifierType modifierTypeNG)
    {
        auto it = modifierToPropertyMap_.find(modifierTypeNG);
        if (it != modifierToPropertyMap_.end()) {
            return it->second;
        }
        return RSPropertyType::INVALID;
    }

private:
    static inline std::unordered_map<RSModifierType, RSPropertyType> modifierToPropertyMap_ = {
        { RSModifierType::TRANSITION_STYLE, RSPropertyType::TRANSITION_STYLE },
        { RSModifierType::BACKGROUND_STYLE, RSPropertyType::BACKGROUND_STYLE },
        { RSModifierType::CONTENT_STYLE, RSPropertyType::CONTENT_STYLE },
        { RSModifierType::FOREGROUND_STYLE, RSPropertyType::FOREGROUND_STYLE },
        { RSModifierType::OVERLAY_STYLE, RSPropertyType::OVERLAY_STYLE },
        { RSModifierType::NODE_MODIFIER, RSPropertyType::NODE_MODIFIER },
    };
};

class RSModifierTypeString {
public:
    static std::string GetPropertyTypeString(RSPropertyType type)
    {
        switch (type) {
            case RSPropertyType::INVALID: return "Invalid";
            case RSPropertyType::BOUNDS: return "Bounds";
            case RSPropertyType::FRAME: return "Frame";
            case RSPropertyType::POSITION_Z: return "PositionZ";
            case RSPropertyType::POSITION_Z_APPLICABLE_CAMERA3D: return "PositionZApplicableCamera3d";
            case RSPropertyType::PIVOT: return "Pivot";
            case RSPropertyType::PIVOT_Z: return "PivotZ";
            case RSPropertyType::QUATERNION: return "Quaternion";
            case RSPropertyType::ROTATION: return "Rotation";
            case RSPropertyType::ROTATION_X: return "RotationX";
            case RSPropertyType::ROTATION_Y: return "RotationY";
            case RSPropertyType::CAMERA_DISTANCE: return "CameraDistance";
            case RSPropertyType::SCALE: return "Scale";
            case RSPropertyType::SCALE_Z: return "ScaleZ";
            case RSPropertyType::SKEW: return "Skew";
            case RSPropertyType::PERSP: return "Persp";
            case RSPropertyType::TRANSLATE: return "Translate";
            case RSPropertyType::TRANSLATE_Z: return "TranslateZ";
            case RSPropertyType::SUBLAYER_TRANSFORM: return "SublayerTransform";
            case RSPropertyType::CORNER_RADIUS: return "CornerRadius";
            case RSPropertyType::ALPHA: return "Alpha";
            case RSPropertyType::ALPHA_OFFSCREEN: return "AlphaOffscreen";
            case RSPropertyType::FOREGROUND_COLOR: return "ForegroundColor";
            case RSPropertyType::BACKGROUND_COLOR: return "BackgroundColor";
            case RSPropertyType::BACKGROUND_SHADER: return "BackgroundShader";
            case RSPropertyType::BACKGROUND_SHADER_PROGRESS: return "BackgroundShaderProgress";
            case RSPropertyType::BACKGROUND_NG_SHADER: return "BackgroundNGShader";
            case RSPropertyType::BG_IMAGE: return "BgImage";
            case RSPropertyType::BG_IMAGE_INNER_RECT: return "BgImageInnerRect";
            case RSPropertyType::BG_IMAGE_WIDTH: return "BgImageWidth";
            case RSPropertyType::BG_IMAGE_HEIGHT: return "BgImageHeight";
            case RSPropertyType::BG_IMAGE_POSITION_X: return "BgImagePositionX";
            case RSPropertyType::BG_IMAGE_POSITION_Y: return "BgImagePositionY";
            case RSPropertyType::BG_IMAGE_RECT: return "BgImageRect";
            case RSPropertyType::BORDER_COLOR: return "BorderColor";
            case RSPropertyType::BORDER_WIDTH: return "BorderWidth";
            case RSPropertyType::BORDER_STYLE: return "BorderStyle";
            case RSPropertyType::BORDER_DASH_WIDTH: return "BorderDashWidth";
            case RSPropertyType::BORDER_DASH_GAP: return "BorderDashGap";
            case RSPropertyType::BACKGROUND_FILTER: return "BackgroundFilter";
            case RSPropertyType::LINEAR_GRADIENT_BLUR_PARA: return "LinearGradientBlurPara";
            case RSPropertyType::DYNAMIC_LIGHT_UP_RATE: return "DynamicLightUpRate";
            case RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE: return "DynamicLightUpDegree";
            case RSPropertyType::FG_BRIGHTNESS_RATES: return "FgBrightnessRates";
            case RSPropertyType::FG_BRIGHTNESS_SATURATION: return "FgBrightnessSaturation";
            case RSPropertyType::FG_BRIGHTNESS_POSCOEFF: return "FgBrightnessPoscoeff";
            case RSPropertyType::FG_BRIGHTNESS_NEGCOEFF: return "FgBrightnessNegcoeff";
            case RSPropertyType::FG_BRIGHTNESS_FRACTION: return "FgBrightnessFraction";
            case RSPropertyType::FG_BRIGHTNESS_HDR: return "FgBrightnessHdr";
            case RSPropertyType::BG_BRIGHTNESS_RATES: return "BgBrightnessRates";
            case RSPropertyType::BG_BRIGHTNESS_SATURATION: return "BgBrightnessSaturation";
            case RSPropertyType::BG_BRIGHTNESS_POSCOEFF: return "BgBrightnessPoscoeff";
            case RSPropertyType::BG_BRIGHTNESS_NEGCOEFF: return "BgBrightnessNegcoeff";
            case RSPropertyType::BG_BRIGHTNESS_FRACTION: return "BgBrightnessFraction";
            case RSPropertyType::SHADOW_BLENDER_PARAMS: return "ShadowBlenderParams";
            case RSPropertyType::FRAME_GRAVITY: return "FrameGravity";
            case RSPropertyType::CLIP_RRECT: return "ClipRrect";
            case RSPropertyType::CLIP_BOUNDS: return "ClipBounds";
            case RSPropertyType::CLIP_TO_BOUNDS: return "ClipToBounds";
            case RSPropertyType::CLIP_TO_FRAME: return "ClipToFrame";
            case RSPropertyType::VISIBLE: return "Visible";
            case RSPropertyType::SHADOW_COLOR: return "ShadowColor";
            case RSPropertyType::SHADOW_OFFSET_X: return "ShadowOffsetX";
            case RSPropertyType::SHADOW_OFFSET_Y: return "ShadowOffsetY";
            case RSPropertyType::SHADOW_ALPHA: return "ShadowAlpha";
            case RSPropertyType::SHADOW_ELEVATION: return "ShadowElevation";
            case RSPropertyType::SHADOW_RADIUS: return "ShadowRadius";
            case RSPropertyType::SHADOW_PATH: return "ShadowPath";
            case RSPropertyType::SHADOW_MASK: return "ShadowMask";
            case RSPropertyType::SHADOW_COLOR_STRATEGY: return "ShadowColorStrategy";
            case RSPropertyType::MASK: return "Mask";
            case RSPropertyType::SPHERIZE: return "Spherize";
            case RSPropertyType::LIGHT_UP_EFFECT_DEGREE: return "LightUpEffectDegree";
            case RSPropertyType::PIXEL_STRETCH_SIZE: return "PixelStretchSize";
            case RSPropertyType::PIXEL_STRETCH_PERCENT: return "PixelStretchPercent";
            case RSPropertyType::PIXEL_STRETCH_TILE_MODE: return "PixelStretchTileMode";
            case RSPropertyType::USE_EFFECT: return "UseEffect";
            case RSPropertyType::USE_EFFECT_TYPE: return "UseEffectType";
            case RSPropertyType::COLOR_BLEND_MODE: return "ColorBlendMode";
            case RSPropertyType::COLOR_BLEND_APPLY_TYPE: return "ColorBlendApplyType";
            case RSPropertyType::SANDBOX: return "Sandbox";
            case RSPropertyType::GRAY_SCALE: return "GrayScale";
            case RSPropertyType::BRIGHTNESS: return "Brightness";
            case RSPropertyType::CONTRAST: return "Contrast";
            case RSPropertyType::SATURATE: return "Saturate";
            case RSPropertyType::SEPIA: return "Sepia";
            case RSPropertyType::INVERT: return "Invert";
            case RSPropertyType::AIINVERT: return "Aiinvert";
            case RSPropertyType::SYSTEMBAREFFECT: return "Systembareffect";
            case RSPropertyType::WATER_RIPPLE_PROGRESS: return "WaterRippleProgress";
            case RSPropertyType::WATER_RIPPLE_PARAMS: return "WaterRippleParams";
            case RSPropertyType::HUE_ROTATE: return "HueRotate";
            case RSPropertyType::COLOR_BLEND: return "ColorBlend";
            case RSPropertyType::PARTICLE: return "Particle";
            case RSPropertyType::SHADOW_IS_FILLED: return "ShadowIsFilled";
            case RSPropertyType::OUTLINE_COLOR: return "OutlineColor";
            case RSPropertyType::OUTLINE_WIDTH: return "OutlineWidth";
            case RSPropertyType::OUTLINE_STYLE: return "OutlineStyle";
            case RSPropertyType::OUTLINE_DASH_WIDTH: return "OutlineDashWidth";
            case RSPropertyType::OUTLINE_DASH_GAP: return "OutlineDashGap";
            case RSPropertyType::OUTLINE_RADIUS: return "OutlineRadius";
            case RSPropertyType::GREY_COEF: return "GreyCoef";
            case RSPropertyType::LIGHT_INTENSITY: return "LightIntensity";
            case RSPropertyType::LIGHT_COLOR: return "LightColor";
            case RSPropertyType::LIGHT_POSITION: return "LightPosition";
            case RSPropertyType::ILLUMINATED_BORDER_WIDTH: return "IlluminatedBorderWidth";
            case RSPropertyType::ILLUMINATED_TYPE: return "IlluminatedType";
            case RSPropertyType::BLOOM: return "Bloom";
            case RSPropertyType::FOREGROUND_EFFECT_RADIUS: return "ForegroundEffectRadius";
            case RSPropertyType::USE_SHADOW_BATCHING: return "UseShadowBatching";
            case RSPropertyType::MOTION_BLUR_PARA: return "MotionBlurPara";
            case RSPropertyType::PARTICLE_EMITTER_UPDATER: return "ParticleEmitterUpdater";
            case RSPropertyType::PARTICLE_NOISE_FIELD: return "ParticleNoiseField";
            case RSPropertyType::FLY_OUT_DEGREE: return "FlyOutDegree";
            case RSPropertyType::FLY_OUT_PARAMS: return "FlyOutParams";
            case RSPropertyType::DISTORTION_K: return "DistortionK";
            case RSPropertyType::DYNAMIC_DIM_DEGREE: return "DynamicDimDegree";
            case RSPropertyType::MAGNIFIER_PARA: return "MagnifierPara";
            case RSPropertyType::BACKGROUND_BLUR_RADIUS: return "BackgroundBlurRadius";
            case RSPropertyType::BACKGROUND_BLUR_SATURATION: return "BackgroundBlurSaturation";
            case RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS: return "BackgroundBlurBrightness";
            case RSPropertyType::BACKGROUND_BLUR_MASK_COLOR: return "BackgroundBlurMaskColor";
            case RSPropertyType::BACKGROUND_BLUR_COLOR_MODE: return "BackgroundBlurColorMode";
            case RSPropertyType::BACKGROUND_BLUR_RADIUS_X: return "BackgroundBlurRadiusX";
            case RSPropertyType::BACKGROUND_BLUR_RADIUS_Y: return "BackgroundBlurRadiusY";
            case RSPropertyType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION: return "BgBlurDisableSystemAdaptation";
            case RSPropertyType::ALWAYS_SNAPSHOT: return "AlwaysSnapshot";
            case RSPropertyType::COMPLEX_SHADER_PARAM: return "ComplexShaderParam";
            case RSPropertyType::FOREGROUND_BLUR_RADIUS: return "ForegroundBlurRadius";
            case RSPropertyType::FOREGROUND_BLUR_SATURATION: return "ForegroundBlurSaturation";
            case RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS: return "ForegroundBlurBrightness";
            case RSPropertyType::FOREGROUND_BLUR_MASK_COLOR: return "ForegroundBlurMaskColor";
            case RSPropertyType::FOREGROUND_BLUR_COLOR_MODE: return "ForegroundBlurColorMode";
            case RSPropertyType::FOREGROUND_BLUR_RADIUS_X: return "ForegroundBlurRadiusX";
            case RSPropertyType::FOREGROUND_BLUR_RADIUS_Y: return "ForegroundBlurRadiusY";
            case RSPropertyType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION: return "FgBlurDisableSystemAdaptation";
            case RSPropertyType::ATTRACTION_FRACTION: return "AttractionFraction";
            case RSPropertyType::ATTRACTION_DSTPOINT: return "AttractionDstpoint";
            case RSPropertyType::CUSTOM: return "Custom";
            case RSPropertyType::CUSTOM_INDEX: return "CustomIndex";
            case RSPropertyType::TRANSITION_STYLE: return "TransitionStyle";
            case RSPropertyType::BACKGROUND_STYLE: return "BackgroundStyle";
            case RSPropertyType::CONTENT_STYLE: return "ContentStyle";
            case RSPropertyType::FOREGROUND_STYLE: return "ForegroundStyle";
            case RSPropertyType::OVERLAY_STYLE: return "OverlayStyle";
            case RSPropertyType::NODE_MODIFIER: return "NodeModifier";
            case RSPropertyType::ENV_FOREGROUND_COLOR: return "EnvForegroundColor";
            case RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY: return "EnvForegroundColorStrategy";
            case RSPropertyType::CUSTOM_CLIP_TO_FRAME: return "CustomClipToFrame";
            case RSPropertyType::HDR_BRIGHTNESS: return "HdrBrightness";
            case RSPropertyType::HDR_UI_BRIGHTNESS: return "HDRUIBrightness";
            case RSPropertyType::HDR_BRIGHTNESS_FACTOR: return "HdrBrightnessFactor";
            case RSPropertyType::BACKGROUND_UI_FILTER: return "BackgroundUIFilter";
            case RSPropertyType::FOREGROUND_UI_FILTER: return "ForegroundUIFilter";
            case RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS: return "BehindWindowFilterRadius";
            case RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION: return "BehindWindowFilterSaturation";
            case RSPropertyType::BEHIND_WINDOW_FILTER_BRIGHTNESS: return "BehindWindowFilterBrightness";
            case RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR: return "BehindWindowFilterMaskColor";
            case RSPropertyType::FOREGROUND_SHADER: return "ForegroundShader";
            case RSPropertyType::CHILDREN: return "Children";
            default: return "Unknown";
        }
        return "Unknown";
    }
};

static constexpr uint16_t MODIFIER_TYPE_COUNT = static_cast<uint16_t>(RSModifierType::MAX);
using ModifierDirtyTypes = std::bitset<static_cast<int>(MODIFIER_TYPE_COUNT)>;
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_RS_MODIFIER_NG_TYPE_H
