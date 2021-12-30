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

#include "rs_surface_frame_ohos.h"
#include "rs_surface_ohos.h"

namespace OHOS {
namespace Rosen {

RSSurfaceOhos::RSSurfaceOhos(const sptr<Surface>& producer) : producer_(producer) {}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhos::RequestFrame(int32_t width, int32_t height)
{
    if (producer_ == nullptr) {
        return nullptr;
    }
    std::unique_ptr<RSSurfaceFrameOhos> frame = std::make_unique<RSSurfaceFrameOhos>(width, height);
    SurfaceError err = producer_->RequestBuffer(frame->buffer_, frame->releaseFence_, frame->requestConfig_);
    if (err != SURFACE_ERROR_OK) {
        return nullptr;
    }
    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));
    return ret;
}

bool RSSurfaceOhos::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame)
{
    // RSSurfaceOhos is the class for platform OHOS, the input pointer should be the pointer to the class RSSurfaceFrameOhos.
    // We use static_cast instead of RTTI and dynamic_cast which are not permitted
    RSSurfaceFrameOhos* oriFramePtr = static_cast<RSSurfaceFrameOhos*>(frame.get());
    SurfaceError ret = producer_->FlushBuffer(oriFramePtr->buffer_, oriFramePtr->releaseFence_, oriFramePtr->flushConfig_);
    if (ret != SURFACE_ERROR_OK) {
        return false;
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS
