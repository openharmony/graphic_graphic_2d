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

#include <chrono>

#include "rs_video_frame_rate_vote.h"
#include "ffrt_inner.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
    const static int64_t MILLI_SECOND = 1000;
    const static int64_t MICRO_SECOND = 1000 * 1000;
    const static uint32_t MAX_RATE = 1000;
    const static uint32_t NORMAL_RATE_MIN = 1;
    const static uint32_t NORMAL_RATE_MAX = 144;
    const static unsigned int VECTOR_SIZE = 30;
    const static unsigned int VECTOR_START = 0;
    const static unsigned int VECTOR_END = 29;
    constexpr int32_t DELAY_TIME = 1000 * 1000;
    const static int32_t MAX_VOTED_COUNT = 5;
    const static int64_t MAX_VOTED_TIME = 60 * 1000;
}

RSVideoFrameRateVote::RSVideoFrameRateVote(uint64_t surfaceNodeId,
    const std::function<void(uint64_t)>& releaseCallback,
    const std::function<void(uint64_t, uint32_t)>& voteCallback)
    : surfaceNodeId_(surfaceNodeId), releaseCallback_(releaseCallback), voteCallback_(voteCallback)
{
    std::string queueName = "video_frame_rate_vote_queue_" + std::to_string(surfaceNodeId);
    ffrtQueue_ = std::make_shared<ffrt::queue>(queueName.c_str());
}

RSVideoFrameRateVote::~RSVideoFrameRateVote()
{
    voteCallback_ = nullptr;
    releaseCallback_ = nullptr;
    taskHandler_ = nullptr;
    ffrtQueue_ = nullptr;
}

void RSVideoFrameRateVote::StartVideoFrameRateVote(int64_t timestamp)
{
    CancelDelayTask();
    if (((timestampType_ == MICRO_SECOND_TYPE) && (timestamp - lastTimestamp_ > MICRO_SECOND)) ||
        ((timestampType_ == MILLI_SECOND_TYPE) && (timestamp - lastTimestamp_ > MILLI_SECOND))) {
        videoTimestamp_.clear();
    }
    videoTimestamp_.emplace_back(timestamp);
    if (videoTimestamp_.size() >= VECTOR_SIZE) {
        uint32_t rate = CalculateVideoFrameRate();
        VoteVideoFrameRate(rate);
        videoTimestamp_.clear();
    }
    lastTimestamp_ = timestamp;
    SendDelayTask();
}

uint32_t RSVideoFrameRateVote::CalculateVideoFrameRate()
{
    uint32_t avgRate = 0;
    if (timestampType_ == MICRO_SECOND_TYPE) {
        avgRate = Calculate(MICRO_SECOND);
    }
    if (avgRate > MAX_RATE || timestampType_ == MILLI_SECOND_TYPE) {
        avgRate = Calculate(MILLI_SECOND);
        timestampType_ = MILLI_SECOND_TYPE;
    }
    RS_LOGI("video vote calculate rate surfaceNodeId(%{public}s), avgRate(%{public}u)",
        std::to_string(surfaceNodeId_).c_str(), avgRate);
    return avgRate;
}

uint32_t RSVideoFrameRateVote::Calculate(int64_t oneSecond)
{
    uint32_t avgRate = 0;
    int64_t timeDiff = videoTimestamp_[VECTOR_END] - videoTimestamp_[VECTOR_START];
    if (timeDiff > 0) {
        avgRate = static_cast<uint32_t>((oneSecond * VECTOR_END) / timeDiff);
    }
    return avgRate;
}

void RSVideoFrameRateVote::VoteVideoFrameRate(uint32_t rate)
{
    if (rate < NORMAL_RATE_MIN || rate > NORMAL_RATE_MAX) {
        RS_LOGE("video vote rate is valied surfaceNodeId(%{public}s)", std::to_string(surfaceNodeId_).c_str());
        return;
    }
    if (rate == lastRate_) {
        return;
    }
    int64_t currentVoteTimestamp = GetCurrentTimeMillis();
    if (lastVotedTimestamp_ != -1 && currentVoteTimestamp - lastVotedTimestamp_ <= MAX_VOTED_TIME &&
        votedCount_ >= MAX_VOTED_COUNT) {
        RS_LOGE("video vote too frequent surfaceNodeId(%{public}s)", std::to_string(surfaceNodeId_).c_str());
        return;
    }
    if (currentVoteTimestamp - lastVotedTimestamp_ <= MAX_VOTED_TIME) {
        votedCount_++;
    } else {
        votedCount_ = 0;
        lastVotedTimestamp_ = currentVoteTimestamp;
    }
    RS_LOGI("video vote surfaceNodeId(%{public}s) rate(%{public}u)", std::to_string(surfaceNodeId_).c_str(), rate);
    DoVoteCallback(rate);
    lastRate_ = rate;
}

void RSVideoFrameRateVote::SendDelayTask()
{
    std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
    auto initTask = [this]() {
        DoReleaseCallback();
    };
    ffrt::task_attr taskAttr;
    taskAttr.delay(DELAY_TIME);
    if (ffrtQueue_) {
        taskHandler_ = ffrtQueue_->submit_h(initTask, taskAttr);
    }
}

void RSVideoFrameRateVote::CancelDelayTask()
{
    std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
    if (taskHandler_) {
        ffrtQueue_->cancel(taskHandler_);
        taskHandler_ = nullptr;
    }
}

void RSVideoFrameRateVote::DoVoteCallback(uint32_t rate)
{
    if (voteCallback_ == nullptr) {
        return;
    }
    voteCallback_(surfaceNodeId_, rate);
}

void RSVideoFrameRateVote::DoReleaseCallback()
{
    if (releaseCallback_ == nullptr) {
        return;
    }
    releaseCallback_(surfaceNodeId_);
}

int64_t RSVideoFrameRateVote::GetCurrentTimeMillis()
{
    auto now = std::chrono::system_clock::now();
    auto milliSecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return milliSecs.count();
}
} // namespace Rosen
} // namespace OHOS