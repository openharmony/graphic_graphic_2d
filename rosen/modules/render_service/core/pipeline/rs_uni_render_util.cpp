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
void RSUniRenderUtil::UpdateRenderNodeDstRect(RSRenderNode& node, const SkMatrix& matrix)
{
    // [planning] use RSRenderNode::Update instead
    auto parentNode = node.GetParent().lock();
    if (!parentNode) {
        RS_LOGE("RSUniRenderUtil::UpdateDstRect: fail to get parent dstRect.");
        return;
    }
    auto rsParent = parentNode->ReinterpretCastTo<RSRenderNode>();
    auto parentProp = rsParent ? &(rsParent->GetRenderProperties()) : nullptr;
    auto& property = node.GetMutableRenderProperties();
    auto surfaceNode = node.ReinterpretCastTo<RSSurfaceRenderNode>();
    auto isSelfDrawingNode = surfaceNode &&
        (surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE ||
        surfaceNode->GetSurfaceNodeType() == RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    Vector2f offset(0.f, 0.f);
    if (isSelfDrawingNode) {
        offset = { parentProp->GetFrameOffsetX(), parentProp->GetFrameOffsetY() };
    }
    property.UpdateGeometry(parentProp, true, offset);
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (geoPtr && node.IsInstanceOf<RSSurfaceRenderNode>()) {
        if (!isSelfDrawingNode) {
            geoPtr->ConcatMatrix(matrix);
        }
        RS_LOGD("RSUniRenderUtil::UpdateDstRect: nodeName: %s, dstRect[%s].",
            surfaceNode->GetName().c_str(), surfaceNode->GetDstRect().ToString().c_str());
    }
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
        if (surfaceNode->GetDstRect().IsInsideOf(surfaceDirtyManager->GetDirtyRegion())) {
            node->GetDirtyManager()->MergeDirtyRect(surfaceNode->GetDstRect());
        }
        if (!node->GetDirtyManager()->GetDirtyRegion().IntersectRect(surfaceNode->GetDstRect()).IsEmpty() &&
            ((surfaceNode->IsTransparent() && surfaceNode->GetRenderProperties().GetBackgroundFilter()) ||
            surfaceNode->GetRenderProperties().GetFilter())) {
            node->GetDirtyManager()->MergeDirtyRect(surfaceNode->GetDstRect());
        }
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

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSSurfaceRenderNode& node, bool forceCPU)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
    params.paint.setAntiAlias(true);

    const RSProperties& property = node.GetRenderProperties();
    params.dstRect = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());

    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());

    RectF localBounds = {0.0f, 0.0f, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(node, localBounds, params);
    return params;
}

} // namespace Rosen
} // namespace OHOS
