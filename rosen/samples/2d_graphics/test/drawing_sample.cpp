/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "drawing_sample.h"

#include <securec.h>
#include <sync_fence.h>
#include <vsync_controller.h>
#include <vsync_distributor.h>
#include <vsync_generator.h>
#include <vsync_receiver.h>

#include <platform/ohos/rs_surface_ohos.h>

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

namespace {
sptr<VSyncReceiver> g_receiver = nullptr;
}

void DrawingSample::Run()
{
    auto generator = CreateVSyncGenerator();
    sptr<VSyncController> vsyncController = new VSyncController(generator, 0);
    sptr<VSyncDistributor> vsyncDistributor = new VSyncDistributor(vsyncController, "DrawingSample");
    sptr<VSyncConnection> vsyncConnection = new VSyncConnection(vsyncDistributor, "DrawingSample");
    vsyncDistributor->AddConnection(vsyncConnection);

    LOGI("start to run drawing sample");
    backend_ = OHOS::Rosen::HdiBackend::GetInstance();
    if (backend_ == nullptr) {
        LOGE("HdiBackend::GetInstance fail");
        return;
    }

    backend_->RegScreenHotplug(DrawingSample::OnScreenPlug, this);
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

    backend_->RegPrepareComplete(DrawingSample::OnPrepareCompleted, this);

    sleep(1);
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::Create(false);
    mainThreadHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    g_receiver = new VSyncReceiver(vsyncConnection, mainThreadHandler_);
    g_receiver->Init();
    mainThreadHandler_->PostTask(std::bind(&DrawingSample::RequestSync, this));
    runner->Run();
}

void DrawingSample::OnScreenPlug(std::shared_ptr<HdiOutput>& output, bool connected, void* data)
{
    LOGI("enter OnScreenPlug, connected is %{public}d", connected);
    auto* thisPtr = static_cast<DrawingSample*>(data);
    thisPtr->OnHotPlugEvent(output, connected);
}

void DrawingSample::OnPrepareCompleted(
    std::shared_ptr<RSSurfaceOhos> &rsSurface, const struct PrepareCompleteParam& param, void* data)
{
    if (!param.needFlushFramebuffer) {
        return;
    }

    if (rsSurface == nullptr) {
        LOGE("surface is null");
        return;
    }

    if (data == nullptr) {
        LOGE("data ptr is null");
        return;
    }

    auto* thisPtr = static_cast<DrawingSample*>(data);
    thisPtr->DoPrepareCompleted(rsSurface->GetSurface(), param);
}

void DrawingSample::CreateShowLayers()
{
    uint32_t screenId = CreatePhysicalScreen();

    LOGI("Create %{public}zu screens", screens_.size());

    InitLayers(screenId);
}

void DrawingSample::RequestSync()
{
    Sync(0, nullptr);
}

void DrawingSample::InitLayers(uint32_t screenId)
{
    LOGI("Init layers, screenId is %{public}d", screenId);
    uint32_t displayWidth = displayWidthsMap_[screenId];
    uint32_t displayHeight = displayHeightsMap_[screenId];

    std::unique_ptr<LayerContext>& drawLayer = drawLayersMap_[screenId];
    // launcher
    drawLayer = std::make_unique<LayerContext>(IRect { 0, 0, displayWidth, displayHeight },
        IRect { 0, 0, displayWidth, displayHeight }, 0, LayerType::LAYER_LAUNCHER);
}

void DrawingSample::Sync(int64_t, void* data)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = data,
        .callback_ = std::bind(&DrawingSample::Sync, this, ::std::placeholders::_1, ::std::placeholders::_2),
    };
    if (g_receiver != nullptr) {
        g_receiver->RequestNextVSync(fcb);
    }
    if (!ready_) {
        return;
    }

    Draw();
}

void DrawingSample::Draw()
{
    for (auto iter = drawLayersMap_.begin(); iter != drawLayersMap_.end(); ++iter) {
        std::vector<std::shared_ptr<HdiOutput>> outputs;
        uint32_t screenId = iter->first;
        std::unique_ptr<LayerContext>& drawLayer = drawLayersMap_[screenId];

        for (auto iter = drawFuncMap_.begin(); iter != drawFuncMap_.end(); iter++) {
            std::cout << "-------------------------------------------------------\n";
            std::cout << "Drawing module " << iter->first << " start.\n";
            for (auto& func : iter->second) {
                drawLayer->DrawBuffer(func); // producer
                drawLayer->FillHDILayer();   // consumer
                std::vector<LayerInfoPtr> layerVec;
                layerVec.emplace_back(drawLayer->GetHdiLayer());
                curOutput_ = outputMap_[screenId];
                outputs.emplace_back(curOutput_);
                curOutput_->SetLayerInfo(layerVec);

                IRect damageRect;
                damageRect.x = 0;
                damageRect.y = 0;
                damageRect.w = static_cast<int32_t>(displayWidthsMap_[screenId]);
                damageRect.h = static_cast<int32_t>(displayHeightsMap_[screenId]);
                curOutput_->SetOutputDamage(1, damageRect);
                backend_->Repaint(outputs);
                sleep(2); // wait 2s
            }
            std::cout << "Drawing module " << iter->first << " end.\n";
        }
    }
}

uint32_t DrawingSample::CreatePhysicalScreen()
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
            LOGI("modes(%{public}d) %{public}dx%{public}d freq:%{public}d", displayModeInfos_[i].id,
                displayModeInfos_[i].width, displayModeInfos_[i].height, displayModeInfos_[i].freshRate);
            if (displayModeInfos_[i].id == static_cast<int32_t>(currentModeIndex_)) {
                freq_ = displayModeInfos_[i].freshRate; // 30 freq
                displayWidthsMap_[screenId] = static_cast<uint32_t>(displayModeInfos_[i].width);
                displayHeightsMap_[screenId] = static_cast<uint32_t>(displayModeInfos_[i].height);
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
         "phyHeight(%{public}d)",
        info.name, info.type, info.phyWidth, info.phyHeight);
    LOGI("ScreenCapability: supportLayers(%{public}d), virtualDispCount(%{public}d), "
         "supportWriteBack(%{public}d), propertyCount(%{public}d)",
        info.supportLayers, info.virtualDispCount, info.supportWriteBack, info.propertyCount);

    ready_ = true;

    screens_.emplace_back(std::move(screen));

    LOGE("CreatePhysicalScreen, screenId is %{public}d", screenId);

    return screenId;
}

void DrawingSample::OnHotPlugEvent(std::shared_ptr<HdiOutput>& output, bool connected)
{
    if (mainThreadHandler_ == nullptr) {
        LOGI("In main thread, call OnHotPlug directly");
        OnHotPlug(output, connected);
    } else {
        LOGI("In sub thread, post msg to main thread");
        mainThreadHandler_->PostTask(std::bind(&DrawingSample::OnHotPlug, this, output, connected));
    }
}

void DrawingSample::OnHotPlug(std::shared_ptr<HdiOutput>& output, bool connected)
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

void DrawingSample::DoPrepareCompleted(sptr<Surface> surface, const struct PrepareCompleteParam& param)
{
    uint32_t screenId = curOutput_->GetScreenId();
    uint32_t displayWidth = displayWidthsMap_[screenId];
    uint32_t displayHeight = displayHeightsMap_[screenId];

    BufferRequestConfig requestConfig = {
        .width = displayWidth,   // need display width
        .height = displayHeight, // need display height
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_BGRA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };

    int32_t releaseFence = -1;
    sptr<SurfaceBuffer> fbBuffer = nullptr;
    SurfaceError ret1 = surface->RequestBuffer(fbBuffer, releaseFence, requestConfig);
    if (ret1 != 0) {
        LOGE("RequestBuffer failed: %{public}s", SurfaceErrorStr(ret1).c_str());
        return;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(100); // 100 ms

    auto addr = static_cast<uint8_t*>(fbBuffer->GetVirAddr());
    int32_t ret2 = memset_s(addr, fbBuffer->GetSize(), 0, fbBuffer->GetSize());
    if (ret2 != 0) {
        LOGE("memset_s failed");
    }

    BufferFlushConfig flushConfig = { .damage = {
                                          .w = displayWidth,
                                          .h = displayHeight,
                                      } };

    /*
     * if use GPU produce data, flush with gpu fence
     */
    SurfaceError ret3 = surface->FlushBuffer(fbBuffer, -1, flushConfig);
    if (ret3 != 0) {
        LOGE("FlushBuffer failed: %{public}s", SurfaceErrorStr(ret3).c_str());
    }
}