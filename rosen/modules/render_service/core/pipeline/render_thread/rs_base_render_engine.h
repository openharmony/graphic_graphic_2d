/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RS_CORE_PIPELINE_BASE_RENDER_ENGINE_H
#define RS_CORE_PIPELINE_BASE_RENDER_ENGINE_H

#include <memory>

#include "hdi_layer_info.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#ifdef RS_ENABLE_VK
#include "feature/gpuComposition/rs_vk_image_manager.h"
#endif
#include "include/gpu/GrDirectContext.h"
#include "rs_base_render_util.h"

#include "platform/drawing/rs_surface_frame.h"
#include "platform/ohos/rs_surface_ohos.h"
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
#include "render_context/render_context.h"
#endif // RS_ENABLE_GL || RS_ENABLE_VK
#if (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_GPU))
#include "feature/gpuComposition/rs_egl_image_manager.h"
#endif // RS_ENABLE_EGLIMAGE
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "colorspace_converter_display.h"
#endif

namespace OHOS {
namespace Rosen {
namespace DrawableV2 {
class RSDisplayRenderNodeDrawable;
class RSSurfaceRenderNodeDrawable;
}
struct FrameContextConfig {
public:
    FrameContextConfig(bool isProtected, bool independentContext)
    {
        this->isProtected = isProtected;
        this->independentContext = independentContext;
    }
    bool isProtected = false;
    bool independentContext = false;
    bool isVirtual = false;
    int32_t timeOut = 3000; // ms
};
// The RenderFrame can do auto flush
class RSRenderFrame {
public:
    // we guarantee when constructing this object, all parameters are valid.
    RSRenderFrame(const std::shared_ptr<RSSurfaceOhos>& target, std::unique_ptr<RSSurfaceFrame>&& frame)
        : targetSurface_(target), surfaceFrame_(std::move(frame))
    {
    }
    ~RSRenderFrame() noexcept
    {
        Flush();
    }

    // noncopyable
    RSRenderFrame(const RSRenderFrame&) = delete;
    void operator=(const RSRenderFrame&) = delete;
    void Flush() noexcept
    {
        if (targetSurface_ != nullptr && surfaceFrame_ != nullptr) {
            targetSurface_->FlushFrame(surfaceFrame_);
            targetSurface_ = nullptr;
            surfaceFrame_ = nullptr;
        }
    }
    const std::shared_ptr<RSSurfaceOhos>& GetSurface() const
    {
        return targetSurface_;
    }
    const std::unique_ptr<RSSurfaceFrame>& GetFrame() const
    {
        return surfaceFrame_;
    }
    std::unique_ptr<RSPaintFilterCanvas> GetCanvas()
    {
        return std::make_unique<RSPaintFilterCanvas>(surfaceFrame_->GetSurface().get());
    }

    int32_t GetBufferAge()
    {
        return surfaceFrame_ != nullptr ? surfaceFrame_->GetBufferAge() : 0;
    }

    void SetDamageRegion(const std::vector<RectI> &rects)
    {
        if (surfaceFrame_ != nullptr) {
            surfaceFrame_->SetDamageRegion(rects);
        }
    }
private:
    std::shared_ptr<RSSurfaceOhos> targetSurface_;
    std::unique_ptr<RSSurfaceFrame> surfaceFrame_;
};

// function that will be called before drawing Buffer / Image.
using PreProcessFunc = std::function<void(RSPaintFilterCanvas&, BufferDrawParam&)>;
// function that will be called after drawing Buffer / Image.
using PostProcessFunc = std::function<void(RSPaintFilterCanvas&, BufferDrawParam&)>;

struct VideoInfo {
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace_ = nullptr;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    GSError retGetColorSpaceInfo_ = GSERROR_OK;
    Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter parameter_ = {};
#endif
};

// This render engine aims to do the client composition for all surfaces that hardware can't handle.
class RSBaseRenderEngine {
public:
    RSBaseRenderEngine();
    virtual ~RSBaseRenderEngine() noexcept;
    void Init(bool independentContext = false);
    RSBaseRenderEngine(const RSBaseRenderEngine&) = delete;
    void operator=(const RSBaseRenderEngine&) = delete;

    // [PLANNING]: this is a work-around for the lack of colorgamut conversion and yuv support in GPU.
    // We should remove this function in someday.
    static bool NeedForceCPU(const std::vector<LayerInfoPtr>& layers);

    // There would only one user(thread) to renderFrame(request frame) at one time.
    // for framebuffer surface
    std::unique_ptr<RSRenderFrame> RequestFrame(const sptr<Surface>& targetSurface,
        const BufferRequestConfig& config, bool forceCPU = false, bool useAFBC = true,
        const FrameContextConfig& frameContextConfig = {false, false});

    // There would only one user(thread) to renderFrame(request frame) at one time.
    std::unique_ptr<RSRenderFrame> RequestFrame(const std::shared_ptr<RSSurfaceOhos>& rsSurface,
        const BufferRequestConfig& config, bool forceCPU = false, bool useAFBC = true,
        const FrameContextConfig& frameContextConfig = {false, false});
    std::shared_ptr<RSSurfaceOhos> MakeRSSurface(const sptr<Surface>& targetSurface, bool forceCPU);
    static void SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame,
        std::shared_ptr<RSSurfaceOhos> surfaceOhos);

    virtual void DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node,
        BufferDrawParam& params, PreProcessFunc preProcess = nullptr, PostProcessFunc postProcess = nullptr) = 0;
    virtual void DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas,
        DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, BufferDrawParam& params,
        PreProcessFunc preProcess = nullptr, PostProcessFunc postProcess = nullptr) {}

    void DrawDisplayNodeWithParams(RSPaintFilterCanvas& canvas, RSDisplayRenderNode& node,
        BufferDrawParam& params);
    void DrawDisplayNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceHandler& surfaceHandler,
        BufferDrawParam& params);
    void RegisterDeleteBufferListener(const sptr<IConsumerSurface>& consumer, bool isForUniRedraw = false);
    void RegisterDeleteBufferListener(RSSurfaceHandler& handler);
    std::shared_ptr<Drawing::Image> CreateImageFromBuffer(RSPaintFilterCanvas& canvas,
        BufferDrawParam& params, VideoInfo& videoInfo);
#ifdef USE_VIDEO_PROCESSING_ENGINE
    virtual void DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU = false,
        const ScreenInfo& screenInfo = {}, GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB) = 0;
#else
    virtual void DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU = false,
        const ScreenInfo& screenInfo = {}) = 0;
#endif

    static void DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& params);

    void ShrinkCachesIfNeeded(bool isForUniRedraw = false);
    void ClearCacheSet(const std::set<uint32_t>& unmappedCache);
    static void SetColorFilterMode(ColorFilterMode mode);
    static ColorFilterMode GetColorFilterMode();
    static void SetHighContrast(bool enabled);
    static bool IsHighContrastEnabled();

#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    const std::shared_ptr<RenderContext>& GetRenderContext()
    {
        return renderContext_;
    }
#endif // RS_ENABLE_GL || RS_ENABLE_VK
    void ResetCurrentContext();

#if (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_GPU))
    const std::shared_ptr<RSEglImageManager>& GetEglImageManager()
    {
        return eglImageManager_;
    }
#endif // RS_ENABLE_EGLIMAGE
#ifdef USE_VIDEO_PROCESSING_ENGINE
    void ColorSpaceConvertor(std::shared_ptr<Drawing::ShaderEffect> &inputShader, BufferDrawParam& params,
        Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter& parameter);
#endif
    static std::shared_ptr<Drawing::ColorSpace> ConvertColorGamutToDrawingColorSpace(GraphicColorGamut colorGamut);
    static std::shared_ptr<Drawing::ColorSpace> ConvertColorSpaceNameToDrawingColorSpace(
        OHOS::ColorManager::ColorSpaceName colorSpaceName);
#ifdef RS_ENABLE_VK
    const std::shared_ptr<RSVkImageManager>& GetVkImageManager() const
    {
        return vkImageManager_;
    }
    const std::shared_ptr<Drawing::GPUContext> GetSkContext() const
    {
        return skContext_;
    }
    const std::shared_ptr<Drawing::GPUContext> GetCaptureSkContext() const
    {
        return captureSkContext_;
    }
#endif
    void DumpVkImageInfo(std::string &dumpString);
protected:
    void DrawImage(RSPaintFilterCanvas& canvas, BufferDrawParam& params);

    static inline std::mutex colorFilterMutex_;
    static inline ColorFilterMode colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;
    static inline std::atomic_bool isHighContrastEnabled_ = false;

private:
    std::shared_ptr<Drawing::Image> CreateEglImageFromBuffer(RSPaintFilterCanvas& canvas,
        const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
        const uint32_t threadIndex = UNI_MAIN_THREAD_INDEX,
        const std::shared_ptr<Drawing::ColorSpace>& drawingColorSpace = nullptr);

    static void DrawImageRect(RSPaintFilterCanvas& canvas, std::shared_ptr<Drawing::Image> image,
        BufferDrawParam& params, Drawing::SamplingOptions& samplingOptions);

    static bool NeedBilinearInterpolation(const BufferDrawParam& params, const Drawing::Matrix& matrix);

    static std::shared_ptr<Drawing::ColorSpace> GetCanvasColorSpace(const RSPaintFilterCanvas& canvas);

#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    std::shared_ptr<RenderContext> renderContext_ = nullptr;
#endif // RS_ENABLE_GL || RS_ENABLE_VK
#if (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_GPU))
    std::shared_ptr<RSEglImageManager> eglImageManager_ = nullptr;
#endif // RS_ENABLE_EGLIMAGE
#ifdef RS_ENABLE_VK
    std::shared_ptr<Drawing::GPUContext> skContext_ = nullptr;
    std::shared_ptr<Drawing::GPUContext> captureSkContext_ = nullptr;
    std::shared_ptr<RSVkImageManager> vkImageManager_ = nullptr;
#endif
    using SurfaceId = uint64_t;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    static bool SetColorSpaceConverterDisplayParameter(
        const BufferDrawParam& params, Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter& parameter);
    static bool ConvertDrawingColorSpaceToSpaceInfo(const std::shared_ptr<Drawing::ColorSpace>& colorSpace,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo);
    std::shared_ptr<Media::VideoProcessingEngine::ColorSpaceConverterDisplay> colorSpaceConverterDisplay_ = nullptr;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_BASE_RENDER_ENGINE_H
