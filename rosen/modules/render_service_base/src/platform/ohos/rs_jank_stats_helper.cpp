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

#include "platform/ohos/rs_jank_stats_helper.h"

namespace OHOS {
namespace Rosen {
RSJankStatsRenderFrameHelper& RSJankStatsRenderFrameHelper::GetInstance()
{
    static RSJankStatsRenderFrameHelper instance;
    return instance;
}

void RSJankStatsRenderFrameHelper::JankStatsStart()
{
    SetSkipJankAnimatorFrame(false);
}

void RSJankStatsRenderFrameHelper::JankStatsAfterSync(const std::unique_ptr<RSRenderThreadParams>& params,
    int accumulatedBufferCount)
{
    if (params == nullptr) {
        doJankStats_ = false;
        return;
    }
    doJankStats_ = params->IsUniRenderAndOnVsync();
    if (!doJankStats_) {
        return;
    }
    rsOnVsyncStartTime_ = params->GetOnVsyncStartTime();
    rsOnVsyncStartTimeSteady_ = params->GetOnVsyncStartTimeSteady();
    rsOnVsyncStartTimeSteadyFloat_ = params->GetOnVsyncStartTimeSteadyFloat();
    rsImplicitAnimationEnd_ = params->GetImplicitAnimationEnd();
    rsDiscardJankFrames_ = params->GetDiscardJankFrames();
    RSJankStats::GetInstance().SetStartTime();
    RSJankStats::GetInstance().SetAccumulatedBufferCount(accumulatedBufferCount);
}

void RSJankStatsRenderFrameHelper::JankStatsEnd(uint32_t dynamicRefreshRate)
{
    if (!doJankStats_) {
        SetDiscardJankFrames(false);
        return;
    }
    RSJankStats::GetInstance().SetOnVsyncStartTime(rsOnVsyncStartTime_, rsOnVsyncStartTimeSteady_,
        rsOnVsyncStartTimeSteadyFloat_);
    RSJankStats::GetInstance().SetImplicitAnimationEnd(rsImplicitAnimationEnd_);
    RSJankStats::GetInstance().SetEndTime(rtSkipJankAnimatorFrame_.load(),
        rtDiscardJankFrames_.load() || rsDiscardJankFrames_, dynamicRefreshRate);
    SetDiscardJankFrames(false);
}
} // namespace Rosen
} // namespace OHOS
