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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_RENDER_THREAD_VISITOR_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_RENDER_THREAD_VISITOR_H

#include <memory>
#include <map>
#include <set>

#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "transaction/rs_transaction_proxy.h"
#include "visitor/rs_node_visitor.h"

#ifdef USE_ROSEN_DRAWING
#include "draw/brush.h"
#include "draw/color.h"
#include "draw/pen.h"
#endif

namespace OHOS {
namespace Rosen {
class RSDirtyRegionManager;
class RSProperties;
class RSRenderNode;

class RSRenderThreadVisitor : public RSNodeVisitor {
public:
    RSRenderThreadVisitor();
    virtual ~RSRenderThreadVisitor();

    void PrepareChildren(RSRenderNode& node) override;
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override;
    void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {}
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
    void PrepareRootRenderNode(RSRootRenderNode& node) override;
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void PrepareEffectRenderNode(RSEffectRenderNode& node) override;

    void ProcessChildren(RSRenderNode& node) override;
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
    void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override {}
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override;
    void ProcessRootRenderNode(RSRootRenderNode& node) override;
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    void ProcessEffectRenderNode(RSEffectRenderNode& node) override;

    void AddSurfaceChangedCallBack(uint64_t id,
        const std::function<void(float, float, float, float)>& callback) override;
    void RemoveSurfaceChangedCallBack(uint64_t id) override;

    // Partial render status and renderForce flag should be updated by rt thread
    void SetPartialRenderStatus(PartialRenderType status, bool isRenderForced);
private:
#ifndef USE_ROSEN_DRAWING
    void DrawRectOnCanvas(const RectI& dirtyRect, const SkColor color, const SkPaint::Style fillType, float alpha,
        int strokeWidth = 6);
#else
    enum class RSPaintStyle {
        FILL,
        STROKE
    };
    void DrawRectOnCanvas(const RectI& dirtyRect, const Drawing::ColorQuad color, RSPaintStyle fillType, float alpha,
        int strokeWidth = 6);
#endif // USE_ROSEN_DRAWING
    void DrawDirtyRegion();
    // Update damageRegion based on buffer age, and then set it through egl api
#ifdef NEW_RENDER_CONTEXT
    void UpdateDirtyAndSetEGLDamageRegion(std::shared_ptr<RSRenderSurface>& surface);
#else
    void UpdateDirtyAndSetEGLDamageRegion(std::unique_ptr<RSSurfaceFrame>& surfaceFrame);
#endif
    // Reset and update children node's info like outOfParent and isRemoveChild
    void ResetAndPrepareChildrenNode(RSRenderNode& node, std::shared_ptr<RSBaseRenderNode> nodeParent);
    void ProcessSurfaceViewInRT(RSSurfaceRenderNode& node);
    int CacRotationFromTransformType(GraphicTransformType transform);

    bool UpdateAnimatePropertyCacheSurface(RSRenderNode& node);
    void ProcessShadowFirst(RSRenderNode& node);

    std::shared_ptr<RSDirtyRegionManager> curDirtyManager_;
    bool isRenderForced_ = false;
    bool isEglSetDamageRegion_ = false;
    bool isOpDropped_ = false;
    DirtyRegionDebugType dfxDirtyType_ = DirtyRegionDebugType::DISABLED;
    PartialRenderType partialRenderStatus_ = PartialRenderType::DISABLED;
    RectI curDirtyRegion_ = RectI();
    bool dirtyFlag_ = false;
    bool isIdle_ = true;
    std::shared_ptr<RSPaintFilterCanvas> canvas_ = nullptr;
    uint32_t queueSize_ = 0;
    uint64_t uiTimestamp_ = 0;

    void ClipHoleForSurfaceNode(RSSurfaceRenderNode& node);

    std::vector<NodeId> childSurfaceNodeIds_;
#ifndef USE_ROSEN_DRAWING
    SkMatrix parentSurfaceNodeMatrix_;
    std::optional<SkIRect> effectRegion_ = std::nullopt;
    std::vector<std::shared_ptr<DrawCmdList>> drawCmdListVector_;
#else
    Drawing::Matrix parentSurfaceNodeMatrix_;
    std::optional<Drawing::RectI> effectRegion_ = std::nullopt;
    std::vector<std::shared_ptr<Drawing::DrawCmdList>> drawCmdListVector_;
#endif // USE_ROSEN_DRAWING

    std::map<NodeId, std::function<void(float, float, float, float)>> surfaceCallbacks_;

    static void SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType);
    static bool IsValidRootRenderNode(RSRootRenderNode& node);
private:
    void ProcessTextureSurfaceRenderNode(RSSurfaceRenderNode& node);
    void ProcessOtherSurfaceRenderNode(RSSurfaceRenderNode& node);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_RENDER_THREAD_VISITOR_H
