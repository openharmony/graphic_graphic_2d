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

#include <iterator>
#include <numeric>
#include <unistd.h>
#include "memory/rs_memory_snapshot.h"
#include "render/rs_typeface_cache.h"
#include "sandbox_utils.h"
#include "src/core/SkLRUCache.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include <sstream>
#include <algorithm>
#ifdef IS_OHOS
#include <file_ex.h>
#endif

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#endif

// after 5 vsync count, destory it
#define DELAY_DESTROY_VSYNC_COUNT 5

namespace OHOS {
namespace Rosen {
// modify the RSTypefaceCache instance as global to extend life cycle, fix destructor crash
static RSTypefaceCache gRSTypefaceCacheInstance;
static const int MAX_CHUNK_SIZE = 20000;
static constexpr int INVALID_FD = -1;
// DoS guard: cap distinct typeface bases so a single app cannot exhaust render-service memory.
// A base is identified by its base hash (the low 32 bits of fullHash); a variable font's
// variations share their base, so they share quota and do not each consume it. Both fd-backed
// (ashmem) and serialized (old IPC) bases count, since both occupy service memory.
static constexpr uint32_t MAX_TYPEFACE_COUNT_PER_PID = 1024;
// Bounded to half a typical per-process fd limit so the cap rejects gracefully before the service
// hits EMFILE: each distinct fd-backed base holds one fd for its cache lifetime, and fd exhaustion
// fails the whole process (sockets, buffers), not just registration.
static constexpr size_t MAX_TYPEFACE_TOTAL_COUNT = 16384;

// Base identity = base hash, i.e. the low 32 bits of fullHash (high 32 bits hold variation
// coords). Variations of one font share this identity and count once.
static inline uint32_t BaseHashOf(uint64_t fullHash)
{
    return static_cast<uint32_t>(fullHash);
}

RSTypefaceCache& RSTypefaceCache::Instance()
{
    return gRSTypefaceCacheInstance;
}

uint64_t RSTypefaceCache::GenGlobalUniqueId(uint32_t id)
{
    static uint64_t shiftedPid = static_cast<uint64_t>(GetRealPid()) << 32; // 32 for 64-bit unsignd number shift
    return (shiftedPid | id);
}

pid_t RSTypefaceCache::GetTypefacePid(uint64_t uniqueId)
{
    // 32 for 64-bit unsignd number shift
    return static_cast<uint32_t>(uniqueId >> 32);
}

uint32_t RSTypefaceCache::GetTypefaceId(uint64_t uniqueId)
{
    return static_cast<uint32_t>(0xFFFFFFFF & uniqueId);
}

bool RSTypefaceCache::CanAddEntry(pid_t pid, uint32_t baseHash) const
{
    auto pidIt = pidBaseMap_.find(pid);
    // Adding a ref to a base this pid already holds (e.g. a variation) never grows the count.
    if (pidIt != pidBaseMap_.end() && pidIt->second.count(baseHash)) {
        return true;
    }
    // Global base cap reuses typefaceBaseHashMap_ (the distinct-base set) as its source of truth:
    // a brand-new base is blocked once full, while re-refcounting a base some other pid already
    // holds does not grow the set.
    if (typefaceBaseHashMap_.find(baseHash) == typefaceBaseHashMap_.end() &&
        typefaceBaseHashMap_.size() >= MAX_TYPEFACE_TOTAL_COUNT) {
        RS_LOGW("RSTypefaceCache global base cap reached, total:%{public}zu", typefaceBaseHashMap_.size());
        return false;
    }
    // Per-pid cap: the distinct-base count is the inner map's size().
    if (pidIt != pidBaseMap_.end() && pidIt->second.size() >= MAX_TYPEFACE_COUNT_PER_PID) {
        RS_LOGW("RSTypefaceCache per-pid base cap reached, pid:%{public}d, count:%{public}zu",
            static_cast<int32_t>(pid), pidIt->second.size());
        return false;
    }
    return true;
}

void RSTypefaceCache::CommitHashCodeEntry(uint64_t uniqueId, uint64_t fullHash)
{
    bool isNew = typefaceHashCode_.find(uniqueId) == typefaceHashCode_.end();
    typefaceHashCode_[uniqueId] = fullHash;
    if (!isNew) {
        return;
    }
    // Per-pid accounting only; the global distinct-base count is typefaceBaseHashMap_.size(),
    // maintained alongside the typeface data in CacheDrawingTypeface / RemoveHashMap.
    pidBaseMap_[GetTypefacePid(uniqueId)][BaseHashOf(fullHash)]++;
}

void RSTypefaceCache::DecPidBaseRef(pid_t pid, uint32_t baseHash)
{
    auto pidIt = pidBaseMap_.find(pid);
    if (pidIt == pidBaseMap_.end()) {
        return;
    }
    auto baseIt = pidIt->second.find(baseHash);
    if (baseIt != pidIt->second.end()) {
        baseIt->second -= 1;
        if (baseIt->second == 0) {
            pidIt->second.erase(baseIt);
            if (pidIt->second.empty()) {
                pidBaseMap_.erase(pidIt);
            }
        }
    }
}

bool RSTypefaceCache::AddIfFound(uint64_t uniqueId, uint32_t hash)
{
    auto iterator = typefaceHashMap_.find(hash);
    if (iterator == typefaceHashMap_.end()) {
        return false;
    }
    // The HasTypeface dedup path is out of scope for cap enforcement, but the entry must
    // still be accounted so the removal path stays balanced. `hash` is the base hash here.
    CommitHashCodeEntry(uniqueId, hash);
    std::get<1>(iterator->second)++;
    pid_t pid = GetTypefacePid(uniqueId);
    if (pid != 0) {
        MemorySnapshot::Instance().AddCpuMemory(pid, (std::get<0>(iterator->second))->GetSize());
    }
    return true;
}

uint8_t RSTypefaceCache::HasTypeface(uint64_t uniqueId, uint32_t hash)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    if (typefaceHashCode_.find(uniqueId) != typefaceHashCode_.end()) {
        // this client has already registered this typeface
        return Drawing::REGISTERED;
    }

    if (hash) {
        // check if someone else has already registered this typeface, add ref count and
        // mapping if so.
        if (AddIfFound(uniqueId, hash)) {
            return Drawing::REGISTERED;
        }

        // check if someone else is about to register this typeface -> queue uid
        auto iterator = typefaceHashQueue_.find(hash);
        if (iterator != typefaceHashQueue_.end()) {
            iterator->second.insert(uniqueId);
            RS_LOGD("TypefaceHashQueue find same hash:%{public}u, size:%{public}zu", hash, iterator->second.size());
            return Drawing::REGISTERING;
        } else {
            typefaceHashQueue_[hash] = { uniqueId };
        }
    }

    return Drawing::NO_REGISTER;
}

uint32_t CalculateFontArgsHash(std::vector<Drawing::FontArguments::VariationPosition::Coordinate>& coords)
{
    size_t size = coords.size() * (sizeof(Drawing::FontArguments::VariationPosition::Coordinate));
#ifdef USE_M133_SKIA
    return SkChecksum::Hash32(coords.data(), std::min(size, static_cast<size_t>(MAX_CHUNK_SIZE)));
#else
    return SkOpts::hash_fn(coords.data(), std::min(size, static_cast<size_t>(MAX_CHUNK_SIZE)), 0);
#endif
}

uint64_t AssembleFullHash(uint32_t fontArgsHash, uint32_t baseHash)
{
    uint64_t fontArgsHash64 = static_cast<uint64_t>(fontArgsHash) << 32;
    return (fontArgsHash64 | baseHash);
}

uint64_t CalculateTypefaceFullHash(std::shared_ptr<Drawing::Typeface> typeface)
{
    std::vector<Drawing::FontArguments::VariationPosition::Coordinate> coords;
    int coordsCount = typeface->GetVariationDesignPosition(nullptr, 0);
    if (coordsCount <= 0) {
        return static_cast<uint64_t>(typeface->GetHash());
    }
    coords.resize(coordsCount);
    typeface->GetVariationDesignPosition(coords.data(), coordsCount);
    return AssembleFullHash(CalculateFontArgsHash(coords), typeface->GetHash());
}

// Computes the fullHash used to key typefaceHashMap_. fd-backed typefaces hash over their
// variation coords + base; others use the adapter hash, falling back to hashing serialized data
// when the adapter provides none. Returns false only when the fallback serialize yields no data.
static bool ComputeFullHash(const std::shared_ptr<Drawing::Typeface>& typeface, uint64_t& fullHash)
{
    if (typeface->GetFd() != INVALID_FD) {
        fullHash = CalculateTypefaceFullHash(typeface);
        return true;
    }
    fullHash = typeface->GetHash();
    if (fullHash != 0) {
        return true;
    }
    std::shared_ptr<Drawing::Data> data = typeface->Serialize();
    if (data == nullptr) {
        return false;
    }
    const void* stream = data->GetData();
    size_t size = data->GetSize();
#ifdef USE_M133_SKIA
    fullHash = SkChecksum::Hash32(stream, std::min(size, static_cast<size_t>(MAX_CHUNK_SIZE)));
#else
    fullHash = SkOpts::hash_fn(stream, std::min(size, static_cast<size_t>(MAX_CHUNK_SIZE)), 0);
#endif
    return true;
}

bool RSTypefaceCache::RefExistingEntry(uint64_t uniqueId, uint64_t fullHash,
    const std::shared_ptr<Drawing::Typeface>& typeface)
{
    auto it = typefaceHashMap_.find(fullHash);
    if (it == typefaceHashMap_.end()) {
        return false;
    }
    auto [faceCache, ref] = it->second;
    if (faceCache->GetFamilyName() != typeface->GetFamilyName()) {
        // hash collision. Overwrites the stored hash with uniqueId so RemoveHashMap can
        // locate typefaceHashMap_[uniqueId]; on removal DecPidBaseRef will then see
        // BaseHashOf(uniqueId) instead of the committed BaseHashOf(fullHash), a base
        // accounting drift of ±1 for this pid. Collisions on the 32-bit base hash are
        // astronomically rare, so this is left as-is rather than complicate the path.
        typefaceHashCode_[uniqueId] = uniqueId;
        typefaceHashMap_[uniqueId] = std::make_tuple(typeface, 1);
        RS_LOGI("CacheDrawingTypeface hash collision");
    } else {
        typefaceHashMap_[fullHash] = std::make_tuple(faceCache, ref + 1);
    }
    return true;
}

void RSTypefaceCache::InsertNewEntry(uint64_t fullHash, const std::shared_ptr<Drawing::Typeface>& typeface)
{
    typefaceHashMap_[fullHash] = std::make_tuple(typeface, 1);
    uint32_t baseHash = typeface->GetHash();
    auto baseEntry = typefaceBaseHashMap_.find(baseHash);
    if (baseEntry != typefaceBaseHashMap_.end()) {
        std::get<1>(baseEntry->second)++;
    } else {
        typefaceBaseHashMap_[baseHash] = std::make_tuple(typeface, 1);
    }
}

void RSTypefaceCache::FulfillQueuedWaiters(uint64_t fullHash, uint64_t uniqueId)
{
    auto iterator = typefaceHashQueue_.find(fullHash);
    if (iterator == typefaceHashQueue_.end()) {
        return;
    }
    for (const uint64_t cacheId : iterator->second) {
        if (cacheId != uniqueId) {
            AddIfFound(cacheId, fullHash);
        }
    }
    typefaceHashQueue_.erase(iterator);
}

bool RSTypefaceCache::CacheDrawingTypeface(uint64_t uniqueId, std::shared_ptr<Drawing::Typeface> typeface,
    bool enforceCap)
{
    if (!typeface || uniqueId == 0) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mapMutex_);
    if (typefaceHashCode_.find(uniqueId) != typefaceHashCode_.end()) {
        return true; // this uniqueId is already registered
    }
    uint64_t fullHash = 0;
    if (!ComputeFullHash(typeface, fullHash)) {
        return false;
    }
    // Cap keyed on the base hash; checked after fullHash so a variation that shares an already-held
    // base is allowed through.
    if (enforceCap && !CanAddEntry(GetTypefacePid(uniqueId), BaseHashOf(fullHash))) {
        return false;
    }
    // General (non-fd) typefaces are deserialized into the render-service process and count against a
    // global CPU memory cap; fd-backed (new IPC) registrations carry their own ashmem and bypass it.
    if (typeface->GetFd() == INVALID_FD) {
        size_t newSize = generalTypefaceTotalCpuMemory_ + typeface->GetSize();
        if (newSize > GENERAL_TYPEFACE_MEMORY_LIMIT) {
            RS_LOGD("CacheDrawingTypeface general typeface total size too big.");
            return false;
        }
        generalTypefaceTotalCpuMemory_ = newSize;
    }

    // The registration now succeeds: record the mapping, charge memory, then attach to an
    // already-cached fullHash (dedup/collision) or insert a new one.
    CommitHashCodeEntry(uniqueId, fullHash);
    pid_t pid = GetTypefacePid(uniqueId);
    if (pid) {
        MemorySnapshot::Instance().AddCpuMemory(pid, typeface->GetSize());
    }
    if (RefExistingEntry(uniqueId, fullHash, typeface)) {
        return true;
    }
    InsertNewEntry(fullHash, typeface);

    // fd-backed (new IPC) registrations carry their own ashmem and bypass the waiter queue, which
    // exists only to dedup serialized (old IPC) registrations behind a concurrent one.
    if (typeface->GetFd() == INVALID_FD) {
        FulfillQueuedWaiters(fullHash, uniqueId);
    }
    return true;
}

void RemoveHashQueue(
    std::unordered_map<uint32_t, std::unordered_set<uint64_t>>& typefaceHashQueue, uint64_t globalUniqueId)
{
    for (auto& ref : typefaceHashQueue) {
        auto it = ref.second.find(globalUniqueId);
        if (it != ref.second.end()) {
            ref.second.erase(it);
            if (ref.second.empty()) {
                typefaceHashQueue.erase(ref.first);
            }
            return;
        }
    }
}

void RSTypefaceCache::RemoveHashMap(pid_t pid, std::unordered_map<uint64_t, TypefaceTuple>& typefaceHashMap,
    uint64_t hash_value)
{
    if (typefaceHashMap.find(hash_value) != typefaceHashMap.end()) {
        auto& [typeface, ref] = typefaceHashMap[hash_value];
        if (pid) {
            MemorySnapshot::Instance().RemoveCpuMemory(pid, typeface->GetSize());
        }
        if (typeface->GetFd() == INVALID_FD) {
            if (generalTypefaceTotalCpuMemory_ >= typeface->GetSize()) {
                generalTypefaceTotalCpuMemory_ -= typeface->GetSize();
            } else {
                generalTypefaceTotalCpuMemory_ = 0;
            }
        }
        ref -= 1;
        if (ref != 0) {
            return;
        }
        typefaceHashMap.erase(hash_value);
        uint32_t baseHash = static_cast<uint32_t>(0xFFFFFFFF & hash_value);
        auto baseTypefaceItem = typefaceBaseHashMap_.find(baseHash);
        if (baseTypefaceItem != typefaceBaseHashMap_.end()) {
            auto& [baseTypeface, baseRef] = baseTypefaceItem->second;
            baseRef -= 1;
            if (baseRef == 0) {
                typefaceBaseHashMap_.erase(baseHash);
            }
        }
    }
}

void RSTypefaceCache::RemoveDrawingTypefaceByGlobalUniqueId(uint64_t globalUniqueId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    // first check the queue;
    RemoveHashQueue(typefaceHashQueue_, globalUniqueId);

    if (typefaceHashCode_.find(globalUniqueId) == typefaceHashCode_.end()) {
        RS_LOGI("RSTypefaceCache:Failed to find typeface, uniqueid:%{public}u", GetTypefaceId(globalUniqueId));
        return;
    }
    auto fullHash = typefaceHashCode_[globalUniqueId];
    typefaceHashCode_.erase(globalUniqueId);
    DecPidBaseRef(GetTypefacePid(globalUniqueId), BaseHashOf(fullHash));
    RS_LOGI("RSTypefaceCache:Remove typeface, uniqueid:%{public}u", GetTypefaceId(globalUniqueId));
    RemoveHashMap(GetTypefacePid(globalUniqueId), typefaceHashMap_, fullHash);
}

std::shared_ptr<Drawing::Typeface> RSTypefaceCache::GetDrawingTypefaceCache(uint64_t uniqueId) const
{
    if (uniqueId > 0) {
        std::lock_guard<std::mutex> lock(mapMutex_);
        if (typefaceHashCode_.find(uniqueId) != typefaceHashCode_.end() &&
                typefaceHashMap_.find(typefaceHashCode_.at(uniqueId)) != typefaceHashMap_.end()) {
            uint64_t hash_value = typefaceHashCode_.at(uniqueId);
            auto [typeface, ref] = typefaceHashMap_.at(hash_value);
            return typeface;
        }
    }
    return nullptr;
}

std::shared_ptr<Drawing::Typeface> RSTypefaceCache::UpdateDrawingTypefaceRef(Drawing::SharedTypeface& sharedTypeface)
{
    std::lock_guard lock(mapMutex_);
    uint64_t fullHash = static_cast<uint64_t>(sharedTypeface.hash_);
    if (sharedTypeface.hasFontArgs_ && sharedTypeface.fd_ != INVALID_FD) {
        uint32_t fontArgsHash = CalculateFontArgsHash(sharedTypeface.coords_);
        fullHash = AssembleFullHash(fontArgsHash, sharedTypeface.hash_);
    }
    // Cap on the base identity (base hash), checked only once fullHash is known.
    if (!CanAddEntry(GetTypefacePid(sharedTypeface.id_), BaseHashOf(fullHash))) {
        return nullptr;
    }
    auto iter = typefaceHashMap_.find(fullHash);
    if (iter != typefaceHashMap_.end()) {
        CommitHashCodeEntry(sharedTypeface.id_, fullHash);
        std::get<1>(iter->second) += 1;
        pid_t pid = GetTypefacePid(sharedTypeface.id_);
        if (pid) {
            MemorySnapshot::Instance().AddCpuMemory(pid, (std::get<0>(iter->second))->GetSize());
        }
        return std::get<0>(iter->second);
    } else if (sharedTypeface.hasFontArgs_) {
        uint32_t baseHash = static_cast<uint32_t>(0xFFFFFFFF & fullHash);
        auto baseTypeface = typefaceBaseHashMap_.find(baseHash);
        if (baseTypeface != typefaceBaseHashMap_.end()) {
            RS_LOGD("UpdateDrawingTypefaceRef: Find same typeface in base cache, use existed base typeface.");
            Drawing::FontArguments fontArgs;
            fontArgs.SetCollectionIndex(sharedTypeface.index_);
            fontArgs.SetVariationDesignPosition({sharedTypeface.coords_.data(), sharedTypeface.coords_.size()});
            auto clonedTypeface = std::get<0>(baseTypeface->second)->MakeClone(fontArgs);
            if(!clonedTypeface) {
                RS_LOGE("UpdateDrawingTypefaceRef: Typeface clone failed");
                return nullptr;
            }
            std::get<1>(baseTypeface->second)++;
            clonedTypeface->SetFd(std::get<0>(baseTypeface->second)->GetFd());
            clonedTypeface->SetHash(std::get<0>(baseTypeface->second)->GetHash());
            CommitHashCodeEntry(sharedTypeface.id_, fullHash);
            typefaceHashMap_[fullHash] = std::make_tuple(clonedTypeface, 1);
            return clonedTypeface;
        }
    }
    return nullptr;
}

int32_t RSTypefaceCache::InsertVariationTypeface(Drawing::SharedTypeface& sharedTypeface)
{
    auto variationTypeface = GetDrawingTypefaceCache(sharedTypeface.id_);
    if (variationTypeface != nullptr) {
        RS_LOGD("InsertVariationTypeface: variationTypefaceId found in cache");
        return variationTypeface->GetFd();
    }
    RS_LOGD("InsertVariationTypeface: variationTypefaceId not found in cache");

    // Validate source typefaceId
    auto typeface = GetDrawingTypefaceCache(sharedTypeface.originId_);
    if (typeface == nullptr) {
        RS_LOGE("InsertVariationTypeface: originId_ not found in cache");
        return -1;
    }

    Drawing::FontArguments fontArgs;
    fontArgs.SetCollectionIndex(typeface->GetIndex());
    fontArgs.SetVariationDesignPosition({sharedTypeface.coords_.data(), sharedTypeface.coords_.size()});
    auto clonedTypeface = typeface->MakeClone(fontArgs);
    if (clonedTypeface == nullptr) {
        RS_LOGE("UpdateDrawingTypefaceRef: Typeface clone failed");
        return -1;
    }
    clonedTypeface->SetFd(typeface->GetFd());
    // A variation shares its base's identity for cap accounting. MakeClone leaves the clone's hash
    // unset; SkTypeface::GetHash() would then lazily hash the already-varied clone to a value
    // unrelated to the base, so BaseHashOf(fullHash) would diverge and each variation would consume
    // its own cap slot instead of sharing the base's. Carry the base hash forward.
    clonedTypeface->SetHash(typeface->GetHash());
    if (!CacheDrawingTypeface(sharedTypeface.id_, clonedTypeface)) {
        RS_LOGW("InsertVariationTypeface rejected by cache cap, id:%{public}u", GetTypefaceId(sharedTypeface.id_));
        return -1;
    }
    return clonedTypeface->GetFd();
}

void PurgeMapWithPid(pid_t pid, std::unordered_map<uint32_t, std::unordered_set<uint64_t>>& map)
{
    // go through queued items;
    std::vector<uint32_t> removeList;

    for (auto& ref : map) {
        std::unordered_set<uint64_t>& uniqueIdSet = ref.second;
        auto it = uniqueIdSet.begin();
        while (it != uniqueIdSet.end()) {
            uint64_t uid = *it;
            pid_t pidCache = static_cast<pid_t>(uid >> 32);
            if (pid == pidCache) {
                it = uniqueIdSet.erase(it);
            } else {
                it++;
            }
        }
        if (uniqueIdSet.empty()) {
            removeList.push_back(ref.first);
        }
    }

    while (removeList.size()) {
        map.erase(removeList.back());
        removeList.pop_back();
    }
}

void RSTypefaceCache::RemoveDrawingTypefacesByPid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    PurgeMapWithPid(pid, typefaceHashQueue_);

    for (auto it = typefaceHashCode_.begin(); it != typefaceHashCode_.end();) {
        if (pid == GetTypefacePid(it->first)) {
            // no need pid, ClearMemoryCache will clear memory snapshot.
            RemoveHashMap(0, typefaceHashMap_, it->second);
            it = typefaceHashCode_.erase(it);
        } else {
            ++it;
        }
    }
    // The whole pid is going away, so drop its per-pid base accounting in one shot. This is also
    // robust against the hash-collision drift: a per-uniqueId decrement would key on
    // BaseHashOf(uniqueId) rather than the committed base and could leave the pid non-empty.
    pidBaseMap_.erase(pid);
}
void RSTypefaceCache::AddDelayDestroyQueue(uint64_t globalUniqueId)
{
    std::lock_guard<std::mutex> lock(listMutex_);
    delayDestroyTypefaces_.push_back({globalUniqueId, DELAY_DESTROY_VSYNC_COUNT});
}

void RSTypefaceCache::HandleDelayDestroyQueue()
{
    RS_TRACE_FUNC();
    std::lock_guard<std::mutex> lock(listMutex_);
    for (auto it = delayDestroyTypefaces_.begin(); it != delayDestroyTypefaces_.end();) {
        it->refCount--;
        if (it->refCount == 0) {
            RemoveDrawingTypefaceByGlobalUniqueId(it->globalUniqueId);
            it = delayDestroyTypefaces_.erase(it);
        } else {
            ++it;
        }
    }
}

void RSTypefaceCache::Dump(DfxString& log) const
{
    RS_TRACE_NAME_FMT("RSTypefaceCache::Dump typefaceHashCode size:%d", typefaceHashCode_.size());
    std::lock_guard<std::mutex> lock(mapMutex_);
    uint32_t totalMem = std::accumulate(typefaceHashMap_.begin(), typefaceHashMap_.end(), 0u,
        [](uint32_t sum, const auto& item) { return sum + std::get<0>(item.second)->GetSize(); });
    constexpr double KB = 1024.0;
    constexpr double MB = KB * KB;
    log.AppendFormat("RSTypefaceCache Dump:\n");
    log.AppendFormat(
        "  Total: %.2fKB (%.2fMB)\n", static_cast<double>(totalMem) / KB, static_cast<double>(totalMem) / MB);
    log.AppendFormat("  Entries: %zu\n", typefaceHashMap_.size());
    log.AppendFormat("  Fonts: %zu base / %zu registrations (per-pid cap %u, global cap %zu)\n",
        typefaceBaseHashMap_.size(), typefaceHashCode_.size(), MAX_TYPEFACE_COUNT_PER_PID,
        MAX_TYPEFACE_TOTAL_COUNT);
    log.AppendFormat("%-6s %-16s %-16s %-4s %-26s %-20s\n", "PID", "UniqueId", "Hash", "Ref", "FamilyName", "Size");

    std::unordered_map<uint64_t, std::vector<uint64_t>> hashToUniqueIds;
    for (const auto& [uniqueId, hash] : typefaceHashCode_) {
        hashToUniqueIds[hash].push_back(uniqueId);
    }

    for (const auto& [hash, tuple] : typefaceHashMap_) {
        auto [typeface, ref] = tuple;
        const std::string& family = typeface->GetFamilyName();
        double sizeMB = static_cast<double>(typeface->GetSize()) / MB;

        auto it = hashToUniqueIds.find(hash);
        if (it == hashToUniqueIds.end() || it->second.empty()) {
            log.AppendFormat("%-6s %-16s %016" PRIx64 " %-4u %-26s %-10u (%.2fMB)\n", "-", "-", hash, ref,
                family.c_str(), typeface->GetSize(), sizeMB);
            continue;
        }
        for (uint64_t uniqueId : it->second) {
            int32_t pid = static_cast<int32_t>(GetTypefacePid(uniqueId));
            log.AppendFormat("%-6d %-16u %016" PRIx64 " %-4u %-26s %-10u (%.2fMB)\n", pid, GetTypefaceId(uniqueId),
                hash, ref, family.c_str(), typeface->GetSize(), sizeMB);
        }
    }
    log.AppendFormat("------------------------------------\n");
}

void RSTypefaceCache::ReplaySerialize(std::stringstream& stream)
{
    mapMutex_.lock();
    const auto hashCode = typefaceHashCode_;
    const auto hashMap = typefaceHashMap_;
    mapMutex_.unlock();

    std::unordered_map<std::shared_ptr<Drawing::Typeface>, std::vector<uint64_t>> fonts;
    for (const auto& [id, hash] : hashCode) {
        const auto entry = hashMap.find(hash);
        const auto font = (entry != hashMap.end()) ? std::get<0>(entry->second) : nullptr;
        if (font) {
            fonts[font].push_back(id);
        }
    }

    size_t count = 0;
    stream.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [font, ids] : fonts) {
        const auto blob = font->Serialize();
        const auto data = blob ? blob->GetData() : nullptr;
        const size_t size = blob ? blob->GetSize() : 0;
        if (!data || !size) {
            continue;
        }

        stream.write(reinterpret_cast<const char*>(&ids[0]), sizeof(ids[0]));
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        stream.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));

        constexpr size_t dummy = std::numeric_limits<size_t>::max();
        for (size_t index = 1; index < ids.size(); index++) {
            stream.write(reinterpret_cast<const char*>(&ids[index]), sizeof(ids[index]));
            stream.write(reinterpret_cast<const char*>(&dummy), sizeof(dummy));
        }

        count += ids.size();
    }

    stream.seekp(0, std::ios_base::beg);
    stream.write(reinterpret_cast<const char*>(&count), sizeof(count));
    stream.seekp(0, std::ios_base::end);
}

std::string RSTypefaceCache::ReplayDeserialize(std::stringstream& stream)
{
    constexpr uint64_t mask = 1ull << 62;
    constexpr size_t maxSize = 40'000'000u;
    constexpr size_t maxTotalSize = 500u * 1024u * 1024u;

    size_t count = 0u;
    if (!stream.read(reinterpret_cast<char*>(&count), sizeof(count))) {
        return "ReplayDeserialize: Cannot read count";
    }

    constexpr size_t maxCount = 1'000u;
    if (count > maxCount) {
        return "ReplayDeserialize: Count exceeds the limit";
    }

    std::shared_ptr<Drawing::Typeface> typeface;
    for (size_t i = 0, totalSize = 0; i < count; i++) {
        uint64_t uniqueId = 0ull;
        if (!stream.read(reinterpret_cast<char*>(&uniqueId), sizeof(uniqueId))) {
            ReplayClear();
            return "ReplayDeserialize: Cannot read unique id";
        }

        size_t size = 0u;
        if (!stream.read(reinterpret_cast<char*>(&size), sizeof(size))) {
            ReplayClear();
            return "ReplayDeserialize: Cannot read size";
        }

        constexpr size_t dummy = std::numeric_limits<size_t>::max();
        if (dummy == size) {
            CacheDrawingTypeface(uniqueId | mask, typeface, false);
            continue;
        }

        if (size > maxSize || totalSize + size > maxTotalSize) {
            ReplayClear();
            return "ReplayDeserialize: Size exceeds the limit";
        }
        totalSize += size;

        std::vector<char> data(size);
        if (!stream.read(data.data(), static_cast<std::streamsize>(data.size()))) {
            ReplayClear();
            return "ReplayDeserialize: Cannot read data";
        }

        typeface = Drawing::Typeface::Deserialize(data.data(), data.size());
        if (!typeface) {
            ReplayClear();
            return "ReplayDeserialize: Cannot create typeface";
        }
        CacheDrawingTypeface(uniqueId | mask, typeface, false);
    }
    return {};
}

void RSTypefaceCache::ReplayClear()
{
    std::vector<uint64_t> removeId;
    constexpr int bitNumber = 30 + 32;
    uint64_t replayMask = (uint64_t)1 << bitNumber;
    {
        std::lock_guard<std::mutex> lock(mapMutex_);
        for (auto co : typefaceHashCode_) {
            if (co.first & replayMask) {
                removeId.emplace_back(co.first);
            }
        }
    }
    for (auto uniqueId : removeId) {
        RemoveDrawingTypefaceByGlobalUniqueId(uniqueId);
    }
}

} // namespace Rosen
} // namespace OHOS
