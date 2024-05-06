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
    void SwitchColorFilter(RSPaintFilterCanvas& canvas) const;
    void SetHighContrastIfEnabled(RSPaintFilterCanvas& canvas) const;

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
    void DrawWatermarkIfNeed(RSDisplayRenderNode& node, RSPaintFilterCanvas& canvas) const;
    void DrawMirrorScreen(RSDisplayRenderNode& displayNodeSp, RSDisplayRenderParams& params,
        std::shared_ptr<RSProcessor> processor);
    using DrawFuncPtr = void(RSDisplayRenderNodeDrawable::*)(Drawing::Canvas&);
    void DrawMirror(RSDisplayRenderNode& displayNodeSp, RSDisplayRenderParams& params,
        std::shared_ptr<RSProcessor> processor, DrawFuncPtr drawFunc, RSRenderThreadParams& uniParam);
    void DrawExpandScreen(RSUniRenderVirtualProcessor& processor);
    void SetVirtualScreenType(RSDisplayRenderNode& node, const ScreenInfo& screenInfo);
    void ScaleMirrorIfNeed(RSDisplayRenderNode& node, std::shared_ptr<RSProcessor> processor);
    void RotateMirrorCanvasIfNeed(RSDisplayRenderNode& node);
    void DrawCurtainScreen(RSDisplayRenderNode& node, RSPaintFilterCanvas& canvas) const;
    void RemoveClearMemoryTask() const;
    void PostClearMemoryTask() const;
    std::shared_ptr<Drawing::Image> GetCacheImageFromMirrorNode(
        std::shared_ptr<RSDisplayRenderNode> mirrorNode);
    void ResetRotateIfNeed(RSDisplayRenderNode& mirroredNode, RSUniRenderVirtualProcessor& mirroredProcessor,
        Drawing::Region& clipRegion);
    void ProcessCacheImage(Drawing::Image& cacheImageProcessed);
    void SetCanvasBlack(RSProcessor& processor);
    // Prepare for off-screen render
    void PrepareOffscreenRender(const RSRenderNode& node);
    void FinishOffscreenRender(const Drawing::SamplingOptions& sampling);

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::DISPLAY_NODE, OnGenerate>;
    static Registrar instance_;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_;
    std::shared_ptr<Drawing::Surface> offscreenSurface_; // temporary holds offscreen surface
    std::shared_ptr<RSPaintFilterCanvas> canvasBackup_; // backup current canvas before offscreen rende
    bool canvasRotation_ = false;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H