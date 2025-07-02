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
}
}