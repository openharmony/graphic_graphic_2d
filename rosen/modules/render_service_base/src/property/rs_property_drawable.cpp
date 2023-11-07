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

// key = RSModifierType, value = RSPropertyDrawableType
static const std::unordered_map<RSModifierType, RSPropertyDrawableSlot> g_propertyToDrawableLut = {
    { RSModifierType::INVALID, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::BOUNDS, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::FRAME, RSPropertyDrawableSlot::FRAME_OFFSET },
    { RSModifierType::POSITION_Z, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::PIVOT, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::PIVOT_Z, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::QUATERNION, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::ROTATION, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::ROTATION_X, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::ROTATION_Y, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::CAMERA_DISTANCE, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::SCALE, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::TRANSLATE, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::TRANSLATE_Z, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::SUBLAYER_TRANSFORM, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::CORNER_RADIUS, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::ALPHA, RSPropertyDrawableSlot::ALPHA },
    { RSModifierType::ALPHA_OFFSCREEN, RSPropertyDrawableSlot::ALPHA },
    { RSModifierType::FOREGROUND_COLOR, RSPropertyDrawableSlot::FOREGROUND_COLOR },
    { RSModifierType::BACKGROUND_COLOR, RSPropertyDrawableSlot::BACKGROUND_COLOR },
    { RSModifierType::BACKGROUND_SHADER, RSPropertyDrawableSlot::BACKGROUND_SHADER },
    { RSModifierType::BG_IMAGE, RSPropertyDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_WIDTH, RSPropertyDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_HEIGHT, RSPropertyDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_POSITION_X, RSPropertyDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_POSITION_Y, RSPropertyDrawableSlot::BACKGROUND_IMAGE },
    { RSModifierType::SURFACE_BG_COLOR, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::BORDER_COLOR, RSPropertyDrawableSlot::BORDER },
    { RSModifierType::BORDER_WIDTH, RSPropertyDrawableSlot::BORDER },
    { RSModifierType::BORDER_STYLE, RSPropertyDrawableSlot::BORDER },
    { RSModifierType::FILTER, RSPropertyDrawableSlot::FOREGROUND_FILTER },
    { RSModifierType::BACKGROUND_FILTER, RSPropertyDrawableSlot::BACKGROUND_FILTER },
    { RSModifierType::LINEAR_GRADIENT_BLUR_PARA, RSPropertyDrawableSlot::LINEAR_GRADIENT_BLUR_FILTER },
    { RSModifierType::DYNAMIC_LIGHT_UP_RATE, RSPropertyDrawableSlot::DYNAMIC_LIGHT_UP },
    { RSModifierType::DYNAMIC_LIGHT_UP_DEGREE, RSPropertyDrawableSlot::DYNAMIC_LIGHT_UP },
    { RSModifierType::FRAME_GRAVITY, RSPropertyDrawableSlot::FRAME_OFFSET },
    { RSModifierType::CLIP_RRECT, RSPropertyDrawableSlot::CLIP_TO_BOUNDS },
    { RSModifierType::CLIP_BOUNDS, RSPropertyDrawableSlot::CLIP_TO_BOUNDS },
    { RSModifierType::CLIP_TO_BOUNDS, RSPropertyDrawableSlot::CLIP_TO_BOUNDS },
    { RSModifierType::CLIP_TO_FRAME, RSPropertyDrawableSlot::CLIP_TO_FRAME },
    { RSModifierType::VISIBLE, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::SHADOW_COLOR, RSPropertyDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_OFFSET_X, RSPropertyDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_OFFSET_Y, RSPropertyDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_ALPHA, RSPropertyDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_ELEVATION, RSPropertyDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_RADIUS, RSPropertyDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_PATH, RSPropertyDrawableSlot::SHADOW },
    { RSModifierType::SHADOW_MASK, RSPropertyDrawableSlot::SHADOW },
    { RSModifierType::MASK, RSPropertyDrawableSlot::MASK },
    { RSModifierType::SPHERIZE, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::LIGHT_UP_EFFECT, RSPropertyDrawableSlot::LIGHT_UP_EFFECT },
    { RSModifierType::PIXEL_STRETCH, RSPropertyDrawableSlot::PIXEL_STRETCH },
    { RSModifierType::PIXEL_STRETCH_PERCENT, RSPropertyDrawableSlot::PIXEL_STRETCH },
    { RSModifierType::USE_EFFECT, RSPropertyDrawableSlot::USE_EFFECT },
    { RSModifierType::SANDBOX, RSPropertyDrawableSlot::BOUNDS_MATRIX },
    { RSModifierType::GRAY_SCALE, RSPropertyDrawableSlot::COLOR_FILTER },
    { RSModifierType::BRIGHTNESS, RSPropertyDrawableSlot::COLOR_FILTER },
    { RSModifierType::CONTRAST, RSPropertyDrawableSlot::COLOR_FILTER },
    { RSModifierType::SATURATE, RSPropertyDrawableSlot::COLOR_FILTER },
    { RSModifierType::SEPIA, RSPropertyDrawableSlot::COLOR_FILTER },
    { RSModifierType::INVERT, RSPropertyDrawableSlot::COLOR_FILTER },
    { RSModifierType::HUE_ROTATE, RSPropertyDrawableSlot::COLOR_FILTER },
    { RSModifierType::COLOR_BLEND, RSPropertyDrawableSlot::COLOR_FILTER },
    { RSModifierType::PARTICLE, RSPropertyDrawableSlot::PARTICLE_EFFECT },
    { RSModifierType::SHADOW_IS_FILLED, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::COLOR_BLENDMODE, RSPropertyDrawableSlot::SAVE_LAYER_CONTENT },
    { RSModifierType::CUSTOM, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::EXTENDED, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::TRANSITION, RSPropertyDrawableSlot::TRANSITION },
    { RSModifierType::BACKGROUND_STYLE, RSPropertyDrawableSlot::BACKGROUND_STYLE },
    { RSModifierType::CONTENT_STYLE, RSPropertyDrawableSlot::CONTENT_STYLE },
    { RSModifierType::FOREGROUND_STYLE, RSPropertyDrawableSlot::FOREGROUND_STYLE },
    { RSModifierType::OVERLAY_STYLE, RSPropertyDrawableSlot::OVERLAY },
    { RSModifierType::NODE_MODIFIER, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::ENV_FOREGROUND_COLOR, RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR },
    { RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY },
    { RSModifierType::GEOMETRYTRANS, RSPropertyDrawableSlot::INVALID },
};

// index = RSPropertyDrawableType, value = DrawableGenerator
static const std::vector<RSPropertyDrawable::DrawableGenerator> g_drawableGeneratorLut = {
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
    nullptr,                                                              // SAVE_LAYER_BACKGROUND
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
    RSSaveLayerContentDrawable::Generate,                                 // SAVE_LAYER_CONTENT

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
    nullptr,                                              // RESTORE_BOUNDS,

    RSPixelStretchDrawable::Generate,                     // PIXEL_STRETCH,
    nullptr, // RESTORE_ALL,
};

inline bool HasPropertyDrawableInRange(
    const RSPropertyDrawable::DrawableVec& drawableVec, RSPropertyDrawableSlot begin, RSPropertyDrawableSlot end)
{
    return std::any_of(drawableVec.begin() + begin, drawableVec.begin() + end + 1,
        [](const auto& drawablePtr) { return drawablePtr != nullptr; });
}
} // namespace

std::set<RSPropertyDrawableSlot> RSPropertyDrawable::GenerateDirtySlots(
    const RSProperties& properties, const std::unordered_set<RSModifierType>& dirtyTypes)
{
    // ====================================================================
    // Step 1.1: collect dirty slots
    std::set<RSPropertyDrawableSlot> dirtySlots;
    for (const auto& type : dirtyTypes) {
        auto it = g_propertyToDrawableLut.find(type);
        if (it == g_propertyToDrawableLut.end() || it->second == RSPropertyDrawableSlot::INVALID) {
            continue;
        }
        dirtySlots.emplace(it->second);
    }

    // Step 1.2: expand dirty slots if needed
    if (dirtyTypes.count(RSModifierType::BOUNDS)) {
        if (properties.GetPixelStretch().has_value()) {
            dirtySlots.emplace(RSPropertyDrawableSlot::PIXEL_STRETCH);
        }
        if (properties.GetBorder() != nullptr) {
            dirtySlots.emplace(RSPropertyDrawableSlot::BORDER);
        }
        // PLANNING: add other slots: ClipToFrame, ColorFilter
    }

    return dirtySlots;
}

bool RSPropertyDrawable::UpdateDrawableVec(const RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec,
    std::set<RSPropertyDrawableSlot>& dirtySlots)
{
    if (dirtySlots.empty()) {
        return false;
    }
    // ====================================================================
    // Step 2.1: re-generate drawables for all dirty slots
    auto drawableSlotChanged = false;
    for (const auto& slot : dirtySlots) {
        auto& origDrawable = drawableVec[slot];
        if (origDrawable != nullptr && origDrawable->Update(context)) {
            continue;
        }
        auto& generator = g_drawableGeneratorLut[static_cast<int>(slot)];
        if (!generator) {
            continue;
        }
        auto drawable = generator(context);
        if (bool(origDrawable) != bool(drawable)) {
            // drawable slot changed (nullptr to non-nullptr or vice versa)
            drawableSlotChanged = true;
        }
        origDrawable = std::move(drawable);
    }

    // Step 2.2: post-generate hooks (PLANNING: refactor this into a separate function)
    if (drawableSlotChanged && dirtySlots.count(RSPropertyDrawableSlot::SAVE_LAYER_CONTENT)) {
        if (drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_CONTENT] != nullptr) {
            drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_BACKGROUND] =
                RSSaveLayerBackgroundDrawable::Generate(context);
        } else {
            drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_BACKGROUND] = nullptr;
        }
    }

    return drawableSlotChanged;
}

void RSPropertyDrawable::UpdateSaveRestore(
    RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t& drawableVecStatus)
{
    // ====================================================================
    // Step 3: Universal save/clip/restore optimization

    // calculate new drawable map status
    auto drawableVecStatusNew = CalculateDrawableVecStatus(context, drawableVec);

    // calculate changed bits
    uint8_t changedBits = drawableVecStatus ^ drawableVecStatusNew;
    if (changedBits & BOUNDS_MASK) {
        // update bounds save/clip if need
        OptimizeBoundsSaveRestore(context, drawableVec, drawableVecStatusNew);
    }
    if (changedBits & FRAME_MASK) {
        // update frame save/clip if need
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

    if (HasPropertyDrawableInRange(drawableVec, RSPropertyDrawableSlot::BACKGROUND_COLOR,
            RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY)) {
        result |= DrawableVecStatus::BOUNDS_PROPERTY_BEFORE;
    }
    if (HasPropertyDrawableInRange(
            drawableVec, RSPropertyDrawableSlot::LIGHT_UP_EFFECT, RSPropertyDrawableSlot::PARTICLE_EFFECT)) {
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
    RSPropertyDrawableSlot::RESTORE_FRAME,
};
} // namespace

void RSPropertyDrawable::InitializeSaveRestore(
    const RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec)
{
    std::tie(drawableVec[RSPropertyDrawableSlot::SAVE_ALL], drawableVec[RSPropertyDrawableSlot::RESTORE_ALL]) =
        GenerateSaveRestore(RSPaintFilterCanvas::kALL);
    drawableVec[RSPropertyDrawableSlot::BOUNDS_MATRIX] = RSBoundsGeometryDrawable::Generate(context);
    drawableVec[RSPropertyDrawableSlot::FRAME_OFFSET] = RSFrameGeometryDrawable::Generate(context);
}

void RSPropertyDrawable::OptimizeBoundsSaveRestore(
    RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t flags)
{
    // Erase existing save/clip/restore before re-generating
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
    // Erase existing save/clip/restore before re-generating
    EraseSlots(drawableVec, frameSlotsToErase);

    // PLANNING: if both clipToFrame and clipToBounds are set, and frame == bounds, we don't need an extra clip
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
