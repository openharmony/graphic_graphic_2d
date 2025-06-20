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
 * @file rs_surface_node.h
 *
 * @brief Defines the properties and methods for RSSurfaceNode class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H

#include <parcel.h>
#include <refbase.h>
#include <string>

#ifdef ROSEN_OHOS
#include "iconsumer_surface.h"
#include "surface.h"
#include "surface_delegate.h"
#include "surface_type.h"
#endif

#ifndef ROSEN_CROSS_PLATFORM
#include "surface_delegate.h"
#endif

#include "platform/drawing/rs_surface.h"
#include "platform/common/rs_surface_ext.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"


namespace OHOS {
namespace Rosen {
/**
 * @struct RSSurfaceNodeConfig
 * @brief Configuration structure for creating or managing a surface node.
 */
struct RSSurfaceNodeConfig {
    /* Name of the surface node. Defaults to "SurfaceNode" */
    std::string SurfaceNodeName = "SurfaceNode";
    /* Pointer to any additional user data. Defaults to nullptr. */
    void* additionalData = nullptr;
    /* Indicates if the node is used for texture export. Defaults to false. */
    bool isTextureExportNode = false;
    /* Unique identifier for the surface. Defaults to 0. */
    SurfaceId surfaceId = 0;
    /* Specifies if the node operates in synchronous mode. Defaults to true. */
    bool isSync = true;
    /* Type of the surface window. Defaults to SurfaceWindowType::DEFAULT_WINDOW */
    enum SurfaceWindowType surfaceWindowType = SurfaceWindowType::DEFAULT_WINDOW;
    /* Shared pointer to the UI context associated with this node. Defaults to nullptr. */
    std::shared_ptr<RSUIContext> rsUIContext = nullptr;
};

/**
 * @class RSSurfaceNode
 *
 * @brief Represents a surface node in the rendering service.
 */
class RSC_EXPORT RSSurfaceNode : public RSNode {
public:
    static constexpr float POINTER_WINDOW_POSITION_Z = 9999;

    using WeakPtr = std::weak_ptr<RSSurfaceNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::SURFACE_NODE;
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
     * @brief Destructor for RSSurfaceNode.
     */
    ~RSSurfaceNode() override;

    /**
     * @brief Creates a new instance of RSSurfaceNode with the specified configuration.
     * 
     * @param surfaceNodeConfig The configuration settings for the surface node.
     * @param isWindow Indicates whether the surface node is a window. Defaults to true.
     * @param rsUIContext An optional shared pointer to an RSUIContext object. Defaults to nullptr.
     * @return SharedPtr A shared pointer to the newly created RSSurfaceNode instance.
     */
    static SharedPtr Create(const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow = true,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    /**
     * @brief Creates a new RSSurfaceNode instance with the specified configuration and parameters.
     *
     * This interface is only available for WMS
     *
     * @param surfaceNodeConfig The configuration settings for the surface node.
     * @param type The type of the surface node, specified as an RSSurfaceNodeType.
     * @param isWindow Indicates whether the surface node represents a window. Defaults to true.
     * @param unobscured Indicates whether the surface node is unobscured. Defaults to false.
     * @param rsUIContext An optional shared pointer to an RSUIContext instance. Defaults to nullptr.
     * @return A shared pointer to the newly created RSSurfaceNode instance.
     */
    static SharedPtr Create(const RSSurfaceNodeConfig& surfaceNodeConfig, RSSurfaceNodeType type, bool isWindow = true,
        bool unobscured = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    // This API is only for abilityView create RSRenderSurfaceNode in RenderThread.
    // Do not call this API unless you are sure what you do.
    // After calling it, this surfaceNode is disallowed to add/remove child.
    void CreateNodeInRenderThread();

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

    void SetSecurityLayer(bool isSecurityLayer);
    bool GetSecurityLayer() const;
    void SetLeashPersistentId(LeashPersistentId leashPersistentId);
    LeashPersistentId GetLeashPersistentId() const;
    void SetSkipLayer(bool isSkipLayer);
    bool GetSkipLayer() const;
    void SetSnapshotSkipLayer(bool isSnapshotSkipLayer);
    bool GetSnapshotSkipLayer() const;
    void SetFingerprint(bool hasFingerprint);
    bool GetFingerprint() const;
    /**
     * @brief Sets the background alpha value for the ability.
     *
     * @param alpha The alpha value to set, ranging from 0 (fully transparent) to 255 (fully opaque).
     */
    void SetAbilityBGAlpha(uint8_t alpha);
    void SetIsNotifyUIBufferAvailable(bool available);
    void MarkUIHidden(bool isHidden);

    using BufferAvailableCallback = std::function<void()>;
    bool SetBufferAvailableCallback(BufferAvailableCallback callback);
    bool IsBufferAvailable() const;
    void SetBoundsChangedCallback(BoundsChangedCallback callback) override;
    void SetAnimationFinished();

    /**
     * @brief Serializes the RSSurfaceNode into a parcel.
     *
     * @param parcel The Parcel object where the RSSurfaceNode data will be written.
     * @return true if the serialization is successful; false otherwise.
     */
    bool Marshalling(Parcel& parcel) const;

    /**
     * @brief Deserializes the RSDisplayNode from a Parcel.
     *
     * @param parcel The Parcel object containing the serialized RSSurfaceNode data.
     * @return A shared pointer to the deserialized RSSurfaceNode instance.
     */
    static SharedPtr Unmarshalling(Parcel& parcel);

    /**
     * @brief Create RSProxyNode by unmarshalling RSSurfaceNode.
     *
     * return existing node if it exists in RSNodeMap
     *
     * @param parcel The Parcel object containing the marshalled data.
     * @return A shared pointer to the unmarshalled RSNode object.
     */
    static RSNode::SharedPtr UnmarshallingAsProxyNode(Parcel& parcel);

    FollowType GetFollowType() const override;

    void AttachToDisplay(uint64_t screenId);
    void DetachToDisplay(uint64_t screenId);
    void SetHardwareEnabled(bool isEnabled, SelfDrawingNodeType selfDrawingType = SelfDrawingNodeType::DEFAULT,
        bool dynamicHardwareEnable = true);

    /**
     * @brief Enable Camera Rotation Unchanged
     *
     * @param flag If flag is set to true, the camera fix rotation is enabled.
     * @return void
     */
    void SetForceHardwareAndFixRotation(bool flag);
    void SetBootAnimation(bool isBootAnimation);
    bool GetBootAnimation() const;
    void SetTextureExport(bool isTextureExportNode) override;
    void SetCompositeLayer(TopLayerZOrder zOrder) {};
    void SetGlobalPositionEnabled(bool isEnabled);
    bool GetGlobalPositionEnabled() const;
    /**
     * @brief Set whether to enable the new version of frame gravity.
     *
     * @param bool enable the new version of frame gravity or not.
     */
    void SetFrameGravityNewVersionEnabled(bool isEnabled);
    /**
     * @brief Get the enable status of new version of frame gravity.
     *
     * @return true if the new version of frame gravity enabled; false otherwise.
     */
    bool GetFrameGravityNewVersionEnabled() const;

#ifndef ROSEN_CROSS_PLATFORM
    sptr<OHOS::Surface> GetSurface() const;
#endif
    void SetColorSpace(GraphicColorGamut colorSpace);
    GraphicColorGamut GetColorSpace()
    {
        return colorSpace_;
    }

    /**
     * @brief Get the name of the node.
     * 
     * @return A string representing the name of the node.
     */
    inline std::string GetName() const
    {
        return name_;
    }

    void ResetContextAlpha() const;

    void SetContainerWindow(bool hasContainerWindow, RRect rrect);
    /**
     * @brief Sets the window ID for the surface node.
     *
     * @param windowId The unique identifier of the window to be set.
     */
    void SetWindowId(uint32_t windowId);

    /**
     * @brief Controls surface content freezing for window snapshot or resource release
     *
     * Only works in Unified Render mode (UniRender)
     *
     * @param isFreeze Freeze control flag:
     *                - true: Freeze current frame into static texture
     *                - false: Resume normal buffer updates
     */
    void SetFreeze(bool isFreeze) override;
    
    // codes for arkui-x
#ifdef USE_SURFACE_TEXTURE
    void SetSurfaceTexture(const RSSurfaceExtConfig& config);
    void MarkUiFrameAvailable(bool available);
    void SetSurfaceTextureAttachCallBack(const RSSurfaceTextureAttachCallBack& attachCallback);
    void SetSurfaceTextureUpdateCallBack(const RSSurfaceTextureUpdateCallBack& updateCallback);
    void SetSurfaceTextureInitTypeCallBack(const RSSurfaceTextureInitTypeCallBack& initTypeCallback);
#endif
    void SetForeground(bool isForeground);
    // [Attention] The function only used for unlocking screen for PC currently
    void SetClonedNodeInfo(NodeId nodeId, bool needOffscreen = true);
    // Force enable UIFirst when set TRUE
    void SetForceUIFirst(bool forceUIFirst);
    void SetAncoFlags(uint32_t flags);
    void SetHDRPresent(bool hdrPresent, NodeId id);
    void SetSkipDraw(bool skip);
    bool GetSkipDraw() const;
    void SetAbilityState(RSSurfaceNodeAbilityState abilityState);
    RSSurfaceNodeAbilityState GetAbilityState() const;

    void SetWatermarkEnabled(const std::string& name, bool isEnabled);

    RSInterfaceErrorCode SetHidePrivacyContent(bool needHidePrivacyContent);
    // Specifying hardware enable is only a 'hint' to RS that
    // the self-drawing node use hardware composer in some condition,
    // such as transparent background.
    void SetHardwareEnableHint(bool enable);
    
    /**
     * @brief Sets the API compatible version for the surface node.
     * @param version The API version to set for compatibility.
     */
    void SetApiCompatibleVersion(uint32_t version);

    void SetSourceVirtualDisplayId(ScreenId screenId);
    void AttachToWindowContainer(ScreenId screenId);
    void DetachFromWindowContainer(ScreenId screenId);
    void SetRegionToBeMagnified(const Vector4f& regionToBeMagnified);
protected:
    bool NeedForcedSendToRemote() const override;
    RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode, NodeId id,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSSurfaceNode(const RSSurfaceNode&) = delete;
    RSSurfaceNode(const RSSurfaceNode&&) = delete;
    RSSurfaceNode& operator=(const RSSurfaceNode&) = delete;
    RSSurfaceNode& operator=(const RSSurfaceNode&&) = delete;

private:
#ifdef USE_SURFACE_TEXTURE
    void CreateSurfaceExt(const RSSurfaceExtConfig& config);
#endif
    bool CreateNode(const RSSurfaceRenderNodeConfig& config);
    bool CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, SurfaceId surfaceId = 0,
        bool unobscured = false);
    /**
     * @brief Called when the bounds size of the surface node changes.
     */
    void OnBoundsSizeChanged() const override;
    // this function is only used in texture export
    void SetSurfaceIdToRenderNode();
    void CreateRenderNodeForTextureExportSwitch() override;
    void SetIsTextureExportNode(bool isTextureExportNode);
    std::pair<std::string, std::string> SplitSurfaceNodeName(std::string surfaceNodeName);
    void RegisterNodeMap() override;
    std::shared_ptr<RSSurface> surface_;
    std::string name_;
    std::string bundleName_;
    mutable std::mutex mutex_;
    BufferAvailableCallback callback_;
    bool bufferAvailable_ = false;
    BoundsChangedCallback boundsChangedCallback_;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    bool isSecurityLayer_ = false;
    bool isSkipLayer_ = false;
    bool isSnapshotSkipLayer_ = false;
    bool hasFingerprint_ = false;
    bool isChildOperationDisallowed_ { false };
    bool isBootAnimation_ = false;
    bool isSkipDraw_ = false;
    RSSurfaceNodeAbilityState abilityState_ = RSSurfaceNodeAbilityState::FOREGROUND;
    bool isGlobalPositionEnabled_ = false;
    bool isFrameGravityNewVersionEnabled_ = false;
    LeashPersistentId leashPersistentId_ = INVALID_LEASH_PERSISTENTID;

    uint32_t windowId_ = 0;
#ifndef ROSEN_CROSS_PLATFORM
    sptr<SurfaceDelegate> surfaceDelegate_;
    sptr<SurfaceDelegate::ISurfaceCallback> surfaceCallback_;
#endif

    friend class RSUIDirector;
    friend class RSAnimation;
    friend class RSPathAnimation;
    friend class RSPropertyAnimation;
    friend class RSSurfaceExtractor;
    friend class RSSurfaceCallback;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H
