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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_CONTENT_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_CONTENT_H

#include <bitset>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_set>

#include "drawable/rs_drawable.h"
#include "modifier/rs_modifier_type.h"

namespace OHOS::Rosen {
class RSRenderNode;
class RSRenderContent;

// NOTE: MUST update DrawableGeneratorLut in rs_property_drawable.cpp when new slots are added
enum class RSDrawableContentSlot : uint8_t {
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

// pure virtual base class
class RSDrawableContent : public std::enable_shared_from_this<RSDrawableContent> {
public:
    RSDrawableContent() = default;
    virtual ~RSDrawableContent() = default;

    // type definition
    using Ptr = std::shared_ptr<RSDrawableContent>;
    using Vec = std::array<Ptr, static_cast<size_t>(RSDrawableContentSlot::MAX)>;
    using Generator = std::function<Ptr(const RSRenderNode&)>;

    // Call on first create, return nullptr if no need to create
    // static Ptr OnGenerate(const RSRenderNode& content) { return nullptr; };

    // Call on property change, return true if update succeed, false if need destroy
    virtual bool OnUpdate(const RSRenderNode& content)
    {
        return true;
    }

    // Call on thread sync
    virtual void OnSync() = 0;

    virtual RSDrawable::Ptr CreateDrawable() const = 0;

    // not copyable and moveable
    RSDrawableContent(const RSDrawableContent&) = delete;
    RSDrawableContent(const RSDrawableContent&&) = delete;
    RSDrawableContent& operator=(const RSDrawableContent&) = delete;
    RSDrawableContent& operator=(const RSDrawableContent&&) = delete;


#ifdef USE_ROSEN_DRAWING
    // Step 1, generate DirtySlots from dirty Modifiers
    using ModifierDirtyTypes = std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)>;
    static std::unordered_set<RSDrawableContentSlot> GenerateDirtySlots(
        ModifierDirtyTypes& dirtyTypes, const Vec& drawableVec);
#endif
    // Step 2, for every DirtySlot, generate DrawableContent
    static bool UpdateDrawableVec(
        const RSRenderNode& node, Vec& drawableVec, std::unordered_set<RSDrawableContentSlot>& dirtySlots);
    // Step 3, add necessary Clip/Save/Restore
    static void UpdateSaveRestore(RSRenderContent& content, Vec& drawableVec, uint8_t& drawableVecStatus);
};

// RSChildrenDrawable, for drawing children of RSRenderNode, updates on child add/remove
class RSRenderNodeDrawableAdapter;
class RSChildrenDrawableContent : public RSDrawableContent {
public:
    RSChildrenDrawableContent() = default;
    ~RSChildrenDrawableContent() override = default;

    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& content) override;
    void OnSync() override;
    RSDrawable::Ptr CreateDrawable() const override;

private:
    bool needSync_ = false;
    std::vector<std::unique_ptr<RSRenderNodeDrawableAdapter>> childrenDrawables_;
    std::vector<std::unique_ptr<RSRenderNodeDrawableAdapter>> stagingChildrenDrawables_;
    friend class RSChildrenDrawable;
};

// RSChildrenDrawable, for drawing custom modifiers
enum class RSModifierType : int16_t;
namespace Drawing {
class DrawCmdList;
}
class RSCustomModifierDrawableContent : public RSDrawableContent {
public:
    RSCustomModifierDrawableContent(RSModifierType type) : type_(type) {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& content, RSModifierType type);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    RSDrawable::Ptr CreateDrawable() const override;

private:
    RSModifierType type_;

    bool needSync_ = false;
    std::vector<const std::shared_ptr<Drawing::DrawCmdList>> drawCmdList_;
    std::vector<const std::shared_ptr<Drawing::DrawCmdList>> stagingDrawCmdList_;
    friend class RSCustomModifierDrawable;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_CONTENT_H
