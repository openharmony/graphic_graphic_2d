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

#define CHECK_DESTROY_CONST(ret)                           \
    if (isDestroyed == true) {                             \
        WMLOGFE("find attempt to use a destroyed object"); \
        return ret;                                        \
    }

#define CHECK_DESTROY(ret)                                 \
{                                                          \
    std::lock_guard<std::mutex> lock(mutex);               \
    CHECK_DESTROY_CONST(ret);                              \
}

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMWindowImpl"};
}

WMError WindowImpl::CheckAndNew(sptr<WindowImpl> &wi,
                                const sptr<WindowOption> &option,
                                const sptr<IWindowManagerService> &wms)
{
    if (option == nullptr) {
        WMLOGFE("WindowOption is nullptr");
        return WM_ERROR_NULLPTR;
    }
    if (wms == nullptr) {
        WMLOGFE("IWindowManagerService is nullptr");
        return WM_ERROR_NULLPTR;
    }

    wi = TESTER_NEW(WindowImpl);
    if (wi == nullptr) {
        WMLOGFE("new WindowImpl failed");
        return WM_ERROR_NEW;
    }

    wi->wms = wms;
    return WM_OK;
}

WMError WindowImpl::CreateRemoteWindow(sptr<WindowImpl> &wi,
                                       const sptr<WindowOption> &option)
{
    wi->wlSurface = SingletonContainer::Get<WlSurfaceFactory>()->Create();
    if (wi->wlSurface == nullptr) {
        WMLOGFE("WlSurfaceFactory::Create return nullptr");
        return WM_ERROR_API_FAILED;
    }

    auto windowManagerServer = SingletonContainer::Get<WindowManagerServer>();
    auto promise = windowManagerServer->CreateWindow(wi->wlSurface, 0, option->GetWindowType());
    if (promise == nullptr) {
        WMLOGFE("CreateWindow return nullptr promise");
        return WM_ERROR_NEW;
    }

    auto wminfo = promise->Await();
    if (wminfo.wret != WM_OK) {
        WMLOGFE("wms->CreateWindow failed %{public}s", WMErrorStr(wminfo.wret).c_str());
        return wminfo.wret;
    }

    wi->attr.SetID(wminfo.wid);
    wi->attr.SetType(option->GetWindowType());
    wi->attr.SetVisibility(true);
    wi->attr.SetXY(wminfo.x, wminfo.y);
    wi->attr.SetWidthHeight(wminfo.width, wminfo.height);
    wi->attr.SetDestWidthHeight(wminfo.width, wminfo.height);
    wi->wlSurface->SetUserData(wi.GetRefPtr());
    return WM_OK;
}

WMError WindowImpl::CreateConsumerSurface(sptr<WindowImpl> &wi,
                                          const sptr<WindowOption> &option)
{
    const auto &sc = SingletonContainer::Get<StaticCall>();
    auto csurface = option->GetConsumerSurface();
    if (csurface != nullptr) {
        wi->csurface = csurface;
        WMLOGFI("use Option Surface");
    } else {
        wi->csurface = sc->SurfaceCreateSurfaceAsConsumer("Window");
        WMLOGFI("use Create Surface");
    }

    if (wi->csurface == nullptr) {
        WMLOGFE("SurfaceCreateSurfaceAsConsumer return nullptr");
        return WM_ERROR_API_FAILED;
    }

    wi->csurface->RegisterConsumerListener(wi.GetRefPtr());
    auto producer = wi->csurface->GetProducer();
    wi->psurface = sc->SurfaceCreateSurfaceAsProducer(producer);
    if (wi->psurface == nullptr) {
        WMLOGFE("SurfaceCreateSurfaceAsProducer return nullptr");
        return WM_ERROR_API_FAILED;
    }

    wi->csurface->SetDefaultWidthAndHeight(wi->attr.GetWidth(), wi->attr.GetHeight());
    wi->csurface->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    return WM_OK;
}

WMError WindowImpl::Create(sptr<Window> &window,
                           const sptr<WindowOption> &option,
                           const sptr<IWindowManagerService> &wms)
{
    sptr<WindowImpl> wi = nullptr;
    auto wret = CheckAndNew(wi, option, wms);
    if (wret != WM_OK) {
        return wret;
    }

    wret = CreateRemoteWindow(wi, option);
    if (wret != WM_OK) {
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

    wret = CreateConsumerSurface(wi, option);
    if (wret != WM_OK) {
        return wret;
    }

    wi->logListener = SingletonContainer::Get<LogListener>()->AddListener(wi.GetRefPtr());
    wi->mmiListener = SingletonContainer::Get<MultimodalListenerManager>()->AddListener(wi.GetRefPtr());
    wi->exportListener = SingletonContainer::Get<InputListenerManager>()->AddListener(wi.GetRefPtr());

    window = wi;
    WMLOGFI("Create Window Success");
    return WM_OK;
}

sptr<WlSurface> WindowImpl::GetWlSurface() const
{
    CHECK_DESTROY_CONST(nullptr);
    return wlSurface;
}

sptr<Surface> WindowImpl::GetSurface() const
{
    CHECK_DESTROY_CONST(nullptr);
    return psurface;
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

sptr<Promise<WMError>> WindowImpl::Show()
{
    WMLOGFI("(%{public}d)", attr.GetID());
    CHECK_DESTROY(new Promise<WMError>(WM_ERROR_DESTROYED_OBJECT));
    attr.SetVisibility(true);
    return wms->Show(attr.GetID());
}

sptr<Promise<WMError>> WindowImpl::Hide()
{
    WMLOGFI("(%{public}d)", attr.GetID());
    CHECK_DESTROY(new Promise<WMError>(WM_ERROR_DESTROYED_OBJECT));
    attr.SetVisibility(false);
    return wms->Hide(attr.GetID());
}

sptr<Promise<WMError>> WindowImpl::Move(int32_t x, int32_t y)
{
    WMLOGFI("(%{public}d) x: %{public}d, y: %{public}d", attr.GetID(), x, y);
    CHECK_DESTROY(new Promise<WMError>(WM_ERROR_DESTROYED_OBJECT));
    attr.SetXY(x, y);
    return wms->Move(attr.GetID(), attr.GetX(), attr.GetY());
}

sptr<Promise<WMError>> WindowImpl::SwitchTop()
{
    WMLOGFI("(%{public}d)", attr.GetID());
    CHECK_DESTROY(new Promise<WMError>(WM_ERROR_DESTROYED_OBJECT));
    return wms->SwitchTop(attr.GetID());
}

sptr<Promise<WMError>> WindowImpl::SetWindowType(WindowType type)
{
    WMLOGFI("(%{public}d)type: %{public}d", attr.GetID(), type);
    CHECK_DESTROY(new Promise<WMError>(WM_ERROR_DESTROYED_OBJECT));
    static sptr<WindowOption> testParam = WindowOption::Get();
    if (testParam->SetWindowType(type) != WM_OK) {
        return new Promise<WMError>(WM_ERROR_INVALID_PARAM);
    }

    attr.SetType(type);
    return wms->SetWindowType(attr.GetID(), type);
}

sptr<Promise<WMError>> WindowImpl::SetWindowMode(WindowMode mode)
{
    WMLOGFI("(%{public}d)mode: %{public}d", attr.GetID(), mode);
    CHECK_DESTROY(new Promise<WMError>(WM_ERROR_DESTROYED_OBJECT));
    static sptr<WindowOption> testParam = WindowOption::Get();
    if (testParam->SetWindowMode(mode) != WM_OK) {
        return new Promise<WMError>(WM_ERROR_INVALID_PARAM);
    }

    attr.SetMode(mode);
    return wms->SetWindowMode(attr.GetID(), mode);
}

sptr<Promise<WMError>> WindowImpl::Resize(uint32_t width, uint32_t height)
{
    WMLOGFI("(%{public}d)%{public}u x %{public}u", attr.GetID(), width, height);
    CHECK_DESTROY(new Promise<WMError>(WM_ERROR_DESTROYED_OBJECT));
    static sptr<WindowOption> testParam = WindowOption::Get();
    if (testParam->SetWidth(width) != WM_OK || testParam->SetHeight(height) != WM_OK) {
        return new Promise<WMError>(WM_ERROR_INVALID_PARAM);
    }

    attr.SetWidthHeight(width, height);
    attr.SetDestWidthHeight(width, height);
    return wms->Resize(attr.GetID(), width, height);
}

sptr<Promise<WMError>> WindowImpl::ScaleTo(uint32_t width, uint32_t height)
{
    WMLOGFI("(%{public}d)%{public}u x %{public}u", attr.GetID(), width, height);
    CHECK_DESTROY(new Promise<WMError>(WM_ERROR_DESTROYED_OBJECT));
    if (width == 0 || height == 0) {
        return new Promise<WMError>(WM_ERROR_INVALID_PARAM);
    }

    attr.SetDestWidthHeight(width, height);
    return wms->ScaleTo(attr.GetID(), width, height);
}

WMError WindowImpl::Rotate(WindowRotateType type)
{
    WMLOGFI("(%{public}d)type: %{public}d", attr.GetID(), type);
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    auto display = SingletonContainer::Get<WlDisplay>();
    if (!(type >= 0 && type < WINDOW_ROTATE_TYPE_MAX)) {
        return WM_ERROR_INVALID_PARAM;
    }

    wlSurface->SetBufferTransform(static_cast<wl_output_transform>(type));
    display->Sync();
    if (display->GetError() != 0) {
        return WM_ERROR_API_FAILED;
    }

    wlSurface->Commit();

    display->Sync();
    if (display->GetError() != 0) {
        return WM_ERROR_API_FAILED;
    }
    return WM_OK;
}

WMError WindowImpl::Destroy()
{
    WMLOGFI("(%{public}d)", attr.GetID());
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    WMError wret;
    {
        std::lock_guard<std::mutex> lock(mutex);
        isDestroyed = true;
        auto promise = wms->DestroyWindow(attr.GetID());
        wret = promise->Await();
        wms = nullptr;
        wlSurface = nullptr;
        csurface = nullptr;
        psurface = nullptr;
    }
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

WMError WindowImpl::OnTouch(OnTouchFunc cb)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    mmiListener->onTouchCb = cb;
    return WM_OK;
}

WMError WindowImpl::OnKey(OnKeyFunc cb)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    mmiListener->keyboardKeyCb = cb;
    return WM_OK;
}

WMError WindowImpl::OnPointerEnter(PointerEnterFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->pointerEnter = func;
    return WM_OK;
}

WMError WindowImpl::OnPointerLeave(PointerLeaveFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->pointerLeave = func;
    return WM_OK;
}

WMError WindowImpl::OnPointerMotion(PointerMotionFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->pointerMotion = func;
    return WM_OK;
}

WMError WindowImpl::OnPointerButton(PointerButtonFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->pointerButton = func;
    return WM_OK;
}

WMError WindowImpl::OnPointerFrame(PointerFrameFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->pointerFrame = func;
    return WM_OK;
}

WMError WindowImpl::OnPointerAxis(PointerAxisFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->pointerAxis = func;
    return WM_OK;
}

WMError WindowImpl::OnPointerAxisSource(PointerAxisSourceFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->pointerAxisSource = func;
    return WM_OK;
}

WMError WindowImpl::OnPointerAxisStop(PointerAxisStopFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->pointerAxisStop = func;
    return WM_OK;
}

WMError WindowImpl::OnPointerAxisDiscrete(PointerAxisDiscreteFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->pointerAxisDiscrete = func;
    return WM_OK;
}

WMError WindowImpl::OnKeyboardKeymap(KeyboardKeymapFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->keyboardKeymap = func;
    return WM_OK;
}

WMError WindowImpl::OnKeyboardEnter(KeyboardEnterFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->keyboardEnter = func;
    return WM_OK;
}

WMError WindowImpl::OnKeyboardLeave(KeyboardLeaveFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->keyboardLeave = func;
    return WM_OK;
}

WMError WindowImpl::OnKeyboardKey(KeyboardKeyFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->keyboardKey = func;
    return WM_OK;
}

WMError WindowImpl::OnKeyboardModifiers(KeyboardModifiersFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->keyboardModifiers = func;
    return WM_OK;
}

WMError WindowImpl::OnKeyboardRepeatInfo(KeyboardRepeatInfoFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->keyboardRepeatInfo = func;
    return WM_OK;
}

WMError WindowImpl::OnTouchDown(TouchDownFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->touchDown = func;
    return WM_OK;
}

WMError WindowImpl::OnTouchUp(TouchUpFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->touchUp = func;
    return WM_OK;
}

WMError WindowImpl::OnTouchMotion(TouchMotionFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->touchMotion = func;
    return WM_OK;
}

WMError WindowImpl::OnTouchFrame(TouchFrameFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->touchFrame = func;
    return WM_OK;
}

WMError WindowImpl::OnTouchCancel(TouchCancelFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->touchCancel = func;
    return WM_OK;
}

WMError WindowImpl::OnTouchShape(TouchShapeFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->touchShape = func;
    return WM_OK;
}

WMError WindowImpl::OnTouchOrientation(TouchOrientationFunc func)
{
    CHECK_DESTROY(WM_ERROR_DESTROYED_OBJECT);
    exportListener->touchOrientation = func;
    return WM_OK;
}

namespace {
void BufferRelease(struct wl_buffer *wbuffer)
{
    WMLOGFI("BufferRelease");
    sptr<Surface> surface = nullptr;
    sptr<SurfaceBuffer> sbuffer = nullptr;
    if (SingletonContainer::Get<WlBufferCache>()->GetSurfaceBuffer(wbuffer, surface, sbuffer)) {
        if (surface != nullptr && sbuffer != nullptr) {
            surface->ReleaseBuffer(sbuffer, -1);
        }
    }
}
} // namespace

void WindowImpl::OnBufferAvailable()
{
    WMLOGFI("OnBufferAvailable enter");
    CHECK_DESTROY();

    sptr<SurfaceBuffer> sbuffer;
    int32_t flushFence;
    int64_t timestamp;
    Rect damage;
    auto sret = csurface->AcquireBuffer(sbuffer, flushFence, timestamp, damage);
    if (sret != SURFACE_ERROR_OK) {
        WMLOGFE("AcquireBuffer failed");
        return;
    }

    auto bc = SingletonContainer::Get<WlBufferCache>();
    auto wbuffer = bc->GetWlBuffer(csurface, sbuffer);
    if (wbuffer == nullptr) {
        auto dmaBufferFactory = SingletonContainer::Get<WlDMABufferFactory>();
        auto dmaWlBuffer = dmaBufferFactory->Create(sbuffer->GetBufferHandle());
        if (dmaWlBuffer == nullptr) {
            WMLOGFE("Create DMA Buffer Failed");
            sret = csurface->ReleaseBuffer(sbuffer, -1);
            if (sret != SURFACE_ERROR_OK) {
                WMLOGFW("ReleaseBuffer failed");
            }
            return;
        }
        dmaWlBuffer->OnRelease(BufferRelease);

        wbuffer = dmaWlBuffer;
        bc->AddWlBuffer(wbuffer, csurface, sbuffer);
    }

    if (wbuffer) {
        wlSurface->Attach(wbuffer, 0, 0);
        wlSurface->Damage(damage.x, damage.y, damage.w, damage.h);
        wlSurface->Commit();
        SingletonContainer::Get<WlDisplay>()->Flush();
    }
    WMLOGFI("OnBufferAvailable exit");
}

WindowImpl::~WindowImpl()
{
    if (logListener != nullptr) {
        SingletonContainer::Get<LogListener>()->RemoveListener(logListener);
    }

    if (mmiListener != nullptr) {
        SingletonContainer::Get<MultimodalListenerManager>()->RemoveListener(mmiListener);
    }

    if (exportListener != nullptr) {
        SingletonContainer::Get<InputListenerManager>()->RemoveListener(exportListener);
    }

    if (csurface != nullptr) {
        csurface->UnregisterConsumerListener();
    }
}
} // namespace OHOS
