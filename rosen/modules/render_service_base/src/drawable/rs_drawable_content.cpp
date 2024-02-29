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

#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {

namespace {
// key = RSModifierType, value = RSDrawableContentSlot
static const std::unordered_map<RSModifierType, RSDrawableContentSlot> g_propertyToDrawableLut = {
    { RSModifierType::INVALID, RSDrawableContentSlot::INVALID },
    { RSModifierType::BOUNDS, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::FRAME, RSDrawableContentSlot::FRAME_OFFSET },
    { RSModifierType::POSITION_Z, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::PIVOT, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::PIVOT_Z, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::QUATERNION, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::ROTATION, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::ROTATION_X, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::ROTATION_Y, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::CAMERA_DISTANCE, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::SCALE, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::SKEW, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::TRANSLATE, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::TRANSLATE_Z, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::SUBLAYER_TRANSFORM, RSDrawableContentSlot::INVALID },
    { RSModifierType::CORNER_RADIUS, RSDrawableContentSlot::INVALID },
    { RSModifierType::ALPHA, RSDrawableContentSlot::ALPHA },
    { RSModifierType::ALPHA_OFFSCREEN, RSDrawableContentSlot::ALPHA },
    { RSModifierType::FOREGROUND_COLOR, RSDrawableContentSlot::FOREGROUND_COLOR },
    { RSModifierType::BACKGROUND_COLOR, RSDrawableContentSlot::BACKGROUND_COLOR },
    { RSModifierType::BACKGROUND_SHADER, RSDrawableContentSlot::BACKGROUND_SHADER },
    { RSModifierType::BG_IMAGE, RSDrawableContentSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_WIDTH, RSDrawableContentSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_HEIGHT, RSDrawableContentSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_POSITION_X, RSDrawableContentSlot::BACKGROUND_IMAGE },
    { RSModifierType::BG_IMAGE_POSITION_Y, RSDrawableContentSlot::BACKGROUND_IMAGE },
    { RSModifierType::SURFACE_BG_COLOR, RSDrawableContentSlot::INVALID },
    { RSModifierType::BORDER_COLOR, RSDrawableContentSlot::BORDER },
    { RSModifierType::BORDER_WIDTH, RSDrawableContentSlot::BORDER },
    { RSModifierType::BORDER_STYLE, RSDrawableContentSlot::BORDER },
    { RSModifierType::FILTER, RSDrawableContentSlot::FOREGROUND_FILTER },
    { RSModifierType::BACKGROUND_FILTER, RSDrawableContentSlot::BACKGROUND_FILTER },
    { RSModifierType::LINEAR_GRADIENT_BLUR_PARA, RSDrawableContentSlot::FOREGROUND_FILTER },
    { RSModifierType::DYNAMIC_LIGHT_UP_RATE, RSDrawableContentSlot::DYNAMIC_LIGHT_UP },
    { RSModifierType::DYNAMIC_LIGHT_UP_DEGREE, RSDrawableContentSlot::DYNAMIC_LIGHT_UP },
    { RSModifierType::FRAME_GRAVITY, RSDrawableContentSlot::FRAME_OFFSET },
    { RSModifierType::CLIP_RRECT, RSDrawableContentSlot::CLIP_TO_BOUNDS },
    { RSModifierType::CLIP_BOUNDS, RSDrawableContentSlot::CLIP_TO_BOUNDS },
    { RSModifierType::CLIP_TO_BOUNDS, RSDrawableContentSlot::CLIP_TO_BOUNDS },
    { RSModifierType::CLIP_TO_FRAME, RSDrawableContentSlot::CLIP_TO_FRAME },
    { RSModifierType::VISIBLE, RSDrawableContentSlot::INVALID },
    { RSModifierType::SHADOW_COLOR, RSDrawableContentSlot::SHADOW },
    { RSModifierType::SHADOW_OFFSET_X, RSDrawableContentSlot::SHADOW },
    { RSModifierType::SHADOW_OFFSET_Y, RSDrawableContentSlot::SHADOW },
    { RSModifierType::SHADOW_ALPHA, RSDrawableContentSlot::SHADOW },
    { RSModifierType::SHADOW_ELEVATION, RSDrawableContentSlot::SHADOW },
    { RSModifierType::SHADOW_RADIUS, RSDrawableContentSlot::SHADOW },
    { RSModifierType::SHADOW_PATH, RSDrawableContentSlot::SHADOW },
    { RSModifierType::SHADOW_MASK, RSDrawableContentSlot::SHADOW },
    { RSModifierType::MASK, RSDrawableContentSlot::MASK },
    { RSModifierType::SPHERIZE, RSDrawableContentSlot::INVALID },
    { RSModifierType::LIGHT_UP_EFFECT, RSDrawableContentSlot::LIGHT_UP_EFFECT },
    { RSModifierType::AIINVERT, RSDrawableContentSlot::BINARIZATION },
    { RSModifierType::SYSTEMBAREFFECT, RSDrawableContentSlot::BACKGROUND_FILTER },
    { RSModifierType::PIXEL_STRETCH, RSDrawableContentSlot::PIXEL_STRETCH },
    { RSModifierType::PIXEL_STRETCH_PERCENT, RSDrawableContentSlot::PIXEL_STRETCH },
    { RSModifierType::USE_EFFECT, RSDrawableContentSlot::USE_EFFECT },
    { RSModifierType::SANDBOX, RSDrawableContentSlot::BOUNDS_MATRIX },
    { RSModifierType::GRAY_SCALE, RSDrawableContentSlot::COLOR_FILTER },
    { RSModifierType::BRIGHTNESS, RSDrawableContentSlot::COLOR_FILTER },
    { RSModifierType::CONTRAST, RSDrawableContentSlot::COLOR_FILTER },
    { RSModifierType::SATURATE, RSDrawableContentSlot::COLOR_FILTER },
    { RSModifierType::SEPIA, RSDrawableContentSlot::COLOR_FILTER },
    { RSModifierType::INVERT, RSDrawableContentSlot::COLOR_FILTER },
    { RSModifierType::HUE_ROTATE, RSDrawableContentSlot::COLOR_FILTER },
    { RSModifierType::COLOR_BLEND, RSDrawableContentSlot::COLOR_FILTER },
    { RSModifierType::PARTICLE, RSDrawableContentSlot::PARTICLE_EFFECT },
    { RSModifierType::SHADOW_IS_FILLED, RSDrawableContentSlot::INVALID },
    { RSModifierType::COLOR_BLEND_MODE, RSDrawableContentSlot::BLEND_MODE },
    { RSModifierType::COLOR_BLEND_APPLY_TYPE, RSDrawableContentSlot::BLEND_MODE },
    { RSModifierType::OUTLINE_COLOR, RSDrawableContentSlot::OUTLINE },
    { RSModifierType::OUTLINE_WIDTH, RSDrawableContentSlot::OUTLINE },
    { RSModifierType::OUTLINE_STYLE, RSDrawableContentSlot::OUTLINE },
    { RSModifierType::OUTLINE_RADIUS, RSDrawableContentSlot::OUTLINE },
    { RSModifierType::USE_SHADOW_BATCHING, RSDrawableContentSlot::INVALID },
    { RSModifierType::LIGHT_INTENSITY, RSDrawableContentSlot::POINT_LIGHT },
    { RSModifierType::LIGHT_POSITION, RSDrawableContentSlot::POINT_LIGHT },
    { RSModifierType::ILLUMINATED_TYPE, RSDrawableContentSlot::POINT_LIGHT },
    { RSModifierType::BLOOM, RSDrawableContentSlot::POINT_LIGHT },
    { RSModifierType::CUSTOM, RSDrawableContentSlot::INVALID },
    { RSModifierType::EXTENDED, RSDrawableContentSlot::INVALID },
    { RSModifierType::TRANSITION, RSDrawableContentSlot::TRANSITION },
    { RSModifierType::BACKGROUND_STYLE, RSDrawableContentSlot::BACKGROUND_STYLE },
    { RSModifierType::CONTENT_STYLE, RSDrawableContentSlot::CONTENT_STYLE },
    { RSModifierType::FOREGROUND_STYLE, RSDrawableContentSlot::FOREGROUND_STYLE },
    { RSModifierType::OVERLAY_STYLE, RSDrawableContentSlot::OVERLAY },
    { RSModifierType::NODE_MODIFIER, RSDrawableContentSlot::INVALID },
    { RSModifierType::ENV_FOREGROUND_COLOR, RSDrawableContentSlot::ENV_FOREGROUND_COLOR },
    { RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, RSDrawableContentSlot::ENV_FOREGROUND_COLOR_STRATEGY },
    { RSModifierType::GEOMETRYTRANS, RSDrawableContentSlot::INVALID },
};

// NOTE: This LUT should always the same size as RSDrawableContentSlot
// index = RSPropertyDrawableType, value = DrawableGenerator
constexpr int LUT_SIZE = static_cast<int>(RSDrawableContentSlot::MAX);
static const std::array<RSDrawableContent::Generator, LUT_SIZE> g_drawableGeneratorLut = { nullptr };
} // namespace

// ==================== RSDrawableContent =====================
std::unordered_set<RSDrawableContentSlot> RSDrawableContent::GenerateDirtySlots(
    ModifierDirtyTypes& dirtyTypes, const Vec& drawableVec)
{
    if (dirtyTypes.none()) {
        return {};
    }

    std::unordered_set<RSDrawableContentSlot> dirtySlots;
    for (size_t type = 0; type < static_cast<size_t>(RSModifierType::MAX_RS_MODIFIER_TYPE); type++) {
        if (dirtyTypes[type]) {
            auto it = g_propertyToDrawableLut.find(static_cast<RSModifierType>(type));
            if (it == g_propertyToDrawableLut.end() || it->second == RSDrawableContentSlot::INVALID) {
                continue;
            }
            dirtySlots.emplace(it->second);
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

bool RSDrawableContent::UpdateDrawableVec(
    const RSRenderNode& node, Vec& drawableVec, std::unordered_set<RSDrawableContentSlot>& dirtySlots)
{
    bool drawableAddedOrRemoved = false;
    for (const auto& slot : dirtySlots) {
        if (auto& drawable = drawableVec[static_cast<size_t>(slot)]) {
            // If the slot is already created, call OnUpdate
            if (drawable->OnUpdate(node)) {
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

void RSDrawableContent::UpdateSaveRestore(RSRenderContent& content, Vec& drawableVec, uint8_t& drawableVecStatus) {}

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
