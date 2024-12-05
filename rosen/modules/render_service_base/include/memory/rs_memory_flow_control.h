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

#ifndef RS_MEMORY_FLOW_CONTROL_H
#define RS_MEMORY_FLOW_CONTROL_H

#include <mutex>
#include <unistd.h>
#include <unordered_map>

#include "common/rs_macros.h"
#include "nocopyable.h"

namespace OHOS {
namespace Rosen {
struct AshmemFlowControlUnit {
    const pid_t callingPid;
    uint32_t bufferSize = 0;
    bool inProgress = false;

    AshmemFlowControlUnit(pid_t pid, uint32_t size = 0) : callingPid(pid), bufferSize(size) {}
};

class RSB_EXPORT MemoryFlowControl {
public:
    static MemoryFlowControl& Instance();
    static constexpr uint32_t GetAshmemFlowControlThreshold()
    {
        return ASHMEM_BUFFER_SIZE_UPPER_BOUND_FOR_EACH_PID;
    }
    bool AddAshmemStatistic(pid_t callingPid, uint32_t bufferSize);
    bool AddAshmemStatistic(std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit);
    void RemoveAshmemStatistic(pid_t callingPid, uint32_t bufferSize);
    void RemoveAshmemStatistic(std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit);

private:
    MemoryFlowControl() = default;
    ~MemoryFlowControl() = default;
    DISALLOW_COPY_AND_MOVE(MemoryFlowControl);

    // handle exceptions when a callingPid is submitting >1GB ashmem parcel data to RS simultaneously
    static constexpr uint32_t ASHMEM_BUFFER_SIZE_UPPER_BOUND_FOR_EACH_PID = 1024 * 1024 * 1024;
    std::unordered_map<pid_t, uint32_t> pidToAshmemBufferSizeMap_;
    std::mutex pidToAshmemBufferSizeMapMutex_;
};
} // namespace OHOS
} // namespace Rosen

#endif // RS_MEMORY_FLOW_CONTROL_H