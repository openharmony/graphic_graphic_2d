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

#include "rs_frame_blur_predict.h"

#include "hilog/log.h"
#include "rs_trace.h"
#include "rs_frame_report.h"

namespace OHOS {
namespace Rosen {
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "OHOS::RS"
#define LOGI(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)
RsFrameBlurPredict& RsFrameBlurPredict::GetInstance()
{
    static RsFrameBlurPredict instance;
    return instance;
}

RsFrameBlurPredict::RsFrameBlurPredict() {}

RsFrameBlurPredict::~RsFrameBlurPredict()
{
    ResetPredictDrawLargeAreaBlur();
    isValidBlurFrame_.store(false);
    predictBegin_.store(false);
}

void RsFrameBlurPredict::AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively()
{
    if (isValidBlurFrame_.load()) {
        GetCurrentFrameDrawLargeAreaBlurPredictively();
        ResetPredictDrawLargeAreaBlur();
    }
}

void RsFrameBlurPredict::UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely()
{
    if (isValidBlurFrame_.load()) {
        GetCurrentFrameDrawLargeAreaBlurPrecisely();
    }
}

void RsFrameBlurPredict::TakeEffectBlurScene(const EventInfo& eventInfo)
{
    isValidBlurFrame_.store(eventInfo.eventStatus);
    RS_TRACE_INT(eventInfo.description.c_str(), eventInfo.eventStatus);
    if (!eventInfo.eventStatus) {
        predictBegin_.store(false);
    }
}

void RsFrameBlurPredict::PredictDrawLargeAreaBlur(RSRenderNode& node)
{
    std::pair<bool, bool> nodeDrawLargeAreaBlur = {false, false};
    node.NodeDrawLargeAreaBlur(nodeDrawLargeAreaBlur);
    predictDrawLargeAreaBlur_.first = predictDrawLargeAreaBlur_.first || nodeDrawLargeAreaBlur.first;
    predictDrawLargeAreaBlur_.second = predictDrawLargeAreaBlur_.second || nodeDrawLargeAreaBlur.second;
}

void RsFrameBlurPredict::GetCurrentFrameDrawLargeAreaBlurPredictively()
{
    std::unordered_map<std::string, std::string> param = {};
    if (!predictBegin_.load()) {
        RS_TRACE_NAME("predict invalid frame");
        param["frameBlurPredict"] = RS_BLUR_PREDICT_INVALID;
        RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_BLUR_PREDICT, param);
        return;
    }
    RS_TRACE_NAME_FMT("predict current %d frame", predictDrawLargeAreaBlur_.first);
    if (predictDrawLargeAreaBlur_.first) {
        param["frameBlurPredict"] = RS_BLUR_PREDICT_LONG;
    } else {
        param["frameBlurPredict"] = RS_BLUR_PREDICT_SHORT;
    }
    RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_BLUR_PREDICT, param);
    predictDrawLargeAreaBlur_.first = false;
}

void RsFrameBlurPredict::GetCurrentFrameDrawLargeAreaBlurPrecisely()
{
    RS_TRACE_NAME_FMT("precise current %d frame", predictDrawLargeAreaBlur_.second);
    std::unordered_map<std::string, std::string> param = {};
    if (predictDrawLargeAreaBlur_.second) {
        predictBegin_.store(true);
        param["frameBlurPredict"] = RS_BLUR_PREDICT_LONG;
    } else {
        param["frameBlurPredict"] = RS_BLUR_PREDICT_SHORT;
    }
    RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_BLUR_PREDICT, param);
    predictDrawLargeAreaBlur_.second = false;
}

void RsFrameBlurPredict::ResetPredictDrawLargeAreaBlur()
{
    predictDrawLargeAreaBlur_.first = false;
    predictDrawLargeAreaBlur_.second = false;
}
} // namespace Rosen
} // namespace OHOS
