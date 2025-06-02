/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 * @file rs_effect_node.h
 *
 * @brief Defines the properties and methods for RSDEffectNode class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_EFFECT_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_EFFECT_NODE_H

#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

/**
 * @class RSEffectNode
 *
 * @brief Represents a effect node in the rendering service.
 */
class RSC_EXPORT RSEffectNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSEffectNode>;
    using SharedPtr = std::shared_ptr<RSEffectNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::EFFECT_NODE;

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
     * @brief Destructor for RSEffectNode.
     */
    ~RSEffectNode() override = default;

    /**
     * @brief Creates a shared pointer to an RSEffectNode.
     * 
     * @param isRenderServiceNode Indicates whether the node is a render service node. Defaults to false.
     * @param isTextureExportNode Indicates whether the node is a texture export node. Defaults to false.
     * @param rsUIContext A shared pointer to an RSUIContext object. Defaults to nullptr.
     * @return A shared pointer to the created RS effect node.
     */
    static SharedPtr Create(bool isRenderServiceNode = false, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    /**
     * @brief Sets the freeze state of the node.
     *
     * The property is valid only for CanvasNode and SurfaceNode in uniRender.
     *
     * @param isFreeze If true, the node will be frozen; if false, the node will be unfrozen.
     */
    void SetFreeze(bool isFreeze) override;

protected:
    RSEffectNode(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSEffectNode(const RSEffectNode&) = delete;
    RSEffectNode(const RSEffectNode&&) = delete;
    RSEffectNode& operator=(const RSEffectNode&) = delete;
    RSEffectNode& operator=(const RSEffectNode&&) = delete;
private:
    /**
     * @brief Registers the node in the node map.
     */
    void RegisterNodeMap() override;
    bool preFreeze_ = false;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_EFFECT_NODE_H
