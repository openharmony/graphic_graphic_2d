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

#ifndef RENDER_SERVICE_DRAWABLE_RS_LOGICAL_DISPLAY_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_LOGICAL_DISPLAY_RENDER_NODE_DRAWABLE_H

#include <memory>

#include "common/rs_common_def.h"

#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "params/rs_logical_display_render_params.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {
class RSScreenRenderParams;
namespace DrawableV2 {
class RSLogicalDisplayRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSLogicalDisplayRenderNodeDrawable() override;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);

    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;

    int32_t GetSpecialLayerType(RSLogicalDisplayRenderParams& params, bool isSecLayerInVisibleRect = true);
    void SetOriginScreenRotation(const ScreenRotation& rotate)
    {
        originScreenRotation_ = rotate;
        isFirstTimeToProcessor_ = false;
    }

    ScreenRotation GetOriginScreenRotation() const
    {
        return originScreenRotation_;
    }

    bool IsFirstTimeToProcessor() const
    {
        return isFirstTimeToProcessor_;
    }

protected:
    explicit RSLogicalDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);

private:
    void DrawSecurityMask();
    void CheckDirtyRefresh(CompositeType type, bool hasSecLayerInVisibleRect);
    void ScaleAndRotateMirrorForWiredScreen(RSLogicalDisplayRenderNodeDrawable& mirroredDrawable);
    void RotateMirrorCanvas(ScreenRotation& rotation, float width, float height);
    void WiredScreenProjection(RSLogicalDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor);
    void DrawMirror(RSLogicalDisplayRenderParams& params, std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser,
        RSRenderThreadParams& uniParam);
    void DrawMirrorCopy(RSLogicalDisplayRenderParams& params,
        std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSRenderThreadParams& uniParam);
    void DrawWiredMirrorCopy(
        RSLogicalDisplayRenderNodeDrawable& mirroredDrawable, RSLogicalDisplayRenderParams& params);
    void DrawWiredMirrorOnDraw(RSLogicalDisplayRenderNodeDrawable& mirroredDrawable,
        RSLogicalDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor);
    void DrawMirrorScreen(RSLogicalDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor);
    void DrawExpandDisplay(RSLogicalDisplayRenderParams& params);
    void PrepareOffscreenRender(const RSLogicalDisplayRenderNodeDrawable& displayDrawable, bool useFixedSize = false,
        bool useCanvasSize = true);
    void FinishOffscreenRender(const Drawing::SamplingOptions& sampling,
        bool isSamplingOn = false, float hdrBrightnessRatio = 1.0f);
    void UpdateSlrScale(ScreenInfo& screenInfo, RSScreenRenderParams* params = nullptr);
    void ScaleCanvasIfNeeded(const ScreenInfo& screenInfo);
    void ClearTransparentBeforeSaveLayer();
    std::vector<RectI> CalculateVirtualDirtyForWiredScreen(
        RSScreenRenderNodeDrawable& curScreenDrawable, Drawing::Matrix canvasMatrix);
    std::vector<RectI> CalculateVirtualDirty(std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser,
        RSScreenRenderNodeDrawable& curScreenDrawable, RSLogicalDisplayRenderParams& params,
        Drawing::Matrix canvasMatrix);
    void UpdateDisplayDirtyManager(std::shared_ptr<RSDirtyRegionManager> dirtyManager,
        int32_t bufferage, bool useAlignedDirtyRegion = false);
    std::shared_ptr<Drawing::ShaderEffect> MakeBrightnessAdjustmentShader(const std::shared_ptr<Drawing::Image>& image,
        const Drawing::SamplingOptions& sampling, float hdrBrightnessRatio);

    void DrawHardwareEnabledNodes(Drawing::Canvas& canvas, RSLogicalDisplayRenderParams& params);
    void DrawAdditionalContent(RSPaintFilterCanvas& canvas);
    void DrawWatermarkIfNeed(RSPaintFilterCanvas& canvas);

    void MirrorRedrawDFX(bool mirrorRedraw, ScreenId screenId);

    void SetScreenRotationForPointLight(RSLogicalDisplayRenderParams& params);

    using AncestorParams = std::pair<std::shared_ptr<RSScreenRenderNodeDrawable>, RSScreenRenderParams*>;
    AncestorParams GetScreenParams(RSRenderParams& params);
    using MirrorSourceParams = std::tuple<std::shared_ptr<RSLogicalDisplayRenderNodeDrawable>,
        RSLogicalDisplayRenderParams*, std::shared_ptr<RSScreenRenderNodeDrawable>, RSScreenRenderParams*>;
    MirrorSourceParams GetMirrorSourceParams(RSRenderParams& params);

    ScreenRotation originScreenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    bool useFixedOffscreenSurfaceSize_ = true;
    int32_t offscreenTranslateX_ = 0;
    int32_t offscreenTranslateY_ = 0;
    // temporarily holds offscreen surface
    std::shared_ptr<Drawing::Surface> offscreenSurface_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> offscreenCanvas_ = nullptr;
    RSPaintFilterCanvas* curCanvas_ = nullptr;
    // backup current canvas before offscreen render
    RSPaintFilterCanvas* canvasBackup_ = nullptr;
    std::unordered_set<NodeId> currentBlackList_ = {};
    std::unordered_set<NodeType> currentTypeBlackList_ = {};
    std::unordered_set<NodeId> lastBlackList_ = {};
    std::unordered_set<NodeType> lastTypeBlackList_ = {};

    bool curSecExemption_ = false;
    bool lastSecExemption_ = false;
    bool virtualDirtyNeedRefresh_ = false;
    bool enableVisibleRect_ = false;
    Drawing::RectI curVisibleRect_;
    Drawing::RectI lastVisibleRect_;
    std::shared_ptr<RSSLRScaleFunction> scaleManager_ = nullptr;
    bool isMirrorSLRCopy_ = false;
    bool isFirstTimeToProcessor_ = true;
    Drawing::Matrix visibleClipRectMatrix_;
    Drawing::Matrix lastCanvasMatrix_;
    Drawing::Matrix lastMirrorMatrix_;
    static std::shared_ptr<Drawing::RuntimeEffect> brightnessAdjustmentShaderEffect_;

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::LOGICAL_DISPLAY_NODE, OnGenerate>;
    static Registrar instance_;

    // mirror display drawing path dfx
    std::optional<bool> mirrorRedraw_;
};
}
}
#endif // RENDER_SERVICE_DRAWABLE_RS_LOGICAL_DISPLAY_RENDER_NODE_DRAWABLE_H