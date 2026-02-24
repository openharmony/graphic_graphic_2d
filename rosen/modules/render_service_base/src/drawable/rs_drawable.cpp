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

#include "common/rs_common_tools.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_point_light_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {
using namespace TemplateUtils;
namespace {
using namespace DrawableV2;

static const std::unordered_map<ModifierNG::RSModifierType, RSDrawableSlot> g_propertyToDrawableLutNG = {
    { ModifierNG::RSModifierType::INVALID,                      RSDrawableSlot::INVALID },
    { ModifierNG::RSModifierType::BOUNDS,                       RSDrawableSlot::CLIP_TO_BOUNDS },
    { ModifierNG::RSModifierType::FRAME,                        RSDrawableSlot::FRAME_OFFSET },
    { ModifierNG::RSModifierType::TRANSFORM,                    RSDrawableSlot::INVALID },
    { ModifierNG::RSModifierType::ALPHA,                        RSDrawableSlot::INVALID },
    { ModifierNG::RSModifierType::FOREGROUND_COLOR,             RSDrawableSlot::FOREGROUND_COLOR },
    { ModifierNG::RSModifierType::BACKGROUND_COLOR,             RSDrawableSlot::BACKGROUND_COLOR },
    { ModifierNG::RSModifierType::BACKGROUND_SHADER,            RSDrawableSlot::BACKGROUND_SHADER },
    { ModifierNG::RSModifierType::BACKGROUND_NG_SHADER,         RSDrawableSlot::BACKGROUND_NG_SHADER },
    { ModifierNG::RSModifierType::BACKGROUND_IMAGE,             RSDrawableSlot::BACKGROUND_IMAGE },
    { ModifierNG::RSModifierType::BORDER,                       RSDrawableSlot::BORDER },
    { ModifierNG::RSModifierType::OUTLINE,                      RSDrawableSlot::OUTLINE },
    { ModifierNG::RSModifierType::CLIP_TO_BOUNDS,               RSDrawableSlot::CLIP_TO_BOUNDS },
    { ModifierNG::RSModifierType::CLIP_TO_FRAME,                RSDrawableSlot::CLIP_TO_FRAME },
    { ModifierNG::RSModifierType::VISIBILITY,                   RSDrawableSlot::INVALID },
    { ModifierNG::RSModifierType::DYNAMIC_LIGHT_UP,             RSDrawableSlot::DYNAMIC_LIGHT_UP },
    { ModifierNG::RSModifierType::SHADOW,                       RSDrawableSlot::SHADOW },
    { ModifierNG::RSModifierType::MASK,                         RSDrawableSlot::MASK },
    { ModifierNG::RSModifierType::PIXEL_STRETCH,                RSDrawableSlot::PIXEL_STRETCH },
    { ModifierNG::RSModifierType::USE_EFFECT,                   RSDrawableSlot::USE_EFFECT },
    { ModifierNG::RSModifierType::BLENDER,                      RSDrawableSlot::BLENDER },
    { ModifierNG::RSModifierType::POINT_LIGHT,                  RSDrawableSlot::POINT_LIGHT },
    { ModifierNG::RSModifierType::PARTICLE_EFFECT,              RSDrawableSlot::PARTICLE_EFFECT },
    { ModifierNG::RSModifierType::COMPOSITING_FILTER,           RSDrawableSlot::COMPOSITING_FILTER },
    { ModifierNG::RSModifierType::BACKGROUND_FILTER,            RSDrawableSlot::BACKGROUND_FILTER },
    { ModifierNG::RSModifierType::FOREGROUND_FILTER,            RSDrawableSlot::FOREGROUND_FILTER },
    { ModifierNG::RSModifierType::HDR_BRIGHTNESS,               RSDrawableSlot::FOREGROUND_FILTER },
    { ModifierNG::RSModifierType::TRANSITION_STYLE,             RSDrawableSlot::TRANSITION },
    { ModifierNG::RSModifierType::BACKGROUND_STYLE,             RSDrawableSlot::BACKGROUND_STYLE },
    { ModifierNG::RSModifierType::CONTENT_STYLE,                RSDrawableSlot::CONTENT_STYLE },
    { ModifierNG::RSModifierType::FOREGROUND_STYLE,             RSDrawableSlot::FOREGROUND_STYLE },
    { ModifierNG::RSModifierType::OVERLAY_STYLE,                RSDrawableSlot::OVERLAY },
    { ModifierNG::RSModifierType::NODE_MODIFIER,                RSDrawableSlot::INVALID },
    { ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR,         RSDrawableSlot::ENV_FOREGROUND_COLOR },
    { ModifierNG::RSModifierType::BEHIND_WINDOW_FILTER,         RSDrawableSlot::BACKGROUND_FILTER },
    { ModifierNG::RSModifierType::FOREGROUND_SHADER,            RSDrawableSlot::FOREGROUND_SHADER },
    { ModifierNG::RSModifierType::COLOR_PICKER,                 RSDrawableSlot::COLOR_PICKER },
    { ModifierNG::RSModifierType::MATERIAL_FILTER,              RSDrawableSlot::MATERIAL_FILTER },
    { ModifierNG::RSModifierType::CHILDREN,                     RSDrawableSlot::CHILDREN },
};

template<ModifierNG::RSModifierType type>
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
    RSMaskDrawable::OnGenerate,                                      // MASK,
    ModifierGenerator<ModifierNG::RSModifierType::TRANSITION_STYLE>, // TRANSITION_STYLE,
    RSEnvFGColorDrawable::OnGenerate,                                // ENV_FOREGROUND_COLOR,
    RSColorPickerDrawable::OnGenerate,                               // COLOR_PICKER,
    RSMaterialFilterDrawable::OnGenerate,                            // MATERIAL_FILTER,
    RSShadowDrawable::OnGenerate,                                    // SHADOW,
    RSForegroundFilterDrawable::OnGenerate,                          // FOREGROUND_FILTER
    RSOutlineDrawable::OnGenerate,                                   // OUTLINE,

    // BG properties in Bounds Clip
    nullptr,                                                         // BG_SAVE_BOUNDS,
    nullptr,                                                         // CLIP_TO_BOUNDS,
    RSBeginBlenderDrawable::OnGenerate,                              // BLENDER,
    RSBackgroundColorDrawable::OnGenerate,                           // BACKGROUND_COLOR,
    RSBackgroundShaderDrawable::OnGenerate,                          // BACKGROUND_SHADER,
    RSBackgroundNGShaderDrawable::OnGenerate,                        // BACKGROUND_NG_SHADER,
    RSBackgroundImageDrawable::OnGenerate,                           // BACKGROUND_IMAGE,
    RSBackgroundFilterDrawable::OnGenerate,                          // BACKGROUND_FILTER,
    RSUseEffectDrawable::OnGenerate,                                 // USE_EFFECT,
    ModifierGenerator<ModifierNG::RSModifierType::BACKGROUND_STYLE>, // BACKGROUND_STYLE,
    RSDynamicLightUpDrawable::OnGenerate,                            // DYNAMIC_LIGHT_UP,
    RSEnvFGColorStrategyDrawable::OnGenerate,                        // ENV_FOREGROUND_COLOR_STRATEGY,
    nullptr,                                                         // BG_RESTORE_BOUNDS,

    // Frame Geometry
    nullptr,                                                         // SAVE_FRAME,
    RSFrameOffsetDrawable::OnGenerate,                               // FRAME_OFFSET,
    RSClipToFrameDrawable::OnGenerate,                               // CLIP_TO_FRAME,
    RSCustomClipToFrameDrawable::OnGenerate,                         // CUSTOM_CLIP_TO_FRAME,
    ModifierGenerator<ModifierNG::RSModifierType::CONTENT_STYLE>,    // CONTENT_STYLE,
    RSChildrenDrawable::OnGenerate,                                  // CHILDREN,
    ModifierGenerator<ModifierNG::RSModifierType::FOREGROUND_STYLE>, // FOREGROUND_STYLE,
    nullptr,                                                         // RESTORE_FRAME,

    // FG properties in Bounds clip
    nullptr,                                                         // FG_SAVE_BOUNDS,
    nullptr,                                                         // FG_CLIP_TO_BOUNDS,
    RSBinarizationDrawable::OnGenerate,                              // BINARIZATION,
    RSColorFilterDrawable::OnGenerate,                               // COLOR_FILTER,
    RSLightUpEffectDrawable::OnGenerate,                             // LIGHT_UP_EFFECT,
    RSDynamicDimDrawable::OnGenerate,                                // DYNAMIC_DIM,
    RSCompositingFilterDrawable::OnGenerate,                         // COMPOSITING_FILTER,
    RSForegroundColorDrawable::OnGenerate,                           // FOREGROUND_COLOR,
    RSForegroundShaderDrawable::OnGenerate,                          // FOREGROUND_SHADER,
    nullptr,                                                         // FG_RESTORE_BOUNDS,

    // No clip (unless ClipToBounds is set)
    RSPointLightDrawable::OnGenerate,                                // POINT_LIGHT,
    RSBorderDrawable::OnGenerate,                                    // BORDER,
    ModifierGenerator<ModifierNG::RSModifierType::OVERLAY_STYLE>,    // OVERLAY,
    RSParticleDrawable::OnGenerate,                                  // PARTICLE_EFFECT,
    RSPixelStretchDrawable::OnGenerate,                              // PIXEL_STRETCH,

    // Restore state
    RSEndBlenderDrawable::OnGenerate,                                // RESTORE_BLENDER,
    RSForegroundFilterRestoreDrawable::OnGenerate,                   // RESTORE_FOREGROUND_FILTER
    nullptr,                                                         // RESTORE_ALL,
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
    auto beginIt = drawableVec.lower_bound(static_cast<int8_t>(begin));
    auto endIt = drawableVec.upper_bound(static_cast<int8_t>(end));
    return std::any_of(beginIt, endIt, [](const auto& drawablePair) {
        return drawablePair.second != nullptr;
    });
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
    if (findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::ENV_FOREGROUND_COLOR)) ||
        findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY)) ||
        findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::BLENDER)) ||
        (node.GetType() == RSRenderNodeType::EFFECT_NODE &&
            findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::BACKGROUND_FILTER))) ||
            findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER))) {
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
        drawableVec[static_cast<int8_t>(slot1)] = std::make_unique<RSSaveDrawable>(count);
        drawableVec[static_cast<int8_t>(slot2)] = std::make_unique<RSRestoreDrawable>(count);
    } else {
        auto status = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
        drawableVec[static_cast<int8_t>(slot1)] = std::make_unique<RSCustomSaveDrawable>(status, type);
        drawableVec[static_cast<int8_t>(slot2)] = std::make_unique<RSCustomRestoreDrawable>(status);
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
        drawableVec.erase(static_cast<int8_t>(slot));
    }

    if (flags & DrawableVecStatus::CLIP_TO_BOUNDS) {
        // case 1: ClipToBounds set.
        // add one clip, and reuse SAVE_ALL and RESTORE_ALL.
        assignOrEraseOnAccess(drawableVec, static_cast<int8_t>(RSDrawableSlot::CLIP_TO_BOUNDS),
            RSClipToBoundsDrawable::OnGenerate(node));
        return;
    }

    if ((flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) && (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY)) {
        // case 2: ClipToBounds not set and we have bounds properties both BG and FG.
        // add two sets of save/clip/restore before & after content.

        // part 1: before children
        SaveRestoreHelper(drawableVec, RSDrawableSlot::BG_SAVE_BOUNDS, RSDrawableSlot::BG_RESTORE_BOUNDS,
            RSPaintFilterCanvas::kCanvas);
        assignOrEraseOnAccess(drawableVec, static_cast<int8_t>(RSDrawableSlot::CLIP_TO_BOUNDS),
            RSClipToBoundsDrawable::OnGenerate(node));

        // part 2: after children, add aliases
        assignOrEraseOnAccess(drawableVec, static_cast<int8_t>(RSDrawableSlot::FG_SAVE_BOUNDS),
            findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::BG_SAVE_BOUNDS)));
        assignOrEraseOnAccess(drawableVec, static_cast<int8_t>(RSDrawableSlot::FG_CLIP_TO_BOUNDS),
            findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::CLIP_TO_BOUNDS)));
        assignOrEraseOnAccess(drawableVec, static_cast<int8_t>(RSDrawableSlot::FG_RESTORE_BOUNDS),
            findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::BG_RESTORE_BOUNDS)));
        return;
    }

    if (flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) {
        // case 3: ClipToBounds not set and we have background bounds properties.
        SaveRestoreHelper(drawableVec, RSDrawableSlot::BG_SAVE_BOUNDS, RSDrawableSlot::BG_RESTORE_BOUNDS,
            RSPaintFilterCanvas::kCanvas);

        assignOrEraseOnAccess(drawableVec, static_cast<int8_t>(RSDrawableSlot::CLIP_TO_BOUNDS),
            RSClipToBoundsDrawable::OnGenerate(node));
        return;
    }

    if (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY) {
        // case 4: ClipToBounds not set and we have foreground bounds properties.
        SaveRestoreHelper(drawableVec, RSDrawableSlot::FG_SAVE_BOUNDS, RSDrawableSlot::FG_RESTORE_BOUNDS,
            RSPaintFilterCanvas::kCanvas);

        assignOrEraseOnAccess(drawableVec, static_cast<int8_t>(RSDrawableSlot::FG_CLIP_TO_BOUNDS),
            RSClipToBoundsDrawable::OnGenerate(node));
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
        drawableVec.erase(static_cast<int8_t>(slot));
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
        drawableVec.erase(static_cast<int8_t>(slot));
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
    RSDrawableSlot::MATERIAL_FILTER,
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
        if (findMapValueRef(drawableVec, static_cast<int8_t>(slot))) {
            dirtySlots.emplace(slot);
        }
    }
}
} // namespace

std::unordered_set<RSDrawableSlot> RSDrawable::CalculateDirtySlotsNG(
    const ModifierNG::ModifierDirtyTypes& dirtyTypes, const Vec& drawableVec)
{
    // Step 1.1: calculate dirty slots by looking up g_propertyToDrawableLut
    std::unordered_set<RSDrawableSlot> dirtySlots;
    for (const auto& [modifierType, drawableSlot] : g_propertyToDrawableLutNG) {
        if (dirtyTypes.test(static_cast<size_t>(modifierType)) && drawableSlot != RSDrawableSlot::INVALID) {
            dirtySlots.emplace(drawableSlot);
        }
    }

    // Step 1.2: expand dirty slots by rules
    // if bounds or cornerRadius changed, mark affected drawables as dirty
    if (dirtyTypes.test(static_cast<size_t>(ModifierNG::RSModifierType::BOUNDS)) ||
        dirtyTypes.test(static_cast<size_t>(ModifierNG::RSModifierType::CLIP_TO_BOUNDS))) {
        MarkAffectedSlots(boundsDirtyTypes, drawableVec, dirtySlots);
    }

    if (dirtyTypes.test(static_cast<size_t>(ModifierNG::RSModifierType::SHADOW))) {
        dirtySlots.emplace(RSDrawableSlot::FOREGROUND_FILTER);
        // adapt to USE_SHADOW_BATCHING
        dirtySlots.emplace(RSDrawableSlot::CHILDREN);
    }

    if (dirtyTypes.test(static_cast<size_t>(ModifierNG::RSModifierType::FRAME))) {
        dirtySlots.emplace(RSDrawableSlot::CONTENT_STYLE);
        dirtySlots.emplace(RSDrawableSlot::FOREGROUND_STYLE);
    }

    if (dirtyTypes.test(static_cast<size_t>(ModifierNG::RSModifierType::COMPOSITING_FILTER))) {
        dirtySlots.emplace(RSDrawableSlot::LIGHT_UP_EFFECT);
        dirtySlots.emplace(RSDrawableSlot::BINARIZATION);
        dirtySlots.emplace(RSDrawableSlot::DYNAMIC_DIM);
        dirtySlots.emplace(RSDrawableSlot::COLOR_FILTER);
    }

    if (dirtyTypes.test(static_cast<size_t>(ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR))) {
        dirtySlots.emplace(RSDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY);
    }

    if (dirtyTypes.test(static_cast<size_t>(ModifierNG::RSModifierType::CLIP_TO_FRAME))) {
        dirtySlots.emplace(RSDrawableSlot::CUSTOM_CLIP_TO_FRAME);
        dirtySlots.emplace(RSDrawableSlot::FRAME_OFFSET);
        // CONTENT_STYLE and FOREGROUND_STYLE are used to adapt to FRAME_GRAVITY.
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

    // if pixel-stretch changed, mark affected drawables as dirty
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
        if (auto& drawable = findMapValueRef(drawableVec, static_cast<int8_t>(slot))) {
            // If the slot is already created, call OnUpdate
            if (!drawable->OnUpdate(node)) {
                // If the slot is no longer needed, destroy it
                drawable.reset();
                drawableAddedOrRemoved = true;
            }
        } else if (auto& generator = g_drawableGeneratorLut[static_cast<int>(slot)]) {
            // If the slot is not created, call OnGenerate
            if (auto drawable = generator(node)) {
                drawableVec[static_cast<int8_t>(slot)] = std::move(drawable);
                drawableAddedOrRemoved = true;
            }
        }
    }
    // If at this point the child node happens to be null, and the scenario involves deleting the child node
    // when the parent node is not on the tree, it is necessary to manually mark drawableAddedOrRemoved as true.
    if (!drawableAddedOrRemoved && dirtySlots.count(RSDrawableSlot::CHILDREN) &&
        findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::CHILDREN)) == nullptr) {
        drawableVec.erase(static_cast<int8_t>(RSDrawableSlot::CHILDREN));
        drawableAddedOrRemoved = true;
    }

    return drawableAddedOrRemoved;
}

void RSDrawable::ResetPixelStretchSlot(const RSRenderNode &node, Vec &drawableVec)
{
    auto &sptrStretchDrawable = findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::PIXEL_STRETCH));
    if (sptrStretchDrawable) {
        auto pixelStretchDrawable = std::static_pointer_cast<RSPixelStretchDrawable>(sptrStretchDrawable);
        pixelStretchDrawable->OnUpdate(node);
        float INFTY = std::numeric_limits<float>::infinity();
        pixelStretchDrawable->SetPixelStretch(Vector4f{INFTY, INFTY, INFTY, INFTY});
    }
}

bool RSDrawable::CanFusePixelStretch(Vec &drawableVec)
{
    if (!findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::BACKGROUND_FILTER)) ||
        !findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::PIXEL_STRETCH))) {
            return false;
    }

    auto itStart = static_cast<int8_t>(RSDrawableSlot::BACKGROUND_FILTER) + 1;
    auto itEnd = static_cast<int8_t>(RSDrawableSlot::PIXEL_STRETCH);
    // we do not fuze if drawableSlots between BACKGROUND_FILTER and PIXEL_STRETCH exist
    for (auto it = itStart; it < itEnd; ++it) {
        if (!fuzeStretchBlurSafeList.count(static_cast<RSDrawableSlot>(it)) && findMapValueRef(drawableVec, it)) {
            return false;
        }
    }
    return true;
}

bool RSDrawable::FuzeDrawableSlots(const RSRenderNode& node, Vec& drawableVec)
{
    // fuze the pixel stretch with MESA blur
    if (!RSSystemProperties::GetMESABlurFuzedEnabled() ||
        !findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::BACKGROUND_FILTER)) ||
        !findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::PIXEL_STRETCH))) {
        return false;
    }

    auto &filterDrawable = findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::BACKGROUND_FILTER));
    auto bgFilterDrawable = std::static_pointer_cast<RSBackgroundFilterDrawable>(filterDrawable);
    bgFilterDrawable->RemovePixelStretch();

    auto &stretchDrawable = findMapValueRef(drawableVec, static_cast<int8_t>(RSDrawableSlot::PIXEL_STRETCH));
    auto pixelStretchDrawable = std::static_pointer_cast<RSPixelStretchDrawable>(stretchDrawable);
    pixelStretchDrawable->OnUpdate(node);

    auto itStart = static_cast<int8_t>(RSDrawableSlot::BACKGROUND_FILTER) + 1;
    auto itEnd = static_cast<int8_t>(RSDrawableSlot::PIXEL_STRETCH);
    // We do not fuze if drawableSlots between BACKGROUND_FILTER and PIXEL_STRETCH exist
    for (auto it = itStart; it < itEnd; ++it) {
        if (!fuzeStretchBlurSafeList.count(static_cast<RSDrawableSlot>(it)) && drawableVec[it]) {
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
