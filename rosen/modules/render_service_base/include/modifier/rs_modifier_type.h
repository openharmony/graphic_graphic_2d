/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_TYPE_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_TYPE_H

#include <bitset>
#include <cstdint>
#include <map>

namespace OHOS {
namespace Rosen {
// NOTE:
// 1. Following LUTs must be updated according when this enum is updated :
//    a. g_propertyResetterLUT in rs_properties.cpp
//    b. g_propertyToDrawableLut in rs_drawable_content.cpp
// 2. Property modifier(i.e. to be applied to RSProperties) MUST be added before CUSTOM enum, elsewise it will not work
// 3. Each command HAVE TO have UNIQUE ID in ALL HISTORY
//    If a command is not used and you want to delete it, just COMMENT it
// 4. MAX_RS_MODIFIER_TYPE always MUST be equla (GREATEST_ID_VALUE_IN_ENUM + 1)
//    Example: If you added new enum value which id equal 400 and it greatest value in enum,
//    you HAVE TO change MAX_RS_MODIFIER_TYPE id to 401
enum class RSModifierType : int16_t {
    INVALID = 0,
    BOUNDS = 1,
    FRAME = 2,
    POSITION_Z = 3,
    POSITION_Z_APPLICABLE_CAMERA3D = 4,
    PIVOT = 5,
    PIVOT_Z = 6,
    QUATERNION = 7,
    ROTATION = 8,
    ROTATION_X = 9,
    ROTATION_Y = 10,
    CAMERA_DISTANCE = 11,
    SCALE = 12,
    SCALE_Z = 13,
    SKEW = 14,
    PERSP = 15,
    TRANSLATE = 16,
    TRANSLATE_Z = 17,
    SUBLAYER_TRANSFORM = 18,
    CORNER_RADIUS = 19,
    ALPHA = 20,
    ALPHA_OFFSCREEN = 21,
    FOREGROUND_COLOR = 22,
    BACKGROUND_COLOR = 23,
    BACKGROUND_SHADER = 24,
    BACKGROUND_SHADER_PROGRESS = 25,
    BG_IMAGE = 26,
    BG_IMAGE_INNER_RECT = 27,
    BG_IMAGE_WIDTH = 28,
    BG_IMAGE_HEIGHT = 29,
    BG_IMAGE_POSITION_X = 30,
    BG_IMAGE_POSITION_Y = 31,
    SURFACE_BG_COLOR = 32,
    BORDER_COLOR = 33,
    BORDER_WIDTH = 34,
    BORDER_STYLE = 35,
    BORDER_DASH_WIDTH = 36,
    BORDER_DASH_GAP = 37,
    FILTER = 38,
    BACKGROUND_FILTER = 39,
    LINEAR_GRADIENT_BLUR_PARA = 40,
    DYNAMIC_LIGHT_UP_RATE = 41,
    DYNAMIC_LIGHT_UP_DEGREE = 42,
    FG_BRIGHTNESS_RATES = 43,
    FG_BRIGHTNESS_SATURATION = 44,
    FG_BRIGHTNESS_POSCOEFF = 45,
    FG_BRIGHTNESS_NEGCOEFF = 46,
    FG_BRIGHTNESS_FRACTION = 47,
    BG_BRIGHTNESS_RATES = 48,
    BG_BRIGHTNESS_SATURATION = 49,
    BG_BRIGHTNESS_POSCOEFF = 50,
    BG_BRIGHTNESS_NEGCOEFF = 51,
    BG_BRIGHTNESS_FRACTION = 52,
    FRAME_GRAVITY = 53,
    CLIP_RRECT = 54,
    CLIP_BOUNDS = 55,
    CLIP_TO_BOUNDS = 56,
    CLIP_TO_FRAME = 57,
    VISIBLE = 58,
    SHADOW_COLOR = 59,
    SHADOW_OFFSET_X = 60,
    SHADOW_OFFSET_Y = 61,
    SHADOW_ALPHA = 62,
    SHADOW_ELEVATION = 63,
    SHADOW_RADIUS = 64,
    SHADOW_PATH = 65,
    SHADOW_MASK = 66,
    SHADOW_COLOR_STRATEGY = 67,
    MASK = 68,
    SPHERIZE = 69,
    LIGHT_UP_EFFECT = 70,
    PIXEL_STRETCH = 71,
    PIXEL_STRETCH_PERCENT = 72,
    PIXEL_STRETCH_TILE_MODE = 73,
    USE_EFFECT = 74,
    USE_EFFECT_TYPE = 75,
    COLOR_BLEND_MODE = 76,
    COLOR_BLEND_APPLY_TYPE = 77,
    SANDBOX = 78,
    GRAY_SCALE = 79,
    BRIGHTNESS = 80,
    CONTRAST = 81,
    SATURATE = 82,
    SEPIA = 83,
    INVERT = 84,
    AIINVERT = 85,
    SYSTEMBAREFFECT = 86,
    WATER_RIPPLE_PROGRESS = 87,
    WATER_RIPPLE_PARAMS = 88,
    HUE_ROTATE = 89,
    COLOR_BLEND = 90,
    PARTICLE = 91,
    SHADOW_IS_FILLED = 92,
    OUTLINE_COLOR = 93,
    OUTLINE_WIDTH = 94,
    OUTLINE_STYLE = 95,
    OUTLINE_DASH_WIDTH = 96,
    OUTLINE_DASH_GAP = 97,
    OUTLINE_RADIUS = 98,
    GREY_COEF = 99,
    LIGHT_INTENSITY = 100,
    LIGHT_COLOR = 101,
    LIGHT_POSITION = 102,
    ILLUMINATED_BORDER_WIDTH = 103,
    ILLUMINATED_TYPE = 104,
    BLOOM = 105,
    FOREGROUND_EFFECT_RADIUS = 106,
    USE_SHADOW_BATCHING = 107,
    MOTION_BLUR_PARA = 108,
    PARTICLE_EMITTER_UPDATER = 109,
    PARTICLE_NOISE_FIELD = 110,
    FLY_OUT_DEGREE = 111,
    FLY_OUT_PARAMS = 112,
    DISTORTION_K = 113,
    DYNAMIC_DIM_DEGREE = 114,
    MAGNIFIER_PARA = 115,
    BACKGROUND_BLUR_RADIUS = 116,
    BACKGROUND_BLUR_SATURATION = 117,
    BACKGROUND_BLUR_BRIGHTNESS = 118,
    BACKGROUND_BLUR_MASK_COLOR = 119,
    BACKGROUND_BLUR_COLOR_MODE = 120,
    BACKGROUND_BLUR_RADIUS_X = 121,
    BACKGROUND_BLUR_RADIUS_Y = 122,
    BG_BLUR_DISABLE_SYSTEM_ADAPTATION = 123,
    FOREGROUND_BLUR_RADIUS = 124,
    FOREGROUND_BLUR_SATURATION = 125,
    FOREGROUND_BLUR_BRIGHTNESS = 126,
    FOREGROUND_BLUR_MASK_COLOR = 127,
    FOREGROUND_BLUR_COLOR_MODE = 128,
    FOREGROUND_BLUR_RADIUS_X = 129,
    FOREGROUND_BLUR_RADIUS_Y = 130,
    FG_BLUR_DISABLE_SYSTEM_ADAPTATION = 131,
    ATTRACTION_FRACTION = 132,
    ATTRACTION_DSTPOINT = 133,
    ALWAYS_SNAPSHOT = 134,
    COMPLEX_SHADER_PARAM = 135,
    BACKGROUND_UI_FILTER = 136,
    HDR_UI_BRIGHTNESS = 137,
    FOREGROUND_UI_FILTER = 138,
    HDR_BRIGHTNESS_FACTOR = 139,
    FOREGROUND_NG_FILTER = 140,
    BACKGROUND_NG_FILTER = 141,

    CUSTOM = 200,
    EXTENDED = 201,
    TRANSITION = 202,
    BACKGROUND_STYLE = 203,
    CONTENT_STYLE = 204,
    FOREGROUND_STYLE = 205,
    OVERLAY_STYLE = 206,
    NODE_MODIFIER = 207,
    ENV_FOREGROUND_COLOR = 208,
    ENV_FOREGROUND_COLOR_STRATEGY = 209,
    GEOMETRYTRANS = 210,
    CUSTOM_CLIP_TO_FRAME = 211,
    HDR_BRIGHTNESS = 212,
    BEHIND_WINDOW_FILTER_RADIUS = 213,
    BEHIND_WINDOW_FILTER_SATURATION = 214,
    BEHIND_WINDOW_FILTER_BRIGHTNESS = 215,
    BEHIND_WINDOW_FILTER_MASK_COLOR = 216,

    CHILDREN = 240, // PLACEHOLDER, no such modifier, but we need a dirty flag

    MAX_RS_MODIFIER_TYPE = 255,
};

enum class RSPropertyModifierType : uint8_t {
    GEOMETRY = 0,
    BACKGROUND,
    CONTENT,
    FOREGROUND,
    OVERLAY,
    APPEARANCE,
    CUSTOM,
};

using ModifierDirtyTypes = std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)>;

class RSModifierTypeString {
public:
    std::string GetModifierTypeString(RSModifierType type) const
    {
        switch (type) {
            case RSModifierType::INVALID: return "Invalid";
            case RSModifierType::BOUNDS: return "Bounds";
            case RSModifierType::FRAME: return "Frame";
            case RSModifierType::POSITION_Z: return "PositionZ";
            case RSModifierType::POSITION_Z_APPLICABLE_CAMERA3D: return "PositionZApplicableCamera3D";
            case RSModifierType::PIVOT: return "Pivot";
            case RSModifierType::PIVOT_Z: return "PivotZ";
            case RSModifierType::QUATERNION: return "Quaternion";
            case RSModifierType::ROTATION: return "Rotation";
            case RSModifierType::ROTATION_X: return "RotationX";
            case RSModifierType::ROTATION_Y: return "RotationY";
            case RSModifierType::CAMERA_DISTANCE: return "CameraDistance";
            case RSModifierType::SCALE: return "Scale";
            case RSModifierType::SCALE_Z: return "ScaleZ";
            case RSModifierType::SKEW: return "Skew";
            case RSModifierType::PERSP: return "Persp";
            case RSModifierType::TRANSLATE: return "Translate";
            case RSModifierType::TRANSLATE_Z: return "TranslateZ";
            case RSModifierType::SUBLAYER_TRANSFORM: return "SublayerTransform";
            case RSModifierType::CORNER_RADIUS: return "CornerRadius";
            case RSModifierType::ALPHA: return "Alpha";
            case RSModifierType::ALPHA_OFFSCREEN: return "AlphaOffscreen";
            case RSModifierType::FOREGROUND_COLOR: return "ForegroundColor";
            case RSModifierType::BACKGROUND_COLOR: return "BackgroundColor";
            case RSModifierType::BACKGROUND_SHADER: return "BackgroundShader";
            case RSModifierType::BACKGROUND_SHADER_PROGRESS: return "BackgroundShaderProgress";
            case RSModifierType::BG_IMAGE: return "BgImage";
            case RSModifierType::BG_IMAGE_INNER_RECT: return "BgImageInnerRect";
            case RSModifierType::BG_IMAGE_WIDTH: return "BgImageWidth";
            case RSModifierType::BG_IMAGE_HEIGHT: return "BgImageHeight";
            case RSModifierType::BG_IMAGE_POSITION_X: return "BgImagePositionX";
            case RSModifierType::BG_IMAGE_POSITION_Y: return "BgImagePositionY";
            case RSModifierType::SURFACE_BG_COLOR: return "SurfaceBgColor";
            case RSModifierType::BORDER_COLOR: return "BorderColor";
            case RSModifierType::BORDER_WIDTH: return "BorderWidth";
            case RSModifierType::BORDER_STYLE: return "BorderStyle";
            case RSModifierType::BORDER_DASH_WIDTH: return "BorderDashWidth";
            case RSModifierType::BORDER_DASH_GAP: return "BorderDashGap";
            case RSModifierType::FILTER: return "Filter";
            case RSModifierType::BACKGROUND_FILTER: return "BackgroundFilter";
            case RSModifierType::LINEAR_GRADIENT_BLUR_PARA: return "LinearGradientBlurPara";
            case RSModifierType::DYNAMIC_LIGHT_UP_RATE: return "DynamicLightUpRate";
            case RSModifierType::DYNAMIC_LIGHT_UP_DEGREE: return "DynamicLightUpDegree";
            case RSModifierType::FG_BRIGHTNESS_RATES: return "FgBrightnessRates";
            case RSModifierType::FG_BRIGHTNESS_SATURATION: return "FgBrightnessSaturation";
            case RSModifierType::FG_BRIGHTNESS_POSCOEFF: return "FgBrightnessPoscoeff";
            case RSModifierType::FG_BRIGHTNESS_NEGCOEFF: return "FgBrightnessNegcoeff";
            case RSModifierType::FG_BRIGHTNESS_FRACTION: return "FgBrightnessFraction";
            case RSModifierType::BG_BRIGHTNESS_RATES: return "BgBrightnessRates";
            case RSModifierType::BG_BRIGHTNESS_SATURATION: return "BgBrightnessSaturation";
            case RSModifierType::BG_BRIGHTNESS_POSCOEFF: return "BgBrightnessPoscoeff";
            case RSModifierType::BG_BRIGHTNESS_NEGCOEFF: return "BgBrightnessNegcoeff";
            case RSModifierType::BG_BRIGHTNESS_FRACTION: return "BgBrightnessFraction";
            case RSModifierType::FRAME_GRAVITY: return "FrameGravity";
            case RSModifierType::CLIP_RRECT: return "ClipRrect";
            case RSModifierType::CLIP_BOUNDS: return "ClipBounds";
            case RSModifierType::CLIP_TO_BOUNDS: return "ClipToBounds";
            case RSModifierType::CLIP_TO_FRAME: return "ClipToFrame";
            case RSModifierType::VISIBLE: return "Visible";
            case RSModifierType::SHADOW_COLOR: return "ShadowColor";
            case RSModifierType::SHADOW_OFFSET_X: return "ShadowOffsetX";
            case RSModifierType::SHADOW_OFFSET_Y: return "ShadowOffsetY";
            case RSModifierType::SHADOW_ALPHA: return "ShadowAlpha";
            case RSModifierType::SHADOW_ELEVATION: return "ShadowElevation";
            case RSModifierType::SHADOW_RADIUS: return "ShadowRadius";
            case RSModifierType::SHADOW_PATH: return "ShadowPath";
            case RSModifierType::SHADOW_MASK: return "ShadowMask";
            case RSModifierType::SHADOW_COLOR_STRATEGY: return "ShadowColorStrategy";
            case RSModifierType::MASK: return "Mask";
            case RSModifierType::SPHERIZE: return "Spherize";
            case RSModifierType::LIGHT_UP_EFFECT: return "LightUpEffect";
            case RSModifierType::PIXEL_STRETCH: return "PixelStretch";
            case RSModifierType::PIXEL_STRETCH_PERCENT: return "PixelStretchPercent";
            case RSModifierType::PIXEL_STRETCH_TILE_MODE: return "PixelStretchTileMode";
            case RSModifierType::USE_EFFECT: return "UseEffect";
            case RSModifierType::USE_EFFECT_TYPE: return "UseEffectType";
            case RSModifierType::COLOR_BLEND_MODE: return "ColorBlendMode";
            case RSModifierType::COLOR_BLEND_APPLY_TYPE: return "ColorBlendApplyType";
            case RSModifierType::SANDBOX: return "Sandbox";
            case RSModifierType::GRAY_SCALE: return "GrayScale";
            case RSModifierType::BRIGHTNESS: return "Brightness";
            case RSModifierType::CONTRAST: return "Contrast";
            case RSModifierType::SATURATE: return "Saturate";
            case RSModifierType::SEPIA: return "Sepia";
            case RSModifierType::INVERT: return "Invert";
            case RSModifierType::AIINVERT: return "Aiinvert";
            case RSModifierType::SYSTEMBAREFFECT: return "Systembareffect";
            case RSModifierType::HUE_ROTATE: return "HueRotate";
            case RSModifierType::COLOR_BLEND: return "ColorBlend";
            case RSModifierType::PARTICLE: return "Particle";
            case RSModifierType::SHADOW_IS_FILLED: return "ShadowIsFilled";
            case RSModifierType::OUTLINE_COLOR: return "OutlineColor";
            case RSModifierType::OUTLINE_WIDTH: return "OutlineWidth";
            case RSModifierType::OUTLINE_STYLE: return "OutlineStyle";
            case RSModifierType::OUTLINE_DASH_WIDTH: return "OutlineDashWidth";
            case RSModifierType::OUTLINE_DASH_GAP: return "OutlineDashGap";
            case RSModifierType::OUTLINE_RADIUS: return "OutlineRadius";
            case RSModifierType::GREY_COEF: return "GreyCoef";
            case RSModifierType::LIGHT_INTENSITY: return "LightIntensity";
            case RSModifierType::LIGHT_COLOR: return "LightColor";
            case RSModifierType::LIGHT_POSITION: return "LightPosition";
            case RSModifierType::ILLUMINATED_BORDER_WIDTH: return "IlluminatedBorderWidth";
            case RSModifierType::ILLUMINATED_TYPE: return "IlluminatedType";
            case RSModifierType::BLOOM: return "Bloom";
            case RSModifierType::FOREGROUND_EFFECT_RADIUS: return "ForegroundEffectRadius";
            case RSModifierType::USE_SHADOW_BATCHING: return "UseShadowBatching";
            case RSModifierType::MOTION_BLUR_PARA: return "MotionBlurPara";
            case RSModifierType::PARTICLE_EMITTER_UPDATER: return "ParticleEmitterUpdater";
            case RSModifierType::PARTICLE_NOISE_FIELD: return "ParticleNoiseField";
            case RSModifierType::DYNAMIC_DIM_DEGREE: return "DynamicDimDegree";
            case RSModifierType::MAGNIFIER_PARA: return "MagnifierPara";
            case RSModifierType::BACKGROUND_BLUR_RADIUS: return "BackgroundBlurRadius";
            case RSModifierType::BACKGROUND_BLUR_SATURATION: return "BackgroundBlurSaturation";
            case RSModifierType::BACKGROUND_BLUR_BRIGHTNESS: return "BackgroundBlurBrightness";
            case RSModifierType::BACKGROUND_BLUR_MASK_COLOR: return "BackgroundBlurMaskColor";
            case RSModifierType::BACKGROUND_BLUR_COLOR_MODE: return "BackgroundBlurColorMode";
            case RSModifierType::BACKGROUND_BLUR_RADIUS_X: return "BackgroundBlurRadiusX";
            case RSModifierType::BACKGROUND_BLUR_RADIUS_Y: return "BackgroundBlurRadiusY";
            case RSModifierType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION: return "BgBlurDisableSystemAdaptation";
            case RSModifierType::FOREGROUND_BLUR_RADIUS: return "ForegroundBlurRadius";
            case RSModifierType::FOREGROUND_BLUR_SATURATION: return "ForegroundBlurSaturation";
            case RSModifierType::FOREGROUND_BLUR_BRIGHTNESS: return "ForegroundBlurBrightness";
            case RSModifierType::FOREGROUND_BLUR_MASK_COLOR: return "ForegroundBlurMaskColor";
            case RSModifierType::FOREGROUND_BLUR_COLOR_MODE: return "ForegroundBlurColorMode";
            case RSModifierType::FOREGROUND_BLUR_RADIUS_X: return "ForegroundBlurRadiusX";
            case RSModifierType::FOREGROUND_BLUR_RADIUS_Y: return "ForegroundBlurRadiusY";
            case RSModifierType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION: return "FgBlurDisableSystemAdaptation";
            case RSModifierType::ALWAYS_SNAPSHOT: return "AlwaysSnapshot";
            case RSModifierType::COMPLEX_SHADER_PARAM: return "ComplexShaderParam";
            case RSModifierType::BACKGROUND_UI_FILTER: return "BackgroundUIFilter";
            case RSModifierType::HDR_UI_BRIGHTNESS: return "HDRUIBrightness";
            case RSModifierType::FOREGROUND_UI_FILTER: return "ForegroundUIFilter";
            case RSModifierType::HDR_BRIGHTNESS_FACTOR: return "HDRBrightnessFactor";
            case RSModifierType::BACKGROUND_NG_FILTER: return "BackgroundNgFilter";
            case RSModifierType::FOREGROUND_NG_FILTER: return "ForegroundNgFilter";
            case RSModifierType::CUSTOM: return "Custom";
            case RSModifierType::EXTENDED: return "Extended";
            case RSModifierType::TRANSITION: return "Transition";
            case RSModifierType::BACKGROUND_STYLE: return "BackgroundStyle";
            case RSModifierType::CONTENT_STYLE: return "ContentStyle";
            case RSModifierType::FOREGROUND_STYLE: return "ForegroundStyle";
            case RSModifierType::OVERLAY_STYLE: return "OverlayStyle";
            case RSModifierType::NODE_MODIFIER: return "NodeModifier";
            case RSModifierType::ENV_FOREGROUND_COLOR: return "EnvForegroundColor";
            case RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY: return "EnvForegroundColorStrategy";
            case RSModifierType::GEOMETRYTRANS: return "Geometrytrans";
            case RSModifierType::CUSTOM_CLIP_TO_FRAME: return "CustomClipToFrame";
            case RSModifierType::HDR_BRIGHTNESS: return "HDRBrightness";
            case RSModifierType::BEHIND_WINDOW_FILTER_RADIUS: return "BehindWindowFilterRadius";
            case RSModifierType::BEHIND_WINDOW_FILTER_SATURATION: return "BehindWindowFilterSaturation";
            case RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS: return "BehindWindowFilterBrightness";
            case RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR: return "BehindWindowFilterMaskColor";
            case RSModifierType::CHILDREN: return "Children";
            case RSModifierType::MAX_RS_MODIFIER_TYPE: return "MaxRsModifierType";
            default:
                return "UNKNOWN";
        }
        return "UNKNOWN";
    }
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_TYPE_H
