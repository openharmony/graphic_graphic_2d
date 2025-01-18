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

#ifndef ROSEN_CROSS_PLATFORM
#include <ibuffer_consumer_listener.h>
#include <iconsumer_surface.h>
#include <surface.h>
#include "sync_fence.h"
#endif

#include "common/rs_macros.h"
#include "common/rs_occlusion_region.h"
#include "memory/rs_memory_track.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include <screen_manager/screen_types.h>
#include "screen_manager/rs_screen_info.h"
#ifdef NEW_RENDER_CONTEXT
#include "rs_render_surface.h"
#else
#include "platform/drawing/rs_surface.h"
#endif

namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNode;
class RSB_EXPORT RSDisplayRenderNode : public RSRenderNode {
public:
    struct ScreenRenderParams
    {
        ScreenInfo screenInfo;
        std::map<ScreenId, bool> displayHasSecSurface;
        std::map<ScreenId, bool> displayHasSkipSurface;
        std::map<ScreenId, bool> displayHasProtectedSurface;
        std::map<ScreenId, bool> displaySpecailSurfaceChanged;
        std::map<ScreenId, bool> hasCaptureWindow;
    };

    enum CompositeType {
        UNI_RENDER_COMPOSITE = 0,
        UNI_RENDER_MIRROR_COMPOSITE,
        UNI_RENDER_EXPAND_COMPOSITE,
        HARDWARE_COMPOSITE,
        SOFTWARE_COMPOSITE
    };
    using WeakPtr = std::weak_ptr<RSDisplayRenderNode>;
    using SharedPtr = std::shared_ptr<RSDisplayRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::DISPLAY_NODE;

    ~RSDisplayRenderNode() override;
    void SetIsOnTheTree(bool flag, NodeId instanceRootNodeId = INVALID_NODEID,
        NodeId firstLevelNodeId = INVALID_NODEID, NodeId cacheNodeId = INVALID_NODEID,
        NodeId uifirstRootNodeId = INVALID_NODEID, NodeId displayNodeId = INVALID_NODEID) override;

    void SetScreenId(uint64_t screenId)
    {
        screenId_ = screenId;
    }

    uint64_t GetScreenId() const
    {
        return screenId_;
    }

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

    int32_t GetDisplayOffsetY() const
    {
        return offsetY_;
    }

    bool GetFingerprint() const
    {
        return hasFingerprint_;
    }

    void SetFingerprint(bool hasFingerprint)
    {
        hasFingerprint_ = hasFingerprint;
    }

    void SetScreenRotation(const ScreenRotation& screenRotation)
    {
        screenRotation_ = screenRotation;
    }

    ScreenRotation GetScreenRotation()
    {
        return screenRotation_;
    }

    bool IsMirrorScreen() const
    {
        return isMirrorScreen_;
    }

    void SetIsMirrorScreen(bool isMirrorScreen)
    {
        isMirrorScreen_ = isMirrorScreen;
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

    void SetSecurityExemption(bool isSecurityExemption)
    {
        isSecurityExemption_ = isSecurityExemption;
    }

    bool GetSecurityExemption() const
    {
        return isSecurityExemption_;
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
    void UpdateScreenRenderParams(ScreenRenderParams& screenRenderParams);
    void UpdateOffscreenRenderParams(bool needOffscreen);
    void RecordMainAndLeashSurfaces(RSBaseRenderNode::SharedPtr surface);
    std::vector<RSBaseRenderNode::SharedPtr>& GetAllMainAndLeashSurfaces() { return curMainAndLeashSurfaceNodes_;}
    std::vector<RSBaseRenderNode::SharedPtr>& GetCurAllSurfaces(bool onlyFirstLevel)
    {
        return onlyFirstLevel ? curAllFirstLevelSurfaces_ : curAllSurfaces_;
    }

    void UpdateRotation();
    bool IsRotationChanged() const;
    bool IsLastRotationChanged() const {
        return lastRotationChanged;
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

    void SetHDRPresent(bool hdrPresent);

    void SetBrightnessRatio(float brightnessRatio);

    void SetColorSpace(const GraphicColorGamut& newColorSpace);
    GraphicColorGamut GetColorSpace() const;

    std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& GetDirtySurfaceNodeMap()
    {
        return dirtySurfaceNodeMap_;
    }

    void SetMainAndLeashSurfaceDirty(bool isDirty);

    // Use in MultiLayersPerf
    size_t GetSurfaceCountForMultiLayersPerf() const
    {
        return surfaceCountForMultiLayersPerf_;
    }

    void SetScbNodePid(const std::vector<int32_t>& oldScbPids, int32_t currentScbPid)
    {
        oldScbPids_ = oldScbPids;
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

    // Window Container
    void SetWindowContainer(std::shared_ptr<RSBaseRenderNode> container);
    std::shared_ptr<RSBaseRenderNode> GetWindowContainer() const;

protected:
    void OnSync() override;
private:
    explicit RSDisplayRenderNode(
        NodeId id, const RSDisplayNodeConfig& config, const std::weak_ptr<RSContext>& context = {});
    void InitRenderParams() override;
    void HandleCurMainAndLeashSurfaceNodes();
    // vector of sufacenodes will records dirtyregions by itself
    std::vector<RSBaseRenderNode::SharedPtr> curMainAndLeashSurfaceNodes_;
    CompositeType compositeType_ { HARDWARE_COMPOSITE };
    bool isMirrorScreen_ = false;
    ScreenRotation screenRotation_ = ScreenRotation::ROTATION_0;
    ScreenRotation originScreenRotation_ = ScreenRotation::ROTATION_0;
    uint64_t screenId_ = 0;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    uint32_t rogWidth_ = 0;
    uint32_t rogHeight_ = 0;
    bool forceSoftComposite_ { false };
    bool isMirroredDisplay_ = false;
    bool isSecurityDisplay_ = false;
    WeakPtr mirrorSource_;
    float lastRotation_ = 0.f;
    bool preRotationStatus_ = false;
    bool curRotationStatus_ = false;
    bool lastRotationChanged = false;
    Drawing::Matrix initMatrix_;
    bool isFirstTimeToProcessor_ = true;
    bool hasFingerprint_ = false;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;

    std::map<NodeId, RectI> lastFrameSurfacePos_;
    std::map<NodeId, RectI> currentFrameSurfacePos_;
    std::vector<std::pair<NodeId, RectI>> lastFrameSurfacesByDescZOrder_;
    std::vector<std::pair<NodeId, RectI>> currentFrameSurfacesByDescZOrder_;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    std::vector<std::string> windowsName_;

    std::vector<NodeId> securityLayerList_;
    bool isSecurityExemption_ = false;

    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces_;
    std::vector<RSBaseRenderNode::SharedPtr> curAllFirstLevelSurfaces_;
    std::mutex mtx_;

    // Use in screen recording optimization
    std::shared_ptr<Drawing::Image> offScreenCacheImgForCapture_ = nullptr;

    // Use in vulkan parallel rendering
    bool isParallelDisplayNode_ = false;

    // Use in MultiLayersPerf
    size_t surfaceCountForMultiLayersPerf_ = 0;

    std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> dirtySurfaceNodeMap_;

    std::vector<int32_t> oldScbPids_ {};
    int32_t currentScbPid_ = -1;
    mutable bool isNeedWaitNewScbPid_ = false;
    mutable std::shared_ptr<std::vector<std::shared_ptr<RSRenderNode>>> currentChildrenList_ =
        std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();

    // Window Container
    std::shared_ptr<RSBaseRenderNode> windowContainer_;

    friend class DisplayNodeCommandHelper;

    int64_t lastRefreshTime_ = 0;

    bool curZoomState_ = false;
    bool preZoomState_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H
