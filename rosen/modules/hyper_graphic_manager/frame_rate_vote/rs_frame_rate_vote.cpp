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
#include <charconv>
#include <chrono>

#include "hgm_core.h"
#include "platform/common/rs_log.h"
#include "rs_video_frame_rate_vote.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string VIDEO_RATE_FLAG = "VIDEO_RATE";
const std::string VIDEO_VOTE_FLAG = "VOTER_VIDEO";
constexpr uint64_t FFRT_QOS_INHERIT = 4;
constexpr uint64_t DANMU_MAX_INTERVAL_TIME = 50;
constexpr int32_t VIDEO_VOTE_DELAYS_TIME = 1000 * 1000;
constexpr int32_t NORMAL_BUFFER_COUNT = 1;
constexpr int32_t BUFFER_COUNT_THRESHOLD = 4;
}
std::atomic<bool> RSFrameRateVote::isVideoApp_{false};

RSFrameRateVote::RSFrameRateVote()
{
    ffrtQueue_ = std::make_shared<ffrt::queue>("frame_rate_vote_queue");
}

void RSFrameRateVote::SetVideoFrameRateSwtich(bool isSwitchOn)
{
    isSwitchOn_ = isSwitchOn;
    RS_LOGI("video vote feature isSwitchOn:%{public}s", isSwitchOn_ ? "true" : "false");
}

RSFrameRateVote::~RSFrameRateVote()
{
    ffrtQueue_ = nullptr;
}

void RSFrameRateVote::SetTransactionFlags(const std::string& transactionFlags)
{
    if (isVideoApp_.load()) {
        std::lock_guard<std::mutex> lock(mutex_);
        transactionFlags_ = transactionFlags;
    }
}

void RSFrameRateVote::CheckSurfaceAndUi(uint64_t timestamp)
{
    if (!isVideoApp_.load() || !hasUiOrSurface) {
        return;
    }
    hasUiOrSurface = false;
    auto lastUpdateTime = currentUpdateTime_;
    currentUpdateTime_ = timestamp / NS_PER_MS;
    auto duration = currentUpdateTime_ > lastUpdateTime ? currentUpdateTime_ - lastUpdateTime : 0;
    if (duration >= DANMU_MAX_INTERVAL_TIME) {
        return;
    }

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

void RSFrameRateVote::SetVoterRateFunc(VideoVoterFunc func)
{
    voterRateFunc_ = func;
}

void RSFrameRateVote::VideoFrameRateVote(uint64_t surfaceNodeId, OHSurfaceSource sourceType,
    sptr<SurfaceBuffer>& buffer, int32_t bufferCount)
{
    if (!isVideoApp_.load() || !isSwitchOn_ || buffer == nullptr) {
        return;
    }

    if (CheckSurfaceNodeIdChange(surfaceNodeId) || CheckAvailableBufferCount(bufferCount)) {
        ReleaseSurfaceMap(lastSurfaceNodeId_);
        return;
    }

    // transactionFlags_ format is [pid, eventId]
    std::string transactionFlags;
    std::string strLastVotedPid;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        transactionFlags = transactionFlags_;
        strLastVotedPid = "[" + std::to_string(lastVotedPid_) + ",";
    }
    if (transactionFlags.find(strLastVotedPid) != std::string::npos) {
        hasUiOrSurface = true;
    }
    RS_LOGD("sourceType: %{public}d, hasUiOrSurface: %{public}d", sourceType, hasUiOrSurface);

    uint32_t videoRate = 0;
    pid_t curPid = ExtractPid(surfaceNodeId);
    {
        std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
        auto rateIt = videoRateInfo_.find(curPid);
        if (rateIt != videoRateInfo_.end()) {
            videoRate = rateIt->second;
        }
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
        ffrt::task_attr taskAttr;
        taskAttr.qos(FFRT_QOS_INHERIT);
        ffrtQueue_->submit_h(initTask, taskAttr);
    }
}

void RSFrameRateVote::ReleaseSurfaceMap(uint64_t surfaceNodeId)
{
    auto initTask = [this, surfaceNodeId]() {
        SurfaceVideoVote(surfaceNodeId, 0);
        std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
        auto it = surfaceVideoFrameRateVote_.find(surfaceNodeId);
        if (it != surfaceVideoFrameRateVote_.end()) {
            RS_LOGI("video vote release surfaceNodeId(%{public}" PRIu64 "), size(%{public}d)",
                surfaceNodeId, static_cast<int>(surfaceVideoFrameRateVote_.size()));
            surfaceVideoFrameRateVote_.erase(it);
        }
    };
    if (ffrtQueue_) {
        ffrt::task_attr taskAttr;
        taskAttr.qos(FFRT_QOS_INHERIT);
        ffrtQueue_->submit_h(initTask, taskAttr);
    }
}

void RSFrameRateVote::SurfaceVideoVote(uint64_t surfaceNodeId, uint32_t rate)
{
    RS_TRACE_FUNC();
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
    RS_TRACE_NAME_FMT("SurfaceVideoVote maxRate: %d, maxPid: %d", maxRate, maxPid);
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
    if (ffrtQueue_) {
        ffrt::task_attr taskAttr;
        taskAttr.qos(FFRT_QOS_INHERIT);
        taskAttr.delay(VIDEO_VOTE_DELAYS_TIME);
        taskHandler_ = ffrtQueue_->submit_h(initTask, taskAttr);
    }
}

void RSFrameRateVote::VoteRate(pid_t pid, std::string eventName, uint32_t rate)
{
    isVoted_ = true;
    if (voterRateFunc_) {
        voterRateFunc_("PID", std::to_string(pid));
        voterRateFunc_("EVENT_NAME", eventName);
        voterRateFunc_("EVENT_STATUS", "true");
        voterRateFunc_("REFRESH_RATE", std::to_string(rate));
    }
}

void RSFrameRateVote::CancelVoteRate(pid_t pid, std::string eventName)
{
    if (!isVoted_) {
        return;
    }
    isVoted_ = false;
    if (voterRateFunc_) {
        voterRateFunc_("PID", std::to_string(pid));
        voterRateFunc_("EVENT_NAME", eventName);
        voterRateFunc_("EVENT_STATUS", "false");
    }
}

void RSFrameRateVote::SetVideoRateInfo(const std::unordered_map<std::string, std::string>& videoRateInfo)
{
    if (!isVideoApp_.load()) {
        return;
    }

    auto pidIt = videoRateInfo.find("pid");
    if (pidIt == videoRateInfo.end()) {
        RS_LOGD("SetVideoRateInfo can not find pid");
        RS_TRACE_NAME("SetVideoRateInfo can not find pid");
        return;
    }
    pid_t pid = 0;
    auto resultPid = std::from_chars(pidIt->second.data(), pidIt->second.data() + pidIt->second.size(), pid);
    if (resultPid.ec != std::errc() || pid <= 0) {
        RS_LOGE("SetVideoRateInfo read pid fail or invalid pid value");
        return;
    }

    auto rateIt = videoRateInfo.find("decRate");
    if (rateIt == videoRateInfo.end()) {
        RS_LOGI("SetVideoRateInfo can not find decRate");
        return;
    }
    uint32_t decRate = 0;
    auto resultRate = std::from_chars(rateIt->second.data(), rateIt->second.data() + rateIt->second.size(), decRate);
    if (resultRate.ec != std::errc()) {
        RS_LOGE("SetVideoRateInfo read decRate fail");
        return;
    }

    {
        std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
        if (decRate == 0) {
            auto it = videoRateInfo_.find(pid);
            if (it != videoRateInfo_.end()) {
                videoRateInfo_.erase(it);
            }
            return;
        }
        videoRateInfo_[pid] = decRate;
    }
}

bool RSFrameRateVote::CheckSurfaceNodeIdChange(uint64_t surfaceNodeId)
{
    const auto lastUpdateTime = lastSurfaceNodeIdUpdateTime_.load(std::memory_order_relaxed);
    lastSurfaceNodeIdUpdateTime_.store(static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count()), std::memory_order_relaxed);
    const auto duration = (lastSurfaceNodeIdUpdateTime_.load(std::memory_order_relaxed) > lastUpdateTime) ?
        (lastSurfaceNodeIdUpdateTime_.load(std::memory_order_relaxed) - lastUpdateTime) : 0;

    const uint64_t lastId = lastSurfaceNodeId_.load();
    if (surfaceNodeId != lastId && static_cast<uint64_t>(duration) < DANMU_MAX_INTERVAL_TIME) {
        RS_LOGD("sId changed, curId: %{public}" PRIu64 ", lastId: %{public}" PRIu64 "",
            surfaceNodeId, lastId);
        RS_TRACE_NAME_FMT("sId changed, curId: %" PRIu64 ", lastId: %" PRIu64 "", surfaceNodeId, lastId);
        return true;
    }
    return false;
}

bool RSFrameRateVote::CheckAvailableBufferCount(int32_t bufferCount)
{
    std::lock_guard<ffrt::mutex> autoLock(ffrtMutex_);
    bufferCountHistory_[bufferCountIndex_] = bufferCount;
    bufferCountIndex_ = (bufferCountIndex_ + 1) % BUFFER_COUNT_HISTORY_SIZE;

    int32_t count = 0;
    for (int32_t i = 0; i < BUFFER_COUNT_HISTORY_SIZE; i++) {
        if (bufferCountHistory_[i] > NORMAL_BUFFER_COUNT) {
            count++;
        }
    }

    if (count >= BUFFER_COUNT_THRESHOLD) {
        RS_LOGD("bufferCount > 1 for 4 times");
        RS_TRACE_NAME("bufferCount > 1 for 4 times");
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS