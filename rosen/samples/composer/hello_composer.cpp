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
        if (!outputMap_.empty()) {
            break;
        }
    }

    if (!initDeviceFinished_) {
        if (deviceConnected_) {
            CreateShowLayers();
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

    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::Create(false);
    mainThreadHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    mainThreadHandler_->PostTask(std::bind(&HelloComposer::RequestSync, this));
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

    if (surface == nullptr) {
        LOGE("surface is null");
        return;
    }

    if (data == nullptr) {
        LOGE("data ptr is null");
        return;
    }

    auto* thisPtr = static_cast<HelloComposer *>(data);
    thisPtr->DoPrepareCompleted(surface, param);
}

void HelloComposer::CreateShowLayers()
{
    uint32_t screenId = CreatePhysicalScreen();

    LOGI("Create %{public}zu screens", screens_.size());

    InitLayers(screenId);
}

void HelloComposer::RequestSync()
{
    Sync(0, nullptr);
}

void HelloComposer::InitLayers(uint32_t screenId)
{
    LOGI("Init layers, screenId is %{public}d", screenId);
    uint32_t displayWidth = displayWidthsMap_[screenId];
    uint32_t displayHeight = displayHeightsMap_[screenId];

    std::vector<std::unique_ptr<LayerContext>> &drawLayers = drawLayersMap_[screenId];

    uint32_t statusHeight = displayHeight / 10; // statusHeight is 1 / 10 displayHeight
    uint32_t launcherHeight = displayHeight - statusHeight * 2; // index 1, cal launcher height 2
    uint32_t navigationY = displayHeight - statusHeight;
    LOGI("displayWidth[%{public}d], displayHeight[%{public}d], statusHeight[%{public}d], "
         "launcherHeight[%{public}d], navigationY[%{public}d]", displayWidth, displayHeight,
         statusHeight, launcherHeight, navigationY);

    // status bar
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        IRect { 0, 0, displayWidth, statusHeight },
        IRect { 0, 0, displayWidth, statusHeight },
        1, LayerType::LAYER_STATUS));

    // launcher
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        IRect { 0, statusHeight, displayWidth, launcherHeight },
        IRect { 0, 0, displayWidth, launcherHeight },
        0, LayerType::LAYER_LAUNCHER));

    // navigation bar
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        IRect { 0, navigationY, displayWidth, statusHeight },
        IRect { 0, 0, displayWidth, statusHeight },
        1, LayerType::LAYER_NAVIGATION));

    uint32_t extraLayerWidth = displayWidth / 4; // layer width is 1 / 4 displayWidth
    uint32_t extraLayerHeight = displayHeight / 4; // layer height is 1 / 4 of displayHeight
    LOGI("extraLayerWidth[%{public}d], extraLayerHeight[%{public}d]", extraLayerWidth, extraLayerHeight);

    // extra layer 1
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        IRect { 300, 300, extraLayerWidth, extraLayerHeight }, // 300 is position
        IRect { 0, 0, extraLayerWidth, extraLayerHeight },
        1, LayerType::LAYER_EXTRA)); // 2 is zorder
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
    for (auto iter = drawLayersMap_.begin(); iter != drawLayersMap_.end(); ++iter) {
        std::vector<std::shared_ptr<HdiOutput>> outputs;
        uint32_t screenId = iter->first;
        std::vector<std::unique_ptr<LayerContext>> &drawLayers = drawLayersMap_[screenId];
        std::vector<LayerInfoPtr> layerVec;
        for (auto &drawLayer : drawLayers) { // producer
            drawLayer->DrawBufferColor();
        }

        for (auto &drawLayer : drawLayers) { // consumer
            drawLayer->FillHDILayer();
            layerVec.emplace_back(drawLayer->GetHdiLayer());
        }

        curOutput_ = outputMap_[screenId];
        outputs.emplace_back(curOutput_);
        curOutput_->SetLayerInfo(layerVec);

        IRect damageRect;
        damageRect.x = 0;
        damageRect.y = 0;
        damageRect.w = displayWidthsMap_[screenId];
        damageRect.h = displayHeightsMap_[screenId];
        curOutput_->SetOutputDamage(1, damageRect);

        if (dump_) {
            std::string result;
            curOutput_->Dump(result);
            LOGI("Dump layer result after ReleaseBuffer : %{public}s", result.c_str());
        }

        backend_->Repaint(outputs);
    }
}

uint32_t HelloComposer::CreatePhysicalScreen()
{
    uint32_t screenId = currScreenId_;
    std::unique_ptr<HdiScreen> screen = HdiScreen::CreateHdiScreen(screenId);
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
                freq_ = 30; // 30 freq
                displayWidthsMap_[screenId] = displayModeInfos_[i].width;
                displayHeightsMap_[screenId] = displayModeInfos_[i].height;
                break;
            }
        }
        screen->SetScreenPowerStatus(DispPowerStatus::POWER_STATUS_ON);
        screen->SetScreenMode(currentModeIndex_);
        LOGI("SetScreenMode: currentModeIndex(%{public}d)", currentModeIndex_);

        DispPowerStatus powerState;
        screen->GetScreenPowerStatus(powerState);
        LOGI("get poweState:%{public}d", powerState);
    }

    DisplayCapability info;
    screen->GetScreenCapability(info);
    LOGI("ScreenCapability: name(%{public}s), type(%{public}d), phyWidth(%{public}d), "
         "phyHeight(%{public}d)", info.name, info.type, info.phyWidth, info.phyHeight);
    LOGI("ScreenCapability: supportLayers(%{public}d), virtualDispCount(%{public}d), "
         "supportWriteBack(%{public}d), propertyCount(%{public}d)", info.supportLayers,
         info.virtualDispCount, info.supportWriteBack, info.propertyCount);

    ready_ = true;

    screens_.emplace_back(std::move(screen));

    LOGE("CreatePhysicalScreen, screenId is %{public}d", screenId);

    return screenId;
}

void HelloComposer::OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected)
{
    if (mainThreadHandler_ == nullptr) {
        LOGI("In main thread, call OnHotPlug directly");
        OnHotPlug(output, connected);
    } else {
        LOGI("In sub thread, post msg to main thread");
        mainThreadHandler_->PostTask(std::bind(&HelloComposer::OnHotPlug, this, output, connected));
    }
}

void HelloComposer::OnHotPlug(std::shared_ptr<HdiOutput> &output, bool connected)
{
    /*
     * Currently, IPC communication cannot be nested. Therefore, Vblank registration can be
     * initiated only after the initialization of the device is complete.
     */
    currScreenId_ = output->GetScreenId();
    outputMap_[currScreenId_] = output;
    deviceConnected_ = connected;

    if (!initDeviceFinished_) {
        LOGI("Init the device has not finished yet");
        return;
    }

    LOGI("Callback HotPlugEvent, connected is %{public}u", connected);

    if (connected) {
        CreateShowLayers();
    }
}

void HelloComposer::DoPrepareCompleted(sptr<Surface> &surface, const struct PrepareCompleteParam &param)
{
    uint32_t screenId = curOutput_->GetScreenId();
    uint32_t displayWidth = displayWidthsMap_[screenId];
    uint32_t displayHeight = displayHeightsMap_[screenId];

    BufferRequestConfig requestConfig = {
        .width = displayWidth,  // need display width
        .height = displayHeight, // need display height
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_BGRA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };

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
            .w = displayWidth,
            .h = displayHeight,
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