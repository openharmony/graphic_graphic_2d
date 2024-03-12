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
#include <memory>

#include "rs_trace.h"

#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "screen_manager/rs_screen_manager.h"
#include "benchmarks/rs_recording_thread.h"

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

    // auto compositeType = displayNodeSp->GetCompositeType();
    auto compositeType = RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE;
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

    curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    if (!curCanvas_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to create canvas");
        return;
    }

    // start recording skia op
    TryCapture(renderNode_->GetRenderProperties().GetBoundsWidth(), renderNode_->GetRenderProperties().GetBoundsHeight());

    // canvas draw
    {
        Drawing::AutoCanvasRestore acr(*curCanvas_, true);
        RSRenderNodeDrawable::OnDraw(curCanvas_.get());
    }

    // Finish recording skia op
    EndCapture();

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable Flush");
    renderFrame->Flush();
    RS_TRACE_END();

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable CommitLayer");
    processor->ProcessDisplaySurface(*displayNodeSp);
    processor->PostProcess(displayNodeSp.get());
    RS_TRACE_END();
}

std::shared_ptr<RenderContext> RSDisplayRenderNodeDrawable::GetRenderContext() const
{
    return RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext();
}

void RSDisplayRenderNodeDrawable::TryCapture(float width, float height) const
{
    if (!RSSystemProperties::GetRecordingEnabled()) {
        return;
    }
    recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height);
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable:Recording begin");
    std::shared_ptr<RenderContext> renderContext;
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    renderContext = GetRenderContext();
    if (!recordingCanvas_) {
        RS_LOGE("TryCapture recordingCanvas_ is nullptr");
        return;
    }
    if (!renderContext) {
        RS_LOGE("TryCapture renderContext is nullptr");
        return;
    }
    recordingCanvas_->SetGrRecordingContext(renderContext->GetSharedDrGPUContext());
#endif
    if (!curCanvas_) {
        RS_LOGE("TryCapture curCanvas_ is nullptr");
        return;
    }
    curCanvas_->AddCanvas(recordingCanvas_.get());
    RSRecordingThread::Instance(renderContext.get()).CheckAndRecording();
}

void RSDisplayRenderNodeDrawable::EndCapture() const
{
    auto renderContext = GetRenderContext();
    if (!renderContext) {
        RS_LOGE("EndCapture renderContext is nullptr");
        return;
    }
    if (!RSRecordingThread::Instance(renderContext.get()).GetRecordingEnabled()) {
        return;
    }
    auto drawCmdList = recordingCanvas_->GetDrawCmdList();
    RS_TRACE_NAME("RSUniRender:RecordingToFile curFrameNum = " +
        std::to_string(RSRecordingThread::Instance(renderContext.get()).GetCurDumpFrame()));
    RSRecordingThread::Instance(renderContext.get()).RecordingToFile(drawCmdList);
}
} // namespace OHOS::Rosen
