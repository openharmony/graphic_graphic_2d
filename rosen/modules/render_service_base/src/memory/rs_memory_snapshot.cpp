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

namespace OHOS {
namespace Rosen {

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
        MemorySnapshotInfo& mInfo = appMemorySnapshots_[pid];
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
}
} // namespace OHOS::Rosen