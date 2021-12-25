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

#include "window_manager_server.h"

#include "window_manager_hilog.h"
#include "wl_display.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMWindowManagerServer"};
} // namespace

sptr<WindowManagerServer> WindowManagerServer::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WindowManagerServer();
        }
    }
    return instance;
}

void WindowManagerServer::Init()
{
    delegator.Dep<WaylandService>()->OnAppear(&WindowManagerServer::OnAppear);
}

void WindowManagerServer::Deinit()
{
    if (wms != nullptr) {
        wms_destroy(wms);
        wms = nullptr;
    }
}

void WindowManagerServer::OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver)
{
    constexpr uint32_t wmsVersion = 1;
    if (iname == "wms") {
        auto ret = get(&wms_interface, wmsVersion);
        wms = static_cast<struct wms *>(ret);
        const struct wms_listener listener = {
            .window_status = &WindowManagerServer::OnWindowChange,
            .window_size_change = &WindowManagerServer::OnWindowSizeChange,
            .split_mode_change = &WindowManagerServer::OnSplitStatusChange,
        };
        wms_add_listener(wms, &listener, nullptr);
    }
}

void WindowManagerServer::OnWindowChange(void *, struct wms *,
    uint32_t status, uint32_t wid, int32_t x, int32_t y, int32_t width, int32_t height)
{
    WMLOGFI("status: %{public}d, wid: %{public}d, (%{public}d, %{public}d), %{public}dx%{public}d",
        status, wid, x, y, width, height);

    if (status == WMS_WINDOW_STATUS_CREATED) {
        struct WMSWindowInfo info = {
            .wret = GSERROR_OK,
            .wid = wid,
            .x = x,
            .y = y,
            .width = width,
            .height = height,
        };
        promiseQueue.front()->Resolve(info);
        promiseQueue.pop();
    }

    if (status == WMS_WINDOW_STATUS_FAILED) {
        struct WMSWindowInfo info = { .wret = GSERROR_SERVER_ERROR, .wid = -1, };
        promiseQueue.front()->Resolve(info);
        promiseQueue.pop();
    }
}

void WindowManagerServer::OnWindowSizeChange(void *, struct wms *, int32_t width, int32_t height)
{
    WMLOGFI("%{public}dx%{public}d", width, height);
    if (onWindowSizeChange) {
        onWindowSizeChange(width, height);
    }
}

void WindowManagerServer::OnSplitStatusChange(void *, struct wms *, uint32_t status)
{
    WMLOGFI("OnSplitStatusChange status: %{public}u", status);
    if (onSplitModeChange) {
        onSplitModeChange(static_cast<enum SplitStatus>(status));
    }
}

sptr<Promise<struct WMSWindowInfo>> WindowManagerServer::CreateWindow(
    const sptr<WlSurface> &wlSurface, int32_t did, WindowType type)
{
    WMLOGFI("wlSurface: %{public}s, did: %{public}d, type: %{public}d",
            (wlSurface != nullptr) ? "Yes" : "No", did, type);
    sptr<Promise<struct WMSWindowInfo>> ret = new Promise<struct WMSWindowInfo>();
    if (wlSurface == nullptr) {
        struct WMSWindowInfo info = { .wret = GSERROR_INVALID_ARGUMENTS, .wid = -1, };
        ret->Resolve(info);
        return ret;
    }
    promiseQueue.push(ret);

    wms_create_window(wms, wlSurface->GetRawPtr(), did, type);
    delegator.Dep<WlDisplay>()->Flush();
    return ret;
}

void WindowManagerServer::RegisterSplitModeChange(SplitStatusChangeFunc func)
{
    onSplitModeChange = func;
}

void WindowManagerServer::RegisterWindowSizeChange(WindowSizeChangeFunc func)
{
    onWindowSizeChange = func;
}
} // namespace OHOS
