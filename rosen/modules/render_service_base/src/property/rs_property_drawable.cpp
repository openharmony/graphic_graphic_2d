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

#include "property/rs_property_drawable.h"

#include <set>

#include "pipeline/rs_render_node.h"
#include "property/rs_properties.h"
#include "property/rs_property_drawable_bounds_geometry.h"
#include "property/rs_property_drawable_frame_geometry.h"
#include "property/rs_property_drawable_utilities.h"

namespace OHOS::Rosen {
namespace {
template<RSModifierType T>
inline std::unique_ptr<RSModifierDrawable> CustomModifierAdapter(const RSPropertyDrawableGenerateContext&)
{
    return std::make_unique<RSModifierDrawable>(T);
}

inline RSPropertyDrawable::DrawablePtr GenerateAlias(RSPropertyDrawableSlot slot)
{
    return std::make_unique<RSAliasDrawable>(slot);
}

inline std::pair<RSPropertyDrawable::DrawablePtr, RSPropertyDrawable::DrawablePtr> GenerateSaveRestore(
    RSPaintFilterCanvas::SaveType type = RSPaintFilterCanvas::kCanvas)
{
    if (type == RSPaintFilterCanvas::kNone) {
        return {};
    } else if (type == RSPaintFilterCanvas::kCanvas) {
        auto count = std::make_shared<int>(-1);
        return { std::make_unique<RSSaveDrawable>(count), std::make_unique<RSRestoreDrawable>(count) };
    } else {
        auto status = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
        return { std::make_unique<RSCustomSaveDrawable>(status, type),
            std::make_unique<RSCustomRestoreDrawable>(status) };
    }
}
template<typename Container>
inline void EraseSlots(RSPropertyDrawable::DrawableMap& map, const Container& slotsToErase)
{
    for (auto& slot : slotsToErase) {
        map.erase(slot);
    }
}
} // namespace

const std::vector<RSPropertyDrawableSlot> RSPropertyDrawable::PropertyToDrawableLut = {
    RSPropertyDrawableSlot::INVALID,                       // INVALID = 0,                   // 0
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // BOUNDS,                        // 1
    RSPropertyDrawableSlot::FRAME_OFFSET,                  // FRAME,                         // 2
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // POSITION_Z,                    // 3
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // PIVOT,                         // 4
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // PIVOT_Z,                       // 5
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // QUATERNION,                    // 6
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // ROTATION,                      // 7
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // ROTATION_X,                    // 8
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // ROTATION_Y,                    // 9
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // CAMERA_DISTANCE,               // 10
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // SCALE,                         // 11
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // TRANSLATE,                     // 12
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // TRANSLATE_Z,                   // 13
    RSPropertyDrawableSlot::INVALID,                       // SUBLAYER_TRANSFORM,            // 14
    RSPropertyDrawableSlot::INVALID,                       // CORNER_RADIUS,                 // 15
    RSPropertyDrawableSlot::ALPHA,                         // ALPHA,                         // 16
    RSPropertyDrawableSlot::ALPHA,                         // ALPHA_OFFSCREEN,               // 17
    RSPropertyDrawableSlot::FOREGROUND_COLOR,              // FOREGROUND_COLOR,              // 18
    RSPropertyDrawableSlot::BACKGROUND,                    // BACKGROUND_COLOR,              // 19
    RSPropertyDrawableSlot::BACKGROUND,                    // BACKGROUND_SHADER,             // 20
    RSPropertyDrawableSlot::BACKGROUND,                    // BG_IMAGE,                      // 21
    RSPropertyDrawableSlot::BACKGROUND,                    // BG_IMAGE_WIDTH,                // 22
    RSPropertyDrawableSlot::BACKGROUND,                    // BG_IMAGE_HEIGHT,               // 23
    RSPropertyDrawableSlot::BACKGROUND,                    // BG_IMAGE_POSITION_X,           // 24
    RSPropertyDrawableSlot::BACKGROUND,                    // BG_IMAGE_POSITION_Y,           // 25
    RSPropertyDrawableSlot::BACKGROUND,                    // SURFACE_BG_COLOR,              // 26
    RSPropertyDrawableSlot::BORDER,                        // BORDER_COLOR,                  // 27
    RSPropertyDrawableSlot::BORDER,                        // BORDER_WIDTH,                  // 28
    RSPropertyDrawableSlot::BORDER,                        // BORDER_STYLE,                  // 29
    RSPropertyDrawableSlot::FOREGROUND_FILTER,             // FILTER,                        // 30
    RSPropertyDrawableSlot::BACKGROUND_FILTER,             // BACKGROUND_FILTER,             // 31
    RSPropertyDrawableSlot::LINEAR_GRADIENT_BLUR_FILTER,   // LINEAR_GRADIENT_BLUR_PARA,     // 32
    RSPropertyDrawableSlot::DYNAMIC_LIGHT_UP,              // DYNAMIC_LIGHT_UP_RATE,         // 33
    RSPropertyDrawableSlot::DYNAMIC_LIGHT_UP,              // DYNAMIC_LIGHT_UP_DEGREE,       // 34
    RSPropertyDrawableSlot::FRAME_OFFSET,                  // FRAME_GRAVITY,                 // 35
    RSPropertyDrawableSlot::INVALID,                       // CLIP_RRECT,                    // 36
    RSPropertyDrawableSlot::INVALID,                       // CLIP_BOUNDS,                   // 37
    RSPropertyDrawableSlot::INVALID,                       // CLIP_TO_BOUNDS,                // 38
    RSPropertyDrawableSlot::CLIP_TO_FRAME,                 // CLIP_TO_FRAME,                 // 39
    RSPropertyDrawableSlot::INVALID,                       // VISIBLE,                       // 40
    RSPropertyDrawableSlot::SHADOW,                        // SHADOW_COLOR,                  // 41
    RSPropertyDrawableSlot::SHADOW,                        // SHADOW_OFFSET_X,               // 42
    RSPropertyDrawableSlot::SHADOW,                        // SHADOW_OFFSET_Y,               // 43
    RSPropertyDrawableSlot::SHADOW,                        // SHADOW_ALPHA,                  // 44
    RSPropertyDrawableSlot::SHADOW,                        // SHADOW_ELEVATION,              // 45
    RSPropertyDrawableSlot::SHADOW,                        // SHADOW_RADIUS,                 // 46
    RSPropertyDrawableSlot::SHADOW,                        // SHADOW_PATH,                   // 47
    RSPropertyDrawableSlot::SHADOW,                        // SHADOW_MASK,                   // 48
    RSPropertyDrawableSlot::MASK,                          // MASK,                          // 49
    RSPropertyDrawableSlot::INVALID,                       // SPHERIZE,                      // 50
    RSPropertyDrawableSlot::LIGHT_UP_EFFECT,               // LIGHT_UP_EFFECT,               // 51
    RSPropertyDrawableSlot::PIXEL_STRETCH,                 // PIXEL_STRETCH,                 // 52
    RSPropertyDrawableSlot::PIXEL_STRETCH,                 // PIXEL_STRETCH_PERCENT,         // 53
    RSPropertyDrawableSlot::USE_EFFECT,                    // USE_EFFECT,                    // 54
    RSPropertyDrawableSlot::BOUNDS_MATRIX,                 // SANDBOX,                       // 55
    RSPropertyDrawableSlot::COLOR_FILTER,                  // GRAY_SCALE,                    // 56
    RSPropertyDrawableSlot::COLOR_FILTER,                  // BRIGHTNESS,                    // 57
    RSPropertyDrawableSlot::COLOR_FILTER,                  // CONTRAST,                      // 58
    RSPropertyDrawableSlot::COLOR_FILTER,                  // SATURATE,                      // 59
    RSPropertyDrawableSlot::COLOR_FILTER,                  // SEPIA,                         // 60
    RSPropertyDrawableSlot::COLOR_FILTER,                  // INVERT,                        // 61
    RSPropertyDrawableSlot::COLOR_FILTER,                  // HUE_ROTATE,                    // 62
    RSPropertyDrawableSlot::COLOR_FILTER,                  // COLOR_BLEND,                   // 63
    RSPropertyDrawableSlot::PARTICLE_EFFECT,               // PARTICLE,                      // 64
    RSPropertyDrawableSlot::INVALID,                       // CUSTOM,                        // 65
    RSPropertyDrawableSlot::INVALID,                       // EXTENDED,                      // 66
    RSPropertyDrawableSlot::TRANSITION,                    // TRANSITION,                    // 67
    RSPropertyDrawableSlot::BACKGROUND,                    // BACKGROUND_STYLE,              // 68
    RSPropertyDrawableSlot::CONTENT_STYLE,                 // CONTENT_STYLE,                 // 69
    RSPropertyDrawableSlot::FOREGROUND_STYLE,              // FOREGROUND_STYLE,              // 70
    RSPropertyDrawableSlot::OVERLAY,                       // OVERLAY_STYLE,                 // 71
    RSPropertyDrawableSlot::INVALID,                       // NODE_MODIFIER,                 // 72
    RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR,          // ENV_FOREGROUND_COLOR,          // 73
    RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY, // ENV_FOREGROUND_COLOR_STRATEGY, // 74
    RSPropertyDrawableSlot::INVALID,                       // GEOMETRYTRANS,                 // 75
};

const std::vector<RSPropertyDrawable::DrawableGenerator> RSPropertyDrawable::DrawableGeneratorLut = {
    nullptr, // INVALID = 0,
    nullptr, // SAVE_ALL,

    // Bounds Geometry
    RSBoundsGeometryDrawable::Generate,                          // BOUNDS_MATRIX,
    RSAlphaDrawable::Generate,                                   // ALPHA,
    RSMaskDrawable::Generate,                                    // MASK,
    CustomModifierAdapter<RSModifierType::TRANSITION>,           // TRANSITION,
    CustomModifierAdapter<RSModifierType::ENV_FOREGROUND_COLOR>, // ENV_FOREGROUND_COLOR
    RSShadowDrawable::Generate,                                  // SHADOW,

    // In Bounds Clip
    nullptr,                                                              // SAVE_BOUNDS,
    nullptr,                                                              // CLIP_TO_BOUNDS,
    RSBackgroundDrawable::Generate,                                       // BACKGROUND,
    RSBackgroundFilterDrawable::Generate,                                 // BACKGROUND_FILTER,
    RSEffectDataApplyDrawable::Generate,                                 // USE_EFFECT
    CustomModifierAdapter<RSModifierType::BACKGROUND_STYLE>,              // ENV_FOREGROUND_COLOR
    RSDynamicLightUpDrawable::Generate,                                   // DYNAMIC_LIGHT_UP,
    CustomModifierAdapter<RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY>, // ENV_FOREGROUND_COLOR_STRATEGY
    nullptr,                                                              // RESTORE_BOUNDS_BEFORE_FRAME,

    // Frame Geometry
    nullptr,                                                 // SAVE_FRAME,
    RSFrameGeometryDrawable::Generate,                       // FRAME_OFFSET,
    RSClipFrameDrawable::Generate,                           // CLIP_TO_FRAME,
    CustomModifierAdapter<RSModifierType::CONTENT_STYLE>,    // CONTENT_STYLE
    nullptr,                                                 // CHILDREN,
    CustomModifierAdapter<RSModifierType::FOREGROUND_STYLE>, // FOREGROUND_STYLE
    RSColorFilterDrawable::Generate,                         // COLOR_FILTER,
    nullptr,                                                 // RESTORE_FRAME,

    // In Bounds clip (again)
    nullptr,                                              // SAVE_BOUNDS_AFTER_CHILDREN,
    nullptr,                                              // CLIP_TO_BOUNDS_AFTER_CHILDREN,
    RSLightUpEffectDrawable::Generate,                    // LIGHT_UP_EFFECT,
    RSForegroundFilterDrawable::Generate,                 // FOREGROUND_FILTER,
    RSLinearGradientBlurFilterDrawable::Generate,         // LINEAR_GRADIENT_BLUR_FILTER,
    RSBorderDrawable::Generate,                           // BORDER,
    CustomModifierAdapter<RSModifierType::OVERLAY_STYLE>, // OVERLAY
    RSForegroundColorDrawable::Generate,                  // FOREGROUND_COLOR,
    RSParticleDrawable::Generate,                         // PARTICLE_EFFECT,
    RSPixelStretchDrawable::Generate,                     // PIXEL_STRETCH,
    nullptr,                                              // RESTORE_BOUNDS,

    nullptr, // RESTORE_ALL,
};

inline bool HasPropertyDrawableInRange(
    const RSPropertyDrawable::DrawableMap& drawableMap, RSPropertyDrawableSlot begin, RSPropertyDrawableSlot end)
{
    return drawableMap.lower_bound(begin) != drawableMap.upper_bound(end);
}

void RSPropertyDrawable::UpdateDrawableMap(RSPropertyDrawableGenerateContext& context, DrawableMap& drawableMap,
    uint8_t& drawableMapStatus, const std::unordered_set<RSModifierType>& dirtyTypes)
{
    // collect dirty slots
    std::set<RSPropertyDrawableSlot> dirtySlots;
    for (const auto& type : dirtyTypes) {
        dirtySlots.emplace(PropertyToDrawableLut[static_cast<int>(type)]);
    }

    // count all slots after INVALID
    if (dirtySlots.lower_bound(RSPropertyDrawableSlot::BOUNDS_MATRIX) == dirtySlots.end()) {
        return;
    }
    // re-generate drawables for all dirty slots
    for (const auto& slot : dirtySlots) {
        auto& generator = DrawableGeneratorLut[static_cast<int>(slot)];
        if (!generator) {
            continue;
        }
        auto drawable = generator(context);
        if (!drawable) {
            drawableMap.erase(slot);
        } else {
            drawableMap[slot] = std::move(drawable);
        }
    }

    auto drawableMapStatusNew = CalculateDrawableMapStatus(context, drawableMap);
    if (drawableMapStatusNew == 0) {
        drawableMap.clear();
        drawableMapStatus = 0;
        return;
    }

    // initialize
    if (drawableMapStatus == 0) {
        std::tie(drawableMap[RSPropertyDrawableSlot::SAVE_ALL], drawableMap[RSPropertyDrawableSlot::RESTORE_ALL]) =
            GenerateSaveRestore();
    }

    // calculate changed bits
    auto changedBits = drawableMapStatus ^ drawableMapStatusNew;
    if (changedBits & BOUNDS_MASK) {
        // update bounds
        OptimizeBoundsSaveRestore(context, drawableMap, drawableMapStatusNew);
    }
    if (changedBits & FRAME_MASK) {
        // update frame
        OptimizeFrameSaveRestore(context, drawableMap, drawableMapStatusNew);
    }
    drawableMapStatus = drawableMapStatusNew;
}

uint8_t RSPropertyDrawable::CalculateDrawableMapStatus(
    RSPropertyDrawableGenerateContext& context, DrawableMap& drawableMap)
{
    uint8_t result = 0;
    auto& properties = context.properties_;

    if (properties.GetClipToBounds() || properties.GetClipToRRect() || properties.GetClipBounds() != nullptr) {
        result |= DrawableMapStatus::CLIP_TO_BOUNDS;
    }
    if (properties.GetClipToFrame()) {
        result |= DrawableMapStatus::CLIP_TO_FRAME;
    }
    if (context.hasChildren_) {
        result |= DrawableMapStatus::HAS_CHILDREN;
    }

    if (HasPropertyDrawableInRange(
        drawableMap, RSPropertyDrawableSlot::BACKGROUND, RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY)) {
        result |= DrawableMapStatus::BOUNDS_PROPERTY_BEFORE;
    }
    if (HasPropertyDrawableInRange(
        drawableMap, RSPropertyDrawableSlot::LIGHT_UP_EFFECT, RSPropertyDrawableSlot::PIXEL_STRETCH)) {
        result |= DrawableMapStatus::BOUNDS_PROPERTY_AFTER;
    }
    if (HasPropertyDrawableInRange(
        drawableMap, RSPropertyDrawableSlot::CONTENT_STYLE, RSPropertyDrawableSlot::COLOR_FILTER)) {
        result |= DrawableMapStatus::FRAME_PROPERTY;
    }

    return result;
}

namespace {
constexpr std::array<RSPropertyDrawableSlot, 6> boundsSlotsToErase = {
    RSPropertyDrawableSlot::SAVE_BOUNDS,
    RSPropertyDrawableSlot::CLIP_TO_BOUNDS,
    RSPropertyDrawableSlot::EXTRA_RESTORE_BOUNDS,
    RSPropertyDrawableSlot::EXTRA_SAVE_BOUNDS,
    RSPropertyDrawableSlot::EXTRA_CLIP_TO_BOUNDS,
    RSPropertyDrawableSlot::RESTORE_BOUNDS,
};

constexpr std::array<RSPropertyDrawableSlot, 3> frameSlotsToErase = {
    RSPropertyDrawableSlot::SAVE_FRAME,
    RSPropertyDrawableSlot::CLIP_TO_FRAME,
    RSPropertyDrawableSlot::RESTORE_FRAME,
};
} // namespace

void RSPropertyDrawable::OptimizeBoundsSaveRestore(
    RSPropertyDrawableGenerateContext& context, DrawableMap& drawableMap, uint8_t flags)
{
    // ======================================
    // PLANNING:
    // 1. erase unused slots - DONE
    // 2. update in a incremental manner - PARTIAL DONE
    // ======================================
    EraseSlots(drawableMap, boundsSlotsToErase);

    if (flags & DrawableMapStatus::CLIP_TO_BOUNDS) {
        // case 1: ClipToBounds set.
        // add one clip, and reuse SAVE_ALL and RESTORE_ALL.
        drawableMap[RSPropertyDrawableSlot::CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);
        return;
    }

    if ((flags & DrawableMapStatus::BOUNDS_PROPERTY_BEFORE) && (flags & DrawableMapStatus::BOUNDS_PROPERTY_AFTER)) {
        // case 2: ClipToBounds not set and we have bounds properties both before & after children.
        // add two sets of save/clip/restore before & after children.

        // part 1: before children
        std::tie(drawableMap[RSPropertyDrawableSlot::SAVE_BOUNDS],
            drawableMap[RSPropertyDrawableSlot::EXTRA_RESTORE_BOUNDS]) =
            GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);
        drawableMap[RSPropertyDrawableSlot::CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);

        // part 2: after children, add aliases
        drawableMap[RSPropertyDrawableSlot::EXTRA_SAVE_BOUNDS] = GenerateAlias(RSPropertyDrawableSlot::SAVE_BOUNDS);
        drawableMap[RSPropertyDrawableSlot::EXTRA_CLIP_TO_BOUNDS] =
            GenerateAlias(RSPropertyDrawableSlot::CLIP_TO_BOUNDS);
        drawableMap[RSPropertyDrawableSlot::RESTORE_BOUNDS] =
            GenerateAlias(RSPropertyDrawableSlot::EXTRA_RESTORE_BOUNDS);
        return;
    }

    if (flags & DrawableMapStatus::BOUNDS_PROPERTY_BEFORE) {
        // case 3: ClipToBounds not set and we have bounds properties before children.
        std::tie(drawableMap[RSPropertyDrawableSlot::SAVE_BOUNDS],
            drawableMap[RSPropertyDrawableSlot::EXTRA_RESTORE_BOUNDS]) =
            GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);

        drawableMap[RSPropertyDrawableSlot::CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);
        return;
    }

    if (flags & DrawableMapStatus::BOUNDS_PROPERTY_AFTER) {
        // case 4: ClipToBounds not set and we have bounds properties after children.
        std::tie(drawableMap[RSPropertyDrawableSlot::EXTRA_SAVE_BOUNDS],
            drawableMap[RSPropertyDrawableSlot::RESTORE_BOUNDS]) = GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);

        drawableMap[RSPropertyDrawableSlot::EXTRA_CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);
        return;
    }
    // case 5: ClipToBounds not set and no bounds properties, no need to save/clip/restore.
    // nothing to do
}

void RSPropertyDrawable::OptimizeFrameSaveRestore(
    RSPropertyDrawableGenerateContext& context, DrawableMap& drawableMap, uint8_t flags)
{
    EraseSlots(drawableMap, frameSlotsToErase);

    // PLANNING: if both clipToFrame and clipToBounds are set, and frame == bounds, we don't need an extra
    if (flags & DrawableMapStatus::FRAME_PROPERTY) {
        // save/restore
        std::tie(drawableMap[RSPropertyDrawableSlot::SAVE_FRAME], drawableMap[RSPropertyDrawableSlot::RESTORE_FRAME]) =
            GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);
    } else {
        // no need to save/clip/restore
    }
}

RSPropertyDrawableGenerateContext::RSPropertyDrawableGenerateContext(RSRenderNode& node)
    : node_(node.shared_from_this()), properties_(node.GetRenderProperties()), hasChildren_(!node.GetChildren().empty())
{}

RSPropertyDrawableRenderContext::RSPropertyDrawableRenderContext(RSRenderNode& node, RSPaintFilterCanvas* canvas)
    : RSModifierContext(node.GetMutableRenderProperties(), canvas), node_(node.shared_from_this()),
      children_(node.GetSortedChildren()), drawableMap_(node.propertyDrawablesMap_),
      drawCmdModifiers_(node.drawCmdModifiers_)
{}

} // namespace OHOS::Rosen
