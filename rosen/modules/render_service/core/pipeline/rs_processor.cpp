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

#include "rs_processor.h"
#include <memory>

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_display_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_base_render_util.h"
#include "rs_main_thread.h"
#include "params/rs_display_render_params.h"

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif
#ifdef FRAME_AWARE_TRACE
#include "render_frame_trace.h"

using namespace FRAME_TRACE;
#endif

namespace OHOS {
namespace Rosen {
bool RSProcessor::needDisableMultiLayersPerf_ = false;
namespace {
constexpr uint32_t PERF_LEVEL_0 = 0;
constexpr uint32_t PERF_LEVEL_1 = 1;
constexpr uint32_t PERF_LEVEL_2 = 2;
constexpr int32_t PERF_LEVEL_1_REQUESTED_CODE = 10013;
constexpr int32_t PERF_LEVEL_2_REQUESTED_CODE = 10014;
constexpr int32_t PERF_LEVEL_3_REQUESTED_CODE = 10015;
constexpr int64_t PERF_TIME_OUT = 950;
constexpr uint32_t PERF_LEVEL_INTERVAL = 10;
constexpr uint32_t PERF_LAYER_START_NUM = 12;
#ifdef FRAME_AWARE_TRACE
constexpr uint32_t FRAME_TRACE_LAYER_NUM_1 = 11;
constexpr uint32_t FRAME_TRACE_LAYER_NUM_2 = 13;
constexpr int32_t FRAME_TRACE_PERF_REQUESTED_CODE = 10024;
#endif

void PerfRequest(int32_t perfRequestCode, bool onOffTag)
{
#ifdef SOC_PERF_ENABLE
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(perfRequestCode, onOffTag, "");
    RS_LOGD("RSProcessor::soc perf info [%{public}d %{public}d]", perfRequestCode, onOffTag);
#endif
}
}

#ifdef FRAME_AWARE_TRACE
bool RSProcessor::FrameAwareTraceBoost(size_t layerNum)
{
    RenderFrameTrace& ft = RenderFrameTrace::GetInstance();
    if (layerNum != FRAME_TRACE_LAYER_NUM_1 && layerNum != FRAME_TRACE_LAYER_NUM_2) {
        if (ft.RenderFrameTraceIsOpen()) {
            ft.RenderFrameTraceClose();
            PerfRequest(FRAME_TRACE_PERF_REQUESTED_CODE, false);
            RS_LOGD("RsDebug RSProcessor::Perf: FrameTrace 0");
        }
        return false;
    }

    static std::chrono::steady_clock::time_point lastRequestPerfTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    bool isTimeOut = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRequestPerfTime).
        count() > PERF_TIME_OUT;
    if (isTimeOut || !ft.RenderFrameTraceIsOpen()) {
        if (!ft.RenderFrameTraceOpen()) {
            return false;
        }
        PerfRequest(FRAME_TRACE_PERF_REQUESTED_CODE, true);
        RS_LOGD("RsDebug RSProcessor::Perf: FrameTrace 1");
        lastRequestPerfTime = currentTime;
    }
    return true;
}
#endif

void RSProcessor::RequestPerf(uint32_t layerLevel, bool onOffTag)
{
    switch (layerLevel) {
        case PERF_LEVEL_0: {
            // do nothing
            RS_LOGD("RsDebug RSProcessor::Perf: perf do nothing");
            break;
        }
        case PERF_LEVEL_1: {
            PerfRequest(PERF_LEVEL_1_REQUESTED_CODE, onOffTag);
            RS_LOGD("RsDebug RSProcessor::Perf: level1 %{public}d", onOffTag);
            break;
        }
        case PERF_LEVEL_2: {
            PerfRequest(PERF_LEVEL_2_REQUESTED_CODE, onOffTag);
            RS_LOGD("RsDebug RSProcessor::Perf: level2 %{public}d", onOffTag);
            break;
        }
        default: {
            PerfRequest(PERF_LEVEL_3_REQUESTED_CODE, onOffTag);
            RS_LOGD("RsDebug RSProcessor::Perf: level3 %{public}d", onOffTag);
            break;
        }
    }
}

bool RSProcessor::InitForRenderThread(RSDisplayRenderNode& node, ScreenId mirroredId, std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    auto params = static_cast<RSDisplayRenderParams*>(node.GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSProcessor::InitForRenderThread params is null!");
        return false;
    }

    offsetX_ = params->GetDisplayOffsetX();
    offsetY_ = params->GetDisplayOffsetY();
    mirroredId_ = mirroredId;

    screenInfo_ = params->GetrScreenInfo();
    screenInfo_.rotation = params->GetNodeRotation();

    renderEngine_ = renderEngine;
    if (renderEngine_ == nullptr) {
        RS_LOGE("renderEngine is nullptr");
        return false;
    }

    // CalculateScreenTransformMatrix
    auto mirrorNode = params->GetMirrorSource().lock();

    if (!mirrorNode) {
        screenTransformMatrix_ = params->GetMatrix();
    } else {
        auto mirrorNodeParam = static_cast<RSDisplayRenderParams*>(mirrorNode->GetRenderParams().get());
        screenTransformMatrix_ = mirrorNodeParam->GetMatrix();
        if (mirroredId_ != INVALID_SCREEN_ID) {
            auto mirroredScreenInfo = mirrorNodeParam->GetrScreenInfo();
            CalculateMirrorAdaptiveCoefficient(
                static_cast<float>(screenInfo_.width), static_cast<float>(screenInfo_.height),
                static_cast<float>(mirroredScreenInfo.width), static_cast<float>(mirroredScreenInfo.height)
            );
        }
    }

    // set default render frame config
    renderFrameConfig_ = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_);
    return true;
}

bool RSProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
    std::shared_ptr<RSBaseRenderEngine> renderEngine, bool isRenderThread)
{
    if (isRenderThread) {
        return InitForRenderThread(node, mirroredId, renderEngine);
    }
    offsetX_ = offsetX;
    offsetY_ = offsetY;
    mirroredId_ = mirroredId;
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSPhysicalScreenProcessor::Init: ScreenManager is nullptr");
        return false;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    screenInfo_.rotation = node.GetRotation();
    auto mirrorNode = node.GetMirrorSource().lock();
    CalculateScreenTransformMatrix(mirrorNode ? *mirrorNode : node);

    renderEngine_ = renderEngine;
    if (renderEngine_ == nullptr) {
        RS_LOGE("renderEngine is nullptr");
        return false;
    }

    if (mirroredId_ != INVALID_SCREEN_ID) {
        auto mirroredScreenInfo = screenManager->QueryScreenInfo(mirroredId_);
        CalculateMirrorAdaptiveCoefficient(
            static_cast<float>(screenInfo_.width), static_cast<float>(screenInfo_.height),
            static_cast<float>(mirroredScreenInfo.width), static_cast<float>(mirroredScreenInfo.height)
        );
    }

    // set default render frame config
    renderFrameConfig_ = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_);
    return true;
}

void RSProcessor::SetMirrorScreenSwap(const RSDisplayRenderNode& node)
{
    auto mirroredNode = node.GetMirrorSource().lock();
    if (mirroredNode == nullptr) {
        RS_LOGE("RSProcessor::Init: Get mirroredNode failed");
        return;
    }
    if (mirroredNode->GetRotation() == ScreenRotation::ROTATION_90 ||
        mirroredNode->GetRotation() == ScreenRotation::ROTATION_270) {
        std::swap(screenInfo_.width, screenInfo_.height);
        std::swap(renderFrameConfig_.width, renderFrameConfig_.height);
    }
}

void RSProcessor::CalculateScreenTransformMatrix(const RSDisplayRenderNode& node)
{
    auto boundsGeoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (boundsGeoPtr != nullptr) {
        boundsGeoPtr->UpdateByMatrixFromSelf();
        screenTransformMatrix_ = boundsGeoPtr->GetMatrix();
    }
}

void RSProcessor::CalculateMirrorAdaptiveCoefficient(float curWidth, float curHeight,
    float mirroredWidth, float mirroredHeight)
{
    if (std::fabs(mirroredWidth) < 1e-6 || std::fabs(mirroredHeight) < 1e-6) {
        RS_LOGE("RSSoftwareProcessor::Init mirroredScreen width or height is zero");
        return;
    }
    mirrorAdaptiveCoefficient_ = std::min(curWidth / mirroredWidth, curHeight / mirroredHeight);
}

void RSProcessor::MirrorScenePerf()
{
    needDisableMultiLayersPerf_ = true;
    static std::chrono::steady_clock::time_point lastRequestPerfTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    bool isTimeOut = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRequestPerfTime).
        count() > PERF_TIME_OUT;
    if (isTimeOut) {
        PerfRequest(PERF_LEVEL_3_REQUESTED_CODE, true);
        lastRequestPerfTime = currentTime;
    }
}

void RSProcessor::MultiLayersPerf(size_t layerNum)
{
    if (needDisableMultiLayersPerf_) {
        auto& context = RSMainThread::Instance()->GetContext();
        std::shared_ptr<RSBaseRenderNode> rootNode = context.GetGlobalRootRenderNode();
        if (rootNode && rootNode->GetChildrenCount() <= 1) {
            needDisableMultiLayersPerf_ = false;
        } else {
            return;
        }
    }
#ifdef FRAME_AWARE_TRACE
    if (FrameAwareTraceBoost(layerNum)) {
        return;
    }
#endif
    static uint32_t lastLayerLevel = 0;
    static std::chrono::steady_clock::time_point lastRequestPerfTime = std::chrono::steady_clock::now();
    auto curLayerLevel = layerNum / PERF_LEVEL_INTERVAL;
    if (curLayerLevel == 0 && layerNum >= PERF_LAYER_START_NUM) {
        curLayerLevel = 1;
    }
    auto currentTime = std::chrono::steady_clock::now();
    bool isTimeOut = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRequestPerfTime).
        count() > PERF_TIME_OUT;
    if (curLayerLevel != lastLayerLevel || isTimeOut) {
        if (!isTimeOut) {
            RequestPerf(lastLayerLevel, false);
        }
        RequestPerf(curLayerLevel, true);
        lastLayerLevel = curLayerLevel;
        lastRequestPerfTime = currentTime;
    }
}

void RSProcessor::SetSecurityDisplay(bool isSecurityDisplay)
{
    isSecurityDisplay_ = isSecurityDisplay;
}

void RSProcessor::SetDisplayHasSecSurface(bool displayHasSecSurface)
{
    displayHasSecSurface_ = displayHasSecSurface;
}

} // namespace Rosen
} // namespace OHOS
