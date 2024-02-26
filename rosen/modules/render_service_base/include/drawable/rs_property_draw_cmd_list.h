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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAW_CMD_LIST_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAW_CMD_LIST_H

#include <bitset>
#include <memory>
#include <set>
#include <vector>

#include "recording/draw_cmd_list.h"

namespace OHOS::Rosen {
class RSRenderContent;
enum class RSModifierType : int16_t;

// NOTE: MUST update DrawableGeneratorLut in rs_property_drawable.cpp when new slots are added
enum class RSPropertyDrawCmdListSlot : uint8_t {
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

class RSPropertyDrawCmdList {
public:
    RSPropertyDrawCmdList(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : drawCmdList_(std::move(drawCmdList)) {}
    virtual ~RSPropertyDrawCmdList() = default;

    // every child class should have this
    // static std::shared_ptr<RSPropertyDrawCmdList> Generate(const RSRenderContent& content) { return nullptr; };

    // return true if this drawable can be updated thus not need to recreate, default to false
    virtual bool Update(const RSRenderContent& content);

    void OnSync();

    using DrawCmdListPtr = std::shared_ptr<RSPropertyDrawCmdList>;
    using DrawCmdListVec = std::array<DrawCmdListPtr, static_cast<size_t>(RSPropertyDrawCmdListSlot::MAX)>;
    using DrawCmdListGenerator = std::function<DrawCmdListPtr(const RSRenderContent&)>;

protected:
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList_;
    std::shared_ptr<Drawing::DrawCmdList> stagingDrawCmdList_;

    friend class RSPropertyDrawCmdListUpdater;
    friend class RSPropertyDrawableNG;
};

class RSCustomModifierDrawCmdList : public RSPropertyDrawCmdList {
public:
    RSCustomModifierDrawCmdList(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList, RSModifierType type)
        : RSPropertyDrawCmdList(std::move(drawCmdList)), type_(type)
    {}
    static std::shared_ptr<RSPropertyDrawCmdList> Generate(const RSRenderContent& content, RSModifierType type);
    bool Update(const RSRenderContent& content) override;

private:
    RSModifierType type_;
};

// DEMO
class RSBackgroundDrawCmdList : public RSPropertyDrawCmdList {
public:
    static std::shared_ptr<RSPropertyDrawCmdList> Generate(const RSRenderContent& content);
    bool Update(const RSRenderContent& content) override;
};

// DEMO
class RSBorderDrawCmdList : public RSPropertyDrawCmdList {
public:
    static std::shared_ptr<RSPropertyDrawCmdList> Generate(const RSRenderContent& content);
    bool Update(const RSRenderContent& content) override;
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAW_CMD_LIST_H
