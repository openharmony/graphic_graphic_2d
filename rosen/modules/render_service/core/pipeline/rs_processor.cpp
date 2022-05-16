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

#include "pipeline/rs_processor.h"

#include <ctime>
#include <sync_fence.h>
#include "rs_trace.h"

#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"

#include <platform/ohos/rs_surface_ohos.h>

namespace OHOS {
namespace Rosen {
SkCanvas* RSProcessor::CreateCanvas(
    const std::shared_ptr<RSSurfaceOhos>& surface,
    const BufferRequestConfig& requestConfig)
{
    RS_TRACE_NAME("CreateCanvas");

    if (surface == nullptr) {
        RS_LOGE("RSProcessor::CreateCanvas: surface is null!");
        return nullptr;
    }

#ifdef RS_ENABLE_GL
    if (renderContext_ != nullptr) {
        surface->SetRenderContext(renderContext_.get());
    }
#endif

    currFrame_ = surface->RequestFrame(requestConfig.width, requestConfig.height);
    if (currFrame_ == nullptr) {
        RS_LOGE("RSProcessor::CreateCanvas: requestFrame failed!");
        return nullptr;
    }
    return currFrame_->GetCanvas();
}

void RSProcessor::SetBufferTimeStamp()
{
    if (!currFrame_) {
        RS_LOGE("RSProcessor::SetBufferTimeStamp currFrame_ is nullptr");
        return;
    }
    auto frameOhosRaster =  static_cast<RSSurfaceFrameOhosRaster *>(currFrame_.get());
    if (!frameOhosRaster || !(frameOhosRaster->GetBuffer())) {
        RS_LOGE("RSProcessor::SetBufferTimeStamp buffer is nullptr");
        return;
    }
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    GSError ret = frameOhosRaster->GetBuffer()->GetExtraData()->ExtraSet("timeStamp", static_cast<int64_t>(duration));
    if (ret != GSERROR_OK) {
        RS_LOGE("RSProcessor::SetBufferTimeStamp buffer ExtraSet failed");
    }
}
} // namespace Rosen
} // namespace OHOS
