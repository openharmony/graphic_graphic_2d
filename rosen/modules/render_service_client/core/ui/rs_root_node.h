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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_root_node.h
 *
 * @brief Defines the properties and methods for RSRootNode class.
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
     * @brief Destructor for RSRootNode.
     */
    ~RSRootNode() override = default;

    /**
     * @brief Creates a shared pointer to an RSRootNode instance.
     *
     * @param isRenderServiceNode Indicates whether the node is a render service node.
     * @param isTextureExportNode Indicates whether the node is a texture export node.
     * @param rsUIContext A shared pointer to the RSUIContext. Defaults to nullptr if not specified.
     * @return A shared pointer to the created RSRootNode instance.
     */
    static std::shared_ptr<RSNode> Create(bool isRenderServiceNode = false, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);

protected:
    void AttachRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode);
    void SetEnableRender(bool flag) const;
    /**
     * @brief Called when the bounds size of the node has changed.
     */
    void OnBoundsSizeChanged() const override;

    explicit RSRootNode(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSRootNode(const RSRootNode&) = delete;
    RSRootNode(const RSRootNode&&) = delete;
    RSRootNode& operator=(const RSRootNode&) = delete;
    RSRootNode& operator=(const RSRootNode&&) = delete;

private:
/**
     * @brief Registers the node in the node map.
     */
    void RegisterNodeMap() override;
    friend class RSUIDirector;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_ROOT_NODE_H