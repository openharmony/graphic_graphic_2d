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

#include "memory/rs_memory_flow_control.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
MemoryFlowControl& MemoryFlowControl::Instance()
{
    static MemoryFlowControl instance;
    return instance;
}

bool MemoryFlowControl::AddAshmemStatistic(pid_t callingPid, uint32_t bufferSize)
{
    if (callingPid == 0) {
        // skip flow control when callingPid is meaningless
        return true;
    }
    if (bufferSize == 0) {
        return true;
    }
    if (bufferSize > ASHMEM_BUFFER_SIZE_UPPER_BOUND_FOR_EACH_PID) {
        ROSEN_LOGE("MemoryFlowControl::AddAshmemStatistic reject ashmem buffer size %{public}" PRIu32 " from pid "
            "%{public}d, ashmem is too large", bufferSize, static_cast<int>(callingPid));
        return false;
    }
    std::lock_guard<std::mutex> lock(pidToAshmemBufferSizeMapMutex_);
    auto [it, isNewElement] = pidToAshmemBufferSizeMap_.try_emplace(callingPid, bufferSize);
    if (isNewElement) {
        return true;
    }
    uint32_t availableSize = ASHMEM_BUFFER_SIZE_UPPER_BOUND_FOR_EACH_PID - it->second;
    if (bufferSize > availableSize) {
        ROSEN_LOGE("MemoryFlowControl::AddAshmemStatistic reject ashmem buffer size %{public}" PRIu32 " from pid "
            "%{public}d, available size %{public}" PRIu32, bufferSize, static_cast<int>(callingPid), availableSize);
        return false;
    }
    it->second += bufferSize;
    return true;
}

void MemoryFlowControl::RemoveAshmemStatistic(pid_t callingPid, uint32_t bufferSize)
{
    if (callingPid == 0) {
        // skip flow control when callingPid is meaningless
        return;
    }
    if (bufferSize == 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(pidToAshmemBufferSizeMapMutex_);
    auto it = pidToAshmemBufferSizeMap_.find(callingPid);
    if (it == pidToAshmemBufferSizeMap_.end()) {
        return;
    }
    if (bufferSize >= it->second) {
        pidToAshmemBufferSizeMap_.erase(it);
        return;
    }
    it->second -= bufferSize;
}

std::shared_ptr<AshmemFlowControlUnit> AshmemFlowControlUnit::CheckOverflowAndCreateInstance(pid_t pid, uint32_t size)
{
    bool success = MemoryFlowControl::Instance().AddAshmemStatistic(pid, size);
    if (!success) {
        return nullptr;
    }
    auto instance = std::make_shared<AshmemFlowControlUnit>(pid, size);
    instance->needStatistic_ = true;
    return instance;
}

AshmemFlowControlUnit::AshmemFlowControlUnit(pid_t pid, uint32_t size) : callingPid_(pid), bufferSize_(size) {}

AshmemFlowControlUnit::~AshmemFlowControlUnit()
{
    if (!needStatistic_) {
        return;
    }
    MemoryFlowControl::Instance().RemoveAshmemStatistic(callingPid_, bufferSize_);
}
} // namespace Rosen
} // namespace OHOS