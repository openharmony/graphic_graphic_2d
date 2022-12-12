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
#include "platform/drawing/rs_surface_frame.h"
#include "platform/ohos/rs_surface_ohos.h"
#include "rs_base_render_util.h"

#ifdef RS_ENABLE_GL
#include "render_context/render_context.h"
#endif // RS_ENABLE_GL

#ifdef RS_ENABLE_EGLIMAGE
#include "rs_egl_image_manager.h"
#endif // RS_ENABLE_EGLIMAGE

namespace OHOS {
namespace Rosen {
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

// This render engine aims to do the client composition for all surfaces that hardware can't handle.
class RSBaseRenderEngine {
public:
    RSBaseRenderEngine();
    virtual ~RSBaseRenderEngine() noexcept;
    static void Init();
    RSBaseRenderEngine(const RSBaseRenderEngine&) = delete;
    void operator=(const RSBaseRenderEngine&) = delete;

    // [PLANNING]: this is a work-around for the lack of colorgamut conversion and yuv support in GPU.
    // We should remove this function in someday.
    static bool NeedForceCPU(const std::vector<LayerInfoPtr>& layers);

    // There would only one user(thread) to renderFrame(request frame) at one time.
    // for framebuffer surface
    static std::unique_ptr<RSRenderFrame> RequestFrame(const sptr<Surface>& rsSurface,
        const BufferRequestConfig& config, bool forceCPU = false, bool useAFBC = true);
    // There would only one user(thread) to renderFrame(request frame) at one time.
    static std::unique_ptr<RSRenderFrame> RequestFrame(const std::shared_ptr<RSSurfaceOhos>& rsSurface,
        const BufferRequestConfig& config, bool forceCPU = false, bool useAFBC = true);

    static void SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame, const uint64_t surfaceId);

    virtual void DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node,
        BufferDrawParam& params, PreProcessFunc preProcess = nullptr, PostProcessFunc postProcess = nullptr) = 0;

    static void DrawDisplayNodeWithParams(RSPaintFilterCanvas& canvas, RSDisplayRenderNode& node,
        BufferDrawParam& params);

    virtual void DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU = false,
        float mirrorAdaptiveCoefficient = 1.0f) = 0;

    static void ShrinkCachesIfNeeded();
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
#ifdef RS_ENABLE_GL
    static const std::shared_ptr<RenderContext>& GetRenderContext()
    {
        return renderContext_;
    }
#endif // RS_ENABLE_GL
protected:
    static void RegisterDeleteBufferListener(const sptr<Surface>& consumer);
    static void DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& params);
    static void DrawImage(RSPaintFilterCanvas& canvas, BufferDrawParam& params);

    static inline ColorFilterMode colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;

private:
    static sk_sp<SkImage> CreateEglImageFromBuffer(const sptr<SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence);

    static inline std::atomic_bool isHighContrastEnabled_ = false;

#ifdef RS_ENABLE_GL
    static inline std::shared_ptr<RenderContext> renderContext_ = nullptr;
#endif // RS_ENABLE_GL

#ifdef RS_ENABLE_EGLIMAGE
    static inline std::shared_ptr<RSEglImageManager> eglImageManager_ = nullptr;
#endif // RS_ENABLE_EGLIMAGE

    // RSSurfaces for framebuffer surfaces.
    static constexpr size_t MAX_RS_SURFACE_SIZE = 32; // used for rsSurfaces_.
    using SurfaceId = uint64_t;
    static inline std::unordered_map<SurfaceId, std::shared_ptr<RSSurfaceOhos>> rsSurfaces_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_BASE_RENDER_ENGINE_H
