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
#include <sys/time.h>
#include <unistd.h>

#include "hisysevent.h"
#include "rs_trace.h"

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float VSYNC_PERIOD = 16.6;                  // 16.6ms
constexpr uint32_t JANK_TIMEOUT = 10000;              // 10s
constexpr uint32_t S_TO_NS = 1000000000;              // ms to ns
}

RSJankStats& RSJankStats::GetInstance()
{
    static RSJankStats instance;
    return instance;
}

void RSJankStats::SetStartTime()
{
    for (auto &[uniqueId, jankFrames] : animateJankFrames_) {
        jankFrames.isReportEventResponse_ = jankFrames.isSetReportEventResponse_;
        jankFrames.isReportEventComplete_ = jankFrames.isSetReportEventComplete_;
    }
    startTime_ = GetCurrentSystimeMs();
    if (isfirstSetStart_) {
        lastReportTime_ = startTime_;
        isfirstSetStart_ = false;
    }
}

void RSJankStats::SetEndTime()
{
    endTime_ = GetCurrentSystimeMs();
    auto duration = endTime_ - startTime_;
    if (duration >= VSYNC_PERIOD) {
        SetRSJankStats(static_cast<int>(duration / VSYNC_PERIOD));
    }
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
        if (jankFrames.isSetReportEventJankFrame_) {
            ReportEventJankFrame(jankFrames);
        }
    }
    if (isFirstFrame_) {
        ReportEventFirstFrame();
    }
}

void RSJankStats::SetRSJankStats(int times)
{
    auto type = JankRangeType::JANK_FRAME_INVALID;
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

void RSJankStats::UpdateJankFrame(uint64_t duration, JankFrames& jankFrames)
{
    jankFrames.totalFrames_++;
    jankFrames.totalFrameTime_ += duration;
    jankFrames.maxFrameTime_ = std::max(jankFrames.maxFrameTime_, duration);
    if (duration >= VSYNC_PERIOD) {
        auto missedFramed = static_cast<int>(duration / VSYNC_PERIOD);
        jankFrames.totalMissedFrames_ += missedFramed;
        jankFrames.SeqMissedFrames =
            jankFrames.SeqMissedFrames ? (jankFrames.SeqMissedFrames + missedFramed) : missedFramed;
        jankFrames.maxSeqMissedFrames_ =
            std::max(jankFrames.maxSeqMissedFrames_, jankFrames.SeqMissedFrames);
    } else {
        jankFrames.SeqMissedFrames = 0;
    }
}

void RSJankStats::ReportJankStats()
{
    if (!isNeedReport_) {
        ROSEN_LOGW("RSJankStats::ReportJankStats Nothing need to report");
        return;
    }
    auto reportTime = GetCurrentSystimeMs();
    auto reportDuration = reportTime - lastReportTime_;
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
    auto setTime = GetCurrentSystimeMs();
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
    auto setTime = GetCurrentSystimeMs();
    if (animateJankFrames_.find(info.uniqueId) == animateJankFrames_.end()) {
        ROSEN_LOGW("RSJankStats::SetReportEventComplete Not find exited uniqueId");
        JankFrames jankFrames;
        jankFrames.info_ = info;
        jankFrames.isSetReportEventResponse_ = true;
        jankFrames.setTime_ = setTime;
        animateJankFrames_.emplace(info.uniqueId, jankFrames);
    } else {
        animateJankFrames_[info.uniqueId].info_ = info;
        animateJankFrames_[info.uniqueId].isSetReportEventComplete_ = true;
    }
}

void RSJankStats::SetReportEventJankFrame(DataBaseRs info)
{
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

void RSJankStats::ReportEventResponse(JankFrames& jankFrames)
{
    auto reportName = "INTERACTION_RESPONSE_LATENCY";
    auto info = jankFrames.info_;
    auto inputTime = ConvertTimeToSystime(info.inputTime);
    auto beginVsyncTime = ConvertTimeToSystime(info.beginVsyncTime);
    auto responseLatency = endTime_ - inputTime;
    RS_TRACE_NAME_FMT("RSJankStats::ReportEventResponse pkgname: %s, pageurl: %s, sceneid: %s, inputTime:% " PRId64 ".",
        info.bundleName.c_str(), info.pageUrl.c_str(), info.pageUrl.c_str(), info.inputTime);
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "APP_PID", info.appPid, "BUNDLE_NAME", info.bundleName,
        "ABILITY_NAME", info.abilityName, "PROCESS_NAME", info.processName, "PAGE_URL", info.pageUrl, "SCENE_ID",
        info.sceneId, "INPUT_TIME", inputTime, "ANIMATION_START_TIME", beginVsyncTime, "RENDER_TIME", endTime_,
        "RESPONSE_LATENCY", responseLatency);
    jankFrames.isSetReportEventResponse_ = false;
}

void RSJankStats::ReportEventComplete(JankFrames& jankFrames)
{
    auto reportName = "INTERACTION_COMPLETED_LATENCY";
    auto info = jankFrames.info_;
    auto inputTime = ConvertTimeToSystime(info.inputTime);
    auto beginVsyncTime = ConvertTimeToSystime(info.beginVsyncTime);
    auto endVsyncTime = ConvertTimeToSystime(info.endVsyncTime);
    auto animationStartLatency = beginVsyncTime - inputTime;
    auto animationEndLatency = endVsyncTime - beginVsyncTime;
    auto completedLatency = endTime_ - inputTime;
    RS_TRACE_NAME_FMT("RSJankStats::ReportEventComplete pkgname: %s, pageurl: %s, sceneid: %s, inputTime:% " PRId64 ".",
        info.bundleName.c_str(), info.pageUrl.c_str(), info.pageUrl.c_str(), inputTime);
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "APP_PID", info.appPid, "BUNDLE_NAME", info.bundleName,
        "ABILITY_NAME", info.abilityName, "PROCESS_NAME", info.processName, "PAGE_URL", info.pageUrl, "SCENE_ID",
        info.sceneId, "INPUT_TIME", inputTime, "ANIMATION_START_LATENCY", animationStartLatency,
        "ANIMATION_END_LATENCY", animationEndLatency, "E2E_LATENCY", completedLatency);
    jankFrames.isSetReportEventComplete_ = false;
}

void RSJankStats::ReportEventJankFrame(JankFrames& jankFrames)
{
    auto reportName = "INTERACTION_RENDER_JANK";
    float aveFrameTime = jankFrames.totalFrameTime_ / (float)jankFrames.totalFrames_;
    auto info = jankFrames.info_;
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "UNIQUE_ID", info.uniqueId, "SCENE_ID", info.sceneId,
        "PROCESS_NAME", info.processName, "MODULE_NAME", info.bundleName, "ABILITY_NAME", info.abilityName, "PAGE_URL",
        info.pageUrl, "TOTAL_FRAMES", jankFrames.totalFrames_, "TOTAL_MISSED_FRAMES", jankFrames.totalMissedFrames_,
        "MAX_FRAMETIME", jankFrames.maxFrameTime_, "AVERAGE_FRAMETIME", aveFrameTime, "MAX_SEQ_MISSED_FRAMES",
        jankFrames.maxSeqMissedFrames_, "IS_FOLD_DISP", 0);
    jankFrames.isSetReportEventJankFrame_ = false;
}

void RSJankStats::ReportEventFirstFrame()
{
    auto reportName = "FIRST_FRAME_DRAWN";
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "APP_PID", appPid_);
    isFirstFrame_ = false;
}

uint64_t RSJankStats::ConvertTimeToSystime(uint64_t time)
{
    if (time == 0) {
        ROSEN_LOGW("RSJankStats::ConvertTimeToSystime, time is error");
        return 0;
    }
    struct timespec ts = { 0, 0 };
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    uint64_t nowTime = ts.tv_sec * S_TO_NS + ts.tv_nsec;
    uint64_t curSysTime = GetCurrentSystimeMs();
    auto sysTime = curSysTime - (nowTime - time) / MS_TO_NS;
    return sysTime;
}

uint64_t RSJankStats::GetCurrentSystimeMs()
{
    auto curTime = std::chrono::system_clock::now().time_since_epoch();
    auto curSysTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
    return curSysTime;
}

} // namespace Rosen
} // namespace OHOS
