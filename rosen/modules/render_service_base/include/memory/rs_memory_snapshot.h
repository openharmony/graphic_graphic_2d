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

namespace OHOS {
namespace Rosen {
struct MemorySnapshotInfo {
    size_t cpuMemory = 0;
    size_t gpuMemory = 0;

    size_t TotalMemory() const
    {
        return cpuMemory + gpuMemory;
    }
};

class RSB_EXPORT MemorySnapshot {
public:
    static MemorySnapshot& Instance();
    void AddCpuMemory(const pid_t pid, const size_t size);
    void RemoveCpuMemory(const pid_t pid, const size_t size);
    bool GetMemorySnapshotInfoByPid(const pid_t pid, MemorySnapshotInfo& info);
    void EraseSnapshotInfoByPid(const std::set<pid_t>& exitedPidSet);
    void UpdateGpuMemoryInfo(const std::unordered_map<pid_t, size_t>& gpuInfo,
        std::unordered_map<pid_t, MemorySnapshotInfo>& pidForReport);
private:
    MemorySnapshot() = default;
    ~MemorySnapshot() = default;
    MemorySnapshot(const MemorySnapshot&) = delete;
    MemorySnapshot(const MemorySnapshot&&) = delete;
    MemorySnapshot& operator=(const MemorySnapshot&) = delete;
    MemorySnapshot& operator=(const MemorySnapshot&&) = delete;
    std::mutex mutex_;
    std::unordered_map<pid_t, MemorySnapshotInfo> appMemorySnapshots_;
};
} // namespace OHOS
} // namespace Rosen
#endif