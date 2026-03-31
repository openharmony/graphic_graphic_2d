/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_RENDER_RS_LAYER_CACHE_MANAGER_H
#define RENDER_SERVICE_BASE_RENDER_RS_LAYER_CACHE_MANAGER_H

#include <memory>
#include <string>
#include <vector>

#include "drawable/rs_canvas_render_node_drawable.h"
#include "feature/layer/rs_layer_cache_manager_base.h"

namespace OHOS {
namespace Rosen {
namespace DrawableV2 {
class RSRenderNodeDrawable;
}

class RSLayerCacheManager : public RSLayerCacheManagerBase {
public:
    static RSLayerCacheManager& Instance();

    // disable copy and move
    RSLayerCacheManager(const RSLayerCacheManager&) = delete;
    RSLayerCacheManager& operator=(const RSLayerCacheManager&) = delete;
    RSLayerCacheManager(RSLayerCacheManager&&) = delete;
    RSLayerCacheManager& operator=(RSLayerCacheManager&&) = delete;

    void IfIsLayerNodeAddToLayerNodeDrawables(std::shared_ptr<RSRenderNode> node);
    void HandleLayerDrawables(Drawing::Canvas& canvas);
    void TryPrepareLayerCache(std::shared_ptr<DrawableV2::RSCanvasRenderNodeDrawable> drawable, Drawing::Canvas& canvas);
    void AddNodeToLayerNodes(std::shared_ptr<RSRenderNode> node) override
    {
        layerNodes_.emplace_back(node);
    }

    void ClearLayerNodes()
    {
        layerNodes_.clear();
    }

    std::vector<std::shared_ptr<DrawableV2::RSRenderNodeDrawableAdapter>> layerNodeDrawables_;
private:
    RSLayerCacheManager() = default;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RS_LAYER_CACHE_MANAGER_H
