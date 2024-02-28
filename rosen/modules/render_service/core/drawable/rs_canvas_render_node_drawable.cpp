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

#include "drawable/rs_canvas_render_node_drawable.h"

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen {
RSCanvasRenderNodeDrawable::RSCanvasRenderNodeDrawable(const std::shared_ptr<RSRenderNode>& renderNode)
    : RSRenderNodeDrawable(renderNode)
{}

std::shared_ptr<RSRenderNodeDrawable> RSCanvasRenderNodeDrawable::OnGenerate(std::shared_ptr<RSRenderNode> node)
{
    return std::make_shared<RSCanvasRenderNodeDrawable>(std::move(node));
}

void RSCanvasRenderNodeDrawable::OnDraw(RSPaintFilterCanvas& canvas) const
{
    // TODO
    RSRenderNodeDrawable::OnDraw(canvas);
}
} // namespace OHOS::Rosen
