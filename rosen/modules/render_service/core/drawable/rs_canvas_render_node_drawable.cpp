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
#include "platform/common/rs_log.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen {
RSCanvasRenderNodeDrawable::Registrar RSCanvasRenderNodeDrawable::instance_;

RSCanvasRenderNodeDrawable::RSCanvasRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSCanvasRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return std::make_unique<RSCanvasRenderNodeDrawable>(std::move(node));
}

void RSCanvasRenderNodeDrawable::OnDraw(Drawing::Canvas* canvas) const
{
    if (!renderNode_) {
        RS_LOGE("There is no CanvasNode in drawable");
        return;
    }

    if (!canvas) {
        RS_LOGE("There is no canvas in drawable");
        return;
    }

    auto& params = renderNode_->GetRenderParams();
    if (!params) {
        RS_LOGE("params is nullptr");
        return;
    }
    if (!params->GetShouldPaint()) {
        return;
    }
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ConcatMatrix(params->GetMatrix());
    bool quickRejected = canvas->QuickReject(params->GetBounds());
    if (quickRejected) {
        RS_LOGD("this drawable has quickRejected");
    }
    RSRenderNodeDrawable::OnDraw(canvas);
}
} // namespace OHOS::Rosen
