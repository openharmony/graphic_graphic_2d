/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "memory/rs_pixelmap_fd_track.h"

#include <fstream>
#include <sstream>

#ifdef RS_ENABLE_UNI_RENDER
#include "ability_manager_client.h"
#include "platform/common/rs_hisysevent.h"
#include "xcollie/process_kill_reason.h"
#endif

#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t FD_COUNT_DMA = 2;
constexpr int32_t FD_COUNT_SHARE_MEM = 1;
constexpr int32_t INVALID_PID = 0;
#ifdef RS_ENABLE_UNI_RENDER
constexpr int32_t KILL_PROCESS_TYPE = 301;
#endif
const std::string ASHMEM_INFO_PATH = "/proc/ashmem_process_info";
const std::string DMABUF_INFO_PATH = "/proc/process_dmabuf_info";
const std::string RS_FD_MEM_PATH = "/data/service/el0/render_service/renderservice_fdmem.txt";
}

void RSPixelMapFdTrack::AddFdRecord(int32_t pid, const void* addr, Media::AllocatorType allocType)
{
    if (pid <= INVALID_PID) {
        return;
    }
    if (allocType != Media::AllocatorType::SHARE_MEM_ALLOC && allocType != Media::AllocatorType::DMA_ALLOC) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto [it, isNewElement] = addrTracker_[pid].insert(addr);
    if (!isNewElement) {
        return;
    }
    if (allocType == Media::AllocatorType::SHARE_MEM_ALLOC) {
        fdCountTracker_[pid] += FD_COUNT_SHARE_MEM;
        fdTotal_ += FD_COUNT_SHARE_MEM;
    } else if (allocType == Media::AllocatorType::DMA_ALLOC) {
        fdCountTracker_[pid] += FD_COUNT_DMA;
        fdTotal_ += FD_COUNT_DMA;
    }
}

void RSPixelMapFdTrack::RemoveFdRecord(int32_t pid, const void* addr, Media::AllocatorType allocType)
{
    if (pid <= INVALID_PID) {
        return;
    }
    if (allocType != Media::AllocatorType::SHARE_MEM_ALLOC && allocType != Media::AllocatorType::DMA_ALLOC) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = addrTracker_.find(pid);
    if (it == addrTracker_.end()) {
        return;
    }
    size_t eraseCount = it->second.erase(addr);
    if (eraseCount == 0) {
        return;
    }
    if (allocType == Media::AllocatorType::SHARE_MEM_ALLOC) {
        fdCountTracker_[pid] -= FD_COUNT_SHARE_MEM;
        fdTotal_ -= FD_COUNT_SHARE_MEM;
    } else if (allocType == Media::AllocatorType::DMA_ALLOC) {
        fdCountTracker_[pid] -= FD_COUNT_DMA;
        fdTotal_ -= FD_COUNT_DMA;
    }
    if (it->second.empty()) {
        addrTracker_.erase(it);
        fdCountTracker_.erase(pid);
    }
}

bool RSPixelMapFdTrack::CheckFdRecordAndKillProcess(int32_t pid)
{
    std::unordered_map<int32_t, int32_t> pidsToKill;
#ifdef RS_ENABLE_UNI_RENDER
    CheckFdRecordAndGetPidsToKill(pidsToKill, pid);
    for (const auto [ pidToKill, fdCount ] : pidsToKill) {
        ReportFdOverLimit(pidToKill);
        std::string reason = "fd count " + std::to_string(fdCount) + " exceeds limit";
        bool success = KillProcessByPid(pidToKill, reason);
        if (success) {
            ClearFdRecordByPid(pidToKill);
        }
    }
#endif
    return pidsToKill.empty();
}

void RSPixelMapFdTrack::ClearFdRecordByPid(int32_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    addrTracker_.erase(pid);
    auto it = fdCountTracker_.find(pid);
    if (it != fdCountTracker_.end()) {
        fdTotal_ -= it->second;
        fdCountTracker_.erase(it);
    }
}

void RSPixelMapFdTrack::CheckFdRecordAndGetPidsToKill(std::unordered_map<int32_t, int32_t>& pidsToKill,
    int32_t pid) const
{
    RS_TRACE_NAME("RSPixelMapFdTrack::CheckFdRecordAndGetPidsToKill");

    std::lock_guard<std::mutex> lock(mutex_);
    // step 1: If current pid exceeds fd limit, add current pid to the kill queue.
    int32_t fdCount = 0;
    if (!CheckFdCountByPid(pid, fdCount)) {
        pidsToKill.emplace(pid, fdCount);
    }
    // step2: If fd total exceeds limit, add top1 pid to the kill queue.
    int32_t topFdCountPid = INVALID_PID;
    int32_t topFdCount = 0;
    if (!CheckFdTotal(topFdCountPid, topFdCount)) {
        pidsToKill.emplace(topFdCountPid, topFdCount);
    }
}

bool RSPixelMapFdTrack::CheckFdCountByPid(int32_t pid, int32_t& fdCount) const
{
    auto it = fdCountTracker_.find(pid);
    if (it != fdCountTracker_.end()) {
        fdCount = it->second;
    }
    if (fdCount <= MAX_APP_FD) {
        return true;
    }
    ROSEN_LOGE("RSPixelMapFdTrack::CheckFdCountByPid failed, pid %{public}" PRId32 " fd count %{public}" PRId32
        " exceeds limit %{public}" PRId32, pid, fdCount, MAX_APP_FD);
    return false;
}

bool RSPixelMapFdTrack::CheckFdTotal(int32_t& topFdCountPid, int32_t& topFdCount) const
{
    if (fdTotal_ <= MAX_TOTAL_FD) {
        return true;
    }
    for (const auto& [ pid, fdCount ] : fdCountTracker_) {
        if (fdCount < topFdCount) {
            continue;
        }
        if (fdCount > topFdCount) {
            topFdCountPid = pid;
            topFdCount = fdCount;
            continue;
        }
        // fdCount == topFdCount
        if (pid > topFdCountPid) {
            // If multiple processes have the same fd count, kill the newer process.
            topFdCountPid = pid;
        }
    }
    ROSEN_LOGE("RSPixelMapFdTrack::CheckFdTotal failed, fd total %{public}" PRId32 " exceeds limit %{public}" PRId32
        ", top1 pid %{public}" PRId32 " fd count %{public}" PRId32, fdTotal_, MAX_TOTAL_FD, topFdCountPid, topFdCount);
    return false;
}

bool RSPixelMapFdTrack::KillProcessByPid(int32_t pid, const std::string& reason)
{
    RS_TRACE_NAME_FMT("RSPixelMapFdTrack::KillProcessByPid pid %" PRId32, pid);

#ifdef RS_ENABLE_UNI_RENDER
    AAFwk::ExitReasonCompability killReason{AAFwk::Reason::REASON_RESOURCE_CONTROL, KILL_PROCESS_TYPE, reason};
    killReason.killId = HiviewDFX::ProcessKillReason::KillEventId::REASON_FD_EXCEEDS_LIMIT;
    int32_t ret = AAFwk::AbilityManagerClient::GetInstance()->KillAppWithReason(pid, killReason);
    ROSEN_LOGE("RSPixelMapFdTrack::KillProcessByPid, pid = %{public}" PRId32 " ret = %{public}" PRId32, pid, ret);
    return ret == ERR_OK;
#else
    return false;
#endif
}

void RSPixelMapFdTrack::ReportFdOverLimit(int32_t pid)
{
    RS_TRACE_NAME_FMT("RSPixelMapFdTrack::ReportFdOverLimit pid %" PRId32, pid);

    std::string ashmemInfo;
    ReadAshmemInfoFromFile(ASHMEM_INFO_PATH, ashmemInfo, pid);

    std::string dmaBufInfo;
    ReadDmaBufInfoFromFile(DMABUF_INFO_PATH, dmaBufInfo, pid);

    std::string combinedInfo = "=== AshmemInfo ===\n" + ashmemInfo + "\n\n=== DmaBufInfo ===\n" + dmaBufInfo;
    WriteFdMemInfoToFile(RS_FD_MEM_PATH, combinedInfo);

#ifdef RS_ENABLE_UNI_RENDER
    RS_TRACE_NAME("RSPixelMapFdTrack::ReportFdOverLimit HiSysEventWrite");
    int ret = RSHiSysEvent::EventWrite(RSEventName::RENDER_MEMORY_OVER_WARNING, RSEventType::RS_STATISTIC,
        "PID", pid,
        "TYPE", "FD",
        "FILEPATH", RS_FD_MEM_PATH);
    ROSEN_LOGW("RSPixelMapFdTrack::ReportFdOverLimit HiSysEventWrite, pid = %{public}" PRId32 " ret = %{public}d",
        pid, ret);
#endif
}

void RSPixelMapFdTrack::WriteFdMemInfoToFile(const std::string& fdMemInfoPath, const std::string& fdMemInfo)
{
    RS_TRACE_NAME("RSPixelMapFdTrack::WriteFdMemInfoToFile");

    std::ofstream fdMemInfoFile(fdMemInfoPath);
    if (fdMemInfoFile.is_open()) {
        fdMemInfoFile << "\n******************************\n";
        fdMemInfoFile << fdMemInfo;
        fdMemInfoFile << "\n************ endl ************\n";
        fdMemInfoFile.close();
    } else {
        ROSEN_LOGE("RSPixelMapFdTrack::WriteFdMemInfoToFile can not open fdMemInfoFile");
    }
}

void RSPixelMapFdTrack::ReadAshmemInfoFromFile(const std::string& ashmemInfoPath, std::string& ashmemInfo, int32_t pid)
{
    RS_TRACE_NAME("RSPixelMapFdTrack::ReadAshmemInfoFromFile");

    std::ifstream ashmemInfoFile;
    ashmemInfoFile.open(ashmemInfoPath);
    if (ashmemInfoFile.is_open()) {
        std::stringstream ashmemInfoStream;
        ashmemInfoStream << ashmemInfoFile.rdbuf();
        FilterAshmemInfoByPid(ashmemInfo, ashmemInfoStream.str(), pid);
        ashmemInfoFile.close();
    } else {
        ashmemInfo = "Failed to open ashmemInfoFile: " + ashmemInfoPath;
        ROSEN_LOGE("RSPixelMapFdTrack::ReadAshmemInfoFromFile can not open ashmemInfoFile");
    }
}

void RSPixelMapFdTrack::ReadDmaBufInfoFromFile(const std::string& dmaBufInfoPath, std::string& dmaBufInfo, int32_t pid)
{
    RS_TRACE_NAME("RSPixelMapFdTrack::ReadDmaBufInfoFromFile");

    std::ifstream dmaBufInfoFile;
    dmaBufInfoFile.open(dmaBufInfoPath);
    if (dmaBufInfoFile.is_open()) {
        std::stringstream dmaBufInfoStream;
        dmaBufInfoStream << dmaBufInfoFile.rdbuf();
        FilterDmaBufInfoByPid(dmaBufInfo, dmaBufInfoStream.str(), pid);
        dmaBufInfoFile.close();
    } else {
        dmaBufInfo = "Failed to open dmaBufInfoFile: " + dmaBufInfoPath;
        ROSEN_LOGE("RSPixelMapFdTrack::ReadDmaBufInfoFromFile can not open dmaBufInfoFile");
    }
}

void RSPixelMapFdTrack::FilterAshmemInfoByPid(std::string& out, const std::string& info, int32_t pid)
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

void RSPixelMapFdTrack::FilterDmaBufInfoByPid(std::string& out, const std::string& info, int32_t pid)
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
} // namespace Rosen
} // namespace OHOS
