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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H

#include "common/rs_macros.h"
#include "memory/rs_memory_track.h"
#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {
class RSSurface;
class RSDirtyRegionManager;
class RSB_EXPORT RSRootRenderNode : public RSCanvasRenderNode {
public:
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::ROOT_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    ~RSRootRenderNode() override;

    virtual void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void AttachRSSurfaceNode(NodeId SurfaceNodeId);
    void AttachToken(uint64_t token);

    std::shared_ptr<RSDirtyRegionManager> GetDirtyManager() const;
    std::shared_ptr<RSSurface> GetSurface();
    NodeId GetRSSurfaceNodeId();
    float GetSuggestedBufferWidth() const;
    float GetSuggestedBufferHeight() const;
    void UpdateSuggestedBufferSize(float width, float height);
    void SetEnableRender(bool enableRender)
    {
        if (enableRender_ != enableRender) {
            enableRender_ = enableRender;
            SetContentDirty();
        }
    }

    bool GetEnableRender() const
    {
        return enableRender_;
    }

    uint64_t GetToken() const
    {
        return token_;
    }

    // [Attention] Only used in PC window resize scene now
    void EnableWindowKeyFrame(bool enable);
    bool IsWindowKeyFrameEnabled();

private:
    explicit RSRootRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {},
        bool isTextureExportNode = false);
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    std::shared_ptr<RSSurface> rsSurface_ = nullptr;
    NodeId surfaceNodeId_ = 0;
    bool enableRender_ = true;
    float suggestedBufferWidth_ = 0.f;
    float suggestedBufferHeight_ = 0.f;
    uint64_t token_ = 0;

    std::vector<NodeId> childSurfaceNodeIds_;

    friend class RootNodeCommandHelper;
    friend class RSRenderThreadVisitor;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ROOT_RENDER_NODE_H
