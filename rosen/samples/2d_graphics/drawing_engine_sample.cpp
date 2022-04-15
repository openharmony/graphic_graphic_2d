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

#include "drawing_engine_sample.h"

#include "window.h"
#include <securec.h>
#include <vsync_generator.h>
#include <vsync_controller.h>
#include <vsync_distributor.h>
#include <vsync_helper.h>
#include <vsync_receiver.h>
#include <iostream>

using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
    sptr<VSyncReceiver> g_receiver = nullptr;
}

void DrawingEngineSample::Run()
{
    auto generator = CreateVSyncGenerator();
    sptr<VSyncController> vsyncController = new VSyncController(generator, 0);
    sptr<VSyncDistributor> vsyncDistributor = new VSyncDistributor(vsyncController, "HelloComposer");
    sptr<VSyncConnection> vsyncConnection = new VSyncConnection(vsyncDistributor, "HelloComposer");
    vsyncDistributor->AddConnection(vsyncConnection);

    std::cout << "start to HdiBackend::GetInstance" << std::endl;
    backend_ = OHOS::Rosen::HdiBackend::GetInstance();
    if (backend_ == nullptr) {
        std::cout << "HdiBackend::GetInstance fail" << std::endl;
        return;
    }

    backend_->RegScreenHotplug(DrawingEngineSample::OnScreenPlug, this);
    while (1) {
        if (output_ != nullptr) {
            break;
        }
    }

    if (!initDeviceFinished_) {
        if (deviceConnected_) {
            CreatePhysicalScreen();
        }
        initDeviceFinished_ = true;
    }
    std::cout << "Init screen succeed" << std::endl;

    backend_->RegPrepareComplete(DrawingEngineSample::OnPrepareCompleted, this);

    sleep(1);

    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    g_receiver = new VSyncReceiver(vsyncConnection, handler);
    g_receiver->Init();
    handler->PostTask(std::bind(&DrawingEngineSample::Init, this));
    runner->Run();
}

void DrawingEngineSample::OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data)
{
    std::cout << "enter OnScreenPlug, connected is " << connected << std::endl;
    auto* thisPtr = static_cast<DrawingEngineSample *>(data);
    thisPtr->OnHotPlugEvent(output, connected);
}

void DrawingEngineSample::OnPrepareCompleted(
    sptr<Surface> &surface, const struct PrepareCompleteParam &param, void* data)
{
    if (!param.needFlushFramebuffer) {
        return;
    }

    if (surface == nullptr) {
        LOGE("surface is null");
        return;
    }

    if (data == nullptr) {
        LOGE("data ptr is null");
        return;
    }

    auto* thisPtr = static_cast<DrawingEngineSample *>(data);
    thisPtr->DoPrepareCompleted(surface, param);
}

void DrawingEngineSample::InitContext()
{
    drawingProxy = new DrawingProxy();
    drawingProxy->InitDrawContext();
}

void DrawingEngineSample::Init()
{
    LOGI("DrawingEngineSample::Init+");
    CreateDrawingSurface();
    InitContext();
    Sync(0, nullptr);
    Initilized = true;
    LOGI("DrawingEngineSample::Init-");
}

void DrawingEngineSample::Sync(int64_t, void *data)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = data,
        .callback_ = std::bind(&DrawingEngineSample::Sync, this, ::std::placeholders::_1, ::std::placeholders::_2),
    };

    if (g_receiver != nullptr) {
        g_receiver->RequestNextVSync(fcb);
    }

    if (!ready_) {
        LOGE("hdi screen is not ready");
        return;
    }

    if (Initilized == false) {
        LOGI("call init function");
        return;
    }

    OutPutDisplay();
}

void DrawingEngineSample::CreateDrawingSurface()
{
    drawingCSurface = Surface::CreateSurfaceAsConsumer();
    drawingCSurface->SetDefaultWidthAndHeight(drawingWidth, drawingHeight);
    drawingCSurface->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);

    sptr<IBufferProducer> producer = drawingCSurface->GetProducer();
    drawingPSurface= Surface::CreateSurfaceAsProducer(producer);
    drawingCSurface->RegisterConsumerListener(this);

    prevBufferMap_[drawingCSurface->GetUniqueId()] = nullptr;
    prevFenceMap_[drawingCSurface->GetUniqueId()] = SyncFence::INVALID_FENCE;
}

void DrawingEngineSample::OnBufferAvailable()
{
}

void DrawingEngineSample::Draw(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0xFF9A67BE);
    paint.setStrokeWidth(20);
    canvas->skew(1, 0);
    canvas->drawLine(32, 96,32, 160, paint); // point position
    canvas->skew(-2, 0);
    canvas->drawLine(288, 96, 288, 160, paint); // point position
}

SurfaceError DrawingEngineSample::DoDraw()
{
    LOGI("DrawingEngineSample::DoDraw+");

    std::shared_ptr<RSSurface> rsSurface = OHOS::Rosen::RSSurfaceOhos::CreateSurface(drawingPSurface);
    rsSurface->SetDrawingProxy(drawingProxy);

    auto surfaceFrame = rsSurface->RequestFrame(drawingWidth, drawingHeight);
    if (surfaceFrame == nullptr) {
        std::cout << "Request Frame Failed" << std::endl;
        return SURFACE_ERROR_ERROR;
    }

    SkCanvas* canvas = rsSurface->GetCanvas(surfaceFrame);
    Draw(canvas);

    rsSurface->FlushFrame(surfaceFrame);
    
    LOGI("DrawingEngineSample::DoDraw-"); 
    return SURFACE_ERROR_OK;
}

bool DrawingEngineSample::DrawDrawingLayer(std::shared_ptr<HdiLayerInfo> &layer)
{
    int32_t zorder = 1;
    IRect dstRect;
    dstRect.x = 0;  // Absolute coordinates, with offset
    dstRect.y = 0;
    dstRect.w = display_w;
    dstRect.h = display_h;
    int index = -1;

    SurfaceError err = DoDraw();
    if (err != SURFACE_ERROR_OK) {
        std::cout << "DoDraw failed" << std::endl;
        return false;
    }

    OHOS::sptr<SurfaceBuffer> cbuffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp;
    Rect damage;
    SurfaceError ret = drawingCSurface->AcquireBuffer(cbuffer, fence, timestamp, damage);
    sptr<SyncFence> acquireSyncFence = new SyncFence(fence);
    if (ret != SURFACE_ERROR_OK) {
        std::cout << "Acquire cBuffer failed: " << ret << std::endl;
        return false;
    }

    IRect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = drawingWidth;
    srcRect.h = drawingHeight;
    LayerAlpha alpha = { .enPixelAlpha = true };
    layer->SetSurface(drawingCSurface);
    layer->SetBuffer(cbuffer, acquireSyncFence, prevBufferMap_[drawingCSurface->GetUniqueId()],
        prevFenceMap_[drawingCSurface->GetUniqueId()]);
    layer->SetZorder(zorder);
    layer->SetAlpha(alpha);
    layer->SetTransform(TransformType::ROTATE_NONE);
    if (index == 0) {
        // COMPOSITION_CLIENT
        layer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    } else {
        layer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    }
    layer->SetVisibleRegion(1, srcRect);
    layer->SetDirtyRegion(srcRect);
    layer->SetLayerSize(dstRect);
    layer->SetBlendType(BlendType::BLEND_SRC);
    layer->SetCropRect(srcRect);
    layer->SetPreMulti(false);

    prevBufferMap_[drawingCSurface->GetUniqueId()] = cbuffer;
    prevFenceMap_[drawingCSurface->GetUniqueId()] = acquireSyncFence;

    return true;
}

void DrawingEngineSample::OutPutDisplay()
{
    static int32_t count = 0;
    std::shared_ptr<HdiLayerInfo> drawingLayer = HdiLayerInfo::CreateHdiLayerInfo();
    do {
        std::cout << " draw count " << count << std::endl;
        LOGI("OutPutDisplay draw count is %{pubic}d", count);

        if (!DrawDrawingLayer(drawingLayer)) {
            std::cout << "DrawDrawingLayer failed!" << std::endl;
            return;
        }

        std::vector<LayerInfoPtr> layers;
        layers.push_back(drawingLayer);
        output_->SetLayerInfo(layers);

        IRect damageRect;
        damageRect.x = 0;
        damageRect.y = 0;
        damageRect.w = display_w;
        damageRect.h = display_h;
        output_->SetOutputDamage(1, damageRect);

        backend_->Repaint(outputs_);
        std::cout << "draw count " << count << std::endl;
        std::cout << "display width is " << display_w << ", display height is " << display_h << std::endl;

        count++;
    } while (false);
}

void DrawingEngineSample::CreatePhysicalScreen()
{
    screen_ = HdiScreen::CreateHdiScreen(output_->GetScreenId());
    screen_->Init();
    screen_->GetScreenSupportedModes(displayModeInfos_);
    outputs_.push_back(output_);
    size_t supportModeNum = displayModeInfos_.size();
    if (supportModeNum > 0) {
        screen_->GetScreenMode(currentModeIndex_);
        for (size_t i = 0; i < supportModeNum; i++) {
            if (displayModeInfos_[i].id == static_cast<int32_t>(currentModeIndex_)) {
                this->freq_ = displayModeInfos_[i].freshRate;
                this->display_w = displayModeInfos_[i].width;
                this->display_h = displayModeInfos_[i].height;
            }
        }
        screen_->SetScreenPowerStatus(DispPowerStatus::POWER_STATUS_ON);
        screen_->SetScreenMode(currentModeIndex_);

        DispPowerStatus powerState;
        screen_->SetScreenPowerStatus(DispPowerStatus::POWER_STATUS_ON);
        screen_->GetScreenPowerStatus(powerState);
    }

    DisplayCapability info;
    screen_->GetScreenCapability(info);

    std::cout << "display width is " << this->display_w << " display height is " << this->display_h << std::endl;

    drawingWidth = this->display_w;
    drawingHeight = this->display_h;

    ready_ = true;
}

void DrawingEngineSample::OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected)
{
    /*
     * Currently, IPC communication cannot be nested. Therefore, Vblank registration can be
     * initiated only after the initialization of the device is complete.
     */
    output_ = output;
    deviceConnected_ = connected;

    if (!initDeviceFinished_) {
        std::cout << "Init the device has not finished yet" << std::endl;
        return;
    }

    std::cout << "Callback HotPlugEvent, connected is " << connected << std::endl;
    if (connected) {
        CreatePhysicalScreen();
    }
}

void DrawingEngineSample::DoPrepareCompleted(sptr<Surface> &surface, const struct PrepareCompleteParam &param)
{
    BufferRequestConfig requestConfig = {
        .width = display_w,  // need display width
        .height = display_h, // need display height
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_BGRA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA | HBM_USE_MEM_FB,
        .timeout = 0,
    };

    int32_t releaseFence = -1;
    sptr<SurfaceBuffer> fbBuffer = nullptr;
    SurfaceError err = surface->RequestBuffer(fbBuffer, releaseFence, requestConfig);
    if (err != 0) {
        LOGE("RequestBuffer failed: %{public}s", SurfaceErrorStr(err).c_str());
        return;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(100); // 100 ms

    int32_t ret = 0;
    auto addr = static_cast<uint8_t *>(fbBuffer->GetVirAddr());
    ret = memset_s(addr, fbBuffer->GetSize(), 0, fbBuffer->GetSize());
    if (ret != 0) {
        std::cout << "memset_s failed" << std::endl;
    }

    BufferFlushConfig flushConfig = {
        .damage = {
            .w = display_w,
            .h = display_h,
        }
    };

    /*
     * if use GPU produce data, flush with gpu fence
     */
    ret = surface->FlushBuffer(fbBuffer, -1, flushConfig);
    if (ret != 0) {
        std::cout << "DoPrepareCompleted FlushBuffer failed"<< std::endl;
    }
}