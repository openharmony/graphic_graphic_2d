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

#ifndef RENDER_SERVICE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_H

#include <memory>

#include "draw/canvas.h"
#include "drawable/rs_render_node_drawable_adapter.h"

namespace OHOS::Rosen {
class RSRenderNode;

enum class ReplayType : uint8_t {
    // Default
    REPLAY_ALL,
    // Shadow batching
    REPLAY_ONLY_SHADOW,
    REPLAY_ALL_EXCEPT_SHADOW,
    // For surface render node
    REPLAY_BG_ONLY,
    REPLAY_FG_ONLY,
    REPLAY_ONLY_CONTENT,
};

// Used by RSUniRenderThread and RSChildrenDrawable
class RSRenderNodeDrawable : public RSRenderNodeDrawableAdapter {
public:
    explicit RSRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    ~RSRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void Draw(Drawing::Canvas& canvas) const override;

    virtual void OnDraw(Drawing::Canvas& canvas) const;
    virtual void OnCapture(Drawing::Canvas& canvas) const;

protected:
    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::RS_NODE, OnGenerate>;
    static Registrar instance_;

    void DrawBackground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawContent(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawForeground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;

private:
    void DrawRangeImpl(Drawing::Canvas& canvas, const Drawing::Rect& rect, int8_t start, int8_t end) const;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_H
