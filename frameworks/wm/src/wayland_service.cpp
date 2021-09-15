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

#include "wayland_service.h"

#include <mutex>

#include <wayland-client-protocol.h>
#include <window_manager_input_type.h>

#include "window_manager_hilog.h"
#include "wl_display.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMWaylandService"};
std::function<void(uint32_t, const std::string, uint32_t)> g_appear;
std::function<void(uint32_t)> g_remove;
} // namespace

sptr<WaylandService> WaylandService::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WaylandService();
        }
    }
    return instance;
}

void WaylandService::OnAppear(const AppearListenFunc &func)
{
    if (func != nullptr) {
        appearListenFuncs.push_back(func);
    }
}

void WaylandService::OnRemove(const RemoveListenFunc &func)
{
    if (func != nullptr) {
        removeListenFuncs.push_back(func);
    }
}

void WaylandService::Appear(uint32_t id, const std::string iname, uint32_t ver) const
{
    auto getService = std::bind(wl_registry_bind,
        registry, id, std::placeholders::_1, std::placeholders::_2);

    for (const auto &func : appearListenFuncs) {
        func(getService, iname, ver);
    }
}

void WaylandService::Remove(uint32_t name) const
{
    for (const auto &func : removeListenFuncs) {
        func(name);
    }
}

namespace {
void RegistryGlobal(void *, struct wl_registry *, uint32_t id, const char *iname, uint32_t ver)
{
    g_appear(id, iname, ver);
}

void RegistryGlobalRemove(void *, struct wl_registry *, uint32_t name)
{
    g_remove(name);
}
} // namespace

WMError WaylandService::Start()
{
    const auto &display = delegator.Dep<WlDisplay>();
    const auto &wlDisplay = display->GetRawPtr();
    if (wlDisplay == nullptr) {
        WMLOGFE("wl_display is nullptr");
        return WM_ERROR_NOT_INIT;
    }

    registry = wl_display_get_registry(wlDisplay);
    if (registry == nullptr) {
        WMLOGFE("wl_display_get_registry failed");
        return WM_ERROR_API_FAILED;
    }

    const struct wl_registry_listener listener = { RegistryGlobal, RegistryGlobalRemove };
    if (wl_registry_add_listener(registry, &listener, nullptr) ==  -1) {
        WMLOGFE("wl_registry_add_listener failed");
        wl_registry_destroy(registry);
        registry = nullptr;
        return WM_ERROR_API_FAILED;
    }

    g_appear = std::bind(&WaylandService::Appear, this, __BIND3_ARGS);
    g_remove = std::bind(&WaylandService::Remove, this, __BIND1_ARGS);
    return WM_OK;
}

void WaylandService::Stop()
{
    if (registry != nullptr) {
        wl_registry_destroy(registry);
        registry = nullptr;
    }
    g_appear = nullptr;
    g_remove = nullptr;
    appearListenFuncs.clear();
    removeListenFuncs.clear();
}
} // namespace OHOS
