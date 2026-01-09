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

#include "rs_pipline_dump_manager.h"
#include "screen_manager/rs_screen_manager.h"
#include "vsync_controller.h"
#include "vsync_distributor.h"

namespace OHOS {
namespace Rosen {
class RSMainThread;

class RSPiplineDumper final {
public:
    RSPiplineDumper() {};
    explicit RSPiplineDumper(std::shared_ptr<AppExecFwk::EventHandler> mainHandler);
    ~RSPiplineDumper() {};

    RSPiplineDumper(const RSPiplineDumper&) = delete;
    RSPiplineDumper& operator=(const RSPiplineDumper&) = delete;

    void RpDumpInit(std::shared_ptr<RSPiplineDumpManager> rpDumpManager);

private:
    void DumpNodesNotOnTheTree(std::string& dumpString) const;
    void DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const;
    void DumpAllNodesMemSize(std::string& dumpString) const;
    void DumpRSEvenParam(std::string& dumpString) const;
    void DumpRenderServiceTree(std::string& dumpString, bool forceDumpSingleFrame = true) const;
    void DumpJankStatsRs(std::string& dumpString) const;
#ifdef RS_ENABLE_VK
    void DumpVkTextureLimit(std::string& dumpString) const;
#endif
    void DumpSurfaceNode(std::string& dumpString, NodeId id) const;
    void DumpExistPidMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const;
    uint32_t GenerateTaskId();

    // RS dump init
    void RegisterRSGfxFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager);
    void RegisterRSTreeFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager);
    void RegisterMemFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager);
    void RegisterGpuFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager);
    void RegisterBufferFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager);
    void RegisterSurfaceInfoFuncs(std::shared_ptr<RSPiplineDumpManager> rpDumpManager);

    void ScheduleTask(std::function<void()> task) const;
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
    sptr<RSScreenManager> screenManager_;
};
} // Rosen
} // OHOS

#endif // RENDER_SERVICE_RS_PIPLINE_DUMPER_H
