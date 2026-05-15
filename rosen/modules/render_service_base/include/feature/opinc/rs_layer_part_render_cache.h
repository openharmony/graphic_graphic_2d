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

#ifndef RS_LAYER_PART_RENDER_CACHE_H
#define RS_LAYER_PART_RENDER_CACHE_H

#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "pipeline/rs_dirty_region_manager.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSLayerPartRenderCache {
public:
    RSLayerPartRenderCache() = default;
    ~RSLayerPartRenderCache() = default;

    void MarkSuggestLayerPartRenderNode(bool isLayerPartRender)
    {
        isSuggestLayerPartRenderNode_ = isLayerPartRender;
    }

    bool IsSuggestLayerPartRenderNode() const
    {
        return isSuggestLayerPartRenderNode_;
    }

    void SetLayerPartRender(bool isLayerPartRender)
    {
        isLayerPartRender_ = isLayerPartRender;
    }

    bool IsLayerPartRender() const
    {
        return isLayerPartRender_;
    }

    void SetLayerPartRenderNodeStrategyType(NodeStrategyType type)
    {
        layerPartRenderNodeStrategyType_ = type;
    }

    NodeStrategyType GetLayerPartRenderNodeStrategyType() const
    {
        return layerPartRenderNodeStrategyType_;
    }

    bool IsLayerPartRenderUnchangeState()
    {
        constexpr int32_t MIN_UNCHANGE_COUNT = 3;
        if (layerPartRenderUnchangeCount_ <= MIN_UNCHANGE_COUNT) {
            layerPartRenderUnchangeCount_++;
            return false;
        }
        return true;
    }

    void ResetLayerPartRenderUnchangeState()
    {
        layerPartRenderUnchangeCount_ = 0;
    }

    std::shared_ptr<RSDirtyRegionManager>& GetLayerPartRenderDirtyManager()
    {
        if (layerPartRenderDirtyManager_ == nullptr) {
            layerPartRenderDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
        }
        return layerPartRenderDirtyManager_;
    }

private:
    bool isSuggestLayerPartRenderNode_ = false;
    bool isLayerPartRender_ = false;
    NodeStrategyType layerPartRenderNodeStrategyType_ = NodeStrategyType::CACHE_NONE;
    int32_t layerPartRenderUnchangeCount_ = 0;
    std::shared_ptr<RSDirtyRegionManager> layerPartRenderDirtyManager_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_LAYER_PART_RENDER_CACHE_H
