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

#include "wl_buffer_cache.h"

#include <mutex>

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMWlBufferCache"};
}

void WlBufferCache::Init()
{
}

void WlBufferCache::Deinit()
{
    std::lock_guard<std::mutex> lock(cacheMutex);
    cache.clear();
}

sptr<WlBufferCache> WlBufferCache::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WlBufferCache();
        }
    }
    return instance;
}

sptr<WlBuffer> WlBufferCache::GetWlBuffer(const sptr<Surface> &surface,
                                          const sptr<SurfaceBuffer> &buffer)
{
    if (surface == nullptr || buffer == nullptr) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(cacheMutex);
    CleanCache();
    for (const auto &c : cache) {
        if (c.csurface != nullptr && c.csurface.promote() == surface &&
            c.sbuffer != nullptr && c.sbuffer.promote() == buffer) {
            return c.wbuffer;
        }
    }
    return nullptr;
}

WMError WlBufferCache::AddWlBuffer(const sptr<WlBuffer> &wbuffer,
                                   const sptr<Surface> &csurface,
                                   const sptr<SurfaceBuffer> &sbuffer)
{
    if (wbuffer == nullptr) {
        WMLOGFW("wbuffer is nullptr");
        return WM_ERROR_NULLPTR;
    }
    if (csurface == nullptr) {
        WMLOGFW("surface is nullptr");
        return WM_ERROR_NULLPTR;
    }
    if (sbuffer == nullptr) {
        WMLOGFW("sbuffer is nullptr");
        return WM_ERROR_NULLPTR;
    }
    if (GetWlBuffer(csurface, sbuffer) == nullptr) {
        std::lock_guard<std::mutex> lock(cacheMutex);
        struct BufferCache ele = {
            .wbuffer = wbuffer,
            .csurface = csurface,
            .sbuffer = sbuffer,
        };
        cache.push_back(ele);
    }
    return WM_OK;
}

bool WlBufferCache::GetSurfaceBuffer(const struct wl_buffer *wbuffer,
                                     sptr<Surface> &csurface,
                                     sptr<SurfaceBuffer> &sbuffer)
{
    if (wbuffer == nullptr) {
        return false;
    }
    std::lock_guard<std::mutex> lock(cacheMutex);
    for (const auto &c : cache) {
        if (c.wbuffer != nullptr && c.wbuffer->GetRawPtr() == wbuffer) {
            csurface = c.csurface.promote();
            sbuffer = c.sbuffer.promote();
            return true;
        }
    }
    return false;
}

void WlBufferCache::CleanCache()
{
    for (auto it = cache.begin(); it != cache.end(); it++) {
        sptr<SurfaceBuffer> buffer = nullptr;
        if (it->sbuffer != nullptr) {
            buffer = it->sbuffer.promote();
        }

        if (it->csurface == nullptr ||
            it->csurface.promote() == nullptr || buffer == nullptr ||
            buffer->GetVirAddr() == nullptr || it->wbuffer == nullptr) {
            cache.erase(it);
            it--;
        }
    }
}
} // namespace OHOS
