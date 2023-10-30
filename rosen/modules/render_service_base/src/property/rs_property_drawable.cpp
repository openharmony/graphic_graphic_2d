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
using namespace Slot;
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
inline void EraseSlots(RSPropertyDrawable::DrawableVec& vec, const Container& slotsToErase)
{
    for (auto& slot : slotsToErase) {
        vec[slot] = nullptr;
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
    RSPropertyDrawableSlot::BACKGROUND_COLOR,              // BACKGROUND_COLOR,              // 19
    RSPropertyDrawableSlot::BACKGROUND_SHADER,             // BACKGROUND_SHADER,             // 20
    RSPropertyDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE,                      // 21
    RSPropertyDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE_WIDTH,                // 22
    RSPropertyDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE_HEIGHT,               // 23
    RSPropertyDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE_POSITION_X,           // 24
    RSPropertyDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE_POSITION_Y,           // 25
    RSPropertyDrawableSlot::INVALID,                       // SURFACE_BG_COLOR,              // 26
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
    RSPropertyDrawableSlot::INVALID,                       // SHADOW_IS_FILLED               // 65
    RSPropertyDrawableSlot::SAVE_LAYER_1,                  // BLEND_MODE,                    // 66
    RSPropertyDrawableSlot::SAVE_LAYER_2,                  // BLEND_MODE,                    // 67
    RSPropertyDrawableSlot::INVALID,                       // CUSTOM,                        // 68
    RSPropertyDrawableSlot::INVALID,                       // EXTENDED,                      // 69
    RSPropertyDrawableSlot::TRANSITION,                    // TRANSITION,                    // 70
    RSPropertyDrawableSlot::BACKGROUND_STYLE,              // BACKGROUND_STYLE,              // 71
    RSPropertyDrawableSlot::CONTENT_STYLE,                 // CONTENT_STYLE,                 // 72
    RSPropertyDrawableSlot::FOREGROUND_STYLE,              // FOREGROUND_STYLE,              // 73
    RSPropertyDrawableSlot::OVERLAY,                       // OVERLAY_STYLE,                 // 74
    RSPropertyDrawableSlot::INVALID,                       // NODE_MODIFIER,                 // 75
    RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR,          // ENV_FOREGROUND_COLOR,          // 76
    RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY, // ENV_FOREGROUND_COLOR_STRATEGY, // 77
    RSPropertyDrawableSlot::INVALID,                       // GEOMETRYTRANS,                 // 78
};

const std::vector<RSPropertyDrawable::DrawableGenerator> RSPropertyDrawable::DrawableGeneratorLut = {
    nullptr, // INVALID = 0,
    nullptr, // SAVE_ALL,

    // Bounds Geometry
    nullptr,                                                     // BOUNDS_MATRIX,
    RSAlphaDrawable::Generate,                                   // ALPHA,
    RSMaskDrawable::Generate,                                    // MASK,
    CustomModifierAdapter<RSModifierType::TRANSITION>,           // TRANSITION,
    CustomModifierAdapter<RSModifierType::ENV_FOREGROUND_COLOR>, // ENV_FOREGROUND_COLOR
    RSShadowDrawable::Generate,                                  // SHADOW,

    // In Bounds Clip
    RSSavelayer1Drawable::Generate,                                       // COLOR_BLENDMODE,SAVE_LAYER_1
    nullptr,                                                              // SAVE_BOUNDS,
    nullptr,                                                              // CLIP_TO_BOUNDS,
    RSBackgroundColorDrawable::Generate,                                  // BACKGROUND_COLOR,
    RSBackgroundShaderDrawable::Generate,                                 // BACKGROUND_SHADER
    RSBackgroundImageDrawable::Generate,                                  // BACKGROUND_IMAGE
    RSBackgroundFilterDrawable::Generate,                                 // BACKGROUND_FILTER,
    RSEffectDataApplyDrawable::Generate,                                  // USE_EFFECT
    CustomModifierAdapter<RSModifierType::BACKGROUND_STYLE>,              // BACKGROUND_STYLE
    RSDynamicLightUpDrawable::Generate,                                   // DYNAMIC_LIGHT_UP,
    CustomModifierAdapter<RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY>, // ENV_FOREGROUND_COLOR_STRATEGY
    nullptr,                                                              // RESTORE_BOUNDS_BEFORE_FRAME,
    RSSavelayer2Drawable::Generate,                                       // SAVE_LAYER_2

    // Frame Geometry
    nullptr,                                                 // SAVE_FRAME,
    nullptr,                                                 // FRAME_OFFSET,
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
    const RSPropertyDrawable::DrawableVec& drawableVec, RSPropertyDrawableSlot begin, RSPropertyDrawableSlot end)
{
    for (uint8_t index = begin; index <= end; index++) {
        if (drawableVec[index]) {
            return true;
        }
    }
    return false;
}

void RSPropertyDrawable::UpdateDrawableVec(RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec,
    uint8_t& drawableVecStatus, const std::unordered_set<RSModifierType>& dirtyTypes)
{
    // collect dirty slots
    std::set<RSPropertyDrawableSlot> dirtySlots;
    for (const auto& type : dirtyTypes) {
        dirtySlots.emplace(PropertyToDrawableLut[static_cast<int>(type)]);
    }

    // no dirty slots (except INVALID), just return
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
        drawableVec[slot] = std::move(drawable);
    }
    if (dirtySlots.count(RSPropertyDrawableSlot::BOUNDS_MATRIX)) {
        for (auto& drawable : drawableVec) {
            if (drawable) {
                drawable->OnBoundsChange(context.properties_);
            }
        }
    }

    if (drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_1] != nullptr) {
        drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_2] = RSSavelayer2Drawable::Generate(context);
    } else {
        drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_2] = nullptr;
    }

    auto drawableVecStatusNew = CalculateDrawableVecStatus(context, drawableVec);
    // initialize if needed
    if (drawableVecStatus == 0) {
        std::tie(drawableVec[RSPropertyDrawableSlot::SAVE_ALL], drawableVec[RSPropertyDrawableSlot::RESTORE_ALL]) =
            GenerateSaveRestore(RSPaintFilterCanvas::kALL);
        drawableVec[RSPropertyDrawableSlot::BOUNDS_MATRIX] = RSBoundsGeometryDrawable::Generate(context);
        drawableVec[RSPropertyDrawableSlot::FRAME_OFFSET] = RSFrameGeometryDrawable::Generate(context);
    }

    // calculate changed bits
    uint8_t changedBits = drawableVecStatus ^ drawableVecStatusNew;
    if (changedBits & BOUNDS_MASK) {
        // update bounds
        OptimizeBoundsSaveRestore(context, drawableVec, drawableVecStatusNew);
    }
    if (changedBits & FRAME_MASK) {
        // update frame
        OptimizeFrameSaveRestore(context, drawableVec, drawableVecStatusNew);
    }
    drawableVecStatus = drawableVecStatusNew;
}

inline uint8_t RSPropertyDrawable::CalculateDrawableVecStatus(
    RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec)
{
    uint8_t result = 0;
    auto& properties = context.properties_;

    if (properties.GetClipToBounds() || properties.GetClipToRRect() || properties.GetClipBounds() != nullptr) {
        result |= DrawableVecStatus::CLIP_BOUNDS;
    }
    if (properties.GetClipToFrame()) {
        result |= DrawableVecStatus::CLIP_FRAME;
    }
    if (context.hasChildren_) {
        result |= DrawableVecStatus::HAS_CHILDREN;
    }

    if (HasPropertyDrawableInRange(
        drawableVec, RSPropertyDrawableSlot::BACKGROUND_COLOR, RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY)) {
        result |= DrawableVecStatus::BOUNDS_PROPERTY_BEFORE;
    }
    if (HasPropertyDrawableInRange(
        drawableVec, RSPropertyDrawableSlot::LIGHT_UP_EFFECT, RSPropertyDrawableSlot::PIXEL_STRETCH)) {
        result |= DrawableVecStatus::BOUNDS_PROPERTY_AFTER;
    }
    if (HasPropertyDrawableInRange(
        drawableVec, RSPropertyDrawableSlot::FRAME_OFFSET, RSPropertyDrawableSlot::COLOR_FILTER)) {
        result |= DrawableVecStatus::FRAME_PROPERTY;
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
    RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t flags)
{
    // ======================================
    // PLANNING:
    // 1. erase unused slots - DONE
    // 2. update in a incremental manner - PARTIAL DONE
    // ======================================
    EraseSlots(drawableVec, boundsSlotsToErase);

    if (flags & DrawableVecStatus::CLIP_BOUNDS) {
        // case 1: ClipToBounds set.
        // add one clip, and reuse SAVE_ALL and RESTORE_ALL.
        drawableVec[RSPropertyDrawableSlot::CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);
        return;
    }

    if ((flags & DrawableVecStatus::BOUNDS_PROPERTY_BEFORE) && (flags & DrawableVecStatus::BOUNDS_PROPERTY_AFTER)) {
        // case 2: ClipToBounds not set and we have bounds properties both before & after children.
        // add two sets of save/clip/restore before & after children.

        // part 1: before children
        std::tie(drawableVec[RSPropertyDrawableSlot::SAVE_BOUNDS],
            drawableVec[RSPropertyDrawableSlot::EXTRA_RESTORE_BOUNDS]) =
            GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);
        drawableVec[RSPropertyDrawableSlot::CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);

        // part 2: after children, add aliases
        drawableVec[RSPropertyDrawableSlot::EXTRA_SAVE_BOUNDS] = GenerateAlias(RSPropertyDrawableSlot::SAVE_BOUNDS);
        drawableVec[RSPropertyDrawableSlot::EXTRA_CLIP_TO_BOUNDS] =
            GenerateAlias(RSPropertyDrawableSlot::CLIP_TO_BOUNDS);
        drawableVec[RSPropertyDrawableSlot::RESTORE_BOUNDS] =
            GenerateAlias(RSPropertyDrawableSlot::EXTRA_RESTORE_BOUNDS);
        return;
    }

    if (flags & DrawableVecStatus::BOUNDS_PROPERTY_BEFORE) {
        // case 3: ClipToBounds not set and we have bounds properties before children.
        std::tie(drawableVec[RSPropertyDrawableSlot::SAVE_BOUNDS],
            drawableVec[RSPropertyDrawableSlot::EXTRA_RESTORE_BOUNDS]) =
            GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);

        drawableVec[RSPropertyDrawableSlot::CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);
        return;
    }

    if (flags & DrawableVecStatus::BOUNDS_PROPERTY_AFTER) {
        // case 4: ClipToBounds not set and we have bounds properties after children.
        std::tie(drawableVec[RSPropertyDrawableSlot::EXTRA_SAVE_BOUNDS],
            drawableVec[RSPropertyDrawableSlot::RESTORE_BOUNDS]) = GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);

        drawableVec[RSPropertyDrawableSlot::EXTRA_CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);
        return;
    }
    // case 5: ClipToBounds not set and no bounds properties, no need to save/clip/restore.
    // nothing to do
}

void RSPropertyDrawable::OptimizeFrameSaveRestore(
    RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t flags)
{
    EraseSlots(drawableVec, frameSlotsToErase);

    // PLANNING: if both clipToFrame and clipToBounds are set, and frame == bounds, we don't need an extra
    if (flags & DrawableVecStatus::FRAME_PROPERTY) {
        // save/restore
        std::tie(drawableVec[RSPropertyDrawableSlot::SAVE_FRAME], drawableVec[RSPropertyDrawableSlot::RESTORE_FRAME]) =
            GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);
    } else {
        // no need to save/clip/restore
    }
}

RSPropertyDrawableGenerateContext::RSPropertyDrawableGenerateContext(RSRenderNode& node)
    : node_(node.shared_from_this()), properties_(node.GetRenderProperties()), hasChildren_(!node.GetChildren().empty())
{}

} // namespace OHOS::Rosen
