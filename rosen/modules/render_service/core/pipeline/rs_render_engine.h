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

#ifndef RS_CORE_PIPELINE_RENDER_ENGINE_H
#define RS_CORE_PIPELINE_RENDER_ENGINE_H

#include <memory>

#include "hdi_layer_info.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/drawing/rs_surface_frame.h"
#include "platform/ohos/rs_surface_ohos.h"
#include "screen_manager/rs_screen_manager.h"

#ifdef RS_ENABLE_GL
#include "render_context/render_context.h"
#endif // RS_ENABLE_GL

#ifdef RS_ENABLE_EGLIMAGE
#include "rs_egl_image_manager.h"
#endif // RS_ENABLE_EGLIMAGE

namespace OHOS {
namespace Rosen {
struct DrawSurfaceNodeInfo {
    bool forceCPU = false;
    float mirrorAdaptiveCoefficient = 1.0f;
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
        return std::make_unique<RSPaintFilterCanvas>(surfaceFrame_->GetCanvas());
    }

private:
    std::shared_ptr<RSSurfaceOhos> targetSurface_;
    std::unique_ptr<RSSurfaceFrame> surfaceFrame_;
};

// This render engine aims to do the client composition for all surfaces that hardware can't handle.
class RSRenderEngine {
public:
    RSRenderEngine();
    ~RSRenderEngine() noexcept;

    RSRenderEngine(const RSRenderEngine&) = delete;
    void operator=(const RSRenderEngine&) = delete;

    // [PLANNING]: this is a work-around for the lack of colorgamut convertion and yuv support in GPU.
    // We should remove this function in someday.
    static bool NeedForceCPU(const std::vector<LayerInfoPtr>& layers);

    // There would only one user(thread) to renderFrame(request frame) at one time.
    std::unique_ptr<RSRenderFrame> RequestFrame(
        const sptr<Surface>& targetSurface,
        const BufferRequestConfig& config,
        bool forceCPU = false);

    void DrawLayers(
        RSPaintFilterCanvas& canvas,
        const std::vector<LayerInfoPtr>& layers,
        const ScreenInfo& screenInfo,
        bool forceCPU = false,
        float mirrorAdaptiveCoefficient = 1.0f);
    void DrawSurfaceNode(
        RSPaintFilterCanvas& canvas,
        RSSurfaceRenderNode& node,
        const ScreenInfo& screenInfo,
        const IRect& clipRect,
        DrawSurfaceNodeInfo& infos);

    void ClipHoleForLayer(
        RSPaintFilterCanvas& canvas,
        RSSurfaceRenderNode& node,
        const ScreenInfo& screenInfo,
        const IRect& clipRect);

#ifdef RS_ENABLE_GL
    const std::shared_ptr<RenderContext>& GetRenderContext() const
    {
        return renderContext_;
    }
#endif // RS_ENABLE_GL

#ifdef RS_ENABLE_EGLIMAGE
    const std::shared_ptr<RSEglImageManager>& GetRSEglImageManager() const
    {
        return eglImageManager_;
    }
#endif // RS_ENABLE_EGLIMAGE

    void ShrinkEGLImageCachesIfNeeded();
private:
    SkMatrix GetSurfaceNodeGravityMatrix(RSSurfaceRenderNode& node, const RectF& targetRect);
    // This func can only by called in DrawLayers().
    void DrawDisplayNode(
        RSPaintFilterCanvas& canvas,
        RSDisplayRenderNode& node,
        const ScreenInfo& screenInfo,
        bool forceCPU = false);
        
#ifdef RS_ENABLE_GL
    std::shared_ptr<RenderContext> renderContext_;
#endif // RS_ENABLE_GL

#ifdef RS_ENABLE_EGLIMAGE
    std::shared_ptr<RSEglImageManager> eglImageManager_;
#endif // RS_ENABLE_EGLIMAGE
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_RENDER_ENGINE_H
