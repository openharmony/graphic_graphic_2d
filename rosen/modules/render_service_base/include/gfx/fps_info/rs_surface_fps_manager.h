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

#ifndef RENDER_SERVICE_BASE_GFX_FPS_INFO_RS_SURFACE_FPS_MANAGER_H
#define RENDER_SERVICE_BASE_GFX_FPS_INFO_RS_SURFACE_FPS_MANAGER_H

#include <shared_mutex>
#include <unordered_map>
#include <list>
#include "rs_surface_fps.h"
#include "common/rs_common_def.h"

namespace OHOS::Rosen {
static const std::string PARAM_NAME = "-name";
static const std::string PARAM_ID = "-id";
static const std::unordered_set<std::string> options{PARAM_NAME, PARAM_ID};
static const std::unordered_set<std::string> uniRenderArgs{"DisplayNode", "composer", "UniRender", "ScreenNode"};
#define NUMBER_OF_HISTORICAL_MAX 32 // 32 Maximum number of historical data records to be stored

template<typename Key, typename Value>
class BoundedUnorderedMap {
private:
    size_t capacity_;
    std::list<std::pair<Key, Value>> list_;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> map_;
    std::function<void(Key)> callback_ = nullptr;
public:
    BoundedUnorderedMap() = default;
    BoundedUnorderedMap(size_t capacity) : capacity_(capacity) {}
    void SetMaxSize(size_t max_size)
    {
        capacity_ = max_size;
    }
    bool Get(const Key& key, Value& value)
    {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }
        value = it->second->second;
        return true;
    }
    void Put(const Key& key, const Value& value)
    {
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->second = value;
            list_.splice(list_.begin(), list_, it->second);
            return;
        }
        if (list_.size() >= capacity_) {
            auto last = list_.back();
            map_.erase(last.first);
            if (callback_ != nullptr) {
                callback_(last.first);
            }
            list_.pop_back();
        }
        list_.emplace_front(key, value);
        map_[key] = list_.begin();
    }
    void Remove(const Key& key)
    {
        auto it = map_.find(key);
        if (it != map_.end()) {
            return;
        }
        list_.erase(it->second);
        map_.erase(key);
    }
    void SetRemoveCallback(std::function<void(Key)> callback)
    {
        callback_ = callback;
    }
};
class RSB_EXPORT RSSurfaceFpsManager {
public:
    static RSSurfaceFpsManager &GetInstance();
    bool RegisterSurfaceFps(NodeId id, const std::string& name);
    bool UnregisterSurfaceFps(NodeId id);
    void RecordFlushTime(NodeId id, uint64_t vsyncId, uint64_t timestamp);
    void RecordPresentFd(uint64_t vsyncId, int32_t presentFd);
    void RecordPresentTime(int32_t presentFd, uint64_t timestamp);
    void Dump(std::string& result, const std::string& name);
    void ClearDump(std::string& result, const std::string& name);
    void Dump(std::string& result, NodeId id);
    void ClearDump(std::string& result, NodeId id);
    void DumpByPid(std::string& result, pid_t pid);
    void ClearDumpByPid(std::string& result, pid_t pid);
    void ProcessParam(
        const std::unordered_set<std::u16string>& argSets, std::string& option, std::string& argStr);
    bool IsSurface(const std::string& option, const std::string& argStr);
    void DumpSurfaceNodeFps(std::string& dumpString, const std::string& option, const std::string& arg);
    void ClearSurfaceNodeFps(std::string& dumpString, const std::string& option, const std::string& arg);
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceFps>> GetSurfaceFpsMap() const;
private:
    RSSurfaceFpsManager()
    {
        vsyncIdWithPresentFdMap_.SetMaxSize(NUMBER_OF_HISTORICAL_MAX);
        vsyncIdWithFlushTimeMap_.SetMaxSize(NUMBER_OF_HISTORICAL_MAX);
        vsyncIdWithFlushTimeMap_.SetRemoveCallback([this](uint64_t vsyncId) {
            auto range = nodeIdWithFlushTimeMap_.equal_range(vsyncId);
            nodeIdWithFlushTimeMap_.erase(range.first, range.second);
        });
    }
    ~RSSurfaceFpsManager() = default;
    RSSurfaceFpsManager(const RSSurfaceFpsManager&) = delete;
    RSSurfaceFpsManager(const RSSurfaceFpsManager&&) = delete;
    RSSurfaceFpsManager& operator=(const RSSurfaceFpsManager&) = delete;
    RSSurfaceFpsManager& operator=(const RSSurfaceFpsManager&&) = delete;

    std::shared_ptr<RSSurfaceFps> GetSurfaceFps(NodeId id);
    std::shared_ptr<RSSurfaceFps> GetSurfaceFps(const std::string& name, bool& isUnique);
    std::shared_ptr<RSSurfaceFps> GetSurfaceFpsByPid(pid_t pid);
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceFps>> surfaceFpsMap_;
    std::shared_mutex smtx;
    BoundedUnorderedMap<int32_t, uint64_t> vsyncIdWithPresentFdMap_;
    BoundedUnorderedMap<uint64_t, NodeId> vsyncIdWithFlushTimeMap_;
    std::multimap<uint64_t, std::pair<NodeId, uint64_t>> nodeIdWithFlushTimeMap_;
};
}
#endif