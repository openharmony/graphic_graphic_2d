/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_uni_render_util.h"

#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
bool RSUniRenderUtil::UpdateRenderNodeDstRect(RSRenderNode& node, const SkMatrix& matrix)
{
    // [planning] use RSRenderNode::Update instead
    auto parentNode = node.GetParent().lock();
    if (!parentNode) {
        RS_LOGE("RSUniRenderUtil::UpdateDstRect: fail to get parent dstRect.");
        return false;
    }
    auto rsParent = parentNode->ReinterpretCastTo<RSRenderNode>();
    auto pareneProp = rsParent ? &(rsParent->GetRenderProperties()) : nullptr;
    auto& property = node.GetMutableRenderProperties();
    auto transitionProperties = node.GetAnimationManager().GetTransitionProperties();
    auto surfaceNode = node.ReinterpretCastTo<RSSurfaceRenderNode>();
    auto isSurfaceView = surfaceNode && surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    Vector2f offset(0.f, 0.f);
    if (isSurfaceView) {
        offset = { pareneProp->GetFrameOffsetX(), pareneProp->GetFrameOffsetY() };
    }
    property.UpdateGeometry(
        pareneProp, true, offset, transitionProperties);
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (geoPtr && node.IsInstanceOf<RSSurfaceRenderNode>()) {
        if (!isSurfaceView) {
            geoPtr->ConcatMatrix(matrix);
        }
        surfaceNode->SetDstRect(geoPtr->GetAbsRect());
        RS_LOGD("RSUniRenderUtil::UpdateDstRect: nodeName: %s, dstRect[%s].",
            surfaceNode->GetName().c_str(), surfaceNode->GetDstRect().ToString().c_str());
    }
    return transitionProperties != nullptr;
}

void RSUniRenderUtil::MergeDirtyHistory(std::shared_ptr<RSDisplayRenderNode>& node, int32_t bufferAge)
{
    // update all child surfacenode history
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        if (!surfaceDirtyManager->SetBufferAge(bufferAge)) {
            ROSEN_LOGE("RSUniRenderUtil::MergeVisibleDirtyRegion with invalid buffer age %d", bufferAge);
        }
        surfaceDirtyManager->IntersectDirtyRect(surfaceNode->GetDstRect());
        surfaceDirtyManager->UpdateDirty();
    }
    // update display dirtymanager
    node->UpdateDisplayDirtyManager(bufferAge);
}

Occlusion::Region RSUniRenderUtil::MergeVisibleDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node)
{
    Occlusion::Region allSurfaceVisibleDirtyRegion;
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        auto surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegion();
        Occlusion::Rect dirtyRect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
            surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
        auto visibleRegion = surfaceNode->GetVisibleRegion();
        Occlusion::Region surfaceDirtyRegion { dirtyRect };
        Occlusion::Region surfaceVisibleDirtyRegion = surfaceDirtyRegion.And(visibleRegion);
        surfaceNode->SetVisibleDirtyRegion(surfaceVisibleDirtyRegion);
        allSurfaceVisibleDirtyRegion = allSurfaceVisibleDirtyRegion.Or(surfaceVisibleDirtyRegion);
    }
    return allSurfaceVisibleDirtyRegion;
}
} // namespace Rosen
} // namespace OHOS
