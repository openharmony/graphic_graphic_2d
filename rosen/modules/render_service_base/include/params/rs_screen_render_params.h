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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_SCREEN_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_SCREEN_RENDER_PARAMS_H

#include <memory>

#include "common/rs_macros.h"
#include "common/rs_occlusion_region.h"
#include "common/rs_special_layer_manager.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_render_node.h"
#include "screen_manager/rs_screen_info.h"
#include "pipeline/rs_surface_render_node.h"
namespace OHOS::Rosen {
class RSB_EXPORT RSScreenRenderParams : public RSRenderParams {
public:
    explicit RSScreenRenderParams(NodeId id);
    ~RSScreenRenderParams() override = default;

    void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    std::vector<RSBaseRenderNode::SharedPtr>& GetAllMainAndLeashSurfaces();
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& GetAllMainAndLeashSurfaceDrawables() override;
    void SetAllMainAndLeashSurfaces(std::vector<RSBaseRenderNode::SharedPtr>& allMainAndLeashSurfaces);
    void SetAllMainAndLeashSurfaceDrawables(
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allMainAndLeashSurfaces);

    uint64_t GetScreenId() const
    {
        return screenInfo_.id;
    }

    const ScreenInfo& GetScreenInfo() const
    {
        return screenInfo_;
    }

    const RSScreenProperty& GetScreenProperty() const { return screenProperty_; }

    bool IsDirtyAlignEnabled() const
    {
        return isDirtyAlignEnabled_;
    }

    void SetDirtyAlignEnabled(bool isDirtyAlignEnabled)
    {
        isDirtyAlignEnabled_ = isDirtyAlignEnabled;
    }

    CompositeType GetCompositeType() const
    {
        return compositeType_;
    }

    bool IsMirrorScreen() const
    {
        return isMirrorScreen_;
    }
    bool IsFirstVisitCrossNodeDisplay() const
    {
        return isFirstVisitCrossNodeDisplay_;
    }
    bool HasChildCrossNode() const
    {
        return hasChildCrossNode_;
    }
    
    uint32_t GetChildDisplayCount() const
    {
        return childDisplayCount_;
    }
    void SetGlobalZOrder(float zOrder);
    float GetGlobalZOrder() const;
    void SetMainAndLeashSurfaceDirty(bool isDirty);
    bool GetMainAndLeashSurfaceDirty() const;

    void SetFingerprint(bool hasFingerprint) override;
    bool GetFingerprint() override;

    void SetFixVirtualBuffer10Bit(bool isFixVirtualBuffer10Bit);
    bool GetFixVirtualBuffer10Bit() const;

    void SetExistHWCNode(bool isExistHWCNode);
    bool GetExistHWCNode() const;

    void CollectHdrStatus(HdrStatus hdrStatus);
    void ResetDisplayHdrStatus();
    HdrStatus GetScreenHDRStatus() const;

    void SetHDRPresent(bool hasHdrPresent);
    bool GetHDRPresent() const;

    void SetHDRStatusChanged(bool isHDRStatusChanged);
    bool IsHDRStatusChanged() const;

    void SetBrightnessRatio (float brightnessRatio);
    float GetBrightnessRatio() const;

    void SetNewColorSpace(const GraphicColorGamut& newColorSpace);
    GraphicColorGamut GetNewColorSpace() const;
    void SetNewPixelFormat(const GraphicPixelFormat& newPixelFormat);
    GraphicPixelFormat GetNewPixelFormat() const;

    void SetZoomed(bool isZoomed);
    bool GetZoomed() const;

    bool HasMirrorScreen() const;
    void SetHasMirrorScreen(bool hasMirrorScreen);

    void SetTargetSurfaceRenderNodeDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable);
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr GetTargetSurfaceRenderNodeDrawable() const;

    // Only used in virtual expand screen to record accumulate frame status
    void SetAccumulatedDirty(bool isAccumulatedDirty)
    {
        isAccumulatedDirty_ = isAccumulatedDirty;
    }

    bool GetAccumulatedDirty() const
    {
        return isAccumulatedDirty_;
    }

    void SetAccumulatedHdrStatusChanged(bool isHdrStatusChanged)
    {
        isAccumulatedHdrStatusChanged_ = isHdrStatusChanged;
    }

    bool GetAccumulatedHdrStatusChanged() const
    {
        return isAccumulatedHdrStatusChanged_;
    }

    void SetAccumulatedSpecialLayerStatusChanged(bool isAccumulatedSpecialLayerStatusChanged)
    {
        isAccumulatedSpecialLayerStatusChanged_ = isAccumulatedSpecialLayerStatusChanged;
    }

    bool GetAccumulatedSpecialLayerStatusChanged() const
    {
        return isAccumulatedSpecialLayerStatusChanged_;
    }

    const std::unordered_set<NodeId> GetLastBlackList() const
    {
        return lastBlackList_;
    }

    void SetLastBlackList(std::unordered_set<NodeId> curBlackList)
    {
        lastBlackList_ = curBlackList;
    }

    bool GetLastSecExemption() const
    {
        return lastSecExemption_;
    }

    void SetLastSecExemption(bool curSecExemption)
    {
        lastSecExemption_ = curSecExemption;
    }

    void ResetVirtualExpandAccumulatedParams()
    {
        isAccumulatedDirty_ = false;
        isAccumulatedHdrStatusChanged_ = false;
        isAccumulatedSpecialLayerStatusChanged_ = false;
    }

    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& GetRoundCornerDrawables()
    {
        return roundCornerSurfaceDrawables_;
    }

    void SetNeedForceUpdateHwcNodes(bool needForceUpdateHwcNodes);
    bool GetNeedForceUpdateHwcNodes() const;

    void SetSlrMatrix(Drawing::Matrix matrix)
    {
        slrMatrix_ = matrix;
    }
    Drawing::Matrix GetSlrMatrix() const
    {
        return slrMatrix_;
    }

    void SetHdrBrightnessRatio(float hdrBrightnessRatio)
    {
        hdrBrightnessRatio_ = hdrBrightnessRatio;
    }
    float GetHdrBrightnessRatio() const
    {
        return hdrBrightnessRatio_;
    }

    void SetDrawnRegion(const Occlusion::Region& region);
    const Occlusion::Region& GetDrawnRegion() const;

    // dfx
    std::string ToString() const override;

    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr GetMirrorSourceDrawable() override;
    const std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& GetDisplayDrawables()
    {
        return logicalDisplayNodeDrawables_;
    }

    bool GetForceFreeze() const;
    void SetForceFreeze(bool forceFreeze);
    bool GetHasMirroredScreenChanged() const;
    void SetHasMirroredScreenChanged(bool hasMirroredScreenChanged);

    bool IsVirtualSurfaceChanged() const
    {
        return isVirtualSurfaceChanged_;
    }

    void SetIsEqualVsyncPeriod(bool isEqualVsyncPeriod)
    {
        isEqualVsyncPeriod_ = isEqualVsyncPeriod;
    }
    bool IsEqualVsyncPeriod() const
    {
        return isEqualVsyncPeriod_;
    }
    void SetCloneNodeMap(
        const std::map<NodeId, DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr>& cloneNodeMap);
    std::map<NodeId, DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr>& GetCloneNodeMap()
    {
        return cloneNodeMap_;
    }

private:

    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> logicalDisplayNodeDrawables_;
    std::vector<RSBaseRenderNode::SharedPtr> allMainAndLeashSurfaces_;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allMainAndLeashSurfaceDrawables_;
    bool isDirtyAlignEnabled_ = false;
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr mirrorSourceDrawable_;
    ScreenInfo screenInfo_;
    RSScreenProperty screenProperty_;
    CompositeType compositeType_ = CompositeType::HARDWARE_COMPOSITE;
    uint32_t childDisplayCount_ = 0;
    HdrStatus screenHDRStatus_ = HdrStatus::NO_HDR;
    bool isMirrorScreen_ = false;
    bool isFirstVisitCrossNodeDisplay_ = false;
    bool hasChildCrossNode_ = false;
    bool isMainAndLeashSurfaceDirty_ = false;
    bool needForceUpdateHwcNodes_ = false;
    bool hasFingerprint_ = false;
    bool isFixVirtualBuffer10Bit_ = false;
    bool existHWCNode_ = false;
    bool hasHdrPresent_ = false;
    bool isHDRStatusChanged_ = false;
    // Only used in virtual expand screen to record accumulate frame status
    bool isAccumulatedDirty_ = false;
    bool isAccumulatedHdrStatusChanged_ = false;
    bool isAccumulatedSpecialLayerStatusChanged_ = false;
    bool isVirtualSurfaceChanged_ = false;
    bool isEqualVsyncPeriod_ = true;
    std::unordered_set<NodeId> lastBlackList_ = {};
    bool lastSecExemption_ = false;
    float brightnessRatio_ = 1.0f;
    float hdrBrightnessRatio_ = 1.0f;
    float zOrder_ = 0.0f;
    bool isZoomed_ = false;
    uint32_t mirrorDstCount_ = 0;
    bool hasMirrorScreen_ = false;
    Drawing::Matrix slrMatrix_;
    // vector of rcd drawable, should be removed in OH 6.0 rcd refactoring
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> roundCornerSurfaceDrawables_;
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr targetSurfaceRenderNodeDrawable_;
    friend class RSUniRenderVisitor;
    friend class RSScreenRenderNode;
    GraphicColorGamut newColorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    GraphicPixelFormat newPixelFormat_ = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    Occlusion::Region drawnRegion_;
    bool forceFreeze_ = false;
    bool hasMirroredScreenChanged_ = false;
    std::map<NodeId, DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr> cloneNodeMap_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_PARAMS_RS_SCREEN_RENDER_PARAMS_H