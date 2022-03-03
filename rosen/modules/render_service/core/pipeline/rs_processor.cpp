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

#include "unique_fd.h"
#include <sync_fence.h>

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_processor.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<SkCanvas> RSProcessor::CreateCanvas(sptr<Surface> producerSurface, BufferRequestConfig requestConfig)
{
    auto ret = producerSurface->RequestBuffer(buffer_, releaseFence_, requestConfig);
    if (ret != SURFACE_ERROR_OK || buffer_ == nullptr) {
        return nullptr;
    }
    sptr<SyncFence> tempFence = new SyncFence(releaseFence_);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        ROSEN_LOGE("RsDebug RSProcessor::CreateCanvas this buffer is not available");
        //[PLANNING]: deal with the buffer is not available
    }
    auto addr = static_cast<uint32_t*>(buffer_->GetVirAddr());
    if (addr == nullptr) {
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(buffer_->GetWidth(), buffer_->GetHeight(),
                                        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    return SkCanvas::MakeRasterDirect(info, addr, buffer_->GetStride());
}

void RSProcessor::FlushBuffer(sptr<Surface> surface, BufferFlushConfig flushConfig)
{
    if (!surface || !buffer_) {
        ROSEN_LOGE("RSProcessor::FlushBuffer surface or buffer is nullptr");
        return;
    }
    surface->FlushBuffer(buffer_, -1, flushConfig);
}

bool RSProcessor::ConsumeAndUpdateBuffer(RSSurfaceRenderNode& node, SpecialTask& task, sptr<SurfaceBuffer>& buffer)
{
    if (node.GetAvailableBufferCount() == 0 && !node.GetBuffer()) {
        ROSEN_LOGI("RsDebug RSProcessor::ProcessSurface have no Available Buffer and"\
        "Node have no buffer node id:%llu", node.GetId());
        return false;
    }
    auto& surfaceConsumer = node.GetConsumer();
    if (!surfaceConsumer) {
        ROSEN_LOGI("RSProcessor::ProcessSurface output is nullptr");
        return false;
    }
    if (node.GetAvailableBufferCount() >= 1) {
        int32_t fence = -1;
        int64_t timestamp = 0;
        Rect damage;
        auto sret = surfaceConsumer->AcquireBuffer(buffer, fence, timestamp, damage);
        if (!buffer || sret != OHOS::SURFACE_ERROR_OK) {
            ROSEN_LOGE("RSProcessor::ProcessSurface: AcquireBuffer failed!");
            return false;
        }
        task();
        node.SetBuffer(buffer);
        node.SetFence(fence);
        node.SetDamageRegion(damage);
        if (node.ReduceAvailableBuffer() >= 1) {
            if (auto mainThread = RSMainThread::Instance()) {
                mainThread->RequestNextVSync();
            }
        }
    } else {
        node.SetBuffer(node.GetBuffer());
        node.SetFence(node.GetFence());
        node.SetDamageRegion(node.GetDamageRegion());
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
