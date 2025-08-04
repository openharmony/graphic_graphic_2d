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

#include <algorithm>

#include "hgm_core.h"
#include "platform/common/rs_log.h"
#include "rs_video_frame_rate_vote.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string VIDEO_RATE_FLAG = "VIDEO_RATE";
const std::string VIDEO_VOTE_FLAG = "VOTER_VIDEO";
// USE DURATION TO DETERMINE BARRAGE AND UI
constexpr uint64_t DANMU_MAX_INTERVAL_TIME = 50;
constexpr int32_t VIDEO_VOTE_DELAYS_TIME = 1000 * 1000;
}

RSFrameRateVote::RSFrameRateVote()
{
    ffrtQueue_ = std::make_shared<ffrt::queue>("frame_rate_vote_queue");
    auto policyConfigData = OHOS::Rosen::HgmCore::Instance().GetPolicyConfigData();
    if (policyConfigData != nullptr) {
        isSwitchOn_ = policyConfigData->videoFrameRateVoteSwitch_;
        RS_LOGI("video vote feature isSwitchOn:%{public}s", isSwitchOn_ ? "true" : "false");
    }
}

RSFrameRateVote::~RSFrameRateVote()
{
    ffrtQueue_ = nullptr;
}

void RSFrameRateVote::SetTransactionFlags(const std::string& transactionFlags)
{
    std::lock_guard<std::mutex> lock(mutex_);
    transactionFlags_ = transactionFlags;
}

void RSFrameRateVote::CheckSurfaceAndUi()
{
    if (!hasUiOrSurface) {
        return;
    }
    hasUiOrSurface = false;
    auto lastUpdateTime = currentUpdateTime_;
    currentUpdateTime_ = OHOS::Rosen::HgmCore::Instance().GetActualTimestamp() / NS_PER_MS;
    auto duration = currentUpdateTime_ > lastUpdateTime ? currentUpdateTime_ - lastUpdateTime : 0;
    if (duration < DANMU_MAX_INTERVAL_TIME) {
        if (lastVotedRate_ == OLED_NULL_HZ) {
            return;
        }
        uint64_t currentId = lastSurfaceNodeId_.load();
        {
            std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
            auto votingAddress = surfaceVideoFrameRateVote_.find(currentId);
            if (votingAddress != surfaceVideoFrameRateVote_.end() && votingAddress->second) {
                votingAddress->second->ReSetLastRate();
            }
        }
        SurfaceVideoVote(currentId, 0);
    }
}

void RSFrameRateVote::VideoFrameRateVote(uint64_t surfaceNodeId, OHSurfaceSource sourceType,
    sptr<SurfaceBuffer>& buffer)
{
    // transactionFlags_ format is [pid, eventId]
    std::string transactionFlags = "";
    std::string strLastVotedPid;
    pid_t lastVotedPid;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        transactionFlags = transactionFlags_;
        strLastVotedPid = "[" + std::to_string(lastVotedPid_) + ",";
        lastVotedPid = lastVotedPid_;
    }
    if (sourceType != OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO || 
            transactionFlags.find(strLastVotedPid) != std::string::npos) {
            hasUiOrSurface = true;
    }
    // OH SURFACE SOURCE VIDEO AN UI VOTE
    if (!isSwitchOn_ || sourceType != OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO || buffer == nullptr) {
        return;
    }
    double videoRate = 0.0;
    sptr<BufferExtraData> extraData = buffer->GetExtraData();
    if (extraData != nullptr) {
        extraData->ExtraGet(VIDEO_RATE_FLAG, videoRate);
    }
    auto initTask = [this, surfaceNodeId, videoRate]() {
        std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
        std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote;
        if (surfaceVideoFrameRateVote_.find(surfaceNodeId) == surfaceVideoFrameRateVote_.end()) {
            rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(surfaceNodeId,
                [this](uint64_t id) { this->ReleaseSurfaceMap(id); },
                [this](uint64_t id, uint32_t rate) { this->SurfaceVideoVote(id, rate); });
            surfaceVideoFrameRateVote_.insert(std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(
                surfaceNodeId, rsVideoFrameRateVote));
        } else {
            rsVideoFrameRateVote = surfaceVideoFrameRateVote_[surfaceNodeId];
        }
        rsVideoFrameRateVote->StartVideoFrameRateVote(videoRate);
    };
    if (ffrtQueue_) {
        ffrtQueue_->submit(initTask);
    }
}

void RSFrameRateVote::ReleaseSurfaceMap(uint64_t surfaceNodeId)
{
    auto initTask = [this, surfaceNodeId]() {
        SurfaceVideoVote(surfaceNodeId, 0);
        std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
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
    if (rate == 0) {
        auto it = surfaceVideoRate_.find(surfaceNodeId);
        if (it != surfaceVideoRate_.end()) {
            surfaceVideoRate_.erase(it);
        }
    } else {
        surfaceVideoRate_[surfaceNodeId] = rate;
    }
    if (surfaceVideoRate_.size() == 0) {
        CancelVoteRate(lastVotedPid_, VIDEO_VOTE_FLAG);
        lastVotedPid_ = DEFAULT_PID;
        lastVotedRate_ = OLED_NULL_HZ;
        if (ffrtQueue_ && taskHandler_) {
            ffrtQueue_->cancel(taskHandler_);
            taskHandler_ = nullptr;
        }
        return;
    }
    auto maxElement = std::max_element(surfaceVideoRate_.begin(), surfaceVideoRate_.end(),
        [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });
    uint32_t maxRate = maxElement->second;
    pid_t maxPid = ExtractPid(maxElement->first);
    lastSurfaceNodeId_ = maxElement->first;
    if (maxRate == lastVotedRate_ && maxPid == lastVotedPid_) {
        return;
    }
    CancelVoteRate(lastVotedPid_, VIDEO_VOTE_FLAG);
    lastVotedPid_ = maxPid;
    lastVotedRate_ = maxRate;
    // Delay for one second to stabilize the voting
    if (ffrtQueue_ && taskHandler_) {
        ffrtQueue_->cancel(taskHandler_);
        taskHandler_ = nullptr;
    }
    auto initTask = [this, maxPid, maxRate]() {
        VoteRate(maxPid, VIDEO_VOTE_FLAG, maxRate);
    };
    ffrt::task_attr taskAttr;
    taskAttr.delay(VIDEO_VOTE_DELAYS_TIME);
    if (ffrtQueue_) {
        taskHandler_ = ffrtQueue_->submit_h(initTask, taskAttr);
    }
}

void RSFrameRateVote::VoteRate(pid_t pid, std::string eventName, uint32_t rate)
{
    isVoted_ = true;
    EventInfo eventInfo = {
        .eventName = eventName,
        .eventStatus = true,
        .minRefreshRate = rate,
        .maxRefreshRate = rate,
    };
    NotifyRefreshRateEvent(pid, eventInfo);
}

void RSFrameRateVote::CancelVoteRate(pid_t pid, std::string eventName)
{
    if (!isVoted_) {
        return;
    }
    isVoted_ = false;
    EventInfo eventInfo = {
        .eventName = eventName,
        .eventStatus = false,
    };
    NotifyRefreshRateEvent(pid, eventInfo);
}

void RSFrameRateVote::NotifyRefreshRateEvent(pid_t pid, EventInfo eventInfo)
{
    if (pid > DEFAULT_PID) {
        RS_LOGI("video vote pid:%{public}d rate(%{public}u, %{public}u)",
            pid, eventInfo.minRefreshRate, eventInfo.maxRefreshRate);
        HgmTaskHandleThread::Instance().PostTask([pid, eventInfo]() {
            auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
            if (frameRateMgr != nullptr) {
                frameRateMgr->HandleRefreshRateEvent(pid, eventInfo);
            }
        });
    }
}
} // namespace Rosen
} // namespace OHOS