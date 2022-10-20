/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_physical_screen_processor.h"

#include "rs_trace.h"
#include "socperf_client.h"
#include "string_utils.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t PERF_LEVEL_0 = 0;
    constexpr uint32_t PERF_LEVEL_1 = 1;
    constexpr uint32_t PERF_LEVEL_2 = 2;
    constexpr int32_t PERF_LEVEL_1_REQUESTED_CODE = 10013;
    constexpr int32_t PERF_LEVEL_2_REQUESTED_CODE = 10014;
    constexpr int32_t PERF_LEVEL_3_REQUESTED_CODE = 10015;
    constexpr int64_t PERF_TIME_OUT = 1000;
    constexpr uint32_t PERF_LEVEL_INTERVAL = 10;
}
RSPhysicalScreenProcessor::RSPhysicalScreenProcessor()
    : composerAdapter_(std::make_unique<RSComposerAdapter>())
{
}

RSPhysicalScreenProcessor::~RSPhysicalScreenProcessor() noexcept
{
}

bool RSPhysicalScreenProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId)) {
        return false;
    }

    return composerAdapter_->Init(screenInfo_, offsetX, offsetY, mirrorAdaptiveCoefficient_,
        [this](const auto& surface, const auto& layers) { Redraw(surface, layers); });
}

void RSPhysicalScreenProcessor::PostProcess()
{
    uint32_t curNum = layers_.size();
    composerAdapter_->CommitLayers(layers_);
    static uint32_t lastLayerLevel = 0;
    static std::chrono::steady_clock::time_point lastRequestPerfTime = std::chrono::steady_clock::now();
    auto curLayerLevel = GetLayerLevel(curNum);
    auto currentTime = std::chrono::steady_clock::now();
    bool isTimeOut = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRequestPerfTime).
        count() > PERF_TIME_OUT;
    if (curLayerLevel != lastLayerLevel || isTimeOut) {
        if (!isTimeOut) {
            ClosePerf(lastLayerLevel);
        }
        RequestPerf(curLayerLevel);
        lastLayerLevel = curLayerLevel;
        lastRequestPerfTime = currentTime;
    }
}

void RSPhysicalScreenProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    auto layer = composerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGD(
            "RSPhysicalScreenProcessor::ProcessSurface: failed to createLayer for node(id: %" PRIu64 ")", node.GetId());
        return;
    }

    layers_.emplace_back(layer);
}

void RSPhysicalScreenProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    RS_LOGI("RSPhysicalScreenProcessor::ProcessDisplaySurface() is not supported.");
}

void RSPhysicalScreenProcessor::Redraw(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers)
{
    RS_TRACE_NAME("Redraw");
    if (surface == nullptr) {
        RS_LOGE("RSPhysicalScreenProcessor::Redraw: surface is null.");
        return;
    }

    RS_LOGD("RsDebug RSPhysicalScreenProcessor::Redraw flush frame buffer start");
    bool forceCPU = RSBaseRenderEngine::NeedForceCPU(layers);
    auto renderFrame = renderEngine_->RequestFrame(surface, renderFrameConfig_, forceCPU);
    if (renderFrame == nullptr) {
        RS_LOGE("RsDebug RSPhysicalScreenProcessor::Redraw：failed to request frame.");
        return;
    }

    auto canvas = renderFrame->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE("RsDebug RSPhysicalScreenProcessor::Redraw：canvas is nullptr.");
        return;
    }
    canvas->concat(screenTransformMatrix_);
    renderEngine_->DrawLayers(*canvas, layers, forceCPU, mirrorAdaptiveCoefficient_);
    renderFrame->Flush();
    RS_LOGD("RsDebug RSPhysicalScreenProcessor::Redraw flush frame buffer end");
}

uint32_t RSPhysicalScreenProcessor::GetLayerLevel(uint32_t layerLevel) const
{
    return layerLevel / PERF_LEVEL_INTERVAL;
}

void RSPhysicalScreenProcessor::ClosePerf(uint32_t layerLevel)
{
    switch (layerLevel) {
        case PERF_LEVEL_0: {
            // do nothing
            RS_LOGI("RsDebug RSPhysicalScreenProcessor::ClosePerf: do nothing");
            break;
        }
        case PERF_LEVEL_1: {
            OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_LEVEL_1_REQUESTED_CODE, false, "");
            RS_LOGI("RsDebug RSPhysicalScreenProcessor::ClosePerf: level1");
            break;
        }
        case PERF_LEVEL_2: {
            OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_LEVEL_2_REQUESTED_CODE, false, "");
            RS_LOGI("RsDebug RSPhysicalScreenProcessor::ClosePerf: level2");
            break;
        }
        default: {
            OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_LEVEL_3_REQUESTED_CODE, false, "");
            RS_LOGI("RsDebug RSPhysicalScreenProcessor::ClosePerf: level3");
            break;
        }
    }
}

void RSPhysicalScreenProcessor::RequestPerf(uint32_t layerLevel)
{
    switch (layerLevel) {
        case PERF_LEVEL_0: {
            // do nothing
            RS_LOGI("RsDebug RSPhysicalScreenProcessor::RequestPerf: do nothing");
            break;
        }
        case PERF_LEVEL_1: {
            OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_LEVEL_1_REQUESTED_CODE, "");
            RS_LOGI("RsDebug RSPhysicalScreenProcessor::RequestPerf: level1");
            break;
        }
        case PERF_LEVEL_2: {
            OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_LEVEL_2_REQUESTED_CODE, "");
            RS_LOGI("RsDebug RSPhysicalScreenProcessor::RequestPerf: level2");
            break;
        }
        default: {
            OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_LEVEL_3_REQUESTED_CODE, "");
            RS_LOGI("RsDebug RSPhysicalScreenProcessor::RequestPerf: level3");
            break;
        }
    }
}
} // namespace Rosen
} // namespace OHOS
