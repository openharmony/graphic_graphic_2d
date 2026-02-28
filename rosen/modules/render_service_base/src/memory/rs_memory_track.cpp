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

#ifdef RS_ENABLE_UNI_RENDER
#include "ability_manager_client.h"
#include "platform/common/rs_hisysevent.h"
#endif

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"
#include <fstream>
#include <sstream>
namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MEM_MAX_SIZE = 2;
constexpr uint32_t MEM_SIZE_STRING_LEN = 10;
constexpr uint32_t MEM_ADDR_STRING_LEN = 16;
constexpr uint32_t MEM_TYPE_STRING_LEN = 16;
constexpr uint32_t PIXELMAP_INFO_STRING_LEN = 36;
constexpr uint32_t MEM_PID_STRING_LEN = 8;
constexpr uint32_t MEM_WID_STRING_LEN = 20;
constexpr uint32_t MEM_UID_STRING_LEN = 8;
constexpr uint32_t MEM_SURNODE_STRING_LEN = 40;
constexpr uint32_t MEM_FRAME_STRING_LEN = 35;
constexpr uint32_t MEM_NODEID_STRING_LEN = 20;
const std::string ASHMEM_INFO_PATH = "/proc/ashmem_process_info";
const std::string DMABUF_INFO_PATH = "/proc/process_dmabuf_info";
#ifdef RS_ENABLE_UNI_RENDER
constexpr int KILL_PROCESS_TYPE = 301;
#endif
struct MemoryStats {
    int64_t totalSize = 0;
    int count = 0;
    int64_t totalWithoutDMASize = 0;
    int countWithoutDMA = 0;
    int64_t totalNullNodeSize = 0;
    int countNullNodes = 0;
    int64_t nullWithoutDMASize = 0;
    int nullWithoutDMA = 0;
    
    int64_t arrTotal[MEM_MAX_SIZE] = {0};
    int arrCount[MEM_MAX_SIZE] = {0};
    int64_t arrWithoutDMATotal[MEM_MAX_SIZE] = {0};
    int arrWithoutDMACount[MEM_MAX_SIZE] = {0};
    };
}

MemoryNodeOfPid::MemoryNodeOfPid(size_t size, NodeId id, size_t drawableNodeSize)
    : nodeSize_(size), nodeId_(id), drawableNodeSize_(drawableNodeSize) {}

size_t MemoryNodeOfPid::GetDrawableMemSize() const
{
    return drawableNodeSize_;
}

void MemoryNodeOfPid::SetDrawableMemSize(size_t size)
{
    drawableNodeSize_ += size;
}


size_t MemoryNodeOfPid::GetMemSize()
{
    return nodeSize_;
}

void MemoryNodeOfPid::SetMemSize(size_t size)
{
    nodeSize_ = size;
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

void MemoryTrack::RegisterNodeMem(const pid_t pid, size_t size, MEMORY_TYPE type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pid == 0) {
        return;
    }

    RS_LOGD("MemoryTrack::RegisterNodeMem: nodeMemOfPid_ map size = %s",
        std::to_string(nodeMemOfPid_.size()).c_str());
    auto& memData = nodeMemOfPid_[pid];
    switch (type) {
        case MEMORY_TYPE::MEM_RENDER_NODE:
            memData.first += size;
            break;
        case MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE:
            memData.second += size;
            break;
        case MEMORY_TYPE::MEM_PIXELMAP:
        case MEMORY_TYPE::MEM_SKIMAGE:
        default:
            RS_LOGE("MemoryTrack::RegisterNodeMem: invalid memory type");
            break;
    }
}

void MemoryTrack::UnRegisterNodeMem(const pid_t pid, size_t size, MEMORY_TYPE type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = nodeMemOfPid_.find(pid);
    if (it != nodeMemOfPid_.end()) {
        auto& memData = it->second;
        switch (type) {
            case MEMORY_TYPE::MEM_RENDER_NODE:
                if (memData.first >= size) {
                    memData.first -= size;
                } else {
                    RS_LOGE("MemoryTrack::UnRegisterNodeMem: MEM_RENDER_NODE size exceeds current value");
                    memData.first = 0;
                }
                break;
            case MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE:
                if (memData.second >= size) {
                    memData.second -= size;
                } else {
                    RS_LOGE("MemoryTrack::UnRegisterNodeMem: MEM_RENDER_DRAWABLE_NODE size exceeds current value");
                    memData.second = 0;
                }
                break;
            case MEMORY_TYPE::MEM_PIXELMAP:
            case MEMORY_TYPE::MEM_SKIMAGE:
            default:
                RS_LOGE("MemoryTrack::UnRegisterNodeMem: invalid memory type");
                break;
        }
        // remove no exist pid
        if (memData.first == 0 && memData.second == 0) {
            nodeMemOfPid_.erase(it);
        }
    }
}

size_t MemoryTrack::GetNodeMemoryOfPid(const pid_t pid, MEMORY_TYPE type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = nodeMemOfPid_.find(pid);
    if (itr == nodeMemOfPid_.end()) {
        return 0;
    }
    switch (type) {
        case MEMORY_TYPE::MEM_RENDER_NODE:
            return itr->second.first / BYTE_CONVERT;
            break;
        case MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE:
            return itr->second.second / BYTE_CONVERT;
            break;
        case MEMORY_TYPE::MEM_PIXELMAP:
        case MEMORY_TYPE::MEM_SKIMAGE:
        default:
            RS_LOGE("MemoryTrack::GetNodeMemoryOfPid: invalid memory type");
            break;
    }
    return 0;
}

void MemoryTrack::AddNodeRecord(const NodeId id, const MemoryInfo& info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MemoryNodeOfPid nodeInfoOfPid(info.size, id);
    auto itr = memNodeMap_.find(id);
    if (itr == memNodeMap_.end()) {
        memNodeMap_.emplace(id, info);
        memNodeOfPidMap_[info.pid].push_back(nodeInfoOfPid);
        return;
    } else if (info.size > itr->second.size) {
        nodeInfoOfPid.SetMemSize(itr->second.size);
        itr->second.size = info.size;
    } else {
        return;
    }
    
    if (memNodeOfPidMap_.find(info.pid) != memNodeOfPidMap_.end()) {
        auto pidMemItr = std::find(memNodeOfPidMap_[info.pid].begin(),
            memNodeOfPidMap_[info.pid].end(), nodeInfoOfPid);
        if (pidMemItr != memNodeOfPidMap_[info.pid].end()) {
            pidMemItr->SetMemSize(info.size);
        } else {
            nodeInfoOfPid.SetMemSize(info.size);
            memNodeOfPidMap_[info.pid].push_back(nodeInfoOfPid);
        }
    } else {
        nodeInfoOfPid.SetMemSize(info.size);
        memNodeOfPidMap_[info.pid].push_back(nodeInfoOfPid);
    }
}

bool MemoryTrack::RemoveNodeFromMap(const NodeId id, pid_t& pid, size_t& size)
{
    auto itr = memNodeMap_.find(id);
    if (itr == memNodeMap_.end()) {
        RS_LOGD("MemoryTrack::RemoveNodeFromMap no this nodeId = %{public}" PRIu64, id);
        return false;
    }
    pid = static_cast<pid_t>(memNodeMap_[id].pid);
    size = memNodeMap_[id].size;
    memNodeMap_.erase(itr);
    return true;
}

void MemoryTrack::RemoveNodeOfPidFromMap(const pid_t pid, const size_t size, const NodeId id)
{
    if (memNodeOfPidMap_.find(pid) == memNodeOfPidMap_.end()) {
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
        uint64_t totalMemSize = 0;
        std::for_each(nodeInfoOfPid.begin(), nodeInfoOfPid.end(), [&totalMemSize](MemoryNodeOfPid& info) {
            totalMemSize += static_cast<uint64_t>(info.GetMemSize());
        });

        for (auto it = memPicRecord_.begin(); it != memPicRecord_.end(); it++) {
            pid_t picPid = static_cast<pid_t>(it->second.pid);
            if (pid == picPid) {
                totalMemSize += static_cast<uint64_t>(it->second.size);
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
    std::function<std::tuple<uint64_t, std::string, RectI, bool> (uint64_t)> func, bool isLite)
{
    std::lock_guard<std::mutex> lock(mutex_);
    DumpMemoryPicStatistics(log, func, isLite);
    DumpMemoryNodeStatistics(log, isLite);
}

void MemoryTrack::DumpMemoryNodeStatistics(DfxString& log, bool isLite)
{
    RS_TRACE_NAME_FMT("MemoryTrack::DumpMemoryNodeStatistics pixlmap record size:%d", memPicRecord_.size());
    log.AppendFormat("\nRSRenderNode:\n");
    uint64_t totalSize = 0;
    int count = 0;
    if (!isLite) {
        // calculate by byte
        for (auto& [nodeId, info] : memNodeMap_) {
            // total of all
            totalSize += static_cast<uint64_t>(info.size);
            count++;
        }
        log.AppendFormat("Total Node Size = %d KB (%d entries)\n", totalSize / BYTE_CONVERT, count);
    } else {
        std::unordered_map<int, int> pidCountMap;  // replace getNodeInfo
        // calculate by byte
        for (auto& [nodeId, info] : memNodeMap_) {
            // total of all
            totalSize += static_cast<uint64_t>(info.size);
            count++;
            pidCountMap[info.pid]++;
        }
        log.AppendFormat("Total Node Size = %d KB (%d entries)\n", totalSize / BYTE_CONVERT, count);
        std::string log_str = "Pid" + std::string("\t") + "Count";
        log.AppendFormat("%s\n", log_str.c_str());
        for (const auto& [pid, pidCount] : pidCountMap) {
            log_str = std::to_string(pid) + "\t" + std::to_string(pidCount);
            log.AppendFormat("%s\n", log_str.c_str());
        }
    }
}

void MemoryTrack::RemovePidRecord(const pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    memNodeOfPidMap_.erase(pid);
}

void MemoryTrack::UpdatePictureInfo(const void* addr, NodeId nodeId, pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = memPicRecord_.find(addr);
    if (itr != memPicRecord_.end()) {
        itr->second.pid = static_cast<uint32_t>(pid);
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

const std::string MemoryTrack::PixelMapInfo2String(MemoryInfo info)
{
    std::string allocTypeStr = AllocatorType2String(info.allocType);
    std::string pixelFormatStr = "UNDEFINED";
#ifdef ROSEN_OHOS
    pixelFormatStr = PixelFormat2String(info.pixelMapFormat);
#endif
    return allocTypeStr + "," + pixelFormatStr;
}

#ifdef ROSEN_OHOS
const std::string MemoryTrack::PixelFormat2String(Media::PixelFormat type)
{
    // sync with foundation/multimedia/image_framework/interfaces/innerkits/include/image_type.h
    switch (type) {
        case Media::PixelFormat::ARGB_8888:
            return "ARGB_8888";
        case Media::PixelFormat::RGB_565:
            return "RGB_565";
        case Media::PixelFormat::RGBA_8888:
            return "RGBA_8888";
        case Media::PixelFormat::BGRA_8888:
            return "BGRA_8888";
        case Media::PixelFormat::RGB_888:
            return "RGB_888";
        case Media::PixelFormat::ALPHA_8:
            return "ALPHA_8";
        case Media::PixelFormat::RGBA_F16:
            return "RGBA_F16";
        case Media::PixelFormat::NV21:
            return "NV21";
        case Media::PixelFormat::NV12:
            return "NV12";
        case Media::PixelFormat::RGBA_1010102:
            return "RGBA_1010102";
        case Media::PixelFormat::YCBCR_P010:
            return "YCBCR_P010";
        case Media::PixelFormat::YCRCB_P010:
            return "YCRCB_P010";
        case Media::PixelFormat::RGBA_U16:
            return "RGBA_U16";
        case Media::PixelFormat::YUV_400:
            return "YUV_400";
        case Media::PixelFormat::CMYK:
            return "CMYK";
        case Media::PixelFormat::ASTC_4x4:
            return "ASTC_4x4";
        case Media::PixelFormat::ASTC_6x6:
            return "ASTC_6x6";
        case Media::PixelFormat::ASTC_8x8:
            return "ASTC_8x8";
        default:
            return std::to_string(static_cast<int32_t>(type));
    }
    return "UNKNOW";
}
#endif

const std::string MemoryTrack::AllocatorType2String(OHOS::Media::AllocatorType type)
{
    switch (type) {
        case OHOS::Media::AllocatorType::DEFAULT:
            return "DEFAULT";
        case OHOS::Media::AllocatorType::HEAP_ALLOC:
            return "HEAP";
        case OHOS::Media::AllocatorType::SHARE_MEM_ALLOC:
            return "SHARE_MEM";
        case OHOS::Media::AllocatorType::CUSTOM_ALLOC:
            return "CUSTOM";
        case OHOS::Media::AllocatorType::DMA_ALLOC:
            return "DMA";
        default :
            return "UNKNOW";
    }
    return "UNKNOW";
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
    std::string pixelmap_info_title = Data2String("Type,Format", PIXELMAP_INFO_STRING_LEN);
    std::string pid_title = Data2String("Pid", MEM_PID_STRING_LEN);
    std::string wid_title = Data2String("Wid", MEM_WID_STRING_LEN);
    std::string uid_title = Data2String("Uid", MEM_UID_STRING_LEN);
    std::string surfaceNode_title = Data2String("SurfaceName", MEM_SURNODE_STRING_LEN);
    std::string frame_title = Data2String("Frame", MEM_FRAME_STRING_LEN);
    std::string nid_title = Data2String("NodeId", MEM_NODEID_STRING_LEN);
    std::string addr_tile = Data2String("Addr", MEM_ADDR_STRING_LEN);
    return size_title + "\t" + type_title + "\t" + pixelmap_info_title + "\t" + pid_title + "\t" + wid_title + "\t" +
        uid_title + "\t" + surfaceNode_title + "\t" + frame_title + nid_title + "\t" + addr_tile;
}

std::string MemoryTrack::GenerateDetail(MemoryInfo info, uint64_t wId, std::string& wName, RectI& nFrame)
{
    std::string size_str = Data2String(std::to_string(info.size), MEM_SIZE_STRING_LEN);
    std::string type_str = Data2String(MemoryType2String(info.type), MEM_TYPE_STRING_LEN);
    std::string pixelmap_info_str = Data2String(PixelMapInfo2String(info), PIXELMAP_INFO_STRING_LEN);
    std::string pid_str = Data2String(std::to_string(ExtractPid(info.nid)), MEM_PID_STRING_LEN);
    std::string wid_str = Data2String(std::to_string(wId), MEM_WID_STRING_LEN);
    std::string uid_str = Data2String(std::to_string(info.uid), MEM_UID_STRING_LEN);
    std::string wname_str = Data2String(wName, MEM_SURNODE_STRING_LEN);
    std::string frame_str = Data2String(nFrame.ToString(), MEM_FRAME_STRING_LEN);
    std::string nid_str = Data2String(std::to_string(info.nid), MEM_NODEID_STRING_LEN);
    return size_str + "\t" + type_str + "\t" + pixelmap_info_str + "\t" + pid_str + "\t" + wid_str + "\t" +
        uid_str + "\t" + wname_str + "\t" + frame_str + nid_str;
}

void MemoryTrack::DumpMemoryPicStatistics(DfxString& log,
    std::function<std::tuple<uint64_t, std::string, RectI, bool>(uint64_t)> func, bool isLite)
{
    RS_TRACE_NAME_FMT("MemoryTrack::DumpMemoryPicStatistics memPicRecord_ size:%d", memPicRecord_.size());
    log.AppendFormat("RSImageCache:\n");
    MemoryStats stats;
    if (!isLite) log.AppendFormat("%s:\n", GenerateDumpTitle().c_str());
    for (auto& [addr, info] : memPicRecord_) {
        int64_t size = static_cast<int64_t>(info.size / BYTE_CONVERT);
        stats.arrTotal[info.type] += size;
        stats.arrCount[info.type]++;
        stats.totalSize += size;
        stats.count++;
        if (info.allocType != OHOS::Media::AllocatorType::DMA_ALLOC) {
            stats.arrWithoutDMATotal[info.type] += size;
            stats.arrWithoutDMACount[info.type]++;
            stats.totalWithoutDMASize += size;
            stats.countWithoutDMA++;
        }
        auto [windowId, windowName, nodeFrameRect, isNodeNull] = func(info.nid);
        if (!isLite) {
            log.AppendFormat("%s", GenerateDetail(info, windowId, windowName, nodeFrameRect).c_str());
            log.AppendFormat("     %d\n", isNodeNull ? 1 : 0);
        }
        if (info.type == MEMORY_TYPE::MEM_PIXELMAP && isNodeNull) {
            stats.totalNullNodeSize += size;
            stats.countNullNodes++;
            if (info.allocType != OHOS::Media::AllocatorType::DMA_ALLOC) {
                stats.nullWithoutDMASize += size;
                stats.nullWithoutDMA++;
            }
        }
    }

    for (uint32_t i = MEM_PIXELMAP; i < MEM_MAX_SIZE; i++) {
        MEMORY_TYPE type = static_cast<MEMORY_TYPE>(i);
        log.AppendFormat("  %s:Size = %lld KB (%d entries)\n", MemoryType2String(type),
            stats.arrTotal[i], stats.arrCount[i]);
        log.AppendFormat("  %s Without DMA:Size = %lld KB (%d entries)\n",
            MemoryType2String(type), stats.arrWithoutDMATotal[i], stats.arrWithoutDMACount[i]);
    }
    log.AppendFormat("Total Size = %lld KB (%d entries)\n", stats.totalSize, stats.count);
    log.AppendFormat("Total Without DMA Size = %lld KB (%d entries)\n",
        stats.totalWithoutDMASize, stats.countWithoutDMA);
    log.AppendFormat("MEM_PIXELMAP Null Nodes: Size = %lld KB (%d entries)\n",
        stats.totalNullNodeSize, stats.countNullNodes);
    log.AppendFormat("Null Nodes Without DMA: Size = %lld KB (%d entries)\n",
        stats.nullWithoutDMASize, stats.nullWithoutDMA);
}

void MemoryTrack::AddPictureRecord(const void* addr, MemoryInfo info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    memPicRecord_.emplace(addr, info);
    if (!(info.allocType == Media::AllocatorType::DMA_ALLOC ||
        info.allocType ==Media::AllocatorType::SHARE_MEM_ALLOC) || info.type != MEM_PIXELMAP) {
        return;
    }
    fdNumOfPid_[info.pid].insert(addr);
}

void MemoryTrack::RemovePictureRecord(const void* addr)
{
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t pid = 0;
    if (memPicRecord_.find(addr) != memPicRecord_.end()) {
        pid = static_cast<uint32_t>(memPicRecord_[addr].pid);
    }
    
    fdNumOfPid_[pid].erase(addr);
    memPicRecord_.erase(addr);
}

void MemoryTrack::KillProcessByPid(const pid_t pid, const std::string& reason)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pid == 0) {
        return;
    }
#ifdef RS_ENABLE_UNI_RENDER
    FdOverReport(pid, RSEventName::RENDER_MEMORY_OVER_WARNING, "");
    AAFwk::ExitReason killReason{AAFwk::Reason::REASON_RESOURCE_CONTROL, KILL_PROCESS_TYPE, reason};
    int32_t ret = (int32_t)AAFwk::AbilityManagerClient::GetInstance()->KillProcessWithReason(pid, killReason);
    if (ret == ERR_OK) {
        auto it = fdNumOfPid_.find(pid);
        if (it != fdNumOfPid_.end()) {
            fdNumOfPid_.erase(it);
        }
    }
#endif
}

void MemoryTrack::FdOverReport(const pid_t pid, const std::string& reportName,
    const std::string& hidumperReport)
{
#ifdef RS_ENABLE_UNI_RENDER
    std::string ashmemInfo;
    std::ifstream ashmemInfoFile;
    ashmemInfoFile.open(ASHMEM_INFO_PATH);
    if (ashmemInfoFile.is_open()) {
        std::stringstream ashmemInfoStream;
        ashmemInfoStream << ashmemInfoFile.rdbuf();
        FilterAshmemInfoByPid(ashmemInfo, ashmemInfoStream.str(), pid);
        ashmemInfoFile.close();
    } else {
        ashmemInfo = reportName;
        RS_LOGE("MemoryTrack::FdOverReport can not open ashmemInfo");
    }

    std::string dmaBufInfo;
    std::ifstream dmaBufInfoFile;
    dmaBufInfoFile.open(DMABUF_INFO_PATH);
    if (dmaBufInfoFile.is_open()) {
        std::stringstream dmaBufInfoStream;
        dmaBufInfoStream << dmaBufInfoFile.rdbuf();
        FilterDmaheapInfoByPid(dmaBufInfo, dmaBufInfoStream.str(), pid);
        dmaBufInfoFile.close();
    } else {
        dmaBufInfo = reportName;
        RS_LOGE("MemoryTrack::FdOverReport can not open dmaBufInfo");
    }

    std::string combinedInfo = "=== AshmemInfo ===\n" + ashmemInfo +
        "\n\n=== DmaBufInfo ===\n" + dmaBufInfo;

    std::string filePath = "/data/service/el0/render_service/renderservice_fdmem.txt";
    WriteInfoToFile(filePath, combinedInfo, hidumperReport);

    RS_TRACE_NAME("MemoryTrack::FdOverReport HiSysEventWrite");

    int ret = RSHiSysEvent::EventWrite(reportName, RSEventType::RS_STATISTIC,
        "PID", pid,
        "TYPE", "FD",
        "FILEPATH", filePath);
    RS_LOGW("hisysevent writ result=%{public}d, send event [FRAMEWORK,PROCESS_KILL], "
        "pid[%{public}d]", ret, pid);
#endif
}

void MemoryTrack::WriteInfoToFile(std::string& filePath, std::string& memInfo, const std::string& hidumperReport)
{
#ifdef RS_ENABLE_UNI_RENDER
    std::ofstream tempFile(filePath);
    if (tempFile.is_open()) {
        tempFile << "\n******************************\n";
        tempFile << memInfo;
        tempFile << "\n************ endl ************\n";
    } else {
        RS_LOGE("MemoryTrack::file open fail!");
    }
    if (!hidumperReport.empty()) {
        if (tempFile.is_open()) {
            tempFile << "\n******************************\n";
            tempFile << "LOGGER_RENDER_SERVICE_MEM\n";
            tempFile << hidumperReport;
        } else {
            RS_LOGE("MemoryTrack::file open fail!");
        }
    }
    tempFile.close();
#endif
}

uint32_t MemoryTrack::CountFdRecordOfPid(uint32_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = fdNumOfPid_.find(pid);
    if (it != fdNumOfPid_.end()) {
        return it->second.size();
    }
    return 0;
}

#ifdef RS_MEMORY_INFO_MANAGER
void MemoryTrack::SetGlobalRootNodeStatusChangeFlag(bool flag)
{
    globalRootNodeStatusChangeFlag.store(flag);
}

bool MemoryTrack::GetGlobalRootNodeStatusChangeFlag()
{
    return globalRootNodeStatusChangeFlag.load();
}

NODE_ON_TREE_STATUS MemoryTrack::GetNodeOnTreeStatus(const void* address)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto picRecordIt = memPicRecord_.find(address);
    if (picRecordIt == memPicRecord_.end()) {
        return NODE_ON_TREE_STATUS::STATUS_INVALID;
    }

    NodeId nodeId = picRecordIt->second.nid;
    auto nodeInfoIt = memNodeMap_.find(nodeId);
    if (nodeInfoIt == memNodeMap_.end()) {
        return NODE_ON_TREE_STATUS::STATUS_INVALID;
    }

    const auto& nodeInfo = nodeInfoIt->second;
    if (nodeInfo.isOnTree && !nodeInfo.rootNodeStatusChangeFlag) {
        return NODE_ON_TREE_STATUS::STATUS_ON_TREE;
    } else if (nodeInfo.isOnTree) {
        return NODE_ON_TREE_STATUS::STATUS_ON_TREE_IN_ROOT;
    } else if (!nodeInfo.isOnTree && !nodeInfo.rootNodeStatusChangeFlag) {
        return NODE_ON_TREE_STATUS::STATUS_OFF_TREE;
    } else {
        return NODE_ON_TREE_STATUS::STATUS_OFF_TREE_IN_ROOT;
    }
}

void MemoryTrack::SetNodeOnTreeStatus(NodeId nodeId, bool rootNodeStatusChangeFlag, bool isOnTree)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto itr = memNodeMap_.find(nodeId);
    if (itr == memNodeMap_.end()) {
        return;
    }
    itr->second.rootNodeStatusChangeFlag = rootNodeStatusChangeFlag;
    itr->second.isOnTree = isOnTree;
}
#endif

void MemoryTrack::FilterAshmemInfoByPid(std::string& out, const std::string& info, const pid_t pid)
{
    std::string pidStr = std::to_string(pid);
    std::istringstream iss(info);
    std::string line;

    while (std::getline(iss, line)) {
        if (line.find("Process ashmem overview info:") != std::string::npos ||
            line.find("Process ashmem detail info:") != std::string::npos ||
            line.find("Process_name") != std::string::npos ||
            line.find("Total ashmem") != std::string::npos ||
            line.find("---") != std::string::npos) {
            out += line + "\n";
            continue;
        }

        std::istringstream lineStream(line);
        std::string token;
        std::getline(lineStream, token, '\t'); // discard first col
        if (std::getline(lineStream, token, '\t') && (token == pidStr)) {
            out += line + "\n";
        }
    }
}

void MemoryTrack::FilterDmaheapInfoByPid(std::string& out, const std::string& info, const pid_t pid)
{
    std::string pidStr = std::to_string(pid);
    std::istringstream iss(info);
    std::string line;
    bool hasMatchingData = false;

    while (std::getline(iss, line)) {
        if (line.find("Dma-buf objects usage of processes:") != std::string::npos ||
            line.find("Process") != std::string::npos) {
            out += line + "\n";
            continue;
        }

        if (line.find("Total dmabuf size") != std::string::npos) {
            if (hasMatchingData) {
                out += line + "\n";
                hasMatchingData = false;
                break;
            }
            continue;
        }

        std::istringstream lineStream(line);
        std::string token;
        lineStream >> token; // discard first col
        if ((lineStream >> token) && (token == pidStr)) {
            out += line + "\n";
            hasMatchingData = true;
        }
    }
}
}
}