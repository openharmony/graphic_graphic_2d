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

#ifndef FRAMEWORKS_WM_INCLUDE_WAYLAND_SERVICE_H
#define FRAMEWORKS_WM_INCLUDE_WAYLAND_SERVICE_H

#include <functional>
#include <string>
#include <vector>

#include <wm_common.h>
#include <refbase.h>
#include <wayland-client-protocol.h>

#include "singleton_delegator.h"

namespace OHOS {
using GetServiceFunc = std::function<void *(const struct wl_interface *interface, uint32_t ver)>;
using AppearListenFunc = std::function<void(
    const GetServiceFunc get, const std::string &iname, uint32_t ver)>;
using RemoveListenFunc = std::function<void(uint32_t name)>;

class WaylandService : public RefBase {
public:
    static sptr<WaylandService> GetInstance();

    // client
    MOCKABLE void OnAppear(const AppearListenFunc &func);
    MOCKABLE void OnRemove(const RemoveListenFunc &func);

    // server
    MOCKABLE WMError Start();
    MOCKABLE void Stop();

private:
    WaylandService() = default;
    MOCKABLE ~WaylandService() = default;
    static inline sptr<WaylandService> instance = nullptr;
    static inline SingletonDelegator<WaylandService> delegator;

    void Appear(uint32_t id, const std::string iname, uint32_t ver) const;
    void Remove(uint32_t name) const;

    struct wl_registry *registry = nullptr;
    std::vector<AppearListenFunc> appearListenFuncs;
    std::vector<RemoveListenFunc> removeListenFuncs;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WAYLAND_SERVICE_H
