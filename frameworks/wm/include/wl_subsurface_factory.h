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

#ifndef FRAMEWORKS_WM_INCLUDE_WL_SUBSURFACE_FACTORY_H
#define FRAMEWORKS_WM_INCLUDE_WL_SUBSURFACE_FACTORY_H

#include <refbase.h>
#include <wayland-client-protocol.h>

#include "singleton_delegator.h"
#include "wayland_service.h"
#include "wl_subsurface.h"
#include "wl_surface.h"

namespace OHOS {
class WlSubsurfaceFactory : public RefBase {
public:
    static sptr<WlSubsurfaceFactory> GetInstance();

    MOCKABLE void Init();
    MOCKABLE void Deinit();

    MOCKABLE sptr<WlSubsurface> Create(const sptr<WlSurface> &surface, const sptr<WlSurface> &parent);

private:
    WlSubsurfaceFactory() = default;
    MOCKABLE ~WlSubsurfaceFactory() = default;
    static inline sptr<WlSubsurfaceFactory> instance = nullptr;
    static inline SingletonDelegator<WlSubsurfaceFactory> delegator;

    static void OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver);
    static inline struct wl_subcompositor *subcompositor = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WL_SUBSURFACE_FACTORY_H
