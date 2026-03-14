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

void RSSpecialLayerUtils::CheckSpecialLayerIntersectMirrorDisplay(const RSLogicalDisplayRenderNode& mirrorNode,
    RSLogicalDisplayRenderNode& sourceNode)
{
    // Use screen AbsDrawRect when VisibleRect is not enabled
    ScreenId screenId = mirrorNode.GetScreenId();
    RectI visibleRect = sourceNode.GetAbsDrawRect();
    auto mirrorScreenNode = std::static_pointer_cast<RSScreenRenderNode>(mirrorNode.GetParent().lock());
    if (mirrorScreenNode) {
        const auto& screenProperty = mirrorScreenNode->GetScreenProperty();
        if (screenProperty.GetEnableVisibleRect()) {
            const auto& rect = screenProperty.GetVisibleRect();
            visibleRect = {rect.x, rect.y, rect.w, rect.h};
        }
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

bool RSSpecialLayerUtils::NeedProcessSecLayerInDisplay(bool enableVisibleRect, RSScreenRenderParams& mirrorScreenParam,
    RSLogicalDisplayRenderParams& mirrorParam, RSLogicalDisplayRenderParams& sourceParam)
{
    // Skip security layer processing if: has security exemption, not a security display, or
    // GetVirtualSecLayerOption() != 0 ( 0 = full screen black, 1 = window black )
    if (mirrorParam.GetSecurityExemption() ||
        !mirrorParam.IsSecurityDisplay() ||
        mirrorScreenParam.GetScreenProperty().GetVirtualSecLayerOption() != 0) {
        return false;
    }
    return enableVisibleRect ? mirrorParam.HasSecLayerInVisibleRect() :
        sourceParam.GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
}

bool RSSpecialLayerUtils::HasMirrorDisplay(const RSRenderNodeMap& nodeMap)
{
    bool hasMirrorDisplay = false;
    nodeMap.TraverseLogicalDisplayNodes(
        [&hasMirrorDisplay](const std::shared_ptr<RSLogicalDisplayRenderNode>& displayRenderNode) {
            if (displayRenderNode != nullptr && displayRenderNode->GetMirrorSource().lock()) {
                hasMirrorDisplay = true;
            }
        }
    );
    return hasMirrorDisplay;
}
} // namespace Rosen
} // namespace OHOS