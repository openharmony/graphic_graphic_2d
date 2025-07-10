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

#ifndef RENDER_SERVICE_DRAWABLE_RS_CANVAS_DRAWING_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_CANVAS_DRAWING_RENDER_NODE_DRAWABLE_H

#include "drawable/rs_render_node_drawable.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen::DrawableV2 {
using ThreadInfo = std::pair<uint32_t, std::function<void(std::shared_ptr<Drawing::Surface>)>>;
class RSCanvasDrawingRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSCanvasDrawingRenderNodeDrawable() override;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;

    void CheckAndSetThreadIdx(uint32_t& threadIdx);
    bool CheckPostplaybackParamValid(NodeId, pid_t);
    void PostPlaybackInCorrespondThread();
    void SetSurfaceClearFunc(ThreadInfo threadInfo, pid_t threadId = 0)
    {
        curThreadInfo_ = threadInfo;
        threadId_ = threadId;
    }
    bool InitSurface(int width, int height, RSPaintFilterCanvas& canvas);
    std::shared_ptr<RSPaintFilterCanvas> GetCanvas();
    void Flush(float width, float height, std::shared_ptr<RSContext> context,
        NodeId nodeId, RSPaintFilterCanvas& rscanvas);
    Drawing::Bitmap GetBitmap(Drawing::GPUContext* grContext);
    bool GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect,
        const uint64_t tid = UINT32_MAX, std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr);
    void DrawCaptureImage(RSPaintFilterCanvas& canvas);
    void ReleaseCaptureImage();
    void DumpCanvasDrawing();

    uint32_t GetTid() const
    {
        return curThreadInfo_.first;
    }
    void ResetSurface();
    virtual bool IsNeedDraw() const override
    {
        return needDraw_;
    }
    virtual void SetNeedDraw(bool flag) override
    {
        needDraw_ = flag;
    }

    std::shared_ptr<Drawing::Image> Snapshot() const override
    {
        return image_;
    }

    RSRenderNodeDrawableType GetDrawableType() const override
    {
        return RSRenderNodeDrawableType::CANVAS_DRAWING_NODE_DRAWABLE;
    }

private:
    explicit RSCanvasDrawingRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::CANVAS_DRAWING_NODE, OnGenerate>;
    void ProcessCPURenderInBackgroundThread(std::shared_ptr<Drawing::DrawCmdList> cmds,
        std::shared_ptr<RSContext> ctx, NodeId nodeId);
    void DrawRenderContent(Drawing::Canvas& canvas, const Drawing::Rect& rect);
    bool ResetSurfaceForGL(int width, int height, RSPaintFilterCanvas& canvas);
    bool ResetSurfaceForVK(int width, int height, RSPaintFilterCanvas& canvas);
    bool GpuContextResetGL(int width, int height, std::shared_ptr<Drawing::GPUContext>& gpuContext);
    bool GpuContextResetVK(int width, int height, std::shared_ptr<Drawing::GPUContext>& gpuContext);
    Drawing::TextureOrigin GetTextureOrigin();
    void DrawRegionForDfx(Drawing::Canvas& canvas, const Drawing::Rect& bounds);
#ifdef RS_ENABLE_VK
    bool ReleaseSurfaceVK(int width, int height);
#endif
    bool ResetSurfaceforPlayback(int width, int height);
    bool GetCurrentContext(std::shared_ptr<Drawing::GPUContext>& grContext);
    bool IsNeedResetSurface() const;
    void FlushForGL(float width, float height, std::shared_ptr<RSContext> context,
        NodeId nodeId, RSPaintFilterCanvas& rscanvas);
    void FlushForVK(float width, float height, std::shared_ptr<RSContext> context,
        NodeId nodeId, RSPaintFilterCanvas& rscanvas);
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool InitSurfaceForVK(int width, int height, RSPaintFilterCanvas& canvas);
    bool InitSurfaceForGL(int width, int height, RSPaintFilterCanvas& canvas);
    bool ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas);
    bool ReuseBackendTexture(int width, int height, RSPaintFilterCanvas& canvas);
    void ClearPreSurface(std::shared_ptr<Drawing::Surface>& surface);
    bool GetCurrentContextAndImage(std::shared_ptr<Drawing::GPUContext>& grContext,
        std::shared_ptr<Drawing::Image>& image, const uint64_t tid);
#endif
    static Registrar instance_;
    std::recursive_mutex drawableMutex_;
    std::shared_ptr<Drawing::Surface> surface_;
    std::shared_ptr<Drawing::Image> image_;
    std::shared_ptr<Drawing::Image> captureImage_;
    std::shared_ptr<ExtendRecordingCanvas> recordingCanvas_;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool isGpuSurface_ = true;
    Drawing::BackendTexture backendTexture_;
    NativeBufferUtils::VulkanCleanupHelper* vulkanCleanupHelper_ = nullptr;
#endif
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    std::atomic<pid_t> threadId_ = RSUniRenderThread::Instance().GetTid();

    ThreadInfo curThreadInfo_ = { UNI_RENDER_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };
    ThreadInfo preThreadInfo_ = { UNI_RENDER_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };

    // setted in render thread, used and resetted in main thread
    std::atomic<bool> needDraw_ = false;
};

} // namespace OHOS::Rosen::DrawableV2
#endif // RENDER_SERVICE_DRAWABLE_RS_CANVAS_DRAWING_RENDER_NODE_DRAWABLE_H
