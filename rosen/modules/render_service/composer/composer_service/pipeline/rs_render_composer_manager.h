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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_PIPELINE_RS_RENDER_COMPOSER_MANAGER_H
#define RENDER_SERVICE_COMPOSER_SERVICE_PIPELINE_RS_RENDER_COMPOSER_MANAGER_H

#include <mutex>
#include "rs_render_composer.h"
#include "rs_render_to_composer_connection.h"

namespace OHOS::Rosen {
class RSRenderComposerManager {
public:
    RSRenderComposerManager(std::shared_ptr<AppExecFwk::EventHandler>& handler,
        sptr<RSVsyncManagerAgent> rsVsyncManagerAgent);
    ~RSRenderComposerManager() = default;

    void SetComposerToRenderConnection(ScreenId screenId, sptr<RSIComposerToRenderConnection> conn);
    void OnScreenConnected(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    void OnScreenDisconnected(ScreenId screenId);
    void OnHwcRestored(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    void OnHwcDead(ScreenId screenId);
    sptr<RSRenderToComposerConnection> GetRSComposerConnection(ScreenId screenId);
    void PostTask(const std::function<void()>& task);
    void PostSyncTask(const std::function<void()>& task);
    void PostDelayTask(const std::function<void()>& task, int64_t delayTime);
    void SurfaceDump(std::string& dumpString);
    void FpsDump(std::string& dumpString, std::string& arg);
    void GetRefreshInfoToSP(std::string& dumpString, NodeId& nodeId);
    void ClearFpsDump(std::string& dumpString, std::string& arg);
    void HitchsDump(std::string& dumpString, std::string& arg);
    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);

private:
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposerAgent>> rsRenderComposerAgentMap_;
    std::unordered_map<ScreenId, sptr<RSRenderToComposerConnection>> rsComposerConnectionMap_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::mutex mutex_;
    sptr<RSVsyncManagerAgent> rsVsyncManagerAgent_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_SERVICE_PIPELINE_RS_RENDER_COMPOSER_MANAGER_H