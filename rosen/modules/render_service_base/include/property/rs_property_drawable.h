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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H

#ifdef USE_ROSEN_DRAWING
#include <bitset>
#endif
#include <memory>
#include <set>
#include <vector>

#include "modifier/rs_render_modifier.h"

namespace OHOS::Rosen {
class RSPaintFilterCanvas;
class RSProperties;
class RSRenderContent;
class RSRenderNode;

// NOTE: MUST update DrawableGeneratorLut in rs_property_drawable.cpp when new slots are added
enum class RSPropertyDrawableSlot : uint8_t {
    INVALID = 0,
    SAVE_ALL,

    // Bounds Geometry
    BOUNDS_MATRIX,
    ALPHA,
    MASK,
    TRANSITION,
    ENV_FOREGROUND_COLOR,
    SHADOW,
    OUTLINE,

    // BG properties in Bounds Clip
    BG_SAVE_BOUNDS,
    CLIP_TO_BOUNDS,
    BLEND_MODE,
    BACKGROUND_COLOR,
    BACKGROUND_SHADER,
    BACKGROUND_IMAGE,
    BACKGROUND_FILTER,
    USE_EFFECT,
    BACKGROUND_STYLE,
    DYNAMIC_LIGHT_UP,
    ENV_FOREGROUND_COLOR_STRATEGY,
    BG_RESTORE_BOUNDS,

    // Frame Geometry
    SAVE_FRAME,
    FRAME_OFFSET,
    CLIP_TO_FRAME,
    CONTENT_STYLE,
    CHILDREN,
    FOREGROUND_STYLE,
    RESTORE_FRAME,

    // FG properties in Bounds clip
    FG_SAVE_BOUNDS,
    FG_CLIP_TO_BOUNDS,
    BINARIZATION,
    COLOR_FILTER,
    LIGHT_UP_EFFECT,
    FOREGROUND_FILTER,
    FOREGROUND_COLOR,
    FG_RESTORE_BOUNDS,

    // No clip (unless ClipToBounds is set)
    POINT_LIGHT,
    BORDER,
    OVERLAY,
    PARTICLE_EFFECT,
    PIXEL_STRETCH,

    RESTORE_BLEND_MODE,
    RESTORE_ALL,

    // Annotations: Please remember to update this when new slots are added.
    // NOTE: MAX and *_END enums are using the one-past-the-end style.
    BG_PROPERTIES_BEGIN      = BACKGROUND_COLOR,
    BG_PROPERTIES_END        = ENV_FOREGROUND_COLOR_STRATEGY + 1,
    CONTENT_PROPERTIES_BEGIN = FRAME_OFFSET,
    CONTENT_PROPERTIES_END   = FOREGROUND_STYLE + 1,
    FG_PROPERTIES_BEGIN      = BINARIZATION,
    FG_PROPERTIES_END        = FOREGROUND_COLOR + 1,
    MAX                      = RESTORE_ALL + 1,
};

// Pure virtual base class
class RSPropertyDrawable {
public:
    RSPropertyDrawable() = default;
    virtual ~RSPropertyDrawable() = default;

    // not copyable and moveable
    RSPropertyDrawable(const RSPropertyDrawable&) = delete;
    RSPropertyDrawable(const RSPropertyDrawable&&) = delete;
    RSPropertyDrawable& operator=(const RSPropertyDrawable&) = delete;
    RSPropertyDrawable& operator=(const RSPropertyDrawable&&) = delete;

    virtual void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const = 0;
    // return true if this drawable can be updated, default is false
    virtual bool Update(const RSRenderContent& content) { return false; };

    // Aliases
    using DrawablePtr = std::unique_ptr<RSPropertyDrawable>;
    using DrawableVec = std::array<DrawablePtr, static_cast<size_t>(RSPropertyDrawableSlot::MAX)>;
    using DrawableGenerator = std::function<DrawablePtr(const RSRenderContent&)>;

    // Generator Utilities
    static void InitializeSaveRestore(const RSRenderContent& content, DrawableVec& drawableVec);
#ifndef USE_ROSEN_DRAWING
    static std::unordered_set<RSPropertyDrawableSlot> GenerateDirtySlots(
        const RSProperties& properties, const std::unordered_set<RSModifierType>& dirtyTypes);
#else
    static std::unordered_set<RSPropertyDrawableSlot> GenerateDirtySlots(
        const RSProperties& properties,
        std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)>& dirtyTypes);
#endif
    static bool UpdateDrawableVec(const RSRenderContent& content, DrawableVec& drawableVec,
        std::unordered_set<RSPropertyDrawableSlot>& dirtySlots);
    static void UpdateSaveRestore(
        RSRenderContent& content, DrawableVec& drawableVec, uint8_t& drawableVecStatus);
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H
