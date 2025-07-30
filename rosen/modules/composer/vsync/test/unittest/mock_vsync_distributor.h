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

#ifndef MOCK_VSYNC_VSYNC_DISTRIBUTOR_H
#define MOCK_VSYNC_VSYNC_DISTRIBUTOR_H

#include "vsync_distributor.h"
namespace OHOS {
namespace Rosen {

class MockVSyncDistributor : public VSyncDistributor {
public:
    MockVSyncDistributor(sptr<VSyncController> controller, std::string name, DVSyncFeatureParam dvsyncParam = {})
        : VSyncDistributor(controller, name, dvsyncParam) {}
    ~MockVSyncDistributor() {};
    // nocopyable
    MockVSyncDistributor(const MockVSyncDistributor &) = delete;
    MockVSyncDistributor &operator=(const MockVSyncDistributor &) = delete;

    bool NeedUpdateVSyncTime(int32_t& pid)
    {
        pid = mockPid_;
        return needUpdateVsyncTime_;
    }

    void DVSyncUpdate(uint64_t dvsyncTime, uint64_t vsyncTime)
    {
        dvsyncTime_ = dvsyncTime;
        vsyncTime_ = vsyncTime;
    }

    int64_t GetUiCommandDelayTime() { return delayTime_; }
    int64_t GetLastUpdateTime() { return lastTimeStamp_; }
    int32_t mockPid_ = 0;
    bool needUpdateVsyncTime_ = false;
    int64_t lastTimeStamp_ = 0;
    int64_t delayTime_ = 0;
    uint64_t dvsyncTime_ = 0;
    uint64_t vsyncTime_ = 0;
};
}
}

#endif