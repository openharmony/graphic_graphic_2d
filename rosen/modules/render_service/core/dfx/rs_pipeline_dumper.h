/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_RS_PIPLINE_DUMPER_H
#define RENDER_SERVICE_RS_PIPLINE_DUMPER_H
#include <unordered_set>

#include <event_handler.h>
#include "rs_pipeline_dump_manager.h"
#include "vsync_controller.h"
#include "vsync_distributor.h"

namespace OHOS {
namespace Rosen {
class RSMainThread;

class RSPipelineDumper final {
public:
    RSPipelineDumper() {};
    explicit RSPipelineDumper(std::shared_ptr<AppExecFwk::EventHandler> mainHandler);
    ~RSPipelineDumper() {};

    RSPipelineDumper(const RSPipelineDumper&) = delete;
    RSPipelineDumper& operator=(const RSPipelineDumper&) = delete;

    void RenderPipelineDumpInit(std::shared_ptr<RSPipelineDumpManager> rpDumpManager);

private:
    void DumpNodesNotOnTheTree(std::string& dumpString) const;
    void DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString,
        bool isLite = false) const;
    void DumpGpuMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const;
    void DumpAllNodesMemSize(std::string& dumpString) const;
    void DumpRSEvenParam(std::string& dumpString) const;
    void DumpRenderServiceTree(std::string& dumpString, bool forceDumpSingleFrame = true) const;
    void DumpJankStatsRs(std::string& dumpString) const;
    void WindowHitchsDump(std::unordered_set<std::u16string>& argSets, std::string& dumpString,
        const std::u16string& arg) const;
    void FPSDumpProcess(std::unordered_set<std::u16string>& argSets, std::string& dumpString,
        const std::u16string& arg) const;
    void DumpFps(std::string& dumpString, std::string& layerName) const;
    void FPSDumpClearProcess(std::unordered_set<std::u16string>& argSets,
        std::string& dumpString, const std::u16string& arg) const;
    void ClearFps(std::string& dumpString, std::string& layerName) const;
#ifdef RS_ENABLE_VK
    void DumpVkTextureLimit(std::string& dumpString) const;
#endif
    void DumpSurfaceNode(std::string& dumpString, NodeId id) const;
    void DumpExistPidMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const;
    uint32_t GenerateTaskId();

    // RS dump init
    void RegisterRSGfxFuncs(std::shared_ptr<RSPipelineDumpManager> rpDumpManager);
    void RegisterRSTreeFuncs(std::shared_ptr<RSPipelineDumpManager> rpDumpManager);
    void RegisterMemFuncs(std::shared_ptr<RSPipelineDumpManager> rpDumpManager);
    void RegisterGpuFuncs(std::shared_ptr<RSPipelineDumpManager> rpDumpManager);
    void RegisterBufferFuncs(std::shared_ptr<RSPipelineDumpManager> rpDumpManager);
    void RegisterSurfaceInfoFuncs(std::shared_ptr<RSPipelineDumpManager> rpDumpManager);
    void RegisterFpsFuncs(std::shared_ptr<RSPipelineDumpManager> rpDumpManager);

    void ScheduleTask(std::function<void()> task) const;
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
};
} // Rosen
} // OHOS

#endif // RENDER_SERVICE_RS_PIPLINE_DUMPER_H
