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
 * @file rs_display_node.h
 *
 * @brief Defines the properties and methods for RSDisplayNode class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_DISPLAY_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_DISPLAY_NODE_H

#include "ui/rs_node.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

/**
 * @class RSDisplayNode
 *
 * @brief Represents a display node in the rendering service.
 *
 * Each screen corresponds to a display node.
 */
class RSC_EXPORT RSDisplayNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSDisplayNode>;
    using SharedPtr = std::shared_ptr<RSDisplayNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::DISPLAY_NODE;

    /**
     * @brief Get the type of the RSUINode.
     * 
     * @return The type of the RSUINode.
     */
    RSUINodeType GetType() const override
    {
        return Type;
    }

    /**
     * @brief Destructor for RSDisplayNode.
     */
    ~RSDisplayNode() override;

    /**
     * @brief Removes all child nodes associated with this node.
     */
    void ClearChildren() override;

    /**
     * @brief Creates a shared pointer to an RSDisplayNode instance.
     *
     * @param displayNodeConfig Indicates the configuration settings for the display node.
     * @param rsUIContext An optional shared pointer to an RSUIContext instance. Defaults to nullptr if not provided.
     * @return A shared pointer to the newly created RSDisplayNode instance.
     */
    static SharedPtr Create(
        const RSDisplayNodeConfig& displayNodeConfig, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    
    /**
     * @brief Add the display node to tree.
     */
    void AddDisplayNodeToTree();

    /**
     * @brief Remove the display node from tree.
     */
    void RemoveDisplayNodeFromTree();

    /**
     * @brief Serializes the RSDisplayNode into a parcel.
     *
     * @param parcel Indicates the Parcel object where the RSDisplayNode data will be written.
     * @return true if the serialization is successful; false otherwise.
     */
    bool Marshalling(Parcel& parcel) const;

    /**
     * @brief Deserializes the RSDisplayNode from a Parcel.
     *
     * @param parcel Indicates the Parcel object containing the serialized RSDisplayNode data.
     * @return A shared pointer to the deserialized RSDisplayNode instance.
     */
    static SharedPtr Unmarshalling(Parcel& parcel);

    /**
     * @brief Sets the screen ID.
     * 
     * @param screenId Indicates the unique identifier of the screen.
     */
    void SetScreenId(uint64_t screenId);

    /**
     * @brief Sets the display offset for the screen.
     *
     * @param offsetX Indicates the X-axis offset to be applied to the screen.
     * @param offsetY Indicates the Y-axis offset to be applied to the screen.
     */
    void SetDisplayOffset(int32_t offsetX, int32_t offsetY);

    /**
     * @brief Sets whether it is a security screen.
     *
     * The secure screen processes secure layers, while the non-secure screen processes insecure layers
     *
     * @param isSecurityDisplay true if the screen is security; false otherwise.
     */
    void SetSecurityDisplay(bool isSecurityDisplay);

    /**
     * @brief Sets the rotation of screen.
     *
     * @param rotation Indicates the rotation value to be applied to the display node.
     */
    void SetScreenRotation(const uint32_t& rotation);

    /**
     * @brief Sets the mirror configuration of screen.
     *
     * @param displayNodeConfig Indicates the configuration settings for the screen.
     */
    void SetDisplayNodeMirrorConfig(const RSDisplayNodeConfig& displayNodeConfig);

    /**
     * @brief Gets whether it is a security screen.
     *
     * The secure screen processes secure layers, while the non-secure screen processes insecure layers
     *
     * @return true if the screen is security; false otherwise.
     */
    bool GetSecurityDisplay() const;

    /**
     * @brief Set whether to forcibly close HDR.
     *
     * @param isForceCloseHdr true if HDR should be forcibly closed; false otherwise.
     */
    void SetForceCloseHdr(bool isForceCloseHdr);

    /**
     * @brief Gets whether the screen mode is mirroring.
     *
     * @return true if the screen mode is mirroring; false otherwise.
     */
    bool IsMirrorDisplay() const;

    /**
     * @brief Sets the boot animation.
     *
     * @param isBootAnimation true if boot animation is enabled; false otherwise.
     */
    void SetBootAnimation(bool isBootAnimation);

    /**
     * @brief Gets whether has boot animation.
     *
     * @return true if has boot animation; false otherwise.
     */
    bool GetBootAnimation() const;

    /**
     * @brief Sets the pid of Sceneboard node.
     *
     * @param oldPids Indicates the old PIDs to be removed.
     * @param currentPid Indicates the current PID to be added.
     */
    void SetScbNodePid(const std::vector<int32_t>& oldPids, int32_t currentPid);

    /**
     * @brief Sets the virtual screen mute status.
     *
     * @param virtualScreenMuteStatus Indicates the mute status of the virtual screen.
     */
    void SetVirtualScreenMuteStatus(bool virtualScreenMuteStatus);

protected:
    explicit RSDisplayNode(const RSDisplayNodeConfig& config, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSDisplayNode(const RSDisplayNodeConfig& config, NodeId id, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSDisplayNode(const RSDisplayNode&) = delete;
    RSDisplayNode(const RSDisplayNode&&) = delete;
    RSDisplayNode& operator=(const RSDisplayNode&) = delete;
    RSDisplayNode& operator=(const RSDisplayNode&&) = delete;
    void OnBoundsSizeChanged() const override;

private:
    bool CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId);
    void RegisterNodeMap() override;
    uint64_t screenId_;
    int32_t offsetX_;
    int32_t offsetY_;
    bool isSecurityDisplay_ = false;
    bool isMirroredDisplay_ = false;
    bool isBootAnimation_ = false;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_DISPLAY_NODE_H
