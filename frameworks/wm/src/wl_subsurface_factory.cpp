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

#include "wl_subsurface_factory.h"

#include <mutex>

namespace OHOS {
sptr<WlSubsurfaceFactory> WlSubsurfaceFactory::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WlSubsurfaceFactory();
        }
    }
    return instance;
}

void WlSubsurfaceFactory::Init()
{
    delegator.Dep<WaylandService>()->OnAppear(&WlSubsurfaceFactory::OnAppear);
}

void WlSubsurfaceFactory::Deinit()
{
    if (subcompositor != nullptr) {
        wl_subcompositor_destroy(subcompositor);
        subcompositor = nullptr;
    }
}

void WlSubsurfaceFactory::OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver)
{
    constexpr uint32_t wlSubcompositorVersion = 1;
    if (iname == "wl_subcompositor") {
        auto ret = get(&wl_subcompositor_interface, wlSubcompositorVersion);
        subcompositor = static_cast<struct wl_subcompositor *>(ret);
    }
}

sptr<WlSubsurface> WlSubsurfaceFactory::Create(const sptr<WlSurface> &surf, const sptr<WlSurface> &parent)
{
    struct wl_subsurface *subsurf = nullptr;
    if (subcompositor != nullptr &&
        surf != nullptr && surf->GetRawPtr() != nullptr &&
        parent != nullptr && parent->GetRawPtr() != nullptr) {
        subsurf = wl_subcompositor_get_subsurface(subcompositor, surf->GetRawPtr(), parent->GetRawPtr());
    }

    if (subsurf) {
        sptr<WlSubsurface> wlSubsurf = new WlSubsurface(subsurf);
        return wlSubsurf;
    }
    return nullptr;
}
} // namespace OHOS
