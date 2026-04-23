/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "feature/dynamic_layer_skip/rs_dynamic_layer_skip_controller.h"

namespace OHOS {
namespace Rosen {
void LayerSkipContext::SyncFrom(const RSDynamicLayerSkipController& controller)
{
    return;
}

void LayerSkipContext::Reset()
{
    return;
}

bool RSDynamicLayerSkipController::IsScreenLayerInvalid() const
{
    return false;
}

void RSDynamicLayerSkipController::Init(const RectI& screenRect, bool globalDisabled)
{
    globalDisabled_ = globalDisabled;
    screenRect_ = screenRect;
    screenLayerInvalid_ = false;
    globalOccluderDetected_ = false;
    occluderInstanceRootNodeId_ = INVALID_NODEID;
    fullScreenSelfDrawingSurface_.clear();
    visitedRenderNodeCount_ = 0;
}

void RSDynamicLayerSkipController::CheckNodeDrawProperty(RSRenderNode& node)
{
    return;
}

void RSDynamicLayerSkipController::MarkParentSubTreeHasDrawContent(RSRenderNode& node) const
{
    return;
}

bool RSDynamicLayerSkipController::HasDrawContentDrawableInRange(
    const RSRenderNode& node, RSDrawableSlot begin, RSDrawableSlot end) const
{
    return false;
}

void RSDynamicLayerSkipController::VisitRenderNode(std::shared_ptr<RSSurfaceRenderNode> surfaceNode, RSRenderNode& node)
{
    return;
}

bool RSDynamicLayerSkipController::HasFullScreenSelfDrawingSurface(RSSurfaceRenderNode& rootNode) const
{
    return false;
}

void RSDynamicLayerSkipController::DetectScreenLayerValidity(RSSurfaceRenderNode& rootNode)
{
    return;
}

void RSDynamicLayerSkipController::VerifyScreenLayerValidity(float screenNodeGlobalZOrder)
{
    return;
}
} // namespace Rosen
} // namespace OHOS