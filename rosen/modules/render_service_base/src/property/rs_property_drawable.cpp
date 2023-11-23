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

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
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
    { RSModifierType::AIINVERT, RSPropertyDrawableSlot::BINARIZATION },
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
    { RSModifierType::COLOR_BLEND_MODE, RSPropertyDrawableSlot::SAVE_LAYER_CONTENT },
    { RSModifierType::OUTER_BORDER_COLOR, RSPropertyDrawableSlot::OUTER_BORDER },
    { RSModifierType::OUTER_BORDER_WIDTH, RSPropertyDrawableSlot::OUTER_BORDER },
    { RSModifierType::OUTER_BORDER_STYLE, RSPropertyDrawableSlot::OUTER_BORDER },
    { RSModifierType::OUTER_BORDER_COLOR, RSPropertyDrawableSlot::OUTER_BORDER },
    { RSModifierType::USE_SHADOW_BATCHING, RSPropertyDrawableSlot::INVALID },
    { RSModifierType::LIGHT_INTENSITY, RSPropertyDrawableSlot::POINT_LIGHT },
    { RSModifierType::LIGHT_POSITION, RSPropertyDrawableSlot::POINT_LIGHT },
    { RSModifierType::ILLUMINATED_TYPE, RSPropertyDrawableSlot::POINT_LIGHT },
    { RSModifierType::BLOOM, RSPropertyDrawableSlot::POINT_LIGHT },
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

// NOTE: This LUT should always the same size as RSPropertyDrawableSlot
// index = RSPropertyDrawableType, value = DrawableGenerator
static const std::array<RSPropertyDrawable::DrawableGenerator, RSPropertyDrawableSlot::MAX> g_drawableGeneratorLut = {
    nullptr, // INVALID = 0,
    nullptr, // SAVE_ALL,

    // Bounds Geometry
    nullptr,                                                     // BOUNDS_MATRIX,
    RSAlphaDrawable::Generate,                                   // ALPHA,
    RSMaskDrawable::Generate,                                    // MASK,
    CustomModifierAdapter<RSModifierType::TRANSITION>,           // TRANSITION,
    CustomModifierAdapter<RSModifierType::ENV_FOREGROUND_COLOR>, // ENV_FOREGROUND_COLOR
    RSShadowDrawable::Generate,                                  // SHADOW,
    RSBorderDrawable::GenerateOuter,                             // OUTER_BORDER,

    // BG properties in Bounds Clip
    nullptr,                                                              // SAVE_LAYER_BACKGROUND
    nullptr,                                                              // BG_SAVE_BOUNDS
    nullptr,                                                              // CLIP_TO_BOUNDS,
    RSBackgroundColorDrawable::Generate,                                  // BACKGROUND_COLOR,
    RSBackgroundShaderDrawable::Generate,                                 // BACKGROUND_SHADER
    RSBackgroundImageDrawable::Generate,                                  // BACKGROUND_IMAGE
    RSBackgroundFilterDrawable::Generate,                                 // BACKGROUND_FILTER,
    RSEffectDataApplyDrawable::Generate,                                  // USE_EFFECT
    CustomModifierAdapter<RSModifierType::BACKGROUND_STYLE>,              // BACKGROUND_STYLE
    RSDynamicLightUpDrawable::Generate,                                   // DYNAMIC_LIGHT_UP,
    CustomModifierAdapter<RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY>, // ENV_FOREGROUND_COLOR_STRATEGY
    nullptr,                                                              // BG_RESTORE_BOUNDS,

    // Frame Geometry
    nullptr,                                                 // SAVE_LAYER_CONTENT
    nullptr,                                                 // SAVE_FRAME,
    nullptr,                                                 // FRAME_OFFSET,
    RSClipFrameDrawable::Generate,                           // CLIP_TO_FRAME,
    CustomModifierAdapter<RSModifierType::CONTENT_STYLE>,    // CONTENT_STYLE
    nullptr,                                                 // CHILDREN,
    CustomModifierAdapter<RSModifierType::FOREGROUND_STYLE>, // FOREGROUND_STYLE
    nullptr,                                                 // RESTORE_FRAME,
    nullptr,                                                 // RESTORE_CONTENT

    // FG properties in Bounds clip
    nullptr,                                      // FG_SAVE_BOUNDS,
    nullptr,                                      // EXTRA_CLIP_TO_BOUNDS,
    RSColorFilterDrawable::Generate,              // COLOR_FILTER,
    RSBinarizationDrawable::Generate,             // BINARIZATION,
    RSLightUpEffectDrawable::Generate,            // LIGHT_UP_EFFECT,
    RSForegroundFilterDrawable::Generate,         // FOREGROUND_FILTER,
    RSLinearGradientBlurFilterDrawable::Generate, // LINEAR_GRADIENT_BLUR_FILTER,
    RSForegroundColorDrawable::Generate,          // FOREGROUND_COLOR,
    nullptr,                                      // FG_RESTORE_BOUNDS
    nullptr,                                      // RESTORE_BACKGROUND

    // No clip (unless ClipToBounds is set)
    RSPointLightDrawable::Generate,                       // POINT_LIGHT
    RSBorderDrawable::Generate,                           // BORDER,
    CustomModifierAdapter<RSModifierType::OVERLAY_STYLE>, // OVERLAY
    RSParticleDrawable::Generate,                         // PARTICLE_EFFECT,
    RSPixelStretchDrawable::Generate,                     // PIXEL_STRETCH,

    nullptr, // RESTORE_ALL,
};
} // namespace

#ifndef USE_ROSEN_DRAWING
std::unordered_set<RSPropertyDrawableSlot> RSPropertyDrawable::GenerateDirtySlots(
    const RSProperties& properties, const std::unordered_set<RSModifierType>& dirtyTypes)
{
    // ====================================================================
    // Step 1.1: collect dirty slots
    std::unordered_set<RSPropertyDrawableSlot> dirtySlots;
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

        if (properties.GetOuterBorder() != nullptr) {
            dirtySlots.emplace(RSPropertyDrawableSlot::OUTER_BORDER);
        }
        // PLANNING: add other slots: ClipToFrame, ColorFilter
    }
    if (dirtyTypes.count(RSModifierType::CORNER_RADIUS)) {
        // border may should be updated with corner radius
        if (properties.GetBorder() != nullptr) {
            dirtySlots.emplace(RSPropertyDrawableSlot::BORDER);
        }

        if (properties.GetOuterBorder() != nullptr) {
            dirtySlots.emplace(RSPropertyDrawableSlot::OUTER_BORDER);
        }
    }

    return dirtySlots;
}
#else
std::unordered_set<RSPropertyDrawableSlot> RSPropertyDrawable::GenerateDirtySlots(
    const RSProperties& properties,
    std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)>& dirtyTypes)
{
    // Step 1.1: collect dirty slots
    std::unordered_set<RSPropertyDrawableSlot> dirtySlots;
    for (uint8_t type = 0; type < static_cast<size_t>(RSModifierType::MAX_RS_MODIFIER_TYPE); type++) {
        if (dirtyTypes[type]) {
            auto it = g_propertyToDrawableLut.find(static_cast<RSModifierType>(type));
            if (it == g_propertyToDrawableLut.end() || it->second == RSPropertyDrawableSlot::INVALID) {
                continue;
            }
            dirtySlots.emplace(it->second);
        }
    }

    // Step 1.2: expand dirty slots if needed
    if (dirtyTypes.test(static_cast<size_t>(RSModifierType::BOUNDS))) {
        if (properties.GetPixelStretch().has_value()) {
            dirtySlots.emplace(RSPropertyDrawableSlot::PIXEL_STRETCH);
        }
        if (properties.GetBorder() != nullptr) {
            dirtySlots.emplace(RSPropertyDrawableSlot::BORDER);
        }
        // PLANNING: add other slots: ClipToFrame, ColorFilter
    }
    if (dirtyTypes.test(static_cast<size_t>(RSModifierType::CORNER_RADIUS))
        && (properties.GetBorder() != nullptr)) {
        // border may should be updated with corner radius
        dirtySlots.emplace(RSPropertyDrawableSlot::BORDER);
    }

    return dirtySlots;
}
#endif

bool RSPropertyDrawable::UpdateDrawableVec(const RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec,
    std::unordered_set<RSPropertyDrawableSlot>& dirtySlots)
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
    if (dirtySlots.count(RSPropertyDrawableSlot::SAVE_LAYER_CONTENT)) {
        UpdateSaveLayerSlots(context, drawableVec);
    }

    // Temporary fix, change of clipToBounds should trigger UpdateSaveRestore
    if (!drawableSlotChanged && dirtySlots.count(RSPropertyDrawableSlot::CLIP_TO_BOUNDS)) {
        drawableSlotChanged = true;
    }

    return drawableSlotChanged;
}

namespace {
inline bool HasPropertyDrawableInRange(
    const RSPropertyDrawable::DrawableVec& drawableVec, RSPropertyDrawableSlot begin, RSPropertyDrawableSlot end)
{
    return std::any_of(drawableVec.begin() + begin, drawableVec.begin() + end,
        [](const auto& drawablePtr) { return drawablePtr != nullptr; });
}

inline uint8_t CalculateDrawableVecStatus(
    RSPropertyDrawableGenerateContext& context, RSPropertyDrawable::DrawableVec& drawableVec)
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
        drawableVec, RSPropertyDrawableSlot::BG_PROPERTIES_BEGIN, RSPropertyDrawableSlot::BG_PROPERTIES_END)) {
        result |= DrawableVecStatus::BG_BOUNDS_PROPERTY;
    }
    if (HasPropertyDrawableInRange(drawableVec, RSPropertyDrawableSlot::FG_PROPERTIES_BEGIN,
        RSPropertyDrawableSlot::FG_PROPERTIES_END)) {
        result |= DrawableVecStatus::FG_BOUNDS_PROPERTY;
    }
    if (HasPropertyDrawableInRange(drawableVec, RSPropertyDrawableSlot::CONTENT_PROPERTIES_BEGIN,
        RSPropertyDrawableSlot::CONTENT_PROPERTIES_END)) {
        result |= DrawableVecStatus::FRAME_PROPERTY;
    }

    return result;
}

constexpr std::array boundsSlotsToErase = {
    RSPropertyDrawableSlot::BG_SAVE_BOUNDS,
    RSPropertyDrawableSlot::CLIP_TO_BOUNDS,
    RSPropertyDrawableSlot::BG_RESTORE_BOUNDS,
    RSPropertyDrawableSlot::FG_SAVE_BOUNDS,
    RSPropertyDrawableSlot::FG_CLIP_TO_BOUNDS,
    RSPropertyDrawableSlot::FG_RESTORE_BOUNDS,
};

constexpr std::array frameSlotsToErase = {
    RSPropertyDrawableSlot::SAVE_FRAME,
    RSPropertyDrawableSlot::RESTORE_FRAME,
};

void OptimizeBoundsSaveRestore(
    RSPropertyDrawableGenerateContext& context, RSPropertyDrawable::DrawableVec& drawableVec, uint8_t flags)
{
    // Erase existing save/clip/restore before re-generating
    for (auto& slot : boundsSlotsToErase) {
        drawableVec[slot] = nullptr;
    }

    if (flags & DrawableVecStatus::CLIP_BOUNDS) {
        // case 1: ClipToBounds set.
        // add one clip, and reuse SAVE_ALL and RESTORE_ALL.
        drawableVec[RSPropertyDrawableSlot::CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);
        return;
    }

    if ((flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) && (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY)) {
        // case 2: ClipToBounds not set and we have bounds properties both BG and FG.
        // add two sets of save/clip/restore before & after content.

        // part 1: before children
        std::tie(drawableVec[RSPropertyDrawableSlot::BG_SAVE_BOUNDS],
            drawableVec[RSPropertyDrawableSlot::BG_RESTORE_BOUNDS]) = GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);
        drawableVec[RSPropertyDrawableSlot::CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);

        // part 2: after children, add aliases
        drawableVec[RSPropertyDrawableSlot::FG_SAVE_BOUNDS] = GenerateAlias(RSPropertyDrawableSlot::BG_SAVE_BOUNDS);
        drawableVec[RSPropertyDrawableSlot::FG_CLIP_TO_BOUNDS] = GenerateAlias(RSPropertyDrawableSlot::CLIP_TO_BOUNDS);
        drawableVec[RSPropertyDrawableSlot::FG_RESTORE_BOUNDS] =
            GenerateAlias(RSPropertyDrawableSlot::BG_RESTORE_BOUNDS);
        return;
    }

    if (flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) {
        // case 3: ClipToBounds not set and we have background bounds properties.
        std::tie(drawableVec[RSPropertyDrawableSlot::BG_SAVE_BOUNDS],
            drawableVec[RSPropertyDrawableSlot::BG_RESTORE_BOUNDS]) = GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);

        drawableVec[RSPropertyDrawableSlot::CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);
        return;
    }

    if (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY) {
        // case 4: ClipToBounds not set and we have foreground bounds properties.
        std::tie(drawableVec[RSPropertyDrawableSlot::FG_SAVE_BOUNDS],
            drawableVec[RSPropertyDrawableSlot::FG_RESTORE_BOUNDS]) = GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);

        drawableVec[RSPropertyDrawableSlot::FG_CLIP_TO_BOUNDS] = RSClipBoundsDrawable::Generate(context);
        return;
    }
    // case 5: ClipToBounds not set and no bounds properties, no need to save/clip/restore.
    // nothing to do
}

void OptimizeFrameSaveRestore(
    RSPropertyDrawableGenerateContext& context, RSPropertyDrawable::DrawableVec& drawableVec, uint8_t flags)
{
    // Erase existing save/clip/restore before re-generating
    for (auto& slot : frameSlotsToErase) {
        drawableVec[slot] = nullptr;
    }

    // PLANNING: if both clipToFrame and clipToBounds are set, and frame == bounds, we don't need an extra clip
    if (flags & DrawableVecStatus::FRAME_PROPERTY) {
        // save/restore
        std::tie(drawableVec[RSPropertyDrawableSlot::SAVE_FRAME], drawableVec[RSPropertyDrawableSlot::RESTORE_FRAME]) =
            GenerateSaveRestore(RSPaintFilterCanvas::kCanvas);
    } else {
        // no need to save/clip/restore
    }
}
} // namespace

void RSPropertyDrawable::InitializeSaveRestore(
    const RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec)
{
    std::tie(drawableVec[RSPropertyDrawableSlot::SAVE_ALL], drawableVec[RSPropertyDrawableSlot::RESTORE_ALL]) =
        GenerateSaveRestore(RSPaintFilterCanvas::kALL);
    drawableVec[RSPropertyDrawableSlot::BOUNDS_MATRIX] = RSBoundsGeometryDrawable::Generate(context);
    drawableVec[RSPropertyDrawableSlot::FRAME_OFFSET] = RSFrameGeometryDrawable::Generate(context);
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

RSPropertyDrawableGenerateContext::RSPropertyDrawableGenerateContext(RSRenderNode& node)
    : node_(node.shared_from_this()), properties_(node.GetRenderProperties()), hasChildren_(!node.GetChildren().empty())
{}

void ConvertBlendmodeToPaint(const RSPropertyDrawableGenerateContext& context, SkPaint& blendPaint)
{
    static const std::unordered_map<int, SkBlendMode> skBlendModeLUT = {
        { static_cast<int>(RSColorBlendModeType::DST_IN), SkBlendMode::kDstIn },
        { static_cast<int>(RSColorBlendModeType::SRC_IN), SkBlendMode::kSrcIn }
    };
    auto& properties = context.properties_;
    int blendMode = properties.GetColorBlendMode();
    auto iter = skBlendModeLUT.find(blendMode);
    if (iter == skBlendModeLUT.end()) {
        ROSEN_LOGE("The desired color_blend_mode is undefined, will behave as no blendmode.");
        return;
    }
    blendPaint.setBlendMode(skBlendModeLUT.at(blendMode));
}

void RSPropertyDrawable::UpdateSaveLayerSlots(
    const RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec)
{
    SkPaint blendPaint;
    ConvertBlendmodeToPaint(context, blendPaint);
    // blendmode value is invalid, clear relative 4 slots
    if (!blendPaint.asBlendMode().has_value()) {
        drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_CONTENT] = nullptr;
        drawableVec[RSPropertyDrawableSlot::RESTORE_CONTENT] = nullptr;
        drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_BACKGROUND] = nullptr;
        drawableVec[RSPropertyDrawableSlot::RESTORE_BACKGROUND] = nullptr;
        return;
    }
    // dirty slots COLOR_BLEND changed from none to valid value
    auto contentCount = std::make_shared<int>(-1);
    drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_CONTENT] =
        std::make_unique<RSSaveLayerContentDrawable>(contentCount, std::move(blendPaint));
    drawableVec[RSPropertyDrawableSlot::RESTORE_CONTENT] = std::make_unique<RSRestoreDrawable>(contentCount);
    auto backgroundCount = std::make_shared<int>(-1);
    drawableVec[RSPropertyDrawableSlot::SAVE_LAYER_BACKGROUND] =
        std::make_unique<RSSaveLayerBackgroundDrawable>(backgroundCount);
    drawableVec[RSPropertyDrawableSlot::RESTORE_BACKGROUND] = std::make_unique<RSRestoreDrawable>(backgroundCount);
}

} // namespace OHOS::Rosen
