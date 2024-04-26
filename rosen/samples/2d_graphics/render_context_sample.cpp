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

#include "render_context_sample.h"

#include "window.h"
#include <securec.h>
#include <event_handler.h>
#include <iostream>

using namespace OHOS;
using namespace OHOS::Rosen;

static const int DRAWING_WIDTH = 1200;
static const int DRAWING_HEIGHT = 800;
static const int SYNC_WAIT_TIMEOUT = 100;

void RenderContextSample::Run()
{
    std::cout << "start to HdiBackend::GetInstance" << std::endl;
    backend_ = OHOS::Rosen::HdiBackend::GetInstance();
    if (backend_ == nullptr) {
        std::cout << "HdiBackend::GetInstance fail" << std::endl;
        return;
    }

    backend_->RegScreenHotplug(RenderContextSample::OnScreenPlug, this);
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

    backend_->RegPrepareComplete(RenderContextSample::OnPrepareCompleted, this);

    sleep(1);

    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&RenderContextSample::Init, this));
    runner->Run();
}

void RenderContextSample::OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data)
{
    std::cout << "enter OnScreenPlug, connected is " << connected << std::endl;
    auto* thisPtr = static_cast<RenderContextSample *>(data);
    thisPtr->OnHotPlugEvent(output, connected);
}

void RenderContextSample::OnPrepareCompleted(
    sptr<Surface> &surface, const struct PrepareCompleteParam &param, void* data)
{
    if (!param.needFlushFramebuffer) {
        return;
    }

    if (surface == nullptr) {
        return;
    }

    if (data == nullptr) {
        return;
    }

    std::cout << "OnPrepareCompleted param.layer size is " << (int)param.layers.size() << std::endl;
    auto* thisPtr = static_cast<RenderContextSample *>(data);
    thisPtr->DoPrepareCompleted(surface, param);
}

void RenderContextSample::InitEGL()
{
    renderContext = new RenderContext();
    renderContext->InitializeEglContext();
}

void RenderContextSample::Init()
{
    backGroundWidth = BACKGROUND_WIDTH;
    backGroundHeight = BACKGROUND_HEIGHT;

    drawingWidth = DRAWING_WIDTH;
    drawingHeight = DRAWING_HEIGHT;

    CreateBackGroundSurface();

    CreateDrawingSurface();

    InitEGL();

    Sync(0, nullptr);
}

void RenderContextSample::Sync(int64_t, void *data)
{
    struct OHOS::FrameCallback cb = {
        .frequency_ = freq_,
        .timestamp_ = 0,
        .userdata_ = data,
        .callback_ = std::bind(&RenderContextSample::Sync, this, SYNC_FUNC_ARG),
    };

    OHOS::VsyncError ret = OHOS::VsyncHelper::Current()->RequestFrameCallback(cb);
    if (ret) {
        std::cout << "RequestFrameCallback inner " <<  ret << std::endl;
    }

    if (!ready_) {
        return;
    }

    Draw();
}

void RenderContextSample::CreateBackGroundSurface()
{
    backGroundCSurface = IConsumerSurface::Create();
    backGroundCSurface->SetDefaultWidthAndHeight(backGroundWidth, backGroundHeight);
    backGroundCSurface->SetDefaultUsage(BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);

    sptr<IBufferProducer> producer = backGroundCSurface->GetProducer();
    backGroundPSurface= Surface::CreateSurfaceAsProducer(producer);
    backGroundCSurface->RegisterConsumerListener(this);

    prevBufferMap_[backGroundCSurface->GetUniqueId()] = nullptr;
    prevFenceMap_[backGroundCSurface->GetUniqueId()] = SyncFence::INVALID_FENCE;
}

void RenderContextSample::CreateDrawingSurface()
{
    drawingCSurface = IConsumerSurface::Create();
    drawingCSurface->SetDefaultWidthAndHeight(backGroundWidth, backGroundHeight);
    drawingCSurface->SetDefaultUsage(BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);

    sptr<IBufferProducer> producer = drawingCSurface->GetProducer();
    drawingPSurface= Surface::CreateSurfaceAsProducer(producer);
    drawingCSurface->RegisterConsumerListener(this);

    prevBufferMap_[drawingCSurface->GetUniqueId()] = nullptr;
    prevFenceMap_[drawingCSurface->GetUniqueId()] = SyncFence::INVALID_FENCE;
}

void RenderContextSample::OnBufferAvailable()
{
}

SurfaceError RenderContextSample::ProduceBackGroundBuffer(uint32_t width, uint32_t height)
{
    OHOS::sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = -1;
    BufferRequestConfig config = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = backGroundPSurface->GetDefaultUsage(),
    };

    SurfaceError ret = backGroundPSurface->RequestBuffer(buffer, releaseFence, config);
    if (ret != 0) {
        std::cout << "RequestBuffer failed:" << SurfaceErrorStr(ret).c_str() << std::endl;
        return ret;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(SYNC_WAIT_TIMEOUT);

    if (buffer == nullptr) {
        std::cout << "buffer is nullptr" << std::endl;
        return SURFACE_ERROR_NULLPTR;
    }

    void* addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    static uint32_t value = 0xffffffff;

    uint32_t *pixel = static_cast<uint32_t*>(addr);

    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0;  y < height; y++) {
            *pixel++ = value;
        }
    }

    BufferFlushConfig flushConfig = {
        .damage = {
            .w = width,
            .h = height,
        },
    };

    int32_t acquireFence = -1;
    ret = backGroundPSurface->FlushBuffer(buffer, acquireFence, flushConfig);

    std::cout << "Sync: " << SurfaceErrorStr(ret).c_str() << std::endl;
    return SURFACE_ERROR_OK;
}

SurfaceError RenderContextSample::ProduceDrawingBuffer(uint32_t width, uint32_t height)
{
    const int CENTER_X = 128;
    const int CENTER_Y = 128;
    const int BIG_CIRCLE_RADIUS = 90;
    const int SMALL_CIRCLE_RADIUS = 20;
    const int MEDIUM_CIRCLE_RADIUS = 35;
    const int SMALL_CIRCLE1_X = 86;
    const int SMALL_CIRCLE1_Y = 86;
    const int SMALL_CIRCLE2_X = 160;
    const int SMALL_CIRCLE2_Y = 76;
    const int MEDIUM_CIRCLE_X = 140;
    const int MEDIUM_CIRCLE_Y = 150;
    std::cout << "ProduceDrawingBuffer start" << std::endl;

    if (nwindow == nullptr) {
        nwindow = CreateNativeWindowFromSurface(&drawingPSurface);
        eglSurface = renderContext->CreateEGLSurface((EGLNativeWindowType)nwindow);
    }
    NativeWindowHandleOpt(nwindow, SET_BUFFER_GEOMETRY, width, height);
    renderContext->MakeCurrent(eglSurface);

    SkCanvas* canvas = renderContext->AcquireSkCanvas(width, height);

    canvas->clear(SkColorSetARGB(0x20, 0x20, 0x20, 0xFF));
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    paint.setAntiAlias(true);
    canvas->drawCircle(CENTER_X, CENTER_Y, BIG_CIRCLE_RADIUS, paint);
    paint.setColor(SK_ColorGREEN);
    canvas->drawCircle(SMALL_CIRCLE1_X, SMALL_CIRCLE1_Y, SMALL_CIRCLE_RADIUS, paint);
    canvas->drawCircle(SMALL_CIRCLE2_X, SMALL_CIRCLE2_Y, SMALL_CIRCLE_RADIUS, paint);
    canvas->drawCircle(MEDIUM_CIRCLE_X, MEDIUM_CIRCLE_Y, MEDIUM_CIRCLE_RADIUS, paint);
    renderContext->RenderFrame();
    renderContext->SwapBuffers(eglSurface);

    std::cout << "ProduceDrawingBuffer end" << std::endl;

    return SURFACE_ERROR_OK;
}

bool RenderContextSample::DrawBackgroundLayer(std::shared_ptr<HdiLayerInfo> &layer)
{
    int32_t zorder = 0;
    GraphicIRect dstRect;

    dstRect.x = 0;  // Absolute coordinates, with offset
    dstRect.y = 0;
    dstRect.w = BACKGROUND_WIDTH;
    dstRect.h = BACKGROUND_HEIGHT;

    if (!FillBackGroundLayer(layer, zorder, dstRect)) {
        return false;
    }
    return true;
}

bool RenderContextSample::DrawDrawingLayer(std::shared_ptr<HdiLayerInfo> &layer)
{
    int32_t zorder = 1;
    GraphicIRect dstRect;

    dstRect.x = 0;  // Absolute coordinates, with offset
    dstRect.y = 0;

    dstRect.w = DRAWING_WIDTH;
    dstRect.h = DRAWING_HEIGHT;

    if (!FillDrawingLayer(layer, 0, zorder, dstRect)) {
        return false;
    }
    return true;
}

void RenderContextSample::Draw()
{
    static int32_t count = 0;
    std::shared_ptr<HdiLayerInfo> backgroundLayer = HdiLayerInfo::CreateHdiLayerInfo();
    std::shared_ptr<HdiLayerInfo> drawingLayer = HdiLayerInfo::CreateHdiLayerInfo();
    do {
        std::cout << "+++++++ draw count " << count << std::endl;
        if (!DrawBackgroundLayer(backgroundLayer)) {
            std::cout << "DrawBackgroundLayer failed!" << std::endl;
            return;
        }

        if (!DrawDrawingLayer(drawingLayer)) {
            std::cout << "DrawDrawingLayer failed!" << std::endl;
            return;
        }

        std::vector<LayerInfoPtr> layers;
        layers.push_back(backgroundLayer);
        layers.push_back(drawingLayer);

        output_->SetLayerInfo(layers);

        GraphicIRect damageRect;
        damageRect.x = 0;
        damageRect.y = 0;
        damageRect.w = display_w;
        damageRect.h = display_h;
        std::vector<GraphicIRect> outputDamages;
        outputDamages.emplace_back(damageRect);
        output_->SetOutputDamages(outputDamages);

        backend_->Repaint(output_);
        for (auto layerI : layers) {
            int32_t releaseFence = -1;
            sptr<SyncFence> tempFence = new SyncFence(releaseFence);
            layerI->GetSurface()->ReleaseBuffer(layerI->GetBuffer(), tempFence);
            tempFence->Wait(SYNC_WAIT_TIMEOUT); // 100 ms
        }
        std::cout << "------ draw count " << count << std::endl;
        count++;
    } while (false);
}

bool RenderContextSample::FillDrawingLayer(std::shared_ptr<HdiLayerInfo> &showLayer, uint32_t index,
    uint32_t zorder, GraphicIRect &dstRect)
{
    if (ProduceDrawingBuffer(drawingWidth, drawingHeight) != SURFACE_ERROR_OK) {
        std::cout << "FillDrawingLayer produce cBuffer failed" << std::endl;
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

    GraphicIRect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = drawingWidth;
    srcRect.h = drawingHeight;

    GraphicLayerAlpha alpha = { .enPixelAlpha = true };

    showLayer->SetSurface(drawingCSurface);
    showLayer->SetBuffer(cbuffer, acquireSyncFence);
    showLayer->SetZorder(zorder);
    showLayer->SetAlpha(alpha);
    showLayer->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    showLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.emplace_back(srcRect);
    showLayer->SetVisibleRegions(visibleRegions);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(srcRect);
    showLayer->SetDirtyRegions(dirtyRegions);
    showLayer->SetLayerSize(dstRect);
    showLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRC);
    showLayer->SetCropRect(srcRect);
    showLayer->SetPreMulti(false);

    prevBufferMap_[drawingCSurface->GetUniqueId()] = cbuffer;
    prevFenceMap_[drawingCSurface->GetUniqueId()] = acquireSyncFence;

    return true;
}

bool RenderContextSample::FillBackGroundLayer(std::shared_ptr<HdiLayerInfo> &showLayer,
                                              uint32_t zorder, GraphicIRect &dstRect)
{
    if (ProduceBackGroundBuffer(dstRect.w, dstRect.h) != SURFACE_ERROR_OK) {
        std::cout << "Produce background cBuffer failed" << std::endl;
        return false;
    }

    OHOS::sptr<SurfaceBuffer> cbuffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp;
    Rect damage;
    SurfaceError ret = backGroundCSurface->AcquireBuffer(cbuffer, fence, timestamp, damage);
    sptr<SyncFence> acquireSyncFence = new SyncFence(fence);
    if (ret != SURFACE_ERROR_OK) {
        std::cout << "Acquire cBuffer failed" << std::endl;
        return false;
    }

    GraphicIRect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = dstRect.w;
    srcRect.h = dstRect.h;

    GraphicLayerAlpha alpha = { .enPixelAlpha = true };

    showLayer->SetSurface(backGroundCSurface);
    showLayer->SetBuffer(cbuffer, acquireSyncFence);
    showLayer->SetZorder(zorder);
    showLayer->SetAlpha(alpha);
    showLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.emplace_back(srcRect);
    showLayer->SetVisibleRegions(visibleRegions);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(srcRect);
    showLayer->SetDirtyRegions(dirtyRegions);
    showLayer->SetLayerSize(dstRect);
    showLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRC);
    showLayer->SetCropRect(srcRect);
    showLayer->SetPreMulti(false);

    prevBufferMap_[backGroundCSurface->GetUniqueId()] = cbuffer;
    prevFenceMap_[backGroundCSurface->GetUniqueId()] = acquireSyncFence;

    std::cout << "FillBackGroundLayer finished" << std::endl;
    return true;
}

void RenderContextSample::CreatePhysicalScreen()
{
    screen_ = HdiScreen::CreateHdiScreen(output_->GetScreenId());
    screen_->Init();
    screen_->GetScreenSupportedModes(displayModeInfos_);
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
        screen_->SetScreenPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON);
        screen_->SetScreenMode(currentModeIndex_);

        GraphicDispPowerStatus powerState;
        screen_->SetScreenPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON);
        screen_->GetScreenPowerStatus(powerState);
    }

    GraphicDisplayCapability info;
    screen_->GetScreenCapability(info);

    ready_ = true;
}

void RenderContextSample::OnHotPlugEvent(const std::shared_ptr<HdiOutput> &output, bool connected)
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

void RenderContextSample::DoPrepareCompleted(sptr<Surface> surface, const struct PrepareCompleteParam &param)
{
    BufferRequestConfig requestConfig = {
        .width = display_w,  // need display width
        .height = display_h, // need display height
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_BGRA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB,
        .timeout = 0,
    };

    if (surface == nullptr) {
        std::cout << "surface is null" << std::endl;
        return;
    }

    int32_t releaseFence = -1;
    sptr<SurfaceBuffer> fbBuffer = nullptr;
    SurfaceError ret1 = surface->RequestBuffer(fbBuffer, releaseFence, requestConfig);
    if (ret1 != 0) {
        std::cout << "RequestBuffer failed " << SurfaceErrorStr(ret1).c_str() << std::endl;
        return;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(SYNC_WAIT_TIMEOUT);
    auto addr = static_cast<uint8_t*>(fbBuffer->GetVirAddr());
    int32_t ret2 = memset_s(addr, fbBuffer->GetSize(), 0, fbBuffer->GetSize());
    if (ret2 != 0) {
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
    ret2 = surface->FlushBuffer(fbBuffer, -1, flushConfig);
    if (ret2 != 0) {
        std::cout << "DoPrepareCompleted FlushBuffer failed"<< std::endl;
    }
}
