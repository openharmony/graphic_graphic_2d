/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_rcd_render_visitor.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node_drawable.h"
#include "rs_rcd_render_listener.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSRcdRenderVisitor::RSRcdRenderVisitor()
{
    renderEngine_ = RSUniRenderThread::Instance().GetRenderEngine();
}

bool RSRcdRenderVisitor::PrepareResourceBuffer(
    RSRcdSurfaceRenderNode &node, std::shared_ptr<rs_rcd::RoundCornerLayer> &layerInfo)
{
    auto drawable = std::static_pointer_cast<DrawableV2::RSRcdSurfaceRenderNodeDrawable>(node.GetRenderDrawable());
    if (drawable == nullptr) {
        RS_LOGE("RSRcdRenderVisitor::PrepareResourceBuffer CreateSurface drawable is nullptr");
        return false;
    }
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(node.GetRSSurfaceHandler());
    if (listener == nullptr || (!drawable->CreateSurface(listener))) {
        RS_LOGE("RSRcdRenderVisitor::PrepareResourceBuffer CreateSurface failed");
        return false;
    }
    if (layerInfo == nullptr || (!node.PrepareHardwareResourceBuffer(layerInfo))) {
        RS_LOGE("PrepareHardwareResourceBuffer is wrong");
        return false;
    }
    return true;
}

void RSRcdRenderVisitor::ProcessRcdSurfaceRenderNode(RSRcdSurfaceRenderNode& node, bool resourceChanged)
{
    if (uniProcessor_ == nullptr || node.IsInvalidSurface() || resourceChanged) {
        RS_LOGE("RSRcdRenderVisitor RSProcessor is null or node invalid or resource is changed!");
        return;
    }

    if (node.GetRSSurfaceHandler() != nullptr && node.GetRSSurfaceHandler()->GetBuffer() != nullptr) {
        uniProcessor_->ProcessRcdSurface(node);
        return;
    }
}

void RSRcdRenderVisitor::SetUniProcessor(std::shared_ptr<RSProcessor> processor)
{
    uniProcessor_ = processor;
}
} // namespace Rosen
} // namespace OHOS