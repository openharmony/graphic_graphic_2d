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

#include "rs_trace.h"

#include "common/rs_optional_trace.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node_allocator.h"
#include "platform/common/rs_log.h"
#include "utils/rect.h"
#include "utils/region.h"
#include "include/gpu/vk/GrVulkanTrackerInterface.h"
#include "rs_root_render_node_drawable.h"

#ifdef SUBTREE_PARALLEL_ENABLE
#include "rs_parallel_manager.h"
#endif
namespace OHOS::Rosen::DrawableV2 {
RSCanvasRenderNodeDrawable::Registrar RSCanvasRenderNodeDrawable::instance_;

RSCanvasRenderNodeDrawable::RSCanvasRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSCanvasRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    auto generator = [] (std::shared_ptr<const RSRenderNode> node,
        RSRenderNodeAllocator::DrawablePtr front) -> RSRenderNodeAllocator::DrawablePtr {
            if (front != nullptr) {
                return new (front)RSCanvasRenderNodeDrawable(std::move(node));
            }
            return new RSCanvasRenderNodeDrawable(std::move(node));
    };
    return RSRenderNodeAllocator::Instance().CreateRSRenderNodeDrawable(node, generator);
}

#ifdef SUBTREE_PARALLEL_ENABLE
bool RSCanvasRenderNodeDrawable::QuickGetDrawState(Drawing::Canvas& canvas)
{
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas->IsQuickGetDrawState()) {
        return false;
    }
    Drawing::Rect bounds = GetRenderParams() ? GetRenderParams()->GetFrameRect() : Drawing::Rect(0, 0, 0, 0);
    if (SkipCulledNodeOrEntireSubtree(canvas, bounds)) {
        return true;
    }

    RSParallelManager::Singleton().OnQuickDraw(this, canvas);
    return true;
}
#endif

/*
 * This function will be called recursively many times, and the logic should be as concise as possible.
 */
void RSCanvasRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
#ifdef RS_ENABLE_GPU
    SetDrawSkipType(DrawSkipType::NONE);
    bool shouldPaint = ShouldPaint() || (canvas.GetUICapture() && RSUniRenderThread::IsEndNodeIdValid());
    if (!shouldPaint) {
        SetDrawSkipType(DrawSkipType::SHOULD_NOT_PAINT);
        return;
    }
    const auto& params = GetRenderParams();
    if (params == nullptr) {
        SetDrawSkipType(DrawSkipType::RENDER_PARAMS_NULL);
        return;
    }
    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (params->GetStartingWindowFlag() && paintFilterCanvas) { // do not draw startingwindows in subthread
        if (paintFilterCanvas->GetIsParallelCanvas()) {
            SetDrawSkipType(DrawSkipType::PARALLEL_CANVAS_SKIP);
            return;
        }
    }

    auto linkedDrawable = std::static_pointer_cast<RSRootRenderNodeDrawable>(
        params->GetLinkedRootNodeDrawable().lock());
    auto isOpincDraw = GetOpincDrawCache().PreDrawableCacheState(*params, isOpincDropNodeExt_);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
    float hdrBrightness = paintFilterCanvas->GetHDRBrightness();
    paintFilterCanvas->SetHDRBrightness(params->GetHDRBrightness());
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    SetOcclusionCullingEnabled((!uniParam || uniParam->IsOpDropped()) && GetOpDropped() &&
        isOpincDraw && !params->HasUnobscuredUEC() && LIKELY(linkedDrawable == nullptr));
    if (IsOcclusionCullingEnabled() && QuickReject(canvas, params->GetLocalDrawRect())) {
        SetDrawSkipType(DrawSkipType::OCCLUSION_SKIP);
        return;
    }

#ifdef SUBTREE_PARALLEL_ENABLE
    if (QuickGetDrawState(canvas)) {
        return;
    }
#endif

    RSRenderNodeSingleDrawableLocker singleLocker(this);
    if (UNLIKELY(!singleLocker.IsLocked())) {
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
        RS_LOGE("RSCanvasRenderNodeDrawable::OnDraw node %{public}" PRIu64 " onDraw!!!", GetId());
        if (RSSystemProperties::GetSingleDrawableLockerEnabled()) {
            SetDrawSkipType(DrawSkipType::MULTI_ACCESS);
            return;
        }
    }

    // [Attention] Only used in PC window resize scene now
    if (UNLIKELY(linkedDrawable != nullptr)) {
        linkedDrawable->DrawWindowKeyFrameOffscreenBuffer(*paintFilterCanvas, params->GetFrameRect(),
            params->GetAlpha(), params->GetRSFreezeFlag());
        return;
    }

    if (LIKELY(isDrawingCacheEnabled_)) {
        GetOpincDrawCache().BeforeDrawCache(canvas, *params, isOpincDropNodeExt_);
        if (!drawBlurForCache_) {
            GenerateCacheIfNeed(canvas, *params);
        }
        CheckCacheTypeAndDraw(canvas, *params);
        GetOpincDrawCache().AfterDrawCache(canvas, *params, isOpincDropNodeExt_, opincRootTotalCount_);
        GetOpincDrawCache().DrawOpincDisabledDfx(canvas, *params);
    } else {
        RSRenderNodeDrawable::OnDraw(canvas);
    }
    paintFilterCanvas->SetHDRBrightness(hdrBrightness);
    RSRenderNodeDrawable::ProcessedNodeCountInc();
#endif
}

/*
 * This function will be called recursively many times, and the logic should be as concise as possible.
 */
void RSCanvasRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
#ifdef RS_ENABLE_GPU
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    bool shouldPaint = ShouldPaint();
    if (canvas.GetUICapture() && captureParam.endNodeId_ != INVALID_NODEID) {
        shouldPaint = true;
    }
    if (!shouldPaint) {
        return;
    }
    const auto& params = GetRenderParams();
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
    if (!RSUiFirstProcessStateCheckerHelper::CheckMatchAndWaitNotify(*params, false)) {
        SetDrawSkipType(DrawSkipType::CHECK_MATCH_AND_WAIT_NOTIFY_FAIL);
        RS_LOGE("RSCanvasRenderNodeDrawable::OnCapture CheckMatchAndWaitNotify failed");
        return;
    }
    RSRenderNodeSingleDrawableLocker singleLocker(this);
    if (UNLIKELY(!singleLocker.IsLocked())) {
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
        RS_LOGE("RSCanvasRenderNodeDrawable::OnCapture node %{public}" PRIu64 " onDraw!!!", GetId());
        if (RSSystemProperties::GetSingleDrawableLockerEnabled()) {
            return;
        }
    }
    if (RSRenderNodeDrawable::DealWithWhiteListNodes(canvas)) {
        return;
    }
    bool stopDrawForRangeCapture = (canvas.GetUICapture() &&
        captureParam.endNodeId_ == GetId() &&
        captureParam.endNodeId_ != INVALID_NODEID);
    if (captureParam.isSoloNodeUiCapture_ || stopDrawForRangeCapture) {
        RSRenderNodeDrawable::OnDraw(canvas);
        return;
    }
    if (LIKELY(isDrawingCacheEnabled_)) {
        if (canvas.GetUICapture() && !drawBlurForCache_) {
            GenerateCacheIfNeed(canvas, *params);
        }
        CheckCacheTypeAndDraw(canvas, *params, true);
    } else {
        RSRenderNodeDrawable::OnDraw(canvas);
    }
    RSRenderNodeDrawable::SnapshotProcessedNodeCountInc();
#endif
}
} // namespace OHOS::Rosen::DrawableV2
