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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_ROOT_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_ROOT_NODE_H

#include "ui/rs_canvas_node.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceNode;
class RSC_EXPORT RSRootNode : public RSCanvasNode {
public:
    using WeakPtr = std::weak_ptr<RSRootNode>;
    using SharedPtr = std::shared_ptr<RSRootNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::ROOT_NODE;
    RSUINodeType GetType() const override
    {
        return Type;
    }

    ~RSRootNode() override = default;

    static std::shared_ptr<RSNode> Create(bool isRenderServiceNode = false, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    /**
     * @brief Enables/disables control-level occlusion culling for the node's subtree
     *
     * When enabled, this node and its entire subtree will participate in
     * control-level occlusion culling. The specified key occlusion node acts as the primary
     * occluder within the subtree.
     *
     * @param enable
     *     - true: Enables occlusion culling for this node and its subtree
     *     - false: Disables occlusion culling for this node and its subtree
     * @param keyOcclusionNodeId
     *     The ID of the key occluding node within the subtree. This node will be treated as
     *     the primary occlusion source when determining visibility of other nodes in the subtree.
     *     Pass INVALID_NODE_ID if no specific occluder is designated.
     */
    void UpdateOcclusionCullingStatus(bool enable, NodeId keyOcclusionNodeId);

protected:
    void AttachRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode);
    void SetEnableRender(bool flag) const;
    void OnBoundsSizeChanged() const override;

    explicit RSRootNode(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSRootNode(const RSRootNode&) = delete;
    RSRootNode(const RSRootNode&&) = delete;
    RSRootNode& operator=(const RSRootNode&) = delete;
    RSRootNode& operator=(const RSRootNode&&) = delete;

private:
    void RegisterNodeMap() override;
    friend class RSUIDirector;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_ROOT_NODE_H