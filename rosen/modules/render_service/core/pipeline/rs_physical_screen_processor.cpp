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
#include "string_utils.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSPhysicalScreenProcessor::RSPhysicalScreenProcessor()
    : composerAdapter_(std::make_unique<RSComposerAdapter>())
{
}

RSPhysicalScreenProcessor::~RSPhysicalScreenProcessor() noexcept
{
}

bool RSPhysicalScreenProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
                                     std::shared_ptr<RSBaseRenderEngine> renderEngine, bool isRenderThread)
{
    // TODO adapt isRenderThread
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId, renderEngine, isRenderThread)) {
        return false;
    }

    if (mirroredId != INVALID_SCREEN_ID) {
        SetMirrorScreenSwap(node);
    }

    return composerAdapter_->Init(screenInfo_, offsetX, offsetY, mirrorAdaptiveCoefficient_,
        [this](const auto& surface, const auto& layers) { Redraw(surface, layers); });
}

void RSPhysicalScreenProcessor::PostProcess(RSDisplayRenderNode* node)
{
    composerAdapter_->CommitLayers(layers_);
    MultiLayersPerf(layers_.size());
}

void RSPhysicalScreenProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    auto layer = composerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGD("RSPhysicalScreenProcessor::ProcessSurface: failed to createLayer for"
            " node(id: %{public}" PRIu64 ")", node.GetId());
        return;
    }

    layers_.emplace_back(layer);
}

void RSPhysicalScreenProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    RS_LOGI("RSPhysicalScreenProcessor::ProcessDisplaySurface() is not supported.");
}

void RSPhysicalScreenProcessor::ProcessRcdSurface(RSRcdSurfaceRenderNode& node)
{
    RS_LOGI("RSPhysicalScreenProcessor::ProcessRcdSurface() is not supported");
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
        RS_LOGE("RsDebug RSPhysicalScreenProcessor::Redraw: failed to request frame.");
        return;
    }

    auto canvas = renderFrame->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE("RsDebug RSPhysicalScreenProcessor::Redraw: canvas is nullptr.");
        return;
    }
    canvas->ConcatMatrix(screenTransformMatrix_);
    renderEngine_->DrawLayers(*canvas, layers, forceCPU, mirrorAdaptiveCoefficient_);
    renderFrame->Flush();
    RS_LOGD("RsDebug RSPhysicalScreenProcessor::Redraw flush frame buffer end");
}
} // namespace Rosen
} // namespace OHOS
