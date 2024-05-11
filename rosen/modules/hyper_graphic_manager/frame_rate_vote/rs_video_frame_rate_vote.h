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

#ifndef RS_VIDEO_FRAME_RATE_VOTE_H
#define RS_VIDEO_FRAME_RATE_VOTE_H

#include "ffrt.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"

namespace OHOS {
namespace Rosen {
class RSVideoFrameRateVote {
public:
    RSVideoFrameRateVote(uint64_t surfaceNodeId, const std::function<void(uint64_t)>& releaseCallback,
        const std::function<void(uint64_t, uint32_t)>& voteCallback);
    ~RSVideoFrameRateVote();

    /**
     * @brief vote frame rate by video rate
     *
     * @param timestamp the timestamp of video
     */
    void StartVideoFrameRateVote(int64_t timestamp);

private:
    uint32_t CalculateVideoFrameRate();
    uint32_t Calculate(int64_t oneSecond);
    void VoteVideoFrameRate(uint32_t rate);
    void SendDelayTask();
    void CancelDelayTask();
    void DoVoteCallback(uint32_t rate);
    void DoReleaseCallback();
    int64_t GetCurrentTimeMillis();

private:
    enum TimestampType: int32_t {
        MICRO_SECOND_TYPE,
        MILLI_SECOND_TYPE,
    };

private:
    uint64_t surfaceNodeId_ {0};
    std::function<void(uint64_t)> releaseCallback_ {nullptr};
    std::function<void(uint64_t, uint32_t)> voteCallback_ {nullptr};
    int64_t lastTimestamp_ {-1};
    uint32_t lastRate_ {0};
    std::vector<int64_t> videoTimestamp_ {};
    TimestampType timestampType_ {MICRO_SECOND_TYPE};
    int64_t lastVotedTimestamp_ {-1};
    int32_t votedCount_ {0};
    std::shared_ptr<ffrt::queue> ffrtQueue_ {nullptr};
    ffrt::mutex ffrtMutex_;
    ffrt::task_handle taskHandler_ {nullptr};
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_VIDEO_FRAME_RATE_VOTE_H