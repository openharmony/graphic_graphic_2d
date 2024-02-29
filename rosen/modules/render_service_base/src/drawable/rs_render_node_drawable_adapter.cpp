/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_render_node_drawable_adapter.h"

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
std::unordered_map<RSRenderNodeType, RSRenderNodeDrawableAdapter::Generator> RSRenderNodeDrawableAdapter::GeneratorMap;

RSRenderNodeDrawableAdapter::Ptr RSRenderNodeDrawableAdapter::OnGenerate(
    const std::shared_ptr<const RSRenderNode>& node)
{
    if (node == nullptr) {
        // Empty child should be filtered on RSRenderNode::GenerateFullChildrenList, we should not reach here
        ROSEN_LOGE("RSRenderNodeDrawableAdapter::OnGenerate, node is null");
        return nullptr;
    }
    if (const auto it = GeneratorMap.find(node->GetType()); it != GeneratorMap.end()) {
        return it->second(node);
    }
    return nullptr;
}

} // namespace OHOS::Rosen
