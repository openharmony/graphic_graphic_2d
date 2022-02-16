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
#include <sync_fence.h>
#include <vsync_helper.h>
#include <iostream>

using namespace OHOS;
using namespace OHOS::Rosen;

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

void RenderContextSample::OnPrepareCompleted(sptr<Surface> &surface, const struct PrepareCompleteParam &param, void* data)
{
    if (!param.needFlushFramebuffer) {
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
    backGroundWidth = 2560;
    backGroundHeight = 1600;

    drawingWidth = 1200;
    drawingHeight = 800;

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
    backGroundCSurface = Surface::CreateSurfaceAsConsumer();
    backGroundCSurface->SetDefaultWidthAndHeight(backGroundWidth, backGroundHeight);
    backGroundCSurface->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);

    sptr<IBufferProducer> producer = backGroundCSurface->GetProducer();
    backGroundPSurface= Surface::CreateSurfaceAsProducer(producer);
    backGroundCSurface->RegisterConsumerListener(this);

    prevBufferMap_[backGroundCSurface->GetUniqueId()] = nullptr;
    prevFenceMap_[backGroundCSurface->GetUniqueId()] = -1;
}

void RenderContextSample::CreateDrawingSurface()
{
    drawingCSurface = Surface::CreateSurfaceAsConsumer();
    drawingCSurface->SetDefaultWidthAndHeight(backGroundWidth, backGroundHeight);
    drawingCSurface->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);

    sptr<IBufferProducer> producer = drawingCSurface->GetProducer();
    drawingPSurface= Surface::CreateSurfaceAsProducer(producer);
    drawingCSurface->RegisterConsumerListener(this);

    prevBufferMap_[drawingCSurface->GetUniqueId()] = nullptr;
    prevFenceMap_[drawingCSurface->GetUniqueId()] = -1;
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
        .format = PIXEL_FMT_RGBA_8888,
        .usage = backGroundPSurface->GetDefaultUsage(),
    };

    SurfaceError ret = backGroundPSurface->RequestBuffer(buffer, releaseFence, config);
    if (ret != 0) {
        std::cout << "RequestBuffer failed:" << SurfaceErrorStr(ret).c_str() << std::endl;
        return ret;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(100);

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
    std::cout << "ProduceDrawingBuffer start" << std::endl;

    if (nwindow == nullptr) {
        nwindow = CreateNativeWindowFromSurface(&drawingPSurface);
        eglSurface = renderContext->CreateEGLSurface((EGLNativeWindowType)nwindow);
    }

    NativeWindowHandleOpt(nwindow, SET_BUFFER_GEOMETRY, width, height);

    renderContext->MakeCurrent(eglSurface);

    SkCanvas* canvas = renderContext->AcquireCanvas(width, height);

    canvas->clear(SkColorSetARGB(0x20, 0x20, 0x20, 0xFF));
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    paint.setAntiAlias(true);
    canvas->drawCircle(128, 128, 90, paint);
    paint.setColor(SK_ColorGREEN);
    canvas->drawCircle(86, 86, 20, paint);
    canvas->drawCircle(160, 76, 20, paint);
    canvas->drawCircle(140, 150, 35, paint);

    renderContext->RenderFrame();
    renderContext->SwapBuffers(eglSurface);

    std::cout << "ProduceDrawingBuffer end" << std::endl;

    return SURFACE_ERROR_OK;
}

bool RenderContextSample::DrawBackgroundLayer(std::shared_ptr<HdiLayerInfo> &layer)
{
    int32_t zorder = 0;
    IRect dstRect;

    dstRect.x = 0;  // Absolute coordinates, with offset
    dstRect.y = 0;
    dstRect.w = 2560;
    dstRect.h = 1600;

    if (!FillBackGroundLayer(layer, zorder, dstRect)) {
        return false;
    }
    return true;
}

bool RenderContextSample::DrawDrawingLayer(std::shared_ptr<HdiLayerInfo> &layer)
{
    int32_t zorder = 1;
    IRect dstRect;

    dstRect.x = 0;  // Absolute coordinates, with offset
    dstRect.y = 0;

    dstRect.w = 1200;
    dstRect.h = 800;

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

        IRect damageRect;
        damageRect.x = 0;
        damageRect.y = 0;
        damageRect.w = display_w;
        damageRect.h = display_h;
        output_->SetOutputDamage(1, damageRect);

        backend_->Repaint(outputs_);
        std::cout << "------ draw count " << count << std::endl;
        count++;
    } while (false);
}

bool RenderContextSample::FillDrawingLayer(std::shared_ptr<HdiLayerInfo> &showLayer, uint32_t index,
    uint32_t zorder, IRect &dstRect)
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

    showLayer->SetSurface(drawingCSurface);
    showLayer->SetBuffer(cbuffer, fence, prevBufferMap_[drawingCSurface->GetUniqueId()],
        prevFenceMap_[drawingCSurface->GetUniqueId()]);
    showLayer->SetZorder(zorder);
    showLayer->SetAlpha(alpha);
    showLayer->SetTransform(TransformType::ROTATE_NONE);
    if (index == 0) {
        // COMPOSITION_CLIENT
        showLayer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    } else {
        showLayer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    }
    showLayer->SetVisibleRegion(1, srcRect);
    showLayer->SetDirtyRegion(srcRect);
    showLayer->SetLayerSize(dstRect);
    showLayer->SetBlendType(BlendType::BLEND_SRC);
    showLayer->SetCropRect(srcRect);
    showLayer->SetPreMulti(false);

    prevBufferMap_[drawingCSurface->GetUniqueId()] = cbuffer;
    prevFenceMap_[drawingCSurface->GetUniqueId()] = fence;

    return true;
}

bool RenderContextSample::FillBackGroundLayer(std::shared_ptr<HdiLayerInfo> &showLayer, uint32_t zorder, IRect &dstRect)
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
    if (ret != SURFACE_ERROR_OK) {
        std::cout << "Acquire cBuffer failed" << std::endl;
        return false;
    }

    IRect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = dstRect.w;
    srcRect.h = dstRect.h;

    LayerAlpha alpha = { .enPixelAlpha = true };

    showLayer->SetSurface(backGroundCSurface);
    showLayer->SetBuffer(cbuffer, fence, prevBufferMap_[backGroundCSurface->GetUniqueId()],
        prevFenceMap_[backGroundCSurface->GetUniqueId()]);
    showLayer->SetZorder(zorder);
    showLayer->SetAlpha(alpha);
    showLayer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    showLayer->SetVisibleRegion(1, srcRect);
    showLayer->SetDirtyRegion(srcRect);
    showLayer->SetLayerSize(dstRect);
    showLayer->SetBlendType(BlendType::BLEND_SRC);
    showLayer->SetCropRect(srcRect);
    showLayer->SetPreMulti(false);

    prevBufferMap_[backGroundCSurface->GetUniqueId()] = cbuffer;
    prevFenceMap_[backGroundCSurface->GetUniqueId()] = fence;

    std::cout << "FillBackGroundLayer finished" << std::endl;
    return true;
}

void RenderContextSample::CreatePhysicalScreen()
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

    ready_ = true;
}

void RenderContextSample::OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected)
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

void RenderContextSample::DoPrepareCompleted(sptr<Surface> &surface, const struct PrepareCompleteParam &param)
{
    BufferRequestConfig requestConfig = {
        .width = display_w,  // need display width
        .height = display_h, // need display height
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_BGRA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA | HBM_USE_MEM_FB,
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
    tempFence->Wait(100);


    auto addr = static_cast<uint8_t *>(fbBuffer->GetVirAddr());
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
