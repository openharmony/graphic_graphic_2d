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

void MemorySnapshot::AddCpuMemory(const pid_t pid, const size_t size, MemorySnapshotInfo& info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MemorySnapshotInfo& mInfo = appMemorySnapshots_[pid];
    mInfo.cpuMemory += size;
    info = mInfo;
}

void MemorySnapshot::RemoveCpuMemory(const pid_t pid, const size_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = appMemorySnapshots_.find(pid);
    if (it != appMemorySnapshots_.end()) {
        it->second.cpuMemory -= size;
    }
}

void MemorySnapshot::AddGpuMemory(const pid_t pid, const size_t size, MemorySnapshotInfo& info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MemorySnapshotInfo& mInfo = appMemorySnapshots_[pid];
    mInfo.gpuMemory += size;
    info = mInfo;
}

void MemorySnapshot::RemoveGpuMemory(const pid_t pid, const size_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = appMemorySnapshots_.find(pid);
    if (it != appMemorySnapshots_.end()) {
        it->second.gpuMemory -= size;
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

void MemorySnapshot::EraseSnapshotInfoByPid(const std::set<pid_t>& exitedPidSet)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto pid : exitedPidSet) {
        appMemorySnapshots_.erase(pid);
    }
}
}
} // namespace OHOS::Rosen