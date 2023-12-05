/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "platform/ohos/rs_jank_stats.h"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>

#include "hisysevent.h"
#include "rs_trace.h"

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float VSYNC_PERIOD = 16.6f;                // 16.6ms
constexpr float S_TO_MS = 1000.f;                    // s to ms
constexpr int64_t ANIMATION_TIMEOUT = 5000;          // 5s
constexpr int64_t S_TO_NS = 1000000000;              // s to ns
}

RSJankStats& RSJankStats::GetInstance()
{
    static RSJankStats instance;
    return instance;
}

void RSJankStats::SetStartTime()
{
    startTime_ = GetCurrentSystimeMs();
    startTimeSteady_ = GetCurrentSteadyTimeMs();
    if (isFirstSetStart_) {
        lastReportTime_ = startTime_;
        lastReportTimeSteady_ = startTimeSteady_;
    }
    {
        std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
        for (auto &[animationId, jankFrames] : animateJankFrames_) {
            jankFrames.isReportEventResponse_ = jankFrames.isSetReportEventResponse_;
            jankFrames.isSetReportEventResponse_ = false;
            jankFrames.isReportEventComplete_ = jankFrames.isSetReportEventComplete_;
            jankFrames.isSetReportEventComplete_ = false;
            jankFrames.isReportEventJankFrame_ = jankFrames.isSetReportEventJankFrame_;
            jankFrames.isSetReportEventJankFrame_ = false;
        }
    }
    isFirstSetStart_ = false;
}

void RSJankStats::SetEndTime()
{
    if (startTime_ == TIMESTAMP_INITIAL || startTimeSteady_ == TIMESTAMP_INITIAL) {
        ROSEN_LOGE("RSJankStats::SetEndTime startTime is not initialized");
        return;
    }
    UpdateEndTime();
    const int64_t vsyncDuration = endTimeSteady_ - startTimeSteady_;
    const int64_t missedVsync = static_cast<int64_t>(vsyncDuration / VSYNC_PERIOD);
    SetRSJankStats(missedVsync);
    const int64_t frameDurationToRecord = (isFirstSetEnd_ ? vsyncDuration : (endTimeSteady_ - lastEndTimeSteady_));
    const int64_t missedFramesToRecord = static_cast<int64_t>(frameDurationToRecord / VSYNC_PERIOD);
    RecordJankFrameInit();
    RecordJankFrame(missedFramesToRecord);
    {
        std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
        for (auto &[animationId, jankFrames] : animateJankFrames_) {
            if (jankFrames.isReportEventResponse_) {
                ReportEventResponse(jankFrames);
                jankFrames.isUpdateJankFrame_ = true;
            }
            if (jankFrames.isUpdateJankFrame_) {
                UpdateJankFrame(jankFrames);
            }
            if (jankFrames.isReportEventComplete_) {
                ReportEventComplete(jankFrames);
            }
            if (jankFrames.isReportEventJankFrame_) {
                ReportEventJankFrame(jankFrames);
            }
            if (jankFrames.isReportEventResponse_) {
                SetAnimationTraceBegin(jankFrames);
            }
            if (jankFrames.isReportEventComplete_ || jankFrames.isReportEventJankFrame_) {
                RecordAnimationDynamicFrameRate(jankFrames);
                SetAnimationTraceEnd(jankFrames);
                jankFrames.isUpdateJankFrame_ = false;
            }
            jankFrames.isFirstFrame_ = false;
        }
    }
    {
        std::lock_guard<std::mutex> lock(firstFrameAppPidsMutex_);
        ReportEventFirstFrame();
    }
    CheckAnimationTraceTimeout();
    isFirstSetEnd_ = false;
}

void RSJankStats::UpdateEndTime()
{
    if (isFirstSetEnd_) {
        lastEndTime_ = GetCurrentSystimeMs();
        endTime_ = lastEndTime_;
        lastEndTimeSteady_ = GetCurrentSteadyTimeMs();
        endTimeSteady_ = lastEndTimeSteady_;
        return;
    }
    lastEndTime_ = endTime_;
    endTime_ = GetCurrentSystimeMs();
    lastEndTimeSteady_ = endTimeSteady_;
    endTimeSteady_ = GetCurrentSteadyTimeMs();
}

void RSJankStats::SetRSJankStats(int64_t missedVsync)
{
    if (missedVsync <= 0) {
        return;
    }
    size_t type = JANK_FRAME_INVALID;
    if (missedVsync < 6) {                                       // JANK_FRAME_6_FREQ   : (0,6)
        type = JANK_FRAME_6_FREQ;
    } else if (missedVsync < 15) {                               // JANK_FRAME_15_FREQ  : [6,15)
        type = JANK_FRAME_15_FREQ;
    } else if (missedVsync < 20) {                               // JANK_FRAME_20_FREQ  : [15,20)
        type = JANK_FRAME_20_FREQ;
    } else if (missedVsync < 36) {                               // JANK_FRAME_36_FREQ  : [20,36)
        type = JANK_FRAME_36_FREQ;
    } else if (missedVsync < 48) {                               // JANK_FRAME_48_FREQ  : [36,48)
        type = JANK_FRAME_48_FREQ;
    } else if (missedVsync < 60) {                               // JANK_FRAME_60_FREQ  : [48,60)
        type = JANK_FRAME_60_FREQ;
    } else if (missedVsync < 120) {                              // JANK_FRAME_120_FREQ : [60,120)
        type = JANK_FRAME_120_FREQ;
    } else if (missedVsync < 180) {                              // JANK_FRAME_180_FREQ : [120,180)
        type = JANK_FRAME_180_FREQ;
    } else {
        ROSEN_LOGW("RSJankStats::SetJankStats jank frames over 180");
        return;
    }
    if (rsJankStats_[type] == USHRT_MAX) {
        ROSEN_LOGW("RSJankStats::SetJankStats rsJankStats_ value oversteps USHRT_MAX");
        return;
    }
    if (type != JANK_FRAME_6_FREQ) {
        RS_TRACE_INT(JANK_FRAME_6F_COUNT_TRACE_NAME, missedVsync);
        lastJankFrame6FreqTimeSteady_ = endTimeSteady_;
    }
    rsJankStats_[type]++;
    isNeedReportJankStats_ = true;
    if (type != JANK_FRAME_6_FREQ) {
        RS_TRACE_INT(JANK_FRAME_6F_COUNT_TRACE_NAME, 0);
    }
}

void RSJankStats::UpdateJankFrame(JankFrames& jankFrames)
{
    if (jankFrames.startTimeSteady_ == TIMESTAMP_INITIAL) {
        jankFrames.startTimeSteady_ = startTimeSteady_;
    }
    const bool isAnimationFirstFrame = isFirstSetEnd_ || jankFrames.isFirstFrame_;
    const int64_t frameDuration =
        (isAnimationFirstFrame ? (endTimeSteady_ - startTimeSteady_) : (endTimeSteady_ - lastEndTimeSteady_));
    const int32_t missedFrames = static_cast<int32_t>(frameDuration / VSYNC_PERIOD);
    const int32_t missedFramesToReport =
        (isAnimationFirstFrame ? missedFrames : std::max<int32_t>(0, missedFrames - 1));
    jankFrames.totalFrames_++;
    jankFrames.totalFrameTimeSteady_ += frameDuration;
    jankFrames.maxFrameTimeSteady_ = std::max<int64_t>(jankFrames.maxFrameTimeSteady_, frameDuration);
    if (missedFramesToReport > 0) {
        jankFrames.totalMissedFrames_ += missedFramesToReport;
        jankFrames.seqMissedFrames_ += missedFramesToReport;
        jankFrames.maxSeqMissedFrames_ =
            std::max<int32_t>(jankFrames.maxSeqMissedFrames_, jankFrames.seqMissedFrames_);
    } else {
        jankFrames.seqMissedFrames_ = 0;
    }
}

void RSJankStats::ReportJankStats()
{
    if (lastReportTime_ == TIMESTAMP_INITIAL || lastReportTimeSteady_ == TIMESTAMP_INITIAL) {
        ROSEN_LOGE("RSJankStats::ReportJankStats lastReportTime is not initialized");
        return;
    }
    int64_t reportTime = GetCurrentSystimeMs();
    int64_t reportTimeSteady = GetCurrentSteadyTimeMs();
    if (!isNeedReportJankStats_) {
        ROSEN_LOGW("RSJankStats::ReportJankStats Nothing need to report");
        lastReportTime_ = reportTime;
        lastReportTimeSteady_ = reportTimeSteady;
        lastJankFrame6FreqTimeSteady_ = TIMESTAMP_INITIAL;
        std::fill(rsJankStats_.begin(), rsJankStats_.end(), 0);
        return;
    }
    int64_t lastJankFrame6FreqTime = ((lastJankFrame6FreqTimeSteady_ == TIMESTAMP_INITIAL) ? 0 :
        (reportTime - (reportTimeSteady - lastJankFrame6FreqTimeSteady_)));
    RS_TRACE_NAME("RSJankStats::ReportJankStats receive notification: reportTime " + std::to_string(reportTime) +
                  ", lastJankFrame6FreqTime " + std::to_string(lastJankFrame6FreqTime));
    int64_t reportDuration = reportTimeSteady - lastReportTimeSteady_;
    auto reportName = "JANK_STATS_RS";
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "STARTTIME", static_cast<uint64_t>(lastReportTime_),
        "DURATION", static_cast<uint64_t>(reportDuration), "JANK_STATS", rsJankStats_,
        "JANK_STATS_VER", JANK_RANGE_VERSION);
    lastReportTime_ = reportTime;
    lastReportTimeSteady_ = reportTimeSteady;
    lastJankFrame6FreqTimeSteady_ = TIMESTAMP_INITIAL;
    std::fill(rsJankStats_.begin(), rsJankStats_.end(), 0);
    isNeedReportJankStats_ = false;
}

void RSJankStats::SetReportEventResponse(const DataBaseRs& info)
{
    RS_TRACE_NAME("RSJankStats::SetReportEventResponse receive notification: " + GetSceneDescription(info));
    int64_t setTimeSteady = GetCurrentSteadyTimeMs();
    std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
    EraseIf(animateJankFrames_, [setTimeSteady](const auto& pair) -> bool {
        return setTimeSteady - pair.second.setTimeSteady_ > ANIMATION_TIMEOUT;
    });
    EraseIf(traceIdRemainder_, [setTimeSteady](const auto& pair) -> bool {
        return setTimeSteady - pair.second.setTimeSteady_ > ANIMATION_TIMEOUT;
    });
    const auto animationId = GetAnimationId(info);
    if (animateJankFrames_.find(animationId) == animateJankFrames_.end()) {
        JankFrames jankFrames;
        jankFrames.info_ = info;
        jankFrames.isSetReportEventResponse_ = true;
        jankFrames.setTimeSteady_ = setTimeSteady;
        jankFrames.isFirstFrame_ = true;
        jankFrames.traceId_ = GetTraceIdInit(info, setTimeSteady);
        animateJankFrames_.emplace(animationId, jankFrames);
    } else {
        animateJankFrames_[animationId].info_ = info;
        animateJankFrames_[animationId].isSetReportEventResponse_ = true;
    }
}

void RSJankStats::SetReportEventComplete(const DataBaseRs& info)
{
    RS_TRACE_NAME("RSJankStats::SetReportEventComplete receive notification: " + GetSceneDescription(info));
    int64_t setTimeSteady = GetCurrentSteadyTimeMs();
    std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
    const auto animationId = GetAnimationId(info);
    if (animateJankFrames_.find(animationId) == animateJankFrames_.end()) {
        ROSEN_LOGW("RSJankStats::SetReportEventComplete Not find exited animationId");
        JankFrames jankFrames;
        jankFrames.info_ = info;
        jankFrames.isSetReportEventComplete_ = true;
        jankFrames.setTimeSteady_ = setTimeSteady;
        jankFrames.traceId_ = GetTraceIdInit(info, setTimeSteady);
        animateJankFrames_.emplace(animationId, jankFrames);
    } else {
        animateJankFrames_[animationId].info_ = info;
        animateJankFrames_[animationId].isSetReportEventComplete_ = true;
    }
}

void RSJankStats::SetReportEventJankFrame(const DataBaseRs& info)
{
    RS_TRACE_NAME("RSJankStats::SetReportEventJankFrame receive notification: " + GetSceneDescription(info));
    std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
    const auto animationId = GetAnimationId(info);
    if (animateJankFrames_.find(animationId) == animateJankFrames_.end()) {
        ROSEN_LOGW("RSJankStats::SetReportEventJankFrame Not find exited animationId");
    } else {
        animateJankFrames_[animationId].info_ = info;
        animateJankFrames_[animationId].isSetReportEventJankFrame_ = true;
    }
}

void RSJankStats::SetAppFirstFrame(pid_t appPid)
{
    std::lock_guard<std::mutex> lock(firstFrameAppPidsMutex_);
    firstFrameAppPids_.push(appPid);
}

void RSJankStats::ReportEventResponse(const JankFrames& jankFrames) const
{
    auto reportName = "INTERACTION_RESPONSE_LATENCY";
    const auto &info = jankFrames.info_;
    int64_t inputTime = ConvertTimeToSystime(info.inputTime);
    int64_t beginVsyncTime = ConvertTimeToSystime(info.beginVsyncTime);
    int64_t responseLatency = endTime_ - inputTime;
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "APP_PID", info.appPid, "VERSION_CODE", info.versionCode,
        "VERSION_NAME", info.versionName, "BUNDLE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName,
        "PROCESS_NAME", info.processName, "PAGE_URL", info.pageUrl, "SCENE_ID", info.sceneId,
        "SOURCE_TYPE", info.sourceType, "NOTE", info.note, "INPUT_TIME", static_cast<uint64_t>(inputTime),
        "ANIMATION_START_TIME", static_cast<uint64_t>(beginVsyncTime), "RENDER_TIME", static_cast<uint64_t>(endTime_),
        "RESPONSE_LATENCY", static_cast<uint64_t>(responseLatency));
}

void RSJankStats::ReportEventComplete(const JankFrames& jankFrames) const
{
    auto reportName = "INTERACTION_COMPLETED_LATENCY";
    const auto &info = jankFrames.info_;
    int64_t inputTime = ConvertTimeToSystime(info.inputTime);
    int64_t beginVsyncTime = ConvertTimeToSystime(info.beginVsyncTime);
    int64_t endVsyncTime = ConvertTimeToSystime(info.endVsyncTime);
    int64_t animationStartLatency = beginVsyncTime - inputTime;
    int64_t animationEndLatency = endVsyncTime - beginVsyncTime;
    int64_t completedLatency = endTime_ - inputTime;
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "APP_PID", info.appPid, "VERSION_CODE", info.versionCode,
        "VERSION_NAME", info.versionName, "BUNDLE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName,
        "PROCESS_NAME", info.processName, "PAGE_URL", info.pageUrl, "SCENE_ID", info.sceneId,
        "SOURCE_TYPE", info.sourceType, "NOTE", info.note, "INPUT_TIME", static_cast<uint64_t>(inputTime),
        "ANIMATION_START_LATENCY", static_cast<uint64_t>(animationStartLatency), "ANIMATION_END_LATENCY",
        static_cast<uint64_t>(animationEndLatency), "E2E_LATENCY", static_cast<uint64_t>(completedLatency));
}

void RSJankStats::ReportEventJankFrame(const JankFrames& jankFrames) const
{
    if (jankFrames.totalFrames_ <= 0) {
        ROSEN_LOGW("RSJankStats::ReportEventJankFrame totalFrames is zero, nothing need to report");
        return;
    }
    auto reportName = "INTERACTION_RENDER_JANK";
    float aveFrameTimeSteady = jankFrames.totalFrameTimeSteady_ / static_cast<float>(jankFrames.totalFrames_);
    const auto &info = jankFrames.info_;
    RS_TRACE_NAME_FMT("RSJankStats::ReportEventJankFrame maxFrameTime is %" PRId64 "ms: %s",
                      jankFrames.maxFrameTimeSteady_, GetSceneDescription(info).c_str());
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "UNIQUE_ID", info.uniqueId, "SCENE_ID", info.sceneId,
        "PROCESS_NAME", info.processName, "MODULE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName, "PAGE_URL",
        info.pageUrl, "TOTAL_FRAMES", jankFrames.totalFrames_, "TOTAL_MISSED_FRAMES", jankFrames.totalMissedFrames_,
        "MAX_FRAMETIME", static_cast<uint64_t>(jankFrames.maxFrameTimeSteady_), "AVERAGE_FRAMETIME", aveFrameTimeSteady,
        "MAX_SEQ_MISSED_FRAMES", jankFrames.maxSeqMissedFrames_, "IS_FOLD_DISP", IS_FOLD_DISP,
        "BUNDLE_NAME_EX", info.note);
}

void RSJankStats::ReportEventFirstFrame()
{
    while (!firstFrameAppPids_.empty()) {
        pid_t appPid = firstFrameAppPids_.front();
        ReportEventFirstFrameByPid(appPid);
        firstFrameAppPids_.pop();
    }
}

void RSJankStats::ReportEventFirstFrameByPid(pid_t appPid) const
{
    RS_TRACE_NAME_FMT("RSJankStats::ReportEventFirstFrame app pid %d", appPid);
    auto reportName = "FIRST_FRAME_DRAWN";
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "APP_PID", static_cast<int32_t>(appPid));
}

void RSJankStats::RecordJankFrameInit()
{
    for (auto& recordStats : jankAnimatorFrameRecorder_) {
        recordStats.isRecorded_ = false;
    }
}

void RSJankStats::RecordJankFrame(int64_t missedFrames)
{
    if (missedFrames <= 0 || animationAsyncTraces_.empty()) {
        return;
    }
    for (auto& recordStats : jankAnimatorFrameRecorder_) {
        RecordJankFrameSingle(missedFrames, recordStats);
    }
}

void RSJankStats::RecordJankFrameSingle(int64_t missedFrames, JankFrameRecordStats& recordStats)
{
    if (recordStats.isRecorded_) {
        return;
    }
    if (missedFrames >= recordStats.recordThreshold_) {
        RS_TRACE_INT(recordStats.countTraceName_, missedFrames);
        recordStats.isRecorded_ = true;
        RS_TRACE_INT(recordStats.countTraceName_, 0);
    }
}

void RSJankStats::RecordAnimationDynamicFrameRate(const JankFrames& jankFrames) const
{
    const int32_t traceId = jankFrames.traceId_;
    if (traceId == TRACE_ID_INITIAL) {
        ROSEN_LOGE("RSJankStats::RecordAnimationDynamicFrameRate traceId not initialized");
        return;
    }
    if (animationAsyncTraces_.find(traceId) == animationAsyncTraces_.end() || jankFrames.totalFrames_ <= 0 ||
        jankFrames.startTimeSteady_ == TIMESTAMP_INITIAL || endTimeSteady_ <= jankFrames.startTimeSteady_) {
        return;
    }
    float animationDuration = static_cast<float>(endTimeSteady_ - jankFrames.startTimeSteady_) / S_TO_MS;
    float animationTotalFrames = static_cast<float>(jankFrames.totalFrames_);
    float animationDynamicFrameRate = animationTotalFrames / animationDuration;
    RS_TRACE_NAME_FMT("RSJankStats::RecordAnimationDynamicFrameRate frame rate is %.2f: %s",
                      animationDynamicFrameRate, animationAsyncTraces_.at(traceId).traceName_.c_str());
}

void RSJankStats::SetAnimationTraceBegin(const JankFrames& jankFrames)
{
    const int32_t traceId = jankFrames.traceId_;
    if (traceId == TRACE_ID_INITIAL) {
        ROSEN_LOGE("RSJankStats::SetAnimationTraceBegin traceId not initialized");
        return;
    }
    if (animationAsyncTraces_.find(traceId) != animationAsyncTraces_.end()) {
        return;
    }
    const auto &info = jankFrames.info_;
    const std::string traceName = GetSceneDescription(info);
    AnimationTraceStats traceStat = {.traceName_ = traceName, .traceCreateTimeSteady_ = endTimeSteady_};
    animationAsyncTraces_.emplace(traceId, traceStat);
    RS_ASYNC_TRACE_BEGIN(traceName, traceId);
}

void RSJankStats::SetAnimationTraceEnd(const JankFrames& jankFrames)
{
    const int32_t traceId = jankFrames.traceId_;
    if (traceId == TRACE_ID_INITIAL) {
        ROSEN_LOGE("RSJankStats::SetAnimationTraceEnd traceId not initialized");
        return;
    }
    if (animationAsyncTraces_.find(traceId) == animationAsyncTraces_.end()) {
        return;
    }
    RS_ASYNC_TRACE_END(animationAsyncTraces_.at(traceId).traceName_, traceId);
    animationAsyncTraces_.erase(traceId);
}

void RSJankStats::CheckAnimationTraceTimeout()
{
    if (++animationTraceCheckCnt_ < ANIMATION_TRACE_CHECK_FREQ) {
        return;
    }
    int64_t checkEraseTimeSteady = endTimeSteady_;
    EraseIf(animationAsyncTraces_, [checkEraseTimeSteady](const auto& pair) -> bool {
        bool needErase = checkEraseTimeSteady - pair.second.traceCreateTimeSteady_ > ANIMATION_TIMEOUT;
        if (needErase) {
            RS_ASYNC_TRACE_END(pair.second.traceName_, pair.first);
        }
        return needErase;
    });
    animationTraceCheckCnt_ = 0;
}
std::string RSJankStats::GetSceneDescription(const DataBaseRs& info) const
{
    std::stringstream sceneDescription;
    int64_t inputTime = ConvertTimeToSystime(info.inputTime);
    sceneDescription << info.sceneId << ", " << info.bundleName << ", "
                     << info.pageUrl << ", " << std::to_string(inputTime);
    return sceneDescription.str();
}

std::pair<int64_t, std::string> RSJankStats::GetAnimationId(const DataBaseRs& info) const
{
    std::pair<int64_t, std::string> animationId(info.uniqueId, info.sceneId);
    return animationId;
}

int32_t RSJankStats::GetTraceIdInit(const DataBaseRs& info, int64_t setTimeSteady)
{
    if (traceIdRemainder_.find(info.uniqueId) == traceIdRemainder_.end()) {
        TraceIdRemainderStats traceIdStat;
        traceIdRemainder_.emplace(info.uniqueId, traceIdStat);
    }
    if (traceIdRemainder_.at(info.uniqueId).remainder_ == TRACE_ID_SCALE_PARAM) {
        traceIdRemainder_[info.uniqueId].remainder_ = 0;
    }
    traceIdRemainder_[info.uniqueId].setTimeSteady_ = setTimeSteady;
    int64_t mappedUniqueId = info.uniqueId * TRACE_ID_SCALE_PARAM + (traceIdRemainder_[info.uniqueId].remainder_++);
    int32_t traceId = static_cast<int32_t>(mappedUniqueId);
    return traceId;
}

int64_t RSJankStats::ConvertTimeToSystime(int64_t time) const
{
    if (time <= 0) {
        ROSEN_LOGW("RSJankStats::ConvertTimeToSystime, time is error");
        return 0;
    }
    struct timespec ts = { 0, 0 };
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    int64_t nowTime = static_cast<int64_t>(ts.tv_sec) * S_TO_NS + static_cast<int64_t>(ts.tv_nsec);
    int64_t curSysTime = GetCurrentSystimeMs();
    int64_t sysTime = curSysTime - (nowTime - time) / MS_TO_NS;
    return sysTime;
}

int64_t RSJankStats::GetCurrentSystimeMs() const
{
    auto curTime = std::chrono::system_clock::now().time_since_epoch();
    int64_t curSysTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
    return curSysTime;
}

int64_t RSJankStats::GetCurrentSteadyTimeMs() const
{
    auto curTime = std::chrono::steady_clock::now().time_since_epoch();
    int64_t curSteadyTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
    return curSteadyTime;
}

} // namespace Rosen
} // namespace OHOS
