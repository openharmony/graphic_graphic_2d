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
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_surface_handler.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSDisplayRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    explicit RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode> &&node);
    ~RSDisplayRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;
    void SwitchColorFilter(RSPaintFilterCanvas& canvas) const;
    void SetHighContrastIfEnabled(RSPaintFilterCanvas& canvas) const;

private:
    bool CheckDisplayNodeSkip(std::shared_ptr<RSDisplayRenderNode> displayNode, RSDisplayRenderParams* params,
        std::shared_ptr<RSProcessor> processor);
    std::unique_ptr<RSRenderFrame> RequestFrame(std::shared_ptr<RSDisplayRenderNode> displayNodeSp,
        RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor) const;
    void FindHardwareEnabledNodes();
    void AdjustZOrderAndDrawSurfaceNode(
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>& nodes, Drawing::Canvas& canvas) const;
    void DrawWatermarkIfNeed(RSDisplayRenderNode& node, RSPaintFilterCanvas& canvas) const;
    void ProcessVirtualScreen(RSDisplayRenderNode& displayNodeSp, RSDisplayRenderParams& params,
        std::shared_ptr<RSProcessor> processor);
    void ScaleMirrorIfNeed(RSDisplayRenderNode& node, bool canvasRotation,
        std::shared_ptr<RSProcessor> processor);
    void RotateMirrorCanvasIfNeed(RSDisplayRenderNode& node, bool canvasRotation);
    void DrawCurtainScreen(RSDisplayRenderNode& node, RSPaintFilterCanvas& canvas) const;
    void RemoveClearMemoryTask() const;
    void PostClearMemoryTask() const;

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::DISPLAY_NODE, OnGenerate>;
    static Registrar instance_;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H
