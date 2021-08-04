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

#include "wp_viewport_factory.h"

#include <mutex>

namespace OHOS {
sptr<WpViewportFactory> WpViewportFactory::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WpViewportFactory();
        }
    }
    return instance;
}

void WpViewportFactory::Init()
{
    delegator.Dep<WaylandService>()->OnAppear(&WpViewportFactory::OnAppear);
}

void WpViewportFactory::Deinit()
{
    if (viewporter != nullptr) {
        wp_viewporter_destroy(viewporter);
        viewporter = nullptr;
    }
}

void WpViewportFactory::OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver)
{
    constexpr uint32_t wpViewporterVersion = 1;
    if (iname == "wp_viewporter") {
        auto ret = get(&wp_viewporter_interface, wpViewporterVersion);
        viewporter = static_cast<struct wp_viewporter *>(ret);
    }
}

sptr<WpViewport> WpViewportFactory::Create(sptr<WlSurface> &surface)
{
    if (viewporter == nullptr) {
        return nullptr;
    }

    auto viewport = wp_viewporter_get_viewport(viewporter, surface->GetRawPtr());
    sptr<WpViewport> ret = new WpViewport(viewport);
    return ret;
}
} // namespace OHOS
