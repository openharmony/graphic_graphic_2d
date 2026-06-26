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

#include "params/rs_canvas_drawing_render_params.h"

#include "platform/common/rs_log.h"
namespace OHOS::Rosen {
RSCanvasDrawingRenderParams::RSCanvasDrawingRenderParams(NodeId id) : RSRenderParams(id) {}

void RSCanvasDrawingRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    RSRenderParams::OnSync(target);
    auto targetParams = static_cast<RSCanvasDrawingRenderParams*>(target.get());
    if (targetParams == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderParams::OnSync targetParams is nullptr");
        return;
    }
 
#ifdef RS_MODIFIERS_DRAW_ENABLE
    targetParams->bufferDraw_ = bufferDraw_;
    if (dirtyType_.test(RSRenderParamsDirtyType::BUFFER_INFO_DIRTY)) {
        targetParams->buffer_ = buffer_;
        targetParams->preBuffer_ = preBuffer_;
        targetParams->acquireFence_ = acquireFence_;
        targetParams->damageRect_ = damageRect_;
        targetParams->bufferOwnerCount_ = bufferOwnerCount_;
        targetParams->preBufferOwnerCount_ = preBufferOwnerCount_;
        if (preBufferOwnerCount_ != nullptr && !bufferSynced_ && preBufferOwnerCount_->DecRef()) {
            targetParams->preBuffer_ = nullptr;
            preBuffer_ = nullptr;
            targetParams->preBufferOwnerCount_ = nullptr;
            preBufferOwnerCount_ = nullptr;
        }
        bufferSynced_ = true;
        dirtyType_.reset(RSRenderParamsDirtyType::BUFFER_INFO_DIRTY);
    }
#endif

    if (!canvasDrawingNodeSurfaceChanged_) {
        return;
    }
    targetParams->canvasDrawingNodeSurfaceChanged_ = true;
    targetParams->surfaceParams_.width = surfaceParams_.width;
    targetParams->surfaceParams_.height = surfaceParams_.height;
    targetParams->surfaceParams_.colorSpace = surfaceParams_.colorSpace;
    targetParams->canvasDrawingResetSurfaceIndex_ = canvasDrawingResetSurfaceIndex_.load();
    if (GetParamsType() == RSRenderParamsType::RS_PARAM_OWNED_BY_DRAWABLE) {
        return;
    }
    canvasDrawingNodeSurfaceChanged_ = false;
}

void RSCanvasDrawingRenderParams::SetCanvasDrawingSurfaceChanged(bool changeFlag)
{
    if (changeFlag) {
        needSync_ = true;
    }
    canvasDrawingNodeSurfaceChanged_ = changeFlag;
}

void RSCanvasDrawingRenderParams::SetCanvasDrawingResetSurfaceIndex(uint32_t index)
{
    if (index == canvasDrawingResetSurfaceIndex_) {
        return;
    }
    canvasDrawingResetSurfaceIndex_.store(index);
    needSync_ = true;
}

void RSCanvasDrawingRenderParams::SetCanvasDrawingSurfaceParams(int width, int height, GraphicColorGamut colorSpace)
{
    surfaceParams_.width = width;
    surfaceParams_.height = height;
    surfaceParams_.colorSpace = colorSpace;
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
void RSCanvasDrawingRenderParams::SetBufferSynced(bool bufferSynced)
{
    bufferSynced_ = bufferSynced;
}
 
bool RSCanvasDrawingRenderParams::IsBufferSynced() const
{
    return bufferSynced_;
}
 
void RSCanvasDrawingRenderParams::SetBufferDraw(bool bufferDraw)
{
    bufferDraw_ = bufferDraw;
    needSync_ = true;
}
 
bool RSCanvasDrawingRenderParams::IsBufferDraw() const
{
    return bufferDraw_;
}
 
void RSCanvasDrawingRenderParams::SetBuffer(const sptr<SurfaceBuffer>& buffer,
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount, const Rect& damageRect)
{
    buffer_ = buffer;
    damageRect_ = damageRect;
    bufferOwnerCount_ = bufferOwnerCount;
    needSync_ = true;
    dirtyType_.set(RSRenderParamsDirtyType::BUFFER_INFO_DIRTY);
}
 
sptr<SurfaceBuffer> RSCanvasDrawingRenderParams::GetBuffer() const
{
    return buffer_;
}
 
const Rect& RSCanvasDrawingRenderParams::GetBufferDamage() const
{
    return damageRect_;
}
 
void RSCanvasDrawingRenderParams::SetPreBuffer(
    const sptr<SurfaceBuffer>& preBuffer, std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> preBufferOwnerCount)
{
    preBuffer_ = preBuffer;
    if (preBufferOwnerCount) {
        preBufferOwnerCount_ = preBufferOwnerCount;
    }
    needSync_ = true;
    dirtyType_.set(RSRenderParamsDirtyType::BUFFER_INFO_DIRTY);
}
 
sptr<SurfaceBuffer> RSCanvasDrawingRenderParams::GetPreBuffer()
{
    return preBuffer_;
}
 
void RSCanvasDrawingRenderParams::SetAcquireFence(const sptr<SyncFence>& acquireFence)
{
    acquireFence_ = acquireFence;
    needSync_ = true;
    dirtyType_.set(RSRenderParamsDirtyType::BUFFER_INFO_DIRTY);
}
 
sptr<SyncFence> RSCanvasDrawingRenderParams::GetAcquireFence() const
{
    return acquireFence_;
}
 
std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> RSCanvasDrawingRenderParams::GetBufferOwnerCount() const
{
    return bufferOwnerCount_;
}
 
std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> RSCanvasDrawingRenderParams::GetPreBufferOwnerCount() const
{
    return preBufferOwnerCount_;
}
#endif // RS_MODIFIERS_DRAW_ENABLE
} // namespace OHOS::Rosen