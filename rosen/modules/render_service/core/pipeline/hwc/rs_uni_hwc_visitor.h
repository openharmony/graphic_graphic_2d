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
#ifndef RS_UNI_HWC_VISITOR_H
#define RS_UNI_HWC_VISITOR_H

#include "pipeline/main_thread/rs_uni_render_visitor.h"

namespace OHOS {
namespace Rosen {
class RSCanvasRenderNode;
class RSUniHwcVisitor {
public:
    explicit RSUniHwcVisitor(RSUniRenderVisitor& visitor);
    ~RSUniHwcVisitor();

    // Compute
    void UpdateSrcRect(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);
    void UpdateDstRect(RSSurfaceRenderNode& node, const RectI& absRect, const RectI& clipRect);
    void UpdateHwcNodeByTransform(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);

    RectI GetHwcVisibleEffectDirty(RSRenderNode& node, const RectI& globalFilterRect) const;

    bool UpdateIsOffscreen(RSCanvasRenderNode& node);
    void RestoreIsOffscreen(bool isOffscreen) { isOffscreen_ = isOffscreen; }

    void UpdateForegroundColorValid(RSCanvasRenderNode& node);

    Color FindAppBackgroundColor(RSSurfaceRenderNode& node);
    bool CheckNodeOcclusion(const std::shared_ptr<RSRenderNode>& node,
        const RectI& nodeAbsRect, Color& nodeBgColor);
    bool CheckSubTreeOcclusion(const std::shared_ptr<RSRenderNode>& branchNode,
        const RectI& nodeAbsRect, std::stack<Color>& nodeBgColor);
    void ProcessSolidLayerDisabled(RSSurfaceRenderNode& node);
    void ProcessSolidLayerEnabled(RSSurfaceRenderNode& node);

    void UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node);
    void UpdateHwcNodeEnableByBufferSize(RSSurfaceRenderNode& node);
    void UpdateHwcNodeEnableByAlpha(const std::shared_ptr<RSSurfaceRenderNode>& node);
    void UpdateHwcNodeEnableByRotate(const std::shared_ptr<RSSurfaceRenderNode>& node);
    void UpdateHwcNodeEnable();
    void UpdateHwcNodeEnableByNodeBelow();
    void UpdateHwcNodeEnableByHwcNodeBelowSelf(std::vector<RectI>& hwcRects,
        std::shared_ptr<RSSurfaceRenderNode>& hwcNode, bool isIntersectWithRoundCorner);
    void UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode,
        std::vector<RectI>& hwcRects);
    void UpdateHardwareStateByBoundNEDstRectInApps(const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes,
        std::vector<RectI>& abovedBounds);
    // Use in updating hwcnode hardware state with background alpha
    void UpdateHardwareStateByHwcNodeBackgroundAlpha(const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes,
        RectI& backgroundAlphaRect, bool& isHardwareEnableByBackgroundAlpha);
    void UpdateChildHwcNodeEnableByHwcNodeBelow(std::vector<RectI>& hwcRects,
        std::shared_ptr<RSSurfaceRenderNode>& appNode);
    void UpdateTransparentHwcNodeEnable(const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes);
    void CalcHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node,
        RSRenderNode& filterNode, int32_t filterZOrder = 0);
    void UpdateHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node,
        RSRenderNode& filterNode, int32_t filterZOrder = 0);
    void UpdateHwcNodeEnableByGlobalFilter(std::shared_ptr<RSSurfaceRenderNode>& node);
    void UpdateHwcNodeEnableByGlobalCleanFilter(const std::vector<std::pair<NodeId, RectI>>& cleanFilter,
        RSSurfaceRenderNode& hwcNode);
    void UpdateHwcNodeEnableByGlobalDirtyFilter(const std::vector<std::pair<NodeId, RectI>>& dirtyFilter,
        RSSurfaceRenderNode& hwcNode);
    void UpdateHwcNodeRectInSkippedSubTree(const RSRenderNode& rootNode);
    void UpdatePrepareClip(RSRenderNode& node);
    void UpdateTopSurfaceSrcRect(RSSurfaceRenderNode& node,
        const Drawing::Matrix& absMatrix, const RectI& absRect);

    bool IsDisableHwcOnExpandScreen() const;

    void UpdateHwcNodeInfo(RSSurfaceRenderNode& node, const Drawing::Matrix& absMatrix, const RectI& absRect,
        bool subTreeSkipped = false);
    void QuickPrepareChildrenOnlyOrder(RSRenderNode& node);
    void PrintHiperfCounterLog(const char* const counterContext, uint64_t counter);
    void PrintHiperfLog(RSSurfaceRenderNode* node, const char* const disabledContext);
    void PrintHiperfLog(const std::shared_ptr<RSSurfaceRenderNode>& node, const char* const disabledContext);

    // DRM
    void UpdateCrossInfoForProtectedHwcNode(RSSurfaceRenderNode& hwcNode);

    // DFX
    HwcDisabledReasonCollection& Statistics() { return hwcDisabledReasonCollection_; }

    void IncreaseSolidLayerHwcEnableCount() { solidLayerHwcEnableCount_++; }
    size_t GetSolidLayerHwcEnableCount() const { return solidLayerHwcEnableCount_; }

private:
    friend class RSUniRenderVisitor;
    RSUniRenderVisitor& uniRenderVisitor_;

    // Functions
    bool FindRootAndUpdateMatrix(std::shared_ptr<RSRenderNode>& parent, Drawing::Matrix& matrix,
        const RSRenderNode& rootNode);
    void UpdateHwcNodeClipRect(const std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr,
        const RSRenderNode& rootNode, RectI& clipRect);

    // indicates if hardware composer is totally disabled
    bool isHardwareForcedDisabled_ = false;

    // record nodes which has transparent clean filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentHwcCleanFilter_;
    // record nodes which has transparent dirty filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentHwcDirtyFilter_;

    int32_t curZOrderForHwcEnableByFilter_ = 0;

    size_t solidLayerHwcEnableCount_ = 0;

    bool isOffscreen_ = false;

    // use for hardware compose disabled reason collection
    HwcDisabledReasonCollection& hwcDisabledReasonCollection_ = HwcDisabledReasonCollection::GetInstance();
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_UNI_HWC_VISITOR_H