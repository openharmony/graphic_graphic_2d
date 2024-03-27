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

#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "property/rs_point_light_manager.h"
#include "screen_manager/rs_screen_manager.h"
#include "system/rs_system_parameters.h"
// dfx
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/dfx/rs_skp_capture_dfx.h"
#include "drawable/rs_surface_render_node_drawable.h"

namespace OHOS::Rosen::DrawableV2 {
RSDisplayRenderNodeDrawable::Registrar RSDisplayRenderNodeDrawable::instance_;

RSDisplayRenderNodeDrawable::RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSDisplayRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSDisplayRenderNodeDrawable(std::move(node));
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
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(
        curAllSurfaces, RSUniRenderThread::Instance().GetDrawStatusVec(), false, true);
    RSUniRenderUtil::SetAllSurfaceGlobalDityRegion(curAllSurfaces, dirtyManager->GetDirtyRegion());

    // DFX START
    rsDirtyRectsDfx.SetDirtyRegion(dirtyRegion);
    // DFX END

    RectI rect = dirtyManager->GetDirtyRegionFlipWithinSurface();
    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
    }

    return rects;
}

std::unique_ptr<RSRenderFrame> RSDisplayRenderNodeDrawable::RequestFrame(
    std::shared_ptr<RSDisplayRenderNode> displayNodeSp, RSDisplayRenderParams& params,
    std::shared_ptr<RSProcessor> processor) const
{
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable:RequestFrame");
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame RenderEngine is null!");
        return nullptr;
    }

    if (!processor->Init(*displayNodeSp, params.GetDisplayOffsetX(), params.GetDisplayOffsetY(), INVALID_SCREEN_ID,
            renderEngine, true)) {
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
    auto bufferConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(params.GetrScreenInfo(), true);

#if 0 // TO-DO wait buffer
    if (!RSUniRenderThread::Instance().WaitUntilDisplayNodeBufferReleased(
            std::static_pointer_cast<RSSurfaceHandler>(displayNodeSp))) {
        RS_TRACE_NAME("RSDisplayRenderNodeDrawable::RequestFrame no released buffer");
    }
#endif

    auto renderFrame = renderEngine->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface), bufferConfig);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame renderEngine requestFrame is null");
        return nullptr;
    }

    return renderFrame;
}

static inline Drawing::Region GetFilpedRegion(std::vector<RectI>& rects, ScreenInfo& screenInfo)
{
    Drawing::Region region;

    for (auto& r : rects) {
        int32_t topAfterFilp = 0;
#ifdef RS_ENABLE_VK
        topAfterFilp = (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                           RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR)
                           ? r.top_
                           : static_cast<int32_t>(screenInfo.GetRotatedHeight()) - r.GetBottom();
#else
        topAfterFilp = static_cast<int32_t>(screenInfo.GetRotatedHeight()) - r.GetBottom();
#endif
        Drawing::Region tmpRegion;
        tmpRegion.SetRect(Drawing::RectI(r.left_, topAfterFilp, r.left_ + r.width_, topAfterFilp + r.height_));
        RS_TRACE_NAME_FMT("GetFilpedRegion orig ltrb[%d %d %d %d] to fliped rect ltrb[%d %d %d %d]", r.left_, r.top_,
            r.left_ + r.width_, r.top_ + r.height_, r.left_, topAfterFilp, r.left_ + r.width_,
            topAfterFilp + r.height_);

        region.Op(tmpRegion, Drawing::RegionOp::UNION);
    }
    return region;
}

static inline void ClipRegion(Drawing::Canvas& canvas, Drawing::Region& region)
{
    if (region.IsEmpty()) {
        // [planning] Remove this after frame buffer can cancel
        canvas.ClipRect(Drawing::Rect());
    } else if (region.IsRect()) {
        canvas.ClipRegion(region);
        canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
    } else {
        RS_TRACE_NAME("RSDisplayDrawable: clipPath");
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            canvas.ClipRegion(region);
        } else {
            Drawing::Path dirtyPath;
            region.GetBoundaryPath(&dirtyPath);
            canvas.ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
        }
#else
        Drawing::Path dirtyPath;
        region.GetBoundaryPath(&dirtyPath);
        canvas.ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
#endif
    }
}

void RSDisplayRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    // canvas will generate in every request frame
    (void)canvas;

    if (!renderNode_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw render node is null!");
        return;
    }

    auto params = static_cast<RSDisplayRenderParams*>(renderNode_->GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw params is null!");
        return;
    }
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw params %s", params->ToString().c_str());

    isDrawingCacheEnabled_ = RSSystemParameters::GetDrawingCacheEnabled();
    isDrawingCacheDfxEnabled_ = RSSystemParameters::GetDrawingCacheEnabledDfx();
    drawingCacheRects_.clear();

    // check rotation for point light
    constexpr int ROTATION_NUM = 4;
    auto screenRotation = params->GetScreenRotation();
    if (RSSystemProperties::IsFoldScreenFlag() && params->GetScreenId() == 0) {
        screenRotation = static_cast<ScreenRotation>((static_cast<int>(screenRotation) + 1) % ROTATION_NUM);
    }
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(nodeSp);
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable[" + std::to_string(params->GetScreenId()) + "]" +
                  displayNodeSp->GetDirtyManager()->GetSyncCurrentFrameDirtyRegion().ToString().c_str());
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw node: %{public}" PRIu64 ", child size:%{public}u", params->GetId(),
        displayNodeSp->GetChildrenCount());

    auto screenInfo = params->GetrScreenInfo();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    if (!processor) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RSProcessor is null!");
        return;
    }

    // do virtual screen
    auto mirrorNode = params->GetMirrorSource().lock();
    if (mirrorNode) {
        auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
        if (renderEngine == nullptr) {
            RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RenderEngine is null!");
            return;
        }

        if (!processor->Init(
            *displayNodeSp, params->GetDisplayOffsetX(), params->GetDisplayOffsetY(),
                mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID, renderEngine)) {
                RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw processor init failed!");
                return;
            }
        processor->ProcessDisplaySurface(*mirrorNode);
        processor->PostProcess();
        return;
    }

    // displayNodeSp to get  rsSurface witch only used in renderThread
    auto renderFrame = RequestFrame(displayNodeSp, *params, processor);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to request frame");
        return;
    }

    RSDirtyRectsDfx rsDirtyRectsDfx(displayNodeSp);
    auto rects = MergeDirtyHistory(displayNodeSp, renderFrame->GetBufferAge(), screenInfo, rsDirtyRectsDfx);
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if (!uniParam || (uniParam->IsPartialRenderEnabled() && !uniParam->IsRegionDebugEnabled())) {
        renderFrame->SetDamageRegion(rects);
    }

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

    auto region = GetFilpedRegion(rects, screenInfo);
    // canvas draw
    {
        RSSkpCaptureDfx capture(curCanvas_);
        Drawing::AutoCanvasRestore acr(*curCanvas_, true);
        if (!uniParam || uniParam->IsOpDropped()) {
            ClipRegion(*curCanvas_, region);
        }
        SetHighContrastIfEnabled(*curCanvas_);
        RSRenderNodeDrawable::OnDraw(*curCanvas_);
    }
    rsDirtyRectsDfx.OnDraw(curCanvas_);

    // switch color filtering
    SwitchColorFilter(*curCanvas_);

    if (isDrawingCacheEnabled_ && isDrawingCacheDfxEnabled_) {
        for (const auto& rect : drawingCacheRects_) {
            RSUniRenderUtil::DrawRectForDfx(*curCanvas_, rect, Drawing::Color::COLOR_GREEN, 0.2f);
        }
    }

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable Flush");
    renderFrame->Flush();
    RS_TRACE_END();

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable CommitLayer");
    auto& selfDrawingNodes = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetSelfDrawingNodes();
    float globalZOrder = 0.f;
    for (const auto& surfaceNode : selfDrawingNodes) {
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
        if (params->GetHardwareEnabled()) {
            processor->CreateLayer(*surfaceNode, *params);
            globalZOrder++;
        }
    }
    displayNodeSp->SetGlobalZOrder(globalZOrder);
    processor->ProcessDisplaySurface(*displayNodeSp);
    processor->PostProcess();
    RS_TRACE_END();
}

void RSDisplayRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    if (!renderNode_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture render node is null!");
        return;
    }

    auto params = static_cast<RSDisplayRenderParams*>(renderNode_->GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture params is null!");
        return;
    }

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(nodeSp);

    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture, rscanvas us nullptr");
        return;
    }

    RS_TRACE_NAME("RSDisplayRenderNodeDrawable::OnCapture:" +
        std::to_string(params->GetId()));
    Drawing::AutoCanvasRestore acr(canvas, true);

    if (params->GetDisplayHasSecSurface() || params->GetDisplayHasSkipSurface()) {
        RS_LOGD("RSDisplayRenderNodeDrawable::OnCapture: params %{public}s \
            process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) Not using UniRender buffer.",
            params->ToString().c_str(), params->GetId());

        // Adding matrix affine transformation logic
        auto geoPtr = (displayNodeSp->GetRenderProperties().GetBoundsGeometry());
        if (geoPtr != nullptr) {
            rscanvas->ConcatMatrix(params->GetMatrix());
        }

        RSRenderNodeDrawable::OnCapture(canvas);
        DrawWatermarkIfNeed(*displayNodeSp, *rscanvas);
    } else {
        auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
        if (!processor) {
            RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture RSProcessor is null!");
            return;
        }

        // displayNodeSp to get rsSurface witch only used in renderThread
        auto renderFrame = RequestFrame(displayNodeSp, *params, processor);
        if (!renderFrame) {
            RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture failed to request frame");
            return;
        }

        if (displayNodeSp->GetBuffer() == nullptr) {
            RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture: buffer is null!");
            return;
        }

        RS_LOGD("RSDisplayRenderNodeDrawable::OnCapture: params %{public}s \
            process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) using UniRender buffer.",
            params->ToString().c_str(), params->GetId());

        if (params->GetHardwareEnabledNodes().size() != 0) {
            AdjustZOrderAndDrawSurfaceNode(params->GetHardwareEnabledNodes(), canvas);
        }

        auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
        auto drawParams = RSUniRenderUtil::CreateBufferDrawParam(*displayNodeSp, false);

        // Screen capture considering color inversion
        ColorFilterMode colorFilterMode = renderEngine->GetColorFilterMode();
        if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
            colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
            RS_LOGD("RSDisplayRenderNodeDrawable::OnCapture: \
                SetColorFilterModeToPaint mode:%{public}d.", static_cast<int32_t>(colorFilterMode));
            RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, drawParams.paint);
        }

        // To get dump image
        // execute "param set rosen.dumpsurfacetype.enabled 4 && setenforce 0 && param set rosen.afbc.enabled 0"
        RSBaseRenderUtil::WriteSurfaceBufferToPng(drawParams.buffer);
        renderEngine->DrawDisplayNodeWithParams(*rscanvas, *displayNodeSp, drawParams);

        if (params->GetHardwareEnabledTopNodes().size() != 0) {
            AdjustZOrderAndDrawSurfaceNode(params->GetHardwareEnabledTopNodes(), canvas);
        }
    }
}

void RSDisplayRenderNodeDrawable::SwitchColorFilter(RSPaintFilterCanvas& canvas) const
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    ColorFilterMode colorFilterMode = renderEngine->GetColorFilterMode();
    if (colorFilterMode == ColorFilterMode::INVERT_COLOR_DISABLE_MODE ||
        colorFilterMode >= ColorFilterMode::DALTONIZATION_NORMAL_MODE){
        return;
    }

    Drawing::AutoCanvasRestore acr(*curCanvas_, true);
    RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable::SetColorFilterModeToPaint mode:%d",
        static_cast<int32_t>(colorFilterMode));
    Drawing::Brush brush;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, brush);
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
#ifdef NEW_RENDER_CONTEXT
    RSTagTracker tagTracker(
        renderEngine->GetDrawingContext()->GetDrawingContext(),
        RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#else
    RSTagTracker tagTracker(
        renderEngine->GetRenderContext()->GetDrGPUContext(),
        RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#endif
#endif
    Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::INIT_WITH_PREVIOUS);
    canvas.SaveLayer(slr);
}

void RSDisplayRenderNodeDrawable::SetHighContrastIfEnabled(RSPaintFilterCanvas& canvas) const
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    canvas.SetHighContrast(renderEngine->IsHighContrastEnabled());
}

void RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode(
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& nodes, Drawing::Canvas& canvas) const
{
    if (!RSSystemProperties::GetHardwareComposerEnabled()) {
        RS_LOGW("RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode: \
            HardwareComposer is not enabled.");
        return;
    }

    // sort the surfaceNodes by ZOrder
    std::stable_sort(
        nodes.begin(), nodes.end(), [](const auto& first, const auto& second) -> bool {
            return first->GetGlobalZOrder() < second->GetGlobalZOrder();
        });

    // draw hardware-composition nodes
    for (auto& surfaceNode : nodes) {
        if (surfaceNode->IsLastFrameHardwareEnabled() && surfaceNode->GetBuffer() != nullptr) {
            Drawing::AutoCanvasRestore acr(canvas, true);
            std::unique_ptr<RSSurfaceRenderNodeDrawable> surfaceNodeDrawable =
                std::make_unique<RSSurfaceRenderNodeDrawable>(surfaceNode);
            surfaceNodeDrawable->OnCapture(canvas);
        }
    }
}

void RSDisplayRenderNodeDrawable::DrawWatermarkIfNeed(
    RSDisplayRenderNode& node, RSPaintFilterCanvas& canvas) const
{
    if (RSUniRenderThread::Instance().GetWatermarkFlag()) {
        sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
        auto screenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
        auto image = RSUniRenderThread::Instance().GetWatermarkImg();
        if (image == nullptr) {
            return;
        }

        auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, screenInfo.width, screenInfo.height);
        Drawing::Brush rectBrush;
        canvas.AttachBrush(rectBrush);
        canvas.DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas.DetachBrush();
    }
}
} // namespace OHOS::Rosen::DrawableV2
