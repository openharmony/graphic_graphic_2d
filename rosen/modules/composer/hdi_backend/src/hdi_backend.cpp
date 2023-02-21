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

#include "hdi_backend.h"

#include <scoped_bytrace.h>
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {

HdiBackend* HdiBackend::GetInstance()
{
    static HdiBackend instance;

    return &instance;
}

RosenError HdiBackend::RegScreenHotplug(OnScreenHotplugFunc func, void* data)
{
    if (func == nullptr) {
        HLOGE("OnScreenHotplugFunc is null");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    onScreenHotplugCb_ = func;
    onHotPlugCbData_ = data;

    return InitDevice();
}

RosenError HdiBackend::RegPrepareComplete(OnPrepareCompleteFunc func, void* data)
{
    if (func == nullptr) {
        HLOGE("OnPrepareCompleteFunc is null");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    onPrepareCompleteCb_ = func;
    onPrepareCompleteCbData_ = data;

    return ROSEN_ERROR_OK;
}

int32_t HdiBackend::PreProcessLayersComp(const OutputPtr &output,
                                         const std::unordered_map<uint32_t, LayerPtr> &layersMap, bool &needFlush)
{
    if (device_ == nullptr) {
        HLOGE("device has not been initialized");
        return DISPLAY_FAILURE;
    }

    uint32_t layersNum = layersMap.size();
    uint32_t layerCompCapacity = output->GetLayerCompCapacity();
    uint32_t screenId = output->GetScreenId();

    // If doClientCompositionDirectly is true then layer->SetHdiLayerInfo and UpdateLayerCompType is no need to run.
    bool doClientCompositionDirectly = ((layerCompCapacity != LAYER_COMPOSITION_CAPACITY_INVALID) &&
                                        (layersNum > layerCompCapacity));
    bool isDirectClientCompositionEnabled = output->GetDirectClientCompEnableStatus();
    if (!isDirectClientCompositionEnabled) {
        doClientCompositionDirectly = false;
    }
    int32_t ret;
    for (auto iter = layersMap.begin(); iter != layersMap.end(); ++iter) {
        const LayerPtr &layer = iter->second;
        if (doClientCompositionDirectly) {
            HLOGD("Direct client composition is enabled.");
            layer->UpdateCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
            continue;
        }
        ret = layer->SetHdiLayerInfo();
        if (ret != DISPLAY_SUCCESS) {
            HLOGE("Set hdi layer[id:%{public}d] info failed, ret %{public}d.", layer->GetLayerId(), ret);
        }
    }

    ret = device_->PrepareScreenLayers(screenId, needFlush);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("PrepareScreenLayers failed, ret is %{public}d", ret);
        return DISPLAY_FAILURE;
    }

    if (doClientCompositionDirectly) {
        ScopedBytrace doClientCompositionDirectlyTag("DoClientCompositionDirectly");
        return DISPLAY_SUCCESS;
    }

    return UpdateLayerCompType(screenId, layersMap);
}

void HdiBackend::Repaint(std::vector<OutputPtr> &outputs)
{
    ScopedBytrace bytrace(__func__);
    HLOGD("%{public}s: start", __func__);

    if (device_ == nullptr) {
        HLOGE("device has not been initialized");
        return;
    }

    if (sampler_ == nullptr) {
        sampler_ = CreateVSyncSampler();
    }

    int32_t ret = DISPLAY_SUCCESS;
    for (auto &output : outputs) {
        if (output == nullptr) {
            continue;
        }
        const std::unordered_map<uint32_t, LayerPtr> &layersMap = output->GetLayers();
        if (layersMap.empty()) {
            HLOGI("layer map is empty, drop this frame");
            continue;
        }
        bool needFlush = false;
        ret = PreProcessLayersComp(output, layersMap, needFlush);
        if (ret != DISPLAY_SUCCESS) {
            HLOGE("Pre process layers composition failed, ret = %{public}d.", ret);
            return;
        }

        uint32_t screenId = output->GetScreenId();
        std::vector<LayerPtr> compClientLayers;
        std::vector<LayerInfoPtr> newLayerInfos;
        for (auto iter = layersMap.begin(); iter != layersMap.end(); ++iter) {
            const LayerPtr &layer = iter->second;
            newLayerInfos.emplace_back(layer->GetLayerInfo());
            if (layer->GetLayerInfo()->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT) {
                compClientLayers.emplace_back(layer);
            }
        }

        if (compClientLayers.size() > 0) {
            needFlush = true;
            HLOGD("Need flush framebuffer, client composition layer num is %{public}zu", compClientLayers.size());
        }

        OnPrepareComplete(needFlush, output, newLayerInfos);
        sptr<SurfaceBuffer> frameBuffer = nullptr;
        if (needFlush) {
            if (FlushScreen(output, compClientLayers, frameBuffer) != DISPLAY_SUCCESS) {
                // return
            }
        }

        sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
        ret = device_->Commit(screenId, fbFence);
        if (ret != DISPLAY_SUCCESS) {
            HLOGE("commit failed, ret is %{public}d", ret);
            // return
        }

        output->UpdatePrevLayerInfo();
        int64_t timestamp = lastPresentFence_->SyncFileReadTimestamp();
        bool startSample = false;
        if (timestamp != SyncFence::FENCE_PENDING_TIMESTAMP) {
            startSample = sampler_->AddPresentFenceTime(timestamp);
            output->RecordCompositionTime(timestamp);
            for (auto iter = layersMap.begin(); iter != layersMap.end(); ++iter) {
                const LayerPtr &layer = iter->second;
                layer->RecordPresentTime(timestamp);
            }
        }
        if (startSample) {
            HLOGD("Enable Screen Vsync");
            device_->SetScreenVsyncEnabled(screenId, true);
            sampler_->BeginSample();
        }

        ReleaseFramebuffer(output, fbFence, frameBuffer);
        lastPresentFence_ = fbFence;
        HLOGD("%{public}s: end", __func__);
    }
}

int32_t HdiBackend::UpdateLayerCompType(uint32_t screenId, const std::unordered_map<uint32_t, LayerPtr> &layersMap)
{
    std::vector<uint32_t> layersId;
    std::vector<int32_t> types;
    int32_t ret = device_->GetScreenCompChange(screenId, layersId, types);
    if (ret != DISPLAY_SUCCESS || layersId.size() != types.size()) {
        HLOGE("GetScreenCompChange failed, ret is %{public}d", ret);
        return ret;
    }

    size_t layerNum = layersId.size();
    for (size_t i = 0; i < layerNum; i++) {
        auto iter = layersMap.find(layersId[i]);
        if (iter == layersMap.end()) {
            HLOGE("Invalid hdi layer id[%{public}u]", layersId[i]);
            continue;
        }

        const LayerPtr &layer = iter->second;
        layer->UpdateCompositionType(static_cast<GraphicCompositionType>(types[i]));
    }

    return ret;
}

void HdiBackend::OnPrepareComplete(bool needFlush, OutputPtr &output, std::vector<LayerInfoPtr> &newLayerInfos)
{
    if (needFlush) {
        ReorderLayerInfo(newLayerInfos);
    }

    struct PrepareCompleteParam param = {
        .needFlushFramebuffer = needFlush,
        .layers = newLayerInfos,
    };

    auto fbSurface = output->GetFrameBufferSurface();
    if (onPrepareCompleteCb_ != nullptr) {
        onPrepareCompleteCb_(fbSurface, param, onPrepareCompleteCbData_);
    }
}

static inline bool Cmp(const LayerInfoPtr &layer1, const LayerInfoPtr &layer2)
{
    return layer1->GetZorder() < layer2->GetZorder();
}

void HdiBackend::ReorderLayerInfo(std::vector<LayerInfoPtr> &newLayerInfos)
{
    std::sort(newLayerInfos.begin(), newLayerInfos.end(), Cmp);
}

int32_t HdiBackend::FlushScreen(
    const OutputPtr &output, std::vector<LayerPtr> &compClientLayers, sptr<SurfaceBuffer> &buffer)
{
    auto fbEntry = output->GetFramebuffer();
    if (fbEntry == nullptr) {
        HLOGE("HdiBackend::FlushScreen: GetFramebuffer failed!");
        return -1;
    }

    const auto& fbAcquireFence = fbEntry->acquireFence;
    for (auto &layer : compClientLayers) {
        layer->MergeWithFramebufferFence(fbAcquireFence);
    }

    buffer = fbEntry->buffer;
    return SetScreenClientInfo(*fbEntry, output);
}

void HdiBackend::ReleaseFramebuffer(
    const OutputPtr &output, sptr<SyncFence> &presentFence, const sptr<SurfaceBuffer> &buffer)
{
    if (buffer == nullptr) {
        return;
    }
    if (lastFrameBuffers_.find(output->GetScreenId()) != lastFrameBuffers_.end()) {
        // wrong check
        (void)output->ReleaseFramebuffer(lastFrameBuffers_[output->GetScreenId()], presentFence);
    }
    lastFrameBuffers_[output->GetScreenId()] = buffer;
}

int32_t HdiBackend::SetScreenClientInfo(const FrameBufferEntry &fbEntry, const OutputPtr &output)
{
    if (fbEntry.buffer == nullptr) {
        HLOGE("SetScreenClientBuffer failed: frame buffer is null");
        return -1;
    }

    int ret = device_->SetScreenClientBuffer(output->GetScreenId(),
        fbEntry.buffer->GetBufferHandle(), fbEntry.acquireFence);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("SetScreenClientBuffer failed, ret is %{public}d", ret);
        return ret;
    }

    ret = device_->SetScreenClientDamage(output->GetScreenId(), output->GetOutputDamageNum(),
                                         output->GetOutputDamage());
    if (ret != DISPLAY_SUCCESS) {
        HLOGD("SetScreenClientDamage failed, ret is %{public}d", ret);
        return ret;
    }

    return DISPLAY_SUCCESS;
}

std::map<LayerInfoPtr, sptr<SyncFence>> HdiBackend::GetLayersReleaseFence(const OutputPtr& output)
{
    if (output == nullptr) {
        return {};
    }
    uint32_t screenId = output->GetScreenId();
    std::vector<uint32_t> layersId;
    std::vector<sptr<SyncFence>> fences;
    int32_t ret = device_->GetScreenReleaseFence(screenId, layersId, fences);
    if (ret != DISPLAY_SUCCESS || layersId.size() != fences.size()) {
        HLOGE("GetScreenReleaseFence failed, ret is %{public}d, layerId size[%{public}d], fence size[%{public}d]",
               ret, (int)layersId.size(), (int)fences.size());
        return {};
    }

    std::map<LayerInfoPtr, sptr<SyncFence>> res;
    auto layersMap = output->GetLayers();
    size_t layerNum = layersId.size();
    for (size_t i = 0; i < layerNum; i++) {
        auto iter = layersMap.find(layersId[i]);
        if (iter == layersMap.end()) {
            HLOGE("Invalid hdi layer id [%{public}u]", layersId[i]);
            continue;
        }

        const LayerPtr &layer = iter->second;
        layer->MergeWithLayerFence(fences[i]);
        res[layer->GetLayerInfo()] = layer->GetReleaseFence();
    }
    return res;
}

void HdiBackend::OnHdiBackendHotPlugEvent(uint32_t screenId, bool connected, void *data)
{
    HLOGI("HotPlugEvent, screenId is %{public}u, connected is %{public}u", screenId, connected);
    HdiBackend *hdiBackend = nullptr;
    if (data != nullptr) {
        hdiBackend = static_cast<HdiBackend *>(data);
    } else {
        hdiBackend = HdiBackend::GetInstance();
    }

    hdiBackend->OnHdiBackendConnected(screenId, connected);
}

void HdiBackend::OnHdiBackendConnected(uint32_t screenId, bool connected)
{
    if (connected) {
        CreateHdiOutput(screenId);
    }

    OnScreenHotplug(screenId, connected);
}

void HdiBackend::CreateHdiOutput(uint32_t screenId)
{
    OutputPtr newOutput = HdiOutput::CreateHdiOutput(screenId);
    newOutput->Init();
    outputs_.emplace(screenId, newOutput);
}

void HdiBackend::OnScreenHotplug(uint32_t screenId, bool connected)
{
    auto iter = outputs_.find(screenId);
    if (iter == outputs_.end()) {
        HLOGE("invalid hotplug screen id[%{public}u]", screenId);
        return;
    }

    if (onScreenHotplugCb_ != nullptr) {
        onScreenHotplugCb_(iter->second, connected, onHotPlugCbData_);
    }

    if (!connected) {
        outputs_.erase(iter);
    }
}

RosenError HdiBackend::InitDevice()
{
    if (device_ != nullptr) {
        return ROSEN_ERROR_OK;
    }

    device_ = HdiDevice::GetInstance();
    if (device_ == nullptr) {
        HLOGE("Get HdiDevice failed");
        return ROSEN_ERROR_NOT_INIT;
    }

    int32_t ret = device_->RegHotPlugCallback(HdiBackend::OnHdiBackendHotPlugEvent, this);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("RegHotPlugCallback failed, ret is %{public}d", ret);
        return ROSEN_ERROR_API_FAILED;
    }

    HLOGI("Init device succeed");

    return ROSEN_ERROR_OK;
}

void HdiBackend::SetHdiBackendDevice(Base::HdiDevice* device)
{
    if (device == nullptr) {
        HLOGE("Input HdiDevice is null");
        return;
    }

    if (device_ != nullptr) {
        HLOGW("HdiDevice has been changed");
        return;
    }
    device_ = device;
}

} // namespace Rosen
} // namespace OHOS
