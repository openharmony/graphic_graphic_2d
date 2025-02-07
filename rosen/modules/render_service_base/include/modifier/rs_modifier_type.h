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
enum class RSModifierType : int16_t {
    INVALID = 0,
    BOUNDS,
    FRAME,
    POSITION_Z,
    POSITION_Z_APPLICABLE_CAMERA3D,
    PIVOT,
    PIVOT_Z,
    QUATERNION,
    ROTATION,
    ROTATION_X,
    ROTATION_Y,
    CAMERA_DISTANCE,
    SCALE,
    SCALE_Z,
    SKEW,
    PERSP,
    TRANSLATE,
    TRANSLATE_Z,
    SUBLAYER_TRANSFORM,
    CORNER_RADIUS,
    ALPHA,
    ALPHA_OFFSCREEN,
    FOREGROUND_COLOR,
    BACKGROUND_COLOR,
    BACKGROUND_SHADER,
    BACKGROUND_SHADER_PROGRESS,
    BG_IMAGE,
    BG_IMAGE_INNER_RECT,
    BG_IMAGE_WIDTH,
    BG_IMAGE_HEIGHT,
    BG_IMAGE_POSITION_X,
    BG_IMAGE_POSITION_Y,
    SURFACE_BG_COLOR,
    BORDER_COLOR,
    BORDER_WIDTH,
    BORDER_STYLE,
    BORDER_DASH_WIDTH,
    BORDER_DASH_GAP,
    FILTER,
    BACKGROUND_FILTER,
    LINEAR_GRADIENT_BLUR_PARA,
    DYNAMIC_LIGHT_UP_RATE,
    DYNAMIC_LIGHT_UP_DEGREE,
    FG_BRIGHTNESS_RATES,
    FG_BRIGHTNESS_SATURATION,
    FG_BRIGHTNESS_POSCOEFF,
    FG_BRIGHTNESS_NEGCOEFF,
    FG_BRIGHTNESS_FRACTION,
    BG_BRIGHTNESS_RATES,
    BG_BRIGHTNESS_SATURATION,
    BG_BRIGHTNESS_POSCOEFF,
    BG_BRIGHTNESS_NEGCOEFF,
    BG_BRIGHTNESS_FRACTION,
    FRAME_GRAVITY,
    CLIP_RRECT,
    CLIP_BOUNDS,
    CLIP_TO_BOUNDS,
    CLIP_TO_FRAME,
    VISIBLE,
    SHADOW_COLOR,
    SHADOW_OFFSET_X,
    SHADOW_OFFSET_Y,
    SHADOW_ALPHA,
    SHADOW_ELEVATION,
    SHADOW_RADIUS,
    SHADOW_PATH,
    SHADOW_MASK,
    SHADOW_COLOR_STRATEGY,
    MASK,
    SPHERIZE,
    LIGHT_UP_EFFECT,
    PIXEL_STRETCH,
    PIXEL_STRETCH_PERCENT,
    PIXEL_STRETCH_TILE_MODE,
    USE_EFFECT,
    USE_EFFECT_TYPE,
    COLOR_BLEND_MODE,
    COLOR_BLEND_APPLY_TYPE,
    SANDBOX,
    GRAY_SCALE,
    BRIGHTNESS,
    CONTRAST,
    SATURATE,
    SEPIA,
    INVERT,
    AIINVERT,
    SYSTEMBAREFFECT,
    WATER_RIPPLE_PROGRESS,
    WATER_RIPPLE_PARAMS,
    HUE_ROTATE,
    COLOR_BLEND,
    PARTICLE,
    SHADOW_IS_FILLED,
    OUTLINE_COLOR,
    OUTLINE_WIDTH,
    OUTLINE_STYLE,
    OUTLINE_DASH_WIDTH,
    OUTLINE_DASH_GAP,
    OUTLINE_RADIUS,
    GREY_COEF,
    LIGHT_INTENSITY,
    LIGHT_COLOR,
    LIGHT_POSITION,
    ILLUMINATED_BORDER_WIDTH,
    ILLUMINATED_TYPE,
    BLOOM,
    FOREGROUND_EFFECT_RADIUS,
    USE_SHADOW_BATCHING,
    MOTION_BLUR_PARA,
    PARTICLE_EMITTER_UPDATER,
    PARTICLE_NOISE_FIELD,
    FLY_OUT_DEGREE,
    FLY_OUT_PARAMS,
    DISTORTION_K,
    DYNAMIC_DIM_DEGREE,
    MAGNIFIER_PARA,
    BACKGROUND_BLUR_RADIUS,
    BACKGROUND_BLUR_SATURATION,
    BACKGROUND_BLUR_BRIGHTNESS,
    BACKGROUND_BLUR_MASK_COLOR,
    BACKGROUND_BLUR_COLOR_MODE,
    BACKGROUND_BLUR_RADIUS_X,
    BACKGROUND_BLUR_RADIUS_Y,
    FOREGROUND_BLUR_RADIUS,
    FOREGROUND_BLUR_SATURATION,
    FOREGROUND_BLUR_BRIGHTNESS,
    FOREGROUND_BLUR_MASK_COLOR,
    FOREGROUND_BLUR_COLOR_MODE,
    FOREGROUND_BLUR_RADIUS_X,
    FOREGROUND_BLUR_RADIUS_Y,
    ATTRACTION_FRACTION,
    ATTRACTION_DSTPOINT,
    CUSTOM,
    EXTENDED,
    TRANSITION,
    BACKGROUND_STYLE,
    CONTENT_STYLE,
    FOREGROUND_STYLE,
    OVERLAY_STYLE,
    NODE_MODIFIER,
    ENV_FOREGROUND_COLOR,
    ENV_FOREGROUND_COLOR_STRATEGY,
    GEOMETRYTRANS,
    CUSTOM_CLIP_TO_FRAME,
    HDR_BRIGHTNESS,
    BEHIND_WINDOW_FILTER_ENABLED,
    BEHIND_WINDOW_FILTER_RADIUS,
    BEHIND_WINDOW_FILTER_SATURATION,
    BEHIND_WINDOW_FILTER_BRIGHTNESS,
    BEHIND_WINDOW_FILTER_MASK_COLOR,
    CHILDREN, // PLACEHOLDER, no such modifier, but we need a dirty flag
    MAX_RS_MODIFIER_TYPE,
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

enum class RSRenderPropertyType : int16_t {
    INVALID = 0,
    PROPERTY_FLOAT,
    PROPERTY_COLOR,
    PROPERTY_MATRIX3F,
    PROPERTY_QUATERNION,
    PROPERTY_FILTER,
    PROPERTY_VECTOR2F,
    PROPERTY_VECTOR3F,
    PROPERTY_VECTOR4F,
    PROPERTY_VECTOR4_COLOR,
    PROPERTY_SKMATRIX,
    PROPERTY_RRECT,
};

enum class RSPropertyUnit : int16_t {
    UNKNOWN = 0,
    PIXEL_POSITION,
    PIXEL_SIZE,
    RATIO_SCALE,
    ANGLE_ROTATION,
};

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
            case RSModifierType::FOREGROUND_BLUR_RADIUS: return "ForegroundBlurRadius";
            case RSModifierType::FOREGROUND_BLUR_SATURATION: return "ForegroundBlurSaturation";
            case RSModifierType::FOREGROUND_BLUR_BRIGHTNESS: return "ForegroundBlurBrightness";
            case RSModifierType::FOREGROUND_BLUR_MASK_COLOR: return "ForegroundBlurMaskColor";
            case RSModifierType::FOREGROUND_BLUR_COLOR_MODE: return "ForegroundBlurColorMode";
            case RSModifierType::FOREGROUND_BLUR_RADIUS_X: return "ForegroundBlurRadiusX";
            case RSModifierType::FOREGROUND_BLUR_RADIUS_Y: return "ForegroundBlurRadiusY";
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
            case RSModifierType::BEHIND_WINDOW_FILTER_ENABLED: return "BehindWindowFilterEnabled";
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
