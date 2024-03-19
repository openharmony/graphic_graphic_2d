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

#include <memory>

#include "image/image.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_recording_canvas.h"

#ifndef ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_CONTENT_H
#define ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_CONTENT_H

namespace OHOS::Rosen {
using ThreadInfo = std::pair<uint32_t, std::function<void(std::shared_ptr<Drawing::Surface>)>>;

class RSB_EXPORT RSCanvasDrawingRenderNodeContent {
public:
    RSCanvasDrawingRenderNodeContent() = default;
    ~RSCanvasDrawingRenderNodeContent();

    bool InitSurface(int width, int height, RSPaintFilterCanvas& canvas);
    std::shared_ptr<RSPaintFilterCanvas> GetCanvas();
    std::shared_ptr<Drawing::Image> Flush();

    Drawing::Bitmap GetBitmap(const uint64_t tid = UINT32_MAX);
    bool GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect,
        const uint64_t tid = UINT32_MAX, std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr);

private:
    uint32_t GetTid() const
    {
        return curThreadInfo_.first;
    }

    bool ResetSurface(int width, int height, RSPaintFilterCanvas& canvas);
    bool IsNeedResetSurface() const;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas);
#endif

    std::mutex imageMutex_;
    std::shared_ptr<Drawing::Surface> surface_;
    std::shared_ptr<Drawing::Image> image_;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool isGpuSurface_ = true;
#endif
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    ThreadInfo curThreadInfo_ = { UNI_MAIN_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };
    ThreadInfo preThreadInfo_ = { UNI_MAIN_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };
};
} // namespace OHOS::Rosen

#endif // ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_CONTENT_H
