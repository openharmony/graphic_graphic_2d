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

#ifndef RENDER_SERVICE_DRAWABLE_RS_CANVAS_DRAWING_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_CANVAS_DRAWING_RENDER_NODE_DRAWABLE_H

#include "drawable/rs_render_node_drawable.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen {
class RSCanvasDrawingRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    explicit RSCanvasDrawingRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    ~RSCanvasDrawingRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;

private:
    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::CANVAS_DRAWING_NODE, OnGenerate>;

    void DrawRenderContent(
        RSCanvasDrawingRenderNodeContent& renderContent, Drawing::Canvas& canvas, const Drawing::Rect& rect) const;

    static Registrar instance_;
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_CANVAS_DRAWING_RENDER_NODE_DRAWABLE_H
