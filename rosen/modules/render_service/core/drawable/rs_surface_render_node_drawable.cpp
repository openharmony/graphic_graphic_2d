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
#include "params/rs_surface_render_params.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_trace.h"
namespace OHOS::Rosen {
RSSurfaceRenderNodeDrawable::Registrar RSSurfaceRenderNodeDrawable::instance_;

RSSurfaceRenderNodeDrawable::RSSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSSurfaceRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return std::make_unique<RSSurfaceRenderNodeDrawable>(std::move(node));
}

void RSSurfaceRenderNodeDrawable::OnDraw(Drawing::Canvas* canvas) const
{
    if (!renderNode_) {
        RS_LOGE("There is no CanvasNode in RSSurfaceRenderNodeDrawable");
        return;
    }

    if (!canvas) {
        RS_LOGE("There is no canvas in drawable");
        return;
    }
    auto& params = renderNode_->GetRenderParams();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(params.get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw params is nullptr");
        return;
    }
    if (!surfaceParams->GetOcclusionVisible()) {
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw occlusion skip name:%" PRIu64"", renderNode_->GetId());
        return;
    }

    RS_LOGD("RSSurfaceRenderNodeDrawable::OnDraw node: %{public}" PRIu64, renderNode_->GetId());

    auto surfaceNode = renderNode_->ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode != nullptr && !surfaceNode->IsNotifyUIBufferAvailable()) {
        // Notify UI buffer available, temporarily fix
        auto mutableSurfaceNode = std::const_pointer_cast<RSSurfaceRenderNode>(surfaceNode);
        mutableSurfaceNode->NotifyUIBufferAvailable();
    }
    RSRenderNodeDrawable::OnDraw(canvas);
}
} // namespace OHOS::Rosen
