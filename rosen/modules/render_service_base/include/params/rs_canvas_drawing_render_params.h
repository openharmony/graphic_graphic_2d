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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_CANVAS_DRAWING_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_CANVAS_DRAWING_RENDER_PARAMS_H

#include <atomic>

#include "params/rs_render_params.h"
#ifdef RS_MODIFIERS_DRAW_ENABLE
#include "surface_buffer.h"
#include "sync_fence.h"
#endif

namespace OHOS::Rosen {
class RSB_EXPORT RSCanvasDrawingRenderParams : public RSRenderParams {
public:
    struct SurfaceParam {
        int width = 0;
        int height = 0;
        GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    };

    explicit RSCanvasDrawingRenderParams(NodeId id);
    ~RSCanvasDrawingRenderParams() override = default;

    void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    bool GetCanvasDrawingSurfaceChanged() const
    {
        return canvasDrawingNodeSurfaceChanged_;
    }

    void SetCanvasDrawingSurfaceChanged(bool changeFlag);

    uint32_t GetCanvasDrawingResetSurfaceIndex() const
    {
        return canvasDrawingResetSurfaceIndex_;
    }

    void SetCanvasDrawingResetSurfaceIndex(uint32_t index);

    SurfaceParam GetCanvasDrawingSurfaceParams()
    {
        return surfaceParams_;
    }

    void SetCanvasDrawingSurfaceParams(int width, int height,
        GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

#ifdef RS_MODIFIERS_DRAW_ENABLE
    void SetBufferSynced(bool bufferSynced);
 
    bool IsBufferSynced() const;
 
    void SetBufferDraw(bool bufferDraw);
 
    bool IsBufferDraw() const;
 
    void SetBuffer(const sptr<SurfaceBuffer>& buffer,
        std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount, const Rect& damageRect) override;
 
    sptr<SurfaceBuffer> GetBuffer() const override;
 
    void SetPreBuffer(const sptr<SurfaceBuffer>& preBuffer,
        std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> preBufferOwnerCount) override;
 
    sptr<SurfaceBuffer> GetPreBuffer() override;
 
    void SetAcquireFence(const sptr<SyncFence>& acquireFence) override;
 
    sptr<SyncFence> GetAcquireFence() const override;
 
    const Rect& GetBufferDamage() const override;
 
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> GetBufferOwnerCount() const override;
 
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> GetPreBufferOwnerCount() const override;
#endif // RS_MODIFIERS_DRAW_ENABLE

private:
    std::atomic_bool canvasDrawingNodeSurfaceChanged_ = false;
    std::atomic_uint32_t canvasDrawingResetSurfaceIndex_ = 0;
    SurfaceParam surfaceParams_;

#ifdef RS_MODIFIERS_DRAW_ENABLE
    bool bufferSynced_ = true;
    bool bufferDraw_ = false;
    sptr<SurfaceBuffer> buffer_ = nullptr;
    sptr<SurfaceBuffer> preBuffer_ = nullptr;
    sptr<SyncFence> acquireFence_ = SyncFence::InvalidFence();
    Rect damageRect_ = { 0, 0, 0, 0 };
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> preBufferOwnerCount_ = nullptr;
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount_ = nullptr;
#endif // RS_MODIFIERS_DRAW_ENABLE
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_CANVAS_DRAWING_RENDER_PARAMS_H
