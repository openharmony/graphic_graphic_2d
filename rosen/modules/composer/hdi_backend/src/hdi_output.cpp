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

#include <scoped_bytrace.h>
#include "hdi_output.h"

#define CHECK_DEVICE_NULL(device)                                   \
    do {                                                            \
        if ((device) == nullptr) {                                  \
            HLOGD("[%{public}s]HdiDevice is nullptr.", __func__);   \
            return ROSEN_ERROR_NOT_INIT;                            \
        }                                                           \
    } while (0)

namespace OHOS {
namespace Rosen {

std::shared_ptr<HdiOutput> HdiOutput::CreateHdiOutput(uint32_t screenId)
{
    return std::make_shared<HdiOutput>(screenId);
}

HdiOutput::HdiOutput(uint32_t screenId) : screenId_(screenId)
{
}

HdiOutput::~HdiOutput()
{
}

RosenError HdiOutput::Init()
{
    if (fbSurface_ != nullptr) {
        return ROSEN_ERROR_OK;
    }

    fbSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    if (fbSurface_ == nullptr) {
        HLOGE("Create framebuffer surface failed");
        return ROSEN_ERROR_NOT_INIT;
    }

    if (device_ != nullptr) {
        return ROSEN_ERROR_OK;
    }

    device_ = HdiDevice::GetInstance();
    CHECK_DEVICE_NULL(device_);

    bufferCacheCountMax_ = fbSurface_->GetBufferQueueSize();
    int32_t ret = device_->SetScreenClientBufferCacheCount(screenId_, bufferCacheCountMax_);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("Set screen client buffer cache count failed, ret is %{public}d", ret);
        return ROSEN_ERROR_INVALID_OPERATING;
    }
    bufferCache_.resize(bufferCacheCountMax_);

    return ROSEN_ERROR_OK;
}

RosenError HdiOutput::SetHdiOutputDevice(HdiDevice* device)
{
    if (device == nullptr) {
        HLOGE("Input HdiDevice is null");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    if (device_ != nullptr) {
        HLOGW("HdiDevice has been changed");
        return ROSEN_ERROR_OK;
    }
    device_ = device;
    return ROSEN_ERROR_OK;
}

void HdiOutput::SetLayerInfo(const std::vector<LayerInfoPtr> &layerInfos)
{
    for (auto &layerInfo : layerInfos) {
        if (layerInfo == nullptr || layerInfo->GetSurface() == nullptr) {
            HLOGE("current layerInfo or layerInfo's cSurface is null");
            continue;
        }

        uint64_t surfaceId = layerInfo->GetSurface()->GetUniqueId();
        auto iter = surfaceIdMap_.find(surfaceId);
        if (iter != surfaceIdMap_.end()) {
            const LayerPtr &layer = iter->second;
            layer->UpdateLayerInfo(layerInfo);
            continue;
        }

        int32_t ret = CreateLayer(surfaceId, layerInfo);
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            return;
        }
    }

    DeletePrevLayers();
    ResetLayerStatus();
}

void HdiOutput::DeletePrevLayers()
{
    auto surfaceIter = surfaceIdMap_.begin();
    while (surfaceIter != surfaceIdMap_.end()) {
        const LayerPtr &layer = surfaceIter->second;
        if (!layer->GetLayerStatus()) {
            surfaceIdMap_.erase(surfaceIter++);
        } else {
            ++surfaceIter;
        }
    }

    auto layerIter = layerIdMap_.begin();
    while (layerIter != layerIdMap_.end()) {
        const LayerPtr &layer = layerIter->second;
        if (!layer->GetLayerStatus()) {
            layerIdMap_.erase(layerIter++);
        } else {
            ++layerIter;
        }
    }
}

void HdiOutput::ResetLayerStatus()
{
    for (auto iter = layerIdMap_.begin(); iter != layerIdMap_.end(); ++iter) {
        iter->second->SetLayerStatus(false);
    }
}

int32_t HdiOutput::CreateLayer(uint64_t surfaceId, const LayerInfoPtr &layerInfo)
{
    LayerPtr layer = HdiLayer::CreateHdiLayer(screenId_);
    if (!layer->Init(layerInfo)) {
        HLOGE("Init hdiLayer failed");
        return GRAPHIC_DISPLAY_FAILURE;
    }

    layer->UpdateLayerInfo(layerInfo);
    uint32_t layerId = layer->GetLayerId();
    layerIdMap_[layerId] = layer;
    surfaceIdMap_[surfaceId] = layer;

    return GRAPHIC_DISPLAY_SUCCESS;
}

void HdiOutput::SetOutputDamages(const std::vector<GraphicIRect> &outputDamages)
{
    outputDamages_ = outputDamages;
}

const std::vector<GraphicIRect>& HdiOutput::GetOutputDamages()
{
    return outputDamages_;
}

const std::unordered_map<uint32_t, std::shared_ptr<HdiLayer>>& HdiOutput::GetLayers()
{
    return layerIdMap_;
}

void HdiOutput::UpdatePrevLayerInfo()
{
    for (auto iter = layerIdMap_.begin(); iter != layerIdMap_.end(); iter++) {
        LayerPtr layer = iter->second;
        layer->SavePrevLayerInfo();
    }
}

uint32_t HdiOutput::GetScreenId() const
{
    return screenId_;
}

void HdiOutput::SetLayerCompCapacity(uint32_t layerCompositionCapacity)
{
    layerCompCapacity_ = layerCompositionCapacity;
}

uint32_t HdiOutput::GetLayerCompCapacity() const
{
    return layerCompCapacity_;
}

sptr<Surface> HdiOutput::GetFrameBufferSurface()
{
    if (!CheckFbSurface()) {
        return nullptr;
    }

    return fbSurface_->GetSurface();
}

std::unique_ptr<FrameBufferEntry> HdiOutput::GetFramebuffer()
{
    if (!CheckFbSurface()) {
        return nullptr;
    }

    return fbSurface_->GetFramebuffer();
}

bool HdiOutput::CheckFbSurface()
{
    if (fbSurface_ == nullptr) {
        HLOGE("fbSurface is nullptr");
        return false;
    }

    return true;
}

void HdiOutput::RecordCompositionTime(int64_t timeStamp)
{
    compositionTimeRecords_[compTimeRcdIndex_] = timeStamp;
    compTimeRcdIndex_ = (compTimeRcdIndex_ + 1) % COMPOSITION_RECORDS_NUM;
}

void HdiOutput::SetDirectClientCompEnableStatus(bool enableStatus)
{
    directClientCompositionEnabled_ = enableStatus;
}

bool HdiOutput::GetDirectClientCompEnableStatus() const
{
    return directClientCompositionEnabled_;
}

int32_t HdiOutput::PreProcessLayersComp(bool &needFlush)
{
    if (layerIdMap_.empty()) {
        HLOGI("layer map is empty, drop this frame");
        return GRAPHIC_DISPLAY_PARAM_ERR;
    }

    uint32_t layersNum = layerIdMap_.size();
    // If doClientCompositionDirectly is true then layer->SetHdiLayerInfo and UpdateLayerCompType is no need to run.
    bool doClientCompositionDirectly = ((layerCompCapacity_ != LAYER_COMPOSITION_CAPACITY_INVALID) &&
                                        (layersNum > layerCompCapacity_));
    if (!directClientCompositionEnabled_) {
        doClientCompositionDirectly = false;
    }

    int32_t ret;
    for (auto iter = layerIdMap_.begin(); iter != layerIdMap_.end(); ++iter) {
        const LayerPtr &layer = iter->second;
        if (doClientCompositionDirectly) {
            layer->UpdateCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
            continue;
        }
        ret = layer->SetHdiLayerInfo();
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGE("Set hdi layer[id:%{public}d] info failed, ret %{public}d.", layer->GetLayerId(), ret);
            return GRAPHIC_DISPLAY_FAILURE;
        }
    }

    CHECK_DEVICE_NULL(device_);
    ret = device_->PrepareScreenLayers(screenId_, needFlush);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("PrepareScreenLayers failed, ret is %{public}d", ret);
        return GRAPHIC_DISPLAY_FAILURE;
    }

    if (doClientCompositionDirectly) {
        ScopedBytrace doClientCompositionDirectlyTag("DoClientCompositionDirectly");
        HLOGD("Direct client composition is enabled.");
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    return UpdateLayerCompType();
}

int32_t HdiOutput::UpdateLayerCompType()
{
    CHECK_DEVICE_NULL(device_);
    std::vector<uint32_t> layersId;
    std::vector<int32_t> types;
    int32_t ret = device_->GetScreenCompChange(screenId_, layersId, types);
    if (ret != GRAPHIC_DISPLAY_SUCCESS || layersId.size() != types.size()) {
        HLOGE("GetScreenCompChange failed, ret is %{public}d", ret);
        return ret;
    }

    size_t layerNum = layersId.size();
    for (size_t i = 0; i < layerNum; i++) {
        auto iter = layerIdMap_.find(layersId[i]);
        if (iter == layerIdMap_.end()) {
            HLOGE("Invalid hdi layer id[%{public}u]", layersId[i]);
            continue;
        }

        const LayerPtr &layer = iter->second;
        layer->UpdateCompositionType(static_cast<GraphicCompositionType>(types[i]));
    }

    return ret;
}

bool HdiOutput::CheckAndUpdateClientBufferCahce(sptr<SurfaceBuffer> buffer, uint32_t& index)
{
    for (uint32_t i = 0; i < bufferCacheCountMax_; i++) {
        if (bufferCache_[i] == buffer) {
            index = i;
            return true;
        }
    }

    if (bufferCacheIndex_ >= bufferCacheCountMax_) {
        HLOGE("HdiOutput::FlushScreen: the length of buffer cache exceeds the limit!");
        return false;
    }
    bufferCache_[bufferCacheIndex_] = buffer;
    index = bufferCacheIndex_;
    bufferCacheIndex_++;
    return false;
}

int32_t HdiOutput::FlushScreen(std::vector<LayerPtr> &compClientLayers)
{
    auto fbEntry = GetFramebuffer();
    if (fbEntry == nullptr) {
        HLOGE("HdiBackend flush screen failed : GetFramebuffer failed!");
        return -1;
    }

    const auto& fbAcquireFence = fbEntry->acquireFence;
    for (auto &layer : compClientLayers) {
        layer->MergeWithFramebufferFence(fbAcquireFence);
    }

    currFrameBuffer_ = fbEntry->buffer;
    if (currFrameBuffer_ == nullptr) {
        HLOGE("HdiBackend flush screen failed : frame buffer is null");
        return -1;
    }

    uint32_t index = INVALID_BUFFER_CACHE_INDEX;
    bool bufferCached = false;
    if (bufferCacheCountMax_ == 0) {
        bufferCache_.clear();
        bufferCacheIndex_ = INVALID_BUFFER_CACHE_INDEX;
        HLOGE("The count of this client buffer cache is 0.");
    } else {
        bufferCached = CheckAndUpdateClientBufferCahce(currFrameBuffer_, index);
    }

    CHECK_DEVICE_NULL(device_);
    int32_t ret = device_->SetScreenClientDamage(screenId_, outputDamages_);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("Set screen client damage failed, ret is %{public}d", ret);
        return ret;
    }

    CHECK_DEVICE_NULL(device_);
    if (bufferCached && index < bufferCacheCountMax_) {
        ret = device_->SetScreenClientBuffer(screenId_, nullptr, index, fbAcquireFence);
    } else {
        ret = device_->SetScreenClientBuffer(screenId_, currFrameBuffer_->GetBufferHandle(), index, fbAcquireFence);
    }
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("Set screen client buffer failed, ret is %{public}d", ret);
        return ret;
    }

    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiOutput::Commit(sptr<SyncFence> &fbFence)
{
    CHECK_DEVICE_NULL(device_);
    return device_->Commit(screenId_, fbFence);
}

int32_t HdiOutput::UpdateInfosAfterCommit(sptr<SyncFence> fbFence)
{
    UpdatePrevLayerInfo();

    if (sampler_ == nullptr) {
        sampler_ = CreateVSyncSampler();
    }
    int64_t timestamp = lastPresentFence_->SyncFileReadTimestamp();
    bool startSample = false;
    if (timestamp != SyncFence::FENCE_PENDING_TIMESTAMP) {
        startSample = sampler_->AddPresentFenceTime(timestamp);
        RecordCompositionTime(timestamp);
        for (auto iter = layerIdMap_.begin(); iter != layerIdMap_.end(); ++iter) {
            const LayerPtr &layer = iter->second;
            layer->RecordPresentTime(timestamp);
        }
    }

    int32_t ret = GRAPHIC_DISPLAY_SUCCESS;
    bool alreadyStartSample = sampler_->GetHardwareVSyncStatus();
    if (startSample && !alreadyStartSample) {
        HLOGD("Enable Screen Vsync");
        CHECK_DEVICE_NULL(device_);
        ret = device_->SetScreenVsyncEnabled(screenId_, true);
        if (ret == GRAPHIC_DISPLAY_SUCCESS) {
            sampler_->BeginSample();
        } else {
            HLOGE("Enable Screen Vsync failed");
        }
    }
    lastPresentFence_ = fbFence;
    return ret;
}

int32_t HdiOutput::ReleaseFramebuffer(const sptr<SyncFence>& releaseFence)
{
    if (currFrameBuffer_ == nullptr) {
        HLOGE("Current frame buffer is nullptr.");
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t ret = GRAPHIC_DISPLAY_SUCCESS;
    if (lastFrameBuffer_ != nullptr) {
        if (!CheckFbSurface()) { // wrong check
            ret = GRAPHIC_DISPLAY_NULL_PTR;
        } else {
            ret = fbSurface_->ReleaseFramebuffer(lastFrameBuffer_, releaseFence);
        }
    }

    lastFrameBuffer_ = currFrameBuffer_;
    currFrameBuffer_ = nullptr;
    return ret;
}

std::map<LayerInfoPtr, sptr<SyncFence>> HdiOutput::GetLayersReleaseFence()
{
    if (device_ == nullptr) {
        return {};
    }
    std::vector<uint32_t> layersId;
    std::vector<sptr<SyncFence>> fences;
    int32_t ret = device_->GetScreenReleaseFence(screenId_, layersId, fences);
    if (ret != GRAPHIC_DISPLAY_SUCCESS || layersId.size() != fences.size()) {
        HLOGE("GetScreenReleaseFence failed, ret is %{public}d, layerId size[%{public}d], fence size[%{public}d]",
              ret, (int)layersId.size(), (int)fences.size());
        return {};
    }

    std::map<LayerInfoPtr, sptr<SyncFence>> res;
    size_t layerNum = layersId.size();
    for (size_t i = 0; i < layerNum; i++) {
        auto iter = layerIdMap_.find(layersId[i]);
        if (iter == layerIdMap_.end()) {
            HLOGE("Invalid hdi layer id [%{public}u]", layersId[i]);
            continue;
        }

        const LayerPtr &layer = iter->second;
        layer->MergeWithLayerFence(fences[i]);
        res[layer->GetLayerInfo()] = layer->GetReleaseFence();
    }
    return res;
}

void HdiOutput::Dump(std::string &result) const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    ReorderLayerInfo(dumpLayerInfos);

    result.append("\n");
    result.append("-- LayerInfo\n");

    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        if (layer == nullptr || layer->GetLayerInfo() == nullptr ||
            layer->GetLayerInfo()->GetSurface() == nullptr) {
            continue;
        }
        const std::string& name = layer->GetLayerInfo()->GetSurface()->GetName();
        const LayerInfoPtr &info = layer->GetLayerInfo();
        result += "\n surface [" + name + "] NodeId[" + std::to_string(layerInfo.surfaceId) + "]";
        result +=  " LayerId[" + std::to_string(layer->GetLayerId()) + "]:\n";
        info->Dump(result);
    }

    if (fbSurface_ != nullptr) {
        result += "\n";
        result += "FrameBufferSurface\n";
        fbSurface_->Dump(result);
    }
}

void HdiOutput::DumpFps(std::string &result, const std::string &arg) const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    ReorderLayerInfo(dumpLayerInfos);

    result.append("\n");
    if (arg == "composer") {
        result += "The fps of screen [Id:" + std::to_string(screenId_) + "] is:\n";
        const int32_t offset = compTimeRcdIndex_;
        for (uint32_t i = 0; i < COMPOSITION_RECORDS_NUM; i++) {
            uint32_t order = (offset + i) % COMPOSITION_RECORDS_NUM;
            result += std::to_string(compositionTimeRecords_[order]) + "\n";
        }
        return;
    }

    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        const std::string& name = layer->GetLayerInfo()->GetSurface()->GetName();
        if (name == arg) {
            result += "\n surface [" + name + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
            layer->Dump(result);
        }
    }
}

void HdiOutput::ClearFpsDump(std::string &result, const std::string &arg)
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    ReorderLayerInfo(dumpLayerInfos);

    result.append("\n");
    if (arg == "composer") {
        result += "The fps info of screen [Id:" + std::to_string(screenId_) + "] is cleared.\n";
        compositionTimeRecords_.fill(0);
        return;
    }

    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        const std::string& name = layer->GetLayerInfo()->GetSurface()->GetName();
        if (name == arg) {
            result += "\n The fps info of surface [" + name + "] Id["
                + std::to_string(layerInfo.surfaceId) + "] is cleared.\n";
            layer->ClearDump();
        }
    }
}

static inline bool Cmp(const LayerDumpInfo &layer1, const LayerDumpInfo &layer2)
{
    return layer1.layer->GetLayerInfo()->GetZorder() < layer2.layer->GetLayerInfo()->GetZorder();
}

void HdiOutput::ReorderLayerInfo(std::vector<LayerDumpInfo> &dumpLayerInfos) const
{
    for (auto iter = surfaceIdMap_.begin(); iter != surfaceIdMap_.end(); ++iter) {
        struct LayerDumpInfo layerInfo = {
            .surfaceId = iter->first,
            .layer = iter->second,
        };
        dumpLayerInfos.emplace_back(layerInfo);
    }

    std::sort(dumpLayerInfos.begin(), dumpLayerInfos.end(), Cmp);
}
} // namespace Rosen
} // namespace OHOS
