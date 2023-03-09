/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <vector>

#include "include/gpu/GrContext.h"
#include "memory/DfxString.h"
#include "memory/MemoryGraphic.h"
#include "memory/MemoryTrack.h"

namespace OHOS::Rosen {

class MemoryManager {
public:
    static void DumpMemoryUsage(DfxString& log, const GrContext* grContext, std::string& type);
    static void DumpPidMemory(DfxString& log, int pid);

    // Count memory for hidumper
    static MemoryGraphic CountPidMemory(int pid, const GrContext* grContext);
    static void CountMemory(std::vector<pid_t> pids, const GrContext* grContext , std::vector<MemoryGraphic>& mems);
private:
    // rs memory = rs + skia cpu + skia gpu
    static void DumpRenderServiceMemory(DfxString& log);
    static void DumpDrawingCpuMemory(DfxString& log);
    static void DumpDrawingGpuMemory(DfxString& log, const GrContext* grContext);

    //jemalloc info
    static void DumpMallocStat(std::string& log);
};
} // namespace OHOS::Rosen