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
#include <string>

#include "benchmarks/rs_recording_thread.h"
#include "rs_trace.h"

#include "params/rs_display_render_params.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "screen_manager/rs_screen_manager.h"

// dfx
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/dfx/rs_skp_capture_dfx.h"
namespace OHOS::Rosen {
RSDisplayRenderNodeDrawable::Registrar RSDisplayRenderNodeDrawable::instance_;

RSDisplayRenderNodeDrawable::RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSDisplayRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return std::make_unique<RSDisplayRenderNodeDrawable>(std::move(node));
}

static inline std::vector<RectI> MergeDirtyHistory(std::shared_ptr<RSDisplayRenderNode> displayNodeSp,
    int32_t bufferAge, ScreenInfo& screenInfo, RSDirtyRectsDfx& rsDirtyRectsDfx)
{
    auto params = static_cast<RSDisplayRenderParams*>(displayNodeSp->GetRenderParams().get());
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams) {
        return {};
    }
    auto& curAllSurfaces = params->GetAllMainAndLeashSurfaces();
    auto dirtyManager = displayNodeSp->GetDirtyManager();

    RSUniRenderUtil::MergeDirtyHistory(displayNodeSp, bufferAge, false, true);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(curAllSurfaces,
        RSUniRenderThread::Instance().GetDrawStatusVec(), false, true);
    RSUniRenderUtil::SetAllSurfaceGlobalDityRegion(curAllSurfaces, dirtyManager->GetDirtyRegion());

    // DFX START
    rsDirtyRectsDfx.SetDirtyRegion(dirtyRegion);
    // DFX END

    RectI rect = dirtyManager->GetDirtyRegionFlipWithinSurface();
    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
    }

    Drawing::Region region;
    for (auto& r : rects) {
        int32_t topAfterFilp = 0;
#ifdef RS_ENABLE_VK
        topAfterFilp =
            (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) ?
            r.top_ : static_cast<int32_t>(screenInfo.GetRotatedHeight()) - r.GetBottom();
#else
        topAfterFilp = static_cast<int32_t>(screenInfo.GetRotatedHeight()) - r.GetBottom();
#endif
        Drawing::Region tmpRegion;
        tmpRegion.SetRect(Drawing::RectI(r.left_, topAfterFilp,
            r.left_ + r.width_, topAfterFilp + r.height_));
        region.Op(tmpRegion, Drawing::RegionOp::UNION);
    }
    return rects;
}

std::unique_ptr<RSRenderFrame> RSDisplayRenderNodeDrawable::RequestFrame(std::shared_ptr<RSDisplayRenderNode> displayNodeSp,
    std::shared_ptr<RSProcessor> processor, ScreenInfo& screenInfo) const
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame RenderEngine is null!");
        return nullptr;
    }

    if (!processor->Init(*displayNodeSp, displayNodeSp->GetDisplayOffsetX(), displayNodeSp->GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine)) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame processor init failed!");
        return nullptr;
    }
    if (!displayNodeSp->IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(displayNodeSp);
        if (!displayNodeSp->CreateSurface(listener)) {
            RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame CreateSurface failed");
            return nullptr;
        }
    }

    auto rsSurface = displayNodeSp->GetRSSurface();
    if (!rsSurface) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame No RSSurface found");
        return nullptr;
    }
    auto bufferConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true);
    auto renderFrame = renderEngine->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface), bufferConfig);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame renderEngine requestFrame is null");
        return nullptr;
    }

    return renderFrame;
}

void RSDisplayRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas) const
{
    // canvas will generate in every request frame
    (void)canvas;

    if (!renderNode_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw render node is null!");
        return;
    }

    auto& params = renderNode_->GetRenderParams();
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw params is null!");
         return;
     }

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(nodeSp);
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable[" + std::to_string(displayNodeSp->GetScreenId()) + "]" +
        displayNodeSp->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str());
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw node: %{public}" PRIu64 ", child size:%{public}u",
        displayNodeSp->GetId(), displayNodeSp->GetChildrenCount());

    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw screen manager is null!");
        return;
    }

    // TO-DO get screenInfo from params
    auto screenInfo = screenManager->QueryScreenInfo(displayNodeSp->GetScreenId());
    // TO-DO auto compositeType = displayNodeSp->GetCompositeType();
    auto compositeType = RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE;
    // can cache?
    auto processor = RSProcessorFactory::CreateProcessor(compositeType);
    if (!processor) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RSProcessor is null!");
        return;
    }
    auto renderFrame = RequestFrame(displayNodeSp, processor, screenInfo);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to request frame");
        return;
    }

    RSDirtyRectsDfx rsDirtyRectsDfx(displayNodeSp);
    auto rects = MergeDirtyHistory(displayNodeSp, renderFrame->GetBufferAge(), screenInfo, rsDirtyRectsDfx);
    // TO-DO open it after fix dirty region error
    // renderFrame->SetDamageRegion(rects);
    auto drSurface = renderFrame->GetFrame()->GetSurface();
    if (!drSurface) {
         RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw DrawingSurface is null");
         return;
     }

    curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    if (!curCanvas_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to create canvas");
        return;
    }

    // canvas draw
    {
        RSSkpCaptureDfx capture(curCanvas_);
        Drawing::AutoCanvasRestore acr(*curCanvas_, true);
        RSRenderNodeDrawable::OnDraw(*curCanvas_);
    }
    rsDirtyRectsDfx.OnDraw(curCanvas_);

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable Flush");
    renderFrame->Flush();
    RS_TRACE_END();

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable CommitLayer");
    processor->ProcessDisplaySurface(*displayNodeSp);
    processor->PostProcess(displayNodeSp.get());
    RS_TRACE_END();
}

} // namespace OHOS::Rosen
