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

#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_TYPEFACE_CACHE_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_TYPEFACE_CACHE_H

#include <cstdint>
#include <unordered_map>
#include <list>
#include "text/typeface.h"

#include "memory/rs_dfx_string.h"
#include "memory/rs_memory_track.h"

namespace OHOS {

namespace Rosen {
using TypefaceTuple = std::tuple<std::shared_ptr<Drawing::Typeface>, uint32_t>;
class RSB_EXPORT RSTypefaceCache {
public:
    static RSTypefaceCache& Instance();
    static pid_t GetTypefacePid(uint64_t globalUniqueId);
    static uint32_t GetTypefaceId(uint64_t globalUniqueId);
    /**
      * Calculate unique id by combining process id and local unique id
      * uniqueId = pid(high 32bit) | typefaceId(low 32bit)
      */
    static uint64_t GenGlobalUniqueId(uint32_t typefaceId);
    /**
      * Checks if the given hash exists in the cache already
      * provided by someone else. If so, increases ref count to reduce registration cost.
      */
    bool HasTypeface(uint64_t globalUniqueId, uint32_t hash);
    void CacheDrawingTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface> typeface);
    std::shared_ptr<Drawing::Typeface> GetDrawingTypefaceCache(uint64_t globalUniqueId) const;
    void RemoveDrawingTypefaceByGlobalUniqueId(uint64_t globalUniqueId);
    void RemoveDrawingTypefacesByPid(pid_t pid);
    void AddDelayDestroyQueue(uint64_t globalUniqueId);
    void HandleDelayDestroyQueue();

    /**
     * @brief    Remove patched typeface IDs (used for profiler replay).
     */
    void ReplayClear();

    /**
     * @brief    Serialize drawing typeface cache (used for profiler replay).
     * @param ss String stream to write serialized data.
     */
    void ReplaySerialize(std::stringstream& ss);

    /**
     * @brief    Deserialize drawing typeface cache (used for profiler replay).
     * @param ss Serialized data.
     */
    std::string ReplayDeserialize(std::stringstream& ss);

    RSTypefaceCache() = default;
    ~RSTypefaceCache() = default;

    class RSTypefaceRef {
    public:
        uint64_t globalUniqueId = 0;
        uint32_t refCount = 0;
    };
    void Dump() const;

private:
    bool AddIfFound(uint64_t uniqueId, uint32_t hash);
    void RemoveHashMap(pid_t pid, std::unordered_map<uint64_t, TypefaceTuple>& typefaceHashMap,
        uint64_t hash_value);
    RSTypefaceCache(const RSTypefaceCache&) = delete;
    RSTypefaceCache(const RSTypefaceCache&&) = delete;
    RSTypefaceCache& operator=(const RSTypefaceCache&) = delete;
    RSTypefaceCache& operator=(const RSTypefaceCache&&) = delete;
    mutable std::mutex mapMutex_;
    std::unordered_map<uint64_t, uint64_t> typefaceHashCode_;
    std::unordered_map<uint64_t, TypefaceTuple> typefaceHashMap_;

    mutable std::mutex listMutex_;
    std::list<RSTypefaceRef> delayDestroyTypefaces_;
    std::unordered_map<uint32_t, std::vector<uint64_t>> typefaceHashQueue_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_TYPEFACE_CACHE_H
