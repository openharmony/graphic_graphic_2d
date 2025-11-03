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

#include "feature/opinc/rs_opinc_manager.h"

#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_render_node.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {

RSOpincManager& RSOpincManager::Instance()
{
    static RSOpincManager instance;
    return instance;
}

bool RSOpincManager::OpincGetNodeSupportFlag(RSRenderNode& node)
{
    auto nodeType = node.GetType();
    auto supportFlag = false;
    if (nodeType == RSRenderNodeType::CANVAS_NODE) {
        supportFlag = OpincGetCanvasNodeSupportFlag(node);
    } else if (nodeType == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        supportFlag = !node.GetOpincCache().IsSuggestOpincNode() && OpincGetCanvasNodeSupportFlag(node);
    }

    node.GetOpincCache().SetCurNodeTreeSupportFlag(supportFlag);
    return supportFlag;
}

bool RSOpincManager::OpincGetCanvasNodeSupportFlag(RSRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto canvasNode = node.ReinterpretCastTo<RSCanvasRenderNode>();
    bool isHDRNode = canvasNode != nullptr && canvasNode->GetHDRPresent();
    if (isHDRNode ||
        node.GetSharedTransitionParam() ||
        property.IsSpherizeValid() ||
        property.IsAttractionValid() ||
        property.NeedFilter() ||
        property.GetUseEffect() ||
        property.HasHarmonium() ||
        property.GetColorBlend().has_value() ||
        node.ChildHasVisibleFilter() ||
        node.ChildHasVisibleEffect()) {
        return false;
    }
    return node.GetOpincCache().GetSubTreeSupportFlag();
}

bool RSOpincManager::IsOpincSubTreeDirty(RSRenderNode& node, bool opincEnable)
{
    auto subTreeDirty = node.GetOpincCache().OpincForcePrepareSubTree(opincEnable,
        node.IsSubTreeDirty() || node.IsContentDirty(), OpincGetNodeSupportFlag(node));
    if (subTreeDirty) {
        node.SetParentSubTreeDirty();
    }
    return subTreeDirty;
}

void RSOpincManager::QuickMarkStableNode(RSRenderNode& node, bool& unchangeMarkInApp, bool& unchangeMarkEnable,
    bool isAccessibilityConfigChanged)
{
    auto& opincCache = node.GetOpincCache();
    // The opinc state needs to be reset in the following cases:
    // 1. subtree is dirty
    // 2. content is dirty
    // 3. the node is marked as a node group
    // 4. the node is marked as a root node of opinc and the high-contrast state change
    auto isSelfDirty = node.IsSubTreeDirty() || node.IsContentDirty() ||
        node.GetNodeGroupType() > RSRenderNode::NodeGroupType::NONE ||
        (opincCache.OpincGetRootFlag() && isAccessibilityConfigChanged);
    opincCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, isSelfDirty);
    auto& stagingRenderParams = node.GetStagingRenderParams();
    if (unchangeMarkEnable && stagingRenderParams) {
        stagingRenderParams->OpincSetCacheChangeFlag(opincCache.GetCacheChangeFlag(), node.GetLastFrameSync());
        stagingRenderParams->OpincUpdateRootFlag(opincCache.OpincGetRootFlag());
        stagingRenderParams->OpincUpdateSupportFlag(opincCache.GetCurNodeTreeSupportFlag());
    }
}

void RSOpincManager::UpdateRootFlag(RSRenderNode& node, bool& unchangeMarkEnable)
{
    auto& opincCache = node.GetOpincCache();
    opincCache.OpincUpdateRootFlag(unchangeMarkEnable, OpincGetNodeSupportFlag(node));
    auto& stagingRenderParams = node.GetStagingRenderParams();
    if (opincCache.isOpincRootFlag_ && stagingRenderParams) {
        stagingRenderParams->OpincUpdateRootFlag(true);
    }
}

OpincUnsupportType RSOpincManager::GetUnsupportReason(RSRenderNode& node)
{
    if (!node.GetOpincCache().GetSubTreeSupportFlag()) {
        return OpincUnsupportType::CHILD_NOT_SUPPORT;
    }
    if (node.GetSharedTransitionParam()) {
        return OpincUnsupportType::SHARED_TRANSITION;
    }
    const auto& property = node.GetRenderProperties();
    if (property.IsSpherizeValid()) {
        return OpincUnsupportType::SPHERIZE;
    }
    if (property.IsAttractionValid()) {
        return OpincUnsupportType::ATTRACTION;
    }
    if (property.NeedFilter()) {
        return OpincUnsupportType::HAS_FILTER;
    }
    if (property.GetUseEffect()) {
        return OpincUnsupportType::USE_EFFECT;
    }
    if (property.HasHarmonium()) {
        return OpincUnsupportType::HAS_HARMONIUM;
    }
    if (property.GetColorBlend().has_value()) {
        return OpincUnsupportType::COLOR_BLEND;
    }
    if (node.ChildHasVisibleFilter()) {
        return OpincUnsupportType::CHILD_HAS_FILTER;
    }
    if (node.ChildHasVisibleEffect()) {
        return OpincUnsupportType::CHILD_HAS_EFFECT;
    }
    return OpincUnsupportType::NONE;
}

std::string RSOpincManager::QuickGetNodeDebugInfo(RSRenderNode& node)
{
    std::string ret("");
    auto& opincCache = node.GetOpincCache();
    AppendFormat(ret, "%" PRIu64 ", subD:%d conD:%d s:%d uc:%d suggest:%d support:%d rootF:%d not_sup_reason:%d",
        node.GetId(), node.IsSubTreeDirty(), node.IsContentDirty(), opincCache.GetNodeCacheState(),
        opincCache.GetUnchangeCount(), opincCache.IsSuggestOpincNode(), opincCache.GetCurNodeTreeSupportFlag(),
        opincCache.OpincGetRootFlag(), GetUnsupportReason(node));
    return ret;
}
}
}