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

#include "subwindow_offscreen_impl.h"

#include <fence.h>
#include <gslogger.h>

#include "static_call.h"

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("SubwindowOffscreenImpl");
} // namespace

GSError SubwindowOffscreenImpl::CreateConsumerSurface(const sptr<SubwindowOption> &option)
{
    auto csurf2 = option->GetConsumerSurface();
    if (csurf2 != nullptr) {
        csurf = csurf2;
        GSLOG2HI(INFO) << "use Option Surface";
    } else {
        const auto &sc = SingletonContainer::Get<StaticCall>();
        csurf = sc->SurfaceCreateEglSurfaceAsConsumer("Offscreen Subwindow");
        GSLOG2HI(INFO) << "use Create Surface";
    }

    if (csurf == nullptr) {
        GSLOG2HI(ERROR) << "SurfaceCreateEglSurfaceAsConsumer return nullptr";
        return GSERROR_API_FAILED;
    }

    auto producer = csurf->GetProducer();
    psurf = SingletonContainer::Get<StaticCall>()->SurfaceCreateSurfaceAsProducer(producer);
    if (psurf == nullptr) {
        GSLOG2HI(ERROR) << "SurfaceCreateSurfaceAsProducer return nullptr";
        return GSERROR_API_FAILED;
    }

    csurf->RegisterConsumerListener(this);
    csurf->SetDefaultWidthAndHeight(attr.GetWidth(), attr.GetHeight());
    csurf->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    return GSERROR_OK;
}

void SubwindowOffscreenImpl::OnBufferAvailable()
{
    GSLOG2HI(INFO) << "(subwindow offscreen) " << "enter";
    std::lock_guard<std::mutex> lock(publicMutex);
    if (isDestroy == true) {
        GSLOG2HI(ERROR) << "(subwindow offscreen) " << "object destroyed";
        return;
    }

    if (csurf == nullptr || wlSurface == nullptr) {
        GSLOG2HI(ERROR) << "(subwindow offscreen) " << "csurf or wlSurface is nullptr";
        return;
    }

    sptr<SurfaceBuffer> sbuffer = nullptr;
    int32_t flushFence = -1;
    int64_t timestamp = 0;
    Rect damage = {};
    GSError ret = csurf->AcquireBuffer(sbuffer, flushFence, timestamp, damage);
    if (ret != GSERROR_OK) {
        GSLOG2HI(ERROR) << "(subwindow offscreen) " << "AcquireBuffer failed";
        return;
    }

    auto bc = SingletonContainer::Get<WlBufferCache>();
    auto wbuffer = bc->GetWlBuffer(csurf, sbuffer);
    if (wbuffer == nullptr) {
        auto dmaBufferFactory = SingletonContainer::Get<WlDMABufferFactory>();
        auto dmaWlBuffer = dmaBufferFactory->Create(sbuffer->GetBufferHandle());
        if (dmaWlBuffer == nullptr) {
            GSLOG2HI(ERROR) << "(subwindow offscreen) " << "Create DMA Buffer Failed";
            auto sret = csurf->ReleaseBuffer(sbuffer, -1);
            if (sret != GSERROR_OK) {
                GSLOG2HI(ERROR) << "(subwindow offscreen) " << "ReleaseBuffer failed";
            }
            return;
        }
        dmaWlBuffer->OnRelease(this);

        wbuffer = dmaWlBuffer;
        bc->AddWlBuffer(wbuffer, csurf, sbuffer);
    }

    int32_t onFrameAvailableFence = GLOperation(sbuffer);
    flushFence = FenceMerge("merge", onFrameAvailableFence, flushFence);

    SendBufferToServer(wbuffer, sbuffer, flushFence, damage);
    GSLOG2HI(INFO) << "(subwindow offscreen) " << "exit";
}

int32_t SubwindowOffscreenImpl::GLOperation(sptr<SurfaceBuffer> &sbuffer)
{
    auto eglData = sbuffer->GetEglData();
    if (eglData == nullptr) {
        GSLOG2HI(WARN) << "(subwindow offscreen) eglData is nullptr, may skip OnFrameAvailable";
        return -1;
    }

    int32_t fd = -1;
    if (onFrameAvailable) {
        GSLOG2HI(INFO) << "OnFrameAvailable begin " << eglData->GetTexture();
        fd = onFrameAvailable(sbuffer);
        GSLOG2HI(INFO) << "OnFrameAvailable end";
    }
    return fd;
}

GSError SubwindowOffscreenImpl::OnFrameAvailable(FrameAvailableFunc func)
{
    onFrameAvailable = func;
    return GSERROR_OK;
}
} // namespace OHOS
