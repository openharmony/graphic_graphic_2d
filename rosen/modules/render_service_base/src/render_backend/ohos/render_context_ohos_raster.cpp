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

#include "ohos/render_context_ohos_raster.h"

#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"

#include "sync_fence.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
void RenderContextOhosRaster::Init()
{
}

RenderContextOhosRaster::~RenderContextOhosRaster()
{
    frame_ = nullptr;
}

bool RenderContextOhosRaster::CreateSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (frame == nullptr) {
        LOGE("Failed to create surface, frame is nullptr");
        return false;
    }

    frame->requestConfig_.width = frame->width_;
    frame->requestConfig_.height = frame->height_;
    frame->requestConfig_.usage = frame->bufferUsage_;
    frame->requestConfig_.format = frame->pixelFormat_;
    frame->flushConfig_.damage.w = frame->width_;
    frame->flushConfig_.damage.h = frame->height_;
    
    if (frame->producer_ == nullptr) {
        LOGE("Failed to create surface, frame_->producer is nullptr");
        return false;
    }
    SurfaceError err = frame->producer_->RequestBuffer(frame->buffer_, frame->releaseFence_, frame->requestConfig_);
    if (err != SURFACE_ERROR_OK) {
        LOGE("RenderContextOhosRaster::RequestBuffer Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return false;
    }
    if (frame->buffer_ == nullptr) {
        LOGE("Failed to create surface, frame_->producer is nullptr");
        return false;
    }
    err = frame->buffer_->Map();
    sptr<SyncFence> tempFence = new SyncFence(frame->releaseFence_);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        LOGE("Failed to create surface, this buffer is not available");
        return false;
    }
    return true;
}

void RenderContextOhosRaster::DamageFrame(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (frame == nullptr) {
        LOGE("Failed to damage frame, frame is nullptr");
        return;
    }
    std::vector<RectI> damageRects = frame->damageRects;
    if (damageRects.size() == 0) {
        LOGE("Failed to damage frame, damageRects is empty");
        return;
    }
    frame->flushConfig_.damage.x = damageRects[0].left_;
    frame->flushConfig_.damage.y = damageRects[0].top_;
    frame->flushConfig_.damage.w = damageRects[0].width_;
    frame->flushConfig_.damage.h = damageRects[0].height_;
}

void RenderContextOhosRaster::SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (frame == nullptr) {
        LOGE("Failed to swap buffers, frame is nullptr");
        return;
    }
    frame->flushConfig_.timestamp = static_cast<int64_t>(frame->uiTimestamp_);
    if (frame->producer_ == nullptr) {
        LOGE("Failed to swap buffers, frame->producer is nullptr");
        return;
    }
    SurfaceError err = frame->producer_->FlushBuffer(frame->buffer_, -1, frame->flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        LOGE("Failed to swap buffers, error is : %s", SurfaceErrorStr(err).c_str());
        return;
    }
}
}
}