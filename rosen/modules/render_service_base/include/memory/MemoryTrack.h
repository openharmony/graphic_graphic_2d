/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef MEMORY_TRACK
#define MEMORY_TRACK

#include <mutex>
#include <vector>

#include "common/rs_common_def.h"
#include "memory/DfxString.h"
namespace OHOS {
namespace Rosen {
constexpr int BYTE_CONVERT = 1024;
enum MEMORY_TYPE {
    MEM_PIXELMAP,
    MEM_SKIMAGE,
    MEM_RENDER_NODE
};

struct MemoryInfo {
    size_t size;
    int pid;
    MEMORY_TYPE type;
};

class MemoryNodeOfPid {
public:
    MemoryNodeOfPid() = default;
    ~MemoryNodeOfPid() = default;
    MemoryNodeOfPid(size_t size, NodeId id);
    size_t GetMemSize() { return nodeSize_; };
    bool operator==(const MemoryNodeOfPid& other);
private:
    size_t nodeSize_;
    NodeId nodeId_;
};

class RSB_EXPORT MemoryTrack {
public:
    static MemoryTrack& Instance();
    void AddNodeRecord(const NodeId id, const MemoryInfo& info);
    void RemoveNodeRecord(const NodeId id);
    void DumpMemoryStatistics(DfxString& log);
    void DumpMemoryStatistics(DfxString& log, const pid_t pid);

    void AddPictureRecord(const void* addr, MemoryInfo info);
    void RemovePictureRecord(const void* addr);
private:
    MemoryTrack() = default;
    ~MemoryTrack() = default;
    MemoryTrack(const MemoryTrack&) = delete;
    MemoryTrack(const MemoryTrack&&) = delete;
    MemoryTrack& operator=(const MemoryTrack&) = delete;
    MemoryTrack& operator=(const MemoryTrack&&) = delete;
    const char* MemoryType2String(MEMORY_TYPE type);
    void DumpMemoryNodeStatistics(DfxString& log);
    void DumpMemoryPicStatistics(DfxString& log);
    std::mutex mutex_;
    std::unordered_map<NodeId, MemoryInfo> memNodeMap_;
    std::unordered_map<const void*, MemoryInfo> memPicRecord_;

    // Data to statistic information of Pid
    std::unordered_map<pid_t, std::vector<MemoryNodeOfPid>> memNodeOfPidMap_;
};
} // namespace OHOS  
} // namespace Rosen
#endif