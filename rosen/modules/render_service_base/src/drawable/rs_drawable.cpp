/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_drawable.h"

#include <limits>

#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {
namespace {
using namespace DrawableV2;

static const std::unordered_map<RSModifierType, RSDrawableSlot> g_propertyToDrawableLut = {
    { RSModifierType::INVALID,                                   RSDrawableSlot::INVALID },
    { RSModifierType::BOUNDS,                                    RSDrawableSlot::CLIP_TO_BOUNDS },
    { RSModifierType::FRAME,                                     RSDrawableSlot::FRAME_OFFSET },
    { RSModifierType::POSITION_Z,                                RSDrawableSlot::INVALID },
    { RSModifierType::POSITION_Z_APPLICABLE_CAMERA3D,            RSDrawableSlot::INVALID },
    { RSModifierType::PIVOT,                                     RSDrawableSlot::INVALID },
    { RSModifierType::PIVOT_Z,                                   RSDrawableSlot::INVALID },
    { RSModifierType::QUATERNION,                                RSDrawableSlot::INVALID },
    { RSModifierType::ROTATION,                                  RSDrawableSlot::INVALID },
    { RSModifierType::ROTATION_X,                                RSDrawableSlot::INVALID },
    { RSModifierType::ROTATION_Y,                                RSDrawableSlot::INVALID },
    { RSModifierType::CAMERA_DISTANCE,                           RSDrawableSlot::INVALID },
    { RSModifierType::SCALE,                                     RSDrawableSlot::INVALID },
    { RSModifierType::SCALE_Z,                                   RSDrawableSlot::INVALID },
    { RSModifierType::SKEW,                                      RSDrawableSlot::INVALID },
    { RSModifierType::PERSP,                                     RSDrawableSlot::INVALID },
    { RSModifierType::TRANSLATE,                                 RSDrawableSlot::INVALID },
    { RSModifierType::TRANSLATE_Z,                               RSDrawableSlot::INVALID },
    { RSModifierType::SUBLAYER_TRANSFORM,                        RSDrawableSlot::INVALID },
    { RSModifierType::CORNER_RADIUS,                             RSDrawableSlot::CLIP_TO_BOUNDS },
    { RSModifierType::ALPHA,                                     RSDrawableSlot::INVALID },
    { RSModifierType::ALPHA_OFFSCREEN,                           RSDrawableSlot::INVALID },
    { RSModifierType::FOREGROUND_COLOR,                          RSDrawableSlot::FOREGROUND_COLOR },
    { RSModifierType::BACKGROUND_COLOR,                          RSDrawableSlot::BACKGROUND_COLOR },
    { RSModifierType::BACKGROUND_SHADER,                         RSDrawableSlot::BACKGROUND_SHADER },
    { RSModifierType::BACKGROUND_SHADER_PROGRESS,                RSDrawableSlot::BACKGROUND_SHADER },
    { RSModifierType::BG_IMAGE,                                  RSDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_INNER_RECT,                       RSDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_WIDTH,                            RSDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_HEIGHT,                           RSDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_POSITION_X,                       RSDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_POSITION_Y,                       RSDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::SURFACE_BG_COLOR,                          RSDrawableSlot::INVALID },
    { RSModifierType::BORDER_COLOR,                              RSDrawableSlot::BORDER },
    { RSModifierType::BORDER_WIDTH,                              RSDrawableSlot::BORDER },
    { RSModifierType::BORDER_STYLE,                              RSDrawableSlot::BORDER },
    { RSModifierType::BORDER_DASH_WIDTH,                         RSDrawableSlot::BORDER },
    { RSModifierType::BORDER_DASH_GAP,                           RSDrawableSlot::BORDER },
    { RSModifierType::FILTER,                                    RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::BACKGROUND_FILTER,                         RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::LINEAR_GRADIENT_BLUR_PARA,                 RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::DYNAMIC_LIGHT_UP_RATE,                     RSDrawableSlot::DYNAMIC_LIGHT_UP },
    { RSModifierType::DYNAMIC_LIGHT_UP_DEGREE,                   RSDrawableSlot::DYNAMIC_LIGHT_UP },
    { RSModifierType::FG_BRIGHTNESS_RATES,                       RSDrawableSlot::BLENDER },
    { RSModifierType::FG_BRIGHTNESS_SATURATION,                  RSDrawableSlot::BLENDER },
    { RSModifierType::FG_BRIGHTNESS_POSCOEFF,                    RSDrawableSlot::BLENDER },
    { RSModifierType::FG_BRIGHTNESS_NEGCOEFF,                    RSDrawableSlot::BLENDER },
    { RSModifierType::FG_BRIGHTNESS_FRACTION,                    RSDrawableSlot::BLENDER },
    { RSModifierType::BG_BRIGHTNESS_RATES,                       RSDrawableSlot::BACKGROUND_COLOR },
    { RSModifierType::BG_BRIGHTNESS_SATURATION,                  RSDrawableSlot::BACKGROUND_COLOR },
    { RSModifierType::BG_BRIGHTNESS_POSCOEFF,                    RSDrawableSlot::BACKGROUND_COLOR },
    { RSModifierType::BG_BRIGHTNESS_NEGCOEFF,                    RSDrawableSlot::BACKGROUND_COLOR },
    { RSModifierType::BG_BRIGHTNESS_FRACTION,                    RSDrawableSlot::BACKGROUND_COLOR },
    { RSModifierType::FRAME_GRAVITY,                             RSDrawableSlot::FRAME_OFFSET },
    { RSModifierType::CLIP_RRECT,                                RSDrawableSlot::CLIP_TO_BOUNDS },
    { RSModifierType::CLIP_BOUNDS,                               RSDrawableSlot::CLIP_TO_BOUNDS },
    { RSModifierType::CLIP_TO_BOUNDS,                            RSDrawableSlot::CLIP_TO_BOUNDS },
    { RSModifierType::CLIP_TO_FRAME,                             RSDrawableSlot::CLIP_TO_FRAME },
    { RSModifierType::VISIBLE,                                   RSDrawableSlot::INVALID },
    { RSModifierType::SHADOW_COLOR,                              RSDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_OFFSET_X,                           RSDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_OFFSET_Y,                           RSDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_ALPHA,                              RSDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_ELEVATION,                          RSDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_RADIUS,                             RSDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_PATH,                               RSDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_MASK,                               RSDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_COLOR_STRATEGY,                     RSDrawableSlot::SHADOW },
    { RSModifierType::MASK,                                      RSDrawableSlot::MASK },
    { RSModifierType::SPHERIZE,                                  RSDrawableSlot::FOREGROUND_FILTER },
    { RSModifierType::LIGHT_UP_EFFECT,                           RSDrawableSlot::LIGHT_UP_EFFECT },
    { RSModifierType::PIXEL_STRETCH,                             RSDrawableSlot::PIXEL_STRETCH },
    { RSModifierType::PIXEL_STRETCH_PERCENT,                     RSDrawableSlot::PIXEL_STRETCH },
    { RSModifierType::PIXEL_STRETCH_TILE_MODE,                   RSDrawableSlot::PIXEL_STRETCH },
    { RSModifierType::USE_EFFECT,                                RSDrawableSlot::USE_EFFECT },
    { RSModifierType::USE_EFFECT_TYPE,                           RSDrawableSlot::USE_EFFECT },
    { RSModifierType::COLOR_BLEND_MODE,                          RSDrawableSlot::BLENDER },
    { RSModifierType::COLOR_BLEND_APPLY_TYPE,                    RSDrawableSlot::BLENDER },
    { RSModifierType::SANDBOX,                                   RSDrawableSlot::INVALID },
    { RSModifierType::GRAY_SCALE,                                RSDrawableSlot::COLOR_FILTER },
    { RSModifierType::BRIGHTNESS,                                RSDrawableSlot::COLOR_FILTER },
    { RSModifierType::CONTRAST,                                  RSDrawableSlot::COLOR_FILTER },
    { RSModifierType::SATURATE,                                  RSDrawableSlot::COLOR_FILTER },
    { RSModifierType::SEPIA,                                     RSDrawableSlot::COLOR_FILTER },
    { RSModifierType::INVERT,                                    RSDrawableSlot::COLOR_FILTER },
    { RSModifierType::AIINVERT,                                  RSDrawableSlot::BINARIZATION },
    { RSModifierType::SYSTEMBAREFFECT,                           RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::WATER_RIPPLE_PROGRESS,                     RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::WATER_RIPPLE_PARAMS,                       RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::HUE_ROTATE,                                RSDrawableSlot::COLOR_FILTER },
    { RSModifierType::COLOR_BLEND,                               RSDrawableSlot::COLOR_FILTER },
    { RSModifierType::PARTICLE,                                  RSDrawableSlot::PARTICLE_EFFECT },
    { RSModifierType::SHADOW_IS_FILLED,                          RSDrawableSlot::SHADOW },
    { RSModifierType::OUTLINE_COLOR,                             RSDrawableSlot::OUTLINE },
    { RSModifierType::OUTLINE_WIDTH,                             RSDrawableSlot::OUTLINE },
    { RSModifierType::OUTLINE_STYLE,                             RSDrawableSlot::OUTLINE },
    { RSModifierType::OUTLINE_DASH_WIDTH,                        RSDrawableSlot::OUTLINE },
    { RSModifierType::OUTLINE_DASH_GAP,                          RSDrawableSlot::OUTLINE },
    { RSModifierType::OUTLINE_RADIUS,                            RSDrawableSlot::OUTLINE },
    { RSModifierType::GREY_COEF,                                 RSDrawableSlot::INVALID },
    { RSModifierType::LIGHT_INTENSITY,                           RSDrawableSlot::POINT_LIGHT },
    { RSModifierType::LIGHT_COLOR,                               RSDrawableSlot::POINT_LIGHT },
    { RSModifierType::LIGHT_POSITION,                            RSDrawableSlot::POINT_LIGHT },
    { RSModifierType::ILLUMINATED_BORDER_WIDTH,                  RSDrawableSlot::POINT_LIGHT },
    { RSModifierType::ILLUMINATED_TYPE,                          RSDrawableSlot::POINT_LIGHT },
    { RSModifierType::BLOOM,                                     RSDrawableSlot::POINT_LIGHT },
    { RSModifierType::FOREGROUND_EFFECT_RADIUS,                  RSDrawableSlot::FOREGROUND_FILTER },
    { RSModifierType::USE_SHADOW_BATCHING,                       RSDrawableSlot::CHILDREN },
    { RSModifierType::MOTION_BLUR_PARA,                          RSDrawableSlot::FOREGROUND_FILTER },
    { RSModifierType::PARTICLE_EMITTER_UPDATER,                  RSDrawableSlot::PARTICLE_EFFECT },
    { RSModifierType::PARTICLE_NOISE_FIELD,                      RSDrawableSlot::PARTICLE_EFFECT },
    { RSModifierType::FLY_OUT_DEGREE,                            RSDrawableSlot::FOREGROUND_FILTER },
    { RSModifierType::FLY_OUT_PARAMS,                            RSDrawableSlot::FOREGROUND_FILTER },
    { RSModifierType::DISTORTION_K,                              RSDrawableSlot::FOREGROUND_FILTER },
    { RSModifierType::DYNAMIC_DIM_DEGREE,                        RSDrawableSlot::DYNAMIC_DIM },
    { RSModifierType::MAGNIFIER_PARA,                            RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BACKGROUND_BLUR_RADIUS,                    RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BACKGROUND_BLUR_SATURATION,                RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BACKGROUND_BLUR_BRIGHTNESS,                RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BACKGROUND_BLUR_MASK_COLOR,                RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BACKGROUND_BLUR_COLOR_MODE,                RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BACKGROUND_BLUR_RADIUS_X,                  RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BACKGROUND_BLUR_RADIUS_Y,                  RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION,         RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::FOREGROUND_BLUR_RADIUS,                    RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::FOREGROUND_BLUR_SATURATION,                RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::FOREGROUND_BLUR_BRIGHTNESS,                RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::FOREGROUND_BLUR_MASK_COLOR,                RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::FOREGROUND_BLUR_COLOR_MODE,                RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::FOREGROUND_BLUR_RADIUS_X,                  RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::FOREGROUND_BLUR_RADIUS_Y,                  RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION,         RSDrawableSlot::COMPOSITING_FILTER },
    { RSModifierType::ATTRACTION_FRACTION,                       RSDrawableSlot::FOREGROUND_FILTER },
    { RSModifierType::ATTRACTION_DSTPOINT,                       RSDrawableSlot::FOREGROUND_FILTER },
    { RSModifierType::ALWAYS_SNAPSHOT,                           RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::CUSTOM,                                    RSDrawableSlot::INVALID },
    { RSModifierType::EXTENDED,                                  RSDrawableSlot::INVALID },
    { RSModifierType::TRANSITION,                                RSDrawableSlot::TRANSITION },
    { RSModifierType::BACKGROUND_STYLE,                          RSDrawableSlot::BACKGROUND_STYLE },
    { RSModifierType::CONTENT_STYLE,                             RSDrawableSlot::CONTENT_STYLE },
    { RSModifierType::FOREGROUND_STYLE,                          RSDrawableSlot::FOREGROUND_STYLE },
    { RSModifierType::OVERLAY_STYLE,                             RSDrawableSlot::OVERLAY },
    { RSModifierType::NODE_MODIFIER,                             RSDrawableSlot::INVALID },
    { RSModifierType::ENV_FOREGROUND_COLOR,                      RSDrawableSlot::ENV_FOREGROUND_COLOR },
    { RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY,             RSDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY },
    { RSModifierType::GEOMETRYTRANS,                             RSDrawableSlot::INVALID },
    { RSModifierType::CUSTOM_CLIP_TO_FRAME,                      RSDrawableSlot::CUSTOM_CLIP_TO_FRAME },
    { RSModifierType::HDR_BRIGHTNESS,                            RSDrawableSlot::INVALID },
    { RSModifierType::BEHIND_WINDOW_FILTER_RADIUS,               RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BEHIND_WINDOW_FILTER_SATURATION,           RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS,           RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR,           RSDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::CHILDREN,                                  RSDrawableSlot::CHILDREN },
};

template<RSModifierType type>
static inline RSDrawable::Ptr ModifierGenerator(const RSRenderNode& node)
{
    return RSCustomModifierDrawable::OnGenerate(node, type);
}

// NOTE: This LUT should always the same size as RSDrawableSlot
// index = RSDrawableSlotType, value = DrawableGenerator
constexpr int GEN_LUT_SIZE = static_cast<int>(RSDrawableSlot::MAX);
static const std::array<RSDrawable::Generator, GEN_LUT_SIZE> g_drawableGeneratorLut = {
    nullptr, // SAVE_ALL

    // Bounds Geometry
    RSMaskDrawable::OnGenerate,                    // MASK,
    ModifierGenerator<RSModifierType::TRANSITION>, // TRANSITION,
    RSEnvFGColorDrawable::OnGenerate,              // ENV_FOREGROUND_COLOR,
    RSShadowDrawable::OnGenerate,                  // SHADOW,
    RSForegroundFilterDrawable::OnGenerate,        // FOREGROUND_FILTER
    RSOutlineDrawable::OnGenerate,                 // OUTLINE,

    // BG properties in Bounds Clip
    nullptr,                                             // BG_SAVE_BOUNDS,
    nullptr,                                             // CLIP_TO_BOUNDS,
    RSBeginBlenderDrawable::OnGenerate,                  // BLENDER,
    RSBackgroundColorDrawable::OnGenerate,               // BACKGROUND_COLOR,
    RSBackgroundShaderDrawable::OnGenerate,              // BACKGROUND_SHADER,
    RSBackgroundImageDrawable::OnGenerate,               // BACKGROUND_IMAGE,
    RSBackgroundFilterDrawable::OnGenerate,              // BACKGROUND_FILTER,
    RSUseEffectDrawable::OnGenerate,                     // USE_EFFECT,
    ModifierGenerator<RSModifierType::BACKGROUND_STYLE>, // BACKGROUND_STYLE,
    RSDynamicLightUpDrawable::OnGenerate,                // DYNAMIC_LIGHT_UP,
    RSEnvFGColorStrategyDrawable::OnGenerate,            // ENV_FOREGROUND_COLOR_STRATEGY,
    nullptr,                                             // BG_RESTORE_BOUNDS,

    // Frame Geometry
    nullptr,                                             // SAVE_FRAME,
    RSFrameOffsetDrawable::OnGenerate,                   // FRAME_OFFSET,
    RSClipToFrameDrawable::OnGenerate,                   // CLIP_TO_FRAME,
    RSCustomClipToFrameDrawable::OnGenerate,             // CUSTOM_CLIP_TO_FRAME,
    ModifierGenerator<RSModifierType::CONTENT_STYLE>,    // CONTENT_STYLE,
    RSChildrenDrawable::OnGenerate,                      // CHILDREN,
    ModifierGenerator<RSModifierType::FOREGROUND_STYLE>, // FOREGROUND_STYLE,
    nullptr,                                             // RESTORE_FRAME,

    // FG properties in Bounds clip
    nullptr,                                 // FG_SAVE_BOUNDS,
    nullptr,                                 // FG_CLIP_TO_BOUNDS,
    RSBinarizationDrawable::OnGenerate,      // BINARIZATION,
    RSColorFilterDrawable::OnGenerate,       // COLOR_FILTER,
    RSLightUpEffectDrawable::OnGenerate,     // LIGHT_UP_EFFECT,
    RSDynamicDimDrawable::OnGenerate,        // DYNAMIC_DIM,
    RSCompositingFilterDrawable::OnGenerate, // COMPOSITING_FILTER,
    RSForegroundColorDrawable::OnGenerate,   // FOREGROUND_COLOR,
    nullptr,                                 // FG_RESTORE_BOUNDS,

    // No clip (unless ClipToBounds is set)
    RSPointLightDrawable::OnGenerate,                 // POINT_LIGHT,
    RSBorderDrawable::OnGenerate,                     // BORDER,
    ModifierGenerator<RSModifierType::OVERLAY_STYLE>, // OVERLAY,
    RSParticleDrawable::OnGenerate,                   // PARTICLE_EFFECT,
    RSPixelStretchDrawable::OnGenerate,               // PIXEL_STRETCH,

    // Restore state
    RSEndBlenderDrawable::OnGenerate,               // RESTORE_BLENDER,
    RSForegroundFilterRestoreDrawable::OnGenerate,  // RESTORE_FOREGROUND_FILTER
    nullptr,                                        // RESTORE_ALL,
};

enum DrawableVecStatus : uint8_t {
    CLIP_TO_BOUNDS     = 1 << 0,
    BG_BOUNDS_PROPERTY = 1 << 1,
    FG_BOUNDS_PROPERTY = 1 << 2,
    ENV_CHANGED        = 1 << 3,
    // Used by skip logic in RSRenderNode::UpdateDisplayList
    FRAME_NOT_EMPTY    = 1 << 4,
    NODE_NOT_EMPTY     = 1 << 5,

    // masks
    BOUNDS_MASK  = CLIP_TO_BOUNDS | BG_BOUNDS_PROPERTY | FG_BOUNDS_PROPERTY,
    FRAME_MASK   = FRAME_NOT_EMPTY,
    OTHER_MASK   = ENV_CHANGED,
};

inline static bool HasPropertyDrawableInRange(
    const RSDrawable::Vec& drawableVec, RSDrawableSlot begin, RSDrawableSlot end) noexcept
{
    // Note: the loop range is [begin, end], both end is included.
    auto beginIt = drawableVec.begin() + static_cast<size_t>(begin);
    auto endIt = drawableVec.begin() + static_cast<size_t>(end) + 1;
    return std::any_of(beginIt, endIt, [](const auto& Ptr) { return Ptr != nullptr; });
}

static uint8_t CalculateDrawableVecStatus(RSRenderNode& node, const RSDrawable::Vec& drawableVec)
{
    uint8_t result = 0;
    bool nodeNotEmpty = false;
    auto& properties = node.GetRenderProperties();

    // ClipToBounds if either 1. is surface node, 2. has explicit clip properties, 3. has blend mode
    bool shouldClipToBounds = node.IsInstanceOf<RSSurfaceRenderNode>() || properties.GetClipToBounds() ||
                              properties.GetClipToRRect() || properties.GetClipBounds() != nullptr ||
                              properties.GetColorBlendMode() != static_cast<int>(RSColorBlendMode::NONE) ||
                              properties.IsFgBrightnessValid();
    if (shouldClipToBounds) {
        result |= DrawableVecStatus::CLIP_TO_BOUNDS;
    }

    if (HasPropertyDrawableInRange(
        drawableVec, RSDrawableSlot::CONTENT_BEGIN, RSDrawableSlot::CONTENT_END)) {
        nodeNotEmpty = true;
        result |= DrawableVecStatus::FRAME_NOT_EMPTY;
    }

    if (HasPropertyDrawableInRange(
        drawableVec, RSDrawableSlot::BG_PROPERTIES_BEGIN, RSDrawableSlot::BG_PROPERTIES_END)) {
        result |= DrawableVecStatus::BG_BOUNDS_PROPERTY;
        nodeNotEmpty = true;
    }
    if (HasPropertyDrawableInRange(
        drawableVec, RSDrawableSlot::FG_PROPERTIES_BEGIN, RSDrawableSlot::FG_PROPERTIES_END)) {
        result |= DrawableVecStatus::FG_BOUNDS_PROPERTY;
        nodeNotEmpty = true;
    }

    nodeNotEmpty = nodeNotEmpty ||
        HasPropertyDrawableInRange(
            drawableVec, RSDrawableSlot::TRANSITION_PROPERTIES_BEGIN, RSDrawableSlot::TRANSITION_PROPERTIES_END) ||
        HasPropertyDrawableInRange(
            drawableVec, RSDrawableSlot::EXTRA_PROPERTIES_BEGIN, RSDrawableSlot::EXTRA_PROPERTIES_END);
    if (nodeNotEmpty) {
        // Set NODE_NOT_EMPTY flag if any drawable (include frame/bg/fg/transition/extra) is set
        result |= DrawableVecStatus::NODE_NOT_EMPTY;
    }

    // Foreground color & Background Effect & Blend Mode should be processed here
    if (drawableVec[static_cast<size_t>(RSDrawableSlot::ENV_FOREGROUND_COLOR)] ||
        drawableVec[static_cast<size_t>(RSDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY)] ||
        drawableVec[static_cast<size_t>(RSDrawableSlot::BLENDER)] ||
        (node.GetType() == RSRenderNodeType::EFFECT_NODE &&
            drawableVec[static_cast<size_t>(RSDrawableSlot::BACKGROUND_FILTER)])) {
        result |= DrawableVecStatus::ENV_CHANGED;
    }

    return result;
}

inline static void SaveRestoreHelper(RSDrawable::Vec& drawableVec, RSDrawableSlot slot1, RSDrawableSlot slot2,
    RSPaintFilterCanvas::SaveType type = RSPaintFilterCanvas::kCanvas)
{
    if (type == RSPaintFilterCanvas::kNone) {
        return;
    }
    if (type == RSPaintFilterCanvas::kCanvas) {
        auto count = std::make_shared<uint32_t>(std::numeric_limits<uint32_t>::max());
        drawableVec[static_cast<size_t>(slot1)] = std::make_unique<RSSaveDrawable>(count);
        drawableVec[static_cast<size_t>(slot2)] = std::make_unique<RSRestoreDrawable>(count);
    } else {
        auto status = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
        drawableVec[static_cast<size_t>(slot1)] = std::make_unique<RSCustomSaveDrawable>(status, type);
        drawableVec[static_cast<size_t>(slot2)] = std::make_unique<RSCustomRestoreDrawable>(status);
    }
}

static void OptimizeBoundsSaveRestore(RSRenderNode& node, RSDrawable::Vec& drawableVec, uint8_t flags)
{
    // Erase existing save/clip/restore before re-generating
    constexpr static std::array boundsSlotsToErase = {
        RSDrawableSlot::BG_SAVE_BOUNDS,
        RSDrawableSlot::CLIP_TO_BOUNDS,
        RSDrawableSlot::BG_RESTORE_BOUNDS,
        RSDrawableSlot::FG_SAVE_BOUNDS,
        RSDrawableSlot::FG_CLIP_TO_BOUNDS,
        RSDrawableSlot::FG_RESTORE_BOUNDS,
    };
    for (auto& slot : boundsSlotsToErase) {
        drawableVec[static_cast<size_t>(slot)] = nullptr;
    }

    if (flags & DrawableVecStatus::CLIP_TO_BOUNDS) {
        // case 1: ClipToBounds set.
        // add one clip, and reuse SAVE_ALL and RESTORE_ALL.
        drawableVec[static_cast<size_t>(RSDrawableSlot::CLIP_TO_BOUNDS)] = RSClipToBoundsDrawable::OnGenerate(node);
        return;
    }

    if ((flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) && (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY)) {
        // case 2: ClipToBounds not set and we have bounds properties both BG and FG.
        // add two sets of save/clip/restore before & after content.

        // part 1: before children
        SaveRestoreHelper(drawableVec, RSDrawableSlot::BG_SAVE_BOUNDS, RSDrawableSlot::BG_RESTORE_BOUNDS,
            RSPaintFilterCanvas::kCanvas);
        drawableVec[static_cast<size_t>(RSDrawableSlot::CLIP_TO_BOUNDS)] = RSClipToBoundsDrawable::OnGenerate(node);

        // part 2: after children, add aliases
        drawableVec[static_cast<size_t>(RSDrawableSlot::FG_SAVE_BOUNDS)] =
            drawableVec[static_cast<size_t>(RSDrawableSlot::BG_SAVE_BOUNDS)];
        drawableVec[static_cast<size_t>(RSDrawableSlot::FG_CLIP_TO_BOUNDS)] =
            drawableVec[static_cast<size_t>(RSDrawableSlot::CLIP_TO_BOUNDS)];
        drawableVec[static_cast<size_t>(RSDrawableSlot::FG_RESTORE_BOUNDS)] =
            drawableVec[static_cast<size_t>(RSDrawableSlot::BG_RESTORE_BOUNDS)];
        return;
    }

    if (flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) {
        // case 3: ClipToBounds not set and we have background bounds properties.
        SaveRestoreHelper(drawableVec, RSDrawableSlot::BG_SAVE_BOUNDS, RSDrawableSlot::BG_RESTORE_BOUNDS,
            RSPaintFilterCanvas::kCanvas);

        drawableVec[static_cast<size_t>(RSDrawableSlot::CLIP_TO_BOUNDS)] = RSClipToBoundsDrawable::OnGenerate(node);
        return;
    }

    if (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY) {
        // case 4: ClipToBounds not set and we have foreground bounds properties.
        SaveRestoreHelper(drawableVec, RSDrawableSlot::FG_SAVE_BOUNDS, RSDrawableSlot::FG_RESTORE_BOUNDS,
            RSPaintFilterCanvas::kCanvas);

        drawableVec[static_cast<size_t>(RSDrawableSlot::FG_CLIP_TO_BOUNDS)] = RSClipToBoundsDrawable::OnGenerate(node);
        return;
    }
    // case 5: ClipToBounds not set and no bounds properties, no need to save/clip/restore.
    // nothing to do
}

static void OptimizeFrameSaveRestore(RSRenderNode& node, RSDrawable::Vec& drawableVec, uint8_t flags)
{
    constexpr static std::array frameSlotsToErase = {
        RSDrawableSlot::SAVE_FRAME,
        RSDrawableSlot::RESTORE_FRAME,
    };
    // Erase existing save/clip/restore before re-generating
    for (auto& slot : frameSlotsToErase) {
        drawableVec[static_cast<size_t>(slot)] = nullptr;
    }

    if (flags & DrawableVecStatus::FRAME_NOT_EMPTY) {
        SaveRestoreHelper(
            drawableVec, RSDrawableSlot::SAVE_FRAME, RSDrawableSlot::RESTORE_FRAME, RSPaintFilterCanvas::kCanvas);
    }
}

static void OptimizeGlobalSaveRestore(RSRenderNode& node, RSDrawable::Vec& drawableVec, uint8_t flags)
{
    constexpr static std::array globalSlotsToErase = {
        RSDrawableSlot::SAVE_ALL,
        RSDrawableSlot::RESTORE_ALL,
    };
    // Erase existing save/clip/restore before re-generating
    for (auto& slot : globalSlotsToErase) {
        drawableVec[static_cast<size_t>(slot)] = nullptr;
    }

    // Parent will do canvas save/restore, we don't need to do it again
    uint8_t saveType = RSPaintFilterCanvas::SaveType::kNone;
    if (flags & DrawableVecStatus::ENV_CHANGED) {
        // If we change env(fg color, effect, blendMode etc), we need to save env
        saveType |= RSPaintFilterCanvas::SaveType::kEnv;
    }

    if (saveType == RSPaintFilterCanvas::SaveType::kNone) {
        return;
    }
    // add save/restore with needed type
    SaveRestoreHelper(drawableVec, RSDrawableSlot::SAVE_ALL, RSDrawableSlot::RESTORE_ALL,
        static_cast<RSPaintFilterCanvas::SaveType>(saveType));
}

constexpr std::array boundsDirtyTypes = {
    RSDrawableSlot::MASK,
    RSDrawableSlot::SHADOW,
    RSDrawableSlot::OUTLINE,
    RSDrawableSlot::FOREGROUND_FILTER,
    RSDrawableSlot::CLIP_TO_BOUNDS,
    RSDrawableSlot::BACKGROUND_COLOR,
    RSDrawableSlot::BACKGROUND_SHADER,
    RSDrawableSlot::BACKGROUND_IMAGE,
    RSDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY,
    RSDrawableSlot::FRAME_OFFSET,
    RSDrawableSlot::FG_CLIP_TO_BOUNDS,
    RSDrawableSlot::FOREGROUND_COLOR,
    RSDrawableSlot::POINT_LIGHT,
    RSDrawableSlot::BORDER,
    RSDrawableSlot::PIXEL_STRETCH,
    RSDrawableSlot::RESTORE_FOREGROUND_FILTER,
};
constexpr std::array frameDirtyTypes = {
    RSDrawableSlot::CLIP_TO_FRAME,
    RSDrawableSlot::COMPOSITING_FILTER,
};
constexpr std::array borderDirtyTypes = {
    RSDrawableSlot::BACKGROUND_COLOR,
    RSDrawableSlot::BACKGROUND_SHADER,
    RSDrawableSlot::BACKGROUND_IMAGE,
};
constexpr std::array bgfilterDirtyTypes = {
    RSDrawableSlot::PIXEL_STRETCH,
};
constexpr std::array stretchDirtyTypes = {
    RSDrawableSlot::BACKGROUND_FILTER,
};
const std::unordered_set<RSDrawableSlot> fuzeStretchBlurSafeList = {
    RSDrawableSlot::BG_RESTORE_BOUNDS,
    RSDrawableSlot::SAVE_FRAME,
    RSDrawableSlot::RESTORE_FRAME,
    RSDrawableSlot::FG_SAVE_BOUNDS,
    RSDrawableSlot::FG_RESTORE_BOUNDS,
};
template<std::size_t SIZE>
inline void MarkAffectedSlots(const std::array<RSDrawableSlot, SIZE>& affectedSlots, const RSDrawable::Vec& drawableVec,
    std::unordered_set<RSDrawableSlot>& dirtySlots)
{
    for (auto slot : affectedSlots) {
        if (drawableVec[static_cast<size_t>(slot)]) {
            dirtySlots.emplace(slot);
        }
    }
}
} // namespace

std::unordered_set<RSDrawableSlot> RSDrawable::CalculateDirtySlots(
    const ModifierDirtyTypes& dirtyTypes, const Vec& drawableVec)
{
    // Step 1.1: calculate dirty slots by looking up g_propertyToDrawableLut
    std::unordered_set<RSDrawableSlot> dirtySlots;
    for (const auto& dirtySlotIt : g_propertyToDrawableLut) {
        if (dirtyTypes.test(static_cast<size_t>(dirtySlotIt.first)) && dirtySlotIt.second != RSDrawableSlot::INVALID) {
            dirtySlots.emplace(dirtySlotIt.second);
        }
    }

    // Step 1.2: expand dirty slots by rules
    // if bounds or cornerRadius changed, mark affected drawables as dirty
    if (dirtyTypes.test(static_cast<size_t>(RSModifierType::BOUNDS)) ||
        dirtyTypes.test(static_cast<size_t>(RSModifierType::CORNER_RADIUS)) ||
        dirtyTypes.test(static_cast<size_t>(RSModifierType::CLIP_BOUNDS))) {
        MarkAffectedSlots(boundsDirtyTypes, drawableVec, dirtySlots);
    }

    if (dirtyTypes.test(static_cast<size_t>(RSModifierType::SHADOW_MASK)) || dirtySlots.count(RSDrawableSlot::SHADOW)) {
        dirtySlots.emplace(RSDrawableSlot::SHADOW);
        dirtySlots.emplace(RSDrawableSlot::FOREGROUND_FILTER);
    }

    if (dirtyTypes.test(static_cast<size_t>(RSModifierType::FRAME_GRAVITY))) {
        dirtySlots.emplace(RSDrawableSlot::CONTENT_STYLE);
        dirtySlots.emplace(RSDrawableSlot::FOREGROUND_STYLE);
    }

    // if frame changed, mark affected drawables as dirty
    if (dirtySlots.count(RSDrawableSlot::FRAME_OFFSET)) {
        MarkAffectedSlots(frameDirtyTypes, drawableVec, dirtySlots);
    }

    // if border changed, mark affected drawables as dirty
    if (dirtySlots.count(RSDrawableSlot::BORDER)) {
        MarkAffectedSlots(borderDirtyTypes, drawableVec, dirtySlots);
    }

    // PLANNING: merge these restore operations with RESTORE_ALL drawable
    if (dirtySlots.count(RSDrawableSlot::FOREGROUND_FILTER)) {
        dirtySlots.emplace(RSDrawableSlot::RESTORE_FOREGROUND_FILTER);
    }

    // if pixel stretch changed, mark affected drawables as dirty
    if (dirtySlots.count(RSDrawableSlot::PIXEL_STRETCH)) {
        MarkAffectedSlots(stretchDirtyTypes, drawableVec, dirtySlots);
    }
    // if background filter changed, mark affected drawables as dirty
    if (dirtySlots.count(RSDrawableSlot::BACKGROUND_FILTER)) {
        MarkAffectedSlots(bgfilterDirtyTypes, drawableVec, dirtySlots);
    }

    return dirtySlots;
}

bool RSDrawable::UpdateDirtySlots(
    const RSRenderNode& node, Vec& drawableVec, std::unordered_set<RSDrawableSlot>& dirtySlots)
{
    // Step 2: Update or generate all dirty slots
    bool drawableAddedOrRemoved = false;

    for (const auto& slot : dirtySlots) {
        if (auto& drawable = drawableVec[static_cast<size_t>(slot)]) {
            // If the slot is already created, call OnUpdate
            if (!drawable->OnUpdate(node)) {
                // If the slot is no longer needed, destroy it
                drawable.reset();
                drawableAddedOrRemoved = true;
            }
        } else if (auto& generator = g_drawableGeneratorLut[static_cast<int>(slot)]) {
            // If the slot is not created, call OnGenerate
            if (auto drawable = generator(node)) {
                drawableVec[static_cast<size_t>(slot)] = std::move(drawable);
                drawableAddedOrRemoved = true;
            }
        }
    }
    // If at this point the child node happens to be null, and the scenario involves deleting the child node
    // when the parent node is not on the tree, it is necessary to manually mark drawableAddedOrRemoved as true.
    if (!drawableAddedOrRemoved && dirtySlots.count(RSDrawableSlot::CHILDREN) &&
        drawableVec[static_cast<int8_t>(RSDrawableSlot::CHILDREN)] == nullptr) {
        drawableAddedOrRemoved = true;
    }

    return drawableAddedOrRemoved;
}

bool RSDrawable::FuzeDrawableSlots(const RSRenderNode& node, Vec& drawableVec)
{
    // fuze the pixel stretch with MESA blur
    if (!RSSystemProperties::GetMESABlurFuzedEnabled() ||
        !drawableVec[static_cast<size_t>(RSDrawableSlot::BACKGROUND_FILTER)] ||
        !drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)]) {
        return false;
    }

    auto &filterDrawable = drawableVec[static_cast<size_t>(RSDrawableSlot::BACKGROUND_FILTER)];
    auto bgFilterDrawable = std::static_pointer_cast<RSBackgroundFilterDrawable>(filterDrawable);
    bgFilterDrawable->RemovePixelStretch();

    auto &stretchDrawable = drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)];
    auto pixelStretchDrawable = std::static_pointer_cast<RSPixelStretchDrawable>(stretchDrawable);
    pixelStretchDrawable->OnUpdate(node);

    size_t start = static_cast<size_t>(RSDrawableSlot::BACKGROUND_FILTER) + 1;
    size_t end = static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH);
    // We do not fuze if drawableSlots between BACKGROUND_FILTER and PIXEL_STRETCH exist
    for (size_t ptr = start; ptr < end; ptr++) {
        if (!fuzeStretchBlurSafeList.count(static_cast<RSDrawableSlot>(ptr)) && drawableVec[ptr]) {
            return false;
        }
    }
    if (bgFilterDrawable->FuzePixelStretch(node)) {
        float INFTY = std::numeric_limits<float>::infinity();
        pixelStretchDrawable->SetPixelStretch(Vector4f{ INFTY, INFTY, INFTY, INFTY });
        return true;
    }

    return false;
}

void RSDrawable::UpdateSaveRestore(RSRenderNode& node, Vec& drawableVec, uint8_t& drawableVecStatus)
{
    // ====================================================================
    // Step 3: Universal save/clip/restore optimization

    // Step 3.1: calculate new drawable map status
    auto drawableVecStatusNew = CalculateDrawableVecStatus(node, drawableVec);

    uint8_t changedBits = drawableVecStatus ^ drawableVecStatusNew;
    if (changedBits == 0) {
        // nothing to do
        return;
    }

    // Step 3.2: update save/clip/restore for changed types
    if (changedBits & BOUNDS_MASK) {
        // update bounds save/clip if need
        OptimizeBoundsSaveRestore(node, drawableVec, drawableVecStatusNew);
    }
    if (changedBits & FRAME_MASK) {
        // update frame save/clip if need
        OptimizeFrameSaveRestore(node, drawableVec, drawableVecStatusNew);
    }
    if (changedBits & OTHER_MASK) {
        // update global save/clip if need
        OptimizeGlobalSaveRestore(node, drawableVec, drawableVecStatusNew);
    }

    drawableVecStatus = drawableVecStatusNew;
}
} // namespace OHOS::Rosen
