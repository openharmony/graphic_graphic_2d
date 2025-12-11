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

#include "rs_special_layer_utils.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {

void RSSpecialLayerUtils::CheckSpecialLayerIntersectMirrorDisplay(const RSLogicalDisplayRenderNode& mirrorNode,
    RSLogicalDisplayRenderNode& sourceNode, bool enableVisibleRect)
{
    // Use screen AbsDrawRect when VisibleRect is not enabled
    ScreenId screenId = mirrorNode.GetScreenId();
    RectI visibleRect = sourceNode.GetAbsDrawRect();
    auto screenManager = CreateOrGetScreenManager();
    if (enableVisibleRect && screenManager) {
        const auto& rect = screenManager->GetMirrorScreenVisibleRect(screenId);
        visibleRect = {rect.x, rect.y, rect.w, rect.h};
    }

    const auto& specialLayerMgr = sourceNode.GetSpecialLayerMgr();
    auto totalSpecialLayerType = specialLayerMgr.Get() | SpecialLayerType::HAS_BLACK_LIST;
    // Only one bit representing the currently processed type
    uint32_t currentType = SpecialLayerType::HAS_SECURITY;
    // skip security process if virtual screen is security exemption
    currentType = mirrorNode.GetSecurityExemption() ? currentType << 1 : currentType;
    bool hasSlInVisibleRect = false;
    // Terminate loop when currentType is outside the totalSpecialLayerType
    while (currentType <= totalSpecialLayerType && !hasSlInVisibleRect) {
        bool isCurrentTypeEnable = (currentType & totalSpecialLayerType) != 0;
        if (isCurrentTypeEnable) {
            const auto nodeIds = currentType & HAS_SCREEN_SPECIAL ?
                specialLayerMgr.GetIdsWithScreen(screenId, currentType) : specialLayerMgr.GetIds(currentType);
            hasSlInVisibleRect = CheckCurrentTypeIntersectVisibleRect(nodeIds, currentType, visibleRect);
        }
        currentType <<= 1;
    }
    
    sourceNode.GetMultableSpecialLayerMgr().SetHasSlInVisibleRect(screenId, hasSlInVisibleRect);
}

bool RSSpecialLayerUtils::CheckCurrentTypeIntersectVisibleRect(const std::unordered_set<NodeId>& nodeIds,
    uint32_t currentType, const RectI& visibleRect)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    bool hasSlInVisibleRect = false;
    for (auto nodeId : nodeIds) {
        const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
            nodeMap.GetRenderNode(nodeId));
        if (surfaceNode == nullptr) {
            continue;
        }
        const auto& absDrawRect = surfaceNode->GetAbsDrawRect();
        // skip screen intersection test for skip-layer-only capture window nodes
        if (currentType == SpecialLayerType::HAS_SKIP &&
            surfaceNode->GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
            continue;
        }
        if (absDrawRect.Intersect(visibleRect) || surfaceNode->IsFirstLevelCrossNode()) {
            RS_TRACE_NAME_FMT("CheckSpecialLayerIntersectMirrorDisplay node:[%s], IsFirstLevelCrossNode:[%d]",
                surfaceNode->GetName().c_str(), surfaceNode->IsFirstLevelCrossNode());
            hasSlInVisibleRect = true;
            break;
        }
    }
    return hasSlInVisibleRect;
}

DisplaySpecialLayerState RSSpecialLayerUtils::GetSpecialLayerStateInVisibleRect(
    RSLogicalDisplayRenderParams* displayParams, RSScreenRenderParams* screenParams)
{
    const auto& specialLayerManager = displayParams->GetSpecialLayerMgr();
    ScreenId screenId = displayParams->GetScreenId();
    const auto& screenProperty = screenParams->GetScreenProperty();
    bool hasGeneralSpecialLayer = !screenProperty.GetWhiteList().empty() ||
        !screenProperty.GetTypeBlackList().empty() || specialLayerManager.GetHasSlInVisibleRect(screenId);
    RS_LOGD("%{public}sWhiteList:%{public}d, TypeBlackList:%{public}d, "
        "HasSlInVisibleRect:%{public}d", __func__, !screenProperty.GetWhiteList().empty(),
        !screenProperty.GetTypeBlackList().empty(), specialLayerManager.GetHasSlInVisibleRect(screenId));
    
    return hasGeneralSpecialLayer ? DisplaySpecialLayerState::HAS_SPECIAL_LAYER :
        DisplaySpecialLayerState::NO_SPECIAL_LAYER;
}

DisplaySpecialLayerState RSSpecialLayerUtils::GetSpecialLayerStateInSubTree(
    RSLogicalDisplayRenderParams& displayParams, RSScreenRenderParams* screenParams)
{
    auto& uniRenderThread = RSUniRenderThread::Instance();
    const auto& specialLayerManager = displayParams.GetSpecialLayerMgr();
    bool hasGeneralSpecialLayer = specialLayerManager.Find(VIRTUALSCREEN_HAS_SPECIAL) ||
        screenParams->GetHDRPresent() || uniRenderThread.IsColorFilterModeOn();
    RS_LOGD("%{public}s:%{public}" PRIu32 ", CurtainScreen:%{public}d, "
        "HDRPresent:%{public}d, ColorFilter:%{public}d", __func__, specialLayerManager.Get(),
        uniRenderThread.IsCurtainScreenOn(), screenParams->GetHDRPresent(), uniRenderThread.IsColorFilterModeOn());
    
    ScreenId screenId = displayParams.GetScreenId();
    const auto& screenProperty = screenParams->GetScreenProperty();
    if (RSUniRenderThread::GetCaptureParam().isSnapshot_) {
        hasGeneralSpecialLayer |= (specialLayerManager.Find(SpecialLayerType::HAS_SNAPSHOT_SKIP) ||
            uniRenderThread.IsCurtainScreenOn());
    } else {
        hasGeneralSpecialLayer |= !screenProperty.GetWhiteList().empty() || !screenProperty.GetTypeBlackList().empty()
            || specialLayerManager.FindWithScreen(screenId, SpecialLayerType::HAS_BLACK_LIST);
    }

    if (hasGeneralSpecialLayer) {
        return DisplaySpecialLayerState::HAS_SPECIAL_LAYER;
    }
    return displayParams.HasCaptureWindow() ? DisplaySpecialLayerState::CAPTURE_WINDOW :
        DisplaySpecialLayerState::NO_SPECIAL_LAYER;
}
} // namespace Rosen
} // namespace OHOS