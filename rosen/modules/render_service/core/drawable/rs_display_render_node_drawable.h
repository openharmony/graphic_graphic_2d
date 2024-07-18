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

#ifndef RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H

#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_occlusion_region.h"
#include "drawable/rs_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSDisplayRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSDisplayRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;
    void DrawHardwareEnabledNodes(Drawing::Canvas& canvas,
        std::shared_ptr<RSDisplayRenderNode> displayNodeSp, RSDisplayRenderParams* params);
    void DrawHardwareEnabledNodes(Drawing::Canvas& canvas);
    void DrawHardwareEnabledNodesMissedInCacheImage(Drawing::Canvas& canvas);
    void SwitchColorFilter(RSPaintFilterCanvas& canvas) const;

    std::shared_ptr<Drawing::Image> GetCacheImgForCapture() const
    {
        return cacheImgForCapture_;
    }

    void SetCacheImgForCapture(std::shared_ptr<Drawing::Image> cacheImgForCapture)
    {
        cacheImgForCapture_ = cacheImgForCapture;
    }

    bool IsFirstTimeToProcessor() const
    {
        return isFirstTimeToProcessor_;
    }

    void SetOriginScreenRotation(const ScreenRotation& rotate)
    {
        originScreenRotation_ = rotate;
        isFirstTimeToProcessor_ = false;
    }

    ScreenRotation GetOriginScreenRotation() const
    {
        return originScreenRotation_;
    }

#ifdef NEW_RENDER_CONTEXT
    void SetVirtualSurface(std::shared_ptr<RSRenderSurface>& virtualSurface, uint64_t pSurfaceUniqueId)
    {
        virtualSurface_ = virtualSurface;
        virtualSurfaceUniqueId_ = pSurfaceUniqueId;
    }

    std::shared_ptr<RSRenderSurface> GetVirtualSurface(uint64_t pSurfaceUniqueId)
    {
        return virtualSurfaceUniqueId_ != pSurfaceUniqueId ? nullptr : virtualSurface_;
    }
#else
    void SetVirtualSurface(std::shared_ptr<RSSurface>& virtualSurface, uint64_t pSurfaceUniqueId)
    {
        virtualSurface_ = virtualSurface;
        virtualSurfaceUniqueId_ = pSurfaceUniqueId;
    }

    std::shared_ptr<RSSurface> GetVirtualSurface(uint64_t pSurfaceUniqueId)
    {
        return virtualSurfaceUniqueId_ != pSurfaceUniqueId ? nullptr : virtualSurface_;
    }
#endif

    const std::shared_ptr<RSSurfaceHandler> GetRSSurfaceHandlerOnDraw() const
    {
        return surfaceHandler_;
    }

private:
    explicit RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    bool CheckDisplayNodeSkip(std::shared_ptr<RSDisplayRenderNode> displayNode, RSDisplayRenderParams* params,
        std::shared_ptr<RSProcessor> processor);
    std::unique_ptr<RSRenderFrame> RequestFrame(std::shared_ptr<RSDisplayRenderNode> displayNodeSp,
        RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor) const;
    void FindHardwareEnabledNodes();
    void AdjustZOrderAndDrawSurfaceNode(
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>& nodes,
        Drawing::Canvas& canvas, RSDisplayRenderParams& params) const;
    void WiredScreenProjection(std::shared_ptr<RSDisplayRenderNode> displayNodeSp, RSDisplayRenderParams& params,
        std::shared_ptr<RSProcessor> processor);
    void ScaleAndRotateMirrorForWiredScreen(RSDisplayRenderNode& node, RSDisplayRenderNode& mirroredNode);
    std::vector<RectI> CalculateVirtualDirtyForWiredScreen(RSDisplayRenderNode& displayNode,
        std::unique_ptr<RSRenderFrame>& renderFrame, RSDisplayRenderParams& params, Drawing::Matrix canvasMatrix);
    void DrawWatermarkIfNeed(RSDisplayRenderParams& params, RSPaintFilterCanvas& canvas) const;
    void RotateMirrorCanvas(ScreenRotation& rotation, float mainWidth, float mainHeight);

    void DrawMirrorScreen(std::shared_ptr<RSDisplayRenderNode>& displayNode, RSDisplayRenderParams& params,
        std::shared_ptr<RSProcessor> processor);
    std::vector<RectI> CalculateVirtualDirty(RSDisplayRenderNode& displayNode,
        std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser,
        RSDisplayRenderParams& params, Drawing::Matrix canvasMatrix);
    using DrawFuncPtr = void(RSDisplayRenderNodeDrawable::*)(Drawing::Canvas&);
    void DrawMirror(std::shared_ptr<RSDisplayRenderNode>& displayNode, RSDisplayRenderParams& params,
        std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, DrawFuncPtr drawFunc,
        RSRenderThreadParams& uniParam);
    void DrawMirrorCopy(std::shared_ptr<RSDisplayRenderNode>& displayNodeSp,
        std::shared_ptr<RSDisplayRenderNode>& mirroredNode, RSDisplayRenderParams& params,
        std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSRenderThreadParams& uniParam);
    void DrawExpandScreen(RSUniRenderVirtualProcessor& processor);
    void DrawCurtainScreen() const;
    void RemoveClearMemoryTask() const;
    void PostClearMemoryTask() const;
    void ResetRotateIfNeed(RSDisplayRenderNode& mirroredNode, RSUniRenderVirtualProcessor& mirroredProcessor,
        Drawing::Region& clipRegion);
    void SetCanvasBlack(RSProcessor& processor);
    // Prepare for off-screen render
    void ClearTransparentBeforeSaveLayer();
    void PrepareOffscreenRender(const RSRenderNode& node);
    void FinishOffscreenRender(const Drawing::SamplingOptions& sampling);
    bool SkipDisplayIfScreenOff() const;
    int32_t GetSpecialLayerType(RSDisplayRenderParams& params);
    void SetDisplayNodeSkipFlag(RSRenderThreadParams& uniParam, bool flag);
    void SetHighContrastIfEnabled(RSPaintFilterCanvas& canvas) const;

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::DISPLAY_NODE, OnGenerate>;
    static Registrar instance_;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_;
    std::shared_ptr<Drawing::Surface> offscreenSurface_; // temporary holds offscreen surface
    std::shared_ptr<RSPaintFilterCanvas> canvasBackup_; // backup current canvas before offscreen rende
    std::unordered_set<NodeId> currentBlackList_;
    std::unordered_set<NodeId> lastBlackList_;
    std::shared_ptr<Drawing::Image> cacheImgForCapture_ = nullptr;
    int32_t specialLayerType_ = 0;
    bool castScreenEnableSkipWindow_ = false;
    bool isDisplayNodeSkip_ = false;
    bool isDisplayNodeSkipStatusChanged_ = false;
    Drawing::Matrix lastMatrix_;
    Drawing::Matrix lastMirrorMatrix_;
    bool useFixedOffscreenSurfaceSize_ = false;
    std::shared_ptr<RSDisplayRenderNodeDrawable> mirrorSourceDrawable_ = nullptr;
    bool isFirstTimeToProcessor_ = false;
    ScreenRotation originScreenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    uint64_t virtualSurfaceUniqueId_ = 0;
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurface> virtualSurface_ = nullptr;
#else
    std::shared_ptr<RSSurface> virtualSurface_ = nullptr;
#endif
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H