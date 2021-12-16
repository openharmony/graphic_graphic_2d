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

#include "subwindow_normal_impl.h"

#include <display_type.h>

#include "static_call.h"
#include "tester.h"
#include "window_impl.h"
#include "window_manager_hilog.h"
#include "wl_buffer_cache.h"
#include "wl_display.h"
#include "wl_dma_buffer_factory.h"
#include "wl_subsurface_factory.h"
#include "wl_surface_factory.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMSubwindowImpl"};
} // namespace

GSError SubwindowNormalImpl::CreateWlSurface(const sptr<WlSurface> &parentWlSurface)
{
    wlSurface = SingletonContainer::Get<WlSurfaceFactory>()->Create();
    if (wlSurface == nullptr) {
        WMLOGFE("WlSurfaceFactory::Create return nullptr");
        return GSERROR_API_FAILED;
    }

    auto subsurfaceFactory = SingletonContainer::Get<WlSubsurfaceFactory>();
    wlSubsurf = subsurfaceFactory->Create(wlSurface, parentWlSurface);
    if (wlSubsurf == nullptr) {
        WMLOGFE("WlSubsurf::Create return nullptr");
        return GSERROR_API_FAILED;
    }

    wlSubsurf->SetPosition(attr.GetX(), attr.GetY());
    wlSubsurf->PlaceBelow(parentWlSurface);
    wlSubsurf->SetDesync();
    return GSERROR_OK;
}

GSError SubwindowNormalImpl::CreateConsumerSurface(const sptr<SubwindowOption> &option)
{
    auto csurf2 = option->GetConsumerSurface();
    if (csurf2 != nullptr) {
        csurf = csurf2;
        WMLOGFI("use Option Surface");
    } else {
        const auto &sc = SingletonContainer::Get<StaticCall>();
        csurf = sc->SurfaceCreateSurfaceAsConsumer("Normal Subwindow");
        WMLOGFI("use Create Surface");
    }

    if (csurf == nullptr) {
        WMLOGFE("SurfaceCreateSurfaceAsConsumer return nullptr");
        return GSERROR_API_FAILED;
    }

    auto producer = csurf->GetProducer();
    psurf = SingletonContainer::Get<StaticCall>()->SurfaceCreateSurfaceAsProducer(producer);
    if (psurf == nullptr) {
        WMLOGFE("SurfaceCreateSurfaceAsProducer return nullptr");
        return GSERROR_API_FAILED;
    }

    csurf->RegisterConsumerListener(this);
    csurf->SetDefaultWidthAndHeight(attr.GetWidth(), attr.GetHeight());
    csurf->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    return GSERROR_OK;
}

GSError SubwindowNormalImpl::Init(const sptr<Window> &window, const sptr<SubwindowOption> &option)
{
    WMLOGFI("Create Normal Subwindow");

    auto windowImpl = static_cast<WindowImpl *>(window.GetRefPtr());
    if (windowImpl == nullptr) {
        WMLOGFE("WindowImpl is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (option == nullptr) {
        WMLOGFE("option is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    attr.SetWidthHeight(option->GetWidth(), option->GetHeight());
    attr.SetXY(option->GetX(), option->GetY());

    auto wret = CreateWlSurface(windowImpl->GetWlSurface());
    if (wret != GSERROR_OK) {
        return wret;
    }

    wret = CreateConsumerSurface(option);
    if (wret != GSERROR_OK) {
        return wret;
    }

    WMLOGFI("Create Normal Subwindow Success");
    return GSERROR_OK;
}

sptr<Surface> SubwindowNormalImpl::GetSurface() const
{
    return psurf;
}

GSError SubwindowNormalImpl::Move(int32_t x, int32_t y)
{
    WMLOGFI("(subwindow normal) x: %{public}d, y: %{public}d", x, y);
    std::lock_guard<std::mutex> lock(publicMutex);
    if (isDestroy == true) {
        WMLOGFI("object destroyed");
        return GSERROR_DESTROYED_OBJECT;
    }

    attr.SetXY(x, y);
    wlSubsurf->SetPosition(attr.GetX(), attr.GetY());
    return GSERROR_OK;
}

GSError SubwindowNormalImpl::Resize(uint32_t width, uint32_t height)
{
    WMLOGFI("(subwindow normal)%{public}u x %{public}u", width, height);
    std::lock_guard<std::mutex> lock(publicMutex);
    if (isDestroy == true) {
        WMLOGFI("object destroyed");
        return GSERROR_DESTROYED_OBJECT;
    }

    attr.SetWidthHeight(width, height);
    wlSurface->Commit();
    return GSERROR_OK;
}

GSError SubwindowNormalImpl::Destroy()
{
    WMLOGFI("(subwindow normal) Destroy");
    std::lock_guard<std::mutex> lock(publicMutex);
    Tester::Get().ScheduleForConcurrent();
    isDestroy = true;
    csurf = nullptr;
    psurf = nullptr;
    wlSubsurf = nullptr;
    wlSurface = nullptr;
    return GSERROR_OK;
}

void SubwindowNormalImpl::OnPositionChange(WindowPositionChangeFunc func)
{
    std::lock_guard<std::mutex> lock(publicMutex);
    attr.OnPositionChange(func);
}

void SubwindowNormalImpl::OnSizeChange(WindowSizeChangeFunc func)
{
    std::lock_guard<std::mutex> lock(publicMutex);
    attr.OnSizeChange(func);
}

namespace {
void BufferRelease(struct wl_buffer *wbuffer, int32_t fence)
{
    WMLOGFI("(subwindow normal) BufferRelease");
    sptr<Surface> surf = nullptr;
    sptr<SurfaceBuffer> sbuffer = nullptr;
    if (SingletonContainer::Get<WlBufferCache>()->GetSurfaceBuffer(wbuffer, surf, sbuffer)) {
        if (surf != nullptr && sbuffer != nullptr) {
            surf->ReleaseBuffer(sbuffer, fence);
        }
    }
}
} // namespace

void SubwindowNormalImpl::OnBufferAvailable()
{
    WMLOGFI("(subwindow normal) OnBufferAvailable enter");
    std::lock_guard<std::mutex> lock(publicMutex);
    if (isDestroy == true) {
        WMLOGFI("object destroyed");
        return;
    }

    if (csurf == nullptr || wlSurface == nullptr) {
        WMLOGFE("csurf or wlSurface is nullptr");
        return;
    }

    sptr<SurfaceBuffer> sbuffer = nullptr;
    int32_t flushFence = -1;
    int64_t timestamp = 0;
    Rect damage = {};
    GSError ret = csurf->AcquireBuffer(sbuffer, flushFence, timestamp, damage);
    if (ret != GSERROR_OK) {
        WMLOGFE("AcquireBuffer failed");
        return;
    }

    auto bc = SingletonContainer::Get<WlBufferCache>();
    auto wbuffer = bc->GetWlBuffer(csurf, sbuffer);
    if (wbuffer == nullptr) {
        auto dmaBufferFactory = SingletonContainer::Get<WlDMABufferFactory>();
        auto dmaWlBuffer = dmaBufferFactory->Create(sbuffer->GetBufferHandle());
        if (dmaWlBuffer == nullptr) {
            WMLOGFE("Create DMA Buffer Failed");
            auto sret = csurf->ReleaseBuffer(sbuffer, -1);
            if (sret != GSERROR_OK) {
                WMLOGFW("ReleaseBuffer failed");
            }
            return;
        }
        dmaWlBuffer->OnRelease(BufferRelease);

        wbuffer = dmaWlBuffer;
        bc->AddWlBuffer(wbuffer, csurf, sbuffer);
    }

    SendBufferToServer(wbuffer, sbuffer, flushFence, damage);
    WMLOGFI("(subwindow normal) OnBufferAvailable exit");
}

void SubwindowNormalImpl::SendBufferToServer(sptr<WlBuffer> &wbuffer,
    sptr<SurfaceBuffer> &sbuffer, int32_t fence, Rect &damage)
{
    if (wbuffer) {
        auto br = wlSurface->GetBufferRelease();
        wbuffer->SetBufferRelease(br);
        wlSurface->Attach(wbuffer, 0, 0);
        wlSurface->SetAcquireFence(fence);
        wlSurface->Damage(damage.x, damage.y, damage.w, damage.h);
        wlSurface->SetSource(0, 0, sbuffer->GetWidth(), sbuffer->GetHeight());
        wlSurface->SetDestination(attr.GetWidth(), attr.GetHeight());
        WMLOGFI("(subwindow normal) Source[%{public}d x %{public}d] Dest[%{public}d x %{public}d]",
                sbuffer->GetWidth(), sbuffer->GetHeight(), attr.GetWidth(), attr.GetHeight());
        wlSurface->Commit();
        SingletonContainer::Get<WlDisplay>()->Flush();
    }
}
} // namespace OHOS
