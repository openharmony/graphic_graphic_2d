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
#include <unistd.h>
#include "memory/rs_memory_snapshot.h"
#include "render/rs_typeface_cache.h"
#include "sandbox_utils.h"
#include "src/core/SkLRUCache.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include <sstream>
#include <algorithm>

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

bool RSTypefaceCache::AddIfFound(uint64_t uniqueId, uint32_t hash)
{
    std::unordered_map<uint64_t, TypefaceTuple>::iterator iterator = typefaceHashMap_.find(hash);
    if (iterator != typefaceHashMap_.end()) {
        typefaceHashCode_[uniqueId] = hash;
        std::get<1>(iterator->second)++;
        pid_t pid = GetTypefacePid(uniqueId);
        if (pid) {
            MemorySnapshot::Instance().AddCpuMemory(pid, (std::get<0>(iterator->second))->GetSize());
        }
        return true;
    }
    return false;
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

void RSTypefaceCache::CacheDrawingTypeface(uint64_t uniqueId,
    std::shared_ptr<Drawing::Typeface> typeface)
{
    if (!(typeface && uniqueId > 0)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mapMutex_);
    if (typefaceHashCode_.find(uniqueId) != typefaceHashCode_.end()) {
        return;
    }

    uint32_t hash_value = typeface->GetHash();
    if (!hash_value) { // fallback to slow path if the adapter does not provide hash
        std::shared_ptr<Drawing::Data> data = typeface->Serialize();
        if (!data) {
            return;
        }
        const void* stream = data->GetData();
        size_t size = data->GetSize();
#ifdef USE_M133_SKIA
        hash_value = SkChecksum::Hash32(stream, std::min(size, static_cast<size_t>(MAX_CHUNK_SIZE)));
#else
        hash_value = SkOpts::hash_fn(stream, std::min(size, static_cast<size_t>(MAX_CHUNK_SIZE)), 0);
#endif
    }
    typefaceHashCode_[uniqueId] = hash_value;
    pid_t pid = GetTypefacePid(uniqueId);
    if (typefaceHashMap_.find(hash_value) != typefaceHashMap_.end()) {
        if (pid) {
            MemorySnapshot::Instance().AddCpuMemory(pid, typeface->GetSize());
        }
        auto [faceCache, ref] = typefaceHashMap_[hash_value];
        if (faceCache->GetFamilyName() != typeface->GetFamilyName()) {
            // hash collision
            typefaceHashCode_[uniqueId] = uniqueId;
            typefaceHashMap_[uniqueId] = std::make_tuple(typeface, 1);
            RS_LOGI("CacheDrawingTypeface hash collision");
        } else {
            typefaceHashMap_[hash_value] = std::make_tuple(faceCache, ref + 1);
        }
        return;
    }
    typefaceHashMap_[hash_value] = std::make_tuple(typeface, 1);
    if (pid) {
        MemorySnapshot::Instance().AddCpuMemory(pid, typeface->GetSize());
    }
    // register queued entries
    auto iterator = typefaceHashQueue_.find(hash_value);
    if (iterator != typefaceHashQueue_.end()) {
        for (const uint64_t cacheId: iterator->second) {
            if (cacheId != uniqueId) {
                AddIfFound(cacheId, hash_value);
            }
        }
        typefaceHashQueue_.erase(iterator);
    }
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
        auto [typeface, ref] = typefaceHashMap[hash_value];
        if (pid) {
            MemorySnapshot::Instance().RemoveCpuMemory(pid, typeface->GetSize());
        }
        if (ref <= 1) {
            typefaceHashMap.erase(hash_value);
        } else {
            typefaceHashMap[hash_value] = std::make_tuple(typeface, ref - 1);
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
    auto hash_value = typefaceHashCode_[globalUniqueId];
    typefaceHashCode_.erase(globalUniqueId);
    RS_LOGI("RSTypefaceCache:Remove typeface, uniqueid:%{public}u", GetTypefaceId(globalUniqueId));
    RemoveHashMap(GetTypefacePid(globalUniqueId), typefaceHashMap_, hash_value);
}

std::shared_ptr<Drawing::Typeface> RSTypefaceCache::GetDrawingTypefaceCache(uint64_t uniqueId) const
{
    if (uniqueId > 0) {
        std::lock_guard<std::mutex> lock(mapMutex_);
        if (typefaceHashCode_.find(uniqueId) != typefaceHashCode_.end() &&
                typefaceHashMap_.find(typefaceHashCode_.at(uniqueId)) != typefaceHashMap_.end()) {
            uint32_t hash_value = typefaceHashCode_.at(uniqueId);
            auto [typeface, ref] = typefaceHashMap_.at(hash_value);
            return typeface;
        }
    }
    return nullptr;
}

std::shared_ptr<Drawing::Typeface> RSTypefaceCache::GetDrawingTypefaceCacheByHash(uint64_t uniqueId) const
{
    std::lock_guard lock(mapMutex_);
    auto hash = GetTypefaceId(uniqueId);
    if (typefaceHashMap_.count(hash)) {
        auto [typeface, ref] = typefaceHashMap_.at(hash);
        return typeface;
    }
    return nullptr;
}

std::shared_ptr<Drawing::Typeface> RSTypefaceCache::UpdateDrawingTypefaceRef(uint64_t globalId)
{
    std::lock_guard lock(mapMutex_);
    auto hash = GetTypefaceId(globalId);
    auto iter = typefaceHashMap_.find(hash);
    if (iter != typefaceHashMap_.end()) {
        typefaceHashCode_[globalId] = hash;
        std::get<1>(iter->second) += 1;
        pid_t pid = GetTypefacePid(globalId);
        if (pid) {
            MemorySnapshot::Instance().AddCpuMemory(pid, (std::get<0>(iter->second))->GetSize());
        }
        return std::get<0>(iter->second);
    }
    return nullptr;
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
        uint64_t uniqueId = it->first;
        pid_t pidCache = static_cast<pid_t>(uniqueId >> 32);
        if (pid == pidCache) {
            // no need pid, ClearMemoryCache will clear memory snapshot.
            RemoveHashMap(0, typefaceHashMap_, it->second);
            it = typefaceHashCode_.erase(it);
        } else {
            ++it;
        }
    }
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

void RSTypefaceCache::Dump() const
{
    RS_LOGI("RSTypefaceCache Dump: [");
    RS_LOGI("RSTypefaceCache Dump: "
            "%{public}-6s %{public}-16s %{public}-16s %{public}-4s %{public}-26s %{public}10s %{public}10s",
            "pid", "typefaceID", "hash_value", "ref", "familyname", "size(B)", "size(MB)");
    constexpr double KB = 1024.0;
    constexpr double MB = KB * KB;
    for (auto co : typefaceHashCode_) {
        auto iter = typefaceHashMap_.find(co.second);
        if (iter != typefaceHashMap_.end()) {
            auto [typeface, ref] = iter->second;

            int pid = GetTypefacePid(co.first);
            uint64_t typefaceId = co.first;
            uint64_t hash = co.second;
            const std::string& family = typeface->GetFamilyName();
            double sizeMB = static_cast<double>(typeface->GetSize()) / MB;
            RS_LOGI("RSTypefaceCache Dump: "
                    "%{public}6d %{public}16" PRIu64 " %{public}16" PRIu64
                    " %{public}4u %{public}26s %{public}10u %{public}10.2f",
                    pid, typefaceId, hash, ref, family.c_str(), typeface->GetSize(), sizeMB);
        }
    }
    RS_LOGI("RSTypefaceCache Dump ]");
}

void RSTypefaceCache::Dump(DfxString& log) const
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    log.AppendFormat("------------------------------------\n");
    log.AppendFormat("RSTypefaceCache Dump: \n");
    log.AppendFormat("%-6s %-16s %-4s %-26s %10s %10s\n",
        "pid", "hash_value", "ref", "familyname", "size(B)", "size(MB)");
    constexpr double KB = 1024.0;
    constexpr double MB = KB * KB;
    std::set<std::pair<int, uint64_t>> processedPairs;
    for (auto co : typefaceHashCode_) {
        auto iter = typefaceHashMap_.find(co.second);
        if (iter == typefaceHashMap_.end()) {
            continue;
        }
        auto [typeface, ref] = iter->second;

        int pid = GetTypefacePid(co.first);
        uint64_t hash = co.second;
        const std::string& family = typeface->GetFamilyName();
        double sizeMB = static_cast<double>(typeface->GetSize()) / MB;
        
        std::pair<int, uint64_t> currentPair = {pid, hash};
        if (processedPairs.find(currentPair) == processedPairs.end()) {
                log.AppendFormat("%6d %16" PRIu64 " %4u %26s %10u %10.2f\n",
                pid, hash, ref, family.c_str(), typeface->GetSize(), sizeMB);
            processedPairs.insert(currentPair);
        }
    }
}

void RSTypefaceCache::ReplaySerialize(std::stringstream& ss)
{
    size_t fontCount = 0;
    ss.write(reinterpret_cast<const char*>(&fontCount), sizeof(fontCount));

    std::lock_guard<std::mutex> lock(mapMutex_);
    for (auto co : typefaceHashCode_) {
        if (typefaceHashMap_.find(co.second) != typefaceHashMap_.end()) {
            auto [typeface, ref] = typefaceHashMap_.at(co.second);

            if (auto data = typeface->Serialize()) {
                const void* stream = data->GetData();
                size_t size = data->GetSize();

                ss.write(reinterpret_cast<const char*>(&co.first), sizeof(co.first));
                ss.write(reinterpret_cast<const char*>(&size), sizeof(size));
                ss.write(reinterpret_cast<const char*>(stream), size);
                fontCount++;
            }
        }
    }

    ss.seekp(0, std::ios_base::beg);
    ss.write(reinterpret_cast<const char*>(&fontCount), sizeof(fontCount));
    ss.seekp(0, std::ios_base::end);
}

std::string RSTypefaceCache::ReplayDeserialize(std::stringstream& ss)
{
    constexpr int bitNumber = 30 + 32;
    uint64_t replayMask = (uint64_t)1 << bitNumber;
    size_t fontCount;
    uint64_t uniqueId;
    size_t dataSize;
    std::vector<uint8_t> data;
    constexpr size_t maxTypefaceDataSize = 40'000'000;

    ss.read(reinterpret_cast<char*>(&fontCount), sizeof(fontCount));
    for (size_t i = 0; i < fontCount; i++) {
        ss.read(reinterpret_cast<char*>(&uniqueId), sizeof(uniqueId));
        ss.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

        if (dataSize > maxTypefaceDataSize) {
            return "Typeface serialized data is over 40MB";
        }
        data.resize(dataSize);
        ss.read(reinterpret_cast<char*>(data.data()), data.size());

        if (ss.eof()) {
            return "Typeface track damaged";
        }

        std::shared_ptr<Drawing::Typeface> typeface;
        typeface = Drawing::Typeface::Deserialize(data.data(), data.size());
        if (typeface) {
            uniqueId |= replayMask;
            CacheDrawingTypeface(uniqueId, typeface);
        } else {
            return "Typeface unmarshalling failed";
        }
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
