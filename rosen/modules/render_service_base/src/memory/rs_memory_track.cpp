/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "memory/rs_memory_track.h"

#include "platform/common/rs_log.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MEM_MAX_SIZE = 2;
constexpr uint32_t MEM_SIZE_STRING_LEN = 10;
constexpr uint32_t MEM_ADDR_STRING_LEN = 16;
constexpr uint32_t MEM_TYPE_STRING_LEN = 16;
constexpr uint32_t MEM_PID_STRING_LEN = 8;
constexpr uint32_t MEM_WID_STRING_LEN = 20;
constexpr uint32_t MEM_SURNODE_STRING_LEN = 40;
constexpr uint32_t MEM_FRAME_STRING_LEN = 35;
constexpr uint32_t MEM_NODEID_STRING_LEN = 20;
}

MemoryNodeOfPid::MemoryNodeOfPid(size_t size, NodeId id) : nodeSize_(size), nodeId_(id) {}

size_t MemoryNodeOfPid::GetMemSize()
{
    return nodeSize_;
}

bool MemoryNodeOfPid::operator==(const MemoryNodeOfPid& other)
{
    return nodeId_ == other.nodeId_;
}

MemoryTrack& MemoryTrack::Instance()
{
    static MemoryTrack instance;
    return instance;
}

void MemoryTrack::AddNodeRecord(const NodeId id, const MemoryInfo& info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    memNodeMap_.emplace(id, info);
    MemoryNodeOfPid nodeInfoOfPid(info.size, id);
    memNodeOfPidMap_[info.pid].push_back(nodeInfoOfPid);
}

bool MemoryTrack::RemoveNodeFromMap(const NodeId id, pid_t& pid, size_t& size)
{
    auto itr = memNodeMap_.find(id);
    if (itr == memNodeMap_.end()) {
        RS_LOGD("MemoryTrack::RemoveNodeFromMap no this nodeId = %{public}" PRIu64, id);
        return false;
    }
    pid = memNodeMap_[id].pid;
    size = memNodeMap_[id].size;
    memNodeMap_.erase(itr);
    return true;
}

void MemoryTrack::RemoveNodeOfPidFromMap(const pid_t pid, const size_t size, const NodeId id)
{
    if (memNodeOfPidMap_.find(pid) == memNodeOfPidMap_.end()) {
        RS_LOGW("MemoryTrack::RemoveNodeOfPidFromMap no this nodeId = %{public}" PRIu64, id);
        return;
    }
    MemoryNodeOfPid nodeInfoOfPid = {size, id};
    auto itr = std::find(memNodeOfPidMap_[pid].begin(), memNodeOfPidMap_[pid].end(), nodeInfoOfPid);
    if (itr != memNodeOfPidMap_[pid].end()) {
        memNodeOfPidMap_[pid].erase(itr);
    }
}

void MemoryTrack::RemoveNodeRecord(const NodeId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    pid_t pid = 0;
    size_t size = 0;
    bool isSuccess = RemoveNodeFromMap(id, pid, size);
    if (!isSuccess) {
        return;
    }
    RemoveNodeOfPidFromMap(pid, size, id);
}

MemoryGraphic MemoryTrack::CountRSMemory(const pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MemoryGraphic memoryGraphic;
    auto itr = memNodeOfPidMap_.find(pid);
    if (itr == memNodeOfPidMap_.end()) {
        return memoryGraphic;
    }
    auto nodeInfoOfPid = memNodeOfPidMap_[pid];
    if (nodeInfoOfPid.empty()) {
        memNodeOfPidMap_.erase(pid);
    } else {
        int totalMemSize = 0;
        std::for_each(nodeInfoOfPid.begin(), nodeInfoOfPid.end(), [&totalMemSize](MemoryNodeOfPid& info) {
            totalMemSize += info.GetMemSize();
        });

        for (auto it = memPicRecord_.begin(); it != memPicRecord_.end(); it++) {
            pid_t picPid = it->second.pid;
            if (pid == picPid) {
                totalMemSize += static_cast<int>(it->second.size);
            }
        }
        memoryGraphic.SetPid(pid);
        memoryGraphic.SetCpuMemorySize(totalMemSize);
    }
    return memoryGraphic;
}

float MemoryTrack::GetAppMemorySizeInMB()
{
    float total = 0.f;
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [_, memInfo] : memPicRecord_) {
        total += static_cast<float>(memInfo.size);
    }
    return total / BYTE_CONVERT / BYTE_CONVERT / 2; // app mem account for 50%
}

void MemoryTrack::DumpMemoryStatistics(DfxString& log,
    std::function<std::tuple<uint64_t, std::string, RectI> (uint64_t)> func)
{
    std::lock_guard<std::mutex> lock(mutex_);
    DumpMemoryPicStatistics(log, func);
    DumpMemoryNodeStatistics(log);
}

void MemoryTrack::DumpMemoryNodeStatistics(DfxString& log)
{
    log.AppendFormat("\nRSRenderNode:\n");

    int totalSize = 0;
    int count = 0;
    //calculate by byte
    for (auto& [nodeId, info] : memNodeMap_) {
        //total of all
        totalSize += static_cast<int>(info.size);
        count++;
    }
    log.AppendFormat("Total Node Size = %d KB (%d entries)\n", totalSize / BYTE_CONVERT, count);
}

void MemoryTrack::UpdatePictureInfo(const void* addr, NodeId nodeId, pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = memPicRecord_.find(addr);
    if (itr != memPicRecord_.end()) {
        itr->second.pid = pid;
        itr->second.nid = nodeId;
    }
}

const char* MemoryTrack::MemoryType2String(MEMORY_TYPE type)
{
    switch (type) {
        case MEM_PIXELMAP : {
            return "pixelmap";
        }
        case MEM_SKIMAGE : {
            return "skimage";
        }
        default : {
            return "";
        }
    }
}

static std::string Data2String(std::string data, uint32_t tagetNumber)
{
    if (data.length() < tagetNumber) {
        return std::string(tagetNumber - data.length(), ' ') + data;
    } else {
        return data;
    }
}

std::string MemoryTrack::GenerateDumpTitle()
{
    std::string size_title = Data2String("Size", MEM_SIZE_STRING_LEN);
    std::string type_title = Data2String("Type", MEM_TYPE_STRING_LEN);
    std::string pid_title = Data2String("Pid", MEM_PID_STRING_LEN);
    std::string wid_title = Data2String("Wid", MEM_WID_STRING_LEN);
    std::string surfaceNode_title = Data2String("SurfaceName", MEM_SURNODE_STRING_LEN);
    std::string frame_title = Data2String("Frame", MEM_FRAME_STRING_LEN);
    std::string nid_title = Data2String("NodeId", MEM_NODEID_STRING_LEN);
    std::string addr_tile = Data2String("Addr", MEM_ADDR_STRING_LEN);
    return size_title + "\t" + type_title + "\t" + pid_title + "\t" + wid_title + "\t" +
        surfaceNode_title + "\t" + frame_title + nid_title + "\t" + addr_tile;
}

std::string MemoryTrack::GenerateDetail(MemoryInfo info, uint64_t wId, std::string& wName, RectI& nFrame)
{
    std::string size_str = Data2String(std::to_string(info.size), MEM_SIZE_STRING_LEN);
    std::string type_str = Data2String(MemoryType2String(info.type), MEM_TYPE_STRING_LEN);
    std::string pid_str = Data2String(std::to_string(ExtractPid(info.nid)), MEM_PID_STRING_LEN);
    std::string wid_str = Data2String(std::to_string(wId), MEM_WID_STRING_LEN);
    std::string wname_str = Data2String(wName, MEM_SURNODE_STRING_LEN);
    std::string frame_str = Data2String(nFrame.ToString(), MEM_FRAME_STRING_LEN);
    std::string nid_str = Data2String(std::to_string(info.nid), MEM_NODEID_STRING_LEN);
    return size_str + "\t"+ type_str + "\t" + pid_str + "\t" + wid_str + "\t" +
        wname_str + "\t" + frame_str + nid_str;
}

void MemoryTrack::DumpMemoryPicStatistics(DfxString& log,
    std::function<std::tuple<uint64_t, std::string, RectI> (uint64_t)> func)
{
    log.AppendFormat("RSImageCache:\n");
    log.AppendFormat("%s:\n", GenerateDumpTitle().c_str());

    int arrTotal[MEM_MAX_SIZE] = {0};
    int arrCount[MEM_MAX_SIZE] = {0};
    int totalSize = 0;
    int count = 0;
    //calculate by byte
    for (auto& [addr, info] : memPicRecord_) {
        int size = static_cast<int>(info.size / BYTE_CONVERT); // k
        //total of type
        arrTotal[info.type] += size;
        arrCount[info.type]++;

        //total of all
        totalSize += size;
        count++;
        auto [windowId, windowName, nodeFrameRect] = func(info.nid);
        log.AppendFormat("%s \t %p\n", GenerateDetail(info, windowId, windowName, nodeFrameRect).c_str(), addr);
    }

    for (uint32_t i = MEM_PIXELMAP; i < MEM_MAX_SIZE; i++) {
        MEMORY_TYPE type = static_cast<MEMORY_TYPE>(i);
        log.AppendFormat("  %s:Size = %d KB (%d entries)\n", MemoryType2String(type), arrTotal[i], arrCount[i]);
    }
    log.AppendFormat("Total Size = %d KB (%d entries)\n", totalSize, count);
}

void MemoryTrack::AddPictureRecord(const void* addr, MemoryInfo info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    memPicRecord_.emplace(addr, info);
}

void MemoryTrack::RemovePictureRecord(const void* addr)
{
    std::lock_guard<std::mutex> lock(mutex_);
    memPicRecord_.erase(addr);
}

}
}