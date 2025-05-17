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
struct RSSurfaceNodeConfig {
    std::string SurfaceNodeName = "SurfaceNode";
    void* additionalData = nullptr;
    bool isTextureExportNode = false;
    SurfaceId surfaceId = 0;
    bool isSync = true;
    enum SurfaceWindowType surfaceWindowType = SurfaceWindowType::DEFAULT_WINDOW;
    std::shared_ptr<RSUIContext> rsUIContext = nullptr;
};

class RSC_EXPORT RSSurfaceNode : public RSNode {
public:
    static constexpr float POINTER_WINDOW_POSITION_Z = 9999;

    using WeakPtr = std::weak_ptr<RSSurfaceNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::SURFACE_NODE;
    RSUINodeType GetType() const override
    {
        return Type;
    }

    ~RSSurfaceNode() override;

    static SharedPtr Create(const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow = true,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    // This interface is only available for WMS
    static SharedPtr Create(const RSSurfaceNodeConfig& surfaceNodeConfig, RSSurfaceNodeType type, bool isWindow = true,
        bool unobscured = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    // This API is only for abilityView create RSRenderSurfaceNode in RenderThread.
    // Do not call this API unless you are sure what you do.
    // After calling it, this surfaceNode is disallowed to add/remove child.
    void CreateNodeInRenderThread();

    void AddChild(std::shared_ptr<RSBaseNode> child, int index) override;
    void RemoveChild(std::shared_ptr<RSBaseNode> child) override;
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
    void SetAbilityBGAlpha(uint8_t alpha);
    void SetIsNotifyUIBufferAvailable(bool available);
    void MarkUIHidden(bool isHidden);

    using BufferAvailableCallback = std::function<void()>;
    bool SetBufferAvailableCallback(BufferAvailableCallback callback);
    bool IsBufferAvailable() const;
    void SetBoundsChangedCallback(BoundsChangedCallback callback) override;
    void SetAnimationFinished();

    bool Marshalling(Parcel& parcel) const;
    static SharedPtr Unmarshalling(Parcel& parcel);
    // Create RSProxyNode by unmarshalling RSSurfaceNode, return existing node if it exists in RSNodeMap.
    static RSNode::SharedPtr UnmarshallingAsProxyNode(Parcel& parcel);

    FollowType GetFollowType() const override;

    void AttachToDisplay(uint64_t screenId);
    void DetachToDisplay(uint64_t screenId);
    void SetHardwareEnabled(bool isEnabled, SelfDrawingNodeType selfDrawingType = SelfDrawingNodeType::DEFAULT,
        bool dynamicHardwareEnable = true);
    void SetForceHardwareAndFixRotation(bool flag);
    void SetBootAnimation(bool isBootAnimation);
    bool GetBootAnimation() const;
    void SetTextureExport(bool isTextureExportNode) override;
    void SetGlobalPositionEnabled(bool isEnabled);
    bool GetGlobalPositionEnabled() const;
    void SetFrameGravityNewVersionEnabled(bool isEnabled);
    bool GetFrameGravityNewVersionEnabled() const;

#ifndef ROSEN_CROSS_PLATFORM
    sptr<OHOS::Surface> GetSurface() const;
#endif
    void SetColorSpace(GraphicColorGamut colorSpace);
    GraphicColorGamut GetColorSpace()
    {
        return colorSpace_;
    }

    inline std::string GetName() const
    {
        return name_;
    }

    void ResetContextAlpha() const;

    void SetContainerWindow(bool hasContainerWindow, RRect rrect);
    void SetWindowId(uint32_t windowId);

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
    void SetApiCompatibleVersion(uint32_t version);

    void SetSourceVirtualDisplayId(ScreenId screenId);
    void AttachToWindowContainer(ScreenId screenId);
    void DetachFromWindowContainer(ScreenId screenId);
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

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H
