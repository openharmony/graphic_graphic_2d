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

#ifndef DRAWING_ENGINE_SAMPLE_H
#define DRAWING_ENGINE_SAMPLE_H

#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"

#include <display_type.h>
#include <surface.h>
#include <event_handler.h>
#include "hdi_backend.h"
#include "hdi_layer_info.h"
#include "hdi_output.h"
#include "hdi_screen.h"
#include "drawing_engine/drawing_utils.h"
#include "drawing_engine/drawing_proxy.h"
#include "drawing_engine/drawing_surface/rs_surface_ohos.h"

namespace OHOS {
namespace Rosen {
class DrawingEngineSample : public IBufferConsumerListenerClazz {
public:
    DrawingEngineSample() = default;
    virtual ~DrawingEngineSample() = default;
    void Run();
private:
    uint32_t freq_ = 30;
    uint32_t display_w = 0;
    uint32_t display_h = 0;
    uint32_t currentModeIndex_ = 0;
    uint32_t drawingWidth = 0;
    uint32_t drawingHeight = 0;
    std::vector<DisplayModeInfo> displayModeInfos_;
    std::vector<std::shared_ptr<HdiOutput>> outputs_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler;
    sptr<Surface> drawingCSurface;
    sptr<Surface> drawingPSurface;

    bool ready_ = false;
    bool initDeviceFinished_ = false;
    bool deviceConnected_ = false;
    bool Initilized = false;
    std::unique_ptr<HdiScreen> screen_ = nullptr;
    HdiBackend* backend_ = nullptr;
    std::shared_ptr<HdiOutput> output_ = nullptr;

    DrawingProxy* drawingProxy = nullptr;

    std::unordered_map<uint64_t, sptr<SurfaceBuffer>> prevBufferMap_;
    std::unordered_map<uint64_t, sptr<SyncFence>> prevFenceMap_;

    void Init();
    void InitContext();
    void OutPutDisplay();
    void Sync(int64_t, void *data);
    void CreatePhysicalScreen();
    void DoPrepareCompleted(sptr<Surface> &surface, const struct PrepareCompleteParam &param);
    virtual void OnBufferAvailable() override;
    SurfaceError DoDraw();
    void Draw(SkCanvas* canvas);
    bool DrawDrawingLayer(std::shared_ptr<HdiLayerInfo> &layer);
    void CreateDrawingSurface();
    void OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected);
    static void OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data);
    static void OnPrepareCompleted(
        std::shared_ptr<RSSurface> &rsSurface, const struct PrepareCompleteParam &param, void* data);
};
} // namespace Rosen
} // namespace OHOS

#endif // HELLO_COMPOSER_H