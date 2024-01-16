/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "surface_ohos_raster.h"

#include "sync_fence.h"
#include "surface_frame_ohos_raster.h"
#include "drawing_utils.h"

namespace OHOS {
namespace Rosen {
SurfaceOhosRaster::SurfaceOhosRaster(const sptr<Surface>& producer)
    : SurfaceOhos(producer), frame_(nullptr)
{
}

SurfaceOhosRaster::~SurfaceOhosRaster()
{
    frame_ = nullptr;
}


std::unique_ptr<SurfaceFrame> SurfaceOhosRaster::NativeRequestFrame(int32_t width, int32_t height)
{
    return RequestFrame(width, height);
}


bool SurfaceOhosRaster::NativeFlushFrame(std::unique_ptr<SurfaceFrame>& frame)
{
    return FlushFrame(frame);
}

std::unique_ptr<SurfaceFrame> SurfaceOhosRaster::RequestFrame(int32_t width, int32_t height)
{
    if (IsValid() == false) {
        LOGE("SurfaceOhosRaster::RequestFrame, producer is nullptr");
        return nullptr;
    }
    frame_ = std::make_unique<SurfaceFrameOhosRaster>(width, height);
    SurfaceError err = producer_->RequestBuffer(frame_->buffer_, frame_->releaseFence_, frame_->requestConfig_);
    if (err != SURFACE_ERROR_OK) {
        LOGE("SurfaceOhosRaster::Requestframe Failed, error is : %{public}s", SurfaceErrorStr(err).c_str());
        return nullptr;
    }
    sptr<SyncFence> tempFence = new SyncFence(frame_->releaseFence_);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        LOGE("RequestFrame this buffer is not available");
    }

    LOGD("SurfaceOhosRaster RequestFrame successfully!, buffer width is %{public}d, height is %{public}d",
        frame_->buffer_->GetWidth(), frame_->buffer_->GetHeight());

    std::unique_ptr<SurfaceFrame> ret(std::move(frame_));
    return ret;
}

bool SurfaceOhosRaster::FlushFrame(std::unique_ptr<SurfaceFrame>& frame)
{
    // SurfaceOhosRaster is the class for platform OHOS, the input pointer should be the pointer to the class
    // SurfaceFrameOhos.
    // We use static_cast instead of RTTI and dynamic_cast which are not permitted

    SurfaceFrameOhosRaster* oriFramePtr = static_cast<SurfaceFrameOhosRaster*>(frame.get());
    SurfaceError err = producer_->FlushBuffer(oriFramePtr->buffer_, -1, oriFramePtr->flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        LOGE("SurfaceOhosRaster::Flushframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return false;
    }
    LOGE("SurfaceOhosRaster::FlushFrame fence:%d", oriFramePtr->releaseFence_);
    return true;
}

SkCanvas* SurfaceOhosRaster::GetSkCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not GetCanvas");
        return nullptr;
    }
    return drawingProxy_->AcquireSkCanvas(frame);
}
} // namespace Rosen
} // namespace OHOS