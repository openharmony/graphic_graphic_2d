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

void RSTypefaceCache::CacheDrawingTypeface(uint64_t uniqueId, std::shared_ptr<Drawing::Typeface> typeface)
{
    if (!typeface || uniqueId == 0) {
        return;
    }

    std::lock_guard<std::mutex> lock(mapMutex_);
    if (typefaceHashCode_.find(uniqueId) != typefaceHashCode_.end()) {
        return;
    }
    uint64_t fullHash = 0;
    if (typeface->GetFd() != INVALID_FD) {
        fullHash = CalculateTypefaceFullHash(typeface);
    } else {
        fullHash = typeface->GetHash();
        if (!fullHash) { // fallback to slow path if the adapter does not provide hash
            std::shared_ptr<Drawing::Data> data = typeface->Serialize();
            if (!data) {
                return;
            }
            const void* stream = data->GetData();
            size_t size = data->GetSize();
#ifdef USE_M133_SKIA
            fullHash = SkChecksum::Hash32(stream, std::min(size, static_cast<size_t>(MAX_CHUNK_SIZE)));
#else
            fullHash = SkOpts::hash_fn(stream, std::min(size, static_cast<size_t>(MAX_CHUNK_SIZE)), 0);
#endif
        }
    }

    typefaceHashCode_[uniqueId] = fullHash;
    pid_t pid = GetTypefacePid(uniqueId);
    if (typefaceHashMap_.find(fullHash) != typefaceHashMap_.end()) {
        if (pid) {
            MemorySnapshot::Instance().AddCpuMemory(pid, typeface->GetSize());
        }
        auto [faceCache, ref] = typefaceHashMap_[fullHash];
        if (faceCache->GetFamilyName() != typeface->GetFamilyName()) {
            // hash collision
            typefaceHashCode_[uniqueId] = uniqueId;
            typefaceHashMap_[uniqueId] = std::make_tuple(typeface, 1);
            RS_LOGI("CacheDrawingTypeface hash collision");
        } else {
            typefaceHashMap_[fullHash] = std::make_tuple(faceCache, ref + 1);
        }
        return;
    }
    typefaceHashMap_[fullHash] = std::make_tuple(typeface, 1);
    if (pid) {
        MemorySnapshot::Instance().AddCpuMemory(pid, typeface->GetSize());
    }
    typefaceBaseHashMap_[typeface->GetHash()] = std::make_tuple(typeface, 1);
    if (typeface->GetFd() != INVALID_FD) {
        return;
    }
    // register queued entries old ipc
    auto iterator = typefaceHashQueue_.find(fullHash);
    if (iterator != typefaceHashQueue_.end()) {
        for (const uint64_t cacheId: iterator->second) {
            if (cacheId != uniqueId) {
                AddIfFound(cacheId, fullHash);
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
        auto& [typeface, ref] = typefaceHashMap[hash_value];
        if (pid) {
            MemorySnapshot::Instance().RemoveCpuMemory(pid, typeface->GetSize());
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
    auto iter = typefaceHashMap_.find(fullHash);
    if (iter != typefaceHashMap_.end()) {
        typefaceHashCode_[sharedTypeface.id_] = fullHash;
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
            typefaceHashCode_[sharedTypeface.id_] = fullHash;
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
    CacheDrawingTypeface(sharedTypeface.id_, clonedTypeface);
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

uint32_t CalcCustomFontPss()
{
    std::string s;
#ifdef IS_OHOS
    LoadStringFromFile("/proc/self/smaps", s);
#endif
    std::stringstream iss(s);
    std::string line;
    bool inCustomFont = false;
    uint32_t pss = 0;

    while (std::getline(iss, line)) {
        if (line.find('-') != std::string::npos && line.find("dev/ashmem/") != std::string::npos) {
            inCustomFont = (line.find("[custom font]") != std::string::npos);
            continue;
        }

        if (inCustomFont && line.rfind("Pss:", 0) == 0) {
            std::istringstream pssLine(line);
            std::string key;
            std::string unit;
            uint32_t value = 0;
            pssLine >> key >> value >> unit;
            pss += value;
            inCustomFont = false;
        }
    }

    return pss;
}

void RSTypefaceCache::Dump(DfxString& log) const
{
    RS_TRACE_NAME_FMT("RSTypefaceCache::Dump typefaceHashCode size:%d", typefaceHashCode_.size());
    std::lock_guard<std::mutex> lock(mapMutex_);
    uint32_t totalMem = std::accumulate(typefaceHashMap_.begin(), typefaceHashMap_.end(), 0u,
        [](uint32_t sum, const auto& item) { return sum + std::get<0>(item.second)->GetSize(); });
    constexpr double KB = 1024.0;
    constexpr double MB = KB * KB;
    log.AppendFormat("------------------------------------\n");
    log.AppendFormat("RSTypefaceCache Dump:\nTotal: %.2fKB, %.2fMB\n",
        static_cast<double>(totalMem) / KB, static_cast<double>(totalMem) / MB);
    double pssMem = static_cast<double>(CalcCustomFontPss());
    log.AppendFormat("Pss:   %.2fKB %.2fMB\n", pssMem, pssMem / KB);
    log.AppendFormat("%-6s %-16s %-4s %-26s %-10s %-10s\n",
        "pid", "hash_value", "ref", "familyname", "size(B)", "size(MB)");
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
            log.AppendFormat("%-6d %-16" PRIu64 " %-4u %-26s %-10u %-10.2f\n",
                pid, hash, ref, family.c_str(), typeface->GetSize(), sizeMB);
            processedPairs.insert(currentPair);
        }
    }
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
    constexpr size_t maxSize = 40'000'000;
    constexpr uint32_t bitNumber = 30 + 32;
    constexpr uint64_t replayMask = uint64_t(1) << bitNumber;

    size_t count = 0;
    stream.read(reinterpret_cast<char*>(&count), sizeof(count));

    std::shared_ptr<Drawing::Typeface> typeface;
    std::vector<uint8_t> data;
    for (size_t i = 0; i < count; i++) {
        uint64_t uniqueId = 0;
        stream.read(reinterpret_cast<char*>(&uniqueId), sizeof(uniqueId));

        size_t size = 0;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));

        constexpr size_t dummy = std::numeric_limits<size_t>::max();
        if (dummy == size) {
            CacheDrawingTypeface(uniqueId | replayMask, typeface);
            continue;
        }

        if (size > maxSize) {
            return "Typeface serialized data is over 40MB";
        }

        data.resize(size);
        stream.read(reinterpret_cast<char*>(data.data()), data.size());

        if (stream.eof()) {
            return "Typeface track is damaged";
        }

        typeface = Drawing::Typeface::Deserialize(data.data(), data.size());
        if (!typeface) {
            return "Typeface unmarshalling failed";
        }
        CacheDrawingTypeface(uniqueId | replayMask, typeface);
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
