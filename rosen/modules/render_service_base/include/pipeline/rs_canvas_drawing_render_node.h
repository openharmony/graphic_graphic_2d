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
#ifdef RS_MODIFIERS_DRAW_ENABLE
#include "pipeline/rs_surface_handler.h"
#endif

namespace OHOS {
namespace Rosen {

using ThreadInfo = std::pair<uint32_t, std::function<void(std::shared_ptr<Drawing::Surface>)>>;
using ModifierCmdList = std::list<SimpleDrawCmdListPtr>;
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

    Drawing::Bitmap GetBitmap();
    Drawing::Bitmap GetBitmap(const uint64_t tid);
    std::shared_ptr<Drawing::Image> GetImage(const uint64_t tid);
    bool GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect,
        const uint64_t tid = UINT32_MAX, Drawing::DrawCmdListPtr drawCmdList = nullptr,
        Drawing::DrawCmdListPtr lastDrawCmdList = nullptr);

    uint32_t GetTid() const;

    void AddDirtyType(ModifierNG::RSModifierType modifierType) override;
    void ClearOp();
    void ResetSurface(int width, int height, uint32_t resetSurfaceIndex);
    bool IsNeedProcess() const;
    void ContentStyleSlotUpdate();
    void SetNeedProcess(bool needProcess);
    const std::map<ModifierNG::RSModifierType, ModifierCmdList>& GetDrawCmdListsNG() const;
    void AccumulateLastDirtyTypes() override;
    void ClearNeverOnTree() override;
    void CheckCanvasDrawingPostPlaybacked();
    bool GetIsPostPlaybacked();

    void ApplyModifiers() override;
    bool CheckCachedOp();
    bool HasCachedOp() const;

    void SetWaitSync(bool waitSync)
    {
        waitSync_ = waitSync;
    }

    bool IsNodeMemClearEnable() override;
 
#ifdef RS_MODIFIERS_DRAW_ENABLE
    const std::shared_ptr<RSSurfaceHandler> GetSurfaceHandler() const
    {
        return surfaceHandler_;
    }
 
    std::shared_ptr<RSSurfaceHandler> GetMutableSurfaceHandler()
    {
        return surfaceHandler_;
    }
 
    void SetSurfaceHandler(std::shared_ptr<RSSurfaceHandler> surfaceHandler);
 
    RSB_EXPORT void UpdateBufferInfo(const sptr<SurfaceBuffer>& buffer,
        std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount, const Rect& damageRect,
        const sptr<SyncFence>& acquireFence, const sptr<SurfaceBuffer>& preBuffer,
        std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> preBufferOwnerCount);
 
    void OnDestoryTokenNode();
 
    bool IsBufferDraw();
 
    static bool IsHybridEnabled();
#endif // RS_MODIFIERS_DRAW_ENABLE

protected:
    void OnApplyModifiers() override;

private:
    explicit RSCanvasDrawingRenderNode(
        NodeId id, const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false);
    void ApplyDrawCmdModifierNG(ModifierNG::RSModifierContext& context, ModifierNG::RSModifierType type);
    void CheckDrawCmdListSizeNG(ModifierNG::RSModifierType type);
    bool ResetSurface(int width, int height, RSPaintFilterCanvas& canvas);
    bool GetSizeFromDrawCmdModifiers(int& width, int& height);
    bool IsNeedResetSurface() const;
    void InitRenderParams() override;
    void ReportOpCount(const std::list<SimpleDrawCmdListPtr>& cmdLists) const;
    void SplitDrawCmdList(size_t firstOpCount, SimpleDrawCmdListPtr drawCmdList, bool splitOrigin);
    size_t ApplyCachedCmdList();
    void AfterSync();
    void ClearResource();
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas);
#endif
#ifdef RS_MODIFIERS_DRAW_ENABLE
    void InitSurfaceHandler();
#endif

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    bool isGpuSurface_ = true;
#endif
    bool isNeverOnTree_ = true;
    bool isPostPlaybacked_ = false;
    bool lastOverflowStatus_ = false;
    std::atomic<bool> isNeedProcess_ = false;
    bool waitSync_ = false;
    // Used in uni render thread.
    uint32_t drawingNodeRenderID = UNI_MAIN_THREAD_INDEX;
    std::shared_ptr<Drawing::Surface> surface_;
    std::shared_ptr<Drawing::Image> image_;
    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas_;
    std::unique_ptr<RSPaintFilterCanvas> canvas_;
    std::mutex imageMutex_;
    std::mutex taskMutex_;
    std::mutex drawCmdListsMutex_;
    std::map<ModifierNG::RSModifierType, ModifierCmdList> drawCmdListsNG_;
    ModifierCmdList outOfLimitCmdList_;
    size_t cachedOpCount_ = 0;

    int64_t lastResetSurfaceTime_ = 0;
    size_t opCountAfterReset_ = 0;

    bool modifiersApplied_ = false;

#ifdef RS_MODIFIERS_DRAW_ENABLE
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_ = nullptr;
    bool sizeOutOfGpuLimit_ = false;
    bool firstBufferAcquired_ = false;
#endif

    friend class RSCanvasDrawingNodeCommandHelper;
    friend class RSRenderNode;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CANVAS_DRAWING_RENDER_NODE_H