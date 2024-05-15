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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_H

#include <bitset>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_set>

#include "recording/recording_canvas.h"

#include "modifier/rs_modifier_type.h"

namespace OHOS::Rosen {
class RSRenderNode;
class RSRenderContent;

// NOTE: MUST update DrawableGeneratorLut in rs_drawable_content.cpp when new slots are added
enum class RSDrawableSlot : int8_t {
    INVALID = -1,
    SAVE_ALL = 0,

    // Bounds Geometry
    MASK,
    TRANSITION,
    ENV_FOREGROUND_COLOR,
    SHADOW,
    FOREGROUND_FILTER,
    OUTLINE,

    // BG properties in Bounds Clip
    BG_SAVE_BOUNDS,
    CLIP_TO_BOUNDS,
    BLENDER,
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
    DYNAMIC_DIM,
    COMPOSITING_FILTER,
    FOREGROUND_COLOR,
    FG_RESTORE_BOUNDS,

    // No clip (unless ClipToBounds is set)
    POINT_LIGHT,
    BORDER,
    OVERLAY,
    PARTICLE_EFFECT,
    PIXEL_STRETCH,

    // Restore state
    RESTORE_BLENDER,
    RESTORE_FOREGROUND_FILTER,
    RESTORE_ALL,

    // Annotations: Please remember to update this when new slots are added.
    // properties before Background, not clipped
    TRANSITION_PROPERTIES_BEGIN = SHADOW,
    TRANSITION_PROPERTIES_END   = OUTLINE,
    // background properties, clipped by bounds by default
    BG_PROPERTIES_BEGIN         = BLENDER,
    BG_PROPERTIES_END           = ENV_FOREGROUND_COLOR_STRATEGY,
    // content properties, can be clipped by ClipToFrame and ClipToBounds
    CONTENT_BEGIN               = FRAME_OFFSET,
    CONTENT_END                 = FOREGROUND_STYLE,
    // foreground properties, clipped by bounds by default
    FG_PROPERTIES_BEGIN         = BINARIZATION,
    FG_PROPERTIES_END           = FOREGROUND_COLOR,
    // post-foreground properties, can be clipped by ClipToBounds
    EXTRA_PROPERTIES_BEGIN      = POINT_LIGHT,
    EXTRA_PROPERTIES_END        = PIXEL_STRETCH,

    MAX = RESTORE_ALL + 1,
};

// pure virtual base class
class RSDrawable : public std::enable_shared_from_this<RSDrawable> {
public:
    RSDrawable() = default;
    virtual ~RSDrawable() = default;

    // Not copyable and moveable
    RSDrawable(const RSDrawable&) = delete;
    RSDrawable(const RSDrawable&&) = delete;
    RSDrawable& operator=(const RSDrawable&) = delete;
    RSDrawable& operator=(const RSDrawable&&) = delete;

    // Type definitions
    using Ptr = std::shared_ptr<RSDrawable>;
    using Vec = std::array<Ptr, static_cast<size_t>(RSDrawableSlot::MAX)>;
    using Generator = std::function<Ptr(const RSRenderNode&)>;

    // UI methods: OnUpdate and OnGenerate (static method defined in every subclass) can only access the UI (staging)
    // members, else may cause crash.
    // OnUpdate and OnGenerate will be invoked if related property has changed, if false is returned, the drawable will
    // be erased.
    virtual bool OnUpdate(const RSRenderNode& content)
    {
        return true;
    }

    // Render helper methods: This func is called in UI thread, but the generated DrawFunc will be called in RT thread,
    // they can only access the RT members, else may cause crash
    virtual Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const = 0;

    // Sync methods, then can access all members and do UI->RT sync
    virtual void OnSync() = 0;

    // static generate & update helper methods
    // Step 1, calculate dirtySlots based on dirty modifiers
    static std::unordered_set<RSDrawableSlot> CalculateDirtySlots(
        const ModifierDirtyTypes& dirtyTypes, const Vec& drawableVec);
    // Step 2, for every dirtySlot, update or generate RSDrawable
    static bool UpdateDirtySlots(
        const RSRenderNode& node, Vec& drawableVec, std::unordered_set<RSDrawableSlot>& dirtySlots);
    // Step 3, insert necessary Clip/Save/Restore into drawableVec
    static void UpdateSaveRestore(RSRenderNode& node, Vec& drawableVec, uint8_t& drawableVecStatus);
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_H
