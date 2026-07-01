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

#include "pipeline/rs_effect_utils.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
bool RSEffectUtils::AccumulateFilterRenderContext(RSRenderNode& node, const RSRenderNode& rootNode,
    FilterRenderContext& context)
{
    auto selfGeo = node.GetRenderProperties().GetBoundsGeometry();
    if (!selfGeo) {
        return false;
    }

    Drawing::Matrix selfMatrix = selfGeo->GetMatrix();
    auto directParent = node.GetParent().lock();
    NodeId offscreenNodeId = RSEffectUtils::IsOffscreenForFilterCache(node) ? node.GetId() : INVALID_NODEID;

    while (directParent && directParent->GetId() != rootNode.GetId()) {
        if (auto parentGeo = directParent->GetRenderProperties().GetBoundsGeometry()) {
            selfMatrix.PostConcat(parentGeo->GetMatrix());
        }

        if (RSEffectUtils::IsOffscreenForFilterCache(*directParent) && offscreenNodeId == INVALID_NODEID) {
            offscreenNodeId = directParent->GetId();
        }
        directParent = directParent->GetParent().lock();
    }

    if (!directParent) {
        return false;
    }

    if (offscreenNodeId != INVALID_NODEID) {
        context.offscreenNodeId = offscreenNodeId;
    }
    selfMatrix.PostConcat(context.absMatrix);
    context.absMatrix = selfMatrix;
    return true;
}

bool RSEffectUtils::IsOffscreenForFilterCache(RSRenderNode& node)
{
    // align with quickprepare offscreenCanvasNodeId_ logic, only check canvasRenderNode and unionRenderNode.
    return (node.GetType() == RSRenderNodeType::CANVAS_NODE || node.GetType() == RSRenderNodeType::UNION_NODE) && (
        node.GetDrawingCacheType() == RSDrawingCacheType::FOREGROUND_FILTER_CACHE || node.IsForegroundFilterEnable());
}

bool RSEffectUtils::HasBackgroundDependentFilter(const RSProperties& properties)
{
    return properties.GetBackgroundFilter() || properties.GetMaterialFilter() || properties.GetNeedDrawBehindWindow();
}
} // namespace Rosen
} // namespace OHOS
