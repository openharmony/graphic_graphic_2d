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

#include "rs_uni_render_processor.h"

#include "rs_trace.h"
#include "string_utils.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSUniRenderProcessor::RSUniRenderProcessor()
    : composerAdapter_(std::make_unique<RSComposerAdapter>())
{
}

RSUniRenderProcessor::~RSUniRenderProcessor() noexcept
{
}

bool RSUniRenderProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId)) {
        return false;
    }

    // In uni render mode, we can handle screen rotation in the rendering process,
    // so we do not need to handle rotation in composer adapter any more,
    // just pass the buffer to composer straightly.
    screenInfo_.rotation = ScreenRotation::ROTATION_0;
    return composerAdapter_->Init(screenInfo_, offsetX, offsetY, mirrorAdaptiveCoefficient_,
        [this](const auto& surface, const auto& layers) {
        Redraw(surface, layers);
    });
}

void RSUniRenderProcessor::PostProcess()
{
    composerAdapter_->CommitLayers(layers_);
    MultiLayersPerf(layerNum);
}

void RSUniRenderProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    if (!node.IsNotifyRTBufferAvailable()) {
        // Only ipc for one time.
        RS_LOGD("RsDebug RSUniRenderProcessor::ProcessSurface id = %" PRIu64 " Notify RT buffer available",
            node.GetId());
        node.NotifyRTBufferAvailable();
    }

    auto layer = composerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessSurface: failed to createLayer for node(id: %" PRIu64 ")", node.GetId());
        return;
    }

    layers_.emplace_back(layer);
    layerNum++;
}

void RSUniRenderProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    auto layer = composerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessDisplaySurface: failed to createLayer for node(id: %" PRIu64 ")",
            node.GetId());
        return;
    }

    layers_.emplace_back(layer);
    layerNum += node.GetCurAllSurfaces().size();
}

void RSUniRenderProcessor::Redraw(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers)
{
    if (surface == nullptr) {
        RS_LOGE("RSUniRenderProcessor::Redraw: surface is null.");
        return;
    }

    RS_LOGD("RsDebug RSUniRenderProcessor::Redraw flush frame buffer start");
    bool forceCPU = RSBaseRenderEngine::NeedForceCPU(layers);
    auto renderFrame = renderEngine_->RequestFrame(surface, renderFrameConfig_, forceCPU);
    if (renderFrame == nullptr) {
        RS_LOGE("RsDebug RSUniRenderProcessor::Redraw：failed to request frame.");
        return;
    }

    auto canvas = renderFrame->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE("RsDebug RSUniRenderProcessor::Redraw：canvas is nullptr.");
        return;
    }
    renderEngine_->DrawLayers(*canvas, layers, forceCPU);
    renderFrame->Flush();
    RS_LOGD("RsDebug RSUniRenderProcessor::Redraw flush frame buffer end");
}
} // namespace Rosen
} // namespace OHOS
