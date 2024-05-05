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
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_thread.h"

namespace OHOS::Rosen::DrawableV2 {
using ThreadInfo = std::pair<uint32_t, std::function<void(std::shared_ptr<Drawing::Surface>)>>;
class RSCanvasDrawingRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSCanvasDrawingRenderNodeDrawable() override;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;

    void PlaybackInCorrespondThread();
    void SetSurfaceClearFunc(ThreadInfo threadInfo, pid_t threadId = 0)
    {
        curThreadInfo_ = threadInfo;
        threadId_ = threadId;
    }
    bool InitSurface(int width, int height, RSPaintFilterCanvas& canvas);
    std::shared_ptr<RSPaintFilterCanvas> GetCanvas();
    void Flush(float width, float height, std::shared_ptr<RSContext> context,
        NodeId nodeId, RSPaintFilterCanvas& rscanvas);
    Drawing::Bitmap GetBitmap(const uint64_t tid = UINT32_MAX);
    bool GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect,
        const uint64_t tid = UINT32_MAX, std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr);

    uint32_t GetTid() const
    {
        return curThreadInfo_.first;
    }
    void ResetSurface();
private:
    explicit RSCanvasDrawingRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::CANVAS_DRAWING_NODE, OnGenerate>;
    void ProcessCPURenderInBackgroundThread(std::shared_ptr<Drawing::DrawCmdList> cmds,
        std::shared_ptr<RSContext> ctx, NodeId nodeId);
    void DrawRenderContent(Drawing::Canvas& canvas, const Drawing::Rect& rect);
    bool ResetSurface(int width, int height, RSPaintFilterCanvas& canvas);
    bool IsNeedResetSurface() const;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas);
    void ClearPreSurface(std::shared_ptr<Drawing::Surface>& surface);
#endif
    static Registrar instance_;
    std::mutex taskMutex_;
    std::mutex imageMutex_;
    std::shared_ptr<Drawing::Surface> surface_;
    std::shared_ptr<Drawing::Image> image_;
    std::shared_ptr<ExtendRecordingCanvas> recordingCanvas_;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool isGpuSurface_ = true;
    Drawing::BackendTexture backendTexture_;
#endif
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    pid_t threadId_ = RSUniRenderThread::Instance().GetTid();

    ThreadInfo curThreadInfo_ = { UNI_RENDER_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };
    ThreadInfo preThreadInfo_ = { UNI_RENDER_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };
};

} // namespace OHOS::Rosen::DrawableV2
#endif // RENDER_SERVICE_DRAWABLE_RS_CANVAS_DRAWING_RENDER_NODE_DRAWABLE_H
