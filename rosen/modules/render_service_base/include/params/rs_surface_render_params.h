/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H

#include <memory>
#include <string>

#include "common/rs_occlusion_region.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_base_render_node.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_buffer.h"
#include "sync_fence.h"
#endif
#include "surface_type.h"

namespace OHOS::Rosen {
class RSSurfaceRenderNode;
struct RSLayerInfo {
#ifndef ROSEN_CROSS_PLATFORM
    GraphicIRect srcRect;
    GraphicIRect dstRect;
    GraphicIRect boundRect;
    Drawing::Matrix matrix;
    int32_t gravity = 0;
    int32_t zOrder = 0;
    float alpha = 1.f;
    GraphicBlendType blendType;
    GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    int32_t layerSource;
    bool arsrTag = true;

    bool operator==(const RSLayerInfo& layerInfo) const
    {
        return (srcRect == layerInfo.srcRect) && (dstRect == layerInfo.dstRect) &&
            (boundRect == layerInfo.boundRect) && (matrix == layerInfo.matrix) && (gravity == layerInfo.gravity) &&
            (zOrder == layerInfo.zOrder) && (blendType == layerInfo.blendType) &&
            (transformType == layerInfo.transformType) && (ROSEN_EQ(alpha, layerInfo.alpha)) &&
            (layerSource == layerInfo.layerSource)  && (arsrTag == layerInfo.arsrTag);
    }
#endif
};
struct RSWindowInfo {
    bool isMainWindowType_ = false;
    bool isLeashWindow_ = false;
    bool isAppWindow_ = false;
    void SetWindowInfo(bool isMainWindowType, bool isLeashWindow, bool isAppWindow)
    {
        isMainWindowType_ = isMainWindowType;
        isLeashWindow_ = isLeashWindow;
        isAppWindow_ = isAppWindow;
    }
};
class RSB_EXPORT RSSurfaceRenderParams : public RSRenderParams {
public:
    explicit RSSurfaceRenderParams(NodeId id);
    ~RSSurfaceRenderParams() override = default;
    inline bool IsMainWindowType() const
    {
        return windowInfo_.isMainWindowType_;
    }
    inline bool IsLeashWindow() const override
    {
        return windowInfo_.isLeashWindow_;
    }
    bool IsAppWindow() const override
    {
        return windowInfo_.isAppWindow_;
    }
    void SetWindowInfo(bool isMainWindowType, bool isLeashWindow, bool isAppWindow)
    {
        windowInfo_.SetWindowInfo(isMainWindowType, isLeashWindow, isAppWindow);
    }
    RSSurfaceNodeType GetSurfaceNodeType() const
    {
        return rsSurfaceNodeType_;
    }
    SelfDrawingNodeType GetSelfDrawingNodeType() const
    {
        return selfDrawingType_;
    }
    void SetAncestorDisplayNode(const RSRenderNode::WeakPtr& ancestorDisplayNode)
    {
        ancestorDisplayNode_ = ancestorDisplayNode;
        auto node = ancestorDisplayNode.lock();
        ancestorDisplayDrawable_ = node ? node->GetRenderDrawable() : nullptr;
    }
  
    RSRenderNode::WeakPtr GetAncestorDisplayNode() const
    {
        return ancestorDisplayNode_;
    }
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr GetAncestorDisplayDrawable() const
    {
        return ancestorDisplayDrawable_;
    }

    float GetAlpha() const
    {
        return alpha_;
    }
    bool IsSpherizeValid() const
    {
        return isSpherizeValid_;
    }
    bool IsAttractionValid() const
    {
        return isAttractionValid_;
    }
    bool NeedBilinearInterpolation() const
    {
        return needBilinearInterpolation_;
    }
    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }
    const RectI& GetAbsDrawRect() const override
    {
        return absDrawRect_;
    }
    const RRect& GetRRect() const
    {
        return rrect_;
    }

    bool GetIsSecurityLayer() const
    {
        return isSecurityLayer_;
    }
    bool GetIsSkipLayer() const
    {
        return isSkipLayer_;
    }
    bool GetIsSnapshotSkipLayer() const
    {
        return isSnapshotSkipLayer_;
    }
    bool GetIsProtectedLayer() const
    {
        return isProtectedLayer_;
    }
    bool GetAnimateState() const
    {
        return animateState_;
    }
    const std::set<NodeId>& GetSecurityLayerIds() const
    {
        return securityLayerIds_;
    }
    const std::set<NodeId>& GetSkipLayerIds() const
    {
        return skipLayerIds_;
    }
    const std::set<NodeId>& GetSnapshotSkipLayerIds() const
    {
        return snapshotSkipLayerIds_;
    }
    bool HasSecurityLayer()
    {
        return securityLayerIds_.size() != 0;
    }
    bool HasSkipLayer()
    {
        return skipLayerIds_.size() != 0;
    }
    bool HasSnapshotSkipLayer()
    {
        return snapshotSkipLayerIds_.size() != 0;
    }
    bool HasProtectedLayer()
    {
        return protectedLayerIds_.size() != 0;
    }
    bool HasPrivacyContentLayer()
    {
        return privacyContentLayerIds_.size() != 0;
    }

    LeashPersistentId GetLeashPersistentId() const
    {
        return leashPersistentId_;
    }

    std::string GetName() const
    {
        return name_;
    }

    void SetLeashWindowVisibleRegionEmptyParam(bool isLeashWindowVisibleRegionEmpty)
    {
        if (isLeashWindowVisibleRegionEmpty_ == isLeashWindowVisibleRegionEmpty) {
            return;
        }
        isLeashWindowVisibleRegionEmpty_ = isLeashWindowVisibleRegionEmpty;
        needSync_ = true;
    }

    bool GetLeashWindowVisibleRegionEmptyParam() const
    {
        return isLeashWindowVisibleRegionEmpty_;
    }

    void SetUifirstNodeEnableParam(MultiThreadCacheType isUifirst)
    {
        if (uiFirstFlag_ == isUifirst) {
            return;
        }
        uiFirstFlag_ = isUifirst;
        needSync_ = true;
    }

    MultiThreadCacheType GetUifirstNodeEnableParam() const
    {
        return uiFirstFlag_;
    }

    void SetBufferClearCacheSet(const std::set<int32_t> bufferCacheSet)
    {
        if (bufferCacheSet.size() > 0) {
            bufferCacheSet_ = bufferCacheSet;
            needSync_ = true;
        }
    }

    const std::set<int32_t> GetBufferClearCacheSet()
    {
        return bufferCacheSet_;
    }

    void SetIsParentUifirstNodeEnableParam(bool isUifirstParent)
    {
        if (uiFirstParentFlag_ == isUifirstParent) {
            return;
        }
        uiFirstParentFlag_ = isUifirstParent;
        needSync_ = true;
    }

    void SetUifirstUseStarting(NodeId id)
    {
        if (uifirstUseStarting_ == id) {
            return;
        }
        uifirstUseStarting_ = id;
        needSync_ = true;
    }

    NodeId GetUifirstUseStarting() const
    {
        return uifirstUseStarting_;
    }

    void SetUifirstChildrenDirtyRectParam(const RectI& rect)
    {
        childrenDirtyRect_ = rect;
        needSync_ = true;
    }

    RectI& GetUifirstChildrenDirtyRectParam()
    {
        return childrenDirtyRect_;
    }
    const RectI& GetDstRect() const
    {
        return dstRect_;
    }
    void SetSurfaceCacheContentStatic(bool contentStatic, bool lastFrameSynced);
    bool GetSurfaceCacheContentStatic() const;
    bool GetPreSurfaceCacheContentStatic() const;

    float GetPositionZ() const;

    void SetSurfaceSubTreeDirty(bool isSubTreeDirty);
    bool GetSurfaceSubTreeDirty() const;

    bool GetParentUifirstNodeEnableParam()
    {
        return uiFirstParentFlag_;
    }

    void SetUIFirstFrameGravity(Gravity gravity)
    {
        if (uiFirstFrameGravity_ == gravity) {
            return;
        }
        uiFirstFrameGravity_ = gravity;
        needSync_ = true;
    }

    Gravity GetUIFirstFrameGravity() const
    {
        return uiFirstFrameGravity_;
    }

    void SetOcclusionVisible(bool visible);
    bool GetOcclusionVisible() const override;

    void SetIsParentScaling(bool isParentScaling);
    bool IsParentScaling() const;

    void SetTransparentRegion(const Occlusion::Region& transparentRegion);
    const Occlusion::Region& GetTransparentRegion() const;

    void SetOldDirtyInSurface(const RectI& oldDirtyInSurface) override;
    RectI GetOldDirtyInSurface() const override;

    void SetVisibleRegion(const Occlusion::Region& visibleRegion);
    Occlusion::Region GetVisibleRegion() const override;

    void SetVisibleRegionInVirtual(const Occlusion::Region& visibleRegion);
    Occlusion::Region GetVisibleRegionInVirtual() const;

    void SetOccludedByFilterCache(bool val);
    bool GetOccludedByFilterCache() const;

    void SetFilterCacheFullyCovered(bool val);
    bool GetFilterCacheFullyCovered() const;

    const std::vector<NodeId>& GetVisibleFilterChild() const;
    bool IsTransparent() const;
    void CheckValidFilterCacheFullyCoverTarget(
        bool isFilterCacheValidForOcclusion, const RectI& filterCachedRect, const RectI& targetRect);

    void SetLayerInfo(const RSLayerInfo& layerInfo);
    const RSLayerInfo& GetLayerInfo() const override;
    void SetHardwareEnabled(bool enabled);
    bool GetHardwareEnabled() const override;
    void SetNeedMakeImage(bool enabled);
    bool GetNeedMakeImage() const override;
    void SetLastFrameHardwareEnabled(bool enabled);
    bool GetLastFrameHardwareEnabled() const override;
    void SetFixRotationByUser(bool flag);
    bool GetFixRotationByUser() const;
    void SetInFixedRotation(bool flag);
    bool IsInFixedRotation() const;
    // source crop tuning
    void SetLayerSourceTuning(int32_t needSourceTuning);
    int32_t GetLayerSourceTuning() const;

    void SetGpuOverDrawBufferOptimizeNode(bool overDrawNode);
    bool IsGpuOverDrawBufferOptimizeNode() const;
    void SetOverDrawBufferNodeCornerRadius(const Vector4f& radius);
    const Vector4f& GetOverDrawBufferNodeCornerRadius() const;

    void SetIsSubSurfaceNode(bool isSubSurfaceNode);
    bool IsSubSurfaceNode() const;

    void SetSkipDraw(bool skip);
    bool GetSkipDraw() const;

    void SetHidePrivacyContent(bool needHidePrivacyContent);
    bool GetHidePrivacyContent() const;

    void SetLayerTop(bool isTop);
    bool IsLayerTop() const;

    bool IsVisibleDirtyRegionEmpty(const Drawing::Region curSurfaceDrawRegion) const;

#ifndef ROSEN_CROSS_PLATFORM
    void SetBuffer(const sptr<SurfaceBuffer>& buffer, const Rect& damageRect) override;
    sptr<SurfaceBuffer> GetBuffer() const override;
    void SetPreBuffer(const sptr<SurfaceBuffer>& preBuffer) override;
    sptr<SurfaceBuffer> GetPreBuffer() override;
    void SetAcquireFence(const sptr<SyncFence>& acquireFence) override;
    sptr<SyncFence> GetAcquireFence() const override;
    const Rect& GetBufferDamage() const override;
    inline void SetBufferSynced(bool bufferSynced)
    {
        bufferSynced_ = bufferSynced;
    }
    bool IsBufferSynced() const
    {
        return bufferSynced_;
    }
#endif

    virtual void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    // DFX
    std::string ToString() const override;
    // Set/Get OpaqueRegion, currently only used for DFX
    void SetOpaqueRegion(const Occlusion::Region& opaqueRegion);
    const Occlusion::Region& GetOpaqueRegion() const;

    void SetNeedOffscreen(bool needOffscreen)
    {
        if (needOffscreen_ == needOffscreen) {
            return;
        }
        needOffscreen_ = needOffscreen;
        needSync_ = true;
    }

    bool GetNeedOffscreen() const
    {
        return RSSystemProperties::GetSurfaceOffscreenEnadbled() ? needOffscreen_ : false;
    }

    void SetLayerCreated(bool layerCreated) override
    {
        layerCreated_ = layerCreated;
    }

    bool GetLayerCreated() const override
    {
        return layerCreated_;
    }
    void SetTotalMatrix(const Drawing::Matrix& totalMatrix) override
    {
        if (totalMatrix_ == totalMatrix) {
            return;
        }
        totalMatrix_ = totalMatrix;
        needSync_ = true;
    }
    const Drawing::Matrix& GetTotalMatrix() override
    {
        return totalMatrix_;
    }
    void SetFingerprint(bool hasFingerprint) override
    {
        if (hasFingerprint_ == hasFingerprint) {
            return;
        }
        hasFingerprint_ = hasFingerprint;
        needSync_ = true;
    }
    bool GetFingerprint() override {
        return false;
    }

    void SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo)
    {
        if (drmCornerRadiusInfo_ == drmCornerRadiusInfo) {
            return;
        }
        drmCornerRadiusInfo_ = drmCornerRadiusInfo;
        needSync_ = true;
    }

    const std::vector<float>& GetCornerRadiusInfoForDRM() const
    {
        return drmCornerRadiusInfo_;
    }

    void SetSdrNit(float sdrNit)
    {
        if (ROSEN_EQ(sdrNit_, sdrNit)) {
            return;
        }
        sdrNit_ = sdrNit;
        needSync_ = true;
    }

    float GetSdrNit() const
    {
        return sdrNit_;
    }

    void SetDisplayNit(float displayNit)
    {
        if (ROSEN_EQ(displayNit_, displayNit)) {
            return;
        }
        displayNit_ = displayNit;
        needSync_ = true;
    }

    float GetDisplayNit() const
    {
        return displayNit_;
    }

    void SetBrightnessRatio(float brightnessRatio)
    {
        if (ROSEN_EQ(brightnessRatio_, brightnessRatio)) {
            return;
        }
        brightnessRatio_ = brightnessRatio;
        needSync_ = true;
    }

    float GetBrightnessRatio() const
    {
        return brightnessRatio_;
    }

    inline bool HasSubSurfaceNodes() const
    {
        return hasSubSurfaceNodes_;
    }
    const std::unordered_set<NodeId>& GetAllSubSurfaceNodeIds() const
    {
        return allSubSurfaceNodeIds_;
    }
    bool GetIsHwcEnabledBySolidLayer()
    {
        return isHwcEnabledBySolidLayer_;
    }

    void SetIsHwcEnabledBySolidLayer(bool isHwcEnabledBySolidLayer)
    {
        isHwcEnabledBySolidLayer_ = isHwcEnabledBySolidLayer;
    }

    void SetApiCompatibleVersion(uint32_t apiCompatibleVersion)
    {
        if (ROSEN_EQ(apiCompatibleVersion_, apiCompatibleVersion)) {
            return;
        }
        apiCompatibleVersion_ = apiCompatibleVersion;
        needSync_ = true;
    }
    uint32_t GetApiCompatibleVersion() const
    {
        return apiCompatibleVersion_;
    }

    void SetNeedCacheSurface(bool needCacheSurface);
    bool GetNeedCacheSurface() const;

protected:
private:
    RSSurfaceNodeType rsSurfaceNodeType_ = RSSurfaceNodeType::DEFAULT;
    SelfDrawingNodeType selfDrawingType_ = SelfDrawingNodeType::DEFAULT;
    RSRenderNode::WeakPtr ancestorDisplayNode_;
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr ancestorDisplayDrawable_;

    float alpha_ = 0;
    bool isTransparent_ = false;
    bool isSpherizeValid_ = false;
    bool isAttractionValid_ = false;
    bool isParentScaling_ = false;
    bool needBilinearInterpolation_ = false;
    MultiThreadCacheType uiFirstFlag_ = MultiThreadCacheType::NONE;
    bool uiFirstParentFlag_ = false;
    NodeId uifirstUseStarting_ = INVALID_NODEID;
    Color backgroundColor_ = RgbPalette::Transparent();
    bool isHwcEnabledBySolidLayer_ = false;

    RectI dstRect_;
    RectI oldDirtyInSurface_;
    RectI childrenDirtyRect_;
    RectI absDrawRect_;
    RRect rrect_;
    Occlusion::Region transparentRegion_;
    Occlusion::Region opaqueRegion_;

    LeashPersistentId leashPersistentId_ = INVALID_LEASH_PERSISTENTID;

    bool surfaceCacheContentStatic_ = false;
    bool preSurfaceCacheContentStatic_ = false;
    bool isSubTreeDirty_ = false;
    float positionZ_ = 0.0f;
    bool occlusionVisible_ = false;
    bool isLeashWindowVisibleRegionEmpty_ = false;
    Occlusion::Region visibleRegion_;
    Occlusion::Region visibleRegionInVirtual_;
    bool isOccludedByFilterCache_ = false;
    // if current surfaceNode has filter cache to occlude the back surfaceNode
    bool isFilterCacheFullyCovered_ = false;
    std::vector<NodeId> visibleFilterChild_;
    RSLayerInfo layerInfo_;
    RSWindowInfo windowInfo_;
#ifndef ROSEN_CROSS_PLATFORM
    sptr<SurfaceBuffer> buffer_ = nullptr;
    sptr<SurfaceBuffer> preBuffer_ = nullptr;
    sptr<SyncFence> acquireFence_ = SyncFence::InvalidFence();
    Rect damageRect_ = {0, 0, 0, 0};
    bool bufferSynced_ = true;
#endif
    bool isHardwareEnabled_ = false;
    bool needMakeImage_ = false;
    bool isLastFrameHardwareEnabled_ = false;
    bool isFixRotationByUser_ = false;
    bool isInFixedRotation_ = false;
    int32_t releaseInHardwareThreadTaskNum_ = 0;
    bool isSecurityLayer_ = false;
    bool isSkipLayer_ = false;
    bool isSnapshotSkipLayer_ = false;
    bool isProtectedLayer_ = false;
    bool animateState_ = false;
    bool isSubSurfaceNode_ = false;
    Gravity uiFirstFrameGravity_ = Gravity::TOP_LEFT;
    std::set<NodeId> skipLayerIds_= {};
    std::set<NodeId> snapshotSkipLayerIds_= {};
    std::set<NodeId> securityLayerIds_= {};
    std::set<NodeId> protectedLayerIds_= {};
    std::set<NodeId> privacyContentLayerIds_ = {};
    std::set<int32_t> bufferCacheSet_ = {};
    std::string name_= "";
    Vector4f overDrawBufferNodeCornerRadius_;
    bool isGpuOverDrawBufferOptimizeNode_ = false;
    bool isSkipDraw_ = false;
    bool isLayerTop_ = false;
    bool needHidePrivacyContent_ = false;
    bool needOffscreen_ = false;
    bool layerCreated_ = false;
    int32_t layerSource_ = 0;
    std::vector<float> drmCornerRadiusInfo_;

    Drawing::Matrix totalMatrix_;
    float globalAlpha_ = 1.0f;
    bool hasFingerprint_ = false;
    // hdr
    float sdrNit_ = 500.0f; // default sdrNit
    float displayNit_ = 500.0f; // default displayNit_
    float brightnessRatio_ = 1.0f; // 1.0f means no discount.
    bool needCacheSurface_ = false;

    bool hasSubSurfaceNodes_ = false;
    std::unordered_set<NodeId> allSubSurfaceNodeIds_ = {};

    uint32_t apiCompatibleVersion_ = 0;

    friend class RSSurfaceRenderNode;
    friend class RSUniRenderProcessor;
    friend class RSUniRenderThread;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H
