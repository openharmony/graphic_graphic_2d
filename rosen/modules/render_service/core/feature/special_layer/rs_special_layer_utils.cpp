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
#include <unordered_set>

namespace OHOS {
namespace Rosen {
std::unordered_set<uint64_t> RSSpecialLayerUtils::GetAllBlackList(const RSRenderNodeMap& nodeMap)
{
    std::unordered_set<uint64_t> allBlackList;
    nodeMap.TraverseScreenNodes(
        [&allBlackList](const std::shared_ptr<RSScreenRenderNode>& screenRenderNode) {
        if (screenRenderNode != nullptr) {
            auto currentBlackList = screenRenderNode->GetScreenProperty().GetMergeBlackList();
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
} // namespace Rosen
} // namespace OHOS