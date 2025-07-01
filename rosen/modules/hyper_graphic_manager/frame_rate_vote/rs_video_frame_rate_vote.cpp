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

#include "rs_video_frame_rate_vote.h"

#include <chrono>

#include "ffrt_inner.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t NORMAL_RATE_MIN = 1;
constexpr uint32_t NORMAL_RATE_MAX = 144;
constexpr int32_t DELAY_TIME = 1000 * 1000;
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

void RSVideoFrameRateVote::StartVideoFrameRateVote(double videoRate)
{
    CancelDelayTask();
    VoteVideoFrameRate(videoRate);
    SendDelayTask();
}

void RSVideoFrameRateVote::ReSetLastRate()
{
    lastRate_ = 0;
}

void RSVideoFrameRateVote::VoteVideoFrameRate(double videoRate)
{
    int32_t intRate = static_cast<int32_t>(videoRate);
    uint32_t rate = static_cast<uint32_t>(intRate);
    if (rate < NORMAL_RATE_MIN || rate > NORMAL_RATE_MAX) {
        return;
    }
    if (rate == lastRate_) {
        return;
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
    if (ffrtQueue_ && taskHandler_) {
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
} // namespace Rosen
} // namespace OHOS