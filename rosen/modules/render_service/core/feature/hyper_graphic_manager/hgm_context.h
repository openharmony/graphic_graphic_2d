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
#ifndef HGM_CONTEXT_H
#define HGM_CONTEXT_H

#include "hgm_frame_rate_manager.h"
#include "rp_frame_rate_policy.h"
#include "vsync_distributor.h"

namespace OHOS {
namespace Rosen {

class HgmContext {
public:
    HgmContext();
    ~HgmContext() noexcept = default;

    void InitHgmTaskHandleThread(
        sptr<VSyncController> rsVSyncController, sptr<VSyncController> appVSyncController,
        sptr<VSyncGenerator> vsyncGenerator, sptr<VSyncDistributor> appVSyncDistributor);
    int32_t InitHgmConfig(std::vector<std::string>& appBufferList);
    void ProcessHgmFrameRate(uint64_t timestamp, sptr<VSyncDistributor> rsVSyncDistributor, uint64_t vsyncId);
    FrameRateRange& GetRSCurrRangeRef()
    {
        return rsCurrRange_;
    }

    std::shared_ptr<RSRenderFrameRateLinker> GetRSFrameRateLinker() const
    {
        return rsFrameRateLinker_;
    }

    const std::function<int32_t(RSPropertyUnit, float, int32_t, int32_t)>& GetConvertFrameRateFunc() const
    {
        return convertFrameRateFunc_;
    }

private:
    void InitHgmUpdateCallback();

    FrameRateRange rsCurrRange_;
    std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker_ = nullptr;
    uint64_t currVsyncId_ = 0;
    uint64_t lastForceUpdateVsyncId_ = UINT64_MAX;

    bool ltpoEnabled_ = false;
    bool isDelayMode_ = false;
    int32_t pipelineOffsetPulseNum_ = 0;

    bool rpHgmConfigDataChange_ = false;
    std::shared_ptr<RPHgmConfigData> rpHgmConfigData_ = nullptr;

    RPFrameRatePolicy rpFrameRatePolicy_;
    std::function<int32_t(RSPropertyUnit, float, int32_t, int32_t)> convertFrameRateFunc_ = nullptr;
};
} // namespace OHOS
} // namespace Rosen
#endif // HGM_CONTEXT_H