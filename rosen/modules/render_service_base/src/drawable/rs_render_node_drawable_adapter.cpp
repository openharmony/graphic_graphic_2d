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

namespace OHOS::Rosen::DrawableV2 {
std::unordered_map<RSRenderNodeType, RSRenderNodeDrawableAdapter::Generator> RSRenderNodeDrawableAdapter::GeneratorMap;

RSRenderNodeDrawableAdapter::RSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode>&& node)
    : renderNode_(std::move(node)) {};

RSRenderNodeDrawableAdapter::SharedPtr RSRenderNodeDrawableAdapter::OnGenerate(
    const std::shared_ptr<const RSRenderNode>& node)
{
    static std::unordered_map<NodeId, WeakPtr> RenderNodeDrawableCache;
    static const auto Destructor = [](RSRenderNodeDrawableAdapter* ptr) {
        RenderNodeDrawableCache.erase(ptr->renderNode_->GetId()); // Remove from cache before deleting
        delete ptr;
    };

    if (node == nullptr) {
        return nullptr;
    }
    auto id = node->GetId();

    // Try to get a cached drawable if it exists.
    if (const auto cacheIt = RenderNodeDrawableCache.find(id); cacheIt != RenderNodeDrawableCache.end()) {
        if (const auto ptr = cacheIt->second.lock()) {
            return ptr;
        } else {
            RenderNodeDrawableCache.erase(cacheIt);
        }
    }

    // If we don't have a cached drawable, try to generate a new one and cache it.
    const auto it = GeneratorMap.find(node->GetType());
    if (it == GeneratorMap.end()) {
        ROSEN_LOGE("RSRenderNodeDrawableAdapter::OnGenerate, node type %d is not supported", node->GetType());
        return nullptr;
    }
    auto ptr = it->second(node);
    auto sharedPtr = std::shared_ptr<RSRenderNodeDrawableAdapter>(ptr, Destructor);
    RenderNodeDrawableCache.emplace(id, sharedPtr);
    return sharedPtr;
}
} // namespace OHOS::Rosen::DrawableV2
