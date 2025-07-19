/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SCREEN_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SCREEN_RENDER_NODE_H

#include <memory>
#include <mutex>
#include "common/rs_common_def.h"

#ifndef ROSEN_CROSS_PLATFORM
#include <ibuffer_consumer_listener.h>
#include <iconsumer_surface.h>
#include <surface.h>
#include "sync_fence.h"
#endif

#include "common/rs_macros.h"
#include "common/rs_occlusion_region.h"
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

class RSB_EXPORT RSScreenRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSScreenRenderNode>;
    using SharedPtr = std::shared_ptr<RSScreenRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::SCREEN_NODE;
    explicit RSScreenRenderNode(
        NodeId id, ScreenId screenId, const std::weak_ptr<RSContext>& context = {});
    ~RSScreenRenderNode() override;
    void SetIsOnTheTree(bool flag, NodeId instanceRootNodeId = INVALID_NODEID,
        NodeId firstLevelNodeId = INVALID_NODEID, NodeId cacheNodeId = INVALID_NODEID,
        NodeId uifirstRootNodeId = INVALID_NODEID, NodeId screenNodeId = INVALID_NODEID,
        NodeId logicalDisplayNodeId = INVALID_NODEID) override;

    uint64_t GetScreenId() const
    {
        return screenId_;
    }

    void SetScreenInfo(ScreenInfo info)
    {
        // To prevent the damage region from spreading, geometry properties
        // are set only when no geometry properties are initialized or when
        // geometry properties change.
        auto& properties = GetMutableRenderProperties();
        auto inGeometryProp = std::tuple {info.width, info.height, info.width, info.height};
        auto curGeometryProp = std::tuple {properties.GetBoundsWidth(), properties.GetBoundsHeight(),
            properties.GetFrameWidth(), properties.GetFrameHeight()};
        if ((isGeometryInitialized_ && inGeometryProp != curGeometryProp) || !isGeometryInitialized_) {
            isGeometryInitialized_ = true;
            properties.SetBounds({0, 0, info.width, info.height});
            properties.SetFrame({0, 0, info.width, info.height});
            screenInfo_ = std::move(info);
        }
    }

    const ScreenInfo& GetScreenInfo() const
    {
        return screenInfo_;
    }

    void SetScreenRect(RectI rect)
    {
        screenRect_ = rect;
    }

    RectI GetScreenRect() const
    {
        return screenRect_;
    }

    void SetBootAnimation(bool isBootAnimation) override
    {
        ROSEN_LOGD("SetBootAnimation:: id:[%{public}" PRIu64 ", isBootAnimation:%{public}d",
            GetId(), isBootAnimation);
        isBootAnimation_ = isBootAnimation;

        auto parent = GetParent().lock();
        if (parent == nullptr) {
            return;
        }
        if (isBootAnimation) {
            parent->SetContainBootAnimation(true);
        }
    }

    static void SetReleaseTask(ReleaseDmaBufferTask callback);

    int32_t GetScreenOffsetX() const
    {
        return screenInfo_.offsetX;
    }

    int32_t GetScreenOffsetY() const
    {
        return screenInfo_.offsetY;
    }
    
    bool HasChildCrossNode() const
    {
        return hasChildCrossNode_;
    }

    void SetHasChildCrossNode(bool hasChildCrossNode)
    {
        hasChildCrossNode_ = hasChildCrossNode;
    }

    void SetIsFirstVisitCrossNodeDisplay(bool isFirstVisitCrossNodeDisplay)
    {
        isFirstVisitCrossNodeDisplay_ = isFirstVisitCrossNodeDisplay;
    }

    bool IsFirstVisitCrossNodeDisplay() const
    {
        return isFirstVisitCrossNodeDisplay_;
    }

    bool GetFingerprint() const
    {
        return hasFingerprint_;
    }

    void SetFingerprint(bool hasFingerprint);

    void CollectSurface(
        const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec,
        bool isUniRender, bool onlyFirstLevel) override;
    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::SCREEN_NODE;
    }

    bool IsMirrorScreen() const;

    inline bool HasMirroredScreenChanged() const noexcept
    {
        return hasMirroredScreenChanged_;
    }

    inline void ResetMirroredScreenChangedFlag() noexcept
    {
        hasMirroredScreenChanged_ = false;
    }

    void SetCompositeType(CompositeType type);
    CompositeType GetCompositeType() const;
    void SetForceSoftComposite(bool flag);
    bool IsForceSoftComposite() const;
    void SetMirrorSource(SharedPtr node);
    void ResetMirrorSource();
    void SetIsMirrorScreen(bool isMirror);
    void SetDisplayGlobalZOrder(float zOrder);
    bool SkipFrame(uint32_t refreshRate, uint32_t skipFrameInterval) override;
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

    void UpdateRenderParams() override;
    void UpdatePartialRenderParams();
    void UpdateScreenRenderParams();
    Occlusion::Region GetTopSurfaceOpaqueRegion() const;
    void RecordTopSurfaceOpaqueRects(Occlusion::Rect rect);
    void RecordMainAndLeashSurfaces(RSBaseRenderNode::SharedPtr surface);
    std::vector<RSBaseRenderNode::SharedPtr>& GetAllMainAndLeashSurfaces() { return curMainAndLeashSurfaceNodes_;}

    std::vector<RSBaseRenderNode::SharedPtr>& GetCurAllSurfaces(bool onlyFirstLevel)
    {
        return onlyFirstLevel ? curAllFirstLevelSurfaces_ : curAllSurfaces_;
    }

    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& GetLogicalDisplayNodeDrawables()
    {
        return logicalDisplayNodeDrawables_;
    }

    void SetInitMatrix(const Drawing::Matrix& matrix) {
        initMatrix_ = matrix;
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
        return hasUniRenderHdrSurface_ && !GetForceCloseHdr();
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

    bool GetFirstFrameVirtualScreenInit() const
    {
        return isFirstFrameVirtualScreenInit_;
    }

    void SetFirstFrameVirtualScreenInit(bool isFirstFrameVirtualScreenInit)
    {
        isFirstFrameVirtualScreenInit_ = isFirstFrameVirtualScreenInit;
    }

    void SetFixVirtualBuffer10Bit(bool isFixVirtualBuffer10Bit);

    bool GetFixVirtualBuffer10Bit() const;

    void SetExistHWCNode(bool existHWCNode);

    bool GetExistHWCNode() const;

    void SetColorSpace(const GraphicColorGamut& newColorSpace);
    void UpdateColorSpace(const GraphicColorGamut& newColorSpace);
    void SelectBestGamut(const std::vector<ScreenColorGamut>& mode);
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
        lastDisplayTotalHdrStatus_ = displayTotalHdrStatus_;
        return displayTotalHdrStatus_;
    }

    HdrStatus GetLastDisplayHDRStatus() const
    {
        return lastDisplayTotalHdrStatus_;
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

    void SetNeedForceUpdateHwcNodes(bool needForceUpdate, bool hasVisibleHwcNodes);

    void SetTargetSurfaceRenderNodeId(NodeId nodeId)
    {
        targetSurfaceRenderNodeId_ = nodeId;
    }

    NodeId GetTargetSurfaceRenderNodeId() const
    {
        return targetSurfaceRenderNodeId_;
    }

    void SetHasMirrorScreen(bool hasMirrorScreen);

    void SetTargetSurfaceRenderNodeDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable);

    // Enable HWCompose
    RSHwcDisplayRecorder& HwcDisplayRecorder() { return hwcDisplayRecorder_; }

protected:
    void OnSync() override;
private:
    void InitRenderParams() override;

    bool hasChildCrossNode_ = false;
    bool isFirstVisitCrossNodeDisplay_ = false;
    bool forceSoftComposite_ { false };
    bool isMirroredScreen_ = false;
    bool hasMirroredScreenChanged_ = false;
    bool isSecurityDisplay_ = false;
    bool isForceCloseHdr_ = false;
    bool isFirstFrameVirtualScreenInit_ = true;
    bool isFixVirtualBuffer10Bit_ = false;
    bool existHWCNode_ = false;
    bool hasUniRenderHdrSurface_ = false;
    bool isLuminanceStatusChange_ = false;
    bool hasFingerprint_ = false;
    bool isGeometryInitialized_ = false;

    // Use in vulkan parallel rendering
    bool isParallelDisplayNode_ = false;
    mutable bool isNeedWaitNewScbPid_ = false;
    bool curZoomState_ = false;
    bool preZoomState_ = false;
    CompositeType compositeType_ = CompositeType::HARDWARE_COMPOSITE;
    int32_t currentScbPid_ = -1;
    int32_t lastScbPid_ = -1;
    HdrStatus displayTotalHdrStatus_ = HdrStatus::NO_HDR;
    mutable HdrStatus lastDisplayTotalHdrStatus_ = HdrStatus::NO_HDR;
    uint64_t screenId_ = 0;
    RectI screenRect_;
    // save children hdr canvasNode id
    std::unordered_set<NodeId> hdrNodeList_;
    // Use in MultiLayersPerf
    size_t surfaceCountForMultiLayersPerf_ = 0;
    int64_t lastRefreshTime_ = 0;
    static ReleaseDmaBufferTask releaseScreenDmaBufferTask_;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    // Use in screen recording optimization
    std::shared_ptr<Drawing::Image> offScreenCacheImgForCapture_ = nullptr;
    WeakPtr mirrorSource_;
    // vector of sufacenodes will records dirtyregions by itself
    std::vector<RSBaseRenderNode::SharedPtr> curMainAndLeashSurfaceNodes_;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> logicalDisplayNodeDrawables_;
    Drawing::Matrix initMatrix_;
    GraphicPixelFormat pixelFormat_ = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;

    std::map<NodeId, RectI> lastFrameSurfacePos_;
    std::map<NodeId, RectI> currentFrameSurfacePos_;
    std::vector<Occlusion::Rect> topSurfaceOpaqueRects_;
    std::vector<std::pair<NodeId, RectI>> lastFrameSurfacesByDescZOrder_;
    std::vector<std::pair<NodeId, RectI>> currentFrameSurfacesByDescZOrder_;
    std::vector<std::string> windowsName_;

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
    ScreenInfo screenInfo_;
    friend class DisplayNodeCommandHelper;

    // Enable HWCompose
    RSHwcDisplayRecorder hwcDisplayRecorder_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SCREEN_RENDER_NODE_H
