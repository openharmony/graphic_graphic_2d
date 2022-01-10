/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "platform/common/rs_log.h"
#include "rs_surface_frame_ohos_raster.h"

namespace OHOS {
namespace Rosen {

RSSurfaceOhosRaster::RSSurfaceOhosRaster(const sptr<Surface>& producer) : RSSurfaceOhos(producer) {}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosRaster::RequestFrame(int32_t width, int32_t height)
{
    if (producer_ == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosRaster::RequestFrame, producer is nullptr");
        return nullptr;
    }

    std::unique_ptr<RSSurfaceFrameOhosRaster> frame = std::make_unique<RSSurfaceFrameOhosRaster>(width, height);
    SurfaceError err = producer_->RequestBuffer(frame->buffer_, frame->releaseFence_, frame->requestConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Requestframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return nullptr;
    }

    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));
    return ret;
}

bool RSSurfaceOhosRaster::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame)
{
    // RSSurfaceOhosRaster is the class for platform OHOS, the input pointer should be the pointer to the class
    // RSSurfaceFrameOhos.
    // We use static_cast instead of RTTI and dynamic_cast which are not permitted

    RSSurfaceFrameOhosRaster* oriFramePtr = static_cast<RSSurfaceFrameOhosRaster*>(frame.get());
    SurfaceError err = producer_->FlushBuffer(oriFramePtr->buffer_, oriFramePtr->releaseFence_, oriFramePtr->flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Flushframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return false;
    }

    return true;
}

} // namespace Rosen
} // namespace OHOS
