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
#include "memory/rs_memory_snapshot.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MEMUNIT_RATE = 1024;
constexpr int64_t MEMORY_SNAPSHOT_INTERVAL = 3 * 60 * 1000; // EachProcess can print at most once per 3 minute.
// Threshold for hilog in rs mem.
constexpr uint32_t MEMORY_SNAPSHOT_PRINT_HILOG_LIMIT = 1300 * MEMUNIT_RATE * MEMUNIT_RATE;
constexpr uint32_t HILOG_INFO_COUNT = 5; // The number of info printed each time.
constexpr float WEIGHT_CPU = 0.3; // Weight of CPU.
constexpr float WEIGHT_GPU = 0.3; // Weight of GPU.
constexpr float WEIGHT_SUM = 0.4; // Weight of CPU+GPU.
}

MemorySnapshot& MemorySnapshot::Instance()
{
    static MemorySnapshot instance;
    return instance;
}

void MemorySnapshot::AddCpuMemory(const pid_t pid, const size_t size)
{
    bool shouldReport = false;
    size_t cpuMemory = 0;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (appMemorySnapshots_.find(pid) == appMemorySnapshots_.end()) {
            dirtyMemorySnapshots_.push_back(pid);
        }

        MemorySnapshotInfo& mInfo = appMemorySnapshots_[pid];
        mInfo.pid = pid;
        mInfo.cpuMemory += size;
        totalMemory_ += size;
        if (mInfo.cpuMemory > singleCpuMemoryLimit_ && mInfo.cpuMemory - size < singleCpuMemoryLimit_) {
            shouldReport = true;
            cpuMemory = mInfo.cpuMemory;
        }
    }
    if (shouldReport && callback_) {
        callback_(pid, cpuMemory, false);
    }
}

void MemorySnapshot::RemoveCpuMemory(const pid_t pid, const size_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = appMemorySnapshots_.find(pid);
    if (it != appMemorySnapshots_.end()) {
        it->second.cpuMemory -= size;
        totalMemory_ -= size;
    }
}

bool MemorySnapshot::GetMemorySnapshotInfoByPid(const pid_t pid, MemorySnapshotInfo& info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = appMemorySnapshots_.find(pid);
    if (it == appMemorySnapshots_.end()) {
        return false;
    }
    info = it->second;
    return true;
}

void MemorySnapshot::UpdateGpuMemoryInfo(const std::unordered_map<pid_t, size_t>& gpuInfo,
    std::unordered_map<pid_t, MemorySnapshotInfo>& pidForReport, bool& isTotalOver)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [pid, info] : appMemorySnapshots_) {
        auto it = gpuInfo.find(pid);
        if (it != gpuInfo.end()) {
            totalMemory_ = totalMemory_ - info.gpuMemory + it->second;
            info.gpuMemory = it->second;
        }
        if (info.TotalMemory() > singleMemoryWarning_) {
            pidForReport.emplace(pid, info);
        }
    }
    if (totalMemory_ > totalMemoryLimit_) {
        pidForReport = appMemorySnapshots_;
        isTotalOver = true;
    }
    if (totalMemory_ > MEMORY_SNAPSHOT_PRINT_HILOG_LIMIT) {
        PrintMemorySnapshotToHilog();
    }
}

void MemorySnapshot::EraseSnapshotInfoByPid(const std::set<pid_t>& exitedPidSet)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto pid : exitedPidSet) {
        auto it = appMemorySnapshots_.find(pid);
        if (it != appMemorySnapshots_.end()) {
            totalMemory_ -= it->second.TotalMemory();
            appMemorySnapshots_.erase(it);
        }
    }
}

void MemorySnapshot::InitMemoryLimit(MemoryOverflowCalllback callback,
    uint64_t warning, uint64_t overflow, uint64_t totalSize)
{
    if (callback_ == nullptr) {
        callback_ = callback;
        singleMemoryWarning_ = warning;
        singleCpuMemoryLimit_ = overflow;
        totalMemoryLimit_ = totalSize;
    }
}

void MemorySnapshot::GetMemorySnapshot(std::unordered_map<pid_t, MemorySnapshotInfo>& map)
{
    std::lock_guard<std::mutex> lock(mutex_);
    map = appMemorySnapshots_;
}

void MemorySnapshot::GetDirtyMemorySnapshot(std::vector<pid_t>& list)
{
    std::lock_guard<std::mutex> lock(mutex_);
    list = dirtyMemorySnapshots_;
}

void MemorySnapshot::FillMemorySnapshot(std::unordered_map<pid_t, MemorySnapshotInfo>& infoMap)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto pPid = dirtyMemorySnapshots_.begin(); pPid != dirtyMemorySnapshots_.end();) {
        auto it = appMemorySnapshots_.find(*pPid);
        if (it != appMemorySnapshots_.end()) {
            it->second.bundleName = infoMap[it->first].bundleName;
        }
        pPid = dirtyMemorySnapshots_.erase(pPid);
    }
}

size_t MemorySnapshot::GetTotalMemory()
{
    return totalMemory_;
}

void MemorySnapshot::PrintMemorySnapshotToHilog()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    if (currentTime < memorySnapshotHilogTime_) {
        return;
    }

    std::vector<MemorySnapshotInfo> memorySnapshotsList;
    size_t maxCpu;
    size_t maxGpu;
    size_t maxSum;
    FindMaxValues(memorySnapshotsList, maxCpu, maxGpu, maxSum);

    // Sort by risk in descending order
    std::sort(memorySnapshotsList.begin(), memorySnapshotsList.end(),
        [=](const MemorySnapshotInfo& a, const MemorySnapshotInfo& b) {
            float scoreA = CalculateRiskScore(a, maxCpu, maxGpu, maxSum);
            float scoreB = CalculateRiskScore(b, maxCpu, maxGpu, maxSum);
            return scoreA > scoreB;
        });

    RS_LOGE("TotalMemoryOverReport. TotalMemory:[%{public}zuKB]", totalMemory_ / MEMUNIT_RATE);
    for (size_t i = 0 ; i < HILOG_INFO_COUNT && i < memorySnapshotsList.size() ; i++) {
        MemorySnapshotInfo info = memorySnapshotsList[i];
        RS_LOGE("pid : %{public}d %{public}s, cpu : %{public}zuKB, gpu : %{public}zuKB",
            static_cast<int32_t>(info.pid), info.bundleName.c_str(),
            info.cpuMemory / MEMUNIT_RATE, info.gpuMemory / MEMUNIT_RATE);
    }

    memorySnapshotHilogTime_ = currentTime + MEMORY_SNAPSHOT_INTERVAL;
}

void MemorySnapshot::FindMaxValues(std::vector<MemorySnapshotInfo>& memorySnapshotsList,
    size_t& maxCpu, size_t& maxGpu, size_t& maxSum)
{
    maxCpu = maxGpu = maxSum = 0;
    for (const auto& [pid, snapshotInfo] : appMemorySnapshots_) {
        memorySnapshotsList.push_back(snapshotInfo);

        if (snapshotInfo.cpuMemory > maxCpu) {
            maxCpu = snapshotInfo.cpuMemory;
        }

        if (snapshotInfo.gpuMemory > maxGpu) {
            maxGpu = snapshotInfo.gpuMemory;
        }

        size_t totalMemory = snapshotInfo.TotalMemory();
        if (totalMemory > maxSum) {
            maxSum = totalMemory;
        }
    }
}

float MemorySnapshot::CalculateRiskScore(const MemorySnapshotInfo snapshotInfo,
    size_t maxCpu, size_t maxGpu, size_t maxSum)
{
    float normCpu = (maxCpu == 0) ? 0 : static_cast<float>(snapshotInfo.cpuMemory) / maxCpu;
    float normGpu = (maxGpu == 0) ? 0 : static_cast<float>(snapshotInfo.gpuMemory) / maxGpu;
    float normSum = (maxSum == 0) ? 0 : static_cast<float>(snapshotInfo.TotalMemory()) / maxSum;
    return WEIGHT_CPU * normCpu + WEIGHT_GPU * normGpu + WEIGHT_SUM * normSum;
}
}
} // namespace OHOS::Rosen