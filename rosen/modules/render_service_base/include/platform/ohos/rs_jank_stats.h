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

#ifndef ROSEN_JANK_STATS_H
#define ROSEN_JANK_STATS_H

#include <cstdint>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "nocopyable.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
namespace {
using UniqueId = int64_t;
using TraceId = int32_t;

constexpr int64_t TIMESTAMP_INITIAL = -1;

struct JankFrames {
    bool isSetReportEventResponse_ = false;
    bool isSetReportEventComplete_ = false;
    bool isSetReportEventJankFrame_ = false;
    bool isReportEventResponse_ = false;
    bool isReportEventComplete_ = false;
    bool isReportEventJankFrame_ = false;
    bool isUpdateJankFrame_ = false;
    bool isFirstFrame_ = false;
    int64_t setTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t startTimeSteady_ = TIMESTAMP_INITIAL;
    int32_t seqMissedFrames_ = 0;
    int32_t totalFrames_ = 0;
    int32_t totalMissedFrames_ = 0;
    int64_t maxFrameTimeSteady_ = 0;
    int32_t maxSeqMissedFrames_ = 0;
    int64_t totalFrameTimeSteady_ = 0;
    Rosen::DataBaseRs info_;
};

struct JankFrameRecordStats {
    const std::string countTraceName_;
    const int64_t recordThreshold_;
    bool isRecorded_ = false;
    JankFrameRecordStats(const std::string& countTraceName, int64_t recordThreshold)
        : countTraceName_(countTraceName), recordThreshold_(recordThreshold) {}
};

struct AnimationTraceStats {
    std::string traceName_;
    int64_t traceCreateTimeSteady_ = TIMESTAMP_INITIAL;
};
} // namespace

class RSJankStats {
public:
    static RSJankStats& GetInstance();
    void SetStartTime();
    void SetEndTime();
    void ReportJankStats();
    void SetReportEventResponse(DataBaseRs info);
    void SetReportEventComplete(DataBaseRs info);
    void SetReportEventJankFrame(DataBaseRs info);
    void SetAppFirstFrame(pid_t appPid);

private:
    RSJankStats() {};
    ~RSJankStats() {};
    DISALLOW_COPY_AND_MOVE(RSJankStats);

    void UpdateEndTime();
    void SetRSJankStats(int64_t missedVsync);
    void UpdateJankFrame(JankFrames& jankFrames);
    void ReportEventResponse(const JankFrames& jankFrames) const;
    void ReportEventComplete(const JankFrames& jankFrames) const;
    void ReportEventJankFrame(const JankFrames& jankFrames) const;
    void ReportEventFirstFrame();
    void ReportEventFirstFrameByPid(pid_t appPid) const;
    void RecordJankFrameInit();
    void RecordJankFrame(int64_t missedFrames);
    void RecordJankFrameSingle(int64_t missedFrames, JankFrameRecordStats& recordStats);
    void RecordAnimationDynamicFrameRate(TraceId traceId, const JankFrames& jankFrames) const;
    void SetAnimationTraceBegin(TraceId traceId, const JankFrames& jankFrames);
    void SetAnimationTraceEnd(TraceId traceId);
    void CheckAnimationTraceTimeout();
    std::string GetSceneDescription(const DataBaseRs& info) const;
    int64_t ConvertTimeToSystime(int64_t time) const;
    int64_t GetCurrentSystimeMs() const;
    int64_t GetCurrentSteadyTimeMs() const;

    static constexpr uint16_t ANIMATION_TRACE_CHECK_FREQ = 20;
    static constexpr uint32_t JANK_RANGE_VERSION = 1;
    static constexpr size_t JANK_STATS_SIZE = 8;
    static constexpr bool IS_FOLD_DISP = false;
    static inline const std::string JANK_FRAME_6F_COUNT_TRACE_NAME = "JANK_FRAME_6F";
    std::vector<JankFrameRecordStats> jankAnimatorFrameRecorder_{ {"JANK_ANIMATOR_FRAME_2F", 2} };
    bool isFirstSetStart_ = true;
    bool isFirstSetEnd_ = true;
    bool isNeedReportJankStats_ = false;
    int64_t startTime_ = TIMESTAMP_INITIAL;
    int64_t startTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t endTime_ = TIMESTAMP_INITIAL;
    int64_t endTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t lastEndTime_ = TIMESTAMP_INITIAL;
    int64_t lastEndTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t lastReportTime_ = TIMESTAMP_INITIAL;
    int64_t lastReportTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t lastJankFrame6FreqTimeSteady_ = TIMESTAMP_INITIAL;
    uint16_t animationTraceCheckCnt_ = 0;
    std::vector<uint16_t> rsJankStats_ = std::vector<uint16_t>(JANK_STATS_SIZE, 0);
    std::queue<pid_t> firstFrameAppPids_;
    std::map<TraceId, AnimationTraceStats> animationAsyncTraces_;
    std::map<UniqueId, JankFrames> animateJankFrames_;
    std::mutex animateJankFramesMutex_;
    std::mutex firstFrameAppPidsMutex_;

    enum JankRangeType : size_t {
        JANK_FRAME_6_FREQ = 0,
        JANK_FRAME_15_FREQ,
        JANK_FRAME_20_FREQ,
        JANK_FRAME_36_FREQ,
        JANK_FRAME_48_FREQ,
        JANK_FRAME_60_FREQ,
        JANK_FRAME_120_FREQ,
        JANK_FRAME_180_FREQ,
        JANK_FRAME_INVALID,
    };
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_JANK_STATS_H
