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
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "nocopyable.h"
#include "transaction/rs_render_service_client.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
constexpr uint32_t STANDARD_REFRESH_RATE = 60;
constexpr uint32_t MAX_REFRESH_RATE_TO_OPTIMIZE_JANK_LOAD = 90;
constexpr int64_t TIMESTAMP_INITIAL = -1;
constexpr int32_t TRACE_ID_INITIAL = -1;
constexpr float TIMESTAMP_INITIAL_FLOAT = -1.f;
constexpr float MS_TO_US = 1000.f; // ms to us

struct JankFrames {
    bool isSetReportEventResponse_ = false;
    bool isSetReportEventResponseTemp_ = false;
    bool isSetReportEventComplete_ = false;
    bool isSetReportEventJankFrame_ = false;
    bool isReportEventResponse_ = false;
    bool isReportEventComplete_ = false;
    bool isReportEventJankFrame_ = false;
    bool isUpdateJankFrame_ = false;
    bool isFirstFrame_ = false;
    bool isFirstFrameTemp_ = false;
    bool isFrameRateRecorded_ = false;
    bool isAnimationEnded_ = false;
    bool isDisplayAnimator_ = false;
    bool isAnimationInterrupted_ = false;
    int64_t setTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t startTime_ = TIMESTAMP_INITIAL;
    int64_t traceCreateTime_ = TIMESTAMP_INITIAL;
    int64_t traceCreateTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t traceTerminateTime_ = TIMESTAMP_INITIAL;
    int64_t traceTerminateTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t maxFrameOccurenceTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t lastMaxFrameOccurenceTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t maxHitchOccurenceTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t lastMaxHitchOccurenceTimeSteady_ = TIMESTAMP_INITIAL;
    int32_t seqMissedFrames_ = 0;
    int32_t totalFrames_ = 0;
    int32_t lastTotalFrames_ = 0;
    int32_t totalMissedFrames_ = 0;
    int32_t lastTotalMissedFrames_ = 0;
    int64_t maxFrameTimeSteady_ = 0;
    int64_t lastMaxFrameTimeSteady_ = 0;
    int64_t maxTechFrameTimeSteady_ = 0;
    int64_t lastMaxTechFrameTimeSteady_ = 0;
    int64_t maxRealFrameTimeSteady_ = 0;
    int64_t lastMaxRealFrameTimeSteady_ = 0;
    int32_t maxSeqMissedFrames_ = 0;
    int32_t lastMaxSeqMissedFrames_ = 0;
    int64_t totalFrameTimeSteady_ = 0;
    int64_t lastTotalFrameTimeSteady_ = 0;
    int32_t traceId_ = TRACE_ID_INITIAL;
    int64_t totalFrameTimeSteadyForHTR_ = 0;
    int64_t lastTotalFrameTimeSteadyForHTR_ = 0;
    uint32_t lastMaxFrameRefreshRate_ = 0;
    uint32_t maxFrameRefreshRate_ = 0;
    float totalHitchTimeSteady_ = 0;
    float lastTotalHitchTimeSteady_ = 0;
    float maxHitchTime_ = 0;
    float lastMaxHitchTime_ = 0;
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
    std::pair<int64_t, std::string> animationId_ = { -1, "" };
    std::string traceName_;
    int64_t traceCreateTimeSteady_ = TIMESTAMP_INITIAL;
    bool isDisplayAnimator_ = false;
};

struct TraceIdRemainderStats {
    int64_t remainder_ = 0;
    int64_t setTimeSteady_ = TIMESTAMP_INITIAL;
};

struct JankDurationParams {
    int64_t timeStart_ = TIMESTAMP_INITIAL;
    int64_t timeStartSteady_ = TIMESTAMP_INITIAL;
    float timeStartSteadyFloat_ = TIMESTAMP_INITIAL_FLOAT;
    int64_t timeEnd_ = TIMESTAMP_INITIAL;
    int64_t timeEndSteady_ = TIMESTAMP_INITIAL;
    float timeEndSteadyFloat_ = TIMESTAMP_INITIAL_FLOAT;
    uint32_t refreshRate_ = 0;
    bool discardJankFrames_ = false;
    bool skipJankAnimatorFrame_ = false;
    bool implicitAnimationEnd_ = false;
};

class RSJankStats {
public:
    static RSJankStats& GetInstance();
    void SetOnVsyncStartTime(int64_t onVsyncStartTime, int64_t onVsyncStartTimeSteady,
                             float onVsyncStartTimeSteadyFloat);
    void SetStartTime(bool doDirectComposition = false);
    void SetEndTime(bool skipJankAnimatorFrame = false, bool discardJankFrames = false,
                    uint32_t dynamicRefreshRate = STANDARD_REFRESH_RATE,
                    bool doDirectComposition = false, bool isReportTaskDelayed = false);
    void HandleDirectComposition(const JankDurationParams& rsParams, bool isReportTaskDelayed,
        std::function<void(const std::function<void()>&)> postTaskHandler = nullptr);
    void ReportJankStats();
    void SetReportEventResponse(const DataBaseRs& info);
    void SetReportEventComplete(const DataBaseRs& info);
    void SetReportEventJankFrame(const DataBaseRs& info, bool isReportTaskDelayed);
    void SetReportRsSceneJankStart(const AppInfo& info);
    void SetReportRsSceneJankEnd(const AppInfo& info);
    void SetAppFirstFrame(pid_t appPid);
    void SetImplicitAnimationEnd(bool isImplicitAnimationEnd);
    void SetAccumulatedBufferCount(int accumulatedBufferCount);
    bool IsAnimationEmpty();

private:
    RSJankStats() = default;
    ~RSJankStats() = default;
    DISALLOW_COPY_AND_MOVE(RSJankStats);

    void SetStartTimeInner(bool doDirectComposition = false);
    void SetEndTimeInner(bool skipJankAnimatorFrame = false, bool discardJankFrames = false,
                         uint32_t dynamicRefreshRate = STANDARD_REFRESH_RATE,
                         bool doDirectComposition = false, bool isReportTaskDelayed = false);
    void SetImplicitAnimationEndInner(bool isImplicitAnimationEnd);
    bool NeedPostTaskToUniRenderThread() const;
    void UpdateEndTime();
    void SetRSJankStats(bool skipJankStats, uint32_t dynamicRefreshRate);
    size_t GetJankRangeType(int64_t missedVsync) const;
    void UpdateJankFrame(JankFrames& jankFrames, bool skipJankStats, uint32_t dynamicRefreshRate);
    void UpdateHitchTime(JankFrames& jankFrames, float standardFrameTime);
    void ReportEventResponse(const JankFrames& jankFrames) const;
    void ReportEventComplete(const JankFrames& jankFrames) const;
    void ReportEventJankFrame(const JankFrames& jankFrames, bool isReportTaskDelayed) const;
    void ReportEventJankFrameWithoutDelay(const JankFrames& jankFrames) const;
    void ReportEventJankFrameWithDelay(const JankFrames& jankFrames) const;
    void GetMaxJankInfo(const JankFrames& jankFrames, bool isReportTaskDelayed,
        int64_t& maxFrameTimeFromStart, int64_t& maxHitchTimeFromStart, int64_t& duration) const;
    void ReportEventHitchTimeRatio(const JankFrames& jankFrames, bool isReportTaskDelayed) const;
    void ReportEventHitchTimeRatioWithoutDelay(const JankFrames& jankFrames) const;
    void ReportEventHitchTimeRatioWithDelay(const JankFrames& jankFrames) const;
    void ReportEventFirstFrame();
    void ReportEventFirstFrameByPid(pid_t appPid) const;
    void ReportSceneJankStats(const AppInfo& appInfo);
    void ReportSceneJankFrame(uint32_t dynamicRefreshRate);
    void HandleImplicitAnimationEndInAdvance(JankFrames& jankFrames, bool isReportTaskDelayed);
    void RecordJankFrame(uint32_t dynamicRefreshRate);
    void RecordJankFrameSingle(int64_t missedFrames, JankFrameRecordStats& recordStats);
    void RecordAnimationDynamicFrameRate(JankFrames& jankFrames, bool isReportTaskDelayed);
    void SetAnimationTraceBegin(std::pair<int64_t, std::string> animationId, JankFrames& jankFrames);
    void SetAnimationTraceEnd(JankFrames& jankFrames);
    void CheckAnimationTraceTimeout();
    void ClearAllAnimation();
    std::string GetSceneDescription(const DataBaseRs& info) const;
    std::pair<int64_t, std::string> GetAnimationId(const DataBaseRs& info) const;
    int32_t GetTraceIdInit(const DataBaseRs& info, int64_t setTimeSteady);
    int64_t ConvertTimeToSystime(int64_t time) const;
    int64_t GetEffectiveFrameTime(bool isConsiderRsStartTime) const;
    float GetEffectiveFrameTimeFloat(bool isConsiderRsStartTime) const;
    int64_t GetCurrentSystimeMs() const;
    int64_t GetCurrentSteadyTimeMs() const;
    float GetCurrentSteadyTimeMsFloat() const;

    static constexpr uint16_t ANIMATION_TRACE_CHECK_FREQ = 20;
    static constexpr uint32_t JANK_RANGE_VERSION = 1;
    static constexpr uint32_t JANK_SCENE_RANGE_VERSION = 2;
    static constexpr size_t JANK_STATS_SIZE = 8;
    static constexpr int64_t TRACE_ID_SCALE_PARAM = 10;
    static constexpr int64_t MIN_FRAME_SHOW_TIME = 16;
    static constexpr int32_t DEFAULT_INT_VALUE = 0;
    static inline const std::string DEFAULT_STRING_VALUE = "";
    const int64_t SCENE_JANK_FRAME_THRESHOLD = RSSystemProperties::GetSceneJankFrameThreshold();
    static constexpr int32_t FILTER_TYPE = 0;
    static constexpr bool IS_FOLD_DISP = false;
    static constexpr bool IS_CALCULATE_PRECISE_HITCH_TIME = true;
    static inline const std::string ACCUMULATED_BUFFER_COUNT_TRACE_NAME = "ACCUMULATED_BUFFER_COUNT";
    static inline const std::string JANK_FRAME_1_TO_5F_COUNT_TRACE_NAME = "JANK_FRAME_1-5F";
    static inline const std::string JANK_FRAME_6F_COUNT_TRACE_NAME = "JANK_FRAME_6F";
    std::vector<JankFrameRecordStats> jankExplicitAnimatorFrameRecorder_{ {"JANK_EXPLICIT_ANIMATOR_FRAME_1F", 1},
                                                                          {"JANK_EXPLICIT_ANIMATOR_FRAME_2F", 2} };
    std::vector<JankFrameRecordStats> jankImplicitAnimatorFrameRecorder_{ {"JANK_IMPLICIT_ANIMATOR_FRAME_1F", 1},
                                                                          {"JANK_IMPLICIT_ANIMATOR_FRAME_2F", 2} };
    bool isFirstSetStart_ = true;
    bool isFirstSetEnd_ = true;
    bool isNeedReportJankStats_ = false;
    bool isNeedReportSceneJankStats_ = false;
    bool isLastReportSceneDone_ = true;
    bool isLastFrameDoDirectComposition_ = false;
    bool isCurrentFrameSwitchToNotDoDirectComposition_ = false;
    float rsStartTimeSteadyFloat_ = TIMESTAMP_INITIAL_FLOAT;
    float rtEndTimeSteadyFloat_ = TIMESTAMP_INITIAL_FLOAT;
    float rtLastEndTimeSteadyFloat_ = TIMESTAMP_INITIAL_FLOAT;
    int64_t rsStartTime_ = TIMESTAMP_INITIAL;
    int64_t rsStartTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t rtStartTime_ = TIMESTAMP_INITIAL;
    int64_t rtStartTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t rtEndTime_ = TIMESTAMP_INITIAL;
    int64_t rtEndTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t rtLastEndTime_ = TIMESTAMP_INITIAL;
    int64_t rtLastEndTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t lastReportTime_ = TIMESTAMP_INITIAL;
    int64_t lastReportTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t lastJankFrame6FreqTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t lastSceneReportTime_ = TIMESTAMP_INITIAL;
    int64_t lastSceneReportTimeSteady_ = TIMESTAMP_INITIAL;
    int64_t lastSceneJankFrame6FreqTimeSteady_ = TIMESTAMP_INITIAL;
    int32_t explicitAnimationTotal_ = 0;
    int32_t implicitAnimationTotal_ = 0;
    uint16_t animationTraceCheckCnt_ = 0;
    int accumulatedBufferCount_ = 0;
    std::vector<uint16_t> rsJankStats_ = std::vector<uint16_t>(JANK_STATS_SIZE, 0);
    std::vector<uint16_t> rsSceneJankStats_ = std::vector<uint16_t>(JANK_STATS_SIZE, 0);
    std::queue<pid_t> firstFrameAppPids_;
    std::map<int32_t, AnimationTraceStats> animationAsyncTraces_;
    std::map<int64_t, TraceIdRemainderStats> traceIdRemainder_;
    std::map<std::pair<int64_t, std::string>, JankFrames> animateJankFrames_;
    std::mutex mutex_;
    Rosen::AppInfo appInfo_;

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
