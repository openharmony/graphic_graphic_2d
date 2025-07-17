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

#include "render_thread/rs_base_render_util.h"
#include "main_thread/rs_main_thread.h"

#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "params/rs_screen_render_params.h"
#include "params/rs_logical_display_render_params.h"
#include "pipeline/rs_screen_render_node.h"
#include "platform/common/rs_log.h"

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif
#ifdef FRAME_AWARE_TRACE
#include "render_frame_trace.h"

using namespace FRAME_TRACE;
#endif

namespace OHOS {
namespace Rosen {

bool RSProcessor::InitForRenderThread(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
    std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
#ifdef RS_ENABLE_GPU
    if (renderEngine == nullptr) {
        RS_LOGE("renderEngine is nullptr");
        return false;
    }
    auto& params = screenDrawable.GetRenderParams();
    if (params == nullptr) {
        RS_LOGE("RSProcessor::InitForRenderThread params is null!");
        return false;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(params.get());
    offsetX_ = screenParams->GetScreenOffsetX();
    offsetY_ = screenParams->GetScreenOffsetY();
    renderEngine_ = renderEngine;
    screenInfo_ = screenParams->GetScreenInfo();

    // set default render frame config
    renderFrameConfig_ = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_);
#endif
    return true;
}
    
bool RSProcessor::UpdateMirrorInfo(DrawableV2::RSLogicalDisplayRenderNodeDrawable& displayDrawable)
{
    auto& params = displayDrawable.GetRenderParams();
    if (params == nullptr) {
        RS_LOGE("RSProcess::UpdateMirrorInfo params is null!");
        return false;
    }
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(params.get());
    screenInfo_.rotation = displayParams->GetNodeRotation();
    // CalculateScreenTransformMatrix
    auto mirroredNodeDrawable = displayParams->GetMirrorSourceDrawable().lock();
    isMirror_ = mirroredNodeDrawable != nullptr;
    if (mirroredNodeDrawable && mirroredNodeDrawable->GetRenderParams()) {
        auto& mirrorNodeParam = mirroredNodeDrawable->GetRenderParams();
        CalculateMirrorAdaptiveCoefficient(static_cast<float>(params->GetBounds().GetWidth()),
            static_cast<float>(params->GetBounds().GetHeight()),
            static_cast<float>(mirrorNodeParam->GetBounds().GetWidth()),
            static_cast<float>(mirrorNodeParam->GetBounds().GetHeight()));
    }
    return true;
}

bool RSProcessor::Init(RSScreenRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
    std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    if (renderEngine == nullptr) {
        RS_LOGE("renderEngine is nullptr");
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSPhysicalScreenProcessor::Init: ScreenManager is nullptr");
        return false;
    }
    renderEngine_ = renderEngine;
    offsetX_ = offsetX;
    offsetY_ = offsetY;
    mirroredId_ = mirroredId;
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());

    auto mirrorNode = node.GetMirrorSource().lock();
    CalculateScreenTransformMatrix(mirrorNode ? *mirrorNode : node);

    if (mirroredId_ != INVALID_SCREEN_ID) {
        mirroredScreenInfo_ = screenManager->QueryScreenInfo(mirroredId_);
        CalculateMirrorAdaptiveMatrix();
    }

    // set default render frame config
    renderFrameConfig_ = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_);
    return true;
}

void RSProcessor::SetMirrorScreenSwap(const RSScreenRenderNode& node)
{
    auto mirroredNode = node.GetMirrorSource().lock();
    if (mirroredNode == nullptr) {
        RS_LOGE("RSProcessor::Init: Get mirroredNode failed");
        return;
    }
}

void RSProcessor::CalculateScreenTransformMatrix(const RSScreenRenderNode& node)
{
    auto& boundsGeoPtr = (node.GetRenderProperties().GetBoundsGeometry());
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

void RSProcessor::SetSecurityDisplay(bool isSecurityDisplay)
{
    isSecurityDisplay_ = isSecurityDisplay;
}

void RSProcessor::SetDisplayHasSecSurface(bool displayHasSecSurface)
{
    displayHasSecSurface_ = displayHasSecSurface;
}

void RSProcessor::CalculateMirrorAdaptiveMatrix()
{
    CalculateMirrorAdaptiveCoefficient(static_cast<float>(screenInfo_.GetRotatedWidth()),
        static_cast<float>(screenInfo_.GetRotatedHeight()), static_cast<float>(mirroredScreenInfo_.GetRotatedWidth()),
        static_cast<float>(mirroredScreenInfo_.GetRotatedHeight()));

    float rotation = 0.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    switch (screenInfo_.rotation) {
        case ScreenRotation::ROTATION_90:
            rotation = -90.0f;
            offsetX = screenInfo_.GetRotatedWidth() * -1.0f;
            break;
        case ScreenRotation::ROTATION_180:
            rotation = -180.0f;
            offsetX = screenInfo_.GetRotatedWidth() * -1.0f;
            offsetY = screenInfo_.GetRotatedHeight() * -1.0f;
            break;
        case ScreenRotation::ROTATION_270:
            rotation = -270.0f;
            offsetY = screenInfo_.GetRotatedHeight() * -1.0f;
            break;
        default:
            break;
    }

    // align center
    offsetX +=
        (screenInfo_.GetRotatedWidth() - mirroredScreenInfo_.GetRotatedWidth() * mirrorAdaptiveCoefficient_) / 2.0f;
    offsetY +=
        (screenInfo_.GetRotatedHeight() - mirroredScreenInfo_.GetRotatedHeight() * mirrorAdaptiveCoefficient_) /
        2.0f;

    mirrorAdaptiveMatrix_.PreRotate(rotation);
    mirrorAdaptiveMatrix_.PreTranslate(offsetX, offsetY);
    mirrorAdaptiveMatrix_.PreScale(mirrorAdaptiveCoefficient_, mirrorAdaptiveCoefficient_);
}

} // namespace Rosen
} // namespace OHOS
