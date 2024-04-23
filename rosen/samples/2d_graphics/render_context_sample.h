/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_CONTEXT_SAMPLE_H
#define RENDER_CONTEXT_SAMPLE_H

#include <surface.h>
#include <sync_fence.h>

#include "hdi_backend.h"
#include "hdi_layer_info.h"
#include "hdi_output.h"
#include "hdi_screen.h"
#include "render_context/render_context.h"
#include <unordered_map>

namespace OHOS {
namespace Rosen {

class RenderContextSample : public IBufferConsumerListenerClazz {
public:
    RenderContextSample() = default;
    virtual ~RenderContextSample() = default;

    void Run();

private:
    uint32_t freq_ = 30;
    uint32_t display_w = 480;
    uint32_t display_h = 960;
    uint32_t currentModeIndex_ = 0;
    uint32_t backGroundWidth = 0;
    uint32_t backGroundHeight = 0;
    uint32_t drawingWidth = 0;
    uint32_t drawingHeight = 0;
    std::vector<GraphicDisplayModeInfo> displayModeInfos_;

    sptr<IConsumerSurface> backGroundCSurface;
    sptr<Surface> backGroundPSurface;
    sptr<IConsumerSurface> drawingCSurface;
    sptr<Surface> drawingPSurface;

    bool ready_ = false;
    bool initDeviceFinished_ = false;
    bool deviceConnected_ = false;
    std::unique_ptr<HdiScreen> screen_ = nullptr;
    HdiBackend* backend_ = nullptr;
    std::shared_ptr<HdiOutput> output_ = nullptr;
    struct NativeWindow* nwindow = nullptr;

    RenderContext* renderContext = nullptr;
    EGLSurface eglSurface = EGL_NO_SURFACE;

    std::unordered_map<uint64_t, sptr<SurfaceBuffer>> prevBufferMap_;
    std::unordered_map<uint64_t, sptr<SyncFence>> prevFenceMap_;
    void Init();
    void InitEGL();
    void Draw();
    void DrawSurface();
    void Sync(int64_t, void *data);
    void CreatePhysicalScreen();
    void DoPrepareCompleted(sptr<Surface> surface, const struct PrepareCompleteParam &param);
    void OnBufferAvailable() override;
    SurfaceError ProduceBackGroundBuffer(uint32_t width, uint32_t height);
    SurfaceError ProduceDrawingBuffer(uint32_t width, uint32_t height);
    bool FillDrawingLayer(std::shared_ptr<HdiLayerInfo> &showLayer, uint32_t index,
                          uint32_t zorder, GraphicIRect &dstRect);
    bool FillBackGroundLayer(std::shared_ptr<HdiLayerInfo> &showLayer, uint32_t zorder, GraphicIRect &dstRect);
    bool DrawBackgroundLayer(std::shared_ptr<HdiLayerInfo> &layer);
    bool DrawDrawingLayer(std::shared_ptr<HdiLayerInfo> &layer);
    void CreateBackGroundSurface();
    void CreateDrawingSurface();
    void OnHotPlugEvent(const std::shared_ptr<HdiOutput> &output, bool connected);

    static void OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data);
    static void OnPrepareCompleted(
        sptr<Surface> &surface, const struct PrepareCompleteParam &param, void* data);
};
} // namespace Rosen
} // namespace OHOS

#endif // HELLO_COMPOSER_H
