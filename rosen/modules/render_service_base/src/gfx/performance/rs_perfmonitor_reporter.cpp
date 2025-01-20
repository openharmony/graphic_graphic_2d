/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "gfx/performance/rs_perfmonitor_reporter.h"
#include "common/rs_background_thread.h"
#ifdef ROSEN_OHOS
#include "hisysevent.h"
#endif
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
const int32_t COUNTER_SIZE = 4;
const int32_t REASON_SIZE = 7;
const char* CPU_ONDRAW = "CPU_ONDRAW";
const char* CPU_FLUSH = "CPU_FLUSH";
// for rendergroup subhealth
#endif

RSPerfMonitorReporter& RSPerfMonitorReporter::GetInstance()
{
    static RSPerfMonitorReporter instance;
    return instance;
}

void RSPerfMonitorReporter::SetFocusAppInfo(const char* bundleName)
{
#ifdef ROSEN_OHOS
    if (!IsOpenPerf()) {
        return;
    }
    SetCurrentBundleName(bundleName);
    auto task = [this]() {
        this->ReportBlurStatEvent();
        this->ReportTextureStatEvent();
        this->ReportCacheReasonEvent();
        Drawing::PerfmonitorReporter::ResetStatsData();
    };
    RSBackgroundThread::Instance().PostTask(task);
#endif
}

void RSPerfMonitorReporter::ReportAtRsFrameEnd()
{
#ifdef ROSEN_OHOS
    if (!IsOpenPerf()) {
        return;
    }
    auto task = [this]() {
        this->ReportBlurPerfEvent();
        this->ReportTexturePerfEvent();
        Drawing::PerfmonitorReporter::ResetPerfEventData();
    };
    RSBackgroundThread::Instance().PostTask(task);
#endif
}

void RSPerfMonitorReporter::ReportBlurStatEvent()
{
#ifdef ROSEN_OHOS
    std::string bundleName = GetCurrentBundleName();
    std::map<std::string, std::vector<uint16_t>> uploadBlur;
    {
        std::lock_guard<std::mutex> guard(mtx_);
        uploadBlur = statsBlur_;
        statsBlur_.clear();
    }
    for (const auto& node : uploadBlur) {
        RS_TRACE_NAME_FMT("SubHealthEvent RS_STATS_BLUR [%s], [%s]", bundleName.c_str(),
            node.first.c_str());
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_STATS_BLUR",
            HiviewDFX::HiSysEvent::EventType::STATISTIC, "BUNDLE_NAME", bundleName,
            "NODE_NAME", node.first, "BLUR_TIME", node.second, "BLUR_STATUS", CPU_ONDRAW);
    }
    uploadBlur.clear();

    uploadBlur = Drawing::PerfmonitorReporter::GetBlurStatsData();
    for (const auto& node : uploadBlur) {
        RS_TRACE_NAME_FMT("SubHealthEvent RS_STATS_BLUR [%s], [%s]", bundleName.c_str(),
            node.first.c_str());
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_STATS_BLUR",
            HiviewDFX::HiSysEvent::EventType::STATISTIC, "BUNDLE_NAME", bundleName,
            "NODE_NAME", node.first, "BLUR_TIME", node.second, "BLUR_STATUS", CPU_FLUSH);
    }
    uploadBlur.clear();
#endif
}

void RSPerfMonitorReporter::ReportBlurPerfEvent()
{
#ifdef ROSEN_OHOS
    std::map<std::string, Drawing::RsBlurEvent> uploadEvents;
    {
        std::lock_guard<std::mutex> guard(mtx_);
        uploadEvents = eventBlur_;
        eventBlur_.clear();
    }

    std::string bundleName = GetCurrentBundleName();
    for (const auto& [nodeName, node] : uploadEvents) {
        RS_TRACE_NAME_FMT("SubHealthEvent RS_NODE_BLUR [%s], [%s], [%lld]", bundleName.c_str(),
            nodeName.c_str(), node.fBlurTime);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_NODE_BLUR",
            HiviewDFX::HiSysEvent::EventType::FAULT, "PID", node.fPid, "NODE_NAME", nodeName,
            "BUNDLE_NAME", bundleName, "FILTER_TYPE", node.fFilterType, "BLUR_RADIUS", node.fBlurRadius,
            "BLUR_WIDTH", node.fWidth, "BLUR_HEIGHT", node.fHeight, "BLUR_TIME", node.fBlurTime,
            "BLUR_STATUS", CPU_ONDRAW);
    }
    uploadEvents.clear();

    uploadEvents = Drawing::PerfmonitorReporter::GetBlurPerfEventData();
    for (const auto& [nodeName, node] : uploadEvents) {
        RS_TRACE_NAME_FMT("SubHealthEvent RS_NODE_BLUR [%s], [%s], [%lld]", bundleName.c_str(),
            nodeName.c_str(), node.fBlurTime);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_NODE_BLUR",
            HiviewDFX::HiSysEvent::EventType::FAULT, "PID", node.fPid, "NODE_NAME", nodeName,
            "BUNDLE_NAME", bundleName, "FILTER_TYPE", node.fFilterType, "BLUR_RADIUS", node.fBlurRadius,
            "BLUR_WIDTH", node.fWidth, "BLUR_HEIGHT", node.fHeight, "BLUR_TIME", node.fBlurTime,
            "BLUR_STATUS", CPU_FLUSH);
    }
    uploadEvents.clear();
#endif
}

void RSPerfMonitorReporter::ReportCacheReasonEvent()
{
#ifdef ROSEN_OHOS
    std::string bundleName = GetCurrentBundleName();
    std::map<std::string, std::vector<uint16_t>> uploadReason;
    {
        std::lock_guard<std::mutex> guard(mtx_);
        uploadReason = statsReason_;
        statsReason_.clear();
    }
    for (const auto& node : uploadReason) {
        RS_TRACE_NAME_FMT("SubHealthEvent RS_STATS_CAHCHE [%s], [%s]", bundleName.c_str(),
            node.first.c_str());
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_STATS_CAHCHE",
            HiviewDFX::HiSysEvent::EventType::STATISTIC, "BUNDLE_NAME", bundleName,
            "NODE_NAME", node.first, "CACHE_REASON", node.second);
    }
    uploadReason.clear();
#endif
}

void RSPerfMonitorReporter::ReportTextureStatEvent()
{
#ifdef ROSEN_OHOS
    std::string bundleName = GetCurrentBundleName();
    std::map<std::string, std::vector<uint16_t>> uploadTexture =
        Drawing::PerfmonitorReporter::GetTextureStatsData();
    for (const auto& node : uploadTexture) {
        RS_TRACE_NAME_FMT("SubHealthEvent RS_STATS_TEXTURE [%s], [%s]",
            bundleName.c_str(), node.first.c_str());
        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_STATS_TEXTURE",
            OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "BUNDLE_NAME", bundleName,
            "NODE_NAME", node.first, "TEXTURE_TIME", node.second, "TEXTURE_STATUS", CPU_FLUSH);
    }
#endif
}

void RSPerfMonitorReporter::ReportTexturePerfEvent()
{
#ifdef ROSEN_OHOS
    std::string bundleName = GetCurrentBundleName();
    std::map<std::string, Drawing::RsTextureEvent> uploadEvents =
        Drawing::PerfmonitorReporter::GetTexturePerfEventData();
    for (const auto& [nodeName, node] : uploadEvents) {
        RS_TRACE_NAME_FMT("SubHealthEvent RS_NODE_TEXTURE [%s], [%s], [%lld]",
            bundleName.c_str(), nodeName.c_str(), node.fAllocTime);
        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_NODE_TEXTURE",
            OHOS::HiviewDFX::HiSysEvent::EventType::FAULT, "PID", node.fPid, "NODE_NAME", nodeName,
            "BUNDLE_NAME", bundleName, "TEXTURE_TIME", node.fAllocTime, "FMAX_BYTES", node.fMaxBytes,
            "FBUDGETED_BYTES", node.fBudgetedBytes, "CLEAR_CACHE", node.fClearCache,
            "TEXTURE_STATUS", CPU_FLUSH);
    }
#endif
}

void RSPerfMonitorReporter::RecordBlurPerfEvent(NodeId nodeId, const std::string& nodeName,
    uint16_t filterType, float blurRadius, int32_t width, int32_t height, int64_t blurTime,
    bool isBlurType)
{
#ifdef ROSEN_OHOS
    if (!IsOpenPerf() || !isBlurType || nodeName.empty() ||
        blurTime < Drawing::COUNTER_MS_THIRD_TYPE) {
        return;
    }
    int32_t pid = ExtractPid(nodeId);
    Drawing::RsBlurEvent bEvent;
    bEvent.initEvent(pid, filterType, blurRadius, width, height, blurTime);
    std::lock_guard<std::mutex> guard(mtx_);
    eventBlur_[nodeName] = bEvent;
#endif
}

void RSPerfMonitorReporter::RecordBlurNode(const std::string& nodeName, int64_t duration,
    bool isBlurType)
{
#ifdef ROSEN_OHOS
    int16_t cType = Drawing::PerfmonitorReporter::GetSplitRange(duration);
    if (!IsOpenPerf() || !isBlurType || nodeName.empty() ||
        cType <= Drawing::COUNTER_INVALID_TYPE) {
        return;
    }
    std::lock_guard<std::mutex> guard(mtx_);
    if (statsBlur_.find(nodeName) == statsBlur_.end()) {
        statsBlur_[nodeName] = std::vector<uint16_t>(COUNTER_SIZE, 0);
    }
    statsBlur_[nodeName][cType]++;
#endif
}

void RSPerfMonitorReporter::RecordBlurCacheReason(const std::string& nodeName, BLUR_CLEAR_CACHE_REASON reason,
    bool isBlurType)
{
#ifdef ROSEN_OHOS
    if (!IsOpenPerf() || !isBlurType || nodeName.empty()) {
        return;
    }
    std::lock_guard<std::mutex> guard(mtx_);
    if (statsReason_.find(nodeName) == statsReason_.end()) {
        statsReason_[nodeName] = std::vector<uint16_t>(REASON_SIZE, 0);
    }
    statsReason_[nodeName][reason]++;
#endif
}

void RSPerfMonitorReporter::SetCurrentBundleName(const char* bundleName)
{
    std::lock_guard<std::mutex> guard(mtx_);
    currentBundleName_ = bundleName;
}

std::string RSPerfMonitorReporter::GetCurrentBundleName()
{
    std::lock_guard<std::mutex> guard(mtx_);
    return currentBundleName_;
}

bool RSPerfMonitorReporter::IsOpenPerf()
{
#ifdef ROSEN_OHOS
    static bool isOpenPerf = Drawing::PerfmonitorReporter::IsOpenPerf();
    return isOpenPerf;
#else
    return false;
#endif
}
} // namespace Rosen
} // namespace OHOS
