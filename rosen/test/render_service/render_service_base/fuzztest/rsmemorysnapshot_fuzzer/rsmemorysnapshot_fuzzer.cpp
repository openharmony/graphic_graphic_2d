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

#include "rsmemorysnapshot_fuzzer.h"

#include <securec.h>
#include <unordered_map>

#include "memory/rs_memory_snapshot.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool RSMemorySnapshotFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto& instance = MemorySnapshot::Instance();
    pid_t tempPid = GetData<pid_t>();
    size_t tempSize = GetData<size_t>();
    instance.InitMemoryLimit(nullptr, GetData<uint64_t>(), GetData<uint64_t>(), GetData<uint64_t>());
    instance.AddCpuMemory(tempPid, tempSize);
    
    MemorySnapshotInfo info;
    instance.GetMemorySnapshotInfoByPid(GetData<pid_t>(), info);
    instance.GetMemorySnapshotInfoByPid(tempPid, info);
    std::unordered_map<pid_t, size_t> gpuInfo;
    gpuInfo.emplace(GetData<pid_t>(), GetData<size_t>());
    gpuInfo.emplace(tempPid, GetData<size_t>());
    std::unordered_map<pid_t, size_t> subThreadGpuInfo;
    subThreadGpuInfo.emplace(GetData<pid_t>(), GetData<size_t>());
    subThreadGpuInfo.emplace(tempPid, GetData<size_t>());
    std::unordered_map<pid_t, MemorySnapshotInfo> pidInfo;
    bool isTotalOver = GetData<bool>();
    instance.UpdateGpuMemoryInfo(gpuInfo, subThreadGpuInfo, pidInfo, isTotalOver);
    instance.GetMemorySnapshot(pidInfo);
    
    instance.RemoveCpuMemory(GetData<pid_t>(), GetData<size_t>());
    instance.RemoveCpuMemory(tempPid, tempSize);
    std::set<pid_t> eraseSet;
    eraseSet.emplace(tempPid);
    eraseSet.emplace(GetData<pid_t>());
    instance.EraseSnapshotInfoByPid(eraseSet);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSMemorySnapshotFuzzTest(data, size);
    return 0;
}
