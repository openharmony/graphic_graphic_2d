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

#include "hello_composer.h"

#include <securec.h>
#include <sync_fence.h>
#include <vsync_helper.h>

using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
#define LOGI(fmt, ...) ::OHOS::HiviewDFX::HiLog::Info(            \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloComposer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define LOGE(fmt, ...) ::OHOS::HiviewDFX::HiLog::Error(           \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloComposer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
}

void HelloComposer::Run(std::vector<std::string> &runArgs)
{
    LOGI("start to run hello composer");
    backend_ = OHOS::Rosen::HdiBackend::GetInstance();
    if (backend_ == nullptr) {
        LOGE("HdiBackend::GetInstance fail");
        return;
    }

    backend_->RegScreenHotplug(HelloComposer::OnScreenPlug, this);
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
    LOGI("Init screen succeed");

    backend_->RegPrepareComplete(HelloComposer::OnPrepareCompleted, this);

    for (std::string &arg : runArgs) {
        if (arg == "--dump") {
            dump_ = true;
        }
    }

    sleep(1);

    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&HelloComposer::Init, this));
    runner->Run();
}

void HelloComposer::OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data)
{
    LOGI("enter OnScreenPlug, connected is %{public}d", connected);
    auto* thisPtr = static_cast<HelloComposer *>(data);
    thisPtr->OnHotPlugEvent(output, connected);
}

void HelloComposer::OnPrepareCompleted(sptr<Surface> &surface, const struct PrepareCompleteParam &param, void* data)
{
    if (!param.needFlushFramebuffer) {
        return;
    }

    if (data == nullptr) {
        return;
    }

    LOGI("OnPrepareCompleted param.layer size is %{public}zu", param.layers.size());
    auto* thisPtr = static_cast<HelloComposer *>(data);
    thisPtr->DoPrepareCompleted(surface, param);
}

void HelloComposer::Init()
{
    uint32_t statusHeight = displayHeight_ / 10; // statusHeight is 1 / 10 displayHeight
    uint32_t launcherHeight = displayHeight_ - statusHeight * 2; // index 1, cal launcher height 2
    uint32_t navigationY = displayHeight_ - statusHeight;
    LOGI("displayWidth[%{public}d], displayHeight[%{public}d], statusHeight[%{public}d], "
         "launcherHeight[%{public}d], navigationY[%{public}d]", displayWidth_, displayHeight_,
         statusHeight, launcherHeight, navigationY);

    // status bar
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        IRect { 0, 0, displayWidth_, statusHeight },
        IRect { 0, 0, displayWidth_, statusHeight },
        1, LayerType::LAYER_STATUS));

    // launcher
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        IRect { 0, statusHeight, displayWidth_, launcherHeight },
        IRect { 0, 0, displayWidth_, launcherHeight },
        0, LayerType::LAYER_LAUNCHER));

    // navigation bar
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        IRect { 0, navigationY, displayWidth_, statusHeight },
        IRect { 0, 0, displayWidth_, statusHeight },
        1, LayerType::LAYER_NAVIGATION));

    uint32_t extraLayerWidth = displayWidth_ / 4; // layer width is 1 / 4 displayWidth
    uint32_t extraLayerHeight = displayHeight_ / 4; // layer height is 1 / 4 of displayHeight
    LOGI("extraLayerWidth[%{public}d], extraLayerHeight[%{public}d]", extraLayerWidth, extraLayerHeight);

    // extra layer 1
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        IRect { 300, 300, extraLayerWidth, extraLayerHeight }, // 300 is position
        IRect { 0, 0, extraLayerWidth, extraLayerHeight },
        1, LayerType::LAYER_EXTRA)); // 2 is zorder

    Sync(0, nullptr);
}

void HelloComposer::Sync(int64_t, void *data)
{
    struct OHOS::FrameCallback cb = {
        .frequency_ = freq_,
        .timestamp_ = 0,
        .userdata_ = data,
        .callback_ = std::bind(&HelloComposer::Sync, this, SYNC_FUNC_ARG),
    };

    OHOS::VsyncError ret = OHOS::VsyncHelper::Current()->RequestFrameCallback(cb);
    if (ret) {
        LOGE("RequestFrameCallback inner %{public}d\n", ret);
    }

    if (!ready_) {
        return;
    }

    Draw();
}

void HelloComposer::Draw()
{
    std::vector<LayerInfoPtr> layerVec;
    for (auto &drawLayer : drawLayers) { // producer
        drawLayer->DrawBufferColor();
    }

    for (auto &drawLayer : drawLayers) { // consumer
        drawLayer->FillHDILayer();
        layerVec.emplace_back(drawLayer->GetHdiLayer());
    }

    LOGI("Draw layer size %{public}zu", layerVec.size());
    output_->SetLayerInfo(layerVec);

    IRect damageRect;
    damageRect.x = 0;
    damageRect.y = 0;
    damageRect.w = displayWidth_;
    damageRect.h = displayHeight_;
    output_->SetOutputDamage(1, damageRect);

    backend_->Repaint(outputs_);

    if (dump_) {
        std::string result;
        output_->Dump(result);
        LOGI("Dump layer result after ReleaseBuffer : %{public}s", result.c_str());
    }
}

void HelloComposer::CreatePhysicalScreen()
{
    screen_ = HdiScreen::CreateHdiScreen(output_->GetScreenId());
    screen_->Init();
    screen_->GetScreenSuppportedModes(displayModeInfos_);
    outputs_.push_back(output_);
    size_t supportModeNum = displayModeInfos_.size();
    if (supportModeNum > 0) {
        screen_->GetScreenMode(currentModeIndex_);
        LOGI("currentModeIndex:%{public}d", currentModeIndex_);
        for (size_t i = 0; i < supportModeNum; i++) {
            LOGI("modes(%{public}d) %{public}dx%{public}d freq:%{public}d",
                displayModeInfos_[i].id, displayModeInfos_[i].width,
                displayModeInfos_[i].height, displayModeInfos_[i].freshRate);
            if (displayModeInfos_[i].id == static_cast<int32_t>(currentModeIndex_)) {
                freq_ = 30; // 30 freq
                displayWidth_ = displayModeInfos_[i].width;
                displayHeight_ = displayModeInfos_[i].height;
                break;
            }
        }
        screen_->SetScreenPowerStatus(DispPowerStatus::POWER_STATUS_ON);
        screen_->SetScreenMode(currentModeIndex_);
        LOGI("SetScreenMode: currentModeIndex(%{public}d)", currentModeIndex_);

        DispPowerStatus powerState;
        screen_->SetScreenPowerStatus(DispPowerStatus::POWER_STATUS_ON);
        screen_->GetScreenPowerStatus(powerState);
        LOGI("get poweState:%{public}d", powerState);
    }

    DisplayCapability info;
    screen_->GetScreenCapability(info);
    LOGI("ScreenCapability: name(%{public}s), type(%{public}d), phyWidth(%{public}d), "
         "phyHeight(%{public}d)", info.name, info.type, info.phyWidth, info.phyHeight);
    LOGI("ScreenCapability: supportLayers(%{public}d), virtualDispCount(%{public}d), "
         "supportWriteBack(%{public}d), propertyCount(%{public}d)", info.supportLayers,
         info.virtualDispCount, info.supportWriteBack, info.propertyCount);

    ready_ = true;
}

void HelloComposer::OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected)
{
    /*
     * Currently, IPC communication cannot be nested. Therefore, Vblank registration can be
     * initiated only after the initialization of the device is complete.
     */
    output_ = output;
    deviceConnected_ = connected;

    if (!initDeviceFinished_) {
        LOGI("Init the device has not finished yet");
        return;
    }

    LOGI("Callback HotPlugEvent, connected is %{public}u", connected);

    if (connected) {
        CreatePhysicalScreen();
    }
}

void HelloComposer::DoPrepareCompleted(sptr<Surface> &surface, const struct PrepareCompleteParam &param)
{
    BufferRequestConfig requestConfig = {
        .width = displayWidth_,  // need display width
        .height = displayHeight_, // need display height
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_BGRA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
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
    tempFence->Wait(100); // 100 ms

    uint32_t clientCount = 0;
    bool hasClient = false;
    const std::vector<LayerInfoPtr> &layers = param.layers;
    for (const LayerInfoPtr &layer : layers) {
        if (layer->GetCompositionType() == CompositionType::COMPOSITION_CLIENT) {
            hasClient = true;
            clientCount++;
        }
    }

    LOGI("fb fence is %{public}d, clientCount is %{public}d", releaseFence, clientCount);

    auto addr = static_cast<uint8_t *>(fbBuffer->GetVirAddr());
    if (hasClient) {
        DrawFrameBufferData(addr, fbBuffer->GetWidth(), fbBuffer->GetHeight());
    } else {
        int32_t ret = memset_s(addr, fbBuffer->GetSize(), 0, fbBuffer->GetSize());
        if (ret != 0) {
            LOGE("memset_s failed");
        }
    }

    BufferFlushConfig flushConfig = {
        .damage = {
            .w = displayWidth_,
            .h = displayHeight_,
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

void HelloComposer::DrawFrameBufferData(void *image, uint32_t width, uint32_t height)
{
    static uint32_t value = 0x00;
    value++;

    uint32_t *pixel = static_cast<uint32_t *>(image);
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0;  y < height; y++) {
            *pixel++ = value;
        }
    }
}