/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_proxy_node.h
 *
 * @brief Defines the properties and methods for RSProxyNode class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_PROXY_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_PROXY_NODE_H

#include <parcel.h>
#include <string>

#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSProxyNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSProxyNode>;
    using SharedPtr = std::shared_ptr<RSProxyNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::PROXY_NODE;

    /**
     * @brief Get the type of the RSNode.
     * 
     * @return The type of the RSNode.
     */
    RSUINodeType GetType() const override
    {
        return Type;
    }

    /**
     * @brief Creates a shared pointer to a ProxyNode instance.
     *
     * @param targetNodeId The ID of the target node to be associated with the ProxyNode.
     * @param name The name of the ProxyNode. Defaults to "ProxyNode" if not specified.
     * @param rsUIContext A shared pointer to the RSUIContext. Defaults to nullptr if not specified.
     * @return A shared pointer to the newly created ProxyNode instance.
     */
    static SharedPtr Create(
        NodeId targetNodeId, std::string name = "ProxyNode", std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    
    /**
     * @brief Destructor for RSProxyNode.
     */
    ~RSProxyNode() override;

    /**
     * @brief Resets the context variable cache for the proxy node.
     */
    void ResetContextVariableCache() const;

    /**
     * @brief Get the name of the node.
     * 
     * @return A string representing the name of the node.
     */
    const std::string& GetName() const
    {
        return name_;
    }

    /**
     * @brief Adds a child node to the current node at the specified index.
     *
     * @param child The shared pointer to the child node to be added.
     * @param index The position at which the child node should be inserted. 
     *              If the index is -1 (default), the child is added to the end.
     */
    void AddChild(std::shared_ptr<RSBaseNode> child, int index) override;

    /**
     * @brief Removes a child node from the current node.
     *
     * @param child A shared pointer to the child node to be removed.
     */
    void RemoveChild(std::shared_ptr<RSBaseNode> child) override;

    /**
     * @brief Removes all child nodes associated with this node.
     */
    void ClearChildren() override;

    /**
     * @brief Sets the bounds of the node.
     *
     * @param bounds A Vector4f representing the new bounds (X, Y, width, height).
     */
    void SetBounds(const Vector4f& bounds) override {}

    /**
     * @brief Sets the bounds of the node.
     *
     * @param positionX The X coordinate of the new bounds.
     * @param positionY The Y coordinate of the new bounds.
     * @param width The width of the new bounds.
     * @param height The height of the new bounds.
     */
    void SetBounds(float positionX, float positionY, float width, float height) override {}

    /**
     * @brief Sets the width of the node's bounds.
     *
     * @param width The new width to set for the node's bounds.
     */
    void SetBoundsWidth(float width) override {}

    /**
     * @brief Sets the height of the node's bounds.
     *
     * @param height The new height to set for the node's bounds.
     */
    void SetBoundsHeight(float height) override {}

    /**
     * @brief Sets the frame of the node.
     *
     * @param frame A Vector4f representing the new frame, containing values for x, y, width, and height.
     */
    void SetFrame(const Vector4f& frame) override {}

    /**
     * @brief Sets the frame of the node.
     *
     * @param positionX The X coordinate of the frame.
     * @param positionY The Y coordinate of the frame.
     * @param width The width of the frame.
     * @param height The height of the frame.
     */
    void SetFrame(float positionX, float positionY, float width, float height) override {}

    /**
     * @brief Sets the X coordinate of the node's frame.
     *
     * @param positionX The X coordinate of the frame.
     */
    void SetFramePositionX(float positionX) override {}

    /**
     * @brief Sets the Y coordinate of the node's frame.
     *
     * @param positionY The Y coordinate of the frame.
     */
    void SetFramePositionY(float positionY) override {}

protected:
    explicit RSProxyNode(NodeId targetNodeId, std::string name, std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    /**
     * @brief Retrieves the NodeId associated with the hierarchy command for this proxy node.
     *
     * when add/remove/update child, construct command using proxy node id, not target node id
     *
     * @return The NodeId of the hierarchy command node.
     */
    NodeId GetHierarchyCommandNodeId() const override
    {
        return proxyNodeId_;
    }

private:
    /**
     * @brief Creates a proxy render node for rendering operations.
     */
    void CreateProxyRenderNode();
    
    /**
     * @brief Registers the node in the node map.
     */
    void RegisterNodeMap() override;
    NodeId proxyNodeId_;
    std::string name_;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_PROXY_NODE_H
