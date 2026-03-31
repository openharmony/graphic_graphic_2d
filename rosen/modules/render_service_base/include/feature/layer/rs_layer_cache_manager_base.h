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

#ifndef RENDER_SERVICE_BASE_RENDER_RS_LAYER_CACHE_MANAGER_BASE_H
#define RENDER_SERVICE_BASE_RENDER_RS_LAYER_CACHE_MANAGER_BASE_H
#include <vector>

#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
// namespace DrawableV2 {
// class RSRenderNodeDrawable;
// }

class RSLayerCacheManagerBase {
public:
    virtual ~RSLayerCacheManagerBase() = default;
    virtual void AddNodeToLayerNodes(std::shared_ptr<RSRenderNode> node) = 0;
    inline static std::vector<std::shared_ptr<RSRenderNode>> layerNodes_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RS_LAYER_CACHE_MANAGER_BASE_H
