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

#ifndef ROSEN_JANK_STATS_HELPER_H
#define ROSEN_JANK_STATS_HELPER_H

#include <atomic>
#include <unordered_map>

#include "params/rs_render_thread_params.h"
#include "platform/ohos/rs_jank_stats.h"
#include "nocopyable.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSJankStatsRenderFrameHelper {
public:
    static RSJankStatsRenderFrameHelper& GetInstance();

    void JankStatsStart();
    void JankStatsAfterSync(const std::unique_ptr<RSRenderThreadParams>& params, int accumulatedBufferCount);
    void JankStatsEnd(uint32_t dynamicRefreshRate);

    void SetSkipJankAnimatorFrame(ScreenId screenId, bool skipJankAnimatorFrame)
    {
        rtSkipJankAnimatorFrameMap_[screenId] = skipJankAnimatorFrame;
    }
    void SetDiscardJankFrames(bool discardJankFrames)
    {
        rtDiscardJankFrames_.store(discardJankFrames);
    }

private:
    RSJankStatsRenderFrameHelper() = default;
    ~RSJankStatsRenderFrameHelper() = default;
    DISALLOW_COPY_AND_MOVE(RSJankStatsRenderFrameHelper);

    bool IsAllScreensSkipJankAnimatorFrame() const;

    bool doJankStats_ = true;

    // main thread params
    int64_t rsOnVsyncStartTime_ = TIMESTAMP_INITIAL;
    int64_t rsOnVsyncStartTimeSteady_ = TIMESTAMP_INITIAL;
    float rsOnVsyncStartTimeSteadyFloat_ = TIMESTAMP_INITIAL;
    bool rsImplicitAnimationEnd_ = false;
    bool rsDiscardJankFrames_ = false;

    // unirender thread params
    std::atomic_bool rtDiscardJankFrames_ = false;
    // render-thread-only; all access is single-threaded via JankStatsStart/End
    // per-screen skip jank animator frame state, cleared per frame by JankStatsStart
    std::unordered_map<ScreenId, bool> rtSkipJankAnimatorFrameMap_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_JANK_STATS_HELPER_H
