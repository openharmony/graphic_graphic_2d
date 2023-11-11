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

#include <list>
#include <map>
#include <unordered_set>
#include <utility>

#include "modifier/rs_render_modifier.h"

namespace OHOS::Rosen {
class RSPaintFilterCanvas;
class RSProperties;
class RSPropertyDrawableGenerateContext;
class RSRenderModifier;
class RSRenderNode;

namespace Slot {
// NOTE: MUST update DrawableGeneratorLut in rs_property_drawable.cpp when new slots are added
enum RSPropertyDrawableSlot : uint8_t {
    INVALID = 0,

    SAVE_ALL,

    // Bounds Geometry
    BOUNDS_MATRIX,
    ALPHA,
    MASK,
    TRANSITION,
    ENV_FOREGROUND_COLOR,
    SHADOW,

    // In Bounds Clip
    SAVE_LAYER_BACKGROUND,
    SAVE_BOUNDS,
    CLIP_TO_BOUNDS,
    BACKGROUND_COLOR,
    BACKGROUND_SHADER,
    BACKGROUND_IMAGE,
    BACKGROUND_FILTER,
    USE_EFFECT,
    BACKGROUND_STYLE,
    DYNAMIC_LIGHT_UP,
    ENV_FOREGROUND_COLOR_STRATEGY,
    EXTRA_RESTORE_BOUNDS,
    SAVE_LAYER_CONTENT,

    // Frame Geometry
    SAVE_FRAME,
    FRAME_OFFSET,
    CLIP_TO_FRAME,
    CONTENT_STYLE,
    CHILDREN,
    FOREGROUND_STYLE,
    COLOR_FILTER,
    RESTORE_FRAME,

    RESTORE_CONTENT,

    // In Bounds clip (again)
    EXTRA_SAVE_BOUNDS,
    EXTRA_CLIP_TO_BOUNDS,
    LIGHT_UP_EFFECT,
    FOREGROUND_FILTER,
    LINEAR_GRADIENT_BLUR_FILTER,
    BORDER,
    OVERLAY,
    FOREGROUND_COLOR,
    PARTICLE_EFFECT,
    RESTORE_BOUNDS,

    RESTORE_BACKGROUND,

    // Without clip
    PIXEL_STRETCH,
    RESTORE_ALL,
    MAX,
};

enum DrawableVecStatus : uint8_t {
    CLIP_BOUNDS            = 1 << 0,
    BOUNDS_PROPERTY_BEFORE = 1 << 1,
    BOUNDS_PROPERTY_AFTER  = 1 << 2,
    CLIP_FRAME             = 1 << 3,
    FRAME_PROPERTY         = 1 << 4,
    HAS_CHILDREN           = 1 << 5,
    BOUNDS_MASK            = CLIP_BOUNDS | BOUNDS_PROPERTY_BEFORE | BOUNDS_PROPERTY_AFTER,
    FRAME_MASK             = CLIP_FRAME | FRAME_PROPERTY | HAS_CHILDREN,
};
} // namespace Slot

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

    virtual void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) = 0;
    // return true if this drawable can be updated, default is false
    virtual bool Update(const RSPropertyDrawableGenerateContext& context) { return false; };

    // Aliases
    using DrawablePtr = std::unique_ptr<RSPropertyDrawable>;
    using DrawableVec = std::vector<DrawablePtr>;
    using DrawableGenerator = std::function<DrawablePtr(const RSPropertyDrawableGenerateContext&)>;

    // Generator Utilities
    static void InitializeSaveRestore(const RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec);
    static std::set<Slot::RSPropertyDrawableSlot> GenerateDirtySlots(
        const RSProperties& properties, const std::unordered_set<RSModifierType>& dirtyTypes);
    static bool UpdateDrawableVec(const RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec,
        std::set<Slot::RSPropertyDrawableSlot>& dirtySlots);
    static void UpdateSaveRestore(
        RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t& drawableVecStatus);

private:
    static inline uint8_t CalculateDrawableVecStatus(
        RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec);
    static void OptimizeBoundsSaveRestore(
        RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t flags);
    static void OptimizeFrameSaveRestore(
        RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t flags);
        static void UpdateSaveLayerSlots(const RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec);
};

class RSPropertyDrawableGenerateContext {
public:
    explicit RSPropertyDrawableGenerateContext(RSRenderNode& node);
    virtual ~RSPropertyDrawableGenerateContext() = default;

    // disable copy and move
    RSPropertyDrawableGenerateContext(const RSPropertyDrawableGenerateContext&) = delete;
    RSPropertyDrawableGenerateContext(const RSPropertyDrawableGenerateContext&&) = delete;
    RSPropertyDrawableGenerateContext& operator=(const RSPropertyDrawableGenerateContext&) = delete;
    RSPropertyDrawableGenerateContext& operator=(const RSPropertyDrawableGenerateContext&&) = delete;

    // member variable
    const std::shared_ptr<RSRenderNode> node_;
    const RSProperties& properties_;
    bool hasChildren_;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H
