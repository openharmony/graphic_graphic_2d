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

using ThreadInfo = std::pair<uint32_t, std::function<void(std::shared_ptr<Drawing::Surface>)>>;
class RSRecordingCanvas;

class RSB_EXPORT RSCanvasDrawingRenderNode : public RSCanvasRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasDrawingRenderNode>;
    using SharedPtr = std::shared_ptr<RSCanvasDrawingRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::CANVAS_DRAWING_NODE;

    virtual ~RSCanvasDrawingRenderNode();

    // Used in uni render thread.
    void InitRenderContent();

    void ProcessRenderContents(RSPaintFilterCanvas& canvas) override;

    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    bool OpincGetNodeSupportFlag() override
    {
        return !GetOpincCache().OpincGetRootFlag();
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

    void AddDirtyType(RSModifierType modifierType) override;
    void ClearOp();
    void ResetSurface(int width, int height);
    bool IsNeedProcess() const;
    void ContentStyleSlotUpdate();
    void SetNeedProcess(bool needProcess);
    void PlaybackInCorrespondThread();
    const std::map<RSModifierType, std::list<Drawing::DrawCmdListPtr>>& GetDrawCmdLists() const;
    void ClearResource() override;
    void ClearNeverOnTree() override;
    void CheckCanvasDrawingPostPlaybacked();
    bool GetIsPostPlaybacked();
private:
    explicit RSCanvasDrawingRenderNode(
        NodeId id, const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false);
    void ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type);
    void CheckDrawCmdListSize(RSModifierType type, size_t originCmdListSize);
    bool ResetSurface(int width, int height, RSPaintFilterCanvas& canvas);
    bool GetSizeFromDrawCmdModifiers(int& width, int& height);
    bool IsNeedResetSurface() const;
    void InitRenderParams() override;
    void ReportOpCount(const std::list<Drawing::DrawCmdListPtr>& cmdLists) const;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas);
#endif

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool isGpuSurface_ = true;
#endif
    bool isNeverOnTree_ = true;
    bool isPostPlaybacked_ = false;
    bool lastOverflowStatus_ = false;
    std::atomic<bool> isNeedProcess_ = false;
    pid_t threadId_ = 0;
    // Used in uni render thread.
    uint32_t drawingNodeRenderID = UNI_MAIN_THREAD_INDEX;
    std::shared_ptr<Drawing::Surface> surface_;
    std::shared_ptr<Drawing::Image> image_;
    std::shared_ptr<ExtendRecordingCanvas> recordingCanvas_;
    std::unique_ptr<RSPaintFilterCanvas> canvas_;
    std::mutex imageMutex_;
    ThreadInfo curThreadInfo_ = { UNI_MAIN_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };
    ThreadInfo preThreadInfo_ = { UNI_MAIN_THREAD_INDEX, std::function<void(std::shared_ptr<Drawing::Surface>)>() };
    std::mutex taskMutex_;
    std::mutex drawCmdListsMutex_;
    std::map<RSModifierType, std::list<Drawing::DrawCmdListPtr>> drawCmdLists_;
    uint32_t cmdCount_ = 0;


    friend class RSCanvasDrawingNodeCommandHelper;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_H