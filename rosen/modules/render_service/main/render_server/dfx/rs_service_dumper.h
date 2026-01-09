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

#ifndef RENDER_SERVICE_RS_SERVICE_DUMPER_H
#define RENDER_SERVICE_RS_SERVICE_DUMPER_H

#include <map>
#include <unordered_set>

#include "screen_manager/rs_screen_manager.h"
#include "render_server/transaction/zidl/rs_render_service_stub.h"
#include "rs_service_dump_manager.h"
#include "vsync_controller.h"
#include "vsync_distributor.h"

namespace OHOS {
namespace Rosen {
class RSMainThread;
class RSRenderComposerManager;

class RSServiceDumper final {
public:
    RSServiceDumper() {};
    explicit RSServiceDumper(std::shared_ptr<AppExecFwk::EventHandler> mainHandler,
        sptr<RSScreenManager> screenManager, std::shared_ptr<RSRenderComposerManager> rsRenderComposerManager);
    ~RSServiceDumper() {};

    RSServiceDumper(const RSServiceDumper&) = delete;
    RSServiceDumper& operator=(const RSServiceDumper&) = delete;

    void RsDumpInit(std::shared_ptr<RSServiceDumpManager> rsDumpManager);

private:
    void DumpAllNodesMemSize(std::string& dumpString) const;
    void DumpGpuInfo(std::string& dumpString) const;
    void DumpRefreshRateCounts(std::string& dumpString) const;
    void DumpClearRefreshRateCounts(std::string& dumpString) const;
    void WindowHitchsDump(std::unordered_set<std::u16string>& argSets, std::string& dumpString,
        const std::u16string& arg) const;
    void FPSDumpProcess(std::unordered_set<std::u16string>& argSets, std::string& dumpString,
        const std::u16string& arg) const;
    void DumpFps(std::string& dumpString, std::string& layerName) const;
    void FPSDumpClearProcess(std::unordered_set<std::u16string>& argSets,
        std::string& dumpString, const std::u16string& arg) const;
    void ClearFps(std::string& dumpString, std::string& layerName) const;

    // RS dump init
    void RegisterRSGfxFuncs(std::shared_ptr<RSServiceDumpManager> rsDumpManager);
    void RegisterMemFuncs(std::shared_ptr<RSServiceDumpManager> rsDumpManager);
    void RegisterFpsFuncs(std::shared_ptr<RSServiceDumpManager> rsDumpManager);
    void RegisterGpuFuncs(std::shared_ptr<RSServiceDumpManager> rsDumpManager);

    void ScheduleTask(std::function<void()> task) const;

    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
    sptr<RSScreenManager> screenManager_;
    std::shared_ptr<RSRenderComposerManager> rsRenderComposerManager_;
};
} // Rosen
} // OHOS

#endif // RENDER_SERVICE_RS_SERVICE_DUMPER_H
