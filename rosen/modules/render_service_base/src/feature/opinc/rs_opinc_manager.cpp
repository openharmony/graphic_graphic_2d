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
    if (node.GetSharedTransitionParam() ||
        property.IsSpherizeValid() ||
        property.IsAttractionValid() ||
        property.NeedFilter() ||
        property.GetUseEffect() ||
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