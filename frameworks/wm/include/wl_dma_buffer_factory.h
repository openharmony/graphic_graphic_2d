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

#ifndef FRAMEWORKS_WM_INCLUDE_WL_DMA_BUFFER_FACTORY_H
#define FRAMEWORKS_WM_INCLUDE_WL_DMA_BUFFER_FACTORY_H

#include <functional>
#include <tuple>

#include <linux-dmabuf-unstable-v1-client-protocol.h>
#include <refbase.h>

#include "singleton_delegator.h"
#include "wayland_service.h"
#include "wl_buffer.h"

namespace OHOS {
class WlDMABufferFactory : public RefBase {
public:
    static sptr<WlDMABufferFactory> GetInstance();

    MOCKABLE void Init();
    MOCKABLE void Deinit();

    MOCKABLE sptr<WlBuffer> Create(int32_t fd, uint32_t w, uint32_t h, int32_t format);

private:
    WlDMABufferFactory() = default;
    MOCKABLE ~WlDMABufferFactory() = default;
    static inline sptr<WlDMABufferFactory> instance = nullptr;
    static inline SingletonDelegator<WlDMABufferFactory> delegator;

    static void OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver);
    static inline struct zwp_linux_dmabuf_v1 *dmabuf = nullptr;

    struct zwp_linux_buffer_params_v1 *CreateParam(int32_t fd, uint32_t w, uint32_t h);
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WL_DMA_BUFFER_FACTORY_H
