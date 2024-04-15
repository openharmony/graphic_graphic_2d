/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_TYPEFACE_CACHE_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_TYPEFACE_CACHE_H

#include <cstdint>
#include <unordered_map>
#include <list>
#include "text/typeface.h"

#include "memory/rs_dfx_string.h"
#include "memory/rs_memory_track.h"

namespace OHOS {

namespace Rosen {
class RSB_EXPORT RSTypefaceCache {
public:
    static RSTypefaceCache& Instance();
    static pid_t GetTypefacePid(uint64_t globalUniqueId);
    static uint32_t GetTypefaceId(uint64_t globalUniqueId);
    static uint64_t GenGlobalUniqueId(uint32_t typefaceId);
    // uniqueId = pid(32bit) | typefaceId(32bit)
    void CacheDrawingTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface> typeface);
    std::shared_ptr<Drawing::Typeface> GetDrawingTypefaceCache(uint64_t globalUniqueId) const;
    void RemoveDrawingTypefaceByGlobalUniqueId(uint64_t globalUniqueId);
    void RemoveDrawingTypefacesByPid(pid_t pid);
    void AddDelayDestroyQueue(uint64_t globalUniqueId);
    void HandleDelayDestroyQueue();

    RSTypefaceCache() = default;
    ~RSTypefaceCache() = default;

    class RSTypefaceRef {
    public:
        uint64_t globalUniqueId = 0;
        uint32_t refCount = 0;
    };

private:
    RSTypefaceCache(const RSTypefaceCache&) = delete;
    RSTypefaceCache(const RSTypefaceCache&&) = delete;
    RSTypefaceCache& operator=(const RSTypefaceCache&) = delete;
    RSTypefaceCache& operator=(const RSTypefaceCache&&) = delete;

    mutable std::mutex mapMutex_;
    std::unordered_map<pid_t, std::unordered_map<uint32_t, std::shared_ptr<Drawing::Typeface>>>
        drawingTypefaceCache_;
    mutable std::mutex listMutex_;
    std::list<RSTypefaceRef> delayDestroyTypefaces_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_TYPEFACE_CACHE_H
