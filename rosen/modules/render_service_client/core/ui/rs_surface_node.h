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

#ifdef NEW_RENDER_CONTEXT
#include "rs_render_surface.h"
#else
#include "platform/drawing/rs_surface.h"
#include "platform/common/rs_surface_ext.h"
#endif
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

    static SharedPtr Create(const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow = true);

    // This interface is only available for WMS
    static SharedPtr Create(const RSSurfaceNodeConfig& surfaceNodeConfig, RSSurfaceNodeType type, bool isWindow = true);

    // This API is only for abilityView create RSRenderSurfaceNode in RenderThread.
    // Do not call this API unless you are sure what you do.
    // After calling it, this surfaceNode is disallowed to add/remove child.
    void CreateNodeInRenderThread();

    void AddChild(std::shared_ptr<RSBaseNode> child, int index) override;
    void RemoveChild(std::shared_ptr<RSBaseNode> child) override;
    void ClearChildren() override;

    void SetSecurityLayer(bool isSecurityLayer);
    bool GetSecurityLayer() const;
    void SetSkipLayer(bool isSkipLayer);
    bool GetSkipLayer() const;
    void SetFingerprint(bool hasFingerprint);
    bool GetFingerprint() const;
    void SetAbilityBGAlpha(uint8_t alpha);
    void SetIsNotifyUIBufferAvailable(bool available);
    void MarkUIHidden(bool isHidden);

    using BufferAvailableCallback = std::function<void()>;
    bool SetBufferAvailableCallback(BufferAvailableCallback callback);
    bool IsBufferAvailable() const;
    using BoundsChangedCallback = std::function<void(const Rosen::Vector4f&)>;
    void SetBoundsChangedCallback(BoundsChangedCallback callback) override;
    void SetAnimationFinished();

    bool Marshalling(Parcel& parcel) const;
    static SharedPtr Unmarshalling(Parcel& parcel);
    // Create RSProxyNode by unmarshalling RSSurfaceNode, return existing node if it exists in RSNodeMap.
    static RSNode::SharedPtr UnmarshallingAsProxyNode(Parcel& parcel);

    FollowType GetFollowType() const override;

    void AttachToDisplay(uint64_t screenId);
    void DetachToDisplay(uint64_t screenId);
    void SetHardwareEnabled(bool isEnabled, SelfDrawingNodeType selfDrawingType = SelfDrawingNodeType::DEFAULT);
    void SetForceHardwareAndFixRotation(bool flag);
    void SetBootAnimation(bool isBootAnimation);
    bool GetBootAnimation() const;
    void SetTextureExport(bool isTextureExportNode) override;

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

    const std::string GetBundleName() const
    {
        return bundleName_;
    }

    void ResetContextAlpha() const;

    void SetContainerWindow(bool hasContainerWindow, float density);
    void SetWindowId(uint32_t windowId);

    void SetFreeze(bool isFreeze) override;
#ifdef USE_SURFACE_TEXTURE
    void SetSurfaceTexture(const RSSurfaceExtConfig& config);
    void MarkUiFrameAvailable(bool available);
    void SetSurfaceTextureAttachCallBack(const RSSurfaceTextureAttachCallBack& attachCallback);
    void SetSurfaceTextureUpdateCallBack(const RSSurfaceTextureUpdateCallBack& updateCallback);
#endif
    void SetForeground(bool isForeground);
    // Force enable UIFirst when set TRUE
    void SetForceUIFirst(bool forceUIFirst);
    void SetAncoForceDoDirect(bool ancoForceDoDirect);

protected:
    bool NeedForcedSendToRemote() const override;
    RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode);
    RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode, NodeId id);
    RSSurfaceNode(const RSSurfaceNode&) = delete;
    RSSurfaceNode(const RSSurfaceNode&&) = delete;
    RSSurfaceNode& operator=(const RSSurfaceNode&) = delete;
    RSSurfaceNode& operator=(const RSSurfaceNode&&) = delete;

private:
#ifdef USE_SURFACE_TEXTURE
    void CreateSurfaceExt(const RSSurfaceExtConfig& config);
#endif
    bool CreateNode(const RSSurfaceRenderNodeConfig& config);
    bool CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, SurfaceId surfaceId = 0);
    void OnBoundsSizeChanged() const override;
    // this function is only used in texture export
    void SetSurfaceIdToRenderNode();
    void CreateTextureExportRenderNodeInRT() override;
    void SetIsTextureExportNode(bool isTextureExportNode);
    std::pair<std::string, std::string> SplitSurfaceNodeName(std::string surfaceNodeName);
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurface> surface_;
#else
    std::shared_ptr<RSSurface> surface_;
#endif
    std::string name_;
    std::string bundleName_;
    mutable std::mutex mutex_;
    BufferAvailableCallback callback_;
    bool bufferAvailable_ = false;
    BoundsChangedCallback boundsChangedCallback_;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    bool isSecurityLayer_ = false;
    bool isSkipLayer_ = false;
    bool hasFingerprint_ = false;
    bool isChildOperationDisallowed_ { false };
    bool isBootAnimation_ = false;

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
