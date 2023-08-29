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
constexpr float VSYNC_PERIOD = 16.6;                 // 16.6ms
constexpr int64_t TRACE_TIMEOUT = 5000;              // 5s
constexpr int64_t JANK_TIMEOUT = 10000;              // 10s
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
    if (isfirstSetStart_) {
        lastReportTime_ = startTime_;
        isfirstSetStart_ = false;
    }
    {
        std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
        for (auto &[uniqueId, jankFrames] : animateJankFrames_) {
            jankFrames.isReportEventResponse_ = jankFrames.isSetReportEventResponse_;
            jankFrames.isSetReportEventResponse_ = false;
            jankFrames.isReportEventComplete_ = jankFrames.isSetReportEventComplete_;
            jankFrames.isSetReportEventComplete_ = false;
            jankFrames.isReportEventJankFrame_ = jankFrames.isSetReportEventJankFrame_;
            jankFrames.isSetReportEventJankFrame_ = false;
        }
    }
}

void RSJankStats::SetEndTime()
{
    if (startTime_ == 0) {
        ROSEN_LOGE("RSJankStats::SetEndTime startTime is not initialized");
        return;
    }
    endTime_ = GetCurrentSystimeMs();
    const int64_t duration = endTime_ - startTime_;
    if (duration >= VSYNC_PERIOD) {
        SetRSJankStats(static_cast<int32_t>(duration / VSYNC_PERIOD));
    }
    {
        std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
        for (auto &[uniqueId, jankFrames] : animateJankFrames_) {
            if (jankFrames.isReportEventResponse_) {
                ReportEventResponse(jankFrames);
                jankFrames.isUpdateJankFrame_ = true;
            }
            if (jankFrames.isUpdateJankFrame_) {
                UpdateJankFrame(duration, jankFrames);
            }
            if (jankFrames.isReportEventComplete_) {
                ReportEventComplete(jankFrames);
            }
            if (jankFrames.isReportEventJankFrame_) {
                ReportEventJankFrame(jankFrames);
            }
            const TraceId traceId = static_cast<TraceId>(uniqueId);
            if (jankFrames.isReportEventResponse_) {
                SetTraceBegin(traceId, jankFrames, endTime_);
            }
            if (jankFrames.isReportEventComplete_ || jankFrames.isReportEventJankFrame_) {
                SetTraceEnd(traceId);
                jankFrames.isUpdateJankFrame_ = false;
            }
        }
    }
    if (isFirstFrame_) {
        ReportEventFirstFrame();
    }
    CheckTraceTimeout(endTime_);
}

void RSJankStats::SetRSJankStats(int32_t times)
{
    size_t type = JANK_FRAME_INVALID;
    if (times < 6) {                                              // JANK_FRAME_6_FREQ   : (0,6)
        type = JANK_FRAME_6_FREQ;
    } else if (times < 15) {                                      // JANK_FRAME_15_FREQ  : [6,15)
        type = JANK_FRAME_15_FREQ;
    } else if (times < 20) {                                      // JANK_FRAME_20_FREQ  : [15,20)
        type = JANK_FRAME_20_FREQ;
    } else if (times < 36) {                                      // JANK_FRAME_36_FREQ  : [20,36)
        type = JANK_FRAME_36_FREQ;
    } else if (times < 48) {                                      // JANK_FRAME_48_FREQ  : [36,48)
        type = JANK_FRAME_48_FREQ;
    } else if (times < 60) {                                      // JANK_FRAME_60_FREQ  : [48,60)
        type = JANK_FRAME_60_FREQ;
    } else if (times < 120) {                                     // JANK_FRAME_120_FREQ : [60,120)
        type = JANK_FRAME_120_FREQ;
    } else if (times < 180) {                                     // JANK_FRAME_180_FREQ : [120,180)
        type = JANK_FRAME_180_FREQ;
    } else {
        ROSEN_LOGW("RSInterfaces::SetJankStatas Jank Frame Skip more than 180");
        return;
    }
    if (rsJankStats_[type] != USHRT_MAX) {
        rsJankStats_[type]++;
    }
    isNeedReport_ = true;
}

void RSJankStats::UpdateJankFrame(int64_t duration, JankFrames& jankFrames)
{
    jankFrames.totalFrames_++;
    jankFrames.totalFrameTime_ += duration;
    jankFrames.maxFrameTime_ = std::max(jankFrames.maxFrameTime_, duration);
    if (duration >= VSYNC_PERIOD) {
        int32_t missedFramed = static_cast<int32_t>(duration / VSYNC_PERIOD);
        jankFrames.totalMissedFrames_ += missedFramed;
        jankFrames.seqMissedFrames_ =
            jankFrames.seqMissedFrames_ ? (jankFrames.seqMissedFrames_ + missedFramed) : missedFramed;
        jankFrames.maxSeqMissedFrames_ =
            std::max(jankFrames.maxSeqMissedFrames_, jankFrames.seqMissedFrames_);
    } else {
        jankFrames.seqMissedFrames_ = 0;
    }
}

void RSJankStats::ReportJankStats()
{
    if (!isNeedReport_) {
        ROSEN_LOGW("RSJankStats::ReportJankStats Nothing need to report");
        return;
    }
    if (lastReportTime_ == 0) {
        ROSEN_LOGE("RSJankStats::ReportJankStats lastReportTime is not initialized");
        return;
    }
    int64_t reportTime = GetCurrentSystimeMs();
    int64_t reportDuration = reportTime - lastReportTime_;
    auto reportName = "JANK_STATS_RS";
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "STARTTIME", lastReportTime_, "DURATION", reportDuration,
        "JANK_STATS", rsJankStats_, "JANK_STATS_VER", 1);
    std::fill(rsJankStats_.begin(), rsJankStats_.end(), 0);
    lastReportTime_ = reportTime;
    isNeedReport_ = false;
}

void RSJankStats::SetReportEventResponse(DataBaseRs info)
{
    int64_t setTime = GetCurrentSystimeMs();
    std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
    EraseIf(animateJankFrames_, [setTime](const auto& pair) -> bool {
        return setTime - pair.second.setTime_ > JANK_TIMEOUT;
    });
    if (animateJankFrames_.find(info.uniqueId) == animateJankFrames_.end()) {
        JankFrames jankFrames;
        jankFrames.info_ = info;
        jankFrames.isSetReportEventResponse_ = true;
        jankFrames.setTime_ = setTime;
        animateJankFrames_.emplace(info.uniqueId, jankFrames);
    } else {
        animateJankFrames_[info.uniqueId].info_ = info;
        animateJankFrames_[info.uniqueId].isSetReportEventResponse_ = true;
    }
}

void RSJankStats::SetReportEventComplete(DataBaseRs info)
{
    int64_t setTime = GetCurrentSystimeMs();
    std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
    if (animateJankFrames_.find(info.uniqueId) == animateJankFrames_.end()) {
        ROSEN_LOGW("RSJankStats::SetReportEventComplete Not find exited uniqueId");
        JankFrames jankFrames;
        jankFrames.info_ = info;
        jankFrames.isSetReportEventComplete_ = true;
        jankFrames.setTime_ = setTime;
        animateJankFrames_.emplace(info.uniqueId, jankFrames);
    } else {
        animateJankFrames_[info.uniqueId].info_ = info;
        animateJankFrames_[info.uniqueId].isSetReportEventComplete_ = true;
    }
}

void RSJankStats::SetReportEventJankFrame(DataBaseRs info)
{
    std::lock_guard<std::mutex> lock(animateJankFramesMutex_);
    if (animateJankFrames_.find(info.uniqueId) == animateJankFrames_.end()) {
        ROSEN_LOGW("RSJankStats::SetReportEventJankFrame Not find exited uniqueId");
    } else {
        animateJankFrames_[info.uniqueId].info_ = info;
        animateJankFrames_[info.uniqueId].isSetReportEventJankFrame_ = true;
    }
}

void RSJankStats::SetFirstFrame()
{
    isFirstFrame_ = true;
}

void RSJankStats::SetPid(pid_t pid)
{
    appPid_ = pid;
}

void RSJankStats::ReportEventResponse(const JankFrames& jankFrames) const
{
    auto reportName = "INTERACTION_RESPONSE_LATENCY";
    const auto &info = jankFrames.info_;
    int64_t inputTime = ConvertTimeToSystime(info.inputTime);
    int64_t beginVsyncTime = ConvertTimeToSystime(info.beginVsyncTime);
    int64_t responseLatency = endTime_ - inputTime;
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "APP_PID", info.appPid, "VERSION_CODE", info.versionCode,
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
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "APP_PID", info.appPid, "VERSION_CODE", info.versionCode,
        "VERSION_NAME", info.versionName, "BUNDLE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName,
        "PROCESS_NAME", info.processName, "PAGE_URL", info.pageUrl, "SCENE_ID", info.sceneId,
        "SOURCE_TYPE", info.sourceType, "NOTE", info.note, "INPUT_TIME", static_cast<uint64_t>(inputTime),
        "ANIMATION_START_LATENCY", static_cast<uint64_t>(animationStartLatency), "ANIMATION_END_LATENCY",
        static_cast<uint64_t>(animationEndLatency), "E2E_LATENCY", static_cast<uint64_t>(completedLatency));
}

void RSJankStats::ReportEventJankFrame(const JankFrames& jankFrames) const
{
    auto reportName = "INTERACTION_RENDER_JANK";
    float aveFrameTime = jankFrames.totalFrameTime_ / static_cast<float>(jankFrames.totalFrames_);
    const auto &info = jankFrames.info_;
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "UNIQUE_ID", info.uniqueId, "SCENE_ID", info.sceneId,
        "PROCESS_NAME", info.processName, "MODULE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName, "PAGE_URL",
        info.pageUrl, "TOTAL_FRAMES", jankFrames.totalFrames_, "TOTAL_MISSED_FRAMES", jankFrames.totalMissedFrames_,
        "MAX_FRAMETIME", static_cast<uint64_t>(jankFrames.maxFrameTime_), "AVERAGE_FRAMETIME", aveFrameTime,
        "MAX_SEQ_MISSED_FRAMES", jankFrames.maxSeqMissedFrames_, "IS_FOLD_DISP", 0);
}

void RSJankStats::ReportEventFirstFrame()
{
    auto reportName = "FIRST_FRAME_DRAWN";
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "APP_PID", appPid_);
    isFirstFrame_ = false;
}

void RSJankStats::SetTraceBegin(const TraceId traceId, const JankFrames& jankFrames, int64_t createTime)
{
    if (aSyncTraces_.find(traceId) != aSyncTraces_.end()) {
        return;
    }
    const auto &info = jankFrames.info_;
    int64_t inputTime = ConvertTimeToSystime(info.inputTime);
    std::stringstream traceNameStream;
    traceNameStream << info.sceneId << ", " << info.bundleName << ", " << info.pageUrl << ", " << inputTime << ".";
    const std::string traceName = traceNameStream.str();
    TraceStats traceStat = {.traceName_ = traceName, .createTime_ = createTime};
    aSyncTraces_[traceId] = std::move(traceStat);
    RS_ASYNC_TRACE_BEGIN(traceName, traceId);
}

void RSJankStats::SetTraceEnd(const TraceId traceId)
{
    if (aSyncTraces_.find(traceId) == aSyncTraces_.end()) {
        return;
    }
    RS_ASYNC_TRACE_END(aSyncTraces_[traceId].traceName_, traceId);
    aSyncTraces_.erase(traceId);
}

void RSJankStats::CheckTraceTimeout(int64_t checkEraseTime)
{
    if (++traceCheckCnt_ < TRACE_CHECK_FREQ) {
        return;
    }
    EraseIf(aSyncTraces_, [checkEraseTime](const auto& pair) -> bool {
        bool needErase = checkEraseTime - pair.second.createTime_ > TRACE_TIMEOUT;
        if (needErase) {
            RS_ASYNC_TRACE_END(pair.second.traceName_, pair.first);
        }
        return needErase;
    });
    traceCheckCnt_ = 0;
}

int64_t RSJankStats::ConvertTimeToSystime(int64_t time) const
{
    if (time == 0) {
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

} // namespace Rosen
} // namespace OHOS
