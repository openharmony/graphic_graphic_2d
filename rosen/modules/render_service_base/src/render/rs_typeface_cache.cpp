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

bool RSTypefaceCache::HasTypeface(uint64_t uniqueId, uint32_t hash)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    if (typefaceHashCode_.find(uniqueId) != typefaceHashCode_.end()) {
        // this client has already registered this typeface
        return true;
    }

    if (hash) {
        // check if someone else has already registered this typeface, add ref count and
        // mapping if so.
        if (AddIfFound(uniqueId, hash)) {
            return true;
        }

        // check if someone else is about to register this typeface -> queue uid
        std::unordered_map<uint32_t, std::vector<uint64_t>>::iterator iterator = typefaceHashQueue_.find(hash);
        if (iterator != typefaceHashQueue_.end()) {
            iterator->second.push_back(uniqueId);
            return true;
        } else {
            typefaceHashQueue_[hash] = { uniqueId };
        }
    }

    return false;
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
    std::unordered_map<uint32_t, std::vector<uint64_t>>::iterator iterator = typefaceHashQueue_.find(hash_value);
    if (iterator != typefaceHashQueue_.end()) {
        while (iterator->second.size()) {
            uint64_t back = iterator->second.back();
            if (back != uniqueId) {
                AddIfFound(back, hash_value);
            }
            iterator->second.pop_back();
        }
        typefaceHashQueue_.erase(iterator);
    }
}

static bool EmptyAfterErase(std::vector<uint64_t>& vec, size_t ix)
{
    vec.erase(vec.begin() + ix);
    return vec.empty();
}

static void RemoveHashQueue(
    std::unordered_map<uint32_t, std::vector<uint64_t>>& typefaceHashQueue, uint64_t globalUniqueId)
{
    for (auto& ref : typefaceHashQueue) {
        auto it = std::find(ref.second.begin(), ref.second.end(), globalUniqueId);
        if (it != ref.second.end()) {
            size_t ix = std::distance(ref.second.begin(), it);
            if (EmptyAfterErase(ref.second, ix)) {
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

static void PurgeMapWithPid(pid_t pid, std::unordered_map<uint32_t, std::vector<uint64_t>>& map)
{
    // go through queued items;
    std::vector<size_t> removeList;

    for (auto& ref : map) {
        size_t ix { 0 };
        std::vector<uint64_t> uniqueIdVec = ref.second;
        for (auto uid : uniqueIdVec) {
            pid_t pidCache = static_cast<pid_t>(uid >> 32);
            if (pid != pidCache) {
                ix++;
                continue;
            }
            if (EmptyAfterErase(ref.second, ix)) {
                removeList.push_back(ref.first);
                break;
            }
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
    RS_LOGI("RSTypefaceCache Dump : [");
    RS_LOGI("RSTypefaceCache Dump %{public}s",
        "---pid typefaceID-------------hash_value------------ref_count-----------familyname--------------");
    for (auto co : typefaceHashCode_) {
        if (typefaceHashMap_.find(co.second) != typefaceHashMap_.end()) {
            auto [typeface, ref] = typefaceHashMap_.at(co.second);
            RS_LOGI("%{public}s    %{public}s             %{public}s            %{public}s           %{public}s",
                "RSTypefaceCache Dump", std::to_string(co.first).c_str(), std::to_string(co.second).c_str(),
                std::to_string(ref).c_str(), typeface->GetFamilyName().c_str());
        }
    }
    RS_LOGI("RSTypefaceCache ]");
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
