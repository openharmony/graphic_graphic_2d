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

#include "rs_frame_rate_vote.h"
#include "hgm_core.h"
#include "platform/common/rs_log.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
    const std::string VIDEO_VOTE_FLAG = "VOTER_VIDEO";
}

RSFrameRateVote::RSFrameRateVote()
{
    frameRateMgr_ = OHOS::Rosen::HgmCore::Instance().GetFrameRateMgr();
    ffrtQueue_ = std::make_shared<ffrt::queue>("frame_rate_vote_queue");
    auto policyConfigData = OHOS::Rosen::HgmCore::Instance().GetPolicyConfigData();
    if (policyConfigData != nullptr) {
        isSwitchOn_ = policyConfigData->videoFrameRateVoteSwitch_;
        RS_LOGI("video vote feature isSwitchOn:%{public}s", isSwitchOn_ ? "true" : "false");
    }
}

RSFrameRateVote::~RSFrameRateVote()
{
    frameRateMgr_ = nullptr;
    ffrtQueue_ = nullptr;
}

void RSFrameRateVote::VideoFrameRateVote(uint64_t surfaceNodeId, OHSurfaceSource sourceType, int64_t timestamp)
{
    if (!isSwitchOn_ || sourceType != OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO || timestamp == 0) {
        return;
    }
    std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
    auto initTask = [this, surfaceNodeId, timestamp]() {
        std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote;
        if (surfaceVideoFrameRateVote_.find(surfaceNodeId) == surfaceVideoFrameRateVote_.end()) {
            rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(surfaceNodeId,
                std::bind(&RSFrameRateVote::ReleaseSurfaceMap, this, std::placeholders::_1),
                std::bind(&RSFrameRateVote::SurfaceVideoVote, this, std::placeholders::_1, std::placeholders::_2));
            surfaceVideoFrameRateVote_.insert(std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(
                surfaceNodeId, rsVideoFrameRateVote));
        } else {
            rsVideoFrameRateVote = surfaceVideoFrameRateVote_[surfaceNodeId];
        }
        rsVideoFrameRateVote->StartVideoFrameRateVote(timestamp);
    };
    if (ffrtQueue_) {
        ffrtQueue_->submit(initTask);
    }
}

void RSFrameRateVote::ReleaseSurfaceMap(uint64_t surfaceNodeId)
{
    std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
    auto initTask = [this, surfaceNodeId]() {
        SurfaceVideoVote(surfaceNodeId, 0);
        auto it = surfaceVideoFrameRateVote_.find(surfaceNodeId);
        if (it != surfaceVideoFrameRateVote_.end()) {
            RS_LOGI("video vote release surfaceNodeId(%{public}s), size(%{public}d)",
                std::to_string(surfaceNodeId).c_str(), static_cast<int>(surfaceVideoFrameRateVote_.size()));
            surfaceVideoFrameRateVote_.erase(it);
        }
    };
    if (ffrtQueue_) {
        ffrtQueue_->submit(initTask);
    }
}

void RSFrameRateVote::SurfaceVideoVote(uint64_t surfaceNodeId, uint32_t rate)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rate <= 0) {
        auto it = surfaceVideoRate_.find(surfaceNodeId);
        if (it != surfaceVideoRate_.end()) {
            surfaceVideoRate_.erase(it);
        }
    } else {
        surfaceVideoRate_[surfaceNodeId] = rate;
    }
    if (surfaceVideoRate_.size() <= 0) {
        CancelVoteRate(VIDEO_VOTE_FLAG);
        lastVotedRate_ = 0;
        return;
    }
    uint32_t maxRate = 0;
    for (const auto& item : surfaceVideoRate_) {
        if (item.second > maxRate) {
            maxRate = item.second;
        }
    }
    if (maxRate == lastVotedRate_) {
        return;
    }
    VoteRate(VIDEO_VOTE_FLAG, maxRate);
    lastVotedRate_ = maxRate;
}

void RSFrameRateVote::VoteRate(std::string eventName, uint32_t rate)
{
    isVoted_ = true;
    EventInfo eventInfo = {
        .eventName = eventName,
        .eventStatus = true,
        .minRefreshRate = rate,
        .maxRefreshRate = rate,
    };
    NotifyRefreshRateEvent(eventInfo);
}

void RSFrameRateVote::CancelVoteRate(std::string eventName)
{
    if (!isVoted_) {
        return;
    }
    isVoted_ = false;
    EventInfo eventInfo = {
        .eventName = eventName,
        .eventStatus = false,
    };
    NotifyRefreshRateEvent(eventInfo);
}

void RSFrameRateVote::NotifyRefreshRateEvent(EventInfo eventInfo)
{
    if (frameRateMgr_) {
        RS_LOGI("video vote rate(%{public}u, %{public}u)", eventInfo.minRefreshRate, eventInfo.maxRefreshRate);
        frameRateMgr_->HandleRefreshRateEvent(GetRealPid(), eventInfo);
    }
}
} // namespace Rosen
} // namespace OHOS