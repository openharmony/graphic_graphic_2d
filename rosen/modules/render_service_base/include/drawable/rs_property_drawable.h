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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H

#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_rect.h"
#include "recording/draw_cmd_list.h"

#include "drawable/rs_drawable.h"
#include "drawable/rs_filter_drawable.h"
#include "pipeline/rs_simple_draw_cmd_list.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen {
class RSRenderNode;
class ExtendRecordingCanvas;
namespace Drawing {
class GEVisualEffectContainer;
}

namespace DrawableV2 {
class RSPropertyDrawable : public RSDrawable {
public:
    RSPropertyDrawable(SimpleDrawCmdListPtr&& drawCmdList) : drawCmdList_(std::move(drawCmdList)) {}
    RSPropertyDrawable() = default;
    ~RSPropertyDrawable() override = default;

    void OnSync() override;
    void OnPurge() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

protected:
    bool needSync_ = false;
    SimpleDrawCmdListPtr drawCmdList_;
    SimpleDrawCmdListPtr stagingDrawCmdList_;
    std::string propertyDescription_;
    std::string stagingPropertyDescription_;

    friend class RSPropertyDrawCmdListUpdater;
};

class RSPropertyDrawCmdListUpdater {
public:
    // not copyable and moveable
    RSPropertyDrawCmdListUpdater(const RSPropertyDrawCmdListUpdater&) = delete;
    RSPropertyDrawCmdListUpdater(const RSPropertyDrawCmdListUpdater&&) = delete;
    RSPropertyDrawCmdListUpdater& operator=(const RSPropertyDrawCmdListUpdater&) = delete;
    RSPropertyDrawCmdListUpdater& operator=(const RSPropertyDrawCmdListUpdater&&) = delete;

    explicit RSPropertyDrawCmdListUpdater(int width, int height, RSPropertyDrawable* target);
    virtual ~RSPropertyDrawCmdListUpdater();
    const std::unique_ptr<ExtendRecordingCanvas>& GetRecordingCanvas() const;

protected:
    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas_;
    RSPropertyDrawable* target_;
};

// ============================================================================
// Frame offset
class RSFrameOffsetDrawable : public RSPropertyDrawable {
public:
    RSFrameOffsetDrawable(SimpleDrawCmdListPtr&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSFrameOffsetDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};

// ============================================================================
// Clip To Bounds and Clip To Frame
class RSClipToBoundsDrawable : public RSDrawable {
public:
    RSClipToBoundsDrawable() = default;
    ~RSClipToBoundsDrawable() override = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

private:
    Drawing::Path stagingDrawingPath_;
    Drawing::Path drawingPath_;
    Drawing::RoundRect stagingClipRRect_;
    Drawing::RoundRect clipRRect_;
    Drawing::Rect stagingBoundsRect_;
    Drawing::Rect boundsRect_;
    NodeId stagingNodeId_ = INVALID_NODEID;
    NodeId nodeId_ = INVALID_NODEID;
    bool stagingIsClipRRectOptimization_ = false;
    bool isClipRRectOptimization_ = false;
    RSClipToBoundsType stagingType_ = RSClipToBoundsType::INVALID;
    RSClipToBoundsType type_ = RSClipToBoundsType::INVALID;
    bool needSync_ = false;
    std::shared_ptr<Drawing::GEVisualEffectContainer> stagingGeContainer_ = nullptr;
    std::shared_ptr<Drawing::GEVisualEffectContainer> geContainer_ = nullptr;
    Drawing::Rect sdfDrawRect_;
    Drawing::Rect stagingSdfDrawRect_;
};

class RSClipToFrameDrawable : public RSPropertyDrawable {
public:
    RSClipToFrameDrawable(SimpleDrawCmdListPtr&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSClipToFrameDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H
