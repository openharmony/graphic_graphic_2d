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
#ifndef RENDER_SERVICE_DFX_RS_DUMP_SERVICE_MANAGER_H
#define RENDER_SERVICE_DFX_RS_DUMP_SERVICE_MANAGER_H

#include <list>
#include <mutex>

#include "gfx/dump/rs_dump_manager.h"
#include "ipc_callbacks/dfx/rs_dump_callback_stub.h"
#include "render_server/rs_render_process_manager.h"

namespace OHOS::Rosen {
class RSRenderComposerManager;
class RSServiceDumper;

const std::list<std::u16string> serviceDumpList = {
    u"screen",
    u"surface",
    u"fps",
    u"fpsClear",
    u"fpsCount",
    u"clearFpsCount",
    u"hitchs",
    u"gles",
    u"rsLogFlag",
    u"allInfo",
};

const std::list<std::u16string> processDumpList = {
    u"nodeNotOnTree",
    u"surface",
    u"allSurfacesMem",
    u"RSTree",
    u"MultiRSTrees",
    u"EventParamList",
    u"dumpMem",
    u"surfacenode",
    u"flushJankStatsRs",
    u"client",
    u"rsLogFlag",
#ifdef RS_ENABLE_VK
    u"vktextureLimit",
#endif
    u"dumpExistPidMem",
    u"buffer",
    u"allInfo",
};

class RSDumpCallbackDirector;
class RSB_EXPORT RSServiceDumpManager : public RSDumpManager {
public:
    RSServiceDumpManager() {};
    ~RSServiceDumpManager() = default;

    static bool IsServiceDumpCmd(const std::u16string& cmd);
    static bool IsProcessDumpCmd(const std::u16string& cmd);

    void InitProcessDumpTask(int32_t processCount);
    void WaitForDump(std::string& dumpString);
    void CollectDump(std::string& dumpString);
    void DoDump(const std::vector<std::u16string>& args, std::string& dumpString,
        const sptr<RSRenderProcessManager> processManager);

private:
    std::condition_variable processDumpCondVar_;
    std::mutex processDumpMutex_;
    std::mutex collectDumpMutex_;
    int32_t completionCount_;
    int32_t processCount_;
    std::vector<std::string> dumpDataList_;
    sptr<RSRenderProcessManager> rsDumpCallbackDirector_ = nullptr;
    bool IsDumpCompleted();
};

class RSDumpCallbackDirector : public RSDumpCallbackStub
{
public:
    explicit RSDumpCallbackDirector(RSServiceDumpManager* rsDumpManger) : rsDumpManger_(rsDumpManger) {}
    ~RSDumpCallbackDirector() override {}
    void OnDumpResult(std::string& dumpString) override
    {
        rsDumpManger_->CollectDump(dumpString);
    };

private:
    RSServiceDumpManager* rsDumpManger_;
};
}
#endif // RENDER_SERVICE_DFX_RS_DUMP_SERVICE_MANAGER_H