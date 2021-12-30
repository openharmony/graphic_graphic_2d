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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H

#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class PlatformCanvas;
class RSSurface;
class RSRootRenderNode : public RSRenderNode {
public:
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::ROOT_NODE;
    explicit RSRootRenderNode(NodeId id);
    ~RSRootRenderNode() override;

    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    // will be replaced for surfaceNode in window
    void AttachSurface(uintptr_t surfaceProducer, int width, int height);
    void AttachRSSurface(std::shared_ptr<RSSurface> rsSurface, int width, int height);
    std::shared_ptr<PlatformCanvas> GetPlatformCanvas();

    RSRenderNodeType GetType() override
    {
        return RSRenderNodeType::ROOT_NODE;
    }

    std::shared_ptr<RSSurface> GetSurface();
    int32_t GetWidth() const;
    int32_t GetHeight() const;
private:
    int32_t surfaceWidth_ = 0;
    int32_t surfaceHeight_ = 0;
    std::shared_ptr<PlatformCanvas> platformCanvas_ = nullptr;
    std::shared_ptr<RSSurface> rsSurface_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H