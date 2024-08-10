/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_surface_ohos_raster.h"

#include <sync_fence.h>

#include "platform/common/rs_log.h"
#include "rs_surface_frame_ohos_raster.h"

namespace OHOS {
namespace Rosen {

RSSurfaceOhosRaster::RSSurfaceOhosRaster(const sptr<Surface>& producer) : RSSurfaceOhos(producer) {}


void RSSurfaceOhosRaster::SetSurfaceBufferUsage(uint64_t usage)
{
    bufferUsage_ = usage;
}

void RSSurfaceOhosRaster::SetSurfacePixelFormat(int32_t pixelFormat)
{
    pixelFormat_ = pixelFormat;
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosRaster::RequestFrame(int32_t width, int32_t height,
    uint64_t uiTimestamp, bool useAFBC, bool isProtected)
{
    if (producer_ == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosRaster::RequestFrame, producer is nullptr");
        return nullptr;
    }

    std::unique_ptr<RSSurfaceFrameOhosRaster> frame = std::make_unique<RSSurfaceFrameOhosRaster>(width, height);
    frame->requestConfig_.usage = bufferUsage_;
    frame->requestConfig_.format = pixelFormat_;
    SurfaceError err = producer_->RequestBuffer(frame->buffer_, frame->releaseFence_, frame->requestConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Requestframe Failed, error is : %{public}s", SurfaceErrorStr(err).c_str());
        return nullptr;
    }

    err = frame->buffer_->Map();
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Map Failed, error is : %{public}s", SurfaceErrorStr(err).c_str());
        return nullptr;
    }

    sptr<SyncFence> tempFence = new SyncFence(frame->releaseFence_);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        ROSEN_LOGE("RsDebug RSProcessor::RequestFrame this buffer is not available");
    }
    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));
    return ret;
}

void RSSurfaceOhosRaster::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    auto frameOhosRaster =  static_cast<RSSurfaceFrameOhosRaster *>(frame.get());
    if (frameOhosRaster == nullptr || frameOhosRaster->GetBuffer() == nullptr) {
        RS_LOGE("RSSurfaceOhosRaster::SetUiTimeStamp: buffer is nullptr");
        return;
    }

    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    GSError ret = frameOhosRaster->GetBuffer()->GetExtraData()->ExtraSet("timeStamp", static_cast<int64_t>(duration));
    if (ret != GSERROR_OK) {
        RS_LOGE("RSSurfaceOhosRaster::SetUiTimeStamp buffer ExtraSet failed");
    }
}

bool RSSurfaceOhosRaster::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    // RSSurfaceOhosRaster is the class for platform OHOS, the input pointer should be the pointer to the class
    // RSSurfaceFrameOhos.
    // We use static_cast instead of RTTI and dynamic_cast which are not permitted
    if (!frame) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Flushframe Failed, frame is nullptr");
        return false;
    }
    RSSurfaceFrameOhosRaster* oriFramePtr = static_cast<RSSurfaceFrameOhosRaster*>(frame.get());
    oriFramePtr->flushConfig_.timestamp = static_cast<int64_t>(uiTimestamp);
    SurfaceError err = producer_->FlushBuffer(oriFramePtr->buffer_, -1, oriFramePtr->flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Flushframe Failed, error is : %{public}s", SurfaceErrorStr(err).c_str());
        return false;
    }
    ROSEN_LOGD("RsDebug RSSurfaceOhosRaster::FlushFrame fence:%{public}d", oriFramePtr->releaseFence_);
    return true;
}

void RSSurfaceOhosRaster::ClearBuffer()
{
    if (producer_ != nullptr) {
        ROSEN_LOGD("RSSurfaceOhosRaster: Clear surface buffer!");
        producer_->GoBackground();
    }
}

void RSSurfaceOhosRaster::ResetBufferAge()
{
    ROSEN_LOGD("RSSurfaceOhosRaster: Reset Buffer Age!");
}
} // namespace Rosen
} // namespace OHOS
