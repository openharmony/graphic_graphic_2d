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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H

#include <memory>
#include <mutex>
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

#ifndef ROSEN_CROSS_PLATFORM
#include <ibuffer_consumer_listener.h>
#include <iconsumer_surface.h>
#include <surface.h>
#include "sync_fence.h"
#endif

#include "common/rs_macros.h"
#include "common/rs_occlusion_region.h"
#include "common/rs_special_layer_manager.h"
#include "display_engine/rs_luminance_control.h"
#include "memory/rs_memory_track.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include <screen_manager/screen_types.h>
#include "screen_manager/rs_screen_info.h"
#include "platform/drawing/rs_surface.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNode;
typedef void (*ReleaseDmaBufferTask)(uint64_t);

class RSB_EXPORT RSDisplayRenderNode : public RSRenderNode {
public:
    struct ScreenRenderParams {
        ScreenInfo screenInfo;
        std::map<ScreenId, bool> displaySpecailSurfaceChanged;
        std::map<ScreenId, bool> hasCaptureWindow;
    };

    enum CompositeType {
        UNI_RENDER_COMPOSITE = 0,
        UNI_RENDER_MIRROR_COMPOSITE,
        UNI_RENDER_EXPAND_COMPOSITE,
        HARDWARE_COMPOSITE,
        SOFTWARE_COMPOSITE,
        UNKNOWN
    };
    using WeakPtr = std::weak_ptr<RSDisplayRenderNode>;
    using SharedPtr = std::shared_ptr<RSDisplayRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::DISPLAY_NODE;

    ~RSDisplayRenderNode() override;
    void SetIsOnTheTree(bool flag, NodeId instanceRootNodeId = INVALID_NODEID,
        NodeId firstLevelNodeId = INVALID_NODEID, NodeId cacheNodeId = INVALID_NODEID,
        NodeId uifirstRootNodeId = INVALID_NODEID, NodeId displayNodeId = INVALID_NODEID) override;

    void SetScreenId(uint64_t screenId);

    uint64_t GetScreenId() const
    {
        return screenId_;
    }

    static void SetReleaseTask(ReleaseDmaBufferTask callback);

    void SetRogSize(uint32_t rogWidth, uint32_t rogHeight)
    {
        rogWidth_ = rogWidth;
        rogHeight_ = rogHeight;
    }

    uint32_t GetRogWidth() const
    {
        return rogWidth_;
    }

    uint32_t GetRogHeight() const
    {
        return rogHeight_;
    }

    void SetDisplayOffset(int32_t offsetX, int32_t offsetY)
    {
        offsetX_ = offsetX;
        offsetY_ = offsetY;
    }

    int32_t GetDisplayOffsetX() const
    {
        return offsetX_;
    }

    bool HasChildCrossNode() const
    {
        return hasChildCrossNode_;
    }

    void SetHasChildCrossNode(bool hasChildCrossNode)
    {
        hasChildCrossNode_ = hasChildCrossNode;
    }

    bool IsMirrorScreen() const
    {
        return isMirrorScreen_;
    }

    void SetIsMirrorScreen(bool isMirrorScreen)
    {
        isMirrorScreen_ = isMirrorScreen;
    }

    void SetIsFirstVisitCrossNodeDisplay(bool isFirstVisitCrossNodeDisplay)
    {
        isFirstVisitCrossNodeDisplay_ = isFirstVisitCrossNodeDisplay;
    }

    bool IsFirstVisitCrossNodeDisplay() const
    {
        return isFirstVisitCrossNodeDisplay_;
    }

    int32_t GetDisplayOffsetY() const
    {
        return offsetY_;
    }

    bool GetFingerprint() const
    {
        return hasFingerprint_;
    }

    void SetFingerprint(bool hasFingerprint);

    void SetScreenRotation(const ScreenRotation& screenRotation)
    {
        screenRotation_ = screenRotation;
    }

    ScreenRotation GetScreenRotation()
    {
        return screenRotation_;
    }

    void SetVirtualScreenMuteStatus(bool virtualScreenMuteStatus)
    {
        virtualScreenMuteStatus_ = virtualScreenMuteStatus;
    }

    bool GetVirtualScreenMuteStatus() const
    {
        return virtualScreenMuteStatus_;
    }

    void CollectSurface(
        const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec,
        bool isUniRender, bool onlyFirstLevel) override;
    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::DISPLAY_NODE;
    }

    bool IsMirrorDisplay() const;

    inline bool HasMirroredDisplayChanged() const noexcept
    {
        return hasMirroredDisplayChanged_;
    }

    inline void ResetMirroredDisplayChangedFlag() noexcept
    {
        hasMirroredDisplayChanged_ = false;
    }

    void SetCompositeType(CompositeType type);
    CompositeType GetCompositeType() const;
    void SetForceSoftComposite(bool flag);
    bool IsForceSoftComposite() const;
    void SetMirrorSource(SharedPtr node);
    void ResetMirrorSource();
    void SetIsMirrorDisplay(bool isMirror);
    void SetSecurityDisplay(bool isSecurityDisplay);
    bool GetSecurityDisplay() const;
    void SetDisplayGlobalZOrder(float zOrder);
    bool SkipFrame(uint32_t refreshRate, uint32_t skipFrameInterval) override;
    void SetBootAnimation(bool isBootAnimation) override;
    bool GetBootAnimation() const override;
    WeakPtr GetMirrorSource() const
    {
        return mirrorSource_;
    }

    bool HasDisappearingTransition(bool) const override
    {
        return false;
    }
    // Use in vulkan parallel rendering
    void SetIsParallelDisplayNode(bool isParallelDisplayNode)
    {
        isParallelDisplayNode_ = isParallelDisplayNode;
    }

    bool IsParallelDisplayNode() const
    {
        return isParallelDisplayNode_;
    }

    ScreenRotation GetRotation() const;

    std::shared_ptr<RSDirtyRegionManager> GetDirtyManager() const
    {
        return dirtyManager_;
    }
    void UpdateDisplayDirtyManager(int32_t bufferage, bool useAlignedDirtyRegion = false);
    void ClearCurrentSurfacePos();
    void UpdateSurfaceNodePos(NodeId id, RectI rect)
    {
        currentFrameSurfacePos_[id] = rect;
    }

    void AddSurfaceNodePosByDescZOrder(NodeId id, RectI rect)
    {
        currentFrameSurfacesByDescZOrder_.emplace_back(id, rect);
    }

    void AddSecurityLayer(NodeId id)
    {
        securityLayerList_.emplace_back(id);
    }

    void ClearSecurityLayerList()
    {
        securityLayerList_.clear();
    }

    const std::vector<NodeId>& GetSecurityLayerList()
    {
        return securityLayerList_;
    }

    RSSpecialLayerManager& GetMultableSpecialLayerMgr()
    {
        return specialLayerManager_;
    }

    const RSSpecialLayerManager& GetSpecialLayerMgr() const
    {
        return specialLayerManager_;
    }

    void SetSecurityExemption(bool isSecurityExemption)
    {
        isSecurityExemption_ = isSecurityExemption;
    }

    bool GetSecurityExemption() const
    {
        return isSecurityExemption_;
    }

    void AddSecurityVisibleLayer(NodeId id)
    {
        securityVisibleLayerList_.emplace_back(id);
    }

    void ClearSecurityVisibleLayerList()
    {
        securityVisibleLayerList_.clear();
    }

    const std::vector<NodeId>& GetSecurityVisibleLayerList()
    {
        return securityVisibleLayerList_;
    }

    void SetHasSecLayerInVisibleRect(bool hasSecLayer) {
        bool lastHasSecLayerInVisibleRect = hasSecLayerInVisibleRect_;
        hasSecLayerInVisibleRect_ = hasSecLayer;
        hasSecLayerInVisibleRectChanged_ =
            lastHasSecLayerInVisibleRect != hasSecLayerInVisibleRect_;
    }

    RectI GetLastFrameSurfacePos(NodeId id)
    {
        if (lastFrameSurfacePos_.count(id) == 0) {
            return {};
        }
        return lastFrameSurfacePos_[id];
    }

    RectI GetCurrentFrameSurfacePos(NodeId id)
    {
        if (currentFrameSurfacePos_.count(id) == 0) {
            return {};
        }
        return currentFrameSurfacePos_[id];
    }

    const std::vector<RectI> GetSurfaceChangedRects() const
    {
        std::vector<RectI> rects;
        for (const auto& lastFrameSurfacePo : lastFrameSurfacePos_) {
            if (currentFrameSurfacePos_.find(lastFrameSurfacePo.first) == currentFrameSurfacePos_.end()) {
                rects.emplace_back(lastFrameSurfacePo.second);
            }
        }
        for (const auto& currentFrameSurfacePo : currentFrameSurfacePos_) {
            if (lastFrameSurfacePos_.find(currentFrameSurfacePo.first) == lastFrameSurfacePos_.end()) {
                rects.emplace_back(currentFrameSurfacePo.second);
            }
        }
        return rects;
    }

    std::vector<RSBaseRenderNode::SharedPtr>& GetCurAllSurfaces()
    {
        return curAllSurfaces_;
    }
    std::vector<RSBaseRenderNode::SharedPtr>& GetCurAllSurfaces(bool onlyFirstLevel)
    {
        return onlyFirstLevel ? curAllFirstLevelSurfaces_ : curAllSurfaces_;
    }

    void UpdateRenderParams() override;
    void UpdatePartialRenderParams();
    void UpdateScreenRenderParams(ScreenRenderParams& screenRenderParams);
    void UpdateOffscreenRenderParams(bool needOffscreen);
    Occlusion::Region GetTopSurfaceOpaqueRegion() const;
    void RecordTopSurfaceOpaqueRects(Occlusion::Rect rect);
    void RecordMainAndLeashSurfaces(RSBaseRenderNode::SharedPtr surface);
    std::vector<RSBaseRenderNode::SharedPtr>& GetAllMainAndLeashSurfaces() { return curMainAndLeashSurfaceNodes_;}

    void UpdateRotation();
    bool IsRotationChanged() const;
    bool IsLastRotationChanged() const {
        return lastRotationChanged_;
    }
    bool GetPreRotationStatus() const {
        return preRotationStatus_;
    }
    bool GetCurRotationStatus() const {
        return curRotationStatus_;
    }
    bool IsFirstTimeToProcessor() const {
        return isFirstTimeToProcessor_;
    }

    void SetOriginScreenRotation(const ScreenRotation& rotate) {
        originScreenRotation_ = rotate;
        isFirstTimeToProcessor_ = false;
    }
    ScreenRotation GetOriginScreenRotation() const {
        return originScreenRotation_;
    }

    void SetInitMatrix(const Drawing::Matrix& matrix) {
        initMatrix_ = matrix;
        isFirstTimeToProcessor_ = false;
    }

    const Drawing::Matrix& GetInitMatrix() const {
        return initMatrix_;
    }

    std::shared_ptr<Drawing::Image> GetOffScreenCacheImgForCapture() {
        return offScreenCacheImgForCapture_;
    }
    void SetOffScreenCacheImgForCapture(std::shared_ptr<Drawing::Image> offScreenCacheImgForCapture) {
        offScreenCacheImgForCapture_ = offScreenCacheImgForCapture;
    }

    void SetHasUniRenderHdrSurface(bool hasUniRenderHdrSurface)
    {
        hasUniRenderHdrSurface_ = hasUniRenderHdrSurface;
    }

    bool GetHasUniRenderHdrSurface() const
    {
        return hasUniRenderHdrSurface_;
    }

    void SetIsLuminanceStatusChange(bool isLuminanceStatusChange)
    {
        isLuminanceStatusChange_ = isLuminanceStatusChange;
    }

    bool GetIsLuminanceStatusChange() const
    {
        return isLuminanceStatusChange_;
    }

    void SetMainAndLeashSurfaceDirty(bool isDirty);

    void SetForceCloseHdr(bool isForceCloseHdr);

    bool GetForceCloseHdr() const;

    void SetHDRPresent(bool hdrPresent);

    void SetBrightnessRatio(float brightnessRatio);

    void SetPixelFormat(const GraphicPixelFormat& pixelFormat)
    {
        pixelFormat_ = pixelFormat;
    }

    GraphicPixelFormat GetPixelFormat() const
    {
        return pixelFormat_;
    }

    void SetColorSpace(const GraphicColorGamut& newColorSpace);
    GraphicColorGamut GetColorSpace() const;

    std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& GetDirtySurfaceNodeMap()
    {
        return dirtySurfaceNodeMap_;
    }

    void ClearSurfaceSrcRect()
    {
        surfaceSrcRects_.clear();
    }

    void ClearSurfaceDstRect()
    {
        surfaceDstRects_.clear();
    }

    void ClearSurfaceTotalMatrix()
    {
        surfaceTotalMatrix_.clear();
    }

    void SetSurfaceSrcRect(NodeId id, RectI rect)
    {
        surfaceSrcRects_[id] = rect;
    }

    void SetSurfaceDstRect(NodeId id, RectI rect)
    {
        surfaceDstRects_[id] = rect;
    }

    void SetSurfaceTotalMatrix(NodeId id, const Drawing::Matrix& totalMatrix)
    {
        surfaceTotalMatrix_[id] = totalMatrix;
    }

    RectI GetSurfaceSrcRect(NodeId id) const
    {
        auto iter = surfaceSrcRects_.find(id);
        if (iter == surfaceSrcRects_.cend()) {
            return RectI();
        }

        return iter->second;
    }

    RectI GetSurfaceDstRect(NodeId id) const
    {
        auto iter = surfaceDstRects_.find(id);
        if (iter == surfaceDstRects_.cend()) {
            return {};
        }

        return iter->second;
    }

    Drawing::Matrix GetSurfaceTotalMatrix(NodeId id) const
    {
        auto iter = surfaceTotalMatrix_.find(id);
        if (iter == surfaceTotalMatrix_.cend()) {
            return {};
        }

        return iter->second;
    }

    // Use in MultiLayersPerf
    size_t GetSurfaceCountForMultiLayersPerf() const
    {
        return surfaceCountForMultiLayersPerf_;
    }

    const std::vector<NodeId>& GetLastSurfaceIds() const {
        return lastSurfaceIds_;
    }

    void SetLastSurfaceIds(std::vector<NodeId> lastSurfaceIds) {
        lastSurfaceIds_ = std::move(lastSurfaceIds);
    }

    void SetScbNodePid(const std::vector<int32_t>& oldScbPids, int32_t currentScbPid)
    {
        oldScbPids_ = oldScbPids;
        lastScbPid_ = currentScbPid_;
        currentScbPid_ = currentScbPid;
        isNeedWaitNewScbPid_ = true;
        isFullChildrenListValid_ = false;
    }

    std::vector<int32_t> GetOldScbPids() const
    {
        return oldScbPids_;
    }

    int32_t GetCurrentScbPid() const
    {
        return currentScbPid_;
    }

    ChildrenListSharedPtr GetSortedChildren() const override;

    Occlusion::Region GetDisappearedSurfaceRegionBelowCurrent(NodeId currentSurface) const;

    void UpdateZoomState(bool state)
    {
        preZoomState_ = curZoomState_;
        curZoomState_ = state;
    }

    bool IsZoomStateChange() const;
    void HandleCurMainAndLeashSurfaceNodes();

    void CollectHdrStatus(HdrStatus hdrStatus)
    {
        displayTotalHdrStatus_ = static_cast<HdrStatus>(displayTotalHdrStatus_ | hdrStatus);
    }

    void ResetDisplayHdrStatus()
    {
        displayTotalHdrStatus_ = HdrStatus::NO_HDR;
    }

    HdrStatus GetDisplayHdrStatus() const
    {
        return displayTotalHdrStatus_;
    }

    void InsertHDRNode(NodeId id)
    {
        hdrNodeList_.insert(id);
    }

    void RemoveHDRNode(NodeId id)
    {
        hdrNodeList_.erase(id);
    }

    std::unordered_set<NodeId>& GetHDRNodeList()
    {
        return hdrNodeList_;
    }
    using ScreenStatusNotifyTask = std::function<void(bool)>;

    static void SetScreenStatusNotifyTask(ScreenStatusNotifyTask task);

    void NotifyScreenNotSwitching();

    // rcd node setter and getter, should be removed in OH 6.0 rcd refactoring
    void SetRcdSurfaceNodeTop(RSBaseRenderNode::SharedPtr node)
    {
        rcdSurfaceNodeTop_ = node;
    }

    void SetRcdSurfaceNodeBottom(RSBaseRenderNode::SharedPtr node)
    {
        rcdSurfaceNodeBottom_ = node;
    }

    RSBaseRenderNode::SharedPtr GetRcdSurfaceNodeTop()
    {
        return rcdSurfaceNodeTop_;
    }
    
    RSBaseRenderNode::SharedPtr GetRcdSurfaceNodeBottom()
    {
        return rcdSurfaceNodeBottom_;
    }

    // Window Container
    void SetWindowContainer(std::shared_ptr<RSBaseRenderNode> container);
    std::shared_ptr<RSBaseRenderNode> GetWindowContainer() const;

    void SetNeedForceUpdateHwcNodes(bool needForceUpdate, bool hasVisibleHwcNodes);

    void SetTargetSurfaceRenderNodeId(NodeId nodeId)
    {
        targetSurfaceRenderNodeId_ = nodeId;
    }

    NodeId GetTargetSurfaceRenderNodeId() const
    {
        return targetSurfaceRenderNodeId_;
    }

    bool HasMirrorDisplay() const
    {
        return hasMirrorDisplay_;
    }

    void SetHasMirrorDisplay(bool hasMirrorDisplay);

    void SetTargetSurfaceRenderNodeDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable);

    // Enable HWCompose
    RSHwcDisplayRecorder& HwcDisplayRecorder() { return hwcDisplayRecorder_; }

protected:
    void OnSync() override;
private:
    explicit RSDisplayRenderNode(
        NodeId id, const RSDisplayNodeConfig& config, const std::weak_ptr<RSContext>& context = {});
    void InitRenderParams() override;

    bool hasChildCrossNode_ = false;
    bool isMirrorScreen_ = false;
    bool isFirstVisitCrossNodeDisplay_ = false;
    bool forceSoftComposite_ { false };
    bool isMirroredDisplay_ = false;
    bool hasMirroredDisplayChanged_ = false;
    bool isSecurityDisplay_ = false;
    bool isForceCloseHdr_ = false;
    bool hasUniRenderHdrSurface_ = false;
    bool isLuminanceStatusChange_ = false;
    bool preRotationStatus_ = false;
    bool curRotationStatus_ = false;
    bool lastRotationChanged_ = false;
    bool isFirstTimeToProcessor_ = true;
    bool hasFingerprint_ = false;
    bool isSecurityExemption_ = false;
    bool hasSecLayerInVisibleRect_ = false;
    bool hasSecLayerInVisibleRectChanged_ = false;
    // Use in vulkan parallel rendering
    bool isParallelDisplayNode_ = false;
    mutable bool isNeedWaitNewScbPid_ = false;
    bool curZoomState_ = false;
    bool preZoomState_ = false;
    bool virtualScreenMuteStatus_ = false;
    CompositeType compositeType_ { HARDWARE_COMPOSITE };
    ScreenRotation screenRotation_ = ScreenRotation::ROTATION_0;
    ScreenRotation originScreenRotation_ = ScreenRotation::ROTATION_0;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    uint32_t rogWidth_ = 0;
    uint32_t rogHeight_ = 0;
    float lastRotation_ = 0.f;
    int32_t currentScbPid_ = -1;
    int32_t lastScbPid_ = -1;
    HdrStatus displayTotalHdrStatus_ = HdrStatus::NO_HDR;
    uint64_t screenId_ = 0;
    // save children hdr canvasNode id
    std::unordered_set<NodeId> hdrNodeList_;
    // Use in MultiLayersPerf
    size_t surfaceCountForMultiLayersPerf_ = 0;
    int64_t lastRefreshTime_ = 0;
    static ReleaseDmaBufferTask releaseScreenDmaBufferTask_;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    // Use in screen recording optimization
    std::shared_ptr<Drawing::Image> offScreenCacheImgForCapture_ = nullptr;
    mutable std::shared_ptr<std::vector<std::shared_ptr<RSRenderNode>>> currentChildrenList_ =
        std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    WeakPtr mirrorSource_;
    // vector of sufacenodes will records dirtyregions by itself
    std::vector<RSBaseRenderNode::SharedPtr> curMainAndLeashSurfaceNodes_;
    Drawing::Matrix initMatrix_;
    GraphicPixelFormat pixelFormat_ = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;

    std::map<NodeId, RectI> lastFrameSurfacePos_;
    std::map<NodeId, RectI> currentFrameSurfacePos_;
    std::vector<Occlusion::Rect> topSurfaceOpaqueRects_;
    std::vector<std::pair<NodeId, RectI>> lastFrameSurfacesByDescZOrder_;
    std::vector<std::pair<NodeId, RectI>> currentFrameSurfacesByDescZOrder_;
    std::vector<std::string> windowsName_;

    // Use in virtual screen security exemption
    std::vector<NodeId> securityLayerList_;  // leashPersistentId and surface node id
    RSSpecialLayerManager specialLayerManager_;

    // Use in mirror screen visible rect projection
    std::vector<NodeId> securityVisibleLayerList_;  // surface node id

    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces_;
    std::vector<RSBaseRenderNode::SharedPtr> curAllFirstLevelSurfaces_;
    std::mutex mtx_;

    std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> dirtySurfaceNodeMap_;

    // support multiscreen
    std::map<NodeId, RectI> surfaceSrcRects_;
    std::map<NodeId, RectI> surfaceDstRects_;
    std::map<NodeId, Drawing::Matrix> surfaceTotalMatrix_;

    std::vector<NodeId> lastSurfaceIds_;

    std::vector<int32_t> oldScbPids_ {};

    bool hasMirrorDisplay_ = false;

    // Use in round corner display
    // removed later due to rcd node will be handled by RS tree in OH 6.0 rcd refactoring
    RSBaseRenderNode::SharedPtr rcdSurfaceNodeTop_ = nullptr;
    RSBaseRenderNode::SharedPtr rcdSurfaceNodeBottom_ = nullptr;

    NodeId targetSurfaceRenderNodeId_ = INVALID_NODEID;
    friend class DisplayNodeCommandHelper;
    static inline ScreenStatusNotifyTask screenStatusNotifyTask_ = nullptr;

    // Window Container
    std::shared_ptr<RSBaseRenderNode> windowContainer_;

    // Enable HWCompose
    RSHwcDisplayRecorder hwcDisplayRecorder_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H
