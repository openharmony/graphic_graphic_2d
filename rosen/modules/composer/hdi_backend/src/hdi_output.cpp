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

#include "hdi_output.h"

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
            const LayerInfoPtr &info = layer->GetLayerInfo();
            if (info->GetLayerSize().w == layerInfo->GetLayerSize().w &&
                info->GetLayerSize().h == layerInfo->GetLayerSize().h)
            {
                layer->UpdateLayerInfo(layerInfo);
                continue;
            }
        }

        int32_t ret = CreateLayer(surfaceId, layerInfo);
        if (ret != DISPLAY_SUCCESS) {
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
        return DISPLAY_FAILURE;
    }

    layer->UpdateLayerInfo(layerInfo);
    uint32_t layerId = layer->GetLayerId();
    layerIdMap_[layerId] = layer;
    surfaceIdMap_[surfaceId] = layer;

    return DISPLAY_SUCCESS;
}

void HdiOutput::SetOutputDamage(uint32_t num, const IRect &outputDamage)
{
    outputDamageNum_ = num;
    outputDamage_ = outputDamage;
}

/* const */ IRect& HdiOutput::GetOutputDamage()
{
    return outputDamage_;
}

uint32_t HdiOutput::GetOutputDamageNum() const
{
    return outputDamageNum_;
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

int32_t HdiOutput::ReleaseFramebuffer(
    sptr<SurfaceBuffer> &buffer,
    const sptr<SyncFence>& releaseFence)
{
    if (!CheckFbSurface()) {
        return -1;
    }

    return fbSurface_->ReleaseFramebuffer(buffer, releaseFence);
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
