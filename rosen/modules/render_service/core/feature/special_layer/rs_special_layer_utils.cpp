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

#include <sstream>

#include "common/rs_special_layer_manager.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::unordered_set<uint64_t> RSSpecialLayerUtils::GetAllBlackList(const RSRenderNodeMap& nodeMap)
{
    std::unordered_set<uint64_t> allBlackList;
    nodeMap.TraverseScreenNodes(
        [&allBlackList](const std::shared_ptr<RSScreenRenderNode>& screenRenderNode) {
        if (screenRenderNode != nullptr) {
            auto currentBlackList = RSSpecialLayerUtils::GetMergeBlackList(screenRenderNode->GetScreenProperty());
            allBlackList.insert(currentBlackList.begin(), currentBlackList.end());
        }
    });
    return allBlackList;
}

std::unordered_set<uint64_t> RSSpecialLayerUtils::GetAllWhiteList(const RSRenderNodeMap& nodeMap)
{
    std::unordered_set<uint64_t> allWhiteList;
    nodeMap.TraverseScreenNodes(
        [&allWhiteList](const std::shared_ptr<RSScreenRenderNode>& screenRenderNode) {
        if (screenRenderNode != nullptr) {
            auto currentWhiteList = screenRenderNode->GetScreenProperty().GetWhiteList();
            allWhiteList.insert(currentWhiteList.begin(), currentWhiteList.end());
        }
    });
    return allWhiteList;
}

void RSSpecialLayerUtils::DumpScreenSpecialLayer(const std::string& funcName,
    SpecialLayerType type, ScreenId screenId, const std::unordered_set<NodeId>& nodeIds)
{
    std::ostringstream out;
    for (const auto nodeId : nodeIds) {
        out << nodeId << " ";
    }
    RS_LOGI("%{public}s : specialLayerType[%{public}" PRIu32 "] screenId[%{public}" PRIu64 "] set[%{public}s]",
        funcName.c_str(), type, screenId, out.str().c_str());
}

void RSSpecialLayerUtils::UpdateScreenSpecialLayer(
    const RSScreenProperty& curProperty, const RSScreenProperty& preProperty)
{
    std::unordered_map<SpecialLayerType, std::unordered_set<NodeId>> screenSpecialLayerInfos;
    
    // check black list
    auto preBlackList = GetMergeBlackList(preProperty);
    auto curBlackList = GetMergeBlackList(curProperty);
    if (preBlackList != curBlackList) {
        screenSpecialLayerInfos[SpecialLayerType::IS_BLACK_LIST] = curBlackList;
    }

    // check white list
    auto preWhiteList = preProperty.GetWhiteList();
    auto curWhiteList = curProperty.GetWhiteList();
    if (preWhiteList != curWhiteList) {
        screenSpecialLayerInfos[SpecialLayerType::IS_WHITE_LIST] = curWhiteList;
    }
    
    auto screenId = curProperty.GetScreenId();
    for (const auto& [type, nodeIds]: screenSpecialLayerInfos) {
        ScreenSpecialLayerInfo::Update(type, screenId, nodeIds);
    }
}

std::unordered_set<NodeId> RSSpecialLayerUtils::GetMergeBlackList(const RSScreenProperty& screenProperty)
{
    if (!screenProperty.EnableSkipWindow()) {
        return screenProperty.GetBlackList();
    }
    std::unordered_set<NodeId> blackList = screenProperty.GetBlackList();
    const auto& globalBlackList = ScreenSpecialLayerInfo::GetGlobalBlackList();
    blackList.insert(globalBlackList.begin(), globalBlackList.end());
    return blackList;
}

void RSSpecialLayerUtils::DealWithSpecialLayer(
    RSSurfaceRenderNode& surfaceNode, RSLogicalDisplayRenderNode& displayNode, bool needCalcScreenSpecialLayer)
{
    UpdateScreenSpecialLayersRecord(surfaceNode, displayNode, needCalcScreenSpecialLayer);
    if (surfaceNode.IsCloneCrossNode()) {
        auto sourceNode = surfaceNode.GetSourceCrossNode().lock();
        auto sourceSurface = sourceNode ? sourceNode->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
        if (sourceSurface == nullptr) {
            return;
        }
        UpdateSpecialLayersRecord(*sourceSurface, displayNode);
    } else {
        UpdateSpecialLayersRecord(surfaceNode, displayNode);
    }
}

void RSSpecialLayerUtils::UpdateScreenSpecialLayersRecord(
    RSSurfaceRenderNode& surfaceNode, RSLogicalDisplayRenderNode& displayNode, bool needCalcScreenSpecialLayer)
{
    surfaceNode.GetMultableSpecialLayerMgr().ClearScreenSpecialLayer();
    if (!needCalcScreenSpecialLayer) {
        return;
    }
    // update whitelist
    surfaceNode.UpdateVirtualScreenWhiteListInfo();
    // update blacklist
    auto screenIds = ScreenSpecialLayerInfo::QueryEnableScreen(
        SpecialLayerType::IS_BLACK_LIST, {surfaceNode.GetId(), surfaceNode.GetLeashPersistentId()});
    for (const auto screenId : screenIds) {
        surfaceNode.SetScreenSpecialLayerStatus(screenId, SpecialLayerType::IS_BLACK_LIST, true);
        displayNode.GetMultableSpecialLayerMgr().AddIdsWithScreen(
            screenId, SpecialLayerType::IS_BLACK_LIST, surfaceNode.GetId());
    }
}

void RSSpecialLayerUtils::UpdateSpecialLayersRecord(
    RSSurfaceRenderNode& surfaceNode, RSLogicalDisplayRenderNode& displayNode)
{
    if (!surfaceNode.ShouldPaint()) {
        return;
    }
    auto specialLayerMgr = surfaceNode.GetMultableSpecialLayerMgr();
    if (specialLayerMgr.Find(SpecialLayerType::HAS_SECURITY)) {
        displayNode.AddSecurityLayer(
            surfaceNode.IsLeashWindow() ? surfaceNode.GetLeashPersistentId() : surfaceNode.GetId());
        displayNode.AddSecurityVisibleLayer(surfaceNode.GetId());
    }
    auto specialLayerType = specialLayerMgr.Get();
    if (surfaceNode.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
        specialLayerType &= ~SpecialLayerType::HAS_SKIP;
    }
    displayNode.GetMultableSpecialLayerMgr().AddIds((specialLayerType >> SPECIAL_TYPE_NUM), surfaceNode.GetId());
    if (surfaceNode.IsSpecialLayerChanged()) {
        displayNode.SetDisplaySpecialSurfaceChanged(true);
    }
}
} // namespace Rosen
} // namespace OHOS