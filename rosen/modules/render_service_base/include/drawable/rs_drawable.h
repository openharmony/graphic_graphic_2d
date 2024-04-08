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
    DYNAMIC_DIM,
    FOREGROUND_FILTER,
    FOREGROUND_COLOR,
    FG_RESTORE_BOUNDS,

    // No clip (unless ClipToBounds is set)
    POINT_LIGHT,
    BORDER,
    OVERLAY,
    PARTICLE_EFFECT,
    PIXEL_STRETCH,

    // Restore state
    RESTORE_BLEND_MODE,
    RESTORE_ALL,

    // Annotations: Please remember to update this when new slots are added.
    // NOTE: MAX and *_END enums are using the one-past-the-end style.
    BG_PROPERTIES_BEGIN      = BLEND_MODE,
    BG_PROPERTIES_END        = ENV_FOREGROUND_COLOR_STRATEGY + 1,
    CONTENT_PROPERTIES_BEGIN = FRAME_OFFSET,
    CONTENT_PROPERTIES_END   = FOREGROUND_STYLE + 1,
    FG_PROPERTIES_BEGIN      = BINARIZATION,
    FG_PROPERTIES_END        = FOREGROUND_COLOR + 1,
    MIN                      = SAVE_ALL,
    MAX                      = RESTORE_ALL + 1,
};

// pure virtual base class
class RSDrawable : public std::enable_shared_from_this<RSDrawable> {
public:
    RSDrawable() = default;
    virtual ~RSDrawable() = default;

    // not copyable and moveable
    RSDrawable(const RSDrawable&) = delete;
    RSDrawable(const RSDrawable&&) = delete;
    RSDrawable& operator=(const RSDrawable&) = delete;
    RSDrawable& operator=(const RSDrawable&&) = delete;

    // =================type definition==================
    using Ptr = std::shared_ptr<RSDrawable>;
    using Vec = std::array<Ptr, static_cast<size_t>(RSDrawableSlot::MAX)>;
    using Generator = std::function<Ptr(const RSRenderNode&)>;

    // =================virtual functions==================

    // Call on property change, return true if update succeed, false if need destroy
    virtual bool OnUpdate(const RSRenderNode& content)
    {
        return true;
    }

    // Call on thread sync
    virtual void OnSync() = 0;

    // !!!!!!!!!!!!!!!!!!!!!!!!!!
    // Important Note:
    // DrawFunc can only access the RT members variables, accessing staging members will cause a crash
    // !!!!!!!!!!!!!!!!!!!!!!!!!!
    virtual Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const = 0;

    // =================Generate & Update helper methods==================
    // Step 1, generate DirtySlots from dirty Modifiers
    static std::unordered_set<RSDrawableSlot> CalculateDirtySlots(
        const ModifierDirtyTypes& dirtyTypes, const Vec& drawableVec);
    // Step 2, for every DirtySlot, generate DrawableContent
    static bool UpdateDirtySlots(
        const RSRenderNode& node, Vec& drawableVec, std::unordered_set<RSDrawableSlot>& dirtySlots);
    // Step 3, add necessary Clip/Save/Restore
    static void UpdateSaveRestore(RSRenderNode& node, Vec& drawableVec, uint8_t& drawableVecStatus);
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_H
