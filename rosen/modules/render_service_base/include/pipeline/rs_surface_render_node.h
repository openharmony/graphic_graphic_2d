/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H

#include <functional>
#include <limits>
#include <memory>
#include <tuple>

#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/gpu/GrContext.h"
#include "refbase.h"

#include "common/rs_macros.h"
#include "common/rs_occlusion_region.h"
#include "common/rs_vector4.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "property/rs_properties_painter.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_occlusion_data.h"

namespace OHOS {
namespace Rosen {
class RSCommand;
class RSDirtyRegionManager;
class RSB_EXPORT RSSurfaceRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::SURFACE_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    explicit RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    explicit RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context = {});
    ~RSSurfaceRenderNode() override;

    void PrepareRenderBeforeChildren(RSPaintFilterCanvas& canvas);
    void PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas);
    void ResetParent() override;

    bool IsAppWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::APP_WINDOW_NODE;
    }

    RSSurfaceNodeType GetSurfaceNodeType() const
    {
        return nodeType_;
    }

    void SetSurfaceNodeType(RSSurfaceNodeType nodeType)
    {
        if (nodeType_ != RSSurfaceNodeType::ABILITY_COMPONENT_NODE) {
            nodeType_ = nodeType;
        }
    }

    std::string GetName() const
    {
        return name_;
    }

    void SetOffSetX(int32_t offset)
    {
        offsetX_ = offset;
    }

    int32_t GetOffSetX()
    {
        return offsetX_;
    }

    void SetOffSetY(int32_t offset)
    {
        offsetY_ = offset;
    }

    int32_t GetOffSetY()
    {
        return offsetY_;
    }

    void SetOffset(int32_t offsetX, int32_t offsetY)
    {
        offsetX_ = offsetX;
        offsetY_ = offsetY;
    }

    void CollectSurface(const std::shared_ptr<RSBaseRenderNode>& node,
                        std::vector<RSBaseRenderNode::SharedPtr>& vec,
                        bool isUniRender) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void SetContextBounds(const Vector4f bounds);

    void SetTotalMatrix(const SkMatrix& totalMatrix)
    {
        totalMatrix_ = totalMatrix;
    }
    const SkMatrix& GetTotalMatrix() const
    {
        return totalMatrix_;
    }

    // pass render context (matrix/alpha/clip) from RT to RS
    void SetContextMatrix(const SkMatrix& transform, bool sendMsg = true);
    const SkMatrix& GetContextMatrix() const;

    void SetContextAlpha(float alpha, bool sendMsg = true);
    float GetContextAlpha() const;

    void SetContextClipRegion(SkRect clipRegion, bool sendMsg = true);
    const SkRect& GetContextClipRegion() const;

    void SetSecurityLayer(bool isSecurityLayer);
    bool GetSecurityLayer() const;

    std::shared_ptr<RSDirtyRegionManager> GetDirtyManager() const;

    void SetSrcRect(const RectI& rect)
    {
        srcRect_ = rect;
    }

    const RectI& GetSrcRect() const
    {
        return srcRect_;
    }

    void SetDstRect(const RectI& dstRect)
    {
        if (dstRect_ != dstRect) {
            dstRectChanged_ = true;
        }
        dstRect_ = dstRect;
    }

    const RectI& GetDstRect() const
    {
        return dstRect_;
    }

    Occlusion::Region& GetTransparentRegion()
    {
        return transparentRegion_;
    }

    Occlusion::Region& GetOpaqueRegion()
    {
        return opaqueRegion_;
    }

    void SetGlobalAlpha(float alpha)
    {
        if (globalAlpha_ == alpha) {
            return;
        }
        alphaChanged_ = true;
        globalAlpha_ = alpha;
    }

    float GetGlobalAlpha() const
    {
        return globalAlpha_;
    }

    void SetOcclusionVisible(bool visible)
    {
        isOcclusionVisible_ = visible;
    }

    bool GetOcclusionVisible() const
    {
        return isOcclusionVisible_;
    }

    const Occlusion::Region& GetVisibleRegion() const
    {
        return visibleRegion_;
    }

    void SetAbilityBGAlpha(uint8_t alpha)
    {
        abilityBgAlpha_ = alpha;
        alphaChanged_ = true;
    }

    uint8_t GetAbilityBgAlpha() const
    {
        return abilityBgAlpha_;
    }

    void setQosCal(bool qosPidCal)
    {
        qosPidCal_ = qosPidCal;
    }

    void SetVisibleRegionRecursive(const Occlusion::Region& region,
                                   VisibleData& visibleVec,
                                   std::map<uint32_t, bool>& pidVisMap);

    const Occlusion::Region& GetVisibleDirtyRegion() const
    {
        return visibleDirtyRegion_;
    }

    void SetVisibleDirtyRegion(const Occlusion::Region& region)
    {
        visibleDirtyRegion_ = region;
    }

    const Occlusion::Region& GetDirtyRegionBelowCurrentLayer() const
    {
        return dirtyRegionBelowCurrentLayer_;
    }

    void SetDirtyRegionBelowCurrentLayer(Occlusion::Region& region)
    {
#ifndef ROSEN_CROSS_PLATFORM
        Occlusion::Rect dirtyRect{GetOldDirtyInSurface()};
        Occlusion::Region dirtyRegion {dirtyRect};
        dirtyRegionBelowCurrentLayer_ = dirtyRegion.And(region);
        dirtyRegionBelowCurrentLayerIsEmpty_ = dirtyRegionBelowCurrentLayer_.IsEmpty();
#endif
    }

    bool GetDstRectChanged() const
    {
        return dstRectChanged_;
    }

    void CleanDstRectChanged()
    {
        dstRectChanged_ = false;
    }

    bool GetAlphaChanged() const
    {
        return alphaChanged_;
    }

    void CleanAlphaChanged()
    {
        alphaChanged_ = false;
    }

    void SetGloblDirtyRegion(const RectI& rect)
    {
        Occlusion::Rect tmpRect { rect.left_, rect.top_, rect.GetRight(), rect.GetBottom() };
        Occlusion::Region region { tmpRect };
        globalDirtyRegion_ = visibleRegion_.And(region);
        globalDirtyRegionIsEmpty_ = globalDirtyRegion_.IsEmpty();
    }

#ifndef ROSEN_CROSS_PLATFORM
    void SetColorSpace(ColorGamut colorSpace);
    ColorGamut GetColorSpace() const;
    void SetConsumer(const sptr<Surface>& consumer);
    GraphicBlendType GetBlendType();
    void SetBlendType(GraphicBlendType blendType);
#endif

    void UpdateSurfaceDefaultSize(float width, float height);

    // Only SurfaceNode in RS calls "RegisterBufferAvailableListener"
    // to save callback method sent by RT or UI which depends on the value of "isFromRenderThread".
    void RegisterBufferAvailableListener(
        sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread);

    // Only SurfaceNode in RT calls "ConnectToNodeInRenderService" to send callback method to RS
    void ConnectToNodeInRenderService();

    void NotifyRTBufferAvailable();
    bool IsNotifyRTBufferAvailable() const;
    bool IsNotifyRTBufferAvailablePre() const;

    void NotifyUIBufferAvailable();
    bool IsNotifyUIBufferAvailable() const;
    void SetIsNotifyUIBufferAvailable(bool available);

    // UI Thread would not be notified when SurfaceNode created by Video/Camera in RenderService has available buffer.
    // And RenderThread does not call mainFunc_ if nothing in UI thread is changed
    // which would cause callback for "clip" on parent SurfaceNode cannot be triggered
    // for "clip" is executed in RenderThreadVisitor::ProcessSurfaceRenderNode.
    // To fix this bug, we set callback which would call RSRenderThread::RequestNextVSync() to forcedly "refresh"
    // RenderThread when SurfaceNode in RenderService has available buffer and execute RSIBufferAvailableCallback.
    void SetCallbackForRenderThreadRefresh(std::function<void(void)> callback);
    bool NeedSetCallbackForRenderThreadRefresh();

    void ParallelVisitLock()
    {
        parallelVisitMutex_.lock();
    }

    void ParallelVisitUnlock()
    {
        parallelVisitMutex_.unlock();
    }

    bool SubNodeVisible(const RectI& r) const
    {
        Occlusion::Rect nodeRect { r.left_, r.top_, r.GetRight(), r.GetBottom() };
        // if current node is in occluded region of the surface, it could be skipped in process step
        return visibleRegion_.IsIntersectWith(nodeRect);
    }

    inline bool IsTransparent() const
    {
        const uint8_t opacity = 255;
        return !(GetAbilityBgAlpha() == opacity && ROSEN_EQ(GetGlobalAlpha(), 1.0f));
    }

    inline bool IsCurrentNodeInTransparentRegion(const Occlusion::Rect& nodeRect) const
    {
        return transparentRegion_.IsIntersectWith(nodeRect);
    }

    bool SubNodeIntersectWithDirty(const RectI& r) const
    {
        Occlusion::Rect nodeRect { r.left_, r.top_, r.GetRight(), r.GetBottom() };
        // if current node rect r is in global dirtyregion, it CANNOT be skipped
        if (!globalDirtyRegionIsEmpty_) {
            auto globalRect = globalDirtyRegion_.IsIntersectWith(nodeRect);
            if (globalRect) {
                return true;
            }
        }
        // if current node is in visible dirtyRegion, it CANNOT be skipped
        bool localIntersect = visibleDirtyRegion_.IsIntersectWith(nodeRect);
        if (localIntersect) {
            return true;
        }
        // if current node is transparent
        if (IsTransparent() || IsCurrentNodeInTransparentRegion(nodeRect)) {
            return dirtyRegionBelowCurrentLayer_.IsIntersectWith(nodeRect);
        }
        return false;
    }

    bool SubNodeNeedDraw(const RectI &r, PartialRenderType opDropType) const
    {
        if (dirtyManager_ == nullptr) {
            return true;
        }
        if (r.IsEmpty()) {
            return true;
        }
        switch (opDropType) {
            case PartialRenderType::SET_DAMAGE_AND_DROP_OP:
                return SubNodeIntersectWithDirty(r);
            case PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION:
                return SubNodeVisible(r);
            case PartialRenderType::SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY:
                return SubNodeVisible(r) && SubNodeIntersectWithDirty(r);
            case PartialRenderType::DISABLED:
            case PartialRenderType::SET_DAMAGE:
            default:
                return true;
        }
        return true;
    }

    void SetCacheSurface(sk_sp<SkSurface> cacheSurface)
    {
        cacheSurface_ = std::move(cacheSurface);
    }

    sk_sp<SkSurface> GetCacheSurface() const
    {
        return cacheSurface_;
    }

    void ClearCacheSurface()
    {
        cacheSurface_ = nullptr;
    }

    void SetAppFreeze(bool isAppFreeze)
    {
        isAppFreeze_ = isAppFreeze;
    }

    bool IsAppFreeze() const
    {
        return isAppFreeze_;
    }

    bool GetZorderChanged() const
    {
        return (std::abs(GetRenderProperties().GetPositionZ() - positionZ_) > (std::numeric_limits<float>::epsilon()));
    }

    bool IsZOrderPromoted() const
    {
        return GetRenderProperties().GetPositionZ() > positionZ_;
    }

    void UpdatePositionZ()
    {
        positionZ_ = GetRenderProperties().GetPositionZ();
    }

    inline bool HasContainerWindow() const
    {
        return hasContainerWindow_;
    }

    void SetContainerWindow(bool hasContainerWindow, float density)
    {
        hasContainerWindow_ = hasContainerWindow;
        // px = vp * density
        containerTitleHeight_ = ceil(CONTAINER_TITLE_HEIGHT * density);
        containerContentPadding_ = ceil(CONTENT_PADDING * density);
        containerBorderWidth_ = ceil(CONTAINER_BORDER_WIDTH * density);
        containerOutRadius_ = ceil(CONTAINER_OUTER_RADIUS * density);
        containerInnerRadius_ = ceil(CONTAINER_INNER_RADIUS * density);
    }

    bool IsOpaqueRegionChanged() const
    {
        return opaqueRegionChanged_;
    }

    bool IsFocusedWindow(pid_t focusedWindowPid)
    {
        return static_cast<pid_t>(GetNodeId() >> 32) == focusedWindowPid; // higher 32 bits of nodeid is pid
    }

    Occlusion::Region ResetOpaqueRegion(const RectI& absRect,
        const ContainerWindowConfigType containerWindowConfigType,
        const bool isFocusWindow)
    {
        if (containerWindowConfigType == ContainerWindowConfigType::DISABLED) {
            Occlusion::Rect opaqueRect{absRect};
            Occlusion::Region opaqueRegion = Occlusion::Region{opaqueRect};
            return opaqueRegion;
        }
        if (isFocusWindow) {
            Occlusion::Rect opaqueRect{ absRect.left_ + containerContentPadding_ + containerBorderWidth_,
                absRect.top_ + containerTitleHeight_ + containerInnerRadius_ + containerBorderWidth_,
                absRect.GetRight() - containerContentPadding_ - containerBorderWidth_,
                absRect.GetBottom() - containerContentPadding_ - containerBorderWidth_};
            Occlusion::Region opaqueRegion{opaqueRect};
            return opaqueRegion;
        } else {
            if (containerWindowConfigType == ContainerWindowConfigType::ENABLED_LEVEL_0) {
                Occlusion::Rect opaqueRect{ absRect.left_ + containerContentPadding_ + containerBorderWidth_,
                    absRect.top_ + containerTitleHeight_ + containerBorderWidth_,
                    absRect.GetRight() - containerContentPadding_ - containerBorderWidth_,
                    absRect.GetBottom() - containerContentPadding_ - containerBorderWidth_};
                Occlusion::Region opaqueRegion{opaqueRect};
                return opaqueRegion;
            } else if (containerWindowConfigType == ContainerWindowConfigType::ENABLED_UNFOCUSED_WINDOW_LEVEL_1) {
                Occlusion::Rect opaqueRect{ absRect.left_,
                    absRect.top_ + containerOutRadius_,
                    absRect.GetRight(),
                    absRect.GetBottom() - containerOutRadius_};
                Occlusion::Region opaqueRegion{opaqueRect};
                return opaqueRegion;
            } else {
                Occlusion::Rect opaqueRect1{ absRect.left_ + containerOutRadius_,
                    absRect.top_,
                    absRect.GetRight() - containerOutRadius_,
                    absRect.GetBottom()};
                Occlusion::Rect opaqueRect2{ absRect.left_,
                    absRect.top_ + containerOutRadius_,
                    absRect.GetRight(),
                    absRect.GetBottom() - containerOutRadius_};
                Occlusion::Region r1{opaqueRect1};
                Occlusion::Region r2{opaqueRect2};
                Occlusion::Region opaqueRegion = r1.Or(r2);
                return opaqueRegion;
            }
        }
    }

    void ResetSurfaceOpaqueRegion(const RectI& screeninfo, const RectI& absRect,
        ContainerWindowConfigType containerWindowConfigType, bool isFocusWindow = true)
    {
        Occlusion::Rect absRectR {absRect};
        Occlusion::Region oldOpaqueRegion { opaqueRegion_ };
        if (IsTransparent()) {
            opaqueRegion_ = Occlusion::Region();
            transparentRegion_ = Occlusion::Region{absRectR};
        } else {
            if (IsAppWindow() && HasContainerWindow()) {
                opaqueRegion_ = ResetOpaqueRegion(absRect, containerWindowConfigType, isFocusWindow);
            } else {
                opaqueRegion_ = Occlusion::Region{absRectR};
            }
            transparentRegion_ = Occlusion::Region{absRectR};
            transparentRegion_.SubSelf(opaqueRegion_);
        }
        Occlusion::Rect screen{screeninfo};
        Occlusion::Region screenRegion{screen};
        transparentRegion_.AndSelf(screenRegion);
        opaqueRegion_.AndSelf(screenRegion);
        opaqueRegionChanged_ = !oldOpaqueRegion.Xor(opaqueRegion_).IsEmpty();
    }

    bool IsStartAnimationFinished() const;
    void SetStartAnimationFinished();
    void SetCachedImage(sk_sp<SkImage> image)
    {
        SetDirty();
        std::lock_guard<std::mutex> lock(cachedImageMutex_);
        cachedImage_ = image;
    }

    sk_sp<SkImage> GetCachedImage() const
    {
        std::lock_guard<std::mutex> lock(cachedImageMutex_);
        return cachedImage_;
    }

    void ClearCachedImage()
    {
        std::lock_guard<std::mutex> lock(cachedImageMutex_);
        cachedImage_ = nullptr;
    }

private:
    void ClearChildrenCache(const std::shared_ptr<RSBaseRenderNode>& node);

    std::mutex mutexRT_;
    std::mutex mutexUI_;
    std::mutex mutex_;

    std::mutex parallelVisitMutex_;

    SkMatrix contextMatrix_ = SkMatrix::I();
    float contextAlpha_ = 1.0f;
    SkRect contextClipRect_ = SkRect::MakeEmpty();

    bool isSecurityLayer_ = false;
    RectI srcRect_;
    SkMatrix totalMatrix_;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    float globalAlpha_ = 1.0f;
    float positionZ_ = 0.0f;
    bool qosPidCal_ = false;

    std::string name_;
    RSSurfaceNodeType nodeType_ = RSSurfaceNodeType::DEFAULT;
#ifndef ROSEN_CROSS_PLATFORM
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    GraphicBlendType blendType_ = GraphicBlendType::GRAPHIC_BLEND_SRCOVER;
#endif
    bool isNotifyRTBufferAvailablePre_ = false;
    std::atomic<bool> isNotifyRTBufferAvailable_ = false;
    std::atomic<bool> isNotifyUIBufferAvailable_ = false;
    std::atomic_bool isBufferAvailable_ = false;
    sptr<RSIBufferAvailableCallback> callbackFromRT_;
    sptr<RSIBufferAvailableCallback> callbackFromUI_;
    std::function<void(void)> callbackForRenderThreadRefresh_ = nullptr;
    std::vector<NodeId> childSurfaceNodeIds_;
    friend class RSRenderThreadVisitor;
    RectI clipRegionFromParent_;
    Occlusion::Region visibleRegion_;
    Occlusion::Region visibleDirtyRegion_;
    bool isOcclusionVisible_ = true;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    RectI dstRect_;
    bool dstRectChanged_ = false;
    uint8_t abilityBgAlpha_ = 0;
    bool alphaChanged_ = false;
    Occlusion::Region globalDirtyRegion_;

    std::atomic<bool> isAppFreeze_ = false;
    sk_sp<SkSurface> cacheSurface_ = nullptr;
    bool globalDirtyRegionIsEmpty_ = false;
    // if a there a dirty layer under transparent clean layer, transparent layer should refreshed
    Occlusion::Region dirtyRegionBelowCurrentLayer_;
    bool dirtyRegionBelowCurrentLayerIsEmpty_ = false;

    // opaque region of the surface
    Occlusion::Region opaqueRegion_;
    bool opaqueRegionChanged_ = false;
    // transparent region of the surface, floating window's container window is always treated as transparent
    Occlusion::Region transparentRegion_;
    // temporary const value from ACE container_modal_constants.h, will be replaced by uniform interface
    bool hasContainerWindow_ = false;           // set to false as default, set by arkui
    const int CONTAINER_TITLE_HEIGHT = 37;        // container title height = 37 vp
    const int CONTENT_PADDING = 4;      // container <--> content distance 4 vp
    const int CONTAINER_BORDER_WIDTH = 1;          // container border width 2 vp
    const int CONTAINER_OUTER_RADIUS = 16;         // container outter radius 16 vp
    const int CONTAINER_INNER_RADIUS = 14;         // container inner radius 14 vp
    int containerTitleHeight_ = 37 * 2;      // The density default value is 2
    int containerContentPadding_ = 4 * 2;    // The density default value is 2
    int containerBorderWidth_ = 1 * 2;       // The density default value is 2
    int containerOutRadius_ = 16 * 2;        // The density default value is 2
    int containerInnerRadius_ = 14 * 2;      // The density default value is 2
    bool startAnimationFinished_ = false;
    mutable std::mutex cachedImageMutex_;
    sk_sp<SkImage> cachedImage_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
