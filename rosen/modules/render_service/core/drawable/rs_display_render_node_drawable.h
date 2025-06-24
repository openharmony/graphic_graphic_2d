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
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/slr_scale/rs_slr_scale.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSDisplayRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSDisplayRenderNodeDrawable() override;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;
    void DrawHardwareEnabledNodes(Drawing::Canvas& canvas, RSDisplayRenderParams& params);
    void DrawHardwareEnabledNodes(Drawing::Canvas& canvas);
    void DrawHardwareEnabledNodesMissedInCacheImage(Drawing::Canvas& canvas);
    void DrawHardwareEnabledTopNodesMissedInCacheImage(Drawing::Canvas& canvas);
    void SwitchColorFilter(RSPaintFilterCanvas& canvas, float hdrBrightnessRatio = 1.f,
        bool displayP3Enable = false) const;
    void SwitchColorFilterWithP3(RSPaintFilterCanvas& canvas, ColorFilterMode colorFilterMode,
        float hdrBrightnessRatio = 1.f) const;
    void DrawHardCursorNodesMissedInCacheImage(Drawing::Canvas& canvas);

    std::shared_ptr<Drawing::Image> GetCacheImgForCapture() const
    {
        return cacheImgForCapture_;
    }

    void SetCacheImgForCapture(std::shared_ptr<Drawing::Image> cacheImgForCapture)
    {
        cacheImgForCapture_ = cacheImgForCapture;
    }

    std::shared_ptr<Drawing::Image> GetCacheImgForMultiScreenView() const
    {
        return cacheImgForMultiScreenView_;
    }

    const std::shared_ptr<RSSurfaceHandler> GetRSSurfaceHandlerOnDraw() const
    {
        return surfaceHandler_;
    }

    std::shared_ptr<RSSurfaceHandler> GetMutableRSSurfaceHandlerOnDraw()
    {
        return surfaceHandler_;
    }

    const std::vector<RectI>& GetDirtyRects() const
    {
        return dirtyRects_;
    }

    void SetDirtyRects(const std::vector<RectI>& rects)
    {
        dirtyRects_ = rects;
    }

    std::shared_ptr<RSDirtyRegionManager> GetSyncDirtyManager() const override
    {
        return syncDirtyManager_;
    }

#ifndef ROSEN_CROSS_PLATFORM
    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    sptr<IBufferConsumerListener> GetConsumerListener() const
    {
        return consumerListener_;
    }
#endif
    bool IsSurfaceCreated() const
    {
        return surfaceCreated_;
    }
    std::shared_ptr<RSSurface> GetRSSurface() const
    {
        return surface_;
    }
    void SetVirtualSurface(std::shared_ptr<RSSurface>& virtualSurface, uint64_t pSurfaceUniqueId)
    {
        virtualSurface_ = virtualSurface;
        virtualSurfaceUniqueId_ = pSurfaceUniqueId;
    }
    std::shared_ptr<RSSurface> GetVirtualSurface(uint64_t pSurfaceUniqueId)
    {
        return virtualSurfaceUniqueId_ != pSurfaceUniqueId ? nullptr : virtualSurface_;
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
    bool SkipFrame(uint32_t refreshRate, ScreenInfo screenInfo);
    int32_t GetSpecialLayerType(RSDisplayRenderParams& params, bool isSecLayerInVisivleRect = true);

private:
    explicit RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    bool CheckDisplayNodeSkip(RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor);
    void ClearCanvasStencil(RSPaintFilterCanvas& canvas, RSDisplayRenderParams& params,
        RSRenderThreadParams& uniParam);
    std::unique_ptr<RSRenderFrame> RequestFrame(RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor);
    void FindHardwareEnabledNodes(RSDisplayRenderParams& params);
    void AdjustZOrderAndDrawSurfaceNode(std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& drawables,
        Drawing::Canvas& canvas, RSDisplayRenderParams& params) const;
    void WiredScreenProjection(RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor);
    void ScaleAndRotateMirrorForWiredScreen(RSDisplayRenderNodeDrawable& mirroredDrawable);
    void DrawWiredMirrorCopy(RSDisplayRenderNodeDrawable& mirroredDrawable);
    void DrawWiredMirrorOnDraw(RSDisplayRenderNodeDrawable& mirroredDrawable, RSDisplayRenderParams& params);
    std::vector<RectI> CalculateVirtualDirtyForWiredScreen(
        std::unique_ptr<RSRenderFrame>& renderFrame, RSDisplayRenderParams& params, Drawing::Matrix canvasMatrix);
    void DrawWatermarkIfNeed(RSDisplayRenderParams& params, RSPaintFilterCanvas& canvas) const;
    void RotateMirrorCanvas(ScreenRotation& rotation, float width, float height);

    void DrawMirrorScreen(RSDisplayRenderParams& params,
        std::shared_ptr<RSProcessor> processor);
    std::vector<RectI> CalculateVirtualDirty(std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser,
        RSDisplayRenderParams& params, Drawing::Matrix canvasMatrix);
    using DrawFuncPtr = void(RSDisplayRenderNodeDrawable::*)(Drawing::Canvas&);
    void DrawMirror(RSDisplayRenderParams& params, std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser,
        DrawFuncPtr drawFunc, RSRenderThreadParams& uniParam);
    void DrawMirrorCopy(RSDisplayRenderNodeDrawable& mirrorDrawable, RSDisplayRenderParams& params,
        std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSRenderThreadParams& uniParam);
    void DrawExpandScreen(RSDisplayRenderParams& params, RSUniRenderVirtualProcessor& processor);
    void DrawCurtainScreen() const;
    void InitTranslateForWallpaper();
    void CalculateTranslationForWallpaper();
    void RemoveClearMemoryTask() const;
    void PostClearMemoryTask() const;
    void SetCanvasBlack(RSProcessor& processor);
    void SetSecurityMask(RSProcessor& processor);
    void SetScreenRotationForPointLight(RSDisplayRenderParams &params);
    // Prepare for off-screen render
    void UpdateSlrScale(ScreenInfo& screenInfo);
    void ScaleCanvasIfNeeded(const ScreenInfo& screenInfo);
    void PrepareOffscreenRender(const RSDisplayRenderNodeDrawable& displayDrawable,
        bool useFixedSize = false, bool useCanvasSize = true);
    void ClearTransparentBeforeSaveLayer();
    static std::shared_ptr<Drawing::ShaderEffect> MakeBrightnessAdjustmentShader(
        const std::shared_ptr<Drawing::Image>& image, const Drawing::SamplingOptions& sampling,
        float hdrBrightnessRatio);
    void FinishOffscreenRender(const Drawing::SamplingOptions& sampling,
        bool isSamplingOn = false, float hdrBrightnessRatio = 1.0f);
    void PrepareHdrDraw(int32_t offscreenWidth, int32_t offscreenHeight);
    void FinishHdrDraw(Drawing::Brush& paint, float hdrBrightnessRatio);
    void SetDisplayNodeSkipFlag(RSRenderThreadParams& uniParam, bool flag);
    void UpdateDisplayDirtyManager(int32_t bufferage, bool useAlignedDirtyRegion = false);
    static void CheckFilterCacheFullyCovered(RSSurfaceRenderParams& surfaceParams, RectI screenRect);
    static void CheckAndUpdateFilterCacheOcclusion(RSDisplayRenderParams& params, ScreenInfo& screenInfo);
    bool HardCursorCreateLayer(std::shared_ptr<RSProcessor> processor);
    void FindHardCursorNodes(RSDisplayRenderParams& params);
    // For P3-scRGB Control
    bool EnablescRGBForP3AndUiFirst(const GraphicColorGamut& currentGamut);
    void RenderOverDraw();
    bool SkipFrameByInterval(uint32_t refreshRate, uint32_t skipFrameInterval);
    bool SkipFrameByRefreshRate(uint32_t refreshRate, uint32_t expectedRefreshRate);

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::DISPLAY_NODE, OnGenerate>;
    static Registrar instance_;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_ = nullptr;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_ = nullptr;
    std::shared_ptr<Drawing::Surface> offscreenSurface_ = nullptr; // temporarily holds offscreen surface
    std::shared_ptr<RSPaintFilterCanvas> canvasBackup_ = nullptr; // backup current canvas before offscreen render
    std::unordered_set<NodeId> currentBlackList_ = {};
    std::unordered_set<NodeId> lastBlackList_ = {};
    bool curSecExemption_ = false;
    bool lastSecExemption_ = false;
    std::shared_ptr<Drawing::Image> cacheImgForCapture_ = nullptr;
    std::shared_ptr<Drawing::Image> cacheImgForMultiScreenView_ = nullptr;
    int32_t specialLayerType_ = 0;
    bool castScreenEnableSkipWindow_ = false;
    bool isDisplayNodeSkip_ = false;
    bool isDisplayNodeSkipStatusChanged_ = false;
    Drawing::Matrix lastMatrix_;
    Drawing::Matrix lastMirrorMatrix_;
    bool useFixedOffscreenSurfaceSize_ = false;
    std::shared_ptr<RSDisplayRenderNodeDrawable> mirrorSourceDrawable_ = nullptr;
    uint64_t virtualSurfaceUniqueId_ = 0;
    bool resetRotate_ = false;
    bool isFirstTimeToProcessor_ = true;
    ScreenRotation originScreenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    // dirty manager
    std::shared_ptr<RSDirtyRegionManager> syncDirtyManager_ = nullptr;
    std::vector<RectI> dirtyRects_;

    // surface create in render thread
    static constexpr uint32_t BUFFER_SIZE = 4;
    bool surfaceCreated_ = false;
    std::shared_ptr<RSSurface> surface_ = nullptr;
    std::shared_ptr<RSSurface> virtualSurface_ = nullptr;
    std::unique_ptr<RSSLRScaleFunction> slrScale_;

    static std::shared_ptr<Drawing::RuntimeEffect> brightnessAdjustmentShaderEffect_;

    std::shared_ptr<RSSLRScaleFunction> scaleManager_ = nullptr;
    bool isMirrorSLRCopy_ = false;
#ifndef ROSEN_CROSS_PLATFORM
    sptr<IBufferConsumerListener> consumerListener_ = nullptr;
#endif
    int64_t lastRefreshTime_ = 0;
    bool virtualDirtyRefresh_ = false;
    bool enableVisibleRect_ = false;
    Drawing::RectI curVisibleRect_;
    Drawing::RectI lastVisibleRect_;
    int32_t offscreenTranslateX_ = 0;
    int32_t offscreenTranslateY_ = 0;

    bool isRenderSkipIfScreenOff_ = false;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H