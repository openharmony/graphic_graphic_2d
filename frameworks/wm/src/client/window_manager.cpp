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

#include "window_manager.h"

#include <cstring>
#include <sys/mman.h>

#include <display_type.h>
#include <iservice_registry.h>
#include <securec.h>
#include <system_ability_definition.h>
#include <window_manager_service_client.h>

#include "video_window.h"
#include "window_manager_controller_client.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace {
sptr<IWindowManagerService> g_windowManagerService = nullptr;
}

WindowBase::WindowBase(int32_t windowid, sptr<Surface>& surf)
{
    WMLOG_I("DEBUG WindowBase");
    m_windowid = windowid;
    surface_ = surf;
    BufferRequestConfig config = {
        .width = 0,
        .height = 0,
        .strideAlignment = 0,
        .format = 0,
        .usage = 0,
        .timeout = 0,
    };
    config_ = config;
}

WindowBase::~WindowBase()
{
    WMLOG_I("DEBUG ~WindowBase");
    surface_ = nullptr;
}

void WindowBase::GetRequestConfig(BufferRequestConfig &config)
{
    config = config_;
}

void WindowBase::SetRequestConfig(BufferRequestConfig &config)
{
    config_ = config;
}

void WindowBase::RegistWindowInfoChangeCb(funcWindowInfoChange cb)
{
    WMLOG_I("WindowBase::RegistWindowInfoChangeCb start");
    LayerControllerClient::GetInstance()->RegistWindowInfoChangeCb(m_windowid, cb);
    WMLOG_I("WindowBase::RegistWindowInfoChangeCb end");
}

int32_t WindowBase::GetWindowID()
{
    return m_windowid;
}

sptr<Surface> WindowBase::GetSurface()
{
    return surface_;
}

WindowManager::WindowManager()
{
    WMLOG_I("DEBUG WindowManager");
    init();
}

WindowManager::~WindowManager()
{
    WMLOG_I("DEBUG ~WindowManager");
    WMLOG_I("WindowManager::~WindowManager");
}

sptr<WindowManager> WindowManager::instance = nullptr;
sptr<WindowManager> WindowManager::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WindowManager();
        }
    }

    return instance;
}

void WindowManager::init()
{
    WMLOG_I("WindowManager::init start");
    if (WlDisplay::GetInstance()->Connect(nullptr) == false) {
        WMLOG_E("create display failed!");
        exit(1);
    }

    if (g_windowManagerService == nullptr) {
        auto wmsc = WindowManagerServiceClient::GetInstance();
        GSError wret = wmsc->Init();
        if (wret != GSERROR_OK) {
            WMLOG_E("WMService init failed");
            fprintf(stderr, "WMService init failed");
            exit(1);
        }
        g_windowManagerService = wmsc->GetService();
    }
    WMLOG_I("WindowManager::init end");

    if (LayerControllerClient::GetInstance()->init(g_windowManagerService) == false) {
        exit(1);
    }
}

namespace {
sptr<WlSurface> CreateWlSurface(WindowConfig *config)
{
    if (config == nullptr) {
        WMLOG_I("WindowManager::CreateWindow config is nullptr");
        return nullptr;
    }

    auto wlSurface = WlSurfaceFactory::GetInstance()->Create();
    if (wlSurface == nullptr) {
        WMLOG_I("Error: CreateSurface WlSurfaceFactory::Create failed");
        return nullptr;
    }
    return wlSurface;
}
}

std::unique_ptr<Window> WindowManager::CreateWindow(WindowConfig *config)
{
    WMLOG_I("WindowManager::CreateWindow start");
    if (g_windowManagerService == nullptr) {
        WMLOG_I("WindowManager::%{public}s g_windowManagerService is nullptr init again", __func__);
        init();
        if (g_windowManagerService == nullptr) {
            WMLOG_I("WindowManager::%{public}s widow failed", __func__);
            return nullptr;
        }
    }

    auto wlSurface = CreateWlSurface(config);
    if (wlSurface == nullptr) {
        return nullptr;
    }

    auto wms = WindowManagerServer::GetInstance();
    auto promise = wms->CreateWindow(wlSurface, 0, static_cast<WindowType>(config->type));
    auto wminfo = promise->Await();
    if (wminfo.wret != GSERROR_OK) {
        WMLOG_E("Error: wms->CreateWindow failed, %{public}s", GSErrorStr(wminfo.wret).c_str());
        return nullptr;
    }
    WMLOG_I("WindowManager::CreateWindow widow ID is %{public}d", wminfo.wid);

    config->pos_x = wminfo.x;
    config->pos_y = wminfo.y;
    config->width = wminfo.width;
    config->height = wminfo.height;

    InnerWindowInfo *windowInfo = LayerControllerClient::GetInstance()->CreateWindow(wminfo.wid, *config);
    if (windowInfo == nullptr) {
        WMLOG_I("WindowManager::CreateWindow widow ID  %{public}d failed", wminfo.wid);
        return nullptr;
    }

    windowInfo->wlSurface = wlSurface;
    windowInfo->wlSurface->SetUserData(&windowInfo->windowid);

    auto producer = windowInfo->surf->GetProducer();
    sptr<Surface> surf = Surface::CreateSurfaceAsProducer(producer);
    std::unique_ptr<Window> ret_win = std::make_unique<Window>(windowInfo->windowid, surf);

    BufferRequestConfig requestConfig = {
        .width = wminfo.width,
        .height = wminfo.height,
        .strideAlignment = 0x8,
        .format = config->format,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    ret_win->SetRequestConfig(requestConfig);

    WMLOG_I("WindowManager::CreateWindow widow ID  %{public}d success", wminfo.wid);
    return ret_win;
}

std::unique_ptr<SubWindow> WindowManager::CreateSubWindow(int32_t parentid, WindowConfig *config)
{
    WMLOG_I("WindowManager::CreateSubWindow start");
    if (config == nullptr) {
        WMLOG_I("WindowManager::CreateSubWindow config is nullptr");
        return nullptr;
    }

    if (g_windowManagerService == nullptr) {
        WMLOG_I("WindowManager::%{public}s g_windowManagerService is nullptr init again", __func__);
        init();
        if (g_windowManagerService == nullptr) {
            WMLOG_I("WindowManager::%{public}s widow failed", __func__);
            return nullptr;
        }
    }

    config->subwindow = true;
    config->parentid = parentid;

    constexpr uint32_t scale = 320;
    static int32_t id = getpid() * scale - 1;
    id++;
    WMLOG_I("WindowManager::CreateSubWindow widow ID is %{public}d", id);

    InnerWindowInfo *windowInfo = LayerControllerClient::GetInstance()->CreateSubWindow(id, parentid, *config);
    if (windowInfo == nullptr) {
        WMLOG_I("WindowManager::CreateSubWindow widow ID  %{public}d failed", id);
        return nullptr;
    }

    std::unique_ptr<SubWindow> ret_win;
    if (config->type != WINDOW_TYPE_VIDEO || windowInfo->voLayerId == -1U) {
        auto producer = windowInfo->surf->GetProducer();
        sptr<Surface> surf = Surface::CreateSurfaceAsProducer(producer);
        ret_win = std::make_unique<SubWindow>(windowInfo->windowid, surf);
    } else {
        ret_win = std::make_unique<VideoWindow>(*windowInfo);
    }
    BufferRequestConfig requestConfig = {
        .width = config->width,
        .height = config->height,
        .strideAlignment = 0x8,
        .format = config->format,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    ret_win->SetRequestConfig(requestConfig);
    WMLOG_I("WindowManager::CreateSubWindow widow ID  %{public}d success", id);
    return ret_win;
}

void WindowManager::StartShotScreen(IScreenShotCallback *cb)
{
    WMLOG_I("WindowManager::StartShotScreen start type");
    auto promise = g_windowManagerService->ShotScreen(0);
    if (promise == nullptr) {
        WMLOG_E("promise is nullptr");
        return;
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
}

void WindowManager::StartShotWindow(int32_t id, IWindowShotCallback *cb)
{
    WMLOG_I("WindowManager::StartShotWindow start winID = %{public}d", id);
    auto promise = g_windowManagerService->ShotScreen(id);
    if (promise == nullptr) {
        WMLOG_E("promise is nullptr");
        return;
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
            WMLOG_I("StartShotScreen memset_s failed");
        }

        if (data != errPtr) {
            munmap(data, wminfo.size);
        }
    };
    promise->Then(then);
}

int32_t WindowManager::GetMaxWidth()
{
    WMLOG_I("WindowManager::GetMaxWidth start");
    return LayerControllerClient::GetInstance()->GetMaxWidth();
}

int32_t WindowManager::GetMaxHeight()
{
    WMLOG_I("WindowManager::GetMaxHeigth start");
    return LayerControllerClient::GetInstance()->GetMaxHeight();
}

void WindowManager::SwitchTop(int32_t windowId)
{
    WMLOG_I("WindowsManager::SwitchTop start id(%{public}d)", windowId);
    if (g_windowManagerService != nullptr) {
        g_windowManagerService->SwitchTop(windowId);
    }
    WMLOG_I("WindowsManager::SwitchTop end");
}

void WindowManager::DestroyWindow(int32_t windowId)
{
    WMLOG_I("WindowsManager::DestroyWindow start id(%{public}d)", windowId);
    LayerControllerClient::GetInstance()->DestroyWindow(windowId);
    WMLOG_I("WindowsManager::DestroyWindow end");
}

Window::Window(int32_t windowid, sptr<Surface>& surf) : WindowBase(windowid, surf)
{
    WMLOG_I("DEBUG Window");
}

Window::~Window()
{
    WMLOG_I("DEBUG ~Window id(%{public}d)", m_windowid);
    LayerControllerClient::GetInstance()->DestroyWindow(m_windowid);
}

void Window::RegistOnTouchCb(funcOnTouch cb)
{
    WMLOG_I("Window::RegistOnTouchCb start, windowid %{public}d", this->m_windowid);
    LayerControllerClient::GetInstance()->RegistOnTouchCb(m_windowid, cb);
    WMLOG_I("Window::RegistOnTouchCb end windowid %{public}d", this->m_windowid);
}

void Window::RegistOnKeyCb(funcOnKey cb)
{
    WMLOG_I("Window::RegistOnKeyCb start");
    LayerControllerClient::GetInstance()->RegistOnKeyCb(m_windowid, cb);
    WMLOG_I("Window::RegistOnKeyCb end");
}
void Window::Move(int32_t x, int32_t y)
{
    WMLOG_I("Window::Move start");
    LayerControllerClient::GetInstance()->Move(m_windowid, x, y);
    WMLOG_I("Window::Move end");
}

void Window::Hide()
{
    WMLOG_I("Window::Hide start");
    LayerControllerClient::GetInstance()->Hide(m_windowid);
    WMLOG_I("Window::Hide end");
}

void Window::Show()
{
    WMLOG_I("Window::Show start");
    LayerControllerClient::GetInstance()->Show(m_windowid);
    WMLOG_I("Window::Show end");
}

void Window::SwitchTop()
{
    WMLOG_I("Window::SwitchTop start");
    if (g_windowManagerService) {
        g_windowManagerService->SwitchTop(m_windowid);
    }
    WMLOG_I("Window::SwitchTop end");
}

void Window::ReSize(int32_t width, int32_t height)
{
    WMLOG_I("Window::Resize start");
    config_.width = width;
    config_.height = height;
    LayerControllerClient::GetInstance()->ReSize(m_windowid, width, height);
    WMLOG_I("Window::Resize end");
}

void Window::Rotate(rotateType type)
{
    WMLOG_I("Window::Rotate start");
    LayerControllerClient::GetInstance()->Rotate(m_windowid, static_cast<int32_t>(type));
    WMLOG_I("Window::Rotate end");
}

void Window::ChangeWindowType(WindowType type)
{
    WMLOG_I("Window::ChangeWindowType start");
    LayerControllerClient::GetInstance()->ChangeWindowType(m_windowid, type);
    WMLOG_I("Window::ChangeWindowType end");
}

void Window::RegistOnWindowCreateCb(void(* cb)(uint32_t pid))
{
    LayerControllerClient::GetInstance()->RegistOnWindowCreateCb(m_windowid, cb);
}

SubWindow::SubWindow(int32_t windowid, sptr<Surface>& surf) : WindowBase(windowid, surf)
{
    WMLOG_I("DEBUG SubWindow");
}

SubWindow::~SubWindow()
{
    WMLOG_I("DEBUG ~SubWindow id(%{public}d)", m_windowid);
    LayerControllerClient::GetInstance()->DestroyWindow(m_windowid);
}

void SubWindow::Move(int32_t x, int32_t y)
{
    WMLOG_I("Window::Move start");
    LayerControllerClient::GetInstance()->Move(m_windowid, x, y);
    WMLOG_I("Window::Move end");
}

void SubWindow::SetSubWindowSize(int32_t width, int32_t height)
{
    WMLOG_I("Window::SetSubWindowSize start");
    LayerControllerClient::GetInstance()->ReSize(m_windowid, width, height);
    config_.width = width;
    config_.height = height;
    WMLOG_I("Window::SetSubWindowSize end");
}
}
