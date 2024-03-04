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

#include "drawable/rs_display_render_node_drawable.h"

#include "rs_trace.h"

#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_thread.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
RSDisplayRenderNodeDrawable::Registrar RSDisplayRenderNodeDrawable::instance_;

RSDisplayRenderNodeDrawable::RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSDisplayRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return std::make_unique<RSDisplayRenderNodeDrawable>(std::move(node));
}

void RSDisplayRenderNodeDrawable::OnDraw(Drawing::Canvas* canvas) const
{
    (void)canvas;

    // PLANNING multi_thread
    if (!renderNode_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RenderNode is null!");
        return;
    }
    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(nodeSp);
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable[" + std::to_string(displayNodeSp->GetScreenId()) + "]" +
        displayNodeSp->GetDirtyManager()->GetDirtyRegion().ToString().c_str());
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode node: %{public}" PRIu64 ", child size:%{public}u",
        displayNodeSp->GetId(), displayNodeSp->GetChildrenCount());

    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw ScreenManager is null!");
        return;
    }

    auto compositeType = displayNodeSp->GetCompositeType();
    // can cache?
    auto processor = RSProcessorFactory::CreateProcessor(compositeType);
    if (processor == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RSProcessor is null!");
        return;
    }

    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RenderEngine is null!");
        return;
    }

    if (!processor->Init(*displayNodeSp, displayNodeSp->GetDisplayOffsetX(), displayNodeSp->GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine)) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw processor init failed!");
        return;
    }

    if (!displayNodeSp->IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(displayNodeSp);
        if (!displayNodeSp->CreateSurface(listener)) {
            RS_LOGE("RSDisplayRenderNodeDrawable::ProcessDisplayRenderNode CreateSurface failed");
            return;
        }
    }

    auto rsSurface = displayNodeSp->GetRSSurface();
    if (!rsSurface) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw No RSSurface found");
        return;
    }

    auto screenInfo = screenManager->QueryScreenInfo(displayNodeSp->GetScreenId());
    auto bufferConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true);
    auto renderFrame = renderEngine->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface), bufferConfig);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RequestFrame is null");
        return;
    }

    auto drSurface = renderFrame->GetFrame()->GetSurface();
    if (drSurface == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw DrawingSurface is null");
        return;
    }

    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    if (!curCanvas) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to create canvas");
        return;
    }

    // canvas draw
    curCanvas->Save();
    RSRenderNodeDrawable::OnDraw(curCanvas.get());
    curCanvas->Restore();
    renderFrame->Flush();
    processor->ProcessDisplaySurface(*displayNodeSp);
    processor->PostProcess(displayNodeSp.get());
}
} // namespace OHOS::Rosen
