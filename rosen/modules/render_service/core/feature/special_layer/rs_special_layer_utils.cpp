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
using GetFunc = std::function<std::unordered_set<NodeId>(const RSScreenProperty&)>;
namespace {
const std::map<ScreenPropertyType, SpecialLayerType> SCREEN_SPECIAL_LAYER_PROPERTY = {
    {ScreenPropertyType::ENABLE_SKIP_WINDOW, SpecialLayerType::IS_BLACK_LIST},
    {ScreenPropertyType::BLACK_LIST, SpecialLayerType::IS_BLACK_LIST},
    {ScreenPropertyType::WHITE_LIST, SpecialLayerType::IS_WHITE_LIST}
};

const GetFunc GET_WHITE_LIST = [](const auto& screenProperty) { return screenProperty.GetWhiteList(); };

const std::map<SpecialLayerType, GetFunc> GET_SCREEN_SPECIAL_LAYER = {
    {SpecialLayerType::IS_BLACK_LIST, RSSpecialLayerUtils::GetMergeBlackList},
    {SpecialLayerType::IS_WHITE_LIST, GET_WHITE_LIST}
};
}

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

void RSSpecialLayerUtils::UpdateInfoWithGlobalBlackList(const RSRenderNodeMap& nodeMap)
{
    nodeMap.TraverseScreenNodes(
        [](std::shared_ptr<RSScreenRenderNode> screenRenderNode) {
            if (screenRenderNode == nullptr) {
                return;
            }
            auto screenProperty = screenRenderNode->GetScreenProperty();
            if (!screenProperty.EnableSkipWindow()) {
                return;
            }
            ScreenSpecialLayerInfo::Update(
                SpecialLayerType::IS_BLACK_LIST, screenProperty.GetScreenId(), GetMergeBlackList(screenProperty));
        }
    );
    NotifyScreenSpecialLayerChange();
}

void RSSpecialLayerUtils::UpdateScreenSpecialLayer(const RSScreenProperty& screenProperty)
{
    for (auto [type, getFunc] : GET_SCREEN_SPECIAL_LAYER) {
        ScreenSpecialLayerInfo::Update(type, screenProperty.GetScreenId(), getFunc(screenProperty));
    }
    NotifyScreenSpecialLayerChange();
}

void RSSpecialLayerUtils::UpdateScreenSpecialLayer(const RSScreenProperty& screenProperty, ScreenPropertyType type)
{
    auto propertIter = SCREEN_SPECIAL_LAYER_PROPERTY.find(type);
    if (propertIter == SCREEN_SPECIAL_LAYER_PROPERTY.end()) {
        return;
    }
    SpecialLayerType specialLayerType = propertIter->second;
    auto funcIter = GET_SCREEN_SPECIAL_LAYER.find(specialLayerType);
    if (funcIter == GET_SCREEN_SPECIAL_LAYER.end()) {
        return;
    }
    GetFunc getfunc = funcIter->second;
    ScreenSpecialLayerInfo::Update(specialLayerType, screenProperty.GetScreenId(), getfunc(screenProperty));
    NotifyScreenSpecialLayerChange();
}

void RSSpecialLayerUtils::NotifyScreenSpecialLayerChange()
{
    // used to mark changes in special layers for other features
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