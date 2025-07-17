/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include "cJSON.h"

#include "hisysevent.h"
#include "rs_trace.h"

#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_hisysevent.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float HITCH_TIME_EPSILON = 0.0001f;        // 0.0001ms
constexpr float VSYNC_PERIOD = 16.6f;                // 16.6ms
constexpr float S_TO_MS = 1000.f;                    // s to ms
constexpr int64_t ANIMATION_TIMEOUT = 10000;          // 10s
constexpr int64_t S_TO_NS = 1000000000;              // s to ns
constexpr int64_t VSYNC_JANK_LOG_THRESHOLED = 6;     // 6 times vsync
constexpr int64_t INPUTTIME_TIMEOUT = 100000000;          // ns, 100ms
constexpr uint64_t DELAY_TIME_MS = 1000;
constexpr uint32_t ACVIDEO_QUIT_FRAME_COUNT = 10;
}

RSJankStats& RSJankStats::GetInstance()
{
    static RSJankStats instance;
    return instance;
}

void RSJankStats::SetOnVsyncStartTime(int64_t onVsyncStartTime, int64_t onVsyncStartTimeSteady,
                                      float onVsyncStartTimeSteadyFloat)
{
    std::lock_guard<std::mutex> lock(mutex_);
    rsStartTime_ = onVsyncStartTime;
    rsStartTimeSteady_ = onVsyncStartTimeSteady;
    if (IS_CALCULATE_PRECISE_HITCH_TIME) {
        rsStartTimeSteadyFloat_ = onVsyncStartTimeSteadyFloat;
    }
}

void RSJankStats::SetAccumulatedBufferCount(int accumulatedBufferCount)
{
    std::lock_guard<std::mutex> lock(mutex_);
    accumulatedBufferCount_ = accumulatedBufferCount;
}

void RSJankStats::SetStartTime(bool doDirectComposition)
{
    std::lock_guard<std::mutex> lock(mutex_);
    SetStartTimeInner(doDirectComposition);
}

void RSJankStats::SetStartTimeInner(bool doDirectComposition)
{
    if (!doDirectComposition) {
        rtStartTime_ = GetCurrentSystimeMs();
        rtStartTimeSteady_ = GetCurrentSteadyTimeMs();
    }
    if (isFirstSetStart_) {
        lastReportTime_ = rtStartTime_;
        lastReportTimeSteady_ = rtStartTimeSteady_;
        lastSceneReportTime_ = rtStartTime_;
        lastSceneReportTimeSteady_  = rtStartTimeSteady_;
    }
    for (auto &[animationId, jankFrames] : animateJankFrames_) {
        jankFrames.isReportEventResponse_ = jankFrames.isSetReportEventResponseTemp_;
        jankFrames.isSetReportEventResponseTemp_ = jankFrames.isSetReportEventResponse_;
        jankFrames.isSetReportEventResponse_ = false;
        jankFrames.isReportEventComplete_ = jankFrames.isSetReportEventComplete_;
        jankFrames.isSetReportEventComplete_ = false;
        jankFrames.isReportEventJankFrame_ = jankFrames.isSetReportEventJankFrame_;
        jankFrames.isSetReportEventJankFrame_ = false;
    }
    isFirstSetStart_ = false;
}

bool RSJankStats::IsAnimationEmpty()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return implicitAnimationTotal_ + explicitAnimationTotal_ == 0;
}

void RSJankStats::SetEndTime(bool skipJankAnimatorFrame, bool discardJankFrames, uint32_t dynamicRefreshRate,
                             bool doDirectComposition, bool isReportTaskDelayed)
{
    std::lock_guard<std::mutex> lock(mutex_);
    SetEndTimeInner(skipJankAnimatorFrame, discardJankFrames, dynamicRefreshRate, doDirectComposition,
        isReportTaskDelayed);
}

void RSJankStats::SetEndTimeInner(bool skipJankAnimatorFrame, bool discardJankFrames, uint32_t dynamicRefreshRate,
                                  bool doDirectComposition, bool isReportTaskDelayed)
{
    if (rtStartTime_ == TIMESTAMP_INITIAL || rtStartTimeSteady_ == TIMESTAMP_INITIAL ||
        rsStartTime_ == TIMESTAMP_INITIAL || rsStartTimeSteady_ == TIMESTAMP_INITIAL) {
        ROSEN_LOGE("RSJankStats::SetEndTime startTime is not initialized");
        return;
    }
    isCurrentFrameSwitchToNotDoDirectComposition_ = isLastFrameDoDirectComposition_ && !doDirectComposition;
    if (!doDirectComposition) { UpdateEndTime(); }
    if (discardJankFrames) { ClearAllAnimation(); }
    SetRSJankStats(skipJankAnimatorFrame || discardJankFrames, dynamicRefreshRate);
    ReportSceneJankFrame(dynamicRefreshRate);
    RecordJankFrame(dynamicRefreshRate);
#ifdef RS_ENABLE_PREFETCH
    __builtin_prefetch(&firstFrameAppPids_, 0, 1);
#endif
    for (auto &[animationId, jankFrames] : animateJankFrames_) {
        if (jankFrames.isReportEventResponse_) {
            SetAnimationTraceBegin(animationId, jankFrames);
            ReportEventResponse(jankFrames);
        }
        // skip jank frame statistics at the first frame of animation && at the end frame of implicit animation
        if (jankFrames.isUpdateJankFrame_ && !jankFrames.isFirstFrame_ && !(!jankFrames.isDisplayAnimator_ &&
            (jankFrames.isReportEventComplete_ || jankFrames.isReportEventJankFrame_))) {
            UpdateJankFrame(jankFrames, skipJankAnimatorFrame, dynamicRefreshRate);
        }
        if (jankFrames.isReportEventJankFrame_) {
            RecordAnimationDynamicFrameRate(jankFrames, isReportTaskDelayed);
        }
        if (jankFrames.isReportEventComplete_ || jankFrames.isReportEventJankFrame_) {
            SetAnimationTraceEnd(jankFrames);
        }
        if (jankFrames.isReportEventComplete_) {
            ReportEventComplete(jankFrames);
        }
        if (jankFrames.isReportEventJankFrame_) {
            ReportEventJankFrame(jankFrames, isReportTaskDelayed);
            ReportEventHitchTimeRatio(jankFrames, isReportTaskDelayed);
        }
        jankFrames.isFirstFrame_ = jankFrames.isFirstFrameTemp_;
        jankFrames.isFirstFrameTemp_ = false;
    }
    ReportEventFirstFrame();
    CheckAnimationTraceTimeout();
    isLastFrameDoDirectComposition_ = doDirectComposition;
    isFirstSetEnd_ = false;
}

void RSJankStats::UpdateEndTime()
{
    if (isFirstSetEnd_) {
        rtLastEndTime_ = GetCurrentSystimeMs();
        rtEndTime_ = rtLastEndTime_;
        rtLastEndTimeSteady_ = GetCurrentSteadyTimeMs();
        rtEndTimeSteady_ = rtLastEndTimeSteady_;
        if (IS_CALCULATE_PRECISE_HITCH_TIME) {
            rtLastEndTimeSteadyFloat_ = GetCurrentSteadyTimeMsFloat();
            rtEndTimeSteadyFloat_ = rtLastEndTimeSteadyFloat_;
        }
        return;
    }
    rtLastEndTime_ = rtEndTime_;
    rtEndTime_ = GetCurrentSystimeMs();
    rtLastEndTimeSteady_ = rtEndTimeSteady_;
    rtEndTimeSteady_ = GetCurrentSteadyTimeMs();
    if (IS_CALCULATE_PRECISE_HITCH_TIME) {
        rtLastEndTimeSteadyFloat_ = rtEndTimeSteadyFloat_;
        rtEndTimeSteadyFloat_ = GetCurrentSteadyTimeMsFloat();
    }
}

bool RSJankStats::NeedPostTaskToUniRenderThread() const
{
    for (const auto &[_, jankFrames] : animateJankFrames_) {
        if (jankFrames.isReportEventResponse_ || jankFrames.isReportEventComplete_ ||
            jankFrames.isReportEventJankFrame_) {
            // report event will occur in this frame
            return true;
        }
    }
    return false;
}

void RSJankStats::HandleDirectComposition(const JankDurationParams& rsParams, bool isReportTaskDelayed,
    std::function<void(const std::function<void()>&)> postTaskHandler)
{
    std::lock_guard<std::mutex> lock(mutex_);
    rsStartTime_ = rsParams.timeStart_;
    rsStartTimeSteady_ = rsParams.timeStartSteady_;
    rtStartTime_ = rsParams.timeEnd_;
    rtStartTimeSteady_ = rsParams.timeEndSteady_;
    rtLastEndTime_ = rtEndTime_;
    rtEndTime_ = rsParams.timeEnd_;
    rtLastEndTimeSteady_ = rtEndTimeSteady_;
    rtEndTimeSteady_ = rsParams.timeEndSteady_;
    if (IS_CALCULATE_PRECISE_HITCH_TIME) {
        rsStartTimeSteadyFloat_ = rsParams.timeStartSteadyFloat_;
        rtLastEndTimeSteadyFloat_ = rtEndTimeSteadyFloat_;
        rtEndTimeSteadyFloat_ = rsParams.timeEndSteadyFloat_;
    }
    SetStartTimeInner(true);
    SetImplicitAnimationEndInner(rsParams.implicitAnimationEnd_);
    if (postTaskHandler == nullptr || !NeedPostTaskToUniRenderThread()) {
        SetEndTimeInner(rsParams.skipJankAnimatorFrame_, rsParams.discardJankFrames_, rsParams.refreshRate_, true,
            isReportTaskDelayed);
    } else { // post task to unirender thread when report event occurs in direct composition scene
        auto task = [this, rsParams, isReportTaskDelayed]() {
            SetEndTime(rsParams.skipJankAnimatorFrame_, rsParams.discardJankFrames_, rsParams.refreshRate_, true,
                isReportTaskDelayed);
        };
        postTaskHandler(task);
    }
}

// dynamicRefreshRate is retained for future algorithm adjustment, keep it unused currently
void RSJankStats::SetRSJankStats(bool skipJankStats, uint32_t /* dynamicRefreshRate */)
{
    if (skipJankStats) {
        RS_TRACE_NAME("RSJankStats::SetRSJankStats skip jank frame statistics");
        return;
    }
    const int64_t frameTime = GetEffectiveFrameTime(true);
    const int64_t missedVsync = static_cast<int64_t>(frameTime / VSYNC_PERIOD);
    if (missedVsync <= 0) {
        return;
    }
    RS_TRACE_NAME_FMT("RSJankStats::SetRSJankStats missedVsync %" PRId64 " frameTime %" PRId64, missedVsync, frameTime);
    if (missedVsync >= VSYNC_JANK_LOG_THRESHOLED) {
        ROSEN_LOGW("RSJankStats::SetJankStats jank frames %{public}" PRId64, missedVsync);
    }
    const size_t type = GetJankRangeType(missedVsync);
    if (type == JANK_FRAME_INVALID) {
        return;
    }
    if (rsJankStats_[type] == USHRT_MAX) {
        ROSEN_LOGD("RSJankStats::SetJankStats rsJankStats_ value oversteps USHRT_MAX");
        return;
    }
    if (type == JANK_FRAME_6_FREQ) {
        RS_TRACE_INT(JANK_FRAME_1_TO_5F_COUNT_TRACE_NAME, missedVsync);
    } else {
        RS_TRACE_INT(JANK_FRAME_6F_COUNT_TRACE_NAME, missedVsync);
        lastJankFrame6FreqTimeSteady_ = rtEndTimeSteady_;
        lastSceneJankFrame6FreqTimeSteady_ = rtEndTimeSteady_;
    }
    rsJankStats_[type]++;
    rsSceneJankStats_[type]++;
    isNeedReportJankStats_ = true;
    isNeedReportSceneJankStats_ = true;
    if (type == JANK_FRAME_6_FREQ) {
        RS_TRACE_INT(JANK_FRAME_1_TO_5F_COUNT_TRACE_NAME, 0);
    } else {
        RS_TRACE_INT(JANK_FRAME_6F_COUNT_TRACE_NAME, 0);
    }
}

size_t RSJankStats::GetJankRangeType(int64_t missedVsync) const
{
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
    }
    return type;
}

void RSJankStats::UpdateJankFrame(JankFrames& jankFrames, bool skipJankStats, uint32_t dynamicRefreshRate)
{
    if (jankFrames.startTime_ == TIMESTAMP_INITIAL) {
        jankFrames.startTime_ = rsStartTime_;
    }
    if (skipJankStats) {
        RS_TRACE_NAME("RSJankStats::UpdateJankFrame skip jank animator frame");
        return;
    }
    jankFrames.lastTotalFrames_ = jankFrames.totalFrames_;
    jankFrames.lastTotalFrameTimeSteady_ = jankFrames.totalFrameTimeSteady_;
    jankFrames.lastTotalMissedFrames_ = jankFrames.totalMissedFrames_;
    jankFrames.lastMaxFrameTimeSteady_ = jankFrames.maxFrameTimeSteady_;
    jankFrames.lastMaxTechFrameTimeSteady_ = jankFrames.maxTechFrameTimeSteady_;
    jankFrames.lastMaxRealFrameTimeSteady_ = jankFrames.maxRealFrameTimeSteady_;
    jankFrames.lastMaxSeqMissedFrames_ = jankFrames.maxSeqMissedFrames_;
    jankFrames.lastMaxFrameOccurenceTimeSteady_ = jankFrames.maxFrameOccurenceTimeSteady_;
    jankFrames.lastMaxFrameRefreshRate_ = jankFrames.maxFrameRefreshRate_;
    if (dynamicRefreshRate == 0) {
        dynamicRefreshRate = STANDARD_REFRESH_RATE;
    }
    const float standardFrameTime = S_TO_MS / dynamicRefreshRate;
    const bool isConsiderRsStartTime =
        jankFrames.isDisplayAnimator_ || jankFrames.isFirstFrame_ || isFirstSetEnd_;
    const float accumulatedTime = accumulatedBufferCount_ * standardFrameTime;
    const int64_t frameDuration = std::max<int64_t>(0, GetEffectiveFrameTime(isConsiderRsStartTime) - accumulatedTime);
    const int64_t frameTechDuration = GetEffectiveFrameTime(true);
    const int32_t missedFramesToReport = static_cast<int32_t>(frameDuration / VSYNC_PERIOD);
    jankFrames.totalFrames_++;
    jankFrames.totalFrameTimeSteady_ += frameDuration;
    if (jankFrames.maxFrameRefreshRate_ == 0) {
        jankFrames.maxFrameRefreshRate_ = dynamicRefreshRate;
    }
    if (frameDuration > jankFrames.maxFrameTimeSteady_) {
        jankFrames.maxFrameOccurenceTimeSteady_ = rtEndTimeSteady_;
        jankFrames.maxFrameTimeSteady_ = frameDuration;
        jankFrames.maxFrameRefreshRate_ = dynamicRefreshRate;
    }
    if (frameTechDuration > jankFrames.maxTechFrameTimeSteady_) {
        jankFrames.maxTechFrameTimeSteady_ = frameTechDuration;
    }
    if (frameDuration > jankFrames.maxRealFrameTimeSteady_ && !jankFrames.isAnimationInterrupted_) {
        jankFrames.maxRealFrameTimeSteady_ = frameDuration;
    }
    if (missedFramesToReport > 0) {
        jankFrames.totalMissedFrames_ += missedFramesToReport;
        jankFrames.seqMissedFrames_ += missedFramesToReport;
        jankFrames.maxSeqMissedFrames_ = std::max<int32_t>(jankFrames.maxSeqMissedFrames_, jankFrames.seqMissedFrames_);
    } else {
        jankFrames.seqMissedFrames_ = 0;
    }
    UpdateHitchTime(jankFrames, standardFrameTime);
}

void RSJankStats::UpdateHitchTime(JankFrames& jankFrames, float standardFrameTime)
{
    jankFrames.lastMaxHitchTime_ = jankFrames.maxHitchTime_;
    jankFrames.lastTotalHitchTimeSteady_ = jankFrames.totalHitchTimeSteady_;
    jankFrames.lastTotalFrameTimeSteadyForHTR_ = jankFrames.totalFrameTimeSteadyForHTR_;
    jankFrames.lastMaxHitchOccurenceTimeSteady_ = jankFrames.maxHitchOccurenceTimeSteady_;
    const float frameTimeForHTR = (IS_CALCULATE_PRECISE_HITCH_TIME ? GetEffectiveFrameTimeFloat(true) :
                                  static_cast<float>(GetEffectiveFrameTime(true)));
    const float frameHitchTime = std::max<float>(0.f, frameTimeForHTR - standardFrameTime);
    const bool isConsiderRsStartTimeForHTR = jankFrames.isFirstFrame_ || isFirstSetEnd_;
    const int64_t frameDurationForHTR = (isConsiderRsStartTimeForHTR ?
        (rtEndTimeSteady_ - rsStartTimeSteady_) : (rtEndTimeSteady_ - rtLastEndTimeSteady_));
    if (frameHitchTime - jankFrames.maxHitchTime_ > HITCH_TIME_EPSILON) {
        jankFrames.maxHitchOccurenceTimeSteady_ = rtEndTimeSteady_;
        jankFrames.maxHitchTime_ = frameHitchTime;
    }
    jankFrames.totalHitchTimeSteady_ += frameHitchTime;
    jankFrames.totalFrameTimeSteadyForHTR_ += frameDurationForHTR;
}

void RSJankStats::ReportJankStats()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (lastReportTime_ == TIMESTAMP_INITIAL || lastReportTimeSteady_ == TIMESTAMP_INITIAL) {
        ROSEN_LOGE("RSJankStats::ReportJankStats lastReportTime is not initialized");
        return;
    }
    int64_t reportTime = GetCurrentSystimeMs();
    int64_t reportTimeSteady = GetCurrentSteadyTimeMs();
    if (!isNeedReportJankStats_) {
        ROSEN_LOGD("RSJankStats::ReportJankStats Nothing need to report");
        lastReportTime_ = reportTime;
        lastReportTimeSteady_ = reportTimeSteady;
        lastJankFrame6FreqTimeSteady_ = TIMESTAMP_INITIAL;
        std::fill(rsJankStats_.begin(), rsJankStats_.end(), 0);
        return;
    }
    int64_t lastJankFrame6FreqTime = ((lastJankFrame6FreqTimeSteady_ == TIMESTAMP_INITIAL) ? 0 :
        (reportTime - (reportTimeSteady - lastJankFrame6FreqTimeSteady_)));
    int64_t reportDuration = reportTimeSteady - lastReportTimeSteady_;
    int64_t lastReportTime = lastReportTime_;
    std::vector<uint16_t> rsJankStats = rsJankStats_;
    RS_TRACE_NAME("RSJankStats::ReportJankStats receive notification: reportTime " + std::to_string(reportTime) +
                  ", lastJankFrame6FreqTime " + std::to_string(lastJankFrame6FreqTime));
    RSBackgroundThread::Instance().PostTask([reportDuration, lastReportTime, rsJankStats]() {
        RS_TRACE_NAME("RSJankStats::ReportJankStats in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::JANK_STATS_RS, RSEventType::RS_STATISTIC,
            "STARTTIME", static_cast<uint64_t>(lastReportTime),
            "DURATION", static_cast<uint64_t>(reportDuration), "JANK_STATS", rsJankStats, "PID", DEFAULT_INT_VALUE,
            "BUNDLE_NAME", DEFAULT_STRING_VALUE, "PROCESS_NAME", DEFAULT_STRING_VALUE,
            "VERSION_NAME", DEFAULT_STRING_VALUE, "VERSION_CODE", DEFAULT_INT_VALUE,
            "JANK_STATS_VER", JANK_RANGE_VERSION);
    });
    lastReportTime_ = reportTime;
    lastReportTimeSteady_ = reportTimeSteady;
    lastJankFrame6FreqTimeSteady_ = TIMESTAMP_INITIAL;
    std::fill(rsJankStats_.begin(), rsJankStats_.end(), 0);
    isNeedReportJankStats_ = false;
}

void RSJankStats::ReportSceneJankStats(const AppInfo& appInfo)
{
    if (lastSceneReportTime_ == TIMESTAMP_INITIAL || lastSceneReportTimeSteady_ == TIMESTAMP_INITIAL) {
        ROSEN_LOGE("RSJankStats::ReportSceneJankStats lastReportTime is not initialized");
        return;
    }
    int64_t reportTime = GetCurrentSystimeMs();
    int64_t reportTimeSteady = GetCurrentSteadyTimeMs();
    if (!isNeedReportSceneJankStats_) {
        ROSEN_LOGD("RSJankStats::ReportSceneJankStats Nothing need to report");
        lastSceneReportTime_ = reportTime;
        lastSceneReportTimeSteady_ = reportTimeSteady;
        lastSceneJankFrame6FreqTimeSteady_ = TIMESTAMP_INITIAL;
        std::fill(rsSceneJankStats_.begin(), rsSceneJankStats_.end(), 0);
        return;
    }
    int64_t lastJankFrame6FreqTime = ((lastSceneJankFrame6FreqTimeSteady_ == TIMESTAMP_INITIAL) ? 0
                                          : (reportTime - (reportTimeSteady - lastSceneJankFrame6FreqTimeSteady_)));
    int64_t reportDuration = reportTimeSteady - lastSceneReportTimeSteady_;
    int64_t lastReportTime = lastSceneReportTime_;
    std::vector<uint16_t> rsSceneJankStats = rsSceneJankStats_;
    RS_TRACE_NAME("RSJankStats::ReportSceneJankStats reportTime " + std::to_string(reportTime) +
                  ", lastJankFrame6FreqTime " + std::to_string(lastJankFrame6FreqTime));

    RSBackgroundThread::Instance().PostTask([reportDuration, lastReportTime, rsSceneJankStats, appInfo]() {
        RS_TRACE_NAME("RSJankStats::ReportSceneJankStats in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::JANK_STATS_RS, RSEventType::RS_STATISTIC,
            "STARTTIME", static_cast<uint64_t>(lastReportTime),
            "DURATION", static_cast<uint64_t>(reportDuration), "JANK_STATS", rsSceneJankStats, "PID", appInfo.pid,
            "BUNDLE_NAME", appInfo.bundleName, "PROCESS_NAME", appInfo.processName,
            "VERSION_NAME", appInfo.versionName, "VERSION_CODE", appInfo.versionCode,
            "JANK_STATS_VER", JANK_SCENE_RANGE_VERSION);
    });
    appInfo_.startTime = 0;
    appInfo_.endTime = 0;
    appInfo_.pid = 0;
    lastSceneReportTime_ = reportTime;
    lastSceneReportTimeSteady_ = reportTimeSteady;
    lastSceneJankFrame6FreqTimeSteady_ = TIMESTAMP_INITIAL;
    std::fill(rsSceneJankStats_.begin(), rsSceneJankStats_.end(), 0);
    isNeedReportSceneJankStats_ = false;
    isLastReportSceneDone_ = true;
}

void RSJankStats::ReportSceneJankFrame(uint32_t dynamicRefreshRate)
{
    if (lastSceneReportTime_ == TIMESTAMP_INITIAL || lastSceneReportTimeSteady_ == TIMESTAMP_INITIAL) {
        ROSEN_LOGE("RSJankStats::ReportSceneJankFrame lastSceneReportTime_ is not initialized");
        return;
    }

    int64_t skipFrameTime = GetEffectiveFrameTime(true);
    if (skipFrameTime < SCENE_JANK_FRAME_THRESHOLD) {
        return;
    }
    if (dynamicRefreshRate == 0) {
        dynamicRefreshRate = STANDARD_REFRESH_RATE;
    }
    const float accumulatedTime = accumulatedBufferCount_ * S_TO_MS / dynamicRefreshRate;

    RS_TRACE_NAME("RSJankStats::ReportSceneJankFrame reportTime " + std::to_string(GetCurrentSystimeMs())
        + ", skipped_frame_time: " + std::to_string(skipFrameTime)
        + ", frame_refresh_rate: " + std::to_string(dynamicRefreshRate));
    const int64_t realSkipFrameTime = static_cast<int64_t>(std::max<float>(0.f, skipFrameTime - accumulatedTime));
    RSBackgroundThread::Instance().PostTask(
        [appInfo = appInfo_, skipFrameTime, realSkipFrameTime, dynamicRefreshRate]() {
            RS_TRACE_NAME("RSJankStats::ReportSceneJankFrame in RSBackgroundThread");
            RSHiSysEvent::EventWrite(RSEventName::JANK_FRAME_RS, RSEventType::RS_FAULT,
                "PID", appInfo.pid, "BUNDLE_NAME", appInfo.bundleName,
                "PROCESS_NAME", appInfo.processName, "VERSION_NAME", appInfo.versionName,
                "VERSION_CODE", appInfo.versionCode, "FILTER_TYPE", FILTER_TYPE,
                "SKIPPED_FRAME_TIME", skipFrameTime, "REAL_SKIPPED_FRAME_TIME", realSkipFrameTime,
                "FRAME_REFRESH_RATE", static_cast<int32_t>(dynamicRefreshRate));
        });
}

void RSJankStats::SetReportEventResponse(const DataBaseRs& info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_TRACE_NAME("RSJankStats::SetReportEventResponse receive notification: " + GetSceneDescription(info));
    int64_t setTimeSteady = GetCurrentSteadyTimeMs();
    EraseIf(animateJankFrames_, [setTimeSteady](const auto& pair) -> bool {
        return setTimeSteady - pair.second.setTimeSteady_ > ANIMATION_TIMEOUT;
    });
    EraseIf(traceIdRemainder_, [setTimeSteady](const auto& pair) -> bool {
        return setTimeSteady - pair.second.setTimeSteady_ > ANIMATION_TIMEOUT;
    });
    const auto animationId = GetAnimationId(info);
    auto it = animateJankFrames_.find(animationId);
    if (it != animateJankFrames_.end()) {
        it->second.info_ = info;
        it->second.isSetReportEventResponse_ = true;
        if (it->second.isDisplayAnimator_ != info.isDisplayAnimator) {
            ROSEN_LOGW("RSJankStats::SetReportEventResponse isDisplayAnimator not consistent");
        }
        return;
    }
    for (auto &[animationId, jankFrames] : animateJankFrames_) {
        if (info.inputTime - jankFrames.info_.beginVsyncTime > INPUTTIME_TIMEOUT &&
            !jankFrames.isAnimationInterrupted_ && !jankFrames.isAnimationEnded_ &&
            jankFrames.isUpdateJankFrame_) {
            jankFrames.isAnimationInterrupted_ = true;
            RS_TRACE_NAME("RSJankStats::SetReportEventResponse " + jankFrames.info_.sceneId +
            " animation is interrupted by " + info.sceneId);
        }
    }
    JankFrames jankFrames;
    jankFrames.info_ = info;
    jankFrames.isSetReportEventResponse_ = true;
    jankFrames.setTimeSteady_ = setTimeSteady;
    jankFrames.isFirstFrame_ = true;
    jankFrames.isFirstFrameTemp_ = true;
    jankFrames.traceId_ = GetTraceIdInit(info, setTimeSteady);
    jankFrames.isDisplayAnimator_ = info.isDisplayAnimator;
    animateJankFrames_.emplace(animationId, jankFrames);
}

void RSJankStats::SetReportEventComplete(const DataBaseRs& info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_TRACE_NAME("RSJankStats::SetReportEventComplete receive notification: " + GetSceneDescription(info));
    const auto animationId = GetAnimationId(info);
    if (animateJankFrames_.find(animationId) == animateJankFrames_.end()) {
        ROSEN_LOGD("RSJankStats::SetReportEventComplete Not find exited animationId");
    } else {
        animateJankFrames_[animationId].info_ = info;
        animateJankFrames_[animationId].isSetReportEventComplete_ = true;
        if (animateJankFrames_.at(animationId).isDisplayAnimator_ != info.isDisplayAnimator) {
            ROSEN_LOGW("RSJankStats::SetReportEventComplete isDisplayAnimator not consistent");
        }
        HandleImplicitAnimationEndInAdvance(animateJankFrames_[animationId], false);
    }
}

void RSJankStats::SetReportEventJankFrame(const DataBaseRs& info, bool isReportTaskDelayed)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_TRACE_NAME("RSJankStats::SetReportEventJankFrame receive notification: " + GetSceneDescription(info));
    const auto animationId = GetAnimationId(info);
    if (animateJankFrames_.find(animationId) == animateJankFrames_.end()) {
        ROSEN_LOGD("RSJankStats::SetReportEventJankFrame Not find exited animationId");
    } else {
        animateJankFrames_[animationId].info_ = info;
        animateJankFrames_[animationId].isSetReportEventJankFrame_ = true;
        if (animateJankFrames_.at(animationId).isDisplayAnimator_ != info.isDisplayAnimator) {
            ROSEN_LOGW("RSJankStats::SetReportEventJankFrame isDisplayAnimator not consistent");
        }
        HandleImplicitAnimationEndInAdvance(animateJankFrames_[animationId], isReportTaskDelayed);
    }
}

void RSJankStats::SetReportRsSceneJankStart(const AppInfo& info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_TRACE_NAME("RSJankStats::SetReportRsSceneJankStart receive notification: " + info.processName);
    auto currentTime = GetCurrentSteadyTimeMs();
    if (!isLastReportSceneDone_ && currentTime - appInfo_.startTime > MIN_FRAME_SHOW_TIME) {
        appInfo_.endTime = currentTime;
        ReportSceneJankStats(appInfo_);
    }
    appInfo_ = info;
    appInfo_.startTime = currentTime;
    isLastReportSceneDone_ = false;
}

void RSJankStats::SetReportRsSceneJankEnd(const AppInfo& info)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RS_TRACE_NAME("RSJankStats::SetReportRsSceneJankEnd receive notification: " + info.processName);
    if (isLastReportSceneDone_ || appInfo_.pid != info.pid) {
        ROSEN_LOGE("RSJankStats::SetReportRsSceneJankEnd error scene");
        return;
    }

    appInfo_.endTime = GetCurrentSteadyTimeMs();
    ReportSceneJankStats(appInfo_);
}

void RSJankStats::HandleImplicitAnimationEndInAdvance(JankFrames& jankFrames, bool isReportTaskDelayed)
{
    if (jankFrames.isDisplayAnimator_) {
        return;
    }
    if (jankFrames.isSetReportEventJankFrame_) {
        RecordAnimationDynamicFrameRate(jankFrames, isReportTaskDelayed);
    }
    if (jankFrames.isSetReportEventComplete_ || jankFrames.isSetReportEventJankFrame_) {
        SetAnimationTraceEnd(jankFrames);
    }
    if (jankFrames.isSetReportEventComplete_) {
        ReportEventComplete(jankFrames);
    }
    if (jankFrames.isSetReportEventJankFrame_) {
        ReportEventJankFrame(jankFrames, isReportTaskDelayed);
        ReportEventHitchTimeRatio(jankFrames, isReportTaskDelayed);
    }
    jankFrames.isSetReportEventComplete_ = false;
    jankFrames.isSetReportEventJankFrame_ = false;
}

void RSJankStats::SetAppFirstFrame(pid_t appPid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    firstFrameAppPids_.push(appPid);
}

void RSJankStats::SetImplicitAnimationEnd(bool isImplicitAnimationEnd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    SetImplicitAnimationEndInner(isImplicitAnimationEnd);
}

void RSJankStats::SetImplicitAnimationEndInner(bool isImplicitAnimationEnd)
{
    if (!isImplicitAnimationEnd) {
        return;
    }
    RS_TRACE_NAME("RSJankStats::SetImplicitAnimationEnd");
    for (auto &[animationId, jankFrames] : animateJankFrames_) {
        if (jankFrames.isDisplayAnimator_) {
            continue;
        }
        SetAnimationTraceEnd(jankFrames);
    }
}

void RSJankStats::ReportEventResponse(const JankFrames& jankFrames) const
{
    const auto &info = jankFrames.info_;
    int64_t inputTime = ConvertTimeToSystime(info.inputTime);
    int64_t beginVsyncTime = ConvertTimeToSystime(info.beginVsyncTime);
    int64_t renderTime = jankFrames.traceCreateTime_;
    if (renderTime == TIMESTAMP_INITIAL) {
        renderTime = rtEndTime_;
    }
    int64_t responseLatency = renderTime - inputTime;
    RS_TRACE_NAME_FMT("RSJankStats::ReportEventResponse responseLatency is %" PRId64 "ms: %s",
                      responseLatency, GetSceneDescription(info).c_str());
    RSBackgroundThread::Instance().PostTask([info, inputTime, beginVsyncTime, responseLatency, renderTime]() {
        RS_TRACE_NAME("RSJankStats::ReportEventResponse in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::INTERACTION_RESPONSE_LATENCY, RSEventType::RS_BEHAVIOR,
            "SCENE_ID", info.sceneId, "APP_PID", info.appPid,
            "VERSION_CODE", info.versionCode, "VERSION_NAME", info.versionName, "BUNDLE_NAME", info.bundleName,
            "ABILITY_NAME", info.abilityName, "PROCESS_NAME", info.processName, "PAGE_URL", info.pageUrl,
            "SOURCE_TYPE", info.sourceType, "NOTE", info.note, "INPUT_TIME", static_cast<uint64_t>(inputTime),
            "ANIMATION_START_TIME", static_cast<uint64_t>(beginVsyncTime), "RENDER_TIME",
            static_cast<uint64_t>(renderTime), "RESPONSE_LATENCY", static_cast<uint64_t>(responseLatency));
    });
}

void RSJankStats::ReportEventComplete(const JankFrames& jankFrames) const
{
    const auto &info = jankFrames.info_;
    int64_t inputTime = ConvertTimeToSystime(info.inputTime);
    int64_t beginVsyncTime = ConvertTimeToSystime(info.beginVsyncTime);
    int64_t endVsyncTime = ConvertTimeToSystime(info.endVsyncTime);
    int64_t animationStartLatency = beginVsyncTime - inputTime;
    int64_t animationEndLatency = endVsyncTime - beginVsyncTime;
    int64_t renderTime = jankFrames.traceTerminateTime_;
    if (renderTime == TIMESTAMP_INITIAL) {
        renderTime = rtEndTime_;
    }
    int64_t completedLatency = renderTime - inputTime;
    RS_TRACE_NAME_FMT("RSJankStats::ReportEventComplete e2eLatency is %" PRId64 "ms: %s",
                      completedLatency, GetSceneDescription(info).c_str());
    RSBackgroundThread::Instance().PostTask([
        info, inputTime, animationStartLatency, animationEndLatency, completedLatency]() {
        RS_TRACE_NAME("RSJankStats::ReportEventComplete in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::INTERACTION_COMPLETED_LATENCY, RSEventType::RS_BEHAVIOR,
            "APP_PID", info.appPid, "VERSION_CODE", info.versionCode,
            "VERSION_NAME", info.versionName, "BUNDLE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName,
            "PROCESS_NAME", info.processName, "PAGE_URL", info.pageUrl, "SCENE_ID", info.sceneId,
            "SOURCE_TYPE", info.sourceType, "NOTE", info.note, "INPUT_TIME", static_cast<uint64_t>(inputTime),
            "ANIMATION_START_LATENCY", static_cast<uint64_t>(animationStartLatency), "ANIMATION_END_LATENCY",
            static_cast<uint64_t>(animationEndLatency), "E2E_LATENCY", static_cast<uint64_t>(completedLatency));
    });
}

void RSJankStats::ReportEventJankFrame(const JankFrames& jankFrames, bool isReportTaskDelayed) const
{
    if (!isReportTaskDelayed) {
        ReportEventJankFrameWithoutDelay(jankFrames);
    } else {
        ReportEventJankFrameWithDelay(jankFrames);
    }
}

void RSJankStats::ReportEventJankFrameWithoutDelay(const JankFrames& jankFrames) const
{
    if (jankFrames.totalFrames_ <= 0) {
        ROSEN_LOGD("RSJankStats::ReportEventJankFrame totalFrames is zero, nothing need to report");
        return;
    }
    const auto &info = jankFrames.info_;
    float aveFrameTimeSteady = jankFrames.totalFrameTimeSteady_ / static_cast<float>(jankFrames.totalFrames_);
    bool isReportTaskDelayed = false;
    int64_t maxFrameTimeFromStart = 0;
    int64_t maxHitchTimeFromStart = 0;
    int64_t duration = 0;
    GetMaxJankInfo(jankFrames, isReportTaskDelayed, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    RS_TRACE_NAME_FMT(
        "RSJankStats::ReportEventJankFrame maxFrameTime is %" PRId64 "ms, maxHitchTime is %" PRId64
        "ms, maxTechFrameTime is %" PRId64 "ms, maxRealFrameTime is %" PRId64 "ms,"
        "maxFrameRefreshRate is %" PRId32 "fps: %s",
        jankFrames.maxFrameTimeSteady_, static_cast<int64_t>(jankFrames.maxHitchTime_),
        jankFrames.maxTechFrameTimeSteady_, jankFrames.maxRealFrameTimeSteady_,
        jankFrames.maxFrameRefreshRate_, GetSceneDescription(info).c_str());
    RSBackgroundThread::Instance().PostTask([
        jankFrames, info, aveFrameTimeSteady, maxFrameTimeFromStart, maxHitchTimeFromStart, duration]() {
        RS_TRACE_NAME("RSJankStats::ReportEventJankFrame in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::INTERACTION_RENDER_JANK, RSEventType::RS_STATISTIC,
            "UNIQUE_ID", info.uniqueId, "SCENE_ID", info.sceneId,
            "PROCESS_NAME", info.processName, "MODULE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName,
            "PAGE_URL", info.pageUrl, "TOTAL_FRAMES", jankFrames.totalFrames_, "TOTAL_MISSED_FRAMES",
            jankFrames.totalMissedFrames_, "MAX_FRAMETIME", static_cast<uint64_t>(jankFrames.maxFrameTimeSteady_),
            "MAX_TECH_FRAMETIME", static_cast<uint64_t>(jankFrames.maxTechFrameTimeSteady_),
            "MAX_REAL_FRAMETIME", static_cast<uint64_t>(jankFrames.maxRealFrameTimeSteady_),
            "MAX_FRAMETIME_SINCE_START", static_cast<uint64_t>(maxFrameTimeFromStart), "AVERAGE_FRAMETIME",
            aveFrameTimeSteady, "MAX_SEQ_MISSED_FRAMES", jankFrames.maxSeqMissedFrames_, "IS_FOLD_DISP", IS_FOLD_DISP,
            "BUNDLE_NAME_EX", info.note, "MAX_HITCH_TIME", static_cast<uint64_t>(jankFrames.maxHitchTime_),
            "MAX_HITCH_TIME_SINCE_START", static_cast<uint64_t>(maxHitchTimeFromStart),
            "DURATION", static_cast<uint64_t>(duration),
            "MAX_FRAME_REFRESH_RATE", static_cast<int32_t>(jankFrames.maxFrameRefreshRate_));
    });
}

void RSJankStats::ReportEventJankFrameWithDelay(const JankFrames& jankFrames) const
{
    if (jankFrames.lastTotalFrames_ <= 0) {
        ROSEN_LOGD("RSJankStats::ReportEventJankFrame totalFrames is zero, nothing need to report");
        return;
    }
    const auto &info = jankFrames.info_;
    float aveFrameTimeSteady =
        jankFrames.lastTotalFrameTimeSteady_ / static_cast<float>(jankFrames.lastTotalFrames_);
    bool isReportTaskDelayed = true;
    int64_t maxFrameTimeFromStart = 0;
    int64_t maxHitchTimeFromStart = 0;
    int64_t duration = 0;
    GetMaxJankInfo(jankFrames, isReportTaskDelayed, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    RS_TRACE_NAME_FMT(
        "RSJankStats::ReportEventJankFrame maxFrameTime is %" PRId64 "ms, maxHitchTime is %" PRId64
        "ms, maxTechFrameTime is %" PRId64 "ms, maxRealFrameTime is %" PRId64 "ms,"
        "maxFrameRefreshRate is %" PRId32 "fps: %s",
        jankFrames.lastMaxFrameTimeSteady_, static_cast<int64_t>(jankFrames.lastMaxHitchTime_),
        jankFrames.lastMaxTechFrameTimeSteady_, jankFrames.lastMaxRealFrameTimeSteady_,
        jankFrames.lastMaxFrameRefreshRate_, GetSceneDescription(info).c_str());
    RSBackgroundThread::Instance().PostTask([
        jankFrames, info, aveFrameTimeSteady, maxFrameTimeFromStart, maxHitchTimeFromStart, duration]() {
        RS_TRACE_NAME("RSJankStats::ReportEventJankFrame in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::INTERACTION_RENDER_JANK, RSEventType::RS_STATISTIC,
            "UNIQUE_ID", info.uniqueId, "SCENE_ID", info.sceneId,
            "PROCESS_NAME", info.processName, "MODULE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName,
            "PAGE_URL", info.pageUrl, "TOTAL_FRAMES", jankFrames.lastTotalFrames_,
            "TOTAL_MISSED_FRAMES", jankFrames.lastTotalMissedFrames_,
            "MAX_FRAMETIME", static_cast<uint64_t>(jankFrames.lastMaxFrameTimeSteady_),
            "MAX_TECH_FRAMETIME", static_cast<uint64_t>(jankFrames.lastMaxTechFrameTimeSteady_),
            "MAX_REAL_FRAMETIME", static_cast<uint64_t>(jankFrames.lastMaxRealFrameTimeSteady_),
            "MAX_FRAMETIME_SINCE_START", static_cast<uint64_t>(maxFrameTimeFromStart),
            "AVERAGE_FRAMETIME", aveFrameTimeSteady, "MAX_SEQ_MISSED_FRAMES", jankFrames.lastMaxSeqMissedFrames_,
            "IS_FOLD_DISP", IS_FOLD_DISP, "BUNDLE_NAME_EX", info.note,
            "MAX_HITCH_TIME", static_cast<uint64_t>(jankFrames.lastMaxHitchTime_),
            "MAX_HITCH_TIME_SINCE_START", static_cast<uint64_t>(maxHitchTimeFromStart),
            "DURATION", static_cast<uint64_t>(duration),
            "MAX_FRAME_REFRESH_RATE", static_cast<int32_t>(jankFrames.lastMaxFrameRefreshRate_));
    });
}

void RSJankStats::GetMaxJankInfo(const JankFrames& jankFrames, bool isReportTaskDelayed,
    int64_t& maxFrameTimeFromStart, int64_t& maxHitchTimeFromStart, int64_t& duration) const
{
    maxFrameTimeFromStart = 0;
    maxHitchTimeFromStart = 0;
    duration = 0;
    const int64_t startTimeSteady = jankFrames.traceCreateTimeSteady_;
    const int64_t endTimeSteady = jankFrames.traceTerminateTimeSteady_;
    if (startTimeSteady == TIMESTAMP_INITIAL || endTimeSteady == TIMESTAMP_INITIAL) {
        return;
    }
    duration = std::max<int64_t>(0, endTimeSteady - startTimeSteady);
    if (!isReportTaskDelayed) {
        if (jankFrames.maxFrameOccurenceTimeSteady_ != TIMESTAMP_INITIAL) {
            maxFrameTimeFromStart = std::max<int64_t>(0, jankFrames.maxFrameOccurenceTimeSteady_ - startTimeSteady);
        }
        if (jankFrames.maxHitchOccurenceTimeSteady_ != TIMESTAMP_INITIAL) {
            maxHitchTimeFromStart = std::max<int64_t>(0, jankFrames.maxHitchOccurenceTimeSteady_ - startTimeSteady);
        }
    } else {
        if (jankFrames.lastMaxFrameOccurenceTimeSteady_ != TIMESTAMP_INITIAL) {
            maxFrameTimeFromStart = std::max<int64_t>(0, jankFrames.lastMaxFrameOccurenceTimeSteady_ - startTimeSteady);
        }
        if (jankFrames.lastMaxHitchOccurenceTimeSteady_ != TIMESTAMP_INITIAL) {
            maxHitchTimeFromStart = std::max<int64_t>(0, jankFrames.lastMaxHitchOccurenceTimeSteady_ - startTimeSteady);
        }
    }
    maxFrameTimeFromStart = std::min<int64_t>(maxFrameTimeFromStart, duration);
    maxHitchTimeFromStart = std::min<int64_t>(maxHitchTimeFromStart, duration);
}

void RSJankStats::ReportEventHitchTimeRatio(const JankFrames& jankFrames, bool isReportTaskDelayed) const
{
    if (!isReportTaskDelayed) {
        ReportEventHitchTimeRatioWithoutDelay(jankFrames);
    } else {
        ReportEventHitchTimeRatioWithDelay(jankFrames);
    }
}

void RSJankStats::ReportEventHitchTimeRatioWithoutDelay(const JankFrames& jankFrames) const
{
    if (jankFrames.totalFrameTimeSteadyForHTR_ <= 0) {
        ROSEN_LOGD("RSJankStats::ReportEventHitchTimeRatio duration is zero, nothing need to report");
        return;
    }
    const auto &info = jankFrames.info_;
    int64_t beginVsyncTime = ConvertTimeToSystime(info.beginVsyncTime);
    float hitchTimeRatio = jankFrames.totalHitchTimeSteady_ / (jankFrames.totalFrameTimeSteadyForHTR_ / S_TO_MS);
    RS_TRACE_NAME_FMT("RSJankStats::ReportEventHitchTimeRatio hitch time ratio is %g ms/s: %s",
        hitchTimeRatio, GetSceneDescription(info).c_str());
    RSBackgroundThread::Instance().PostTask([jankFrames, info, beginVsyncTime, hitchTimeRatio]() {
        RS_TRACE_NAME("RSJankStats::ReportEventHitchTimeRatio in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::INTERACTION_HITCH_TIME_RATIO, RSEventType::RS_STATISTIC,
            "UNIQUE_ID", info.uniqueId, "SCENE_ID", info.sceneId,
            "PROCESS_NAME", info.processName, "MODULE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName,
            "PAGE_URL", info.pageUrl, "UI_START_TIME", static_cast<uint64_t>(beginVsyncTime),
            "RS_START_TIME", static_cast<uint64_t>(jankFrames.startTime_), "DURATION",
            static_cast<uint64_t>(jankFrames.totalFrameTimeSteadyForHTR_), "HITCH_TIME",
            static_cast<uint64_t>(jankFrames.totalHitchTimeSteady_), "HITCH_TIME_RATIO", hitchTimeRatio,
            "IS_FOLD_DISP", IS_FOLD_DISP, "BUNDLE_NAME_EX", info.note);
    });
}

void RSJankStats::ReportEventHitchTimeRatioWithDelay(const JankFrames& jankFrames) const
{
    if (jankFrames.lastTotalFrameTimeSteadyForHTR_ <= 0) {
        ROSEN_LOGD("RSJankStats::ReportEventHitchTimeRatio duration is zero, nothing need to report");
        return;
    }
    const auto &info = jankFrames.info_;
    int64_t beginVsyncTime = ConvertTimeToSystime(info.beginVsyncTime);
    float hitchTimeRatio =
        jankFrames.lastTotalHitchTimeSteady_ / (jankFrames.lastTotalFrameTimeSteadyForHTR_ / S_TO_MS);
    RS_TRACE_NAME_FMT("RSJankStats::ReportEventHitchTimeRatio hitch time ratio is %g ms/s: %s",
        hitchTimeRatio, GetSceneDescription(info).c_str());
    RSBackgroundThread::Instance().PostTask([jankFrames, info, beginVsyncTime, hitchTimeRatio]() {
        RS_TRACE_NAME("RSJankStats::ReportEventHitchTimeRatio in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::INTERACTION_HITCH_TIME_RATIO, RSEventType::RS_STATISTIC,
            "UNIQUE_ID", info.uniqueId, "SCENE_ID", info.sceneId,
            "PROCESS_NAME", info.processName, "MODULE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName,
            "PAGE_URL", info.pageUrl, "UI_START_TIME", static_cast<uint64_t>(beginVsyncTime),
            "RS_START_TIME", static_cast<uint64_t>(jankFrames.startTime_), "DURATION",
            static_cast<uint64_t>(jankFrames.lastTotalFrameTimeSteadyForHTR_), "HITCH_TIME",
            static_cast<uint64_t>(jankFrames.lastTotalHitchTimeSteady_), "HITCH_TIME_RATIO", hitchTimeRatio,
            "IS_FOLD_DISP", IS_FOLD_DISP, "BUNDLE_NAME_EX", info.note);
    });
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
    RSBackgroundThread::Instance().PostTask([appPid]() {
        RS_TRACE_NAME("RSJankStats::ReportEventFirstFrame in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::FIRST_FRAME_DRAWN, RSEventType::RS_BEHAVIOR,
            "APP_PID", static_cast<int32_t>(appPid));
    });
}

void RSJankStats::RecordJankFrame(uint32_t dynamicRefreshRate)
{
    RS_TRACE_INT(ACCUMULATED_BUFFER_COUNT_TRACE_NAME, accumulatedBufferCount_);
    if (dynamicRefreshRate == 0) {
        dynamicRefreshRate = STANDARD_REFRESH_RATE;
    }
    const float accumulatedTime = accumulatedBufferCount_ * S_TO_MS / dynamicRefreshRate;
    for (auto& recordStats : jankExplicitAnimatorFrameRecorder_) {
        recordStats.isRecorded_ = false;
    }
    const int64_t missedFramesByDuration = static_cast<int64_t>(
        std::max<float>(0.f, GetEffectiveFrameTime(true) - accumulatedTime) / VSYNC_PERIOD);
    if (missedFramesByDuration > 0 && explicitAnimationTotal_ > 0) {
        for (auto& recordStats : jankExplicitAnimatorFrameRecorder_) {
            RecordJankFrameSingle(missedFramesByDuration, recordStats);
        }
    }
    for (auto& recordStats : jankImplicitAnimatorFrameRecorder_) {
        recordStats.isRecorded_ = false;
    }
    const int64_t missedFramesByInterval = static_cast<int64_t>(
        std::max<float>(0.f, GetEffectiveFrameTime(isFirstSetEnd_) - accumulatedTime) / VSYNC_PERIOD);
    if (missedFramesByInterval > 0 && implicitAnimationTotal_ > 0) {
        for (auto& recordStats : jankImplicitAnimatorFrameRecorder_) {
            RecordJankFrameSingle(missedFramesByInterval, recordStats);
        }
    }
    RS_TRACE_INT(ACCUMULATED_BUFFER_COUNT_TRACE_NAME, 0);
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

void RSJankStats::RecordAnimationDynamicFrameRate(JankFrames& jankFrames, bool isReportTaskDelayed)
{
    if (jankFrames.isFrameRateRecorded_) {
        return;
    }
    const int32_t traceId = jankFrames.traceId_;
    if (traceId == TRACE_ID_INITIAL) {
        ROSEN_LOGE("RSJankStats::RecordAnimationDynamicFrameRate traceId not initialized");
        return;
    }
    if (!isReportTaskDelayed) {
        if (animationAsyncTraces_.find(traceId) == animationAsyncTraces_.end() || jankFrames.totalFrames_ <= 0 ||
            jankFrames.totalFrameTimeSteadyForHTR_ <= 0) {
            return;
        }
        float animationDuration = static_cast<float>(jankFrames.totalFrameTimeSteadyForHTR_) / S_TO_MS;
        float animationTotalFrames = static_cast<float>(jankFrames.totalFrames_);
        float animationDynamicFrameRate = animationTotalFrames / animationDuration;
        RS_TRACE_NAME_FMT("RSJankStats::RecordAnimationDynamicFrameRate frame rate is %.2f: %s",
                          animationDynamicFrameRate, animationAsyncTraces_.at(traceId).traceName_.c_str());
    } else {
        if (animationAsyncTraces_.find(traceId) == animationAsyncTraces_.end() || jankFrames.lastTotalFrames_ <= 0 ||
            jankFrames.lastTotalFrameTimeSteadyForHTR_ <= 0) {
            return;
        }
        float animationDuration = static_cast<float>(jankFrames.lastTotalFrameTimeSteadyForHTR_) / S_TO_MS;
        float animationTotalFrames = static_cast<float>(jankFrames.lastTotalFrames_);
        float animationDynamicFrameRate = animationTotalFrames / animationDuration;
        RS_TRACE_NAME_FMT("RSJankStats::RecordAnimationDynamicFrameRate frame rate is %.2f: %s",
                          animationDynamicFrameRate, animationAsyncTraces_.at(traceId).traceName_.c_str());
    }
    jankFrames.isFrameRateRecorded_ = true;
}

bool RSJankStats::GetEarlyZEnableFlag()
{
    std::lock_guard<std::mutex> lock(mutex_);
    isFlushEarlyZ_ = false;
    return ddgrEarlyZEnableFlag_;
}

bool RSJankStats::GetFlushEarlyZ()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return isFlushEarlyZ_;
}

void RSJankStats::SetAnimationTraceBegin(std::pair<int64_t, std::string> animationId, JankFrames& jankFrames)
{
    jankFrames.isUpdateJankFrame_ = true;
    if (jankFrames.isAnimationEnded_) {
        return;
    }
    const int32_t traceId = jankFrames.traceId_;
    if (traceId == TRACE_ID_INITIAL) {
        ROSEN_LOGE("RSJankStats::SetAnimationTraceBegin traceId not initialized");
        return;
    }
    if (animationAsyncTraces_.find(traceId) != animationAsyncTraces_.end()) {
        return;
    }
    jankFrames.traceCreateTime_ = rtEndTime_;
    jankFrames.traceCreateTimeSteady_ = rtEndTimeSteady_;
    const auto &info = jankFrames.info_;
    const std::string traceName = GetSceneDescription(info);
    AnimationTraceStats traceStat = {.animationId_ = animationId,
                                     .traceName_ = traceName,
                                     .traceCreateTimeSteady_ = rtEndTimeSteady_,
                                     .isDisplayAnimator_ = info.isDisplayAnimator};
    animationAsyncTraces_.emplace(traceId, traceStat);
    if (info.isDisplayAnimator) {
        explicitAnimationTotal_++;
    } else {
        implicitAnimationTotal_++;
    }
    RS_ASYNC_TRACE_BEGIN(traceName, traceId);
    if (RSSystemProperties::GetEarlyZEnable() && info.sceneId == SWITCH_SCENE_NAME) {
        ddgrEarlyZEnableFlag_ = true;
        isFlushEarlyZ_ = true;
        lastReportEarlyZTraceId_ = traceId;
    }
}

void RSJankStats::SetAnimationTraceEnd(JankFrames& jankFrames)
{
    jankFrames.isUpdateJankFrame_ = false;
    jankFrames.isAnimationEnded_ = true;
    const int32_t traceId = jankFrames.traceId_;
    if (traceId == TRACE_ID_INITIAL) {
        ROSEN_LOGE("RSJankStats::SetAnimationTraceEnd traceId not initialized");
        return;
    }
    if (animationAsyncTraces_.find(traceId) == animationAsyncTraces_.end()) {
        return;
    }
    jankFrames.traceTerminateTime_ = rtEndTime_;
    jankFrames.traceTerminateTimeSteady_ = rtEndTimeSteady_;
    const bool isDisplayAnimator = animationAsyncTraces_.at(traceId).isDisplayAnimator_;
    RS_ASYNC_TRACE_END(animationAsyncTraces_.at(traceId).traceName_, traceId);
    if (ddgrEarlyZEnableFlag_ && lastReportEarlyZTraceId_ == traceId) {
        ddgrEarlyZEnableFlag_ = false;
        isFlushEarlyZ_ = true;
        lastReportEarlyZTraceId_ = traceId;
    }
    animationAsyncTraces_.erase(traceId);
    if (isDisplayAnimator) {
        explicitAnimationTotal_--;
    } else {
        implicitAnimationTotal_--;
    }
}

void RSJankStats::CheckAnimationTraceTimeout()
{
    if (++animationTraceCheckCnt_ < ANIMATION_TRACE_CHECK_FREQ) {
        return;
    }
    EraseIf(animationAsyncTraces_, [this](const auto& pair) -> bool {
        bool needErase = rtEndTimeSteady_ - pair.second.traceCreateTimeSteady_ > ANIMATION_TIMEOUT;
        if (needErase) {
            RS_ASYNC_TRACE_END(pair.second.traceName_, pair.first);
            if (pair.second.isDisplayAnimator_) {
                explicitAnimationTotal_--;
            } else {
                implicitAnimationTotal_--;
            }
            animateJankFrames_.erase(pair.second.animationId_);
        }
        return needErase;
    });
    animationTraceCheckCnt_ = 0;
}

void RSJankStats::ClearAllAnimation()
{
    RS_TRACE_NAME("RSJankStats::ClearAllAnimation");
    EraseIf(animationAsyncTraces_, [](const auto& pair) -> bool {
        RS_ASYNC_TRACE_END(pair.second.traceName_, pair.first);
        return true;
    });
    explicitAnimationTotal_ = 0;
    implicitAnimationTotal_ = 0;
    animateJankFrames_.clear();
}

std::string RSJankStats::GetSceneDescription(const DataBaseRs& info) const
{
    std::stringstream sceneDescription;
    int64_t inputTime = ConvertTimeToSystime(info.inputTime);
    std::string animatorType = (info.isDisplayAnimator ? "EXPLICIT_ANIMATOR" : "IMPLICIT_ANIMATOR");
    sceneDescription << info.sceneId << ", " << info.bundleName << ", " << info.pageUrl
                     << ", " << inputTime << ", " << animatorType << ", uid" << info.uniqueId;
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
    if (traceIdRemainder_.at(info.uniqueId).remainder_ >= TRACE_ID_SCALE_PARAM) {
        traceIdRemainder_[info.uniqueId].remainder_ = 0;
    }
    traceIdRemainder_[info.uniqueId].setTimeSteady_ = setTimeSteady;
    int64_t mappedUniqueId = info.uniqueId * TRACE_ID_SCALE_PARAM + (traceIdRemainder_[info.uniqueId].remainder_++);
    int32_t traceId = static_cast<int32_t>(mappedUniqueId);
    return traceId;
}

void RSJankStats::AvcodecVideoDump(
    std::string& dumpString, std::string& type, const std::string& avcodecVideo)
{
    std::string func;
    uint64_t uniqueId;
    std::string surfaceName = "";
    uint32_t fps = 0;
    uint64_t reportTime = UINT64_MAX;

    RS_LOGD("AvcodecVideoDump start. type %{public}s", type.substr(avcodecVideo.length()).c_str());
    cJSON* jsonObject = cJSON_Parse(type.substr(avcodecVideo.length()).c_str());
    if (jsonObject == nullptr) {
        dumpString.append("AvcodecVideoDump can not parse type to json.\n");
        cJSON_Delete(jsonObject);
        return;
    }

    cJSON* jsonItemFunc = cJSON_GetObjectItem(jsonObject, "func");
    if (jsonItemFunc != nullptr && cJSON_IsString(jsonItemFunc)) {
        func = jsonItemFunc->valuestring;
    }

    cJSON* jsonItemUniqueId = cJSON_GetObjectItem(jsonObject, "uniqueId");
    if (jsonItemUniqueId != nullptr && cJSON_IsString(jsonItemUniqueId)) {
        std::string strM = jsonItemUniqueId->valuestring;
        char* end = nullptr;
        errno = 0;
        long long sizeM = std::strtoll(strM.c_str(), &end, 10);
        if (end != nullptr && end != strM.c_str() && errno == 0 && *end == '\0' && sizeM > 0) {
            uniqueId = static_cast<uint64_t>(sizeM);
        } else {
            dumpString.append("AvcodecVideoDump param error : uniqueId.\n");
            cJSON_Delete(jsonObject);
            return;
        }
    } else {
        dumpString.append("AvcodecVideoDump param error : uniqueId.\n");
        cJSON_Delete(jsonObject);
        return;
    }

    cJSON* jsonItemSurface = cJSON_GetObjectItem(jsonObject, "surfaceName");
    if (jsonItemSurface != nullptr && cJSON_IsString(jsonItemSurface)) {
        surfaceName = jsonItemSurface->valuestring;
    }

    cJSON* jsonItemFps = cJSON_GetObjectItem(jsonObject, "fps");
    if (jsonItemFps != nullptr && cJSON_IsNumber(jsonItemFps)) {
        fps = static_cast<uint32_t>(jsonItemFps->valueint);
    }

    if (func == "start") {
        cJSON* jsonItemReportTime = cJSON_GetObjectItem(jsonObject, "reportTime");
        if (jsonItemReportTime != nullptr && cJSON_IsNumber(jsonItemReportTime)) {
            reportTime = static_cast<uint64_t>(jsonItemReportTime->valueint);
        } else {
            dumpString.append("AvcodecVideoDump param error : reportTime.\n");
            cJSON_Delete(jsonObject);
            return;
        }

        AvcodecVideoStart(uniqueId, surfaceName, fps, reportTime);
        dumpString.append("AvcodecVideoStart ");
    } else {
        AvcodecVideoStop(uniqueId, surfaceName, fps);
        dumpString.append("AvcodecVideoStop ");
    }
    cJSON_Delete(jsonObject);
    dumpString.append("[uniqueId:" + std::to_string(uniqueId) + ", surfaceName:" + surfaceName +
        ", fps:" + std::to_string(fps) + ", reportTime:" + std::to_string(reportTime) + "]\n");
}

void RSJankStats::AvcodecVideoStart(
    const uint64_t queueId, const std::string& surfaceName, const uint32_t fps, const uint64_t reportTime)
{
    if (avcodecVideoMap_.find(queueId) != avcodecVideoMap_.end()) {
        RS_LOGE("AvcodecVideoStart mission exists. %{public}" PRIu64 ".", queueId);
        return;
    }

    AvcodecVideoParam& info = avcodecVideoMap_[queueId];
    info.surfaceName = surfaceName;
    info.fps = fps;
    info.reportTime = reportTime;
    info.startTime = static_cast<uint64_t>(GetCurrentSystimeMs());
    info.decodeCount = 0;

    avcodecVideoCollectOpen_ = true;
    RS_TRACE_NAME_FMT("AvcodecVideoStart [queueId %lu, surfaceName %s, fps %u, reportTime %lu, startTime %lu]",
        queueId, info.surfaceName.c_str(), info.fps, info.reportTime, info.startTime);
}

void RSJankStats::AvcodecVideoStop(const uint64_t queueId, const std::string& surfaceName, const uint32_t fps)
{
    auto it = avcodecVideoMap_.find(queueId);
    if (it == avcodecVideoMap_.end()) {
        RS_LOGE("AvcodecVideoStop mission does not exist. %{public}" PRIu64 ".", queueId);
        return;
    }

    uint64_t duration = static_cast<uint64_t>(GetCurrentSystimeMs()) - it->second.startTime;
    RS_TRACE_NAME_FMT("AvcodecVideoStop mission complete. NotifyVideoEventToXperf" \
        "[uniqueId %lu, duration %lu, avgFps %lu]",
        queueId, duration, it->second.decodeCount * DELAY_TIME_MS / duration);
    RS_LOGD("AvcodecVideoStop mission complete. NotifyVideoEventToXperf" \
        "[uniqueId %{public}" PRIu64 ", duration %{public}" PRIu64 ", avgFps %{public}" PRIu64 "]",
        queueId, duration, it->second.decodeCount * DELAY_TIME_MS / duration);

    avcodecVideoMap_.erase(it);

    if (avcodecVideoMap_.empty()) {
        avcodecVideoCollectOpen_ = false;
    }
}

void RSJankStats::AvcodecVideoCollectBegin()
{
    if (!avcodecVideoCollectOpen_) {
        return;
    }

    for (auto& [queueId, info] : avcodecVideoMap_) {
        info.continuousFrameLoss++;
    }
}

void RSJankStats::AvcodecVideoCollectFinish()
{
    if (!avcodecVideoCollectOpen_) {
        return;
    }

    std::vector<uint64_t> finishedVideos;
    for (auto it = avcodecVideoMap_.begin(); it != avcodecVideoMap_.end(); it++) {
        if (it->second.continuousFrameLoss >= ACVIDEO_QUIT_FRAME_COUNT) {
            RS_LOGD("AvcodecVideoCollectFinish. [uniqueId %{public}" PRIu64 "]", it->first);
            finishedVideos.push_back(it->first);
        }
    }
    for (auto queueId : finishedVideos) {
        AvcodecVideoStop(queueId);
    }
}

void RSJankStats::AvcodecVideoCollect(const uint64_t queueId, const uint32_t sequence)
{
    if (!avcodecVideoCollectOpen_) {
        return;
    }

    auto it = avcodecVideoMap_.find(queueId);
    if (it == avcodecVideoMap_.end()) {
        return;
    }

    uint64_t now = static_cast<uint64_t>(GetCurrentSystimeMs());
    if (it->second.previousSequence == 0) {
        // first sequence
        it->second.decodeCount++;
        it->second.previousSequence = sequence;
        it->second.previousFrameTime = now;
        it->second.continuousFrameLoss = 0;
    } else if (it->second.previousSequence != sequence) {
        uint64_t frameTime = now - it->second.previousFrameTime;
        if (frameTime > it->second.reportTime) {
            RSBackgroundThread::Instance().PostTask([queueId, frameTime]() {
                RS_TRACE_NAME_FMT("AvcodecVideoCollect NotifyVideoFaultToXperf [uniqueId %lu, frameTime %lu]",
                    queueId, frameTime);
                RS_LOGD("AvcodecVideoCollect NotifyVideoFaultToXperf" \
                    "[uniqueId %{public}" PRIu64 ", frameTime %{public}" PRIu64 "]",
                    queueId, frameTime);
            });
        }
        it->second.decodeCount++;
        it->second.previousSequence = sequence;
        it->second.previousFrameTime = now;
        it->second.continuousFrameLoss = 0;
    }
}

int64_t RSJankStats::GetEffectiveFrameTime(bool isConsiderRsStartTime) const
{
    if (isConsiderRsStartTime) {
        return std::min<int64_t>(rtEndTimeSteady_ - rtLastEndTimeSteady_, rtEndTimeSteady_ - rsStartTimeSteady_);
    }
    if (isCurrentFrameSwitchToNotDoDirectComposition_) {
        return rtEndTimeSteady_ - rsStartTimeSteady_;
    }
    return rtEndTimeSteady_ - rtLastEndTimeSteady_;
}

float RSJankStats::GetEffectiveFrameTimeFloat(bool isConsiderRsStartTime) const
{
    if (isConsiderRsStartTime) {
        return std::min<float>(rtEndTimeSteadyFloat_ - rtLastEndTimeSteadyFloat_,
                               rtEndTimeSteadyFloat_ - rsStartTimeSteadyFloat_);
    }
    if (isCurrentFrameSwitchToNotDoDirectComposition_) {
        return rtEndTimeSteadyFloat_ - rsStartTimeSteadyFloat_;
    }
    return rtEndTimeSteadyFloat_ - rtLastEndTimeSteadyFloat_;
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

float RSJankStats::GetCurrentSteadyTimeMsFloat() const
{
    auto curTime = std::chrono::steady_clock::now().time_since_epoch();
    int64_t curSteadyTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(curTime).count();
    float curSteadyTime = curSteadyTimeUs / MS_TO_US;
    return curSteadyTime;
}
} // namespace Rosen
} // namespace OHOS
