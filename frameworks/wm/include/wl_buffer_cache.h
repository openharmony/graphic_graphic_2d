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

#ifndef FRAMEWORKS_WM_INCLUDE_WL_BUFFER_CACHE_H
#define FRAMEWORKS_WM_INCLUDE_WL_BUFFER_CACHE_H

#include <refbase.h>
#include <surface.h>

#include <window_manager_type.h>

#include "singleton_delegator.h"
#include "wl_buffer.h"
#include "wl_display.h"

namespace OHOS {
class WlBufferCache : public RefBase {
public:
    static sptr<WlBufferCache> GetInstance();

    MOCKABLE void Init();
    MOCKABLE void Deinit();

    MOCKABLE sptr<WlBuffer> GetWlBuffer(const sptr<Surface> &csurface,
                                        const sptr<SurfaceBuffer> &buffer);

    MOCKABLE WMError AddWlBuffer(const sptr<WlBuffer> &wbuffer,
                                 const sptr<Surface> &csurface,
                                 const sptr<SurfaceBuffer> &sbuffer);

    MOCKABLE bool GetSurfaceBuffer(const struct wl_buffer *wbuffer,
                                   sptr<Surface> &surface,
                                   sptr<SurfaceBuffer> &sbuffer);

private:
    WlBufferCache() = default;
    MOCKABLE ~WlBufferCache() = default;
    static inline sptr<WlBufferCache> instance = nullptr;
    static inline SingletonDelegator<WlBufferCache> delegator;

    void CleanCache();

    struct BufferCache {
        sptr<WlBuffer> wbuffer;
        wptr<Surface> csurface;
        wptr<SurfaceBuffer> sbuffer;
    };
    std::vector<WlBufferCache::BufferCache> cache;
    std::mutex cacheMutex;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WL_BUFFER_CACHE_H
