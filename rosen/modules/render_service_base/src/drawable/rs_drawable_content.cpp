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

#include "drawable/rs_drawable_content.h"

#include "drawable/rs_property_drawable_content.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {
namespace {
// NOTE: This LUT should always the same size and order as RSModifierType
// key = RSModifierType, value = RSDrawableContentSlot
constexpr int DIRTY_LUT_SIZE = static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE);
static const std::array<RSDrawableContentSlot, DIRTY_LUT_SIZE> g_propertyToDrawableLut = {
    RSDrawableContentSlot::INVALID,                       // INVALID
    RSDrawableContentSlot::INVALID,                       // BOUNDS
    RSDrawableContentSlot::FRAME_OFFSET,                  // FRAME
    RSDrawableContentSlot::INVALID,                       // POSITION_Z
    RSDrawableContentSlot::INVALID,                       // PIVOT
    RSDrawableContentSlot::INVALID,                       // PIVOT_Z
    RSDrawableContentSlot::INVALID,                       // QUATERNION
    RSDrawableContentSlot::INVALID,                       // ROTATION
    RSDrawableContentSlot::INVALID,                       // ROTATION_X
    RSDrawableContentSlot::INVALID,                       // ROTATION_Y
    RSDrawableContentSlot::INVALID,                       // CAMERA_DISTANCE
    RSDrawableContentSlot::INVALID,                       // SCALE
    RSDrawableContentSlot::INVALID,                       // SKEW
    RSDrawableContentSlot::INVALID,                       // TRANSLATE
    RSDrawableContentSlot::INVALID,                       // TRANSLATE_Z
    RSDrawableContentSlot::INVALID,                       // SUBLAYER_TRANSFORM
    RSDrawableContentSlot::INVALID,                       // CORNER_RADIUS
    RSDrawableContentSlot::ALPHA,                         // ALPHA
    RSDrawableContentSlot::ALPHA,                         // ALPHA_OFFSCREEN
    RSDrawableContentSlot::FOREGROUND_COLOR,              // FOREGROUND_COLOR
    RSDrawableContentSlot::BACKGROUND_COLOR,              // BACKGROUND_COLOR
    RSDrawableContentSlot::BACKGROUND_SHADER,             // BACKGROUND_SHADER
    RSDrawableContentSlot::BACKGROUND_IMAGE,              // BG_IMAGE
    RSDrawableContentSlot::BACKGROUND_IMAGE,              // BG_IMAGE_WIDTH
    RSDrawableContentSlot::BACKGROUND_IMAGE,              // BG_IMAGE_HEIGHT
    RSDrawableContentSlot::BACKGROUND_IMAGE,              // BG_IMAGE_POSITION_X
    RSDrawableContentSlot::BACKGROUND_IMAGE,              // BG_IMAGE_POSITION_Y
    RSDrawableContentSlot::INVALID,                       // SURFACE_BG_COLOR
    RSDrawableContentSlot::BORDER,                        // BORDER_COLOR
    RSDrawableContentSlot::BORDER,                        // BORDER_WIDTH
    RSDrawableContentSlot::BORDER,                        // BORDER_STYLE
    RSDrawableContentSlot::FOREGROUND_FILTER,             // FILTER
    RSDrawableContentSlot::BACKGROUND_FILTER,             // BACKGROUND_FILTER
    RSDrawableContentSlot::FOREGROUND_FILTER,             // LINEAR_GRADIENT_BLUR_PARA
    RSDrawableContentSlot::DYNAMIC_LIGHT_UP,              // DYNAMIC_LIGHT_UP_RATE
    RSDrawableContentSlot::DYNAMIC_LIGHT_UP,              // DYNAMIC_LIGHT_UP_DEGREE
    RSDrawableContentSlot::FRAME_OFFSET,                  // FRAME_GRAVITY
    RSDrawableContentSlot::CLIP_TO_BOUNDS,                // CLIP_RRECT
    RSDrawableContentSlot::CLIP_TO_BOUNDS,                // CLIP_BOUNDS
    RSDrawableContentSlot::CLIP_TO_BOUNDS,                // CLIP_TO_BOUNDS
    RSDrawableContentSlot::CLIP_TO_FRAME,                 // CLIP_TO_FRAME
    RSDrawableContentSlot::INVALID,                       // VISIBLE
    RSDrawableContentSlot::SHADOW,                        // SHADOW_COLOR
    RSDrawableContentSlot::SHADOW,                        // SHADOW_OFFSET_X
    RSDrawableContentSlot::SHADOW,                        // SHADOW_OFFSET_Y
    RSDrawableContentSlot::SHADOW,                        // SHADOW_ALPHA
    RSDrawableContentSlot::SHADOW,                        // SHADOW_ELEVATION
    RSDrawableContentSlot::SHADOW,                        // SHADOW_RADIUS
    RSDrawableContentSlot::SHADOW,                        // SHADOW_PATH
    RSDrawableContentSlot::SHADOW,                        // SHADOW_MASK
    RSDrawableContentSlot::SHADOW,                        // SHADOW_COLOR_STRATEGY
    RSDrawableContentSlot::MASK,                          // MASK
    RSDrawableContentSlot::INVALID,                       // SPHERIZE
    RSDrawableContentSlot::LIGHT_UP_EFFECT,               // LIGHT_UP_EFFECT
    RSDrawableContentSlot::PIXEL_STRETCH,                 // PIXEL_STRETCH
    RSDrawableContentSlot::PIXEL_STRETCH,                 // PIXEL_STRETCH_PERCENT
    RSDrawableContentSlot::USE_EFFECT,                    // USE_EFFECT
    RSDrawableContentSlot::BLEND_MODE,                    // COLOR_BLEND_MODE
    RSDrawableContentSlot::BLEND_MODE,                    // COLOR_BLEND_APPLY_TYPE
    RSDrawableContentSlot::INVALID,                       // SANDBOX
    RSDrawableContentSlot::COLOR_FILTER,                  // GRAY_SCALE
    RSDrawableContentSlot::COLOR_FILTER,                  // BRIGHTNESS
    RSDrawableContentSlot::COLOR_FILTER,                  // CONTRAST
    RSDrawableContentSlot::COLOR_FILTER,                  // SATURATE
    RSDrawableContentSlot::COLOR_FILTER,                  // SEPIA
    RSDrawableContentSlot::COLOR_FILTER,                  // INVERT
    RSDrawableContentSlot::BINARIZATION,                  // AIINVERT
    RSDrawableContentSlot::BACKGROUND_FILTER,             // SYSTEMBAREFFECT
    RSDrawableContentSlot::COLOR_FILTER,                  // HUE_ROTATE
    RSDrawableContentSlot::COLOR_FILTER,                  // COLOR_BLEND
    RSDrawableContentSlot::PARTICLE_EFFECT,               // PARTICLE
    RSDrawableContentSlot::INVALID,                       // SHADOW_IS_FILLED
    RSDrawableContentSlot::OUTLINE,                       // OUTLINE_COLOR
    RSDrawableContentSlot::OUTLINE,                       // OUTLINE_WIDTH
    RSDrawableContentSlot::OUTLINE,                       // OUTLINE_STYLE
    RSDrawableContentSlot::OUTLINE,                       // OUTLINE_RADIUS
    RSDrawableContentSlot::INVALID,                       // USE_SHADOW_BATCHING
    RSDrawableContentSlot::INVALID,                       // GREY_COEF1
    RSDrawableContentSlot::INVALID,                       // GREY_COEF2
    RSDrawableContentSlot::POINT_LIGHT,                   // LIGHT_INTENSITY
    RSDrawableContentSlot::POINT_LIGHT,                   // LIGHT_POSITION
    RSDrawableContentSlot::POINT_LIGHT,                   // ILLUMINATED_BORDER_WIDTH
    RSDrawableContentSlot::POINT_LIGHT,                   // ILLUMINATED_TYPE
    RSDrawableContentSlot::POINT_LIGHT,                   // BLOOM
    RSDrawableContentSlot::INVALID,                       // CUSTOM
    RSDrawableContentSlot::INVALID,                       // EXTENDED
    RSDrawableContentSlot::TRANSITION,                    // TRANSITION
    RSDrawableContentSlot::BACKGROUND_STYLE,              // BACKGROUND_STYLE
    RSDrawableContentSlot::CONTENT_STYLE,                 // CONTENT_STYLE
    RSDrawableContentSlot::FOREGROUND_STYLE,              // FOREGROUND_STYLE
    RSDrawableContentSlot::OVERLAY,                       // OVERLAY_STYLE
    RSDrawableContentSlot::INVALID,                       // NODE_MODIFIER
    RSDrawableContentSlot::ENV_FOREGROUND_COLOR,          // ENV_FOREGROUND_COLOR
    RSDrawableContentSlot::ENV_FOREGROUND_COLOR_STRATEGY, // ENV_FOREGROUND_COLOR_STRATEGY
    RSDrawableContentSlot::INVALID,                       // GEOMETRYTRANS
};

template<RSModifierType type>
static inline RSDrawableContent::Ptr ModifierGenerator(const RSRenderNode& node)
{
    return RSCustomModifierDrawableContent::OnGenerate(node, type);
}

// NOTE: This LUT should always the same size as RSDrawableContentSlot
// index = RSDrawableContentSlotType, value = DrawableGenerator
constexpr int GEN_LUT_SIZE = static_cast<int>(RSDrawableContentSlot::MAX);
static const std::array<RSDrawableContent::Generator, GEN_LUT_SIZE> g_drawableGeneratorLut = {
    nullptr, // INVALID = 0,

    // Bounds Geometry
    nullptr,                                                 // BOUNDS_MATRIX,
    nullptr,                                                 // ALPHA,
    nullptr,                                                 // MASK,
    ModifierGenerator<RSModifierType::TRANSITION>,           // TRANSITION,
    ModifierGenerator<RSModifierType::ENV_FOREGROUND_COLOR>, // ENV_FOREGROUND_COLOR,
    nullptr,                                                 // SHADOW,
    nullptr,                                                 // OUTLINE,

    // BG properties in Bounds Clip
    nullptr,                                                          // BG_SAVE_BOUNDS,
    nullptr,                                                          // CLIP_TO_BOUNDS,
    nullptr,                                                          // BLEND_MODE,
    RSBackgroundColorContent::OnGenerate,                             // BACKGROUND_COLOR,
    RSBackgroundShaderContent::OnGenerate,                            // BACKGROUND_SHADER,
    RSBackgroundImageContent::OnGenerate,                             // BACKGROUND_IMAGE,
    RSBackgroundFilterContent::OnGenerate,                            // BACKGROUND_FILTER,
    nullptr,                                                          // USE_EFFECT,
    ModifierGenerator<RSModifierType::BACKGROUND_STYLE>,              // BACKGROUND_STYLE,
    RSDynamicLightUpContent::OnGenerate,                              // DYNAMIC_LIGHT_UP,
    ModifierGenerator<RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY>, // ENV_FOREGROUND_COLOR_STRATEGY,
    nullptr,                                                          // BG_RESTORE_BOUNDS,

    // Frame Geometry
    nullptr,                                             // SAVE_FRAME,
    nullptr,                                             // FRAME_OFFSET,
    nullptr,                                             // CLIP_TO_FRAME,
    ModifierGenerator<RSModifierType::CONTENT_STYLE>,    // CONTENT_STYLE,
    RSChildrenDrawableContent::OnGenerate,               // CHILDREN,
    ModifierGenerator<RSModifierType::FOREGROUND_STYLE>, // FOREGROUND_STYLE,
    nullptr,                                             // RESTORE_FRAME,

    // FG properties in Bounds clip
    nullptr,                                 // FG_SAVE_BOUNDS,
    nullptr,                                 // FG_CLIP_TO_BOUNDS,
    RSBinarizationShaderContent::OnGenerate, // BINARIZATION,
    RSColorFilterContent::OnGenerate,        // COLOR_FILTER,
    RSLightUpEffectContent::OnGenerate,      // LIGHT_UP_EFFECT,
    nullptr,                                 // FOREGROUND_FILTER,
    RSForegroundColorContent::OnGenerate,    // FOREGROUND_COLOR,
    nullptr,                                 // FG_RESTORE_BOUNDS,

    // No clip (unless ClipToBounds is set)
    nullptr,                                          // POINT_LIGHT,
    RSBorderContent::OnGenerate,                      // BORDER,
    ModifierGenerator<RSModifierType::OVERLAY_STYLE>, // OVERLAY,
    RSParticleContent::OnGenerate,                    // PARTICLE_EFFECT,
    RSPixelStretchContent::OnGenerate,                // PIXEL_STRETCH,

    nullptr, // RESTORE_BLEND_MODE,
};

inline std::pair<RSDrawableContent::Ptr, RSDrawableContent::Ptr> GenerateSaveRestore(
    RSPaintFilterCanvas::SaveType type = RSPaintFilterCanvas::kCanvas)
{
    // if (type == RSPaintFilterCanvas::kNone) {
    //     return {};
    // } else if (type == RSPaintFilterCanvas::kCanvas) {
    //     auto count = std::make_shared<int>(-1);
    //     return { std::make_unique<RSSaveDrawable>(count), std::make_unique<RSRestoreDrawable>(count) };
    // } else {
    //     auto status = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
    //     return { std::make_unique<RSCustomSaveDrawable>(status, type),
    //         std::make_unique<RSCustomRestoreDrawable>(status) };
    // }
    return {};
}

inline void SaveRestoreHelper(RSDrawableContent::Vec& drawableVec, RSDrawableContentSlot slot1,
    RSDrawableContentSlot slot2, RSPaintFilterCanvas::SaveType saveType)
{
    std::tie(drawableVec[static_cast<size_t>(slot1)], drawableVec[static_cast<size_t>(slot2)]) =
        GenerateSaveRestore(saveType);
}
} // namespace

// ==================== RSDrawableContent =====================
std::unordered_set<RSDrawableContentSlot> RSDrawableContent::CalculateDirtySlots(
    ModifierDirtyTypes& dirtyTypes, const Vec& drawableVec)
{
    if (dirtyTypes.none()) {
        return {};
    }

    std::unordered_set<RSDrawableContentSlot> dirtySlots;
    for (size_t type = 0; type < static_cast<size_t>(RSModifierType::MAX_RS_MODIFIER_TYPE); type++) {
        if (!dirtyTypes.test(type)) {
            continue;
        }
        auto dirtySlot = g_propertyToDrawableLut[type];
        if (dirtySlot != RSDrawableContentSlot::INVALID) {
            dirtySlots.emplace(dirtySlot);
        }
    }

    // if bounds changed, every existing drawable needs to be updated
    if (dirtyTypes.test(static_cast<size_t>(RSModifierType::BOUNDS))) {
        for (size_t i = 0; i < drawableVec.size(); i++) {
            if (drawableVec[i]) {
                dirtySlots.emplace(static_cast<RSDrawableContentSlot>(i));
            }
        }
    }

    return dirtySlots;
}

bool RSDrawableContent::UpdateDirtySlots(
    const RSRenderNode& node, Vec& drawableVec, std::unordered_set<RSDrawableContentSlot>& dirtySlots)
{
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

    return drawableAddedOrRemoved;
}

namespace {
enum DrawableVecStatus : uint8_t {
    CLIP_TO_BOUNDS          = 1 << 0,
    BG_BOUNDS_PROPERTY      = 1 << 1,
    FG_BOUNDS_PROPERTY      = 1 << 2,
    CLIP_TO_FRAME           = 1 << 3,
    FRAME_PROPERTY          = 1 << 4,
    HAS_CHILDREN            = 1 << 5,
    BOUNDS_MASK             = CLIP_TO_BOUNDS | BG_BOUNDS_PROPERTY | FG_BOUNDS_PROPERTY,
    FRAME_MASK              = CLIP_TO_FRAME | FRAME_PROPERTY | HAS_CHILDREN,
};

inline bool HasPropertyDrawableInRange(
    const RSDrawableContent::Vec& drawableVec, RSDrawableContentSlot begin, RSDrawableContentSlot end)
{
    return std::any_of(drawableVec.begin() + static_cast<size_t>(begin), drawableVec.begin() + static_cast<size_t>(end),
        [](const auto& Ptr) { return Ptr != nullptr; });
}

uint8_t CalculateDrawableVecStatus(RSRenderContent& content, const RSDrawableContent::Vec& drawableVec)
{
    uint8_t result = 0;
    auto& properties = content.GetRenderProperties();

    // color blend mode has implicit dependency on clipToBounds
    if (properties.GetClipToBounds() || properties.GetClipToRRect() || properties.GetClipBounds() != nullptr ||
        properties.GetColorBlendMode()) {
        result |= DrawableVecStatus::CLIP_TO_BOUNDS;
    }
    if (properties.GetClipToFrame()) {
        result |= DrawableVecStatus::CLIP_TO_FRAME;
    }

    if (HasPropertyDrawableInRange(
        drawableVec, RSDrawableContentSlot::BG_PROPERTIES_BEGIN, RSDrawableContentSlot::BG_PROPERTIES_END)) {
        result |= DrawableVecStatus::BG_BOUNDS_PROPERTY;
    }
    if (HasPropertyDrawableInRange(
        drawableVec, RSDrawableContentSlot::FG_PROPERTIES_BEGIN, RSDrawableContentSlot::FG_PROPERTIES_END)) {
        result |= DrawableVecStatus::FG_BOUNDS_PROPERTY;
    }
    if (HasPropertyDrawableInRange(drawableVec, RSDrawableContentSlot::CONTENT_PROPERTIES_BEGIN,
        RSDrawableContentSlot::CONTENT_PROPERTIES_END)) {
        result |= DrawableVecStatus::FRAME_PROPERTY;
    }

    return result;
}

constexpr std::array boundsSlotsToErase = {
    RSDrawableContentSlot::BG_SAVE_BOUNDS,
    RSDrawableContentSlot::CLIP_TO_BOUNDS,
    RSDrawableContentSlot::BG_RESTORE_BOUNDS,
    RSDrawableContentSlot::FG_SAVE_BOUNDS,
    RSDrawableContentSlot::FG_CLIP_TO_BOUNDS,
    RSDrawableContentSlot::FG_RESTORE_BOUNDS,
};

constexpr std::array frameSlotsToErase = {
    RSDrawableContentSlot::SAVE_FRAME,
    RSDrawableContentSlot::RESTORE_FRAME,
};

void OptimizeBoundsSaveRestore(RSRenderContent& content, RSDrawableContent::Vec& drawableVec, uint8_t flags)
{
    // Erase existing save/clip/restore before re-generating
    for (auto& slot : boundsSlotsToErase) {
        drawableVec[static_cast<size_t>(slot)] = nullptr;
    }

    // if (flags & DrawableVecStatus::CLIP_TO_BOUNDS) {
    //     // case 1: ClipToBounds set.
    //     // add one clip, and reuse SAVE_ALL and RESTORE_ALL.
    //     drawableVec[static_cast<size_t>(RSDrawableContentSlot::CLIP_TO_BOUNDS)] =
    //         RSClipBoundsDrawable::Generate(content);
    //     return;
    // }

    // if ((flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) && (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY)) {
    //     // case 2: ClipToBounds not set and we have bounds properties both BG and FG.
    //     // add two sets of save/clip/restore before & after content.

    //     // part 1: before children
    //     SaveRestoreHelper(drawableVec, RSDrawableContentSlot::BG_SAVE_BOUNDS,
    //         RSDrawableContentSlot::BG_RESTORE_BOUNDS, RSPaintFilterCanvas::kCanvas);
    //     drawableVec[static_cast<size_t>(RSDrawableContentSlot::CLIP_TO_BOUNDS)] =
    //         RSClipBoundsDrawable::Generate(content);

    //     // part 2: after children, add aliases
    //     drawableVec[static_cast<size_t>(RSDrawableContentSlot::FG_SAVE_BOUNDS)] =
    //         GenerateAlias(RSDrawableContentSlot::BG_SAVE_BOUNDS);
    //     drawableVec[static_cast<size_t>(RSDrawableContentSlot::FG_CLIP_TO_BOUNDS)] =
    //         GenerateAlias(RSDrawableContentSlot::CLIP_TO_BOUNDS);
    //     drawableVec[static_cast<size_t>(RSDrawableContentSlot::FG_RESTORE_BOUNDS)] =
    //         GenerateAlias(RSDrawableContentSlot::BG_RESTORE_BOUNDS);
    //     return;
    // }

    // if (flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) {
    //     // case 3: ClipToBounds not set and we have background bounds properties.
    //     SaveRestoreHelper(drawableVec, RSDrawableContentSlot::BG_SAVE_BOUNDS,
    //         RSDrawableContentSlot::BG_RESTORE_BOUNDS, RSPaintFilterCanvas::kCanvas);

    //     drawableVec[static_cast<size_t>(RSDrawableContentSlot::CLIP_TO_BOUNDS)] =
    //         RSClipBoundsDrawable::Generate(content);
    //     return;
    // }

    // if (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY) {
    //     // case 4: ClipToBounds not set and we have foreground bounds properties.
    //     SaveRestoreHelper(drawableVec, RSDrawableContentSlot::FG_SAVE_BOUNDS,
    //         RSDrawableContentSlot::FG_RESTORE_BOUNDS, RSPaintFilterCanvas::kCanvas);

    //     drawableVec[static_cast<size_t>(RSDrawableContentSlot::FG_CLIP_TO_BOUNDS)] =
    //         RSClipBoundsDrawable::Generate(content);
    //     return;
    // }
    // case 5: ClipToBounds not set and no bounds properties, no need to save/clip/restore.
    // nothing to do
}

void OptimizeFrameSaveRestore(RSRenderContent& content, RSDrawableContent::Vec& drawableVec, uint8_t flags)
{
    // Erase existing save/clip/restore before re-generating
    for (auto& slot : frameSlotsToErase) {
        drawableVec[static_cast<size_t>(slot)] = nullptr;
    }

    // PLANNING: if both clipToFrame and clipToBounds are set, and frame == bounds, we don't need an extra clip
    if (flags & DrawableVecStatus::FRAME_PROPERTY) {
        // save/restore
        SaveRestoreHelper(drawableVec, RSDrawableContentSlot::SAVE_FRAME,
            RSDrawableContentSlot::RESTORE_FRAME, RSPaintFilterCanvas::kCanvas);
    } else {
        // no need to save/clip/restore
    }
}
} // namespace

void RSDrawableContent::UpdateSaveRestore(RSRenderContent& content, Vec& drawableVec, uint8_t& drawableVecStatus)
{
    // ====================================================================
    // Step 3: Universal save/clip/restore optimization

    // calculate new drawable map status
    auto drawableVecStatusNew = CalculateDrawableVecStatus(content, drawableVec);

    // calculate changed bits
    uint8_t changedBits = drawableVecStatus ^ drawableVecStatusNew;
    if (changedBits & BOUNDS_MASK) {
        // update bounds save/clip if need
        OptimizeBoundsSaveRestore(content, drawableVec, drawableVecStatusNew);
    }
    if (changedBits & FRAME_MASK) {
        // update frame save/clip if need
        OptimizeFrameSaveRestore(content, drawableVec, drawableVecStatusNew);
    }
    drawableVecStatus = drawableVecStatusNew;
}

// ==================== RSChildrenDrawableContent =====================
RSDrawableContent::Ptr RSChildrenDrawableContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSChildrenDrawableContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
}

bool RSChildrenDrawableContent::OnUpdate(const RSRenderNode& node)
{
    auto children = node.GetSortedChildren();
    if (children == nullptr || children->empty()) {
        return false;
    }

    // Regenerate children drawables
    needSync_ = true;
    stagingChildrenDrawables_.clear();
    for (const auto& child : *children) {
        stagingChildrenDrawables_.push_back(RSRenderNodeDrawableAdapter::OnGenerate(child));
    }
    return true;
}

void RSChildrenDrawableContent::OnSync()
{
    if (!needSync_) {
        return;
    }
    childrenDrawables_ = std::move(stagingChildrenDrawables_);
    needSync_ = false;
}

// RSDrawable::Ptr RSChildrenDrawableContent::CreateDrawable() const
Drawing::RecordingCanvas::DrawFunc RSChildrenDrawableContent::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSChildrenDrawableContent>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        for (const auto& drawable : ptr->childrenDrawables_) {
            drawable->OnDraw(canvas);
        }
    };
}

// ==================== RSCustomModifierDrawableContent ===================
RSDrawableContent::Ptr RSCustomModifierDrawableContent::OnGenerate(const RSRenderNode& node, RSModifierType type)
{
    if (auto ret = std::make_shared<RSCustomModifierDrawableContent>(type); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
}

bool RSCustomModifierDrawableContent::OnUpdate(const RSRenderNode& node)
{
    const auto& drawCmdModifiers = node.GetDrawCmdModifiers();
    auto itr = drawCmdModifiers.find(type_);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    needSync_ = true;
    stagingDrawCmdList_.clear();
    for (const auto& modifier : itr->second) {
        auto property = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(modifier->GetProperty());
        if (property == nullptr) {
            continue;
        }
        if (const auto& drawCmdList = property->GetRef()) {
            stagingDrawCmdList_.push_back(drawCmdList);
        }
    }
    return true;
}

void RSCustomModifierDrawableContent::OnSync()
{
    if (!needSync_) {
        return;
    }
    drawCmdList_ = std::move(stagingDrawCmdList_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSCustomModifierDrawableContent::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSCustomModifierDrawableContent>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        for (const auto& drawCmdList : ptr->drawCmdList_) {
            drawCmdList->Playback(*canvas);
        }
    };
}
} // namespace OHOS::Rosen
