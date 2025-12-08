/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_FEATURE_RS_KEYFRAME_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_FEATURE_RS_KEYFRAME_NODE_H

#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSWindowKeyFrameNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSWindowKeyFrameNode>;
    using SharedPtr = std::shared_ptr<RSWindowKeyFrameNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::WINDOW_KEYFRAME_NODE;

    /**
     * @brief Destructor for RSWindowKeyFrameNode.
     */
    ~RSWindowKeyFrameNode() override;

    /**
     * @brief Get the type of the RSWindowKeyFrameNode.
     *
     * @return The type of the RSWindowKeyFrameNode.
     */
    RSUINodeType GetType() const override
    {
        return Type;
    }

    /**
     * @brief Creates a shared pointer to an RSWindowKeyFrameNode.
     *
     * @param isRenderServiceNode Indicates whether the node is a render service node. Defaults to false.
     * @param isTextureExportNode Indicates whether the node is a texture export node. Defaults to false.
     * @param rsUIContext A shared pointer to an RSUIContext object. Defaults to nullptr.
     * @return A shared pointer to the created RSWindowKeyFrameNode.
     */
    static SharedPtr Create(bool isRenderServiceNode = false,
        bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    /**
     * @brief Freezes current frame data when enabled, preventing content refresh until unfrozen.
     *
     * @param isFreeze Freeze state flag
     *                - true: Freeze current frame content
     *                - false: Resume dynamic updates
     */
    void SetFreeze(bool isFreeze) override;

    /**
     * @brief Set linked node id in PC window resize scenario.
     * @param nodeId source RSNode id.
     */
    void SetLinkedNodeId(NodeId nodeId);

    /**
     * @brief Get linked node id.
     * @return linked node id.
     */
    NodeId GetLinkedNodeId() const;

    /**
     * @brief Serializes the RSWindowKeyFrameNode into a parcel.
     *
     * @param parcel The Parcel object where the RSWindowKeyFrameNode data will be written.
     * @return true if the serialization is successful; false otherwise.
     */
    bool WriteToParcel(Parcel& parcel) const;

    /**
     * @brief Deserializes the RSWindowKeyFrameNode from a Parcel.
     *
     * @param parcel The Parcel object containing the serialized RSWindowKeyFrameNode data.
     * @return A shared pointer to the deserialized RSWindowKeyFrameNode instance.
     */
    static SharedPtr ReadFromParcel(Parcel& parcel);

protected:
    RSWindowKeyFrameNode(bool isRenderServiceNode,
        bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSWindowKeyFrameNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSWindowKeyFrameNode(const RSWindowKeyFrameNode&) = delete;
    RSWindowKeyFrameNode(const RSWindowKeyFrameNode&&) = delete;
    RSWindowKeyFrameNode& operator=(const RSWindowKeyFrameNode&) = delete;
    RSWindowKeyFrameNode& operator=(const RSWindowKeyFrameNode&&) = delete;

private:
    void RegisterNodeMap() override;

    NodeId linkedNodeId_ = INVALID_NODEID;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_FEATURE_RS_KEYFRAME_NODE_H
