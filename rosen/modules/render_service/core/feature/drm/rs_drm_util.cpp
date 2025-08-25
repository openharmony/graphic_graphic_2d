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
#include <parameters.h>
#include "common/rs_background_thread.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_effect_render_node.h"
#include "graphic_feature_param_manager.h"

namespace OHOS {
namespace Rosen {
constexpr const int BLUR_MIN_ALPHA = 100;

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
        surfaceNode->GetScreenNodeId() != INVALID_NODEID) {
        if (const auto it = drmNodes_.find(leashWindowNode->GetId()); it != drmNodes_.end()) {
            for (const auto& node : it->second) {
                if (node->IsOnTheTree()) {
                    hardwareEnabledDrawables.emplace_back(surfaceNode->GetScreenNodeId(),
                        node->GetLogicalDisplayNodeId(), node->GetRenderDrawable());
                }
            }
        }
    }
}

void RSDrmUtil::DRMCreateLayer(std::shared_ptr<RSProcessor> processor, Drawing::Matrix canvasMatrix)
{
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& [screenNodeId, _, drawable] : hardwareDrawables) {
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

void RSDrmUtil::PreAllocateProtectedBuffer(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    const std::shared_ptr<RSSurfaceHandler>& surfaceHandler)
{
    auto displayLock = surfaceNode->GetAncestorScreenNode().lock();
    std::shared_ptr<RSScreenRenderNode> ancestor = nullptr;
    if (displayLock != nullptr) {
        ancestor = displayLock->ReinterpretCastTo<RSScreenRenderNode>();
    }
    if (ancestor == nullptr) {
        return;
    }
    auto protectedLayerScreenId = ancestor->GetScreenId();
    auto screenManager = CreateOrGetScreenManager();

    auto output = screenManager->GetOutput(ToScreenPhysicalId(protectedLayerScreenId));
    if (UNLIKELY(output == nullptr)) {
        RS_LOGE("output is NULL");
        return;
    }
    if (output->GetProtectedFrameBufferState()) {
        return;
    }
    auto protectedBuffer = surfaceHandler->GetBuffer();
    if (UNLIKELY(protectedBuffer == nullptr)) {
        RS_LOGE("buffer is NULL");
        return;
    }
    auto preAllocateProtectedBufferTask = [buffer = protectedBuffer, screenId = protectedLayerScreenId]() {
        RSHardwareThread::Instance().PreAllocateProtectedBuffer(buffer, screenId);
    };
    RSBackgroundThread::Instance().PostTask(preAllocateProtectedBufferTask);
}

void RSDrmUtil::MarkBlurIntersectWithDRM(const std::shared_ptr<RSRenderNode>& node,
    const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& drmNodes,
    const std::shared_ptr<RSScreenRenderNode>& curScreenNode)
{
    if (node == nullptr) {
        return;
    }
    static bool isMockAllBlurEffectIntersectWithDRMEnabled = DRMParam::IsMockAllBlurEffectIntersectWithDRMEnable();
    if (isMockAllBlurEffectIntersectWithDRMEnabled) {
        // mark all blurEffect for pc
        MarkAllBlurIntersectWithDRM(node, drmNodes, curScreenNode);
    } else {
        MarkBlurIntersectWithDRM(node, drmNodes);
    }
}

void RSDrmUtil::MarkAllBlurIntersectWithDRM(const std::shared_ptr<RSRenderNode>& node,
    const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& drmNodes,
    const std::shared_ptr<RSScreenRenderNode>& curScreenNode)
{
    if (curScreenNode == nullptr) {
        return;
    }
    
    if (node->GetRenderProperties().GetBackgroundBlurMaskColor().GetAlpha() >= BLUR_MIN_ALPHA) {
        // if alpha is greater than 100, blurEffect is not transparent
        return;
    }
    auto appWindowNodeId = node->GetInstanceRootNodeId();
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto appWindowNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(appWindowNodeId));
    if (appWindowNode == nullptr) {
        return;
    }
    static std::vector<std::string> drmKeyWins = DRMParam::GetBlackList();
    for (const auto& win : drmKeyWins) {
        if (appWindowNode->GetName().find(win) != std::string::npos) {
            return;
        }
    }
    for (auto& drmNode : drmNodes) {
        auto drmNodePtr = drmNode.lock();
        if (drmNodePtr == nullptr) {
            continue;
        }
        bool isIntersect =
            drmNodePtr->GetRenderProperties().GetBoundsGeometry()->GetAbsRect().Intersect(node->GetFilterRegion());
        bool isVisible = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
            drmNodePtr->GetInstanceRootNode())->GetVisibleRegion().IsEmpty();
        if (isIntersect && !isVisible && IsDRMBelowFilter(curScreenNode, appWindowNode, drmNodePtr)) {
            node->MarkBlurIntersectWithDRM(true, GetDarkColorMode(node, appWindowNode));
        }
    }
}

void RSDrmUtil::MarkBlurIntersectWithDRM(const std::shared_ptr<RSRenderNode>& node,
    const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& drmNodes)
{
    auto appWindowNodeId = node->GetInstanceRootNodeId();
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto appWindowNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(appWindowNodeId));
    if (appWindowNode == nullptr) {
        return;
    }
    static std::vector<std::string> drmKeyWins = DRMParam::GetWhiteList();
    auto effectNode = node->ReinterpretCastTo<RSEffectRenderNode>() ?
        node->ReinterpretCastTo<RSEffectRenderNode>() : nullptr;
    if (effectNode) {
        effectNode->SetEffectIntersectWithDRM(false);
        effectNode->SetDarkColorMode(RSMainThread::Instance()->GetGlobalDarkColorMode());
    }
    for (const auto& win : drmKeyWins) {
        if (appWindowNode->GetName().find(win) == std::string::npos) {
            continue;
        }
        for (auto& drmNode : drmNodes) {
            auto drmNodePtr = drmNode.lock();
            if (drmNodePtr == nullptr) {
                continue;
            }
            bool isIntersect =
                drmNodePtr->GetRenderProperties().GetBoundsGeometry()->GetAbsRect().Intersect(node->GetFilterRegion());
            if (!isIntersect) {
                continue;
            }
            node->MarkBlurIntersectWithDRM(true, RSMainThread::Instance()->GetGlobalDarkColorMode());
            if (effectNode) {
                effectNode->SetEffectIntersectWithDRM(true);
            }
        }
    }
}

bool RSDrmUtil::IsDRMBelowFilter(const std::shared_ptr<RSScreenRenderNode>& curScreenNode,
    const std::shared_ptr<RSSurfaceRenderNode>& appWindowNode,
    const std::shared_ptr<RSSurfaceRenderNode>& drmNode)
{
    auto& curMainAndLeashSurfaces = curScreenNode->GetAllMainAndLeashSurfaces();
    auto filterNodeIndex = 0;
    auto drmNodeIndex = 0;
    for (size_t i = 0; i < curMainAndLeashSurfaces.size(); ++i) {
        auto leashWindowNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(curMainAndLeashSurfaces[i]);
        if (appWindowNode == leashWindowNode) {
            filterNodeIndex = i;
        }
        auto drmLeashNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(drmNode->GetInstanceRootNode());
        if (drmLeashNode == leashWindowNode) {
            drmNodeIndex = i;
        }
    }
    return drmNodeIndex > filterNodeIndex;
}

bool RSDrmUtil::GetDarkColorMode(const std::shared_ptr<RSRenderNode>& node,
    const std::shared_ptr<RSSurfaceRenderNode>& appWindowNode)
{
    bool isDarkColorMode = RSMainThread::Instance()->GetGlobalDarkColorMode();
    if (appWindowNode->GetName().find("SCBSmartDock") != std::string::npos) {
        float saturation = node->GetRenderProperties().GetBgBrightnessSaturation();
        isDarkColorMode = ROSEN_GNE(saturation, 1.4f) ? false : true;
    }
    return isDarkColorMode;
}
} // namespace Rosen
} // namespace OHOS