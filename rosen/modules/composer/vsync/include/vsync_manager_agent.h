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

#ifndef RS_VSYNC_MANAGER_AGENT_H
#define RS_VSYNC_MANAGER_AGENT_H

#include "vsync_generator.h"
#include "vsync_distributor.h"

namespace OHOS {
namespace Rosen {
class RSVsyncManagerAgent : public RefBase {
public:
    RSVsyncManagerAgent(sptr<VSyncGenerator> vsyncGenerator, sptr<VSyncDistributor> rsVsyncDistributor);
    ~RSVsyncManagerAgent();
    void ForceRsDVsync(const std::string& sceneId);
    void DVSyncUpdate(uint64_t dvsyncTime, uint64_t vsyncTime);
    void SetTaskEndWithTime(uint64_t time);
    void NotifyPackageEvent(const std::vector<std::string>& packageList);
    void SetBufferInfo(uint64_t id, const std::string &name, uint32_t queueSize,
        int32_t bufferCount, int64_t lastConsumeTime, bool isUrgent);
    uint64_t GetRealTimeOffsetOfDvsync(int64_t time);
    void SetHardwareTaskNum(uint32_t num);
    uint64_t CheckVsyncReceivedAndGetRelTs(uint64_t timestamp);
    void PrintConnectionsStatus();
    void PrintGeneratorStatus();
    void SetFrameIsRender(bool isRender);
    void SetHasNativeBuffer();
    int64_t GetUiCommandDelayTime();
    void SetVSyncTimeUpdated();
    bool NeedUpdateVSyncTime(int32_t& pid);
    int64_t GetLastUpdateTime();
    bool IsUiDvsyncOn();
    bool GetWaitForDvsyncFrame();
    void SetWaitForDvsyncFrame(bool wait);

private:
    sptr<VSyncGenerator> vsyncGenerator_ = nullptr;
    sptr<VSyncDistributor> rsVsyncDistributor_ = nullptr;
    std::atomic<bool> waitForDVSyncFrame_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif