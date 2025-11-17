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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_union_node.h
 *
 * @brief Defines the properties and methods for RSUnionNode class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_UNION_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_UNION_NODE_H

#include "ui/rs_canvas_node.h"

namespace OHOS {
namespace Rosen {

/**
 * @class RSUnionNode
 *
 * @brief Represents a union node in the rendering service.
 */
class RSC_EXPORT RSUnionNode : public RSCanvasNode {
public:
    using WeakPtr = std::weak_ptr<RSUnionNode>;
    using SharedPtr = std::shared_ptr<RSUnionNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::UNION_NODE;

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
     * @brief Destructor for RSUnionNode.
     */
    ~RSUnionNode() override = default;

    /**
     * @brief Creates a shared pointer to an RSUnionNode.
     * 
     * @param isRenderServiceNode Indicates whether the node is a render service node. Defaults to false.
     * @param isTextureExportNode Indicates whether the node is a texture export node. Defaults to false.
     * @param rsUIContext A shared pointer to an RSUIContext object. Defaults to nullptr.
     * @return A shared pointer to the created RSUnionNode.
     */
    static SharedPtr Create(bool isRenderServiceNode = false, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);

     /**
     * @brief Sets the union spaing of the node.
     *
     * @param spacing SDF Smooth Union spacing.
     */
    void SetUnionSpacing(float spacing);

protected:
    RSUnionNode(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSUnionNode(const RSUnionNode&) = delete;
    RSUnionNode(const RSUnionNode&&) = delete;
    RSUnionNode& operator=(const RSUnionNode&) = delete;
    RSUnionNode& operator=(const RSUnionNode&&) = delete;
private:
    /**
     * @brief Registers the node in the node map.
     */
    void RegisterNodeMap() override;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UNION_NODE_H
