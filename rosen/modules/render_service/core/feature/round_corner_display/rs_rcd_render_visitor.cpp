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
#include "rs_rcd_render_listener.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSRcdRenderVisitor::RSRcdRenderVisitor()
{
    renderEngine_ = RSUniRenderThread::Instance().GetRenderEngine();
}

bool RSRcdRenderVisitor::ConsumeAndUpdateBuffer(RSRcdSurfaceRenderNode& node)
{
    auto availableBufferCnt = node.GetAvailableBufferCount();
    if (availableBufferCnt <= 0) {
        // this node has no new buffer, try use old buffer.
        RS_LOGI("RSRcdRenderVisitor no availablebuffer(%{public}d) use old buffer!", availableBufferCnt);
        return true;
    }

    auto consumer = node.GetConsumer();
    if (consumer == nullptr) {
        RS_LOGE("RSRcdRenderVisitor no consumer is null!");
        return false;
    }

    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    int64_t timestamp = 0;
    Rect damage;
    auto ret = consumer->AcquireBuffer(buffer, acquireFence, timestamp, damage);
    if (buffer == nullptr || ret != SURFACE_ERROR_OK) {
        RS_LOGE("RsDebug RSRcdSurfaceRenderNode(id: %{public}" PRIu64 ") AcquireBuffer failed(ret: %{public}d)!",
            node.GetNodeId(), ret);
        return false;
    }

    node.SetBuffer(buffer, acquireFence, damage, timestamp);

    if (!node.SetHardwareResourceToBuffer()) {
        RS_LOGE("RSRcdRenderVisitor SetHardwareResourceToBuffer Failed!");
        return false;
    }

    node.SetCurrentFrameBufferConsumed();
    node.ReduceAvailableBuffer();
    return true;
}

void RSRcdRenderVisitor::ProcessRcdSurfaceRenderNodeMainThread(RSRcdSurfaceRenderNode& node, bool resourceChanged)
{
    if (uniProcessor_ == nullptr || node.IsInvalidSurface() || resourceChanged) {
        RS_LOGE("RSRcdRenderVisitor RSProcessor null, node invalid, resource changed %{public}d %{public}d %{public}d",
            static_cast<int>(uniProcessor_ == nullptr), static_cast<int>(node.IsInvalidSurface()),
            static_cast<int>(resourceChanged));
        return;
    }

    sptr<SurfaceBuffer> buffer = node.GetBuffer();
    if (buffer != nullptr) {
        uniProcessor_->ProcessRcdSurface(node);
        return;
    }
    RS_LOGD_IF(DEBUG_PIPELINE, "RSRcdRenderVisitor node buffer is null!");
}

bool RSRcdRenderVisitor::ProcessRcdSurfaceRenderNode(
    RSRcdSurfaceRenderNode &node, const std::shared_ptr<rs_rcd::RoundCornerLayer> &layerInfo, bool resourceChanged)
{
    std::lock_guard<std::mutex> lock(bufferMut_);
    if (uniProcessor_ == nullptr || node.IsInvalidSurface() || renderEngine_ == nullptr) {
        RS_LOGE("RSRcdRenderVisitor ProcessRcd NG %{public}d %{public}d %{public}d", static_cast<int>(uniProcessor_ ==
            nullptr), static_cast<int>(node.IsInvalidSurface()), static_cast<int>(renderEngine_ == nullptr));
        return false;
    }

    sptr<SurfaceBuffer> buffer = node.GetBuffer();
    if (!resourceChanged && buffer != nullptr) {
        uniProcessor_->ProcessRcdSurface(node);
        return true;
    }

    auto surfaceNodePtr = node.ReinterpretCastTo<RSRcdSurfaceRenderNode>();
    if (surfaceNodePtr == nullptr || (!node.IsSurfaceCreated())) {
        sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNodePtr);
        if (listener == nullptr || (!node.CreateSurface(listener))) {
            RS_LOGE("RSRcdRenderVisitor::RenderExpandedFrame CreateSurface failed");
            return false;
        }
    }

    auto rsSurface = std::static_pointer_cast<RSSurfaceOhos>(node.GetRSSurface());
    if (rsSurface == nullptr) {
        RS_LOGE("RSRcdRenderVisitor::RenderExpandedFrame no RSSurface found");
        return false;
    }

    if (layerInfo == nullptr || (!node.PrepareHardwareResourceBuffer(layerInfo))) {
        RS_LOGE("PrepareHardwareResourceBuffer is wrong");
        return false;
    }

    rsSurface->SetTimeOut(node.GetHardenBufferRequestConfig().timeout);
    auto renderFrame = renderEngine_->RequestFrame(rsSurface,
        node.GetHardenBufferRequestConfig(), true, false);
    if (renderFrame == nullptr) {
        rsSurface->GetSurface()->CleanCache(true);
        RS_LOGE("RSRcdRenderVisitor Request Frame Failed");
        return false;
    }
    renderFrame->Flush();
    if (!ConsumeAndUpdateBuffer(node)) {
        RS_LOGE("RSRcdRenderVisitor ConsumeAndUpdateBuffer Failed");
        return false;
    }
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    if (node.GetConsumer() && node.GetBuffer()) {
        node.GetConsumer()->SetScalingMode(node.GetBuffer()->GetSeqNum(), scalingMode);
    }

    uniProcessor_->ProcessRcdSurface(node);
    node.PrintRcdNodeInfo();
    return true;
}

void RSRcdRenderVisitor::SetUniProcessor(std::shared_ptr<RSProcessor> processor)
{
    uniProcessor_ = processor;
}
} // namespace Rosen
} // namespace OHOS