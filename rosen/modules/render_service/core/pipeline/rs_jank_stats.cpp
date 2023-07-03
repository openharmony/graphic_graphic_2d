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

#include "rs_jank_stats.h"

#include <chrono>
#include <unistd.h>

#include "hisysevent.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float VSYNC_PERIOD = 16.6;        // ms
}

RSJankStats& RSJankStats::GetInstance()
{
    static RSJankStats instance;
    return instance;
}

void RSJankStats::SetStartTime()
{
    auto start = std::chrono::system_clock::now().time_since_epoch();
    startTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(start).count();
    if (isfirstSetStart_) {
        lastReportTime_ = startTime_;
        isfirstSetStart_ = false;
    }
}

void RSJankStats::SetEndTime()
{
    auto end = std::chrono::system_clock::now().time_since_epoch();
    endTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(end).count();
    auto duration = endTime_ - startTime_;
    if (duration >= VSYNC_PERIOD) {
        SetRSJankStats(static_cast<int>(duration / VSYNC_PERIOD));
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

void RSJankStats::ReportJankStats()
{
    if (!isNeedReport_) {
        ROSEN_LOGW("RSInterfaces::ReportJankStats Nothing need to report");
        return;
    }
    auto report = std::chrono::system_clock::now().time_since_epoch();
    auto reportTime = std::chrono::duration_cast<std::chrono::milliseconds>(report).count();
    auto reportDuration = reportTime - lastReportTime_;
    auto reportName = "JANK_STATS_RS";
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "STARTTIME", lastReportTime_, "DURATION", reportDuration,
        "JANK_STATS", rsJankStats_, "JANK_STATS_VER", 1);
    std::fill(rsJankStats_.begin(), rsJankStats_.end(), 0);
    lastReportTime_ = reportTime;
    isNeedReport_ = false;
}

} // namespace Rosen
} // namespace OHOS
