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

#include "drawable/rs_render_node_drawable.h"

#include "drawable/rs_canvas_render_node_drawable.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_effect_render_node_drawable.h"
#include "drawable/rs_root_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {
RSRenderNodeDrawable::Registrar RSRenderNodeDrawable::instance_;

RSRenderNodeDrawable::RSRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : renderNode_(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    if (node == nullptr) {
        return nullptr;
    }
    auto Generator = GeneratorMap.find(node->GetType());
    if (Generator != GeneratorMap.end()) {
        return Generator->second(node);
    }
    return nullptr;
}

void RSRenderNodeDrawable::OnDraw(Drawing::Canvas* canvas) const
{
    if (renderNode_ == nullptr) {
        return;
    }

    if (!canvas) {
        RS_LOGE("There is no canvas in drawable");
        return;
    }

    const auto& drawCmdList_ = renderNode_->drawCmdList_;
    if (drawCmdList_ == nullptr) {
        return;
    }
    auto& renderParams = renderNode_->GetRenderParams();
    Drawing::Rect bounds;
    if (renderParams) {
        bounds = renderParams->GetBounds();
    }
    Drawing::AutoCanvasRestore arc(*canvas, true);
    drawCmdList_->Playback(*canvas, &bounds);
}

} // namespace OHOS::Rosen
