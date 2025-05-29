/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_drm_util.h"

#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"

namespace OHOS {
namespace Rosen {

void RSDrmUtil::ClearDrmNodes()
{
    drmNodes_.clear();
}
void RSDrmUtil::CollectDrmNodes(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
        surfaceNode->GetFirstLevelNode());
    if (firstLevelNode && firstLevelNode->IsOnTheTree()) {
        drmNodes_[firstLevelNode->GetId()].push_back(surfaceNode);
    }
}

void RSDrmUtil::AddDrmCloneCrossNode(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    DrawablesVec& hardwareEnabledDrawables)
{
    auto sourceNode = surfaceNode->GetSourceCrossNode().lock();
    auto sourceSurface = sourceNode == nullptr ? nullptr : sourceNode->ReinterpretCastTo<RSSurfaceRenderNode>();
    auto leashWindowNode = sourceSurface == nullptr ? nullptr :
        RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(sourceSurface->GetFirstLevelNode());
    if (leashWindowNode && leashWindowNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED) &&
        surfaceNode->GetDisplayNodeId() != INVALID_NODEID) {
        if (const auto it = drmNodes_.find(leashWindowNode->GetId()); it != drmNodes_.end()) {
            for (const auto& node : it->second) {
                if (node->IsOnTheTree()) {
                    hardwareEnabledDrawables.emplace_back(surfaceNode->GetDisplayNodeId(), node->GetRenderDrawable());
                }
            }
        }
    }
}

void RSDrmUtil::DRMCreateLayer(std::shared_ptr<RSProcessor> processor, Drawing::Matrix canvasMatrix)
{
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& [displayNodeId, drawable] : hardwareDrawables) {
        if (UNLIKELY(!drawable) || !drawable->GetRenderParams()) {
            continue;
        }
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        auto& params = surfaceDrawable->GetRenderParams();
        if (UNLIKELY(!params)) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(params.get());
        if (UNLIKELY(!surfaceParams) || !surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED)) {
            continue;
        }
        // calculate matrix
        auto layerInfo = surfaceParams->GetLayerInfo();
        Drawing::Matrix tmpMatrix = surfaceParams->GetTotalMatrix();
        tmpMatrix.PostConcat(canvasMatrix);
        layerInfo.matrix = tmpMatrix;
        // calculate dstRect
        Drawing::Rect absRect;
        tmpMatrix.MapRect(absRect, surfaceParams->GetBounds());
        layerInfo.dstRect.x = absRect.left_;
        layerInfo.dstRect.y = absRect.top_;
        layerInfo.dstRect.w = absRect.right_ - absRect.left_;
        layerInfo.dstRect.h = absRect.bottom_ - absRect.top_;
        // calculate transformType
        GraphicTransformType rotateEnum =
            RSBaseRenderUtil::RotateEnumToInt(RSUniRenderUtil::GetRotationFromMatrix(tmpMatrix) % 360,
                RSBaseRenderUtil::GetFlipTransform(layerInfo.transformType));
        layerInfo.transformType = rotateEnum;
        surfaceParams->SetLayerInfo(layerInfo);
        processor->CreateLayerForRenderThread(*surfaceDrawable);
    }
}
} // namespace Rosen
} // namespace OHOS