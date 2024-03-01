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
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // BOUNDS
    RSDrawableContentSlot::FRAME_OFFSET,                  // FRAME
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // POSITION_Z
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // PIVOT
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // PIVOT_Z
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // QUATERNION
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // ROTATION
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // ROTATION_X
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // ROTATION_Y
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // CAMERA_DISTANCE
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // SCALE
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // SKEW
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // TRANSLATE
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // TRANSLATE_Z
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
    RSDrawableContentSlot::BOUNDS_MATRIX,                 // SANDBOX
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
// index = RSPropertyDrawableType, value = DrawableGenerator
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
    nullptr,                                                          // BACKGROUND_FILTER,
    nullptr,                                                          // USE_EFFECT,
    ModifierGenerator<RSModifierType::BACKGROUND_STYLE>,              // BACKGROUND_STYLE,
    nullptr,                                                          // DYNAMIC_LIGHT_UP,
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
    nullptr, // FG_SAVE_BOUNDS,
    nullptr, // FG_CLIP_TO_BOUNDS,
    nullptr, // BINARIZATION,
    nullptr, // COLOR_FILTER,
    nullptr, // LIGHT_UP_EFFECT,
    nullptr, // FOREGROUND_FILTER,
    nullptr, // FOREGROUND_COLOR,
    nullptr, // FG_RESTORE_BOUNDS,

    // No clip (unless ClipToBounds is set)
    nullptr,                                          // POINT_LIGHT,
    RSBorderContent::OnGenerate,                      // BORDER,
    ModifierGenerator<RSModifierType::OVERLAY_STYLE>, // OVERLAY,
    nullptr,                                          // PARTICLE_EFFECT,
    nullptr,                                          // PIXEL_STRETCH,

    nullptr, // RESTORE_BLEND_MODE,
};
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

void RSDrawableContent::UpdateSaveRestore(RSRenderContent& content, Vec& drawableVec, uint8_t& drawableVecStatus)
{
    // TODO: fill necessary save/restore into slots
    (void)content;
    (void)drawableVec;
    (void)drawableVecStatus;
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
    needSync_ = true;
    stagingChildrenDrawables_.clear();

    auto children = node.GetSortedChildren();
    if (children == nullptr || children->empty()) {
        return false;
    }
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

RSDrawable::Ptr RSChildrenDrawableContent::CreateDrawable() const
{
    auto ptr = std::static_pointer_cast<const RSChildrenDrawableContent>(shared_from_this());
    return std::make_shared<RSChildrenDrawable>(ptr);
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
    stagingDrawCmdList_.clear();
    needSync_ = true;

    const auto& drawCmdModifiers = node.GetDrawCmdModifiers();
    auto itr = drawCmdModifiers.find(type_);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }
    // regenerate stagingDrawCmdList_
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

RSDrawable::Ptr RSCustomModifierDrawableContent::CreateDrawable() const
{
    auto ptr = std::static_pointer_cast<const RSCustomModifierDrawableContent>(shared_from_this());
    return std::make_shared<RSCustomModifierDrawable>(ptr);
}
} // namespace OHOS::Rosen
