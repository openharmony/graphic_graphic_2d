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

#include "drawable/rs_surface_render_node_drawable.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {
RSSurfaceRenderNodeDrawable::Registrar RSSurfaceRenderNodeDrawable::instance_;

RSSurfaceRenderNodeDrawable::RSSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSSurfaceRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return std::make_unique<RSSurfaceRenderNodeDrawable>(std::move(node));
}

void RSSurfaceRenderNodeDrawable::OnDraw(RSPaintFilterCanvas* canvas) const
{
    if (!renderNode_) {
        RS_LOGE("There is no CanvasNode in RSSurfaceRenderNodeDrawable");
        return;
    }

    if (!canvas) {
        RS_LOGE("There is no canvas in drawable");
        return;
    }

    RS_LOGD("RSSurfaceRenderNodeDrawable::OnDraw node: %{public}" PRIu64, renderNode_->GetId());

    RSRenderNodeDrawable::OnDraw(canvas);
}
} // namespace OHOS::Rosen
