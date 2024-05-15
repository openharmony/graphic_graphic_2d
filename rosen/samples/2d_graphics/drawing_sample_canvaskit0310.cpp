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
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <securec.h>
#include <sstream>
#include <unistd.h>

#include <sync_fence.h>

#include <event_handler.h>
#include <surface.h>
#include "hdi_backend.h"
#include "hdi_layer.h"
#include "hdi_layer_info.h"
#include "hdi_output.h"
#include "hdi_screen.h"

#include "draw/color.h"
#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/pen.h"
#include "draw/path.h"
#include "draw/clip.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "utils/rect.h"

#include "utils/matrix.h"
#include "utils/camera3d.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

namespace {
/*
 * base layer: status bar/launcher/navigation bar
 */
constexpr uint32_t BASE_LAYER_NUM = 3;
}

class HelloDrawing : public IBufferConsumerListenerClazz {
public:
    void Init(uint32_t width, uint32_t height, HdiBackend* backend);
    void DoDrawData(void *image, uint32_t width, uint32_t height);
    void DoDrawBaseData(void *image, uint32_t width, uint32_t height, uint32_t index);
    void Draw();
    void DrawSurface();
    void Sync(int64_t, void *data);
    void CreatePyhsicalScreen();
    void DoPrepareCompleted(sptr<Surface>& surface, const struct PrepareCompleteParam &param);
    void OnBufferAvailable() override;
    SurfaceError ProduceBuffer(sptr<Surface> &produceSurface, uint32_t width,
                               uint32_t height, uint32_t index, bool baseLayer);
    bool FillBaseLayer(std::shared_ptr<HdiLayerInfo> &showLayer, uint32_t index, uint32_t zorder,
                       GraphicIRect &dstRect);
    bool DrawBaseLayer(std::vector<LayerInfoPtr> &layerVec);
    void CreateBaseSurface(uint32_t index);
    void OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected);

    void TestDrawPathPro(Canvas &canvas, uint32_t width, uint32_t height);
    void TestDrawImage(Canvas &canvas, uint32_t width, uint32_t height);

    uint32_t freq_ = 30;
    uint32_t width_ = 0;
    uint32_t height_ = 0;

    uint32_t display_w = 480;
    uint32_t display_h = 960;

    bool initDeviceFinished_ = false;
    bool deviceConnected_ = false;
    std::shared_ptr<HdiOutput> output = nullptr;

private:
    uint32_t currentModeIndex_ = 0;
    std::vector<GraphicDisplayModeInfo> displayModeInfos_;
    std::vector<uint32_t> baseWidthVec_;
    std::vector<uint32_t> baseHeightVec_;
    std::unique_ptr<HdiScreen> screen = nullptr;

    std::vector<sptr<Surface>> basePSurfaceVec_;
    std::vector<sptr<Surface>> baseCSurfaceVec_;

    HdiBackend* backend = nullptr;

    bool ready = false;
};

void HelloDrawing::OnBufferAvailable()
{
}

void HelloDrawing::TestDrawPathPro(Canvas &canvas, uint32_t width, uint32_t height)
{
    int len = 300;
    Point a(500, 500); // point position

    Point c;
    Point d;

    d.SetX(a.GetX() - len * std::sin(18.0f)); // degree is 18
    d.SetY(a.GetY() + len * std::cos(18.0f)); // degree is 18

    c.SetX(a.GetX() + len * std::sin(18.0f)); // degree is 18
    c.SetY(d.GetY());

    Point b;
    b.SetX(a.GetX() + (len / 2.0));
    b.SetY(a.GetY() + std::sqrt((c.GetX() - d.GetX()) * (c.GetX() - d.GetX()) + (len / 2.0) * (len / 2.0)));

    Point e;
    e.SetX(a.GetX() - (len / 2.0));
    e.SetY(b.GetY());

    Path path;
    path.MoveTo(a.GetX(), a.GetY());
    path.LineTo(b.GetX(), b.GetY());
    path.LineTo(c.GetX(), c.GetY());
    path.LineTo(d.GetX(), d.GetY());
    path.LineTo(e.GetX(), e.GetY());
    path.Close();

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(10); // The thickness of the pen is 10
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    canvas.AttachBrush(brush);

    canvas.DrawPath(path);
}

void HelloDrawing::TestDrawImage(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawImage");
    Bitmap bmp;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE};
    bmp.Build(300, 300, format); // bitmap width and height
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);

    Image image;
    image.BuildFromBitmap(bmp);
    int imageWidth = image.GetWidth();
    int imageHeight = image.GetHeight();
    LOGI("image width = %{public}d, image height = %{public}d", imageWidth, imageHeight);
    Matrix matrix;
    // Set matrix to rotate by degrees 45 about a pivot point at (0, 0).
    matrix.Rotate(45, 0, 0);
    auto e = ShaderEffect::CreateImageShader(image, TileMode::REPEAT, TileMode::MIRROR, SamplingOptions(), matrix);
    auto c = Drawing::ColorSpace::CreateRefImage(image);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetColor(pen.GetColor4f(), c);
    pen.SetWidth(10); // The thickness of the pen is 10
    pen.SetShaderEffect(e);
    canvas.AttachPen(pen);
    canvas.DrawImage(image, 500, 500, SamplingOptions()); // draw image at (500,500)

    LOGI("------- TestDrawImage");
}

void HelloDrawing::DoDrawData(void *image, uint32_t width, uint32_t height)
{
    Bitmap bitmap;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE};
    bitmap.Build(width, height, format);

    Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(Color::COLOR_WHITE);

    TestDrawImage(canvas, width, height);
    constexpr uint32_t stride = 4;
    int32_t addrSize = width * height * stride;
    auto ret = memcpy_s(image, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        LOGE("memcpy_s failed");
    }
}

void HelloDrawing::DoDrawBaseData(void *image, uint32_t width, uint32_t height, uint32_t index)
{
    Bitmap bitmap;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE};
    bitmap.Build(width, height, format);

    Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(Color::COLOR_RED);
    if (index == 1) {
        canvas.Clear(Color::COLOR_WHITE);
        TestDrawImage(canvas, width, height);
    }
    constexpr uint32_t stride = 4;
    int32_t addrSize = width * height * stride;
    auto ret = memcpy_s(image, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        LOGE("memcpy_s failed");
    }
}

SurfaceError HelloDrawing::ProduceBuffer(sptr<Surface> &produceSurface, uint32_t width,
                                         uint32_t height, uint32_t index, bool baseLayer)
{
    OHOS::sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = -1;
    BufferRequestConfig config = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = produceSurface->GetDefaultUsage(),
    };

    SurfaceError ret = produceSurface->RequestBuffer(buffer, releaseFence, config);
    if (ret != 0) {
        LOGE("RequestBuffer failed: %{public}s", SurfaceErrorStr(ret).c_str());
        return ret;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(100); // 100ms

    if (buffer == nullptr) {
        LOGE("%s: buffer is nullptr", __func__);
        return SURFACE_ERROR_NULLPTR;
    }

    LOGI("ProduceBuffer: width: %{public}d height:%{public}d stride:%{public}d size:%{public}d",
        buffer->GetWidth(), buffer->GetHeight(), buffer->GetBufferHandle()->stride, buffer->GetSize());

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    if (baseLayer) {
        DoDrawBaseData(addr, buffer->GetWidth(), buffer->GetHeight(), index);
    } else {
        DoDrawData(addr, buffer->GetWidth(), buffer->GetHeight());
    }

    BufferFlushConfig flushConfig = {
        .damage = {
            .w = width,
            .h = height,
        },
    };

    int32_t acquireFence = -1;
    ret = produceSurface->FlushBuffer(buffer, acquireFence, flushConfig);

    LOGI("Sync %{public}s", SurfaceErrorStr(ret).c_str());
    return SURFACE_ERROR_OK;
}

void HelloDrawing::Draw()
{
    static int32_t count = 0;

    do {
        std::vector<LayerInfoPtr> layerVec;
        layerVec.resize(BASE_LAYER_NUM);
        for (uint32_t i = 0; i < BASE_LAYER_NUM; i++) {
            std::shared_ptr<HdiLayerInfo> showLayer = HdiLayerInfo::CreateHdiLayerInfo();
            layerVec[i] = showLayer;
        }

        if (BASE_LAYER_NUM == 3) {
            if (!DrawBaseLayer(layerVec)) {
                continue;
            }
        }

        output->SetLayerInfo(layerVec);

        GraphicIRect damageRect;
        damageRect.x = 0; // Absolute coordinates, with offset
        damageRect.y = 0;
        damageRect.w = display_w;
        damageRect.h = display_h;
        std::vector<GraphicIRect> outputDamages;
        outputDamages.emplace_back(damageRect);
        output->SetOutputDamages(outputDamages);

        backend->Repaint(output);
        for (uint32_t i = 0; i < BASE_LAYER_NUM; i++) {
            int32_t releaseFence = -1;
            sptr<SyncFence> tempFence = new SyncFence(releaseFence);
            baseCSurfaceVec_[i]->ReleaseBuffer(baseCSurfaceVec_[i]->GetBuffer(), tempFence);
            tempFence->Wait(100); // 100 ms
        }
        
        count++;
#ifdef DEBUG_DUMP
        std::string result;
        output->Dump(result);
        LOGI("Dump layer result after ReleaseBuffer : %{public}s", result.c_str());
#endif
    } while (false);
}

bool HelloDrawing::DrawBaseLayer(std::vector<LayerInfoPtr> &layerVec)
{
    // status bar
    int32_t zorder = 1;
    GraphicIRect dstRect;
    dstRect.x = 0;  // Absolute coordinates, with offset
    dstRect.y = 0;
    dstRect.w = baseWidthVec_[0];
    dstRect.h = baseHeightVec_[0];
    if (!FillBaseLayer(layerVec[0], 0, zorder, dstRect)) {
        return false;
    }

    // launcher
    zorder = 0;
    dstRect.x = 0;  // Absolute coordinates, with offset
    dstRect.y = 112;
    dstRect.w = baseWidthVec_[1];
    dstRect.h = baseHeightVec_[1];
    if (!FillBaseLayer(layerVec[1], 1, zorder, dstRect)) {
        return false;
    }

    // navigation bar
    zorder = 1;
    dstRect.x = 0;  // Absolute coordinates, with offset
    dstRect.y = 1488;
    dstRect.w = baseWidthVec_[2];
    dstRect.h = baseHeightVec_[2];
    if (!FillBaseLayer(layerVec[2], 2, zorder, dstRect)) {
        return false;
    }

    return true;
}

bool HelloDrawing::FillBaseLayer(std::shared_ptr<HdiLayerInfo> &showLayer, uint32_t index,
                                 uint32_t zorder, GraphicIRect &dstRect)
{
    sptr<Surface> pSurface = basePSurfaceVec_[index];
    sptr<Surface> cSurface = baseCSurfaceVec_[index];

    if (ProduceBuffer(pSurface, dstRect.w, dstRect.h, index, true) != SURFACE_ERROR_OK) {
        LOGE("Produce cBuffer failed");
        return false;
    }

    OHOS::sptr<SurfaceBuffer> cbuffer = nullptr;
    int32_t fence;
    int64_t timestamp;
    OHOS::Rect damage;
    SurfaceError ret = cSurface->AcquireBuffer(cbuffer, fence, timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        LOGE("Acquire cBuffer failed");
        return false;
    }

    GraphicIRect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = dstRect.w;
    srcRect.h = dstRect.h;

    GraphicLayerAlpha alpha = { .enPixelAlpha = true };

    showLayer->SetSurface(cSurface);
    showLayer->SetBuffer(cbuffer, fence);
    showLayer->SetZorder(zorder);
    showLayer->SetAlpha(alpha);
    // if rotate is not null, SetTransform
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

    return true;
}

void HelloDrawing::Sync(int64_t, void *data)
{
    struct OHOS::FrameCallback cb = {
        .frequency_ = freq_,
        .timestamp_ = 0,
        .userdata_ = data,
        .callback_ = std::bind(&HelloDrawing::Sync, this, SYNC_FUNC_ARG),
    };

    OHOS::VsyncError ret = OHOS::VsyncHelper::Current()->RequestFrameCallback(cb);
    if (ret) {
        LOGE("RequestFrameCallback inner %{public}d\n", ret);
    }

    if (!ready) {
        return;
    }

    Draw();
}

void HelloDrawing::Init(uint32_t width, uint32_t height, HdiBackend* backend)
{
    this->backend = backend;

    baseWidthVec_.resize(BASE_LAYER_NUM);
    baseHeightVec_.resize(BASE_LAYER_NUM);
    baseCSurfaceVec_.resize(BASE_LAYER_NUM);
    basePSurfaceVec_.resize(BASE_LAYER_NUM);

    if (BASE_LAYER_NUM == 3) {
        baseWidthVec_[0] = 2560;
        baseWidthVec_[1] = 2560;
        baseWidthVec_[2] = 2560;
        baseHeightVec_[0] = 112;
        baseHeightVec_[1] = 1376;
        baseHeightVec_[2] = 112;
    }

    for (uint32_t i = 0; i < BASE_LAYER_NUM; i++) {
        CreateBaseSurface(i);
    }
    Sync(0, nullptr);
}

void HelloDrawing::CreateBaseSurface(uint32_t index)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    cSurface->SetDefaultWidthAndHeight(baseWidthVec_[index], baseHeightVec_[index]);
    cSurface->SetDefaultUsage(BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);

    sptr<IBufferProducer> producer = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    cSurface->RegisterConsumerListener(this);

    baseCSurfaceVec_[index] = cSurface;
    basePSurfaceVec_[index] = pSurface;
}

void HelloDrawing::CreatePyhsicalScreen()
{
    screen = HdiScreen::CreateHdiScreen(output->GetScreenId());
    screen->Init();
    screen->GetScreenSupportedModes(displayModeInfos_);
    size_t supportModeNum = displayModeInfos_.size();
    if (supportModeNum > 0) {
        screen->GetScreenMode(currentModeIndex_);
        LOGI("currentModeIndex:%{public}d", currentModeIndex_);
        for (size_t i = 0; i < supportModeNum; i++) {
            LOGI("modes(%{public}d) %{public}dx%{public}d freq:%{public}d",
                 displayModeInfos_[i].id, displayModeInfos_[i].width,
                 displayModeInfos_[i].height, displayModeInfos_[i].freshRate);
            if (displayModeInfos_[i].id == static_cast<int32_t>(currentModeIndex_)) {
                this->freq_ = displayModeInfos_[i].freshRate;
                this->display_w = displayModeInfos_[i].width;
                this->display_h = displayModeInfos_[i].height;
                break;
            }
        }
        screen->SetScreenPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON);
        screen->SetScreenMode(currentModeIndex_);
        LOGI("SetScreenMode: currentModeIndex(%{public}d)", currentModeIndex_);

        GraphicDispPowerStatus powerState;
        screen->SetScreenPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON);
        screen->GetScreenPowerStatus(powerState);
        LOGI("get poweState:%{public}d", powerState);
    }

    GraphicDisplayCapability info;
    screen->GetScreenCapability(info);
    LOGI("GetScreenCapability: name(%{public}s), type(%{public}d), phyWidth(%{public}d), phyHeight(%{public}d)",
         info.name, info.type, info.phyWidth, info.phyHeight);
    LOGI("GetScreenCapability: supportLayers(%{public}d), virtualDispCount(%{public}d)",
         info.supportLayers, info.virtualDispCount);
    LOGI("GetScreenCapability: supportWriteBack(%{public}d), propertyCount(%{public}d)",
         info.supportWriteBack, info.propertyCount);
    ready = true;
}

static void OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data)
{
    LOGI("enter OnScreenPlug, connected is %{public}d", connected);
    auto* thisPtr = static_cast<HelloDrawing *>(data);
    thisPtr->OnHotPlugEvent(output, connected);
}

void HelloDrawing::OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected)
{
    /*
     * Currently, IPC communication cannot be nested. Therefore, Vblank registration can be
     * initiated only after the initialization of the device is complete.
     */
    this->output = output;
    deviceConnected_ = connected;

    if (!initDeviceFinished_) {
        LOGI("Init the device has not finished yet");
        return;
    }

    LOGI("Callback HotPlugEvent, connected is %{public}u", connected);

    if (connected) {
        CreatePyhsicalScreen();
    }
}

void HelloDrawing::DoPrepareCompleted(sptr<Surface>& surface, const struct PrepareCompleteParam &param)
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
        LOGE("surface is null");
        return;
    }

    int32_t releaseFence = -1;
    sptr<SurfaceBuffer> fbBuffer = nullptr;
    SurfaceError ret = surface->RequestBuffer(fbBuffer, releaseFence, requestConfig);
    if (ret != 0) {
        LOGE("RequestBuffer failed: %{public}s", SurfaceErrorStr(ret).c_str());
        return;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(100); // 100ms

    uint32_t clientCount = 0;
    bool hasClient = false;
    const std::vector<LayerInfoPtr> &layers = param.layers;
    for (const LayerInfoPtr &layer : layers) {
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT) {
            hasClient = true;
            clientCount++;
        }
    }

    LOGI("fb fence is %{public}d, clientCount is %{public}d", releaseFence, clientCount);

    auto addr = static_cast<uint8_t *>(fbBuffer->GetVirAddr());
    if (hasClient) {
        DoDrawData(addr, fbBuffer->GetWidth(), fbBuffer->GetHeight());
    } else {
        int32_t ret = memset_s(addr, fbBuffer->GetSize(), 0, fbBuffer->GetSize());
        if (ret != 0) {
            LOGE("memset_s failed");
        }
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
        LOGE("FlushBuffer failed: %{public}s", SurfaceErrorStr(ret).c_str());
    }
}

static void OnPrepareCompleted(
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

    LOGI("OnPrepareCompleted param.layer size is %{public}d", param.layers.size());
    auto* thisPtr = static_cast<HelloDrawing *>(data);
    thisPtr->DoPrepareCompleted(surface, param);
}

int32_t main(int32_t argc, const char *argv[])
{
    HelloDrawing m;

    LOGI("start to HdiBackend::GetInstance");
    HdiBackend* backend = OHOS::Rosen::HdiBackend::GetInstance();
    if (backend == nullptr) {
        LOGE("HdiBackend::GetInstance fail");
        return -1;
    }

    backend->RegScreenHotplug(OnScreenPlug, &m);
    while (1) {
        if (m.output != nullptr) {
            break;
        }
    }

    if (!m.initDeviceFinished_) {
        if (m.deviceConnected_) {
            m.CreatePyhsicalScreen();
        }
        m.initDeviceFinished_ = true;
    }
    LOGI("Init screen succeed");

    backend->RegPrepareComplete(OnPrepareCompleted, &m);

    m.width_ = 480; // display width
    m.height_ = 960; // display height
    sleep(1); // wait 1s

    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&HelloDrawing::Init, &m, m.width_, m.height_, backend));
    runner->Run();
    return 0;
}
