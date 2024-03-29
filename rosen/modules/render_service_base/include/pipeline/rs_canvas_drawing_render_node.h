/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_H
#define ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_H

#include <atomic>
#include <functional>
#include <memory>

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_recording_canvas.h"

namespace OHOS {
namespace Rosen {
static std::mutex drawingMutex_;
using ThreadInfo = std::pair<uint32_t, std::function<void(std::shared_ptr<Drawing::Surface>)>>;
class RSRecordingCanvas;

class RSB_EXPORT RSCanvasDrawingRenderNode : public RSCanvasRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasDrawingRenderNode>;
    using SharedPtr = std::shared_ptr<RSCanvasDrawingRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::CANVAS_DRAWING_NODE;

    explicit RSCanvasDrawingRenderNode(
        NodeId id, const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false);
    virtual ~RSCanvasDrawingRenderNode();

    // Used in uni render thread.
    void InitRenderContent();

    void ProcessRenderContents(RSPaintFilterCanvas& canvas) override;

    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    bool GetUifirstSupportFlag() override
    {
        return false;
    }

    Drawing::Bitmap GetBitmap();
    Drawing::Bitmap GetBitmap(const uint64_t tid);
    std::shared_ptr<Drawing::Image> GetImage(const uint64_t tid);
    bool GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect,
        const uint64_t tid = UINT32_MAX, std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr);

    void SetSurfaceClearFunc(ThreadInfo threadInfo, pid_t threadId = 0)
    {
        curThreadInfo_ = threadInfo;
        threadId_ = threadId;
    }

    uint32_t GetTid() const;

    void OnApplyModifiers() override;
    void ClearOp();
    void ResetSurface();
    bool IsNeedProcess() const
    {
        return isNeedProcess_;
    }
    void PlaybackInCorrespondThread();

private:
    void ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type);
    bool ResetSurface(int width, int height, RSPaintFilterCanvas& canvas);
    bool GetSizeFromDrawCmdModifiers(int& width, int& height);
    bool IsNeedResetSurface() const;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas);
#endif
    void ProcessCPURenderInBackgroundThread(std::shared_ptr<Drawing::DrawCmdList> cmds);

    std::mutex imageMutex_;
    std::shared_ptr<Drawing::Surface> surface_;
    std::shared_ptr<Drawing::Image> image_;
    std::shared_ptr<ExtendRecordingCanvas> recordingCanvas_;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool isGpuSurface_ = true;
#endif
    std::unique_ptr<RSPaintFilterCanvas> canvas_;
    ThreadInfo curThreadInfo_ = { UNI_MAIN_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };
    ThreadInfo preThreadInfo_ = { UNI_MAIN_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };
    std::mutex taskMutex_;
    std::atomic<bool> isNeedProcess_ = false;
    pid_t threadId_ = 0;
    std::mutex drawCmdListsMutex_;
    std::map<RSModifierType, std::list<Drawing::DrawCmdListPtr>> drawCmdLists_;

    // Used in uni render thread.
    uint32_t drawingNodeRenderID = UNI_MAIN_THREAD_INDEX;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_H