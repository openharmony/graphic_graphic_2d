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

#include <securec.h>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>

#include <vsync_helper.h>
#include <display_type.h>
#include <surface.h>
#include "hdi_backend.h"
#include "hdi_layer.h"
#include "hdi_layer_info.h"
#include "hdi_output.h"
#include "hdi_screen.h"
#include <window.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <GLES3/gl32.h>
#include <egl_surface.h>

#include <platform/ohos/backend/rs_surface_ohos_gl.h>
#include <platform/ohos/backend/rs_surface_frame_ohos_gl.h>
#include <render_context/render_context.h>

using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
#define LOG(fmt, ...) ::OHOS::HiviewDFX::HiLog::Info(             \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "RenderContextSample"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
}

class RenderContextSample : public IBufferConsumerListenerClazz {
public:
    void Init(int32_t width, int32_t height, HdiBackend* backend);
    void DoDrawData(void *image, int width, int height);
    void Draw();
    void Sync(int64_t, void *data);
    void CreatePhysicalScreen(std::shared_ptr<HdiOutput> &output);
    void DoPrepareCompleted(OHOS::sptr<Surface> &surface, const struct PrepareCompleteParam &param);
    virtual void OnBufferAvailable() override;

    bool FillLayer(std::shared_ptr<HdiLayerInfo> &showLayer, OHOS::sptr<Surface> &pSurface,
        OHOS::sptr<Surface> &cSurface, uint32_t zorder, IRect &dstRect);

    void DrawSurface();

    int32_t freq = 30;
    int width = 0;
    int height = 0;

    int display_w = 480;
    int display_h = 960;

private:
    uint32_t currentModeIndex = 0;
    std::vector<DisplayModeInfo> displayModeInfos;
    OHOS::sptr<Surface> pSurface = nullptr;
    OHOS::sptr<Surface> cSurface = nullptr;
    OHOS::sptr<Surface> pSurface_GL = nullptr;
    OHOS::sptr<Surface> cSurface_GL = nullptr;
    std::unique_ptr<HdiScreen> screen = nullptr;
    std::shared_ptr<HdiOutput> output = nullptr;
    std::vector<std::shared_ptr<HdiOutput>> outputs;
    HdiBackend* backend = nullptr;
    OHOS::sptr<SurfaceBuffer> fbBuffer = nullptr;
    bool ready = false;
    sptr<EglRenderSurface> eglSurface_ = nullptr;

    RSSurfaceOhosGl *rSSurfaceOhosGl_ = nullptr;

};

void RenderContextSample::OnBufferAvailable()
{
}

void RenderContextSample::Draw()
{
    do {
        static int count = 0;

        if (count >= 2000) {
            count = 0;
        }

        std::vector<LayerInfoPtr> layerVec;
        std::shared_ptr<HdiLayerInfo> showLayer = HdiLayerInfo::CreateHdiLayerInfo();
        std::shared_ptr<HdiLayerInfo> showLayer1 = HdiLayerInfo::CreateHdiLayerInfo();

        int32_t zorder = 0;
        IRect dstRect;
        dstRect.x = 0;  // Absolute coordinates, with offset
        dstRect.y = 0;
        dstRect.w = width;
        dstRect.h = height;

        if (!FillLayer(showLayer1, pSurface, cSurface, zorder, dstRect)) {
            continue;
        }
        layerVec.emplace_back(showLayer1);

        output->SetLayerInfo(layerVec);
        backend->Repaint(outputs);
        count++;
    } while (false);
}

bool RenderContextSample::FillLayer(std::shared_ptr<HdiLayerInfo> &showLayer, OHOS::sptr<Surface> &pSurface,
        OHOS::sptr<Surface> &cSurface, uint32_t zorder, IRect &dstRect)
{
    DrawSurface();

    OHOS::sptr<SurfaceBuffer> cbuffer = nullptr;
    int32_t fence;
    int64_t timestamp;
    Rect damage;
    SurfaceError ret = cSurface->AcquireBuffer(cbuffer, fence, timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        LOG("Acquire cBuffer failed");
        return false;
    }

    IRect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = width;
    srcRect.h = height;

    LayerAlpha alpha = { .enPixelAlpha = true };

    showLayer->SetSurface(cSurface);
    showLayer->SetBuffer(cbuffer, fence);
    showLayer->SetZorder(zorder);
    showLayer->SetAlpha(alpha);
    showLayer->SetTransform(TransformType::ROTATE_NONE);
    showLayer->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    showLayer->SetVisibleRegion(1, srcRect);
    showLayer->SetDirtyRegion(srcRect);
    showLayer->SetLayerSize(dstRect);
    showLayer->SetBlendType(BlendType::BLEND_SRC);
    showLayer->SetCropRect(srcRect);
    showLayer->SetPreMulti(false);

    return true;
}

void RenderContextSample::Sync(int64_t, void *data)
{
    struct OHOS::FrameCallback cb = {
        .frequency_ = freq,
        .timestamp_ = 0,
        .userdata_ = data,
        .callback_ = std::bind(&RenderContextSample::Sync, this, SYNC_FUNC_ARG),
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

void RenderContextSample::DrawSurface()
{
    std::cout << "DrawSurface" << std::endl;
    RenderContextFactory::GetInstance().CreateEngine()->InitializeEglContext();

    auto width = 400;
    auto height = 400;

    auto frame = rSSurfaceOhosGl_->RequestFrame(width, height);
    auto canvas = frame->GetCanvas();
    if (canvas == nullptr) {
        std::cout << "canvas is null" << std::endl;
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorRED);
    canvas->drawCircle(128, 128, 90, paint);
    paint.setColor(SK_ColorWHITE);
    canvas->drawCircle(86, 86, 20, paint);
    canvas->drawCircle(160, 76, 20, paint);
    canvas->drawCircle(140, 150, 35, paint);

    //framePtr->SetDamageRegion(0, 0, width, height);
    rSSurfaceOhosGl_->FlushFrame(frame);
}

void RenderContextSample::Init(int32_t width, int32_t height, HdiBackend* backend)
{
    this->backend = backend;

    cSurface = Surface::CreateSurfaceAsConsumer();
    cSurface->SetDefaultWidthAndHeight(width, height);
    cSurface->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);

    OHOS::sptr<IBufferProducer> producer = cSurface->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    cSurface->RegisterConsumerListener(this);

    rSSurfaceOhosGl_ = new RSSurfaceOhosGl(pSurface);

    Sync(0, nullptr);
}

void RenderContextSample::CreatePhysicalScreen(std::shared_ptr<HdiOutput> &output)
{
    screen = HdiScreen::CreateHdiScreen(output->GetScreenId());
    screen->Init();
    screen->GetScreenSuppportedModes(displayModeInfos);
    this->output = output;
    outputs.push_back(output);
    int supportModeNum = displayModeInfos.size();
    if (supportModeNum > 0) {
        screen->GetScreenMode(currentModeIndex);
        LOG("currentModeIndex:%{public}d", currentModeIndex);
        for (int i = 0; i < supportModeNum; i++) {
            LOG("modes(%{public}d) %{public}dx%{public}d freq:%{public}d", displayModeInfos[i].id, displayModeInfos[i].width,
                displayModeInfos[i].height, displayModeInfos[i].freshRate);
            if (displayModeInfos[i].id == static_cast<int32_t>(currentModeIndex)) {
                this->freq = 60;
                this->display_w = displayModeInfos[i].width;
                this->display_h = displayModeInfos[i].height;
                break;
            }
        }
        screen->SetScreenPowerStatus(DispPowerStatus::POWER_STATUS_ON);
    }
    ready = true;
}

void RenderContextSample::DoPrepareCompleted(OHOS::sptr<Surface> &surface, const struct PrepareCompleteParam &param)
{
    std::cout << "DoPrepareCompleted start" << std::endl;
    if (param.needFlushFramebuffer) {
        BufferRequestConfig requestConfig = {
            .width = display_w,  // need display width
            .height = display_h, // need display height
            .strideAlignment = 0x8,
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

        int fenced = -1;
        surface->RequestBuffer(fbBuffer, fenced, requestConfig);

        LOG("enter DoPrepareCompleted, param.layers %{public}d, fb size:%{public}d", (int)param.layers.size(), fbBuffer->GetSize());

        if (fbBuffer->GetVirAddr() == nullptr) {
            LOG("get virAddr failed, virAddr is nullptr");
            return;
        }
        auto addr = static_cast<uint8_t *>(fbBuffer->GetVirAddr());
        memset_s(addr, fbBuffer->GetSize(), 0, fbBuffer->GetSize());

        surface->FlushBuffer(fbBuffer, -1, flushConfig);
    }
}

static void OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data)
{
    LOG("enter OnScreenPlug, connected is %d", connected);
    auto* thisPtr = static_cast<RenderContextSample *>(data);
    if (connected) {
        thisPtr->CreatePhysicalScreen(output);
    }
}

static void OnPrepareCompleted(OHOS::sptr<Surface> &surface, const struct PrepareCompleteParam &param, void* data)
{
    LOG("enter OnPrepareCompleted");
    auto* thisPtr = static_cast<RenderContextSample *>(data);
    thisPtr->DoPrepareCompleted(surface, param);
}

int main(int argc, const char *argv[])
{
    RenderContextSample m;
    LOG("start to HdiBackend::GetInstance");
    HdiBackend* backend = OHOS::Rosen::HdiBackend::GetInstance();
    if (backend == nullptr) {
        LOG("HdiBackend::GetInstance fail");
        return -1;
    }

    backend->RegScreenHotplug(OnScreenPlug, &m);
    backend->RegPrepareComplete(OnPrepareCompleted, &m);

    m.width = 400;
    m.height = 400;

    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&RenderContextSample::Init, &m, m.width, m.height, backend));
    runner->Run();
    return 0;
}
