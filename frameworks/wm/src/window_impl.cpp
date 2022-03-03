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

#include "window_impl.h"

#include <display_type.h>
#include <scoped_bytrace.h>

#include "singleton_delegator.h"
#include "static_call.h"
#include "tester.h"
#include "window_manager_hilog.h"
#include "window_manager_impl.h"
#include "window_manager_server.h"
#include "wl_buffer_cache.h"
#include "wl_display.h"
#include "wl_dma_buffer_factory.h"
#include "wl_surface_factory.h"

#define CHECK_DESTROY_CONST(ret)                               \
    do {                                                       \
        if (isDestroyed == true) {                             \
            WMLOGFE("find attempt to use a destroyed object"); \
            return ret;                                        \
        }                                                      \
    }while (0)

#define CHECK_DESTROY(ret)                                 \
    do {                                                   \
        std::lock_guard<std::mutex> lock(mutex);           \
        CHECK_DESTROY_CONST(ret);                          \
    }while (0)

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMWindowImpl"};
}

GSError WindowImpl::CheckAndNew(sptr<WindowImpl> &wi,
                                const sptr<WindowOption> &option,
                                const sptr<IWindowManagerService> &wms)
{
    if (wms == nullptr) {
        WMLOGFE("IWindowManagerService is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (option == nullptr) {
        WMLOGFE("WindowOption is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    wi = TESTER_NEW(WindowImpl);
    if (wi == nullptr) {
        WMLOGFE("new WindowImpl failed");
        return GSERROR_NO_MEM;
    }

    wi->wms = wms;
    return GSERROR_OK;
}

GSError WindowImpl::CreateRemoteWindow(sptr<WindowImpl> &wi,
                                       const sptr<WindowOption> &option)
{
    wi->wlSurface = SingletonContainer::Get<WlSurfaceFactory>()->Create();
    if (wi->wlSurface == nullptr) {
        WMLOGFE("WlSurfaceFactory::Create return nullptr");
        return GSERROR_API_FAILED;
    }

    auto windowManagerServer = SingletonContainer::Get<WindowManagerServer>();
    auto promise = windowManagerServer->CreateWindow(wi->wlSurface,
        option->GetDisplay(), option->GetWindowType());
    if (promise == nullptr) {
        WMLOGFE("CreateWindow return nullptr promise");
        return GSERROR_NO_MEM;
    }

    auto wminfo = promise->Await();
    if (wminfo.wret != GSERROR_OK) {
        WMLOGFE("wms->CreateWindow failed %{public}s", GSErrorStr(wminfo.wret).c_str());
        return wminfo.wret;
    }

    auto onWindowSizeChange = [&attr = wi->attr](int32_t width, int32_t height) {
        attr.SetWidthHeight(width, height);
    };
    windowManagerServer->RegisterWindowSizeChange(onWindowSizeChange);

    wi->attr.SetID(wminfo.wid);
    wi->attr.SetType(option->GetWindowType());
    wi->attr.SetVisibility(true);
    wi->attr.SetXY(wminfo.x, wminfo.y);
    wi->attr.SetWidthHeight(wminfo.width, wminfo.height);
    wi->attr.SetDestWidthHeight(wminfo.width, wminfo.height);
    wi->wlSurface->SetUserData(wi.GetRefPtr());
    return GSERROR_OK;
}

GSError WindowImpl::CreateConsumerSurface(sptr<WindowImpl> &wi,
                                          const sptr<WindowOption> &option)
{
    const auto &sc = SingletonContainer::Get<StaticCall>();
    auto csurf = option->GetConsumerSurface();
    if (csurf != nullptr) {
        wi->csurf = csurf;
        WMLOGFI("use Option Surface");
    } else {
        wi->csurf = sc->SurfaceCreateSurfaceAsConsumer("Window");
        WMLOGFI("use Create Surface");
    }

    if (wi->csurf == nullptr) {
        WMLOGFE("SurfaceCreateSurfaceAsConsumer return nullptr");
        return GSERROR_API_FAILED;
    }

    wi->csurf->RegisterConsumerListener(wi.GetRefPtr());
    auto producer = wi->csurf->GetProducer();
    wi->psurf = sc->SurfaceCreateSurfaceAsProducer(producer);
    if (wi->psurf == nullptr) {
        WMLOGFE("SurfaceCreateSurfaceAsProducer return nullptr");
        return GSERROR_API_FAILED;
    }

    wi->csurf->SetDefaultWidthAndHeight(wi->attr.GetWidth(), wi->attr.GetHeight());
    wi->csurf->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    return GSERROR_OK;
}

GSError WindowImpl::Create(sptr<Window> &window,
                           const sptr<WindowOption> &option,
                           const sptr<IWindowManagerService> &wms)
{
    sptr<WindowImpl> wi = nullptr;
    auto wret = CheckAndNew(wi, option, wms);
    if (wret != GSERROR_OK) {
        return wret;
    }

    wret = CreateRemoteWindow(wi, option);
    if (wret != GSERROR_OK) {
        return wret;
    }

    if (option->IsSettingX() || option->IsSettingY()) {
        wi->attr.SetXY(option->GetX(), option->GetY());
        wi->Move(option->GetX(), option->GetY())->Await();
    }

    if (option->IsSettingWidth() || option->IsSettingHeight()) {
        wi->attr.SetWidthHeight(option->GetWidth(), option->GetHeight());
        wi->Resize(option->GetWidth(), option->GetHeight())->Await();
    }

    if (option->GetWindowMode() != WINDOW_MODE_UNSET) {
        wi->attr.SetMode(option->GetWindowMode());
        wi->SetWindowMode(option->GetWindowMode());
    }

    wret = CreateConsumerSurface(wi, option);
    if (wret != GSERROR_OK) {
        return wret;
    }

    window = wi;
    WMLOGFI("Create Window Success");
    return GSERROR_OK;
}

sptr<WlSurface> WindowImpl::GetWlSurface() const
{
    CHECK_DESTROY_CONST(nullptr);
    return wlSurface;
}

sptr<Surface> WindowImpl::GetSurface() const
{
    CHECK_DESTROY_CONST(nullptr);
    return psurf;
}

sptr<IBufferProducer> WindowImpl::GetProducer() const
{
    CHECK_DESTROY_CONST(nullptr);
    return csurf->GetProducer();
}

int32_t WindowImpl::GetID() const
{
    CHECK_DESTROY_CONST(-1);
    return attr.GetID();
}

int32_t WindowImpl::GetX() const
{
    CHECK_DESTROY_CONST(-1);
    return attr.GetX();
}

int32_t WindowImpl::GetY() const
{
    CHECK_DESTROY_CONST(-1);
    return attr.GetY();
}

uint32_t WindowImpl::GetWidth() const
{
    CHECK_DESTROY_CONST(0);
    return attr.GetWidth();
}

uint32_t WindowImpl::GetHeight() const
{
    CHECK_DESTROY_CONST(0);
    return attr.GetHeight();
}

uint32_t WindowImpl::GetDestWidth() const
{
    CHECK_DESTROY_CONST(0);
    return attr.GetDestWidth();
}

uint32_t WindowImpl::GetDestHeight() const
{
    CHECK_DESTROY_CONST(0);
    return attr.GetDestHeight();
}

bool WindowImpl::GetVisibility() const
{
    CHECK_DESTROY_CONST(false);
    return attr.GetVisibility();
}

WindowType WindowImpl::GetType() const
{
    CHECK_DESTROY_CONST(static_cast<WindowType>(-1));
    return attr.GetType();
}

WindowMode WindowImpl::GetMode() const
{
    CHECK_DESTROY_CONST(static_cast<WindowMode>(-1));
    return attr.GetMode();
}

sptr<Promise<GSError>> WindowImpl::Show()
{
    WMLOGFI("(%{public}d)", attr.GetID());
    CHECK_DESTROY(new Promise<GSError>(GSERROR_DESTROYED_OBJECT));
    attr.SetVisibility(true);
    return wms->Show(attr.GetID());
}

sptr<Promise<GSError>> WindowImpl::Hide()
{
    WMLOGFI("(%{public}d)", attr.GetID());
    CHECK_DESTROY(new Promise<GSError>(GSERROR_DESTROYED_OBJECT));
    attr.SetVisibility(false);
    return wms->Hide(attr.GetID());
}

sptr<Promise<GSError>> WindowImpl::Move(int32_t x, int32_t y)
{
    WMLOGFI("(%{public}d) x: %{public}d, y: %{public}d", attr.GetID(), x, y);
    CHECK_DESTROY(new Promise<GSError>(GSERROR_DESTROYED_OBJECT));
    attr.SetXY(x, y);
    return wms->Move(attr.GetID(), attr.GetX(), attr.GetY());
}

sptr<Promise<GSError>> WindowImpl::SwitchTop()
{
    WMLOGFI("(%{public}d)", attr.GetID());
    CHECK_DESTROY(new Promise<GSError>(GSERROR_DESTROYED_OBJECT));
    return wms->SwitchTop(attr.GetID());
}

sptr<Promise<GSError>> WindowImpl::SetWindowType(WindowType type)
{
    WMLOGFI("(%{public}d)type: %{public}d", attr.GetID(), type);
    CHECK_DESTROY(new Promise<GSError>(GSERROR_DESTROYED_OBJECT));
    static sptr<WindowOption> testParam = WindowOption::Get();
    if (testParam->SetWindowType(type) != GSERROR_OK) {
        return new Promise<GSError>(GSERROR_INVALID_ARGUMENTS);
    }

    attr.SetType(type);
    return wms->SetWindowType(attr.GetID(), type);
}

sptr<Promise<GSError>> WindowImpl::SetWindowMode(WindowMode mode)
{
    WMLOGFI("(%{public}d)mode: %{public}d", attr.GetID(), mode);
    CHECK_DESTROY(new Promise<GSError>(GSERROR_DESTROYED_OBJECT));
    static sptr<WindowOption> testParam = WindowOption::Get();
    if (testParam->SetWindowMode(mode) != GSERROR_OK) {
        return new Promise<GSError>(GSERROR_INVALID_ARGUMENTS);
    }

    attr.SetMode(mode);
    return wms->SetWindowMode(attr.GetID(), mode);
}

sptr<Promise<GSError>> WindowImpl::Resize(uint32_t width, uint32_t height)
{
    WMLOGFI("(%{public}d)%{public}u x %{public}u", attr.GetID(), width, height);
    CHECK_DESTROY(new Promise<GSError>(GSERROR_DESTROYED_OBJECT));
    static sptr<WindowOption> testParam = WindowOption::Get();
    if (testParam->SetWidth(width) != GSERROR_OK || testParam->SetHeight(height) != GSERROR_OK) {
        return new Promise<GSError>(GSERROR_INVALID_ARGUMENTS);
    }

    attr.SetWidthHeight(width, height);
    attr.SetDestWidthHeight(width, height);
    return wms->Resize(attr.GetID(), width, height);
}

sptr<Promise<GSError>> WindowImpl::ScaleTo(uint32_t width, uint32_t height)
{
    WMLOGFI("(%{public}d)%{public}u x %{public}u", attr.GetID(), width, height);
    CHECK_DESTROY(new Promise<GSError>(GSERROR_DESTROYED_OBJECT));
    if (width == 0 || height == 0) {
        return new Promise<GSError>(GSERROR_INVALID_ARGUMENTS);
    }

    attr.SetDestWidthHeight(width, height);
    return wms->ScaleTo(attr.GetID(), width, height);
}

GSError WindowImpl::Rotate(WindowRotateType type)
{
    WMLOGFI("(%{public}d)type: %{public}d", attr.GetID(), type);
    CHECK_DESTROY(GSERROR_DESTROYED_OBJECT);
    auto display = SingletonContainer::Get<WlDisplay>();
    if (!(type >= 0 && type < WINDOW_ROTATE_TYPE_MAX)) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    wlSurface->SetBufferTransform(static_cast<wl_output_transform>(type));
    display->Sync();
    if (display->GetError() != 0) {
        return GSERROR_API_FAILED;
    }

    wlSurface->Commit();

    display->Sync();
    if (display->GetError() != 0) {
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

GSError WindowImpl::Destroy()
{
    WMLOGFI("(%{public}d)", attr.GetID());
    CHECK_DESTROY(GSERROR_DESTROYED_OBJECT);
    GSError wret;
    {
        std::lock_guard<std::mutex> lock(mutex);
        isDestroyed = true;
        auto promise = wms->DestroyWindow(attr.GetID());
        wret = promise->Await();
        if (csurf != nullptr) {
            csurf->UnregisterConsumerListener();
        }
        wms = nullptr;
        wlSurface = nullptr;
        csurf = nullptr;
        psurf = nullptr;
    }
    SingletonContainer::Get<WlBufferCache>()->CleanCache();
    return wret;
}

void WindowImpl::OnPositionChange(WindowPositionChangeFunc func)
{
    attr.OnPositionChange(func);
}

void WindowImpl::OnSizeChange(WindowSizeChangeFunc func)
{
    attr.OnSizeChange(func);
}

void WindowImpl::OnVisibilityChange(WindowVisibilityChangeFunc func)
{
    attr.OnVisibilityChange(func);
}

void WindowImpl::OnTypeChange(WindowTypeChangeFunc func)
{
    attr.OnTypeChange(func);
}

void WindowImpl::OnModeChange(WindowModeChangeFunc func)
{
    attr.OnModeChange(func);
}

void WindowImpl::OnSplitStatusChange(SplitStatusChangeFunc func)
{
    auto windowManagerServer = SingletonContainer::Get<WindowManagerServer>();
    windowManagerServer->RegisterSplitModeChange(func);
}

bool WindowImpl::GetPIPMode() const
{
    CHECK_DESTROY_CONST(false);
    return attr.GetPIPMode();
}

GSError WindowImpl::EnterPIPMode(int32_t x, int32_t y,
                                 uint32_t width, uint32_t height)
{
    WMLOGFI("(%{public}d) %{public}d %{public}d %{public}u %{public}u",
            attr.GetID(), x, y, width, height);
    CHECK_DESTROY(GSERROR_DESTROYED_OBJECT);
    if (GetPIPMode()) {
        return GSERROR_INVALID_OPERATING;
    }

    pipBackup.x = GetX();
    pipBackup.y = GetY();
    pipBackup.w = GetWidth();
    pipBackup.h = GetHeight();
    pipBackup.mode = GetMode();

    auto pret1 = Move(x, y);
    auto pret2 = Resize(width, height);
    auto pret3 = SetWindowMode(WINDOW_MODE_FREE);

    auto ret1 = pret1->Await();
    auto ret2 = pret2->Await();
    auto ret3 = pret3->Await();
    if (ret1 || ret2 || ret3) {
        WMLOGFE("Move/Resize/SetWindowMode failed");
        Move(pipBackup.x, pipBackup.y);
        Resize(pipBackup.w, pipBackup.h);
        SetWindowMode(pipBackup.mode);
        return GSERROR_API_FAILED;
    }

    attr.SetPIPMode(true);
    return GSERROR_OK;
}

GSError WindowImpl::ExitPIPMode()
{
    WMLOGFI("(%{public}d)", attr.GetID());
    CHECK_DESTROY(GSERROR_DESTROYED_OBJECT);
    if (!GetPIPMode()) {
        return GSERROR_INVALID_OPERATING;
    }

    Move(pipBackup.x, pipBackup.y);
    Resize(pipBackup.w, pipBackup.h);
    SetWindowMode(pipBackup.mode);

    attr.SetPIPMode(false);
    return GSERROR_OK;
}

GSError WindowImpl::OnPIPModeChange(WindowPIPModeChangeFunc func)
{
    attr.OnPIPModeChange(func);
    return GSERROR_OK;
}

void WindowImpl::OnBeforeFrameSubmit(BeforeFrameSubmitFunc func)
{
    onBeforeFrameSubmitFunc = func;
}

void WindowImpl::OnWlBufferRelease(struct wl_buffer *wbuffer, int32_t fence)
{
    ScopedBytrace bytrace("OnWlBufferRelease");
    WMLOGFI("BufferRelease");
    CHECK_DESTROY();
    sptr<Surface> surf = nullptr;
    sptr<SurfaceBuffer> sbuffer = nullptr;
    if (SingletonContainer::Get<WlBufferCache>()->GetSurfaceBuffer(wbuffer, surf, sbuffer)) {
        if (surf != nullptr && sbuffer != nullptr) {
            surf->ReleaseBuffer(sbuffer, fence);
        }
    }
}

void WindowImpl::OnBufferAvailable()
{
    WMLOGFI("OnBufferAvailable enter");
    CHECK_DESTROY();

    if (onBeforeFrameSubmitFunc != nullptr) {
        onBeforeFrameSubmitFunc();
    }

    sptr<SurfaceBuffer> sbuffer;
    int32_t flushFence;
    int64_t timestamp;
    Rect damage;
    auto sret = csurf->AcquireBuffer(sbuffer, flushFence, timestamp, damage);
    if (sret != GSERROR_OK) {
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
            sret = csurf->ReleaseBuffer(sbuffer, -1);
            if (sret != GSERROR_OK) {
                WMLOGFW("ReleaseBuffer failed");
            }
            return;
        }
        dmaWlBuffer->OnRelease(this);

        wbuffer = dmaWlBuffer;
        bc->AddWlBuffer(wbuffer, csurf, sbuffer);
    }

    if (wbuffer) {
        auto br = wlSurface->GetBufferRelease();
        wbuffer->SetBufferRelease(br);
        wlSurface->Attach(wbuffer, 0, 0);
        wlSurface->SetAcquireFence(flushFence);
        wlSurface->Damage(damage.x, damage.y, damage.w, damage.h);
        wlSurface->SetSource(0, 0, sbuffer->GetWidth(), sbuffer->GetHeight());
        wlSurface->SetDestination(attr.GetWidth(), attr.GetHeight());
        wlSurface->Commit();
        SingletonContainer::Get<WlDisplay>()->Flush();
    }
    WMLOGFI("OnBufferAvailable exit");
}

WindowImpl::~WindowImpl()
{
    Destroy();
}
} // namespace OHOS
