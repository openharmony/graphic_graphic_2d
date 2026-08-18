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
#include <unordered_set>
#include "text/typeface.h"

#include "memory/rs_dfx_string.h"
#include "memory/rs_memory_track.h"

namespace OHOS {

namespace Rosen {
using TypefaceTuple = std::tuple<std::shared_ptr<Drawing::Typeface>, uint32_t>;

class RSB_EXPORT RSTypefaceCache {
public:
    // Maximum total CPU memory for non-FD (general) typefaces before caching is rejected.
    // Limit: 1300 MiB, based on typical render service memory budget.
    static constexpr uint32_t GENERAL_TYPEFACE_MEMORY_LIMIT = 1300 * 1024 * 1024;
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
    uint8_t HasTypeface(uint64_t globalUniqueId, uint32_t hash);
    // Returns false when the entry is rejected by the per-pid/global cache cap (DoS guard).
    // enforceCap is false only for the trusted profiler replay path.
    bool CacheDrawingTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface> typeface,
        bool enforceCap = true);
    std::shared_ptr<Drawing::Typeface> GetDrawingTypefaceCache(uint64_t globalUniqueId) const;
    std::shared_ptr<Drawing::Typeface> UpdateDrawingTypefaceRef(Drawing::SharedTypeface& sharedTypeface);
    int32_t InsertVariationTypeface(Drawing::SharedTypeface& sharedTypeface);
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
    void ReplaySerialize(std::stringstream& stream);

    /**
     * @brief    Deserialize drawing typeface cache (used for profiler replay).
     * @param ss Serialized data.
     */
    std::string ReplayDeserialize(std::stringstream& stream);

    RSTypefaceCache() = default;
    ~RSTypefaceCache() = default;

    class RSTypefaceRef {
    public:
        uint64_t globalUniqueId = 0;
        uint32_t refCount = 0;
    };
    void Dump(DfxString& log) const;

private:
    bool AddIfFound(uint64_t uniqueId, uint32_t hash);
    void RemoveHashMap(pid_t pid, std::unordered_map<uint64_t, TypefaceTuple>& typefaceHashMap,
        uint64_t hash_value);
    // All below assume mapMutex_ is already held by the caller.
    // Returns true if a new entry with this base hash may be committed under the per-pid and
    // global base caps. A variation that shares an already-held base is always allowed.
    bool CanAddEntry(pid_t pid, uint32_t baseHash) const;
    // Inserts the uniqueId->hash mapping and updates per-pid base accounting. The global
    // distinct-base count is typefaceBaseHashMap_.size(), maintained with the typeface data.
    void CommitHashCodeEntry(uint64_t uniqueId, uint64_t fullHash);
    void DecPidBaseRef(pid_t pid, uint32_t baseHash);
    // Attaches uniqueId to an already-cached fullHash: bumps the refcount on a dedup, or re-keys
    // under uniqueId on a 32-bit hash collision. Returns true if an existing entry was attached;
    // false when fullHash is not cached (caller inserts a new entry). Assumes mapMutex_ is held.
    bool RefExistingEntry(uint64_t uniqueId, uint64_t fullHash, const std::shared_ptr<Drawing::Typeface>& typeface);
    // Caches a brand-new fullHash entry and accounts its base. Increments (not overwrites) an
    // existing base so several variations of one origin share one baseRef and a removed variation
    // cannot prematurely drop the base. Assumes mapMutex_ is held.
    void InsertNewEntry(uint64_t fullHash, const std::shared_ptr<Drawing::Typeface>& typeface);
    // Old-IPC path: fulfills other clients whose HasTypeFace call queued behind this fullHash
    // while it was being registered, then drops the queue entry. Assumes mapMutex_ is held.
    void FulfillQueuedWaiters(uint64_t fullHash, uint64_t uniqueId);
    RSTypefaceCache(const RSTypefaceCache&) = delete;
    RSTypefaceCache(const RSTypefaceCache&&) = delete;
    RSTypefaceCache& operator=(const RSTypefaceCache&) = delete;
    RSTypefaceCache& operator=(const RSTypefaceCache&&) = delete;
    mutable std::mutex mapMutex_;
    std::unordered_map<uint64_t, uint64_t> typefaceHashCode_;
    std::unordered_map<uint64_t, TypefaceTuple> typefaceHashMap_;
    std::unordered_map<uint32_t, TypefaceTuple> typefaceBaseHashMap_;
    // Per-pid base DoS accounting: pid -> (baseHash -> uniqueId refcount). A base is identified
    // by its base hash (low 32 bits of fullHash); a variable font's variations share their base,
    // so they share quota and count once. The per-pid distinct-base count is the inner map's
    // size(); the global distinct-base count is typefaceBaseHashMap_.size(). Guarded by mapMutex_.
    std::unordered_map<pid_t, std::unordered_map<uint32_t, uint32_t>> pidBaseMap_;

    mutable std::mutex listMutex_;
    std::list<RSTypefaceRef> delayDestroyTypefaces_;
    std::unordered_map<uint32_t, std::unordered_set<uint64_t>> typefaceHashQueue_;
    size_t generalTypefaceTotalCpuMemory_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_TYPEFACE_CACHE_H
