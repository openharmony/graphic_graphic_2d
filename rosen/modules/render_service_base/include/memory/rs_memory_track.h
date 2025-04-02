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

#include "include/core/SkImage.h"

#include "common/rs_common_def.h"
#include "common/rs_rect.h"
#include "memory/rs_dfx_string.h"
#include "memory/rs_memory_graphic.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
constexpr int BYTE_CONVERT = 1024;
enum MEMORY_TYPE {
    MEM_PIXELMAP,
    MEM_SKIMAGE,
    MEM_RENDER_NODE
};

struct MemoryInfo {
    size_t size = 0;
    int pid = 0;
    uint64_t nid = 0;
    uint64_t uid = 0;
    MEMORY_TYPE type = MEMORY_TYPE::MEM_PIXELMAP;
    int initialPid = 0; // Do not update it
    OHOS::Media::AllocatorType allocType;
    OHOS::Media::PixelFormat pixelMapFormat;
};

class MemoryNodeOfPid {
public:
    MemoryNodeOfPid() = default;
    ~MemoryNodeOfPid() = default;
    MemoryNodeOfPid(size_t size, NodeId id);
    size_t GetMemSize();
    void SetMemSize(size_t size);
    bool operator==(const MemoryNodeOfPid& other);
private:
    size_t nodeSize_ = 0;
    NodeId nodeId_ = 0;
};

class RSB_EXPORT MemoryTrack {
public:
    static MemoryTrack& Instance();
    void AddNodeRecord(const NodeId id, const MemoryInfo& info);
    void RemoveNodeRecord(const NodeId id);
    void DumpMemoryStatistics(DfxString& log,
        std::function<std::tuple<uint64_t, std::string, RectI, bool> (uint64_t)> func);
    void AddPictureRecord(const void* addr, MemoryInfo info);
    void RemovePictureRecord(const void* addr);
    bool GetPictureRecordMemInfo(const void* addr, MemoryInfo& info);
    void UpdatePictureInfo(const void* addr, NodeId nodeId, pid_t pid);
    // count memory for hidumper
    MemoryGraphic CountRSMemory(const pid_t pid);
    float GetAppMemorySizeInMB();
    const std::unordered_map<NodeId, MemoryInfo>& GetMemNodeMap() { return memNodeMap_; }
private:
    MemoryTrack() = default;
    ~MemoryTrack() = default;
    MemoryTrack(const MemoryTrack&) = delete;
    MemoryTrack(const MemoryTrack&&) = delete;
    MemoryTrack& operator=(const MemoryTrack&) = delete;
    MemoryTrack& operator=(const MemoryTrack&&) = delete;
    const char* MemoryType2String(MEMORY_TYPE type);
    const std::string PixelMapInfo2String(MemoryInfo info);
    const std::string AllocatorType2String(OHOS::Media::AllocatorType);
    const std::string PixelFormat2String(OHOS::Media::PixelFormat);
    std::string GenerateDumpTitle();
    std::string GenerateDetail(MemoryInfo info, uint64_t windowId, std::string& windowName, RectI& nodeFrameRect);
    void DumpMemoryNodeStatistics(DfxString& log);
    void DumpMemoryPicStatistics(DfxString& log,
        std::function<std::tuple<uint64_t, std::string, RectI, bool> (uint64_t)> func,
        const std::vector<MemoryInfo>& memPicRecord = {});
    bool RemoveNodeFromMap(const NodeId id, pid_t& pid, size_t& size);
    void RemoveNodeOfPidFromMap(const pid_t pid, const size_t size, const NodeId id);
    std::mutex mutex_;
    std::unordered_map<NodeId, MemoryInfo> memNodeMap_;
    std::unordered_map<const void*, MemoryInfo> memPicRecord_;

    // Data to statistic information of Pid
    std::unordered_map<pid_t, std::vector<MemoryNodeOfPid>> memNodeOfPidMap_;
};
} // namespace OHOS
} // namespace Rosen
#endif