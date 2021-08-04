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

#ifndef FRAMEWORKS_WM_INCLUDE_WL_SHM_BUFFER_FACTORY_H
#define FRAMEWORKS_WM_INCLUDE_WL_SHM_BUFFER_FACTORY_H

#include <refbase.h>

#include "singleton_delegator.h"
#include "wayland_service.h"
#include "wl_buffer.h"

namespace OHOS {
class WlSHMBufferFactory : public RefBase {
public:
    static sptr<WlSHMBufferFactory> GetInstance();

    MOCKABLE void Init();
    MOCKABLE void Deinit();

    MOCKABLE sptr<WlBuffer> Create(uint32_t w, uint32_t h, int32_t format);

private:
    WlSHMBufferFactory() = default;
    MOCKABLE ~WlSHMBufferFactory() = default;
    static inline sptr<WlSHMBufferFactory> instance = nullptr;
    static inline SingletonDelegator<WlSHMBufferFactory> delegator;

    static void OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver);
    static inline struct wl_shm *shmbuf = nullptr;

    wl_shm_pool *CreatePool(uint32_t mmapSize, int32_t &fd, void *&mmapPtr);
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WL_SHM_BUFFER_FACTORY_H
