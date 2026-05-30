/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "feature/layer/rs_layer_cache_manager_base.h"

constexpr int ENABLE_LAYER_FRAME_NUM = 3;

namespace OHOS {
namespace Rosen {
    std::vector<std::shared_ptr<DrawableV2::RSRenderNodeDrawableAdapter>> RSLayerCacheManagerBase::layerDrawables_;
    std::unordered_map<NodeId, bool> RSLayerCacheManagerBase::unSupportLayerNodeMap_;
    int RSLayerCacheManagerBase::layerFrameCount_ = 0;
    bool RSLayerCacheManagerBase::isLayerSuggested_ = false;
    std::vector<std::weak_ptr<RSRenderNode>> RSLayerCacheManagerBase::suggestedLayerNodes_;

    void RSLayerCacheManagerBase::ProcessLayerNodes()
    {
        if (!isLayerSuggested_) {
            layerFrameCount_ = 0;
            unSupportLayerNodeMap_.clear();
            return;
        }
        layerFrameCount_++;
        if (layerFrameCount_ == ENABLE_LAYER_FRAME_NUM) {
            for (auto node : suggestedLayerNodes_) {
                auto nodePtr = node.lock();
                if (!isNodeUnSupportLayer(nodePtr)) {
                    nodePtr->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_LAYER, true, false);
                }
            }
        }
    }

    bool RSLayerCacheManagerBase::isNodeUnSupportLayer(std::shared_ptr<RSRenderNode> node)
    {
        bool isUnSupportLayer = unSupportLayerNodeMap_.find(node->GetId()) != unSupportLayerNodeMap_.end() &&
                                unSupportLayerNodeMap_[node->GetId()];
        return isUnSupportLayer;
    }

    bool RSLayerCacheManagerBase::isNodeUnSupportLayer(RSRenderNode& node)
    {
        bool isUnSupportLayer = unSupportLayerNodeMap_.find(node.GetId()) != unSupportLayerNodeMap_.end() &&
                                unSupportLayerNodeMap_[node.GetId()];
        return isUnSupportLayer;
    }
} // namespace Rosen
} // namespace OHOS