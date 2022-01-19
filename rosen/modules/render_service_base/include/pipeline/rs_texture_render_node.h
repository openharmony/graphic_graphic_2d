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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_TEXTURE_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_TEXTURE_RENDER_NODE_H

#include "pipeline/rs_canvas_render_node.h"

namespace flutter::OHOS {
class TextureRegistry;
}

namespace OHOS {
namespace Rosen {
class RSTextureRenderNode : public RSCanvasRenderNode {
public:
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::TEXTURE_NODE;

    static void SetTextureRegistry(std::shared_ptr<flutter::OHOS::TextureRegistry> registry);
    void UpdateTexture(int64_t textureId, bool freeze, RectF drawRect);

    explicit RSTextureRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    ~RSTextureRenderNode() override;

    void ProcessRenderContents(RSPaintFilterCanvas& canvas) override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::TEXTURE_NODE;
    }

private:
    bool IsDirty() const override;
    int64_t textureId_ = -1;
    bool freeze_ = false;
    RectF drawRect_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_TEXTURE_RENDER_NODE_H