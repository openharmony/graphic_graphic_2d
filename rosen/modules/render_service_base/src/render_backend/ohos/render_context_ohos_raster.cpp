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

#include "render_backend_utils.h"

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
    if (!RenderBackendUtils::IsValidFrame(frame)) {
        LOGE("Failed to create surface, frame is invalid");
        return false;
    }

    std::shared_ptr<FrameConfig> frameConfig = frame->frameConfig;
    frameConfig->requestConfig.width = frameConfig->width;
    frameConfig->requestConfig.height = frameConfig->height;
    
    frameConfig->requestConfig.usage = frameConfig->bufferUsage;
    frameConfig->requestConfig.format = frameConfig->pixelFormat;

    frameConfig->flushConfig.damage.w = frameConfig->width;
    frameConfig->flushConfig.damage.h = frameConfig->height;

    frameConfig->releaseFence = -1;
    std::shared_ptr<SurfaceConfig> surfaceConfig = frame->surfaceConfig;
    if (surfaceConfig->producer == nullptr) {
        LOGE("Failed to create surface, producer is nullptr");
        return false;
    }

    SurfaceError err = surfaceConfig->producer->RequestBuffer(frameConfig->buffer, frameConfig->releaseFence,
        frameConfig->requestConfig);
    if (err != SURFACE_ERROR_OK) {
        LOGE("Failed to create surface, error is : %{public}s", SurfaceErrorStr(err).c_str());
        return false;
    }

    if (frameConfig->buffer == nullptr) {
        LOGE("Failed to create surface, producer is nullptr");
        return false;
    }

    err = frameConfig->buffer->Map();
    if (err != SURFACE_ERROR_OK) {
        LOGE("Failed to create surface, map failed, error is : %{public}s", SurfaceErrorStr(err).c_str());
        return false;
    }
    sptr<SyncFence> tempFence = new SyncFence(frameConfig->releaseFence);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        LOGE("Failed to create surface, this buffer is not available");
    }
    return true;
}

void RenderContextOhosRaster::DamageFrame(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (!RenderBackendUtils::IsValidFrame(frame)) {
        LOGE("Failed to damage frame, frame is invalid");
        return;
    }

    std::shared_ptr<FrameConfig> frameConfig = frame->frameConfig;
    std::vector<RectI> damageRects = frameConfig->damageRects;
    if (damageRects.size() == 0) {
        LOGE("Failed to damage frame, damageRects is empty");
        return;
    }

    frameConfig->flushConfig.damage.x = damageRects[0].left_;
    frameConfig->flushConfig.damage.y = damageRects[0].top_;
    frameConfig->flushConfig.damage.w = damageRects[0].width_;
    frameConfig->flushConfig.damage.h = damageRects[0].height_;
}

void RenderContextOhosRaster::SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (!RenderBackendUtils::IsValidFrame(frame)) {
        LOGE("Failed to swap buffers, frame is invalid");
        return;
    }

    std::shared_ptr<FrameConfig> frameConfig = frame->frameConfig;
    frameConfig->flushConfig.timestamp = static_cast<int64_t>(frameConfig->uiTimestamp);

    std::shared_ptr<SurfaceConfig> surfaceConfig = frame->surfaceConfig;
    if (surfaceConfig->producer == nullptr) {
        LOGE("Failed to swap buffers, producer is nullptr");
        return;
    }
    SurfaceError err = surfaceConfig->producer->FlushBuffer(frameConfig->buffer, -1, frameConfig->flushConfig);
    if (err != SURFACE_ERROR_OK) {
        LOGE("Failed to swap buffers, error is : %s", SurfaceErrorStr(err).c_str());
        return;
    }
    LOGD("SwapBuffers in raster successfully");
}
}
}