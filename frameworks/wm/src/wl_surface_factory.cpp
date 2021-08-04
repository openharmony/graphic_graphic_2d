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

#include "wl_surface_factory.h"

#include <mutex>

namespace OHOS {
sptr<WlSurfaceFactory> WlSurfaceFactory::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WlSurfaceFactory();
        }
    }
    return instance;
}

void WlSurfaceFactory::Init()
{
    delegator.Dep<WaylandService>()->OnAppear(&WlSurfaceFactory::OnAppear);
}

void WlSurfaceFactory::Deinit()
{
    if (compositor != nullptr) {
        wl_compositor_destroy(compositor);
        compositor = nullptr;
    }
}

void WlSurfaceFactory::OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver)
{
    constexpr uint32_t wlCompositorVersion = 2;
    if (iname == "wl_compositor") {
        auto ret = get(&wl_compositor_interface, wlCompositorVersion);
        compositor = static_cast<struct wl_compositor *>(ret);
    }
}

sptr<WlSurface> WlSurfaceFactory::Create()
{
    if (compositor == nullptr) {
        return nullptr;
    }

    auto surface = wl_compositor_create_surface(compositor);
    sptr<WlSurface> ret = new WlSurface(surface);
    return ret;
}
} // namespace OHOS
