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

#include "window_manager_service_client_impl.h"

#include <cerrno>
#include <mutex>
#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "window_manager_hilog.h"
#include "window_manager_service_proxy.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMServiceClientImpl"};
}

sptr<WindowManagerServiceClientImpl> WindowManagerServiceClientImpl::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WindowManagerServiceClientImpl();
        }
    }
    return instance;
}

namespace {
void OnReply(void *, struct wms *, uint32_t a)
{
    WindowManagerServiceProxy::OnReply(static_cast<wms_error>(a));
}

void OnDisplayChange(void *, struct wms *,
    uint32_t a, const char *b, uint32_t c, int32_t d, int32_t e, uint32_t f)
{
    WindowManagerServiceProxy::OnDisplayChange(a, b, static_cast<wms_screen_status>(c), d, e,
                                                    static_cast<wms_screen_type>(f));
}

void OnDisplayPower(void *, struct wms *, uint32_t a, int32_t b)
{
    WindowManagerServiceProxy::OnDisplayPower(a, b);
}

void OnDisplayBacklight(void *, struct wms *, uint32_t a, uint32_t b)
{
    WindowManagerServiceProxy::OnDisplayBacklight(a, b);
}

void OnDisplayModeChange(void *, struct wms *, uint32_t a)
{
    WindowManagerServiceProxy::OnDisplayModeChange(a);
}

void OnGlobalWindowStatus(void *, struct wms *, uint32_t a, uint32_t b, uint32_t c)
{
    WindowManagerServiceProxy::OnGlobalWindowStatus(a, b, c);
}

void OnScreenShotDone(void *, struct wms *,
    uint32_t a, int32_t b, int32_t c, int32_t d, int32_t e, uint32_t f, uint32_t g, uint32_t h)
{
    WindowManagerServiceProxy::OnScreenShot(WMS_ERROR_OK, a, b, c, d, e, f, g, h);
}

void OnScreenShotError(void *, struct wms *, uint32_t a, uint32_t b)
{
    WindowManagerServiceProxy::OnScreenShot(static_cast<wms_error>(a), b, -1, 0, 0, 0, 0, 0, 0);
}

void OnWindowShotDone(void *, struct wms *,
    uint32_t a, int32_t b, int32_t c, int32_t d, int32_t e, uint32_t f, uint32_t g, uint32_t h)
{
    WindowManagerServiceProxy::OnWindowShot(WMS_ERROR_OK, a, b, c, d, e, f, g, h);
}

void OnWindowShotError(void *, struct wms *, uint32_t a, uint32_t b)
{
    WindowManagerServiceProxy::OnWindowShot(static_cast<wms_error>(a), b, -1, 0, 0, 0, 0, 0, 0);
}

void RegistryGlobal(void *ppwms, struct wl_registry *registry,
    uint32_t id, const char *interface, uint32_t version)
{
    if (ppwms == nullptr) {
        return;
    }

    if (strcmp(interface, "wms") == 0) {
        auto &pwms = *static_cast<struct wms**>(ppwms);
        constexpr uint32_t wmsVersion = 1;
        pwms = (struct wms *)wl_registry_bind(registry, id, &wms_interface, wmsVersion);
        const struct wms_listener listener = {
            OnReply,
            OnDisplayChange,
            OnDisplayPower,
            OnDisplayBacklight,
            OnDisplayModeChange,
            nullptr,
            nullptr,
            OnGlobalWindowStatus,
            OnScreenShotDone,
            OnScreenShotError,
            OnWindowShotDone,
            OnWindowShotError,
            nullptr,
        };
        if (pwms != nullptr) {
            wms_add_listener(pwms, &listener, nullptr);
        }
    }
}
} // namespace

void WindowManagerServiceClientImpl::StartDispatchThread()
{
    startOnceFlag = std::make_unique<std::once_flag>();
    startPromise = new Promise<bool>();

    dispatchThread = std::make_unique<std::thread>(
        std::bind(&WindowManagerServiceClientImpl::DispatchThreadMain, this));

    startPromise->Await();
}

void WindowManagerServiceClientImpl::StopDispatchThread()
{
    InterruptDispatchThread();
    dispatchThread->join();
}

void WindowManagerServiceClientImpl::DispatchThreadMain()
{
    WMLOGFI("dispatch loop start");
    interruptFd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    int32_t displayFd = wl_display_get_fd(display);

    while (true) {
        if (startOnceFlag != nullptr) {
            static const auto onceFunc = [this]() {
                if (startPromise != nullptr) {
                    startPromise->Resolve(true);
                }
            };
            std::call_once(*startOnceFlag, onceFunc);
        }

        while (wl_display_prepare_read(display) != 0) {
            wl_display_dispatch_pending(display);
        }

        if (wl_display_flush(display) == -1) {
            WMLOGFE("wl_display_flush return -1");
            break;
        }

        struct pollfd pfd[] = {
            { .fd = displayFd,   .events = POLLIN, },
            { .fd = interruptFd, .events = POLLIN, },
        };

        int32_t ret = 0;
        do {
            ret = poll(pfd, sizeof(pfd) / sizeof(*pfd), -1);
        } while (ret == -1 && errno == EINTR);

        if (ret == -1) {
            WMLOGFE("poll return -1");
            wl_display_cancel_read(display);
            break;
        }

        if ((uint32_t)pfd[1].revents & POLLIN) {
            WMLOGFI("return by interrupt");
            wl_display_cancel_read(display);
            return;
        }

        if ((uint32_t)pfd[0].revents & POLLIN) {
            wl_display_read_events(display);
            if (wl_display_dispatch_pending(display) == -1) {
                WMLOGFE("wl_display_dispatch_pending return -1");
                break;
            }
        }
    }
    WMLOGFI("return %{public}d, errno: %{public}d", wl_display_get_error(display), errno);
}

void WindowManagerServiceClientImpl::InterruptDispatchThread()
{
    if (interruptFd == -1) {
        return;
    }
    uint64_t buf = 1;
    int32_t ret = 0;
    WMLOGFD("send interrupt");

    do {
        ret = write(interruptFd, &buf, sizeof(buf));
    } while (ret == -1 && errno == EINTR);
}

WMError WindowManagerServiceClientImpl::Init()
{
    WMLOGFI("init");
    if (wmservice != nullptr) {
        return WM_OK;
    }

    if (display == nullptr) {
        display = wl_display_connect(nullptr);
        if (display == nullptr) {
            WMLOGFE("Create display failed!");
            return WM_ERROR_CONNOT_CONNECT_WESTON;
        }
    }

    if (registry == nullptr) {
        registry = wl_display_get_registry(display);
        if (registry == nullptr) {
            WMLOGFE("wl_display_get_registry failed");
            return WM_ERROR_API_FAILED;
        }
    }

    struct wl_registry_listener listener = { RegistryGlobal };
    if (wl_registry_add_listener(registry, &listener, &wms) == -1) {
        WMLOGFE("wl_registry_add_listener failed");
        return WM_ERROR_API_FAILED;
    }

    if (wl_display_roundtrip(display) == -1) {
        WMLOGFE("wl_display_roundtrip failed");
        return WM_ERROR_API_FAILED;
    }

    if (wl_display_roundtrip(display) == -1) {
        WMLOGFE("wl_display_roundtrip failed");
        return WM_ERROR_API_FAILED;
    }

    if (wms == nullptr) {
        WMLOGFE("weston not have wms module");
        return WM_ERROR_WMS_NOT_FOUND;
    }

    auto gret = IAnimationService::Init();
    if (gret != GSERROR_OK) {
        WMLOGFW("animationService init failed: %{public}s", GSErrorStr(gret).c_str());
    }

    auto as = IAnimationService::Get();
    wmservice = new WindowManagerServiceProxy(wms, display, as);
    StartDispatchThread();
    return WM_OK;
}

WMError WindowManagerServiceClientImpl::Deinit()
{
    WMLOGFI("deinit");
    if (display == nullptr) {
        return WM_OK;
    }
    StopDispatchThread();
    wms_destroy(wms);
    wms = nullptr;
    wl_registry_destroy(registry);
    registry = nullptr;
    wl_display_roundtrip(display);
    wl_display_disconnect(display);
    display = nullptr;
    return WM_OK;
}

sptr<IWindowManagerService> WindowManagerServiceClientImpl::GetService() const
{
    return wmservice;
}

WindowManagerServiceClientImpl::~WindowManagerServiceClientImpl()
{
    Deinit();
}
}
