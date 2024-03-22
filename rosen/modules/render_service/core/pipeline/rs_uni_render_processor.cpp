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

#include "pipeline/round_corner_display/rs_rcd_surface_render_node.h"

namespace OHOS {
namespace Rosen {
RSUniRenderProcessor::RSUniRenderProcessor()
    : uniComposerAdapter_(std::make_unique<RSUniRenderComposerAdapter>())
{
}

RSUniRenderProcessor::~RSUniRenderProcessor() noexcept
{
}

bool RSUniRenderProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
                                std::shared_ptr<RSBaseRenderEngine> renderEngine, bool isRenderThread)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId, renderEngine, isRenderThread)) {
        return false;
    }
    // In uni render mode, we can handle screen rotation in the rendering process,
    // so we do not need to handle rotation in composer adapter any more,
    // just pass the buffer to composer straightly.
    screenInfo_.rotation = ScreenRotation::ROTATION_0;
    isPhone_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    return uniComposerAdapter_->Init(screenInfo_, offsetX_, offsetY_, mirrorAdaptiveCoefficient_);
}

void RSUniRenderProcessor::PostProcess(RSDisplayRenderNode* node)
{
    if (node != nullptr) {
        auto acquireFence = node->GetAcquireFence();
        auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
        for (auto surfaceNode : selfDrawingNodes) {
            if (!surfaceNode->IsCurrentFrameHardwareEnabled()) {
                // current frame : gpu
                // use display node's acquire fence as release fence to ensure not release buffer until gpu finish
                surfaceNode->SetCurrentReleaseFence(acquireFence);
                if (surfaceNode->IsLastFrameHardwareEnabled()) {
                    // last frame : hwc
                    // use display node's acquire fence as release fence to ensure not release buffer until its real
                    // release fence signals
                    surfaceNode->SetReleaseFence(acquireFence);
                }
            }
        }
    }
    uniComposerAdapter_->CommitLayers(layers_);
    if (!isPhone_) {
        MultiLayersPerf(layerNum);
    }
    RS_LOGD("RSUniRenderProcessor::PostProcess layers_:%{public}zu", layers_.size());
}

void RSUniRenderProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessSurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    node.MarkCurrentFrameHardwareEnabled();
    layers_.emplace_back(layer);
}

void RSUniRenderProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessDisplaySurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    if (node.GetFingerprint()) {
        layer->SetLayerMaskInfo(HdiLayerInfo::LayerMask::LAYER_MASK_HBM_SYNC);
        RS_LOGD("RSUniRenderProcessor::ProcessDisplaySurface, set layer mask hbm sync");
    } else {
        layer->SetLayerMaskInfo(HdiLayerInfo::LayerMask::LAYER_MASK_NORMAL);
    }
    layers_.emplace_back(layer);
    for (auto surface : node.GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surface);
        if (!surfaceNode || !surfaceNode->GetOcclusionVisible() || surfaceNode->IsLeashWindow()) {
            continue;
        }
        layerNum++;
    }
}

void RSUniRenderProcessor::ProcessRcdSurface(RSRcdSurfaceRenderNode& node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessRcdSurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    layers_.emplace_back(layer);
}

} // namespace Rosen
} // namespace OHOS
