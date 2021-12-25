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

#include "window_manager_impl.h"

#include <mutex>
#include <sys/mman.h>

#include <display_type.h>
#include <iservice_registry.h>
#include <securec.h>
#include <system_ability_definition.h>

#include "static_call.h"
#include "subwindow_normal_impl.h"
#include "subwindow_video_impl.h"
#include "window_impl.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WMWMImpl" };
} // namespace

WindowManagerImpl::WindowManagerImpl()
{
    WMLOGFD("WindowManagerImpl");
}

WindowManagerImpl::~WindowManagerImpl()
{
    WMLOGFD("~WindowManagerImpl");
    Deinit();
}

sptr<WindowManagerImpl> WindowManagerImpl::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WindowManagerImpl();
        }
    }
    return instance;
}

void WindowManagerImpl::InitSingleton()
{
    if (initSingleton == false) {
        initSingleton = true;
        windowManagerServer = SingletonContainer::Get<WindowManagerServer>();
        wlBufferCache = SingletonContainer::Get<WlBufferCache>();
        wlDMABufferFactory = SingletonContainer::Get<WlDMABufferFactory>();
        wlSHMBufferFactory = SingletonContainer::Get<WlSHMBufferFactory>();
        wlSubsurfaceFactory = SingletonContainer::Get<WlSubsurfaceFactory>();
        wlSurfaceFactory = SingletonContainer::Get<WlSurfaceFactory>();

        windowManagerServer->Init();
        wlBufferCache->Init();
        wlDMABufferFactory->Init();
        wlSHMBufferFactory->Init();
        wlSubsurfaceFactory->Init();
        wlSurfaceFactory->Init();

        waylandService = SingletonContainer::Get<WaylandService>();
        waylandService->Start();
        display->Roundtrip();
        display->Roundtrip();
    }
}

GSError WindowManagerImpl::Init()
{
    std::lock_guard<std::mutex> lock(initMutex);

    display = SingletonContainer::Get<WlDisplay>();
    if (init == true) {
        if (display->GetError() == 0) {
            return GSERROR_OK; // already init
        }
        WMLOGFI("Reinit");
        Deinit(); // Reinit
    }
    WMLOGFI("init");

    if (display->GetFd() == -1) {
        if (display->Connect(nullptr) == false) {
            WMLOGFE("create display failed!");
            return GSERROR_CONNOT_CONNECT_WESTON;
        }
    }

    InitSingleton();
    display->StartDispatchThread();

    if (wmservice == nullptr) {
        wmsc = SingletonContainer::Get<WindowManagerServiceClient>();
        GSError wret = wmsc->Init();
        if (wret != GSERROR_OK) {
            WMLOGFE("WMService init failed");
            display->StopDispatchThread();
            wmsc = nullptr;
            return wret;
        }

        wmservice = wmsc->GetService();
    }
    init = true;
    return GSERROR_OK;
}

void WindowManagerImpl::DeinitSingleton()
{
    if (initSingleton == true) {
        initSingleton = false;
        waylandService->Stop();

        wlSurfaceFactory->Deinit();
        wlSubsurfaceFactory->Deinit();
        wlSHMBufferFactory->Deinit();
        wlDMABufferFactory->Deinit();
        wlBufferCache->Deinit();
        windowManagerServer->Deinit();

        waylandService = nullptr;
        wlSurfaceFactory = nullptr;
        wlSubsurfaceFactory = nullptr;
        wlSHMBufferFactory = nullptr;
        wlDMABufferFactory = nullptr;
        wlBufferCache = nullptr;
        windowManagerServer = nullptr;

        display->Roundtrip();
    }
}

void WindowManagerImpl::Deinit()
{
    WMLOGFD("deinit");
    display = SingletonContainer::Get<WlDisplay>();

    if (wmservice != nullptr) {
        wmservice = nullptr;
        wmsc = nullptr;
    }

    display->StopDispatchThread();
    DeinitSingleton();

    display->Disconnect();
}

GSError WindowManagerImpl::GetDisplays(std::vector<struct WMDisplayInfo> &displays) const
{
    if (wmservice == nullptr) {
        return GSERROR_NOT_INIT;
    }
    return wmservice->GetDisplays(displays);
}

sptr<Window> WindowManagerImpl::GetWindowByID(int32_t wid)
{
    sptr<Window> ret = nullptr;
    auto cache = windowCache;
    windowCache.clear();
    for (const auto &wptrWindow : cache) {
        auto window = wptrWindow.promote();
        if (window != nullptr) {
            windowCache.push_back(window);
            if (window->GetID() == wid) {
                ret = window;
            }
        }
    }
    return ret;
}

GSError WindowManagerImpl::CreateVirtualDisplay(const sptr<VirtualDisplayOption> &option)
{
    if (wmservice == nullptr) {
        return GSERROR_NOT_INIT;
    }

    auto promise = wmservice->CreateVirtualDisplay(option->GetX(), option->GetY(),
        option->GetWidth(), option->GetHeight());
    if (promise == nullptr) {
        WMLOGFE("CreateVirtualDisplay return nullptr promise");
        return GSERROR_NO_MEM;
    }

    auto wret = promise->Await();
    if (wret != GSERROR_OK) {
        WMLOGFE("wms->CreateVirtualDisplay failed %{public}s", GSErrorStr(wret).c_str());
        return wret;
    }

    return GSERROR_OK;
}

GSError WindowManagerImpl::DestroyVirtualDisplay(uint32_t did)
{
    if (wmservice == nullptr) {
        return GSERROR_NOT_INIT;
    }

    auto promise = wmservice->DestroyVirtualDisplay(did);
    if (promise == nullptr) {
        WMLOGFE("DestroyVirtualDisplay return nullptr promise");
        return GSERROR_NO_MEM;
    }

    auto wret = promise->Await();
    if (wret != GSERROR_OK) {
        WMLOGFE("wms->DestroyVirtualDisplay failed %{public}s", GSErrorStr(wret).c_str());
        return wret;
    }

    return GSERROR_OK;
}

GSError WindowManagerImpl::SetDisplayMode(WMSDisplayMode mode)
{
    if (wmservice == nullptr) {
        return GSERROR_NOT_INIT;
    }

    auto promise = wmservice->SetDisplayMode(mode);
    if (promise == nullptr) {
        WMLOGFE("SetDisplayMode return nullptr promise");
        return GSERROR_NO_MEM;
    }

    auto wret = promise->Await();
    if (wret != GSERROR_OK) {
        WMLOGFE("wms->SetDisplayMode failed %{public}s", GSErrorStr(wret).c_str());
        return wret;
    }

    return GSERROR_OK;
}

GSError WindowManagerImpl::CreateWindow(sptr<Window> &window, const sptr<WindowOption> &option)
{
    if (option == nullptr) {
        WMLOGFE("WindowOption is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (wmservice == nullptr) {
        return GSERROR_NOT_INIT;
    }

    auto wret = SingletonContainer::Get<StaticCall>()->WindowImplCreate(window, option, wmservice);
    if (window != nullptr) {
        windowCache.push_back(window);
    }
    return wret;
}

GSError WindowManagerImpl::CreateSubwindow(sptr<Subwindow> &subwindow,
                                           const sptr<Window> &window,
                                           const sptr<SubwindowOption> &option)
{
    if (window == nullptr) {
        WMLOGFE("Window is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (option == nullptr) {
        WMLOGFE("WindowOption is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto staticCall = SingletonContainer::Get<StaticCall>();
    auto type = option->GetWindowType();
    if (type == SUBWINDOW_TYPE_NORMAL) {
        return staticCall->SubwindowNormalImplCreate(subwindow, window, option);
    }

    if (type == SUBWINDOW_TYPE_VIDEO) {
        return staticCall->SubwindowVideoImplCreate(subwindow, window, option);
    }

    if (type == SUBWINDOW_TYPE_OFFSCREEN) {
        return staticCall->SubwindowOffscreenImplCreate(subwindow, window, option);
    }
    return GSERROR_NOT_SUPPORT;
}

GSError WindowManagerImpl::ListenNextScreenShot(int32_t id, IScreenShotCallback *cb)
{
    if (wmservice == nullptr) {
        WMLOGFE("wmservice is nullptr");
        return GSERROR_NOT_INIT;
    }

    if (cb == nullptr) {
        WMLOGFE("callback is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto promise = wmservice->ShotScreen(id);
    if (promise == nullptr) {
        WMLOGFE("promise is nullptr");
        return GSERROR_NO_MEM;
    }

    auto then = [cb](const auto &wmsinfo) {
        WMImageInfo wminfo = {
            .wret = wmsinfo.wret,
            .width = wmsinfo.width,
            .height = wmsinfo.height,
            .format = wmsinfo.format,
            .size = wmsinfo.stride * wmsinfo.height,
            .data = nullptr,
        };
        auto data = mmap(nullptr, wminfo.size, PROT_READ, MAP_SHARED, wmsinfo.fd, 0);
        wminfo.data = data;

        cb->OnScreenShot(wminfo);

        // 0xffffffff
        uint8_t *errPtr = nullptr;
        errPtr--;
        if (data != errPtr) {
            munmap(data, wminfo.size);
        }
    };
    promise->Then(then);
    return GSERROR_OK;
}

GSError WindowManagerImpl::ListenNextWindowShot(const sptr<Window> &window, IWindowShotCallback *cb)
{
    if (wmservice == nullptr) {
        WMLOGFE("wmservice is nullptr");
        return GSERROR_NOT_INIT;
    }

    if (window == nullptr) {
        WMLOGFE("window is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (cb == nullptr) {
        WMLOGFE("callback is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto promise = wmservice->ShotWindow(window->GetID());
    if (promise == nullptr) {
        WMLOGFE("promise is nullptr");
        return GSERROR_NO_MEM;
    }

    auto then = [cb](const auto &wmsinfo) {
        WMImageInfo wminfo = {
            .wret = wmsinfo.wret,
            .width = wmsinfo.width,
            .height = wmsinfo.height,
            .format = wmsinfo.format,
            .size = wmsinfo.stride * wmsinfo.height,
            .data = nullptr,
        };
        auto data = mmap(nullptr, wminfo.size, PROT_READ, MAP_SHARED, wmsinfo.fd, 0);
        wminfo.data = data;

        cb->OnWindowShot(wminfo);

        // 0xffffffff
        uint8_t *errPtr = nullptr;
        errPtr--;
        if (data != errPtr) {
            munmap(data, wminfo.size);
        }
    };
    promise->Then(then);
    return GSERROR_OK;
}
} // namespace OHOS
