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

#include <display_type.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "video_window.h"
#include "window_manager_controller_client.h"
#include "window_manager_hilog.h"
#include "window_manager_proxy.h"

namespace OHOS {
namespace {
sptr<IWindowManagerService> g_windowManagerService = nullptr;
constexpr float BAR_WIDTH_PERCENT = 0.07;
constexpr int ALARM_WINDOW_WIDTH = 400;
constexpr int ALARM_WINDOW_HEIGHT = 300;
constexpr int ALARM_WINDOW_WIDTH_HALF = 200;
constexpr int ALARM_WINDOW_HEIGHT_HALF = 150;
constexpr int ALARM_WINDOW_HALF = 2;
}

WindowBase::WindowBase(int32_t windowid, sptr<Surface>& surface)
{
    WMLOG_I("DEBUG WindowBase");
    m_windowid = windowid;
    surface_ = surface;
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
    if (g_windowManagerService == nullptr) {
        constexpr int32_t retryTimes = 60;
        for (int32_t i = 0; i < retryTimes; i++) {
            constexpr int32_t sleepTimeFactor = 50 * 1000;
            int32_t sleepTime = i * sleepTimeFactor;
            auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (sam == nullptr) {
                WMLOG_E("samgr == nullptr, retry %{public}d/%{public}d", i, retryTimes);
                usleep(sleepTime);
                continue;
            }

            auto object = sam->GetSystemAbility(WINDOW_MANAGER_ID);
            if (object == nullptr) {
                WMLOG_E("object == nullptr, retry %{public}d/%{public}d", i, retryTimes);
                usleep(sleepTime);
                continue;
            }

            g_windowManagerService = iface_cast<IWindowManagerService>(object);
            if (g_windowManagerService == nullptr) {
                WMLOG_E("service == nullptr, retry %{public}d/%{public}d", i, retryTimes);
                usleep(sleepTime);
                continue;
            }

            break;
        }
        WMLOG_E("init failed");
    }
    WMLOG_I("WindowManager::init end");
}

static void ProcessWindowConfig(WindowConfig &config)
{
    config.subwindow = false;
    config.width = LayerControllerClient::GetInstance()->GetMaxWidth();
    config.pos_x = 0;
    int maxWitdh = LayerControllerClient::GetInstance()->GetMaxWidth();
    int maxHeight = LayerControllerClient::GetInstance()->GetMaxHeight();
    int barHeight = static_cast<int>(BAR_WIDTH_PERCENT * maxHeight);
    int allBarsHeight = barHeight + barHeight;
    switch (config.type) {
        case WINDOW_TYPE_NORMAL:
            config.height = maxHeight - allBarsHeight;
            config.pos_y = barHeight;
            break;
        case WINDOW_TYPE_STATUS_BAR:
            config.height = barHeight;
            config.pos_y = 0;
            break;
        case WINDOW_TYPE_NAVI_BAR:
            config.height = barHeight;
            config.pos_y = maxHeight - config.height;
            break;
        case WINDOW_TYPE_ALARM_SCREEN:
            config.width = ALARM_WINDOW_WIDTH;
            config.height = ALARM_WINDOW_HEIGHT;
            config.pos_x = maxWitdh / ALARM_WINDOW_HALF - ALARM_WINDOW_WIDTH_HALF;
            config.pos_y = maxHeight / ALARM_WINDOW_HALF - ALARM_WINDOW_HEIGHT_HALF;
            break;
        default:
            config.height = maxHeight - allBarsHeight;
            config.pos_y = barHeight;
            break;
    }
}

std::unique_ptr<Window> WindowManager::CreateWindow(WindowConfig* config)
{
    WMLOG_I("WindowManager::CreateWindow start");
    if (config == nullptr) {
        WMLOG_I("WindowManager::CreateWindow config is nullptr");
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

    ProcessWindowConfig(*config);

    int id = g_windowManagerService->CreateWindow(*config);
    WMLOG_I("WindowManager::CreateWindow widow ID is %{public}d", id);

    InnerWindowInfo* windowInfo = LayerControllerClient::GetInstance()->CreateWindow(id, *config);
    if (windowInfo == nullptr) {
        WMLOG_I("WindowManager::CreateWindow widow ID  %{public}d failed", id);
        return nullptr;
    }

    auto producer = windowInfo->surface->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    std::unique_ptr<Window> ret_win = std::make_unique<Window>(windowInfo->windowid, surface);

    BufferRequestConfig requestConfig = {
        .width = config->width,
        .height = config->height,
        .strideAlignment = 8,
        .format = config->format,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    ret_win->SetRequestConfig(requestConfig);

    WMLOG_I("WindowManager::CreateWindow widow ID  %{public}d success", id);
    return ret_win;
}

std::unique_ptr<SubWindow> WindowManager::CreateSubWindow(int32_t parentid, WindowConfig* config)
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

    int id = g_windowManagerService->CreateWindow(*config);
    WMLOG_I("WindowManager::CreateSubWindow widow ID is %{public}d", id);

    InnerWindowInfo* windowInfo = LayerControllerClient::GetInstance()->CreateSubWindow(id, parentid, *config);
    if (windowInfo == nullptr) {
        WMLOG_I("WindowManager::CreateSubWindow widow ID  %{public}d failed", id);
        return nullptr;
    }
    std::unique_ptr<SubWindow> ret_win;
    if (config->type != WINDOW_TYPE_VIDEO || windowInfo->voLayerId == -1U) {
        auto producer = windowInfo->surface->GetProducer();
        sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
        ret_win = std::make_unique<SubWindow>(windowInfo->windowid, surface);
        BufferRequestConfig requestConfig = {
            .width = config->width,
            .height = config->height,
            .strideAlignment = 8,
            .format = config->format,
            .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
            .timeout = 0,
        };
        ret_win->SetRequestConfig(requestConfig);
    } else {
        ret_win = std::make_unique<VideoWindow>(*windowInfo);
    }
    WMLOG_I("WindowManager::CreateSubWindow widow ID  %{public}d success", id);
    return ret_win;
}

void WindowManager::StartShotScreen(FuncShotDone done_cb)
{
    WMLOG_I("WindowManager::StartShotScreen start type");
    LayerControllerClient::GetInstance()->StartShotScreen(done_cb);
}

void WindowManager::StartShotWindow(int32_t winID, FuncShotDone done_cb)
{
    WMLOG_I("WindowManager::StartShotWindow start winID = %{public}d", winID);
    LayerControllerClient::GetInstance()->StartShotWindow(winID, done_cb);
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
    if (g_windowManagerService != nullptr) {
        g_windowManagerService->DestroyWindow(windowId);
    }
    LayerControllerClient::GetInstance()->DestroyWindow(windowId);
    WMLOG_I("WindowsManager::DestroyWindow end");
}

Window::Window(int32_t windowid, sptr<Surface>& surface) : WindowBase(windowid, surface)
{
    WMLOG_I("DEBUG Window");
}

Window::~Window()
{
    WMLOG_I("DEBUG ~Window id(%{public}d)", m_windowid);
    if (g_windowManagerService != nullptr) {
        g_windowManagerService->DestroyWindow(m_windowid);
    }

    LayerControllerClient::GetInstance()->DestroyWindow(m_windowid);
}

void Window::RegistPointerButtonCb(funcPointerButton cb)
{
    WMLOG_I("Window::RegistPointerButtonCb start");
    LayerControllerClient::GetInstance()->RegistPointerButtonCb(m_windowid, cb);
    WMLOG_I("Window::RegistPointerButtonCb end");
}

void Window::RegistPointerEnterCb(funcPointerEnter cb)
{
    WMLOG_I("Window::RegistPointerEnterCb start");
    LayerControllerClient::GetInstance()->RegistPointerEnterCb(m_windowid, cb);
    WMLOG_I("Window::RegistPointerEnterCb end");
}

void Window::RegistPointerLeaveCb(funcPointerLeave cb)
{
    WMLOG_I("Window::RegistPointerLeaveCb start");
    LayerControllerClient::GetInstance()->RegistPointerLeaveCb(m_windowid, cb);
    WMLOG_I("Window::RegistPointerLeaveCb end");
}

void Window::RegistPointerMotionCb(funcPointerMotion cb)
{
    WMLOG_I("Window::RegistPointerMotionCb start");
    LayerControllerClient::GetInstance()->RegistPointerMotionCb(m_windowid, cb);
    WMLOG_I("Window::RegistPointerMotionCb end");
}

void Window::RegistPointerAxisDiscreteCb(funcPointerAxisDiscrete cb)
{
    WMLOG_I("Window::RegistPointerAxisDiscreteCb start");
    LayerControllerClient::GetInstance()->RegistPointerAxisDiscreteCb(m_windowid, cb);
    WMLOG_I("Window::RegistPointerAxisDiscreteCb end");
}
void Window::RegistPointerAxisSourceCb(funcPointerAxisSource cb)
{
    WMLOG_I("Window::RegistPointerAxisSourceCb start");
    LayerControllerClient::GetInstance()->RegistPointerAxisSourceCb(m_windowid, cb);
    WMLOG_I("Window::RegistPointerAxisSourceCb end");
}
void Window::RegistPointerAxisStopCb(funcPointerAxisStop cb)
{
    WMLOG_I("Window::RegistPointerAxisStopCb start");
    LayerControllerClient::GetInstance()->RegistPointerAxisStopCb(m_windowid, cb);
    WMLOG_I("Window::RegistPointerAxisStopCb end");
}

void Window::RegistPointerAxisCb(funcPointerAxis cb)
{
    WMLOG_I("Window::RegistPointerAxisCb start");
    LayerControllerClient::GetInstance()->RegistPointerAxisCb(m_windowid, cb);
    WMLOG_I("Window::RegistPointerAxisCb end");
}

void Window::RegistTouchUpCb(funcTouchUp cb)
{
    WMLOG_I("Window::RegistTouchUpCb start");
    LayerControllerClient::GetInstance()->RegistTouchUpCb(m_windowid, cb);
    WMLOG_I("Window::RegistTouchUpCb end");
}

void Window::RegistTouchDownCb(funcTouchDown cb)
{
    WMLOG_I("Window::RegistTouchDownCb start");
    LayerControllerClient::GetInstance()->RegistTouchDownCb(m_windowid, cb);
    WMLOG_I("Window::RegistTouchDownCb end");
}

void Window::RegistTouchEmotionCb(funcTouchEmotion cb)
{
    WMLOG_I("Window::RegistTouchEmotionCb start");
    LayerControllerClient::GetInstance()->RegistTouchEmotionCb(m_windowid, cb);
    WMLOG_I("Window::RegistTouchEmotionCb end");
}

void Window::RegistTouchFrameCb(funcTouchFrame cb)
{
    WMLOG_I("Window::RegistTouchFrameCb start");
    LayerControllerClient::GetInstance()->RegistTouchFrameCb(m_windowid, cb);
    WMLOG_I("Window::RegistTouchFrameCb end");
}
void Window::RegistTouchCancelCb(funcTouchCancel cb)
{
    WMLOG_I("Window::RegistTouchCancelCb start");
    LayerControllerClient::GetInstance()->RegistTouchCancelCb(m_windowid, cb);
    WMLOG_I("Window::RegistTouchCancelCb end");
}
void Window::RegistTouchShapeCb(funcTouchShape cb)
{
    WMLOG_I("Window::RegistTouchShapeCb start");
    LayerControllerClient::GetInstance()->RegistTouchShapeCb(m_windowid, cb);
    WMLOG_I("Window::RegistTouchShapeCb end");
}
void Window::RegistTouchOrientationCb(funcTouchOrientation cb)
{
    WMLOG_I("Window::RegistTouchOrientationCb start");
    LayerControllerClient::GetInstance()->RegistTouchOrientationCb(m_windowid, cb);
    WMLOG_I("Window::RegistTouchOrientationCb end");
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

SubWindow::SubWindow(int32_t windowid, sptr<Surface>& surface) : WindowBase(windowid, surface)
{
    WMLOG_I("DEBUG SubWindow");
}

SubWindow::~SubWindow()
{
    WMLOG_I("DEBUG ~SubWindow id(%{public}d)", m_windowid);
    if (g_windowManagerService != nullptr) {
        g_windowManagerService->DestroyWindow(m_windowid);
    }
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
    WMLOG_I("Window::SetSubWindowSize end");
}
}
