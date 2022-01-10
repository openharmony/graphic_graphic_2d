/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_TEXTURE_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_TEXTURE_NODE_H

#include "ui/rs_canvas_node.h"

namespace flutter::OHOS {
class TextureRegistry;
}

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSTextureNode : public RSCanvasNode {
public:
    using WeakPtr = std::weak_ptr<RSTextureNode>;
    using SharedPtr = std::shared_ptr<RSTextureNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::TEXTURE_NODE;

    virtual ~RSTextureNode() {}

    static std::shared_ptr<RSNode> Create();

    static void SetTextureRegistry(std::shared_ptr<flutter::OHOS::TextureRegistry> registry);
    void UpdateTexture(int64_t textureId, bool freeze, RectF drawRect);

    RSUINodeType GetType() const override
    {
        return RSUINodeType::TEXTURE_NODE;
    }

protected:
    RSTextureNode(bool isRenderServiceNode);
    RSTextureNode(const RSTextureNode&) = delete;
    RSTextureNode(const RSTextureNode&&) = delete;
    RSTextureNode& operator=(const RSTextureNode&) = delete;
    RSTextureNode& operator=(const RSTextureNode&&) = delete;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_TEXTURE_NODE_H