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

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <memory>
#include <securec.h>
#include <iostream>
#include <unistd.h>

#include <vector>
#include <vsync_helper.h>
#include <display_type.h>
#include <surface.h>
#include "graphic_common_c.h"
#include "hdi_backend.h"
#include "hdi_layer.h"
#include "hdi_layer_info.h"
#include "hdi_output.h"
#include "hdi_screen.h"
#include "refbase.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

namespace {
#define LOG(fmt, ...) ::OHOS::HiviewDFX::HiLog::Info(             \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "Hellocomposer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
}

class LayerContext : public IBufferConsumerListenerClazz {
public:
    LayerContext(IRect dst, IRect src, uint32_t zorder, bool dynamic = false) : dst_(dst),
        src_(src),
        zorder_(zorder),
        dynamic_(dynamic)
    {
        cSurface_ = Surface::CreateSurfaceAsConsumer();
        cSurface_->SetDefaultWidthAndHeight(src.w, src.h);
        cSurface_->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);

        OHOS::sptr<IBufferProducer> producer = cSurface_->GetProducer();
        pSurface_ = Surface::CreateSurfaceAsProducer(producer);
        cSurface_->RegisterConsumerListener(this);

        hdiLayer_ = HdiLayerInfo::CreateHdiLayerInfo();
        LOG("%s: create surface w:%{public}d, h:%{public}d", __func__, src.w, src.h);
    }

    ~LayerContext()
    {
        cSurface_ = nullptr;
        pSurface_ = nullptr;
        buffer_ = nullptr;
        hdiLayer_ = nullptr;
    }

    virtual void OnBufferAvailable() override { }

    GSError DrawBufferColor()
    {
        int32_t releaseFence;
        BufferRequestConfig config = {
            .width = src_.w,
            .height = src_.h,
            .strideAlignment = 0x8,
            .format = PIXEL_FMT_RGBA_8888,
            .usage = pSurface_->GetDefaultUsage(),
        };

        GSError ret = pSurface_->RequestBufferWithFence(buffer_, releaseFence, config);
        if (ret != 0) {
            LOG("RequestBuffer failed: %{public}s", SurfaceErrorStr(ret).c_str());
            return ret;
        }
        if (buffer_ == nullptr) {
            LOG("%s: buffer is nullptr", __func__);
            return SURFACE_ERROR_NULLPTR;
        }

        auto addr = static_cast<uint8_t *>(buffer_->GetVirAddr());
        LOG("buffer w:%{public}d h:%{public}d stride:%{public}d", buffer_->GetWidth(), buffer_->GetHeight(), buffer_->GetBufferHandle()->stride);
        DrawColor(addr, buffer_->GetWidth(), buffer_->GetHeight());

        BufferFlushConfig flushConfig = {
            .damage = {
                .w = src_.w,
                .h = src_.h,
            },
        };
        ret = pSurface_->FlushBuffer(buffer_, -1, flushConfig);

        return ret;
    }

    GSError FillHDILayer()
    {
        OHOS::sptr<SurfaceBuffer> cbuffer = nullptr;
        int32_t fence = -1;
        int64_t timestamp;
        Rect damage;
        GSError ret = cSurface_->AcquireBuffer(cbuffer, fence, timestamp, damage);
        if (ret != SURFACE_ERROR_OK) {
            LOG("Acquire cBuffer failed");
            return ret;
        }

        LayerAlpha alpha = { .enPixelAlpha = true };

        hdiLayer_->SetSurface(cSurface_);
        hdiLayer_->SetBuffer(cbuffer, fence);
        hdiLayer_->SetZorder(zorder_);
        hdiLayer_->SetAlpha(alpha);
        hdiLayer_->SetTransform(TransformType::ROTATE_NONE);
        hdiLayer_->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
        hdiLayer_->SetVisibleRegion(1, src_);
        hdiLayer_->SetDirtyRegion(src_);
        hdiLayer_->SetLayerSize(dst_);
        hdiLayer_->SetBlendType(BlendType::BLEND_SRCOVER);
        hdiLayer_->SetCropRect(src_);
        hdiLayer_->SetPreMulti(false);
        return ret;
    }

    const std::vector<uint32_t> colors_ = {0xff0000ff, 0xff00ff00, 0xff00ffff, 0xffff00ff};
    IRect dst_;
    IRect src_;
    uint32_t colorIndex_ = 0;
    uint32_t frameCounter_ = 0;
    uint32_t zorder_ = 0;
    uint32_t color_ = 0xffff0000;
    OHOS::sptr<Surface> pSurface_;
    OHOS::sptr<Surface> cSurface_;
    OHOS::sptr<SurfaceBuffer> buffer_;
    std::shared_ptr<HdiLayerInfo> hdiLayer_;
    bool dynamic_ = false;

private:
    void DrawColor(void *image, int width, int height)
    {
        frameCounter_ = ++frameCounter_ % 60;
        if (frameCounter_ == 0 && dynamic_) {
            colorIndex_ = ++colorIndex_ % colors_.size();
            color_ = colors_[colorIndex_];
            LOG("%{public}s: index:%{public}u color:%{public}x", __func__, colorIndex_, colors_[colorIndex_]);
        }
        uint32_t *pixel = static_cast<uint32_t *>(image);
        for (int x = 0; x < width; x++) {
            for (int y = 0;  y < height; y++) {
                *pixel++ = color_;
            }
        }
    }
};

class HelloComposer {
public:
    void Init(HdiBackend* backend);
    void Draw();
    void Sync(int64_t, void *data);
    void CreatePhysicalScreen();
    void DoPrepareCompleted(OHOS::sptr<Surface> &surface, const struct PrepareCompleteParam &param);
    void OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected);

    int32_t freq = 30;
    int display_w = 480;
    int display_h = 960;
    bool initDeviceFinished_ = false;
    bool deviceConnected_ = false;
    std::shared_ptr<HdiOutput> output = nullptr;

private:
    uint32_t currentModeIndex = 0;
    std::vector<DisplayModeInfo> displayModeInfos;
    std::unique_ptr<HdiScreen> screen = nullptr;
    
    std::vector<std::shared_ptr<HdiOutput>> outputs;
    HdiBackend* backend = nullptr;
    std::vector<std::unique_ptr<LayerContext>> drawLayers;
    sptr<SurfaceBuffer> fbBuffer = nullptr;
    bool ready = false;
};

void HelloComposer::Draw()
{
    std::vector<LayerInfoPtr> layerVec;
    for (auto &drawLayer : drawLayers) {    // productor
        drawLayer->DrawBufferColor();
    }
    for (auto &drawLayer : drawLayers) {    // comsumer
        drawLayer->FillHDILayer();
        layerVec.emplace_back(drawLayer->hdiLayer_);
    }
    LOG("Draw layer size %{public}d", (int32_t)layerVec.size());
    output->SetLayerInfo(layerVec);
    backend->Repaint(outputs);
}

void HelloComposer::Sync(int64_t, void *data)
{
    // cout << "hello OnvSync: ready" << ready << std::endl;
    struct OHOS::FrameCallback cb = {
        .frequency_ = freq,
        .timestamp_ = 0,
        .userdata_ = data,
        .callback_ = std::bind(&HelloComposer::Sync, this, SYNC_FUNC_ARG),
    };

    OHOS::VsyncError ret = OHOS::VsyncHelper::Current()->RequestFrameCallback(cb);
    if (ret) {
        LOG("RequestFrameCallback inner %{public}d\n", ret);
    }

    if (!ready) {
        return;
    }

    Draw();
}

void HelloComposer::Init(HdiBackend* backend)
{
    this->backend = backend;

    // status bar rect for hi3516
    drawLayers.emplace_back(std::make_unique<LayerContext>(IRect{0, 0, 480, 67}, IRect{0, 0, 480, 67}, 0));
    // nav bar rect for hi3516
    drawLayers.emplace_back(std::make_unique<LayerContext>(IRect{0, 893, 480, 67}, IRect{0, 0, 480, 67}, 1));
    // launch rect for hi3516
    drawLayers.emplace_back(std::make_unique<LayerContext>(IRect{0, 67, 480, 826}, IRect{0, 0, 480, 826}, 2, true));

    // // status bar rect for rk3568
    // drawLayers.emplace_back(std::make_unique<LayerContext>(IRect{0, 0, 720, 89}, IRect{0, 0, 720, 89}, 0));
    // // nav bar rect for rk3568
    // drawLayers.emplace_back(std::make_unique<LayerContext>(IRect{0, 1191, 720, 89}, IRect{0, 0, 720, 89}, 1));
    // // launch rect for rk3568
    // drawLayers.emplace_back(std::make_unique<LayerContext>(IRect{0, 89, 720, 1102}, IRect{0, 0, 720, 1102}, 2, true));

    Sync(0, nullptr);
}

void HelloComposer::CreatePhysicalScreen()
{
    std::cout << "CreatePhysicalScreen begin" << std::endl;
    screen = HdiScreen::CreateHdiScreen(output->GetScreenId());
    screen->Init();
    screen->GetScreenSuppportedModes(displayModeInfos);
    outputs.push_back(output);
    int supportModeNum = displayModeInfos.size();
    if (supportModeNum > 0) {
        screen->GetScreenMode(currentModeIndex);
        LOG("currentModeIndex:%{public}d", currentModeIndex);
        for (int i = 0; i < supportModeNum; i++) {
            LOG("modes(%{public}d) %{public}dx%{public}d freq:%{public}d", displayModeInfos[i].id, displayModeInfos[i].width,
                displayModeInfos[i].height, displayModeInfos[i].freshRate);
            if (displayModeInfos[i].id == static_cast<int32_t>(currentModeIndex)) {
                this->freq = 30;
                this->display_w =  displayModeInfos[i].width;
                this->display_h = displayModeInfos[i].height;
                break;
            }
        }
        screen->SetScreenPowerStatus(DispPowerStatus::POWER_STATUS_ON);
    }
    ready = true;
    std::cout << "CreatePhysicalScreen end " << std::endl;
}

void HelloComposer::DoPrepareCompleted(OHOS::sptr<Surface> &surface, const struct PrepareCompleteParam &param)
{
    if (param.needFlushFramebuffer) {
        BufferRequestConfig requestConfig = {
            .width = display_w,
            .height = display_h,
            .strideAlignment = 0x08,
            .format = PIXEL_FMT_BGRA_8888,
            .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
            .timeout = 0,
        };

        BufferFlushConfig flushConfig = {
            .damage = {
                .w = display_w,
                .h = display_h,
            }
        };
        int releaseFence = -1;
        surface->RequestBufferWithFence(fbBuffer, releaseFence, requestConfig);
        auto addr = static_cast<uint8_t *>(fbBuffer->GetVirAddr());

        memset_s(addr, fbBuffer->GetSize(), 0, fbBuffer->GetSize());
        surface->FlushBuffer(fbBuffer, -1, flushConfig);
    }
}


static void OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data)
{
    LOG("enter OnScreenPlug, connected is %{public}d", connected);
    auto* thisPtr = static_cast<HelloComposer *>(data);
    thisPtr->OnHotPlugEvent(output, connected);
}

void HelloComposer::OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected)
{
    /*
     * Currently, IPC communication cannot be nested. Therefore, Vblank registration can be
     * initiated only after the initialization of the device is complete.
     */
    this->output = output;
    deviceConnected_ = connected;

    if (!initDeviceFinished_) {
        LOG("Init the device has not finished yet");
        return;
    }

    LOG("Callback HotPlugEvent, connected is %{public}u", connected);

    if (connected) {
        CreatePhysicalScreen();
    }
 }

static void OnPrepareCompleted(OHOS::sptr<Surface> &surface, const struct PrepareCompleteParam &param, void* data)
{
    LOG("enter OnPrepareCompleted");
    auto *thisPtr = static_cast<HelloComposer *> (data);
    thisPtr->DoPrepareCompleted(surface, param);
}

int main(int argc, const char *argv[])
{
    HelloComposer m;

    LOG("start to HdiBackend::GetInstance");
    std::cout << "begin hello composer" << std::endl;
    HdiBackend* backend = OHOS::Rosen::HdiBackend::GetInstance();
    if (backend == nullptr) {
        LOG("HdiBackend::GetInstance fail");
        return -1;
    }
    std::cout << "start RegScreenHotplug " << std::endl;
    backend->RegScreenHotplug(OnScreenPlug, &m);
    while (1) {
        if (m.output != nullptr) {
            break;
        }
    }

    if (!m.initDeviceFinished_) {
        if (m.deviceConnected_) {
            m.CreatePhysicalScreen();
        }
        m.initDeviceFinished_ = true;
    }
    LOG("Init screen succeed");

    std::cout << "RegPrepareComplete " << std::endl;
    backend->RegPrepareComplete(OnPrepareCompleted, &m);

    std::cout << "main EventRunner " << std::endl;
    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&HelloComposer::Init, &m, backend));
    runner->Run();
    return 0;
}
