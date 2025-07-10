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
#ifndef RS_MEMORY_SNAPSHOT
#define RS_MEMORY_SNAPSHOT

#include <mutex>
#include <set>

#include "common/rs_common_def.h"
#include <vector>

namespace OHOS {
namespace Rosen {
struct MemorySnapshotInfo {
    pid_t pid = 0;
    int32_t uid;
    std::string bundleName = "";
    size_t cpuMemory = 0;
    size_t gpuMemory = 0;
    size_t subThreadGpuMemory = 0;

    size_t TotalMemory() const
    {
        return cpuMemory + gpuMemory + subThreadGpuMemory;
    }
};

using MemoryOverflowCalllback = std::function<void(pid_t, uint64_t, bool)>;
class RSB_EXPORT MemorySnapshot {
public:
    static MemorySnapshot& Instance();
    void AddCpuMemory(const pid_t pid, const size_t size);
    void RemoveCpuMemory(const pid_t pid, const size_t size);
    bool GetMemorySnapshotInfoByPid(const pid_t pid, MemorySnapshotInfo& info);
    void EraseSnapshotInfoByPid(const std::set<pid_t>& exitedPidSet);
    void UpdateGpuMemoryInfo(const std::unordered_map<pid_t, size_t>& uniRenderGpuInfo,
        const std::unordered_map<pid_t, size_t>& subThreadGpuInfo,
        std::unordered_map<pid_t, MemorySnapshotInfo>& pidForReport, bool& isTotalOver);
    void InitMemoryLimit(MemoryOverflowCalllback callback, uint64_t warning, uint64_t overflow, uint64_t totalSize);
    void GetMemorySnapshot(std::unordered_map<pid_t, MemorySnapshotInfo>& map);
    void GetDirtyMemorySnapshot(std::vector<pid_t>& list);
    void FillMemorySnapshot(std::unordered_map<pid_t, MemorySnapshotInfo>& infoMap);
    size_t GetTotalMemory();
    void PrintMemorySnapshotToHilog();
    void SetMemSnapshotPrintHilogLimit(int memSnapshotPrintHilogLimit);
    int GetMemSnapshotPrintHilogLimit();
private:
    MemorySnapshot() = default;
    ~MemorySnapshot() = default;
    MemorySnapshot(const MemorySnapshot&) = delete;
    MemorySnapshot(const MemorySnapshot&&) = delete;
    MemorySnapshot& operator=(const MemorySnapshot&) = delete;
    MemorySnapshot& operator=(const MemorySnapshot&&) = delete;
    
    void FindMaxValues(std::vector<MemorySnapshotInfo>& memorySnapshotsList,
        size_t& maxCpu, size_t& maxGpu, size_t& maxSum);
    float CalculateRiskScore(const MemorySnapshotInfo memorySnapshotInfo, size_t maxCpu, size_t maxGpu, size_t maxSum);

    std::mutex mutex_;
    std::unordered_map<pid_t, MemorySnapshotInfo> appMemorySnapshots_;
    std::vector<pid_t> dirtyMemorySnapshots_;

    uint64_t singleMemoryWarning_ = UINT64_MAX; // warning threshold for total memory of a single process
    uint64_t singleCpuMemoryLimit_ = UINT64_MAX; // error threshold for cpu memory of a single process
    uint64_t totalMemoryLimit_ = UINT64_MAX; // error threshold for total memory of all process
    size_t totalMemory_ = 0; // record the total memory of all processes
    MemoryOverflowCalllback callback_ = nullptr;
    int64_t memorySnapshotHilogTime_ = 0;
    int memSnapshotPrintHilogLimit_ = 0;
};
} // namespace OHOS
} // namespace Rosen
#endif