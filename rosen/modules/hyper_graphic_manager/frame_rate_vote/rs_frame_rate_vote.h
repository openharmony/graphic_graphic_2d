/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_FRAME_RATE_VOTE_H
#define RS_FRAME_RATE_VOTE_H

#include <iconsumer_surface.h>

#include "rs_video_frame_rate_vote.h"
#include "ffrt.h"
#include "hgm_frame_rate_manager.h"
#include "singleton.h"

namespace OHOS {
namespace Rosen {
class RSFrameRateVote {
    DECLARE_DELAYED_SINGLETON(RSFrameRateVote);

public:
    /**
     * @brief vote frame rate by video rate
     *
     * @param surfaceNodeId the surfaceNode unique id
     * @param sourceType the buffer queue type of the surfaceNode
     * @param buffer the buffer queue
     */
    void VideoFrameRateVote(uint64_t surfaceNodeId, OHSurfaceSource sourceType, sptr<SurfaceBuffer>& buffer);
    void SetTransactionFlags(const std::string& transactionFlags);
    void CheckSurfaceAndUi();

private:
    void ReleaseSurfaceMap(uint64_t surfaceNodeId);
    void SurfaceVideoVote(uint64_t surfaceNodeId, uint32_t rate);
    void VoteRate(pid_t pid, std::string eventName, uint32_t rate);
    void CancelVoteRate(pid_t pid, std::string eventName);
    void NotifyRefreshRateEvent(pid_t pid, EventInfo eventInfo);

private:
    bool isSwitchOn_{ false };
    pid_t lastVotedPid_{ DEFAULT_PID };
    std::atomic<uint32_t> lastVotedRate_{ OLED_NULL_HZ };
    bool hasUiOrSurface = true;
    bool isVoted_{ false };
    std::atomic<uint64_t> lastSurfaceNodeId_ {0};
    uint64_t currentUpdateTime_{ 0 };
    std::string transactionFlags_ = "";
    std::unordered_map<uint64_t, std::shared_ptr<RSVideoFrameRateVote>> surfaceVideoFrameRateVote_{};
    std::unordered_map<uint64_t, uint32_t> surfaceVideoRate_{};
    std::mutex mutex_;
    std::shared_ptr<ffrt::queue> ffrtQueue_{ nullptr };
    ffrt::mutex ffrtMutex_;
    ffrt::task_handle taskHandler_{ nullptr };
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_FRAME_RATE_VOTE_H