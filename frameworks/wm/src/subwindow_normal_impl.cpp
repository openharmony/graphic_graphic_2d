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

WMError SubwindowNormalImpl::CheckAndNew(sptr<SubwindowNormalImpl> &si,
                                         const sptr<Window> &window,
                                         const sptr<SubwindowOption> &option,
                                         sptr<WlSurface> &parent)
{
    if (option == nullptr) {
        WMLOGFE("SubwindowOption is nullptr");
        return WM_ERROR_NULLPTR;
    }

    auto windowImpl = static_cast<WindowImpl *>(window.GetRefPtr());
    if (windowImpl == nullptr) {
        WMLOGFE("WindowImpl is nullptr");
        return WM_ERROR_NULLPTR;
    }
    parent = windowImpl->GetWlSurface();

    si = TESTER_NEW(SubwindowNormalImpl);
    if (si == nullptr) {
        WMLOGFE("new SubwindowNormalImpl return nullptr");
        return WM_ERROR_NEW;
    }
    return WM_OK;
}

WMError SubwindowNormalImpl::CreateWlSurface(sptr<SubwindowNormalImpl> &si,
                                             const sptr<WlSurface> &parentWlSurface)
{
    si->wlSurface = SingletonContainer::Get<WlSurfaceFactory>()->Create();
    if (si->wlSurface == nullptr) {
        WMLOGFE("WlSurfaceFactory::Create return nullptr");
        return WM_ERROR_API_FAILED;
    }

    auto subsurfaceFactory = SingletonContainer::Get<WlSubsurfaceFactory>();
    si->wlSubsurface = subsurfaceFactory->Create(si->wlSurface, parentWlSurface);
    if (si->wlSubsurface == nullptr) {
        WMLOGFE("WlSubsurface::Create return nullptr");
        return WM_ERROR_API_FAILED;
    }

    si->wlSubsurface->SetPosition(si->attr.GetX(), si->attr.GetY());
    si->wlSubsurface->PlaceBelow(parentWlSurface);
    si->wlSubsurface->SetDesync();
    return WM_OK;
}

WMError SubwindowNormalImpl::CreateConsumerSurface(sptr<SubwindowNormalImpl> &si,
                                                   const sptr<SubwindowOption> &option)
{
    auto csurface = option->GetConsumerSurface();
    if (csurface != nullptr) {
        si->csurface = csurface;
        WMLOGFI("use Option Surface");
    } else {
        const auto &sc = SingletonContainer::Get<StaticCall>();
        si->csurface = sc->SurfaceCreateSurfaceAsConsumer("Normal Subwindow");
        WMLOGFI("use Create Surface");
    }

    if (si->csurface == nullptr) {
        WMLOGFE("SurfaceCreateSurfaceAsConsumer return nullptr");
        return WM_ERROR_API_FAILED;
    }

    auto producer = si->csurface->GetProducer();
    si->psurface = SingletonContainer::Get<StaticCall>()->SurfaceCreateSurfaceAsProducer(producer);
    if (si->psurface == nullptr) {
        WMLOGFE("SurfaceCreateSurfaceAsProducer return nullptr");
        return WM_ERROR_API_FAILED;
    }

    si->csurface->RegisterConsumerListener(si.GetRefPtr());
    si->csurface->SetDefaultWidthAndHeight(si->attr.GetWidth(), si->attr.GetHeight());
    si->csurface->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    return WM_OK;
}

WMError SubwindowNormalImpl::Create(sptr<Subwindow> &subwindow,
                                    const sptr<Window> &window,
                                    const sptr<SubwindowOption> &option)
{
    WMLOGFI("Create Normal Subwindow");

    sptr<SubwindowNormalImpl> si = nullptr;
    sptr<WlSurface> parentWlSurface = nullptr;
    auto wret = CheckAndNew(si, window, option, parentWlSurface);
    if (wret != WM_OK) {
        return wret;
    }

    si->attr.SetWidthHeight(option->GetWidth(), option->GetHeight());
    si->attr.SetXY(option->GetX(), option->GetY());

    wret = CreateWlSurface(si, parentWlSurface);
    if (wret != WM_OK) {
        return wret;
    }

    wret = CreateConsumerSurface(si, option);
    if (wret != WM_OK) {
        return wret;
    }

    subwindow = si;
    WMLOGFI("Create Normal Subwindow Success");
    return WM_OK;
}

sptr<Surface> SubwindowNormalImpl::GetSurface() const
{
    return psurface;
}

WMError SubwindowNormalImpl::Move(int32_t x, int32_t y)
{
    WMLOGFI("(subwindow normal) x: %{public}d, y: %{public}d", x, y);
    std::lock_guard<std::mutex> lock(publicMutex);
    if (isDestroy == true) {
        WMLOGFI("object destroyed");
        return WM_ERROR_DESTROYED_OBJECT;
    }

    attr.SetXY(x, y);
    wlSubsurface->SetPosition(attr.GetX(), attr.GetY());
    return WM_OK;
}

WMError SubwindowNormalImpl::Resize(uint32_t width, uint32_t height)
{
    WMLOGFI("(subwindow normal)%{public}u x %{public}u", width, height);
    std::lock_guard<std::mutex> lock(publicMutex);
    if (isDestroy == true) {
        WMLOGFI("object destroyed");
        return WM_ERROR_DESTROYED_OBJECT;
    }

    attr.SetWidthHeight(width, height);
    wlSurface->Commit();
    return WM_OK;
}

WMError SubwindowNormalImpl::Destroy()
{
    WMLOGFI("(subwindow normal) Destroy");
    std::lock_guard<std::mutex> lock(publicMutex);
    Tester::Get().ScheduleForConcurrent();
    isDestroy = true;
    csurface = nullptr;
    psurface = nullptr;
    wlSubsurface = nullptr;
    wlSurface = nullptr;
    return WM_OK;
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

void SubwindowNormalImpl::OnBeforeFrameSubmit(BeforeFrameSubmitFunc func)
{
    onBeforeFrameSubmitFunc = func;
}

namespace {
void BufferRelease(struct wl_buffer *wbuffer, int32_t fence)
{
    WMLOGFI("(subwindow normal) BufferRelease");
    sptr<Surface> surface = nullptr;
    sptr<SurfaceBuffer> sbuffer = nullptr;
    if (SingletonContainer::Get<WlBufferCache>()->GetSurfaceBuffer(wbuffer, surface, sbuffer)) {
        if (surface != nullptr && sbuffer != nullptr) {
            surface->ReleaseBuffer(sbuffer, fence);
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

    if (onBeforeFrameSubmitFunc != nullptr) {
        onBeforeFrameSubmitFunc();
    }

    if (csurface == nullptr || wlSurface == nullptr) {
        WMLOGFE("csurface or wlSurface is nullptr");
        return;
    }

    sptr<SurfaceBuffer> sbuffer;
    int32_t flushFence;
    int64_t timestamp;
    Rect damage;
    SurfaceError ret = csurface->AcquireBuffer(sbuffer, flushFence, timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        WMLOGFE("AcquireBuffer failed");
        return;
    }

    auto bc = SingletonContainer::Get<WlBufferCache>();
    auto wbuffer = bc->GetWlBuffer(csurface, sbuffer);
    if (wbuffer == nullptr) {
        auto dmaBufferFactory = SingletonContainer::Get<WlDMABufferFactory>();
        auto dmaWlBuffer = dmaBufferFactory->Create(sbuffer->GetBufferHandle());
        dmaWlBuffer->OnRelease(BufferRelease);

        wbuffer = dmaWlBuffer;
        bc->AddWlBuffer(wbuffer, csurface, sbuffer);
    }

    if (wbuffer) {
        auto br = wlSurface->GetBufferRelease();
        wbuffer->SetBufferRelease(br);
        wlSurface->Attach(wbuffer, 0, 0);
        wlSurface->SetAcquireFence(flushFence);
        wlSurface->Damage(damage.x, damage.y, damage.w, damage.h);
        wlSurface->SetSource(0, 0, sbuffer->GetWidth(), sbuffer->GetHeight());
        wlSurface->SetDestination(attr.GetWidth(), attr.GetHeight());
        WMLOGFI("(subwindow normal) Source[%{public}d x %{public}d] Dest[%{public}d x %{public}d]",
                sbuffer->GetWidth(), sbuffer->GetHeight(), attr.GetWidth(), attr.GetHeight());
        wlSurface->Commit();
        SingletonContainer::Get<WlDisplay>()->Flush();
    }
    WMLOGFI("(subwindow normal) OnBufferAvailable exit");
}
} // namespace OHOS
