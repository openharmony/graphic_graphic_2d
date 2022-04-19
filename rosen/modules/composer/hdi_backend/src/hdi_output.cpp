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
    for (auto iter = surfaceIdMap_.begin(); iter != surfaceIdMap_.end(); ++iter) {
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

uint32_t HdiOutput::GetScreenId() const
{
    return screenId_;
}

sptr<Surface> HdiOutput::GetFrameBufferSurface()
{
    if (!CheckFbSurface()) {
        return nullptr;
    }

    return fbSurface_->GetSurface();
}

sptr<SurfaceBuffer> HdiOutput::GetFramebuffer()
{
    if (!CheckFbSurface()) {
        return nullptr;
    }

    return fbSurface_->GetFramebuffer();
}

sptr<SyncFence> HdiOutput::GetFramebufferFence()
{
    if (!CheckFbSurface()) {
        return SyncFence::INVALID_FENCE;
    }

    return fbSurface_->GetFramebufferFence();
}

int32_t HdiOutput::ReleaseFramebuffer(const sptr<SyncFence> &releaseFence)
{
    if (!CheckFbSurface()) {
        return -1;
    }

    return fbSurface_->ReleaseFramebuffer(releaseFence);
}

bool HdiOutput::CheckFbSurface()
{
    if (fbSurface_ == nullptr) {
        HLOGE("fbSurface is nullptr");
        return false;
    }

    return true;
}

void HdiOutput::Dump(std::string &result) const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    ReorderLayerInfo(dumpLayerInfos);

    result.append("\n");
    result.append("-- LayerInfo\n");

    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        const std::string& name = layer->GetLayerInfo()->GetSurface()->GetName();
        const LayerInfoPtr &info = layer->GetLayerInfo();
        result += "\n surface [" + name + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
        info->Dump(result);
    }
}

void HdiOutput::DumpFps(std::string &result, const std::string &arg) const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    ReorderLayerInfo(dumpLayerInfos);

    result.append("\n");

    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        const std::string& name = layer->GetLayerInfo()->GetSurface()->GetName();
        if (name == arg) {
            result += "\n surface [" + name + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
            layer->Dump(result);
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
