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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMPOSITE_LAYER_RS_COMPOSITE_LAYER_UTILS_H
#define RENDER_SERVICE_CLIENT_CORE_COMPOSITE_LAYER_RS_COMPOSITE_LAYER_UTILS_H
 
#include "ui/rs_node.h"
#include "ui/rs_texture_export.h"

namespace OHOS {
namespace Rosen {

class RSSurfaceNode;
class RSCompositeLayerUtils {
public:
    RSCompositeLayerUtils(std::shared_ptr<RSNode> rootNode, uint32_t zOrder);
    ~RSCompositeLayerUtils();
    bool CreateCompositeLayer();
    void UpdateVirtualNodeBounds(const Vector4f& bounds);
    uint32_t GetTopLayerZOrder();
    std::shared_ptr<RSSurfaceNode> GetCompositeNode();
    bool DealWithSelfDrawCompositeNode(std::shared_ptr<RSNode> node, uint32_t zOrder);
private:
    std::shared_ptr<RSNode> rootNode_;
    std::shared_ptr<RSTextureExport> textureExport_;
    std::shared_ptr<RSSurfaceNode> compositeNode_;
    uint32_t topLayerZOrder_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_COMPOSITE_LAYER_RS_COMPOSITE_LAYER_UTILS_H