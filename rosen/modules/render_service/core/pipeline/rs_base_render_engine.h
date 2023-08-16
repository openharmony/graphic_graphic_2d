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
#include "rs_base_render_util.h"

#ifdef NEW_RENDER_CONTEXT
#include "render_backend/rs_render_surface_frame.h"
#include "ohos/rs_render_surface_ohos.h"
#include "render_context_base.h"
#else
#include "platform/drawing/rs_surface_frame.h"
#include "platform/ohos/rs_surface_ohos.h"
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
#include "render_context/render_context.h"
#endif // RS_ENABLE_GL || RS_ENABLE_VK
#endif
#ifdef RS_ENABLE_EGLIMAGE
#include "rs_egl_image_manager.h"
#endif // RS_ENABLE_EGLIMAGE

namespace OHOS {
namespace Rosen {
// The RenderFrame can do auto flush
class RSRenderFrame {
public:
    // we guarantee when constructing this object, all parameters are valid.
#ifdef NEW_RENDER_CONTEXT
    explicit RSRenderFrame(const std::shared_ptr<RSRenderSurfaceOhos>& target)
        : targetSurface_(target)
    {
    }
#else
    RSRenderFrame(const std::shared_ptr<RSSurfaceOhos>& target, std::unique_ptr<RSSurfaceFrame>&& frame)
        : targetSurface_(target), surfaceFrame_(std::move(frame))
    {
    }
#endif
    ~RSRenderFrame() noexcept
    {
        Flush();
    }

    // noncopyable
    RSRenderFrame(const RSRenderFrame&) = delete;
    void operator=(const RSRenderFrame&) = delete;
#ifdef NEW_RENDER_CONTEXT
    void Flush() noexcept
    {
        if (targetSurface_ != nullptr) {
            targetSurface_->FlushFrame();
            targetSurface_ = nullptr;
        }
    }
#else
    void Flush() noexcept
    {
        if (targetSurface_ != nullptr && surfaceFrame_ != nullptr) {
            targetSurface_->FlushFrame(surfaceFrame_);
            targetSurface_ = nullptr;
            surfaceFrame_ = nullptr;
        }
    }
#endif
#ifdef NEW_RENDER_CONTEXT
    const std::shared_ptr<RSRenderSurfaceOhos>& GetSurface() const
#else
    const std::shared_ptr<RSSurfaceOhos>& GetSurface() const
#endif
    {
        return targetSurface_;
    }
#ifndef NEW_RENDER_CONTEXT
    const std::unique_ptr<RSSurfaceFrame>& GetFrame() const
    {
        return surfaceFrame_;
    }
#endif
    std::unique_ptr<RSPaintFilterCanvas> GetCanvas()
    {
#ifdef NEW_RENDER_CONTEXT
        return std::make_unique<RSPaintFilterCanvas>(targetSurface_->GetSurface().get());
#else
        return std::make_unique<RSPaintFilterCanvas>(surfaceFrame_->GetSurface().get());
#endif
    }

    int32_t GetBufferAge()
    {
#ifdef NEW_RENDER_CONTEXT
        return targetSurface_ != nullptr ? targetSurface_->GetBufferAge() : 0;
#else
        return surfaceFrame_ != nullptr ? surfaceFrame_->GetBufferAge() : 0;
#endif
    }

    void SetDamageRegion(const std::vector<RectI> &rects)
    {
#ifdef NEW_RENDER_CONTEXT
        if (targetSurface_ != nullptr) {
            targetSurface_ ->SetDamageRegion(rects);
        }
#else
        if (surfaceFrame_ != nullptr) {
            surfaceFrame_->SetDamageRegion(rects);
        }
#endif
    }
private:
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurfaceOhos> targetSurface_;
#else
    std::shared_ptr<RSSurfaceOhos> targetSurface_;
    std::unique_ptr<RSSurfaceFrame> surfaceFrame_;
#endif
};

// function that will be called before drawing Buffer / Image.
using PreProcessFunc = std::function<void(RSPaintFilterCanvas&, BufferDrawParam&)>;
// function that will be called after drawing Buffer / Image.
using PostProcessFunc = std::function<void(RSPaintFilterCanvas&, BufferDrawParam&)>;

// This render engine aims to do the client composition for all surfaces that hardware can't handle.
class RSBaseRenderEngine {
public:
    RSBaseRenderEngine();
    virtual ~RSBaseRenderEngine() noexcept;
    void Init();
    RSBaseRenderEngine(const RSBaseRenderEngine&) = delete;
    void operator=(const RSBaseRenderEngine&) = delete;

    // [PLANNING]: this is a work-around for the lack of colorgamut conversion and yuv support in GPU.
    // We should remove this function in someday.
    static bool NeedForceCPU(const std::vector<LayerInfoPtr>& layers);

    // There would only one user(thread) to renderFrame(request frame) at one time.
    // for framebuffer surface
    std::unique_ptr<RSRenderFrame> RequestFrame(const sptr<Surface>& targetSurface,
        const BufferRequestConfig& config, bool forceCPU = false, bool useAFBC = true);

    // There would only one user(thread) to renderFrame(request frame) at one time.
#ifdef NEW_RENDER_CONTEXT
    std::unique_ptr<RSRenderFrame> RequestFrame(const std::shared_ptr<RSRenderSurfaceOhos>& rsSurface,
        const BufferRequestConfig& config, bool forceCPU = false, bool useAFBC = true);
    void SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame,
        std::shared_ptr<RSRenderSurfaceOhos> surfaceOhos);
#else
    std::unique_ptr<RSRenderFrame> RequestFrame(const std::shared_ptr<RSSurfaceOhos>& rsSurface,
        const BufferRequestConfig& config, bool forceCPU = false, bool useAFBC = true);
    void SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame,
        std::shared_ptr<RSSurfaceOhos> surfaceOhos);
#endif

    virtual void DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node,
        BufferDrawParam& params, PreProcessFunc preProcess = nullptr, PostProcessFunc postProcess = nullptr) = 0;

    void DrawDisplayNodeWithParams(RSPaintFilterCanvas& canvas, RSDisplayRenderNode& node,
        BufferDrawParam& params);

    virtual void DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU = false,
        float mirrorAdaptiveCoefficient = 1.0f) = 0;

    void DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& params);

    void ShrinkCachesIfNeeded(bool isForUniRedraw = false);
    static void SetColorFilterMode(ColorFilterMode mode);
    static ColorFilterMode GetColorFilterMode()
    {
        return colorFilterMode_;
    }
    static void SetHighContrast(bool enabled)
    {
        isHighContrastEnabled_  = enabled;
    }
    static bool IsHighContrastEnabled()
    {
        return isHighContrastEnabled_;
    }
#if defined(NEW_RENDER_CONTEXT)
    const std::shared_ptr<RenderContextBase>& GetRenderContext() const
    {
        return renderContext_;
    }
    const std::shared_ptr<DrawingContext>& GetDrawingContext() const
    {
        return drawingContext_;
    }
#else
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    const std::shared_ptr<RenderContext>& GetRenderContext()
    {
        return renderContext_;
    }
#endif // RS_ENABLE_GL || RS_ENABLE_VK
#endif
#ifdef RS_ENABLE_EGLIMAGE
    const std::shared_ptr<RSEglImageManager>& GetEglImageManager()
    {
        return eglImageManager_;
    }
#endif // RS_ENABLE_EGLIMAGE
protected:
    void RegisterDeleteBufferListener(const sptr<IConsumerSurface>& consumer, bool isForUniRedraw = false);
    void RegisterDeleteBufferListener(RSSurfaceHandler& handler);
    void DrawImage(RSPaintFilterCanvas& canvas, BufferDrawParam& params);

    static inline ColorFilterMode colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;

private:
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> CreateEglImageFromBuffer(RSPaintFilterCanvas& canvas,
        const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
        const uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);
#else
    std::shared_ptr<Drawing::Image> CreateEglImageFromBuffer(RSPaintFilterCanvas& canvas,
        const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
        const uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);
#endif

    static inline std::atomic_bool isHighContrastEnabled_ = false;
#if defined(NEW_RENDER_CONTEXT)
    std::shared_ptr<RenderContextBase> renderContext_ = nullptr;
    std::shared_ptr<DrawingContext> drawingContext_ = nullptr;
#else
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    std::shared_ptr<RenderContext> renderContext_ = nullptr;
#endif // RS_ENABLE_GL || RS_ENABLE_VK
#endif
#ifdef RS_ENABLE_EGLIMAGE
    std::shared_ptr<RSEglImageManager> eglImageManager_ = nullptr;
#endif // RS_ENABLE_EGLIMAGE
    using SurfaceId = uint64_t;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_BASE_RENDER_ENGINE_H
