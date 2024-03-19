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

#include <vsync_generator.h>
#include <vsync_controller.h>
#include <vsync_distributor.h>
#include <vsync_receiver.h>
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

sptr<VSyncReceiver> g_receiver = nullptr;
}

void HelloComposer::Run(const std::vector<std::string> &runArgs)
{
    auto generator = CreateVSyncGenerator();
    sptr<VSyncController> vsyncController = new VSyncController(generator, 0);
    sptr<VSyncDistributor> vsyncDistributor = new VSyncDistributor(vsyncController, "HelloComposer");
    sptr<VSyncConnection> vsyncConnection = new VSyncConnection(vsyncDistributor, "HelloComposer");
    vsyncDistributor->AddConnection(vsyncConnection);

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
            CreateLayers();
        }
        initDeviceFinished_ = true;
    }
    LOGI("Init screen succeed");

    backend_->RegPrepareComplete(HelloComposer::OnPrepareCompleted, this);

    ParseArgs(runArgs);

    sleep(1);
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::Create(false);
    mainThreadHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    g_receiver = new VSyncReceiver(vsyncConnection, nullptr, mainThreadHandler_);
    g_receiver->Init();
    mainThreadHandler_->PostTask(std::bind(&HelloComposer::RequestSync, this));
    runner->Run();
}

void HelloComposer::ParseArgs(const std::vector<std::string> &runArgs)
{
    for (const std::string &arg : runArgs) {
        if (arg == "--dump") {
            dump_ = true;
        } else if (arg == "--testClient") {
            testClient_ = true;
        } else if (arg == "--testLayerRotate") {
            testLayerRotate_ = true;
        } else if (arg == "--YUV") {
            yuvFormat_ = true;
        } else if (arg == "--testLayerColor") {
            testLayerColor_ = true;
        }
    }
}

void HelloComposer::OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data)
{
    LOGI("enter OnScreenPlug, connected is %{public}d", connected);
    auto* thisPtr = static_cast<HelloComposer *>(data);
    thisPtr->OnHotPlugEvent(output, connected);
}

void HelloComposer::OnPrepareCompleted(
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

    auto* thisPtr = static_cast<HelloComposer *>(data);
    thisPtr->DoPrepareCompleted(surface, param);
}

void HelloComposer::CreateLayers()
{
    for (auto iter = outputMap_.begin(); iter != outputMap_.end(); ++iter) {
        currScreenId_ = iter->first;
        CreateShowLayers();
    }
}

void HelloComposer::CreateShowLayers()
{
    if (screensMap_.count(currScreenId_) != 0) {
        LOGI("Screen[%{public}d] has already created", currScreenId_);
        return;
    }

    uint32_t screenId = CreatePhysicalScreen();

    LOGI("Create screen[%{public}d], and created a total of %{public}zu screens", screenId, screensMap_.size());

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
        GraphicIRect { 0, 0, displayWidth, statusHeight },
        GraphicIRect { 0, 0, displayWidth, statusHeight },
        1, LayerType::LAYER_STATUS));

    // launcher
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        GraphicIRect { 0, statusHeight, displayWidth, launcherHeight },
        GraphicIRect { 0, 0, displayWidth, launcherHeight },
        0, LayerType::LAYER_LAUNCHER));

    // navigation bar
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        GraphicIRect { 0, navigationY, displayWidth, statusHeight },
        GraphicIRect { 0, 0, displayWidth, statusHeight },
        1, LayerType::LAYER_NAVIGATION));

    uint32_t layerWidth = displayWidth / 4; // layer width is 1/4 displayWidth
    uint32_t layerHeight = displayHeight / 4; // layer height is 1/4 of displayHeight
    uint32_t layerPositionX = displayWidth / 2 - layerWidth / 2; // x is (displayWidth - layerWidth) / 2
    uint32_t layerPositionY = displayHeight / 2 - layerHeight / 2; // y is (displayHeight - layerHeight) / 2
    LOGI("Layer position is: [x, y, w, h: [%{public}d, %{public}d, %{public}d, %{public}d]]",
          layerPositionX, layerPositionY, layerWidth, layerHeight);

    // extra layer 1
    drawLayers.emplace_back(std::make_unique<LayerContext>(
        GraphicIRect { layerPositionX, layerPositionY, layerWidth, layerHeight },
        GraphicIRect { 0, 0, layerWidth, layerHeight },
        1, LayerType::LAYER_EXTRA)); // 2 is zorder
}

void HelloComposer::Sync(int64_t, void *data)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = data,
        .callback_ = std::bind(&HelloComposer::Sync, this, ::std::placeholders::_1, ::std::placeholders::_2),
    };

    if (g_receiver != nullptr) {
        g_receiver->RequestNextVSync(fcb);
    }

    if (!ready_) {
        LOGE("hdi screen is not ready");
        return;
    }

    Draw();
}

void HelloComposer::SetRunArgs(const std::unique_ptr<LayerContext> &drawLayer) const
{
    LayerType type = drawLayer->GetLayerType();
    if (testLayerColor_) {
        drawLayer->SetTestLayerColor(true);
    }
    
    if (type < LayerType::LAYER_EXTRA) {
        return;
    }

    if (testClient_) {
        drawLayer->SetTestClientStatus(true);
    }

    if (testLayerRotate_) {
        drawLayer->SetTestRotateStatus(true);
    }

    if (yuvFormat_) {
        drawLayer->SetTestYUVStatus(true);
    }
}

void HelloComposer::Draw()
{
    for (auto iter = drawLayersMap_.begin(); iter != drawLayersMap_.end(); ++iter) {
        uint32_t screenId = iter->first;
        std::vector<std::unique_ptr<LayerContext>> &drawLayers = drawLayersMap_[screenId];
        std::vector<LayerInfoPtr> layerVec;
        for (auto &drawLayer : drawLayers) { // producer
            SetRunArgs(drawLayer);
            drawLayer->DrawBufferColor();
        }

        for (auto &drawLayer : drawLayers) { // consumer
            drawLayer->FillHDIBuffer();
            drawLayer->FillHDILayer();
            layerVec.emplace_back(drawLayer->GetHdiLayer());
        }

        curOutput_ = outputMap_[screenId];
        curOutput_->SetLayerInfo(layerVec);

        GraphicIRect damageRect;
        damageRect.x = 0;
        damageRect.y = 0;
        damageRect.w = static_cast<int32_t>(displayWidthsMap_[screenId]);
        damageRect.h = static_cast<int32_t>(displayHeightsMap_[screenId]);
        std::vector<GraphicIRect> outputDamages;
        outputDamages.emplace_back(damageRect);
        curOutput_->SetOutputDamages(outputDamages);

        if (dump_) {
            std::string result;
            curOutput_->Dump(result);
            LOGI("Dump layer result after ReleaseBuffer : %{public}s", result.c_str());
        }

        backend_->Repaint(curOutput_);
        auto layersReleaseFence = curOutput_->GetLayersReleaseFence();
        for (auto& layerContext : drawLayers) {
            auto preBuffer = layerContext->GetPreBuffer();
            int32_t releaseFence = -1;
            sptr<SyncFence> tempFence = new SyncFence(releaseFence);
            layerContext->GetHdiLayer()->GetSurface()->ReleaseBuffer(preBuffer, tempFence);
            tempFence->Wait(100); // 100 ms
        }
    }
}

uint32_t HelloComposer::CreatePhysicalScreen()
{
    std::vector<GraphicDisplayModeInfo> displayModeInfos;
    uint32_t screenId = currScreenId_;
    std::unique_ptr<HdiScreen> screen = HdiScreen::CreateHdiScreen(screenId);
    screen->Init();
    screen->GetScreenSupportedModes(displayModeInfos);
    size_t supportModeNum = displayModeInfos.size();
    if (supportModeNum > 0) {
        uint32_t currentModeIndex = 0;
        screen->GetScreenMode(currentModeIndex);
        LOGI("currentModeIndex:%{public}d", currentModeIndex);
        for (size_t i = 0; i < supportModeNum; i++) {
            LOGI("modes(%{public}d) %{public}dx%{public}d freq:%{public}d",
                displayModeInfos[i].id, displayModeInfos[i].width,
                displayModeInfos[i].height, displayModeInfos[i].freshRate);
            if (displayModeInfos[i].id == static_cast<int32_t>(currentModeIndex)) {
                freq_ = 30; // 30 freq
                displayWidthsMap_[screenId] = static_cast<uint32_t>(displayModeInfos[i].width);
                displayHeightsMap_[screenId] = static_cast<uint32_t>(displayModeInfos[i].height);
                break;
            }
        }
        screen->SetScreenPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON);
        screen->SetScreenMode(currentModeIndex);
        LOGI("SetScreenMode: currentModeIndex(%{public}d)", currentModeIndex);

        GraphicDispPowerStatus powerState;
        screen->GetScreenPowerStatus(powerState);
        LOGI("get poweState:%{public}d", powerState);
    }

    GraphicDisplayCapability info;
    screen->GetScreenCapability(info);
    LOGI("ScreenCapability: name(%{public}s), type(%{public}d), phyWidth(%{public}d), "
         "phyHeight(%{public}d)", info.name.c_str(), info.type, info.phyWidth, info.phyHeight);
    LOGI("ScreenCapability: supportLayers(%{public}d), virtualDispCount(%{public}d), "
         "supportWriteBack(%{public}d), propertyCount(%{public}d)", info.supportLayers,
         info.virtualDispCount, info.supportWriteBack, info.propertyCount);

    ready_ = true;

    screensMap_[screenId] = std::move(screen);

    LOGI("CreatePhysicalScreen, screenId is %{public}d", screenId);

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

    if (!connected) {
        RemoveOffScreenData(currScreenId_);
    }

    if (!initDeviceFinished_) {
        LOGI("Init the device has not finished yet");
        return;
    }

    LOGI("Callback HotPlugEvent, screenId is %{public}d, connected is %{public}u", currScreenId_, connected);

    if (connected) {
        CreateShowLayers();
    }
}

void HelloComposer::RemoveOffScreenData(uint32_t offScreenId)
{
    LOGI("Screen[%{public}d] is unplugged, and remove data", offScreenId);

    auto widthIter = displayWidthsMap_.begin();
    while (widthIter != displayWidthsMap_.end()) {
        uint32_t screenId = widthIter->first;
        if (screenId == offScreenId) {
            displayWidthsMap_.erase(widthIter++);
        } else {
            ++widthIter;
        }
    }

    auto heightIter = displayHeightsMap_.begin();
    while (heightIter != displayHeightsMap_.end()) {
        uint32_t screenId = heightIter->first;
        if (screenId == offScreenId) {
            displayHeightsMap_.erase(heightIter++);
        } else {
            ++heightIter;
        }
    }

    auto outputIter = outputMap_.begin();
    while (outputIter != outputMap_.end()) {
        uint32_t screenId = outputIter->first;
        if (screenId == offScreenId) {
            outputMap_.erase(outputIter++);
        } else {
            ++outputIter;
        }
    }

    auto screenIter = screensMap_.begin();
    while (screenIter != screensMap_.end()) {
        uint32_t screenId = screenIter->first;
        if (screenId == offScreenId) {
            screensMap_.erase(screenIter++);
        } else {
            ++screenIter;
        }
    }

    auto layerIter = drawLayersMap_.begin();
    while (layerIter != drawLayersMap_.end()) {
        uint32_t screenId = layerIter->first;
        if (screenId == offScreenId) {
            drawLayersMap_.erase(layerIter++);
        } else {
            ++layerIter;
        }
    }
}

namespace {
void DrawFrameBufferData(void *image, uint32_t width, uint32_t height)
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
}

void HelloComposer::DoPrepareCompleted(sptr<Surface> surface, const struct PrepareCompleteParam &param)
{
    uint32_t screenId = curOutput_->GetScreenId();
    uint32_t displayWidth = displayWidthsMap_[screenId];
    uint32_t displayHeight = displayHeightsMap_[screenId];

    BufferRequestConfig requestConfig = {
        .width = displayWidth,  // need display width
        .height = displayHeight, // need display height
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_BGRA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
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

    bool hasClient = false;
    const std::vector<LayerInfoPtr> &layers = param.layers;
    for (const LayerInfoPtr &layer : layers) {
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT) {
            hasClient = true;
        }
    }

    auto addr = static_cast<uint8_t *>(fbBuffer->GetVirAddr());
    if (hasClient) {
        DrawFrameBufferData(addr, static_cast<uint32_t>(fbBuffer->GetWidth()),
            static_cast<uint32_t>(fbBuffer->GetHeight()));
    } else {
        int32_t memsetRet = memset_s(addr, fbBuffer->GetSize(), 0, fbBuffer->GetSize());
        if (memsetRet != 0) {
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
