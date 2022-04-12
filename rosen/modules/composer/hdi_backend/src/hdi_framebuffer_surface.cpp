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

#include "hdi_framebuffer_surface.h"

#include "hdi_log.h"

#include <drawing_surface/rs_surface_ohos.h>

using namespace OHOS;

namespace OHOS {
namespace Rosen {

HdiFramebufferSurface::HdiFramebufferSurface()
{
}

HdiFramebufferSurface::~HdiFramebufferSurface()
{
}

sptr<HdiFramebufferSurface> HdiFramebufferSurface::CreateFramebufferSurface()
{
    sptr<HdiFramebufferSurface> fbSurface = new HdiFramebufferSurface();

    SurfaceError ret = fbSurface->CreateSurface(fbSurface);
    if (ret != SURFACE_ERROR_OK) {
        HLOGE("FramebufferSurface CreateSurface failed, ret is %{public}d", ret);
        return nullptr;
    }

    ret = fbSurface->SetBufferQueueSize(MAX_BUFFER_SIZE);
    if (ret != SURFACE_ERROR_OK) {
        HLOGE("FramebufferSurface SetBufferQueueSize failed, ret is %{public}d", ret);
        return nullptr;
    }

    return fbSurface;
}

SurfaceError HdiFramebufferSurface::CreateSurface(sptr<HdiFramebufferSurface> &fbSurface)
{
    consumerSurface_ = Surface::CreateSurfaceAsConsumer();

    sptr<IBufferProducer> producer = consumerSurface_->GetProducer();
    producerSurface_ = Surface::CreateSurfaceAsProducer(producer);

    rsSurface_ = RSSurfaceOhos::CreateSurface(producerSurface_);

    sptr<IBufferConsumerListener> listener = fbSurface;
    SurfaceError ret = consumerSurface_->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        return SURFACE_ERROR_NO_CONSUMER;
    }

    return SURFACE_ERROR_OK;
}

SurfaceError HdiFramebufferSurface::SetBufferQueueSize(uint32_t bufferSize)
{
    SurfaceError ret = consumerSurface_->SetQueueSize(bufferSize);
    if (ret != SURFACE_ERROR_OK) {
        HLOGE("fb SetQueueSize failed, ret is %{public}d", ret);
        return ret;
    }

    return SURFACE_ERROR_OK;
}

void HdiFramebufferSurface::OnBufferAvailable()
{
    // check, how to use timestamp and damage
    oldBuffer_ = currentBuffer_;
    int64_t timestamp;
    Rect damage;
    int32_t fbAcquireFence = -1;
    SurfaceError ret = consumerSurface_->AcquireBuffer(currentBuffer_, fbAcquireFence,
                                                       timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        HLOGE("AcquireBuffer failed, ret is %{public}d", ret);
        return;
    }

    fbAcquireFence_ = new SyncFence(fbAcquireFence);
}

std::shared_ptr<RSSurface> HdiFramebufferSurface::GetSurface()
{
    return rsSurface_;
}

sptr<SurfaceBuffer> HdiFramebufferSurface::GetFramebuffer()
{
    return currentBuffer_;
}

sptr<SyncFence> HdiFramebufferSurface::GetFramebufferFence()
{
    return fbAcquireFence_;
}

int32_t HdiFramebufferSurface::ReleaseFramebuffer(const sptr<SyncFence> &releaseFence)
{
    if (oldBuffer_ == nullptr) {
        return SURFACE_ERROR_OK;
    }

    if (releaseFence == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    int32_t fenceFd = releaseFence->Dup();
    SurfaceError ret = consumerSurface_->ReleaseBuffer(oldBuffer_, fenceFd);
    if (ret != SURFACE_ERROR_OK) {
        HLOGE("ReleaseBuffer failed ret is %{public}d", ret);
    }

    oldBuffer_ = nullptr;

    return ret;
}

} // namespace Rosen
} // namespace OHOS
