/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_FEATURE_SELECTIVE_PREPARE_SELECTIVE_PREPARE_MANAGER_H
#define RENDER_SERVICE_CORE_FEATURE_SELECTIVE_PREPARE_SELECTIVE_PREPARE_MANAGER_H

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "modifier_ng/rs_modifier_ng_type.h"

namespace OHOS::Rosen {
class RSContext;
class RSDirtyRegionManager;
class RSRenderNode;
class RSScreenRenderNode;
class RSSurfaceRenderNode;

// SelectivePrepareOpt: when only a few whitelisted nodes animate (rotation-only, infinite
// loop, shallow canvas-only subtree), skip the whole-tree QuickPrepare and only prepare these
// nodes, reducing CPU noise of the prepare phase.
class RSSelectivePrepareManager {
public:
    explicit RSSelectivePrepareManager(const std::weak_ptr<RSContext>& context);
    ~RSSelectivePrepareManager() = default;

    // main pipeline: check activation conditions each frame (after CollectInfoForHardwareComposer)
    void CheckAndSetup();
    // UniRender: run fast path if active, return true when fast path is taken
    bool PrepareOptNodes();

    void SetHasGpuSurfaceDirty(bool hasGpuSurfaceDirty)
    {
        hasGpuSurfaceDirty_ = hasGpuSurfaceDirty;
    }
    void SetHasCommandInFrame(bool hasCommandInFrame)
    {
        hasCommandInFrame_ = hasCommandInFrame;
    }
    void ResetState();

    // DFX
    bool IsActive() const
    {
        return selectivePrepareOptActive_;
    }
    bool HasGpuSurfaceDirty() const
    {
        return hasGpuSurfaceDirty_;
    }
    uint32_t GetHitCount() const
    {
        return selectivePrepareOptHitCount_;
    }

private:
    // result of the single-pass ancestor traversal to logicalDisplayNode
    struct AncestorCheckResult {
        bool hasNodeGroup = false;   // an ancestor above optNode is marked as NodeGroup (blur cache)
        bool hasAlphaNotOne = false; // an intermediate ancestor above optNode has alpha != 1.0
        uint32_t traverseCount = 0;  // number of traversed nodes, for DFX trace
    };

    void SelectivePrepareFastPath(
        const std::shared_ptr<RSRenderNode>& node, const std::shared_ptr<RSSurfaceRenderNode>& hostSurfaceNode);
    // eligibility entry: surface-level O(1) checks + single ancestor traversal
    bool CheckSurfaceEligibility(const std::shared_ptr<RSRenderNode>& optNode,
        const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, bool debugEnabled);
    // R3/R4: surface-level filter/effect/PointLight aggregate flags, O(1)
    bool CheckSurfaceFilterAndLight(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, bool debugEnabled);
    // R1+R3: single traversal from optNode up to logicalDisplayNode (exclusive), checks
    // NodeGroup and intermediate alpha; skips optNode's own state
    AncestorCheckResult CheckAncestorStateToDisplay(
        const std::shared_ptr<RSRenderNode>& optNode, NodeId logicalDisplayNodeId);
    // already-active frame safety net: re-check surface alpha only, O(1)
    bool HandleAlreadyActive(const std::shared_ptr<RSRenderNode>& optNode,
        const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, bool debugEnabled);
    // infinite loop (repeatCount == -1) + subtree depth <= 2 + no branching
    bool IsSubtreeShallow(const std::shared_ptr<RSRenderNode>& node);
    // all nodes of the subtree (optNode included) are CANVAS_NODE or CANVAS_DRAWING_NODE
    bool IsCanvasOnlySubtree(const std::shared_ptr<RSRenderNode>& node);
    // all running animations only modify rotation properties (ROTATION/ROTATION_X/ROTATION_Y/QUATERNION)
    bool IsRotationOnlyAnimation(const std::shared_ptr<RSRenderNode>& node);
    // find PropertyId's RSPropertyType in TRANSFORM modifiers, check whether it is a rotation property
    bool IsRotationProperty(const std::shared_ptr<RSRenderNode>& node, PropertyId propertyId);
    // collect on-tree animating nodes and total active node count, return false when count mismatch
    bool CollectAndCheckNodes(
        std::shared_ptr<RSRenderNode>& optNode, uint32_t& onTreeAnimatingCount, uint32_t& activeNodeCount);
    // per-frame (opt node, host main/leash window surface) pairs
    using OptNodeSurfacePair = std::pair<std::shared_ptr<RSRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>;
    using OptNodeSurfacePairs = std::vector<OptNodeSurfacePair>;
    // resolve (opt node, host surface) pairs of all opt nodes and the enclosing screen node,
    // init the deduped surface/screen dirty managers; return false on any invalid state
    bool CollectNodeSurfacePairs(
        std::shared_ptr<RSScreenRenderNode>& screenNode, OptNodeSurfacePairs& nodeSurfacePairs);
    // merge each surface's current-frame dirty region into the screen dirty manager (both in
    // screen coordinates) and add host surfaces + screen to the pending sync list
    void PropagateDirtyRegions(
        const std::shared_ptr<RSScreenRenderNode>& screenNode, const OptNodeSurfacePairs& nodeSurfacePairs);

    std::weak_ptr<RSContext> context_;
    bool selectivePrepareOptActive_ = false;
    std::vector<std::weak_ptr<RSRenderNode>> selectivePrepareOptNodes_;
    uint32_t selectivePrepareOptHitCount_ = 0;
    bool hasGpuSurfaceDirty_ = false;
    bool hasCommandInFrame_ = false;
    // two-frame activation: set on the first frame O(1) conditions pass (QuickPrepare still runs and
    // fills aggregate caches), verified and activated on the next frame
    bool pendingActivation_ = false;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_CORE_FEATURE_SELECTIVE_PREPARE_SELECTIVE_PREPARE_MANAGER_H
