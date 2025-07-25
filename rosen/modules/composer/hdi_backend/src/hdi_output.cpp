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

#include <cstdint>
#include <scoped_bytrace.h>
#include <unordered_set>
#include "rs_trace.h"
#include "hdi_log.h"
#include "hdi_output.h"
#include "string_utils.h"
#include "metadata_helper.h"
#include "vsync_generator.h"
#include "vsync_sampler.h"
// DISPLAYENGINE
#include "syspara/parameters.h"

using namespace OHOS::HDI::Display::Graphic::Common::V1_0;

#define CHECK_DEVICE_NULL(device)                                   \
    do {                                                            \
        if ((device) == nullptr) {                                  \
            HLOGD("[%{public}s]HdiDevice is nullptr.", __func__);   \
            return ROSEN_ERROR_NOT_INIT;                            \
        }                                                           \
    } while (0)

namespace OHOS {
namespace Rosen {
static constexpr uint32_t NUMBER_OF_HISTORICAL_FRAMES = 2;
static const std::string GENERIC_METADATA_KEY_ARSR_PRE_NEEDED = "ArsrDoEnhance";
static const std::string GENERIC_METADATA_KEY_COPYBIT_NEEDED = "TryToDoCopybit";

std::shared_ptr<HdiOutput> HdiOutput::CreateHdiOutput(uint32_t screenId)
{
    return std::make_shared<HdiOutput>(screenId);
}

HdiOutput::HdiOutput(uint32_t screenId) : screenId_(screenId)
{
    // DISPLAYENGINE ARSR_PRE FLAG
    arsrPreEnabled_ = system::GetBoolParameter("const.display.enable_arsr_pre", true);
    arsrPreEnabledForVm_ = system::GetBoolParameter("const.display.enable_arsr_pre_for_vm", false);
    vmArsrWhiteList_ = system::GetParameter("const.display.vmlayer.whitelist", "unknown");

    // LOAD OPTIMIZATION FLAG
    isMergeFenceSkippedDfx_ = system::GetBoolParameter("persist.sys.graphic.enableSkipMergeFence", true);
}

HdiOutput::~HdiOutput()
{
    ClearBufferCache();
}

GSError HdiOutput::ClearFrameBuffer()
{
    GSError ret = GSERROR_OK;
    if (!CheckFbSurface()) {
        return ret;
    }
    currFrameBuffer_ = nullptr;
    lastFrameBuffer_ = nullptr;
    ClearBufferCache();
    fbSurface_->ClearFrameBuffer();
    sptr<Surface> pFrameSurface = GetFrameBufferSurface();
    if (pFrameSurface != nullptr) {
        ret = pFrameSurface->CleanCache();
    }
    // reset HdiOutput preallocted framebuffer state
    if (isProtectedBufferAllocated_.load()) {
        isProtectedBufferAllocated_.store(false);
    }
    return ret;
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
    ClearBufferCache();
    bufferCache_.reserve(bufferCacheCountMax_);
    historicalPresentfences_.clear();

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
    uint32_t solidLayerCount = 0;
    std::unique_lock<std::mutex> lock(mutex_);
    for (auto &layerInfo : layerInfos) {
        if (layerInfo == nullptr) {
            HLOGE("current layerInfo is null");
            continue;
        }
        if (layerInfo->IsMaskLayer()) {
            DirtyRegions(solidLayerCount, layerInfo);
            continue;
        }
        if (layerInfo->GetSurface() == nullptr) {
            if (layerInfo->GetCompositionType() != GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
                continue;
            }
            auto iter = solidSurfaceIdMap_.find(solidLayerCount);
            if (iter != solidSurfaceIdMap_.end()) {
                const LayerPtr &layer = iter->second;
                layer->UpdateLayerInfo(layerInfo);
                solidLayerCount++;
                continue;
            }
            CreateLayerLocked(solidLayerCount++, layerInfo);
            continue;
        }

        uint64_t surfaceId = layerInfo->GetSurface()->GetUniqueId();
        auto iter = surfaceIdMap_.find(surfaceId);
        if (iter != surfaceIdMap_.end()) {
            const LayerPtr& layer = iter->second;
            layer->UpdateLayerInfo(layerInfo);
            continue;
        }

        CreateLayerLocked(surfaceId, layerInfo);
    }

    DeletePrevLayersLocked();
    ResetLayerStatusLocked();
}

void HdiOutput::DirtyRegions(uint32_t solidLayerCount, const LayerInfoPtr &layerInfo)
{
    std::vector<GraphicIRect> dirtyRegions;
    if (maskLayer_) {
        dirtyRegions.emplace_back(GraphicIRect {0, 0, 0, 0});
        layerInfo->SetDirtyRegions(dirtyRegions);
        maskLayer_->UpdateLayerInfo(layerInfo);
    } else {
        auto laysize = layerInfo->GetLayerSize();
        dirtyRegions.emplace_back(laysize);
        layerInfo->SetDirtyRegions(dirtyRegions);
        CreateLayerLocked(solidLayerCount++, layerInfo);
    }
}

void HdiOutput::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    std::unique_lock<std::mutex> lock(mutex_);
    RS_TRACE_NAME_FMT("HdiOutput::CleanLayerBufferById, screenId=%u, surfaceId=%lu", screenId_, surfaceId);
    auto iter = surfaceIdMap_.find(surfaceId);
    if (iter == surfaceIdMap_.end()) {
        return;
    }
    const LayerPtr& layer = iter->second;
    if (layer) {
        layer->ClearBufferCache();
    }
}

void HdiOutput::DeletePrevLayersLocked()
{
    if (maskLayer_ && !maskLayer_->GetLayerStatus()) {
        maskLayer_ = nullptr;
    }
    auto solidSurfaceIter = solidSurfaceIdMap_.begin();
    while (solidSurfaceIter != solidSurfaceIdMap_.end()) {
        const LayerPtr& layer = solidSurfaceIter->second;
        if (!layer->GetLayerStatus()) {
            solidSurfaceIdMap_.erase(solidSurfaceIter++);
        } else {
            ++solidSurfaceIter;
        }
    }

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

    auto iter = layersTobeRelease_.begin();
    while (iter != layersTobeRelease_.end()) {
        const LayerPtr &layer = *iter;
        if (!layer->GetLayerStatus()) {
            layersTobeRelease_.erase(iter++);
        } else {
            ++iter;
        }
    }
}

void HdiOutput::ResetLayerStatusLocked()
{
    for (auto iter = layerIdMap_.begin(); iter != layerIdMap_.end(); ++iter) {
        iter->second->SetLayerStatus(false);
    }
    for (auto iter = layersTobeRelease_.begin(); iter != layersTobeRelease_.end(); ++iter) {
        (*iter)->SetLayerStatus(false);
    }
    if (maskLayer_) {
        maskLayer_->SetLayerStatus(false);
    }
}

bool HdiOutput::CheckSupportArsrPreMetadata()
{
    const auto& validKeys = device_->GetSupportedLayerPerFrameParameterKey();
    if (std::find(validKeys.begin(), validKeys.end(), GENERIC_METADATA_KEY_ARSR_PRE_NEEDED) != validKeys.end()) {
        return true;
    }
    return false;
}

bool HdiOutput::CheckSupportCopybitMetadata()
{
    const auto& validKeys = device_->GetSupportedLayerPerFrameParameterKey();
    if (std::find(validKeys.begin(), validKeys.end(), GENERIC_METADATA_KEY_COPYBIT_NEEDED) != validKeys.end()) {
        return true;
    }
    return false;
}

int32_t HdiOutput::CreateLayerLocked(uint64_t surfaceId, const LayerInfoPtr &layerInfo)
{
    LayerPtr layer = HdiLayer::CreateHdiLayer(screenId_);
    if (!layer->Init(layerInfo)) {
        layer->UpdateLayerInfo(layerInfo);
        layersTobeRelease_.emplace_back(layer);
        HLOGE("Init hdiLayer failed");
        return GRAPHIC_DISPLAY_FAILURE;
    }

    if (layerInfo->GetSurface() == nullptr && layerInfo->GetCompositionType() !=
        GRAPHIC_COMPOSITION_SOLID_COLOR) {
        HLOGE("CreateLayerLocked failed because the surface is null");
        return GRAPHIC_DISPLAY_FAILURE;
    }

    layer->UpdateLayerInfo(layerInfo);
    uint32_t layerId = layer->GetLayerId();

    if (layerInfo->IsMaskLayer()) {
        maskLayer_ = layer;
    }

    layerIdMap_[layerId] = layer;

    if (layerInfo->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
        // solid layer's surfaceId is unique, use solidLayerCount as key, to avoid conflict with normal layer
        solidSurfaceIdMap_[surfaceId] = layer;
    } else {
        surfaceIdMap_[surfaceId] = layer;
    }

    if (device_ == nullptr) {
        HLOGE("[%{public}s]HdiDevice is nullptr.", __func__);
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    if ((arsrPreEnabledForVm_ && CheckSupportArsrPreMetadata() && CheckIfDoArsrPreForVm(layerInfo)) ||
        (arsrPreEnabled_ && CheckSupportArsrPreMetadata() && CheckIfDoArsrPre(layerInfo))) {
        const std::vector<int8_t> valueBlob{static_cast<int8_t>(1)};
        if (device_->SetLayerPerFrameParameter(screenId_,
            layerId, GENERIC_METADATA_KEY_ARSR_PRE_NEEDED, valueBlob) != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGE("SetLayerPerFrameParameter Fail!");
        }
    }

    if (layerInfo->GetLayerCopybit() && CheckSupportCopybitMetadata()) {
        const std::vector<int8_t> valueBlob{static_cast<int8_t>(1)};
        if (device_->SetLayerPerFrameParameter(screenId_,
            layerId, GENERIC_METADATA_KEY_COPYBIT_NEEDED, valueBlob) != GRAPHIC_DISPLAY_SUCCESS) {
                HLOGE("SetLayerPerFrameParameter Fail!");
        }
    }

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

void HdiOutput::GetComposeClientLayers(std::vector<LayerPtr>& clientLayers)
{
    std::unique_lock<std::mutex> lock(mutex_);
    for (const auto &[first, layer] : layerIdMap_) {
        if (layer == nullptr || layer->GetLayerInfo() == nullptr) {
            continue;
        }
        if (layer->GetLayerInfo()->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT ||
            layer->GetLayerInfo()->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT_CLEAR ||
            layer->GetLayerInfo()->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_TUNNEL) {
            clientLayers.emplace_back(layer);
        }
    }
}

void HdiOutput::GetLayerInfos(std::vector<LayerInfoPtr>& layerInfos)
{
    std::unique_lock<std::mutex> lock(mutex_);
    for (const auto &it : layerIdMap_) {
        if (it.second != nullptr) {
            layerInfos.emplace_back(it.second->GetLayerInfo());
        }
    }
}

void HdiOutput::UpdatePrevLayerInfoLocked()
{
    RS_TRACE_NAME_FMT("HdiOutput::UpdatePrevLayerInfoLocked, layerIdMap size %u", layerIdMap_.size());
    for (auto iter = layerIdMap_.begin(); iter != layerIdMap_.end(); iter++) {
        LayerPtr layer = iter->second;
        layer->SavePrevLayerInfo();
    }
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

int32_t HdiOutput::PreProcessLayersComp()
{
    int32_t ret = GRAPHIC_DISPLAY_SUCCESS;
    std::unique_lock<std::mutex> lock(mutex_);
    if (layerIdMap_.empty()) {
        HLOGI("layer map is empty, drop this frame");
        return GRAPHIC_DISPLAY_PARAM_ERR;
    }

    for (const auto &[layerId, layer] : layerIdMap_) {
        ret = layer->SetHdiLayerInfo(isActiveRectSwitching_);
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGE("Set hdi layer[id:%{public}d] info failed, ret %{public}d.", layer->GetLayerId(), ret);
            return GRAPHIC_DISPLAY_FAILURE;
        }
    }

    return ret;
}

int32_t HdiOutput::UpdateLayerCompType()
{
    CHECK_DEVICE_NULL(device_);
    std::vector<uint32_t> layersId;
    std::vector<int32_t> types;
    int32_t ret = device_->GetScreenCompChange(screenId_, layersId, types);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("GetScreenCompChange failed, ret is %{public}d", ret);
        return ret;
    }
    if (layersId.size() != types.size()) {
        HLOGE("HdiOutput::UpdateLayerCompType layersId size is not types size");
        return GRAPHIC_DISPLAY_FAILURE;
    }

    std::unique_lock<std::mutex> lock(mutex_);
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
    uint32_t bufferCahceSize = static_cast<uint32_t>(bufferCache_.size());
    for (uint32_t i = 0; i < bufferCahceSize; i++) {
        if (bufferCache_[i] == buffer) {
            index = i;
            return true;
        }
    }

    if (bufferCahceSize >= bufferCacheCountMax_) {
        HLOGI("the length of buffer cache exceeds the limit, and not find the aim buffer!");
        ClearBufferCache();
    }

    index = static_cast<uint32_t>(bufferCache_.size());
    bufferCache_.push_back(buffer);
    return false;
}

// DISPLAY ENGINE
bool HdiOutput::CheckIfDoArsrPre(const LayerInfoPtr &layerInfo)
{
    static const std::unordered_set<GraphicPixelFormat> yuvFormats {
        GRAPHIC_PIXEL_FMT_YUV_422_I,
        GRAPHIC_PIXEL_FMT_YCBCR_422_SP,
        GRAPHIC_PIXEL_FMT_YCRCB_422_SP,
        GRAPHIC_PIXEL_FMT_YCBCR_420_SP,
        GRAPHIC_PIXEL_FMT_YCRCB_420_SP,
        GRAPHIC_PIXEL_FMT_YCBCR_422_P,
        GRAPHIC_PIXEL_FMT_YCRCB_422_P,
        GRAPHIC_PIXEL_FMT_YCBCR_420_P,
        GRAPHIC_PIXEL_FMT_YCRCB_420_P,
        GRAPHIC_PIXEL_FMT_YUYV_422_PKG,
        GRAPHIC_PIXEL_FMT_UYVY_422_PKG,
        GRAPHIC_PIXEL_FMT_YVYU_422_PKG,
        GRAPHIC_PIXEL_FMT_VYUY_422_PKG,
        GRAPHIC_PIXEL_FMT_YCBCR_P010,
        GRAPHIC_PIXEL_FMT_YCRCB_P010,
    };

    static const std::unordered_set<std::string> videoLayers {
        "xcomponentIdSurface",
        "componentIdSurface",
        "SceneViewer Model totemweather0",
    };

    if (layerInfo == nullptr || layerInfo->GetSurface() == nullptr || layerInfo->GetBuffer() == nullptr) {
        return false;
    }

    if (((yuvFormats.count(static_cast<GraphicPixelFormat>(layerInfo->GetBuffer()->GetFormat())) > 0) ||
        (videoLayers.count(layerInfo->GetSurface()->GetName()) > 0)) && layerInfo->GetLayerArsr()) {
        return true;
    }

    return false;
}

bool HdiOutput::CheckIfDoArsrPreForVm(const LayerInfoPtr &layerInfo)
{
    char sep = ';';
    std::unordered_set<std::string> vmLayers;
    SplitString(vmArsrWhiteList_, vmLayers, sep);
    if (layerInfo->GetSurface() != nullptr && vmLayers.count(layerInfo->GetSurface()->GetName()) > 0) {
        return true;
    }
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
        if (layer != nullptr) {
            layer->MergeWithFramebufferFence(fbAcquireFence);
        }
    }

    currFrameBuffer_ = fbEntry->buffer;
    if (currFrameBuffer_ == nullptr) {
        HLOGE("HdiBackend flush screen failed : frame buffer is null");
        return -1;
    }

    uint32_t index = INVALID_BUFFER_CACHE_INDEX;
    bool bufferCached = false;
    if (bufferCacheCountMax_ == 0) {
        ClearBufferCache();
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

int32_t HdiOutput::CommitAndGetReleaseFence(
    sptr<SyncFence> &fbFence, int32_t &skipState, bool &needFlush, bool isValidated)
{
    CHECK_DEVICE_NULL(device_);
    layersId_.clear();
    fences_.clear();
    return device_->CommitAndGetReleaseFence(
        screenId_, fbFence, skipState, needFlush, layersId_, fences_, isValidated);
}

int32_t HdiOutput::UpdateInfosAfterCommit(sptr<SyncFence> fbFence)
{
    RS_TRACE_NAME("HdiOutput::UpdateInfosAfterCommit");
    std::unique_lock<std::mutex> lock(mutex_);
    if (thirdFrameAheadPresentFence_ == nullptr) {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }
    UpdatePrevLayerInfoLocked();

    if (sampler_ == nullptr) {
        sampler_ = CreateVSyncSampler();
    }
    RS_TRACE_BEGIN("HdiOutput::SyncFileReadTimestamp");
    int64_t timestamp = thirdFrameAheadPresentFence_->SyncFileReadTimestamp();
    RS_TRACE_END();
    bool startSample = false;
    if (timestamp != SyncFence::FENCE_PENDING_TIMESTAMP) {
        startSample = sampler_->GetVsyncSamplerEnabled() && sampler_->AddPresentFenceTime(screenId_, timestamp);
        RecordCompositionTime(timestamp);
        bool presentTimeUpdated = false;
        LayerPtr uniRenderLayer = nullptr;
        for (auto iter = layerIdMap_.begin(); iter != layerIdMap_.end(); ++iter) {
            const LayerPtr &layer = iter->second;
            RS_TRACE_NAME_FMT("HdiOutput::Iterate layerIdMap_ %u", iter->first);
            if (layer->RecordPresentTime(timestamp)) {
                presentTimeUpdated = true;
            }
            if (layer->GetLayerInfo() && layer->GetLayerInfo()->GetUniRenderFlag()) {
                uniRenderLayer = layer;
            }
        }
        if (presentTimeUpdated && uniRenderLayer) {
            RS_TRACE_NAME_FMT("HdiOutput::RecordMergedPresentTime %llu", timestamp);
            uniRenderLayer->RecordMergedPresentTime(timestamp);
        }
    }

    int32_t ret = GRAPHIC_DISPLAY_SUCCESS;
    if (startSample) {
        ret = StartVSyncSampler();
    }
    RS_TRACE_NAME_FMT("presentFenceIndex_ = %d", presentFenceIndex_);
    if (historicalPresentfences_.size() == NUMBER_OF_HISTORICAL_FRAMES) {
        thirdFrameAheadPresentFence_ = historicalPresentfences_[presentFenceIndex_];
        historicalPresentfences_[presentFenceIndex_] = fbFence;
        presentFenceIndex_ = (presentFenceIndex_ + 1) % NUMBER_OF_HISTORICAL_FRAMES;
    } else {
        historicalPresentfences_.push_back(fbFence);
    }
    return ret;
}

void HdiOutput::SetVsyncSamplerEnabled(bool enabled)
{
    if (sampler_ == nullptr) {
        sampler_ = CreateVSyncSampler();
    }
    sampler_->SetVsyncSamplerEnabled(enabled);
}

bool HdiOutput::GetVsyncSamplerEnabled()
{
    if (sampler_ == nullptr) {
        sampler_ = CreateVSyncSampler();
    }
    return sampler_->GetVsyncSamplerEnabled();
}

int32_t HdiOutput::ReleaseFramebuffer(const sptr<SyncFence>& releaseFence)
{
    if (currFrameBuffer_ == nullptr) {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t ret = GRAPHIC_DISPLAY_SUCCESS;
    if (lastFrameBuffer_ != nullptr) {
        if (!CheckFbSurface()) { // wrong check
            ret = GRAPHIC_DISPLAY_NULL_PTR;
        } else {
            RS_TRACE_NAME_FMT("HdiOutput::ReleaseFramebuffer, seqNum %u", lastFrameBuffer_->GetSeqNum());
            ret = fbSurface_->ReleaseFramebuffer(lastFrameBuffer_, releaseFence);
        }
    }

    lastFrameBuffer_ = currFrameBuffer_;
    currFrameBuffer_ = nullptr;
    return ret;
}

void HdiOutput::ReleaseSurfaceBuffer(sptr<SyncFence>& releaseFence)
{
    auto releaseBuffer = [](sptr<SurfaceBuffer> buffer, sptr<SyncFence> releaseFence,
        sptr<IConsumerSurface> cSurface) -> void {
        if (cSurface == nullptr) {
            HLOGD("HdiOutput:: ReleaseBuffer failed, no consumer!");
            return;
        }
        if (buffer == nullptr) {
            return;
        }
        RS_TRACE_NAME_FMT("HdiOutput::ReleaseBuffer, seqNum %u", buffer->GetSeqNum());
        auto ret = cSurface->ReleaseBuffer(buffer, releaseFence);
        if (ret == OHOS::SURFACE_ERROR_OK) {
            // reset prevBuffer if we release it successfully,
            // to avoid releasing the same buffer next frame in some situations.
            buffer = nullptr;
            releaseFence = SyncFence::InvalidFence();
        }
    };
    const auto layersReleaseFence = GetLayersReleaseFenceLocked();
    if (layersReleaseFence.size() == 0) {
        // When release fence's size is 0, the output may invalid, release all buffer
        // This situation may happen when killing composer_host
        for (const auto& [id, layer] : layerIdMap_) {
            if (layer == nullptr || layer->GetLayerInfo() == nullptr ||
                layer->GetLayerInfo()->GetSurface() == nullptr) {
                HLOGD("HdiOutput::ReleaseLayers: layer or layerInfo or layer's cSurface is nullptr");
                continue;
            }
            auto preBuffer = layer->GetLayerInfo()->GetPreBuffer();
            auto consumer = layer->GetLayerInfo()->GetSurface();
            releaseBuffer(preBuffer, SyncFence::InvalidFence(), consumer);
        }
        HLOGD("HdiOutput::ReleaseLayers: no layer needs to release");
    }
    for (const auto& [layer, fence] : layersReleaseFence) {
        if (layer != nullptr) {
            auto preBuffer = layer->GetPreBuffer();
            auto consumer = layer->GetSurface();
            ANCOTransactionOnComplete(layer, fence);
            releaseBuffer(preBuffer, fence, consumer);
            if (layer->GetUniRenderFlag()) {
                releaseFence = fence;
            }
        }
    }
    for (const auto& layer : layersTobeRelease_) {
        if (layer == nullptr || layer->GetLayerInfo() == nullptr ||
            layer->GetLayerInfo()->GetSurface() == nullptr) {
            continue;
        }
        auto preBuffer = layer->GetLayerInfo()->GetPreBuffer();
        auto consumer = layer->GetLayerInfo()->GetSurface();
        releaseBuffer(preBuffer, SyncFence::InvalidFence(), consumer);
    }
}

void HdiOutput::ReleaseLayers(sptr<SyncFence>& releaseFence)
{
    auto layerPresentTimestamp = [](const LayerInfoPtr& layer, const sptr<IConsumerSurface>& cSurface) -> void {
        if (!layer->IsSupportedPresentTimestamp()) {
            return;
        }
        const auto& buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            return;
        }
        if (cSurface->SetPresentTimestamp(buffer->GetSeqNum(), layer->GetPresentTimestamp()) != GSERROR_OK) {
            HLOGD("LayerPresentTimestamp: SetPresentTimestamp failed");
        }
    };

    // get present timestamp from and set present timestamp to cSurface
    std::unique_lock<std::mutex> lock(mutex_);
    for (const auto& [id, layer] : layerIdMap_) {
        if (layer == nullptr || layer->GetLayerInfo() == nullptr || layer->GetLayerInfo()->GetSurface() == nullptr) {
            HLOGW("HdiOutput::ReleaseLayers: layer or layerInfo or layer's cSurface is nullptr");
            continue;
        }
        layerPresentTimestamp(layer->GetLayerInfo(), layer->GetLayerInfo()->GetSurface());
    }
    ReleaseSurfaceBuffer(releaseFence);
}

std::map<LayerInfoPtr, sptr<SyncFence>> HdiOutput::GetLayersReleaseFence()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return GetLayersReleaseFenceLocked();
}

std::map<LayerInfoPtr, sptr<SyncFence>> HdiOutput::GetLayersReleaseFenceLocked()
{
    std::map<LayerInfoPtr, sptr<SyncFence>> res;
    size_t layerNum = layersId_.size();
    for (size_t i = 0; i < layerNum; i++) {
        auto iter = layerIdMap_.find(layersId_[i]);
        if (iter == layerIdMap_.end()) {
            HLOGE("Invalid hdi layer id [%{public}u]", layersId_[i]);
            continue;
        }

        const LayerPtr &layer = iter->second;
        if (isMergeFenceSkipped_ && isMergeFenceSkippedDfx_) {
            layer->SetReleaseFence(fences_[i]);
            res[layer->GetLayerInfo()] = fences_[i];
        } else {
            layer->MergeWithLayerFence(fences_[i]);
            res[layer->GetLayerInfo()] = layer->GetReleaseFence();
        }
    }
    return res;
}

int32_t HdiOutput::StartVSyncSampler(bool forceReSample)
{
    if (sampler_ == nullptr) {
        sampler_ = CreateVSyncSampler();
    }
    if (sampler_->StartSample(forceReSample) == VSYNC_ERROR_OK) {
        return GRAPHIC_DISPLAY_SUCCESS;
    } else {
        return GRAPHIC_DISPLAY_FAILURE;
    }
}

void HdiOutput::SetPendingMode(int64_t period, int64_t timestamp)
{
    ScopedBytrace func("VSyncSampler::SetPendingMode period:" + std::to_string(period) +
                        ", timestamp:" + std::to_string(timestamp));
    if (sampler_ == nullptr) {
        sampler_ = CreateVSyncSampler();
    }
    sampler_->SetPendingPeriod(period);
    CreateVSyncGenerator()->SetPendingMode(period, timestamp);
}

void HdiOutput::Dump(std::string &result) const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    std::unique_lock<std::mutex> lock(mutex_);
    ReorderLayerInfoLocked(dumpLayerInfos);

    result.append("\n");
    result.append("-- LayerInfo\n");

    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        if (layer == nullptr || layer->GetLayerInfo() == nullptr) {
            continue;
        }
        auto surface = layer->GetLayerInfo()->GetSurface();
        const std::string& name = surface ? surface->GetName() :
            "Layer Without Surface" + std::to_string(layer->GetLayerInfo()->GetZorder());
        auto info = layer->GetLayerInfo();
        result += "\n surface [" + name + "] NodeId[" + std::to_string(layerInfo.nodeId) + "]";
        result +=  " LayerId[" + std::to_string(layer->GetLayerId()) + "]:\n";
        info->Dump(result);
    }

    if (fbSurface_ != nullptr) {
        result += "\n";
        result += "FrameBufferSurface\n";
        fbSurface_->Dump(result);
    }
    CreateVSyncGenerator()->Dump(result);
    CreateVSyncSampler()->Dump(result);
}

void HdiOutput::DumpCurrentFrameLayers() const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    std::unique_lock<std::mutex> lock(mutex_);
    ReorderLayerInfoLocked(dumpLayerInfos);

    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        if (layer == nullptr || layer->GetLayerInfo() == nullptr ||
            layer->GetLayerInfo()->GetSurface() == nullptr) {
            continue;
        }
        auto info = layer->GetLayerInfo();
        info->DumpCurrentFrameLayer();
    }
}

void HdiOutput::DumpFps(std::string &result, const std::string &arg) const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    std::unique_lock<std::mutex> lock(mutex_);
    ReorderLayerInfoLocked(dumpLayerInfos);
    result.append("\n");
    if (arg == "composer") {
        result += "The fps of screen [Id:" + std::to_string(screenId_) + "] is:\n";
        const int32_t offset = compTimeRcdIndex_;
        for (uint32_t i = 0; i < COMPOSITION_RECORDS_NUM; i++) {
            uint32_t order = (offset + COMPOSITION_RECORDS_NUM - i - 1) % COMPOSITION_RECORDS_NUM;
            result += std::to_string(compositionTimeRecords_[order]) + "\n";
        }
        return;
    }

    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        if (arg == "UniRender") {
            if (layer->GetLayerInfo()->GetUniRenderFlag()) {
                result += "\n surface [" + arg + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
                layer->DumpMergedResult(result);
                break;
            }
            continue;
        }
        if (layer->GetLayerInfo()->GetSurface() == nullptr) {
            continue;
        }
        const std::string& name = layer->GetLayerInfo()->GetSurface()->GetName();
        if (name == arg) {
            result += "\n surface [" + name + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
            layer->Dump(result);
        }
        if (layer->GetLayerInfo()->GetUniRenderFlag()) {
            auto windowsName = layer->GetLayerInfo()->GetWindowsName();
            auto iter = std::find(windowsName.begin(), windowsName.end(), arg);
            if (iter != windowsName.end()) {
                result += "\n window [" + arg + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
                layer->DumpByName(arg, result);
            }
        }
    }
}

void HdiOutput::DumpHitchs(std::string &result, const std::string &arg) const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    std::unique_lock<std::mutex> lock(mutex_);
    ReorderLayerInfoLocked(dumpLayerInfos);
    result.append("\n");
    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        if (layer->GetLayerInfo()->GetUniRenderFlag()) {
            result += "\n window [" + arg + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
            layer->SelectHitchsInfo(arg, result);
        }
    }
}

void HdiOutput::ClearFpsDump(std::string &result, const std::string &arg)
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    std::unique_lock<std::mutex> lock(mutex_);
    ReorderLayerInfoLocked(dumpLayerInfos);

    result.append("\n");
    if (arg == "composer") {
        result += "The fps info of screen [Id:" + std::to_string(screenId_) + "] is cleared.\n";
        compositionTimeRecords_.fill(0);
        return;
    }

    for (const LayerDumpInfo &layerInfo : dumpLayerInfos) {
        const LayerPtr &layer = layerInfo.layer;
        if (layer == nullptr || layer->GetLayerInfo() == nullptr ||
            layer->GetLayerInfo()->GetSurface() == nullptr) {
            result += "layer is null.\n";
            return;
        }
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

void HdiOutput::ReorderLayerInfoLocked(std::vector<LayerDumpInfo> &dumpLayerInfos) const
{
    for (const auto& [surfaceId, layer] : surfaceIdMap_) {
        if (layer == nullptr || layer->GetLayerInfo() == nullptr) {
            continue;
        }
        struct LayerDumpInfo layerInfo = {
            .nodeId = layer->GetLayerInfo()->GetNodeId(),
            .surfaceId = surfaceId,
            .layer = layer,
        };
        dumpLayerInfos.emplace_back(layerInfo);
    }

    for (const auto& [solidSurfaceId, solidLayer] : solidSurfaceIdMap_) {
        if (solidLayer == nullptr || solidLayer->GetLayerInfo() == nullptr) {
            continue;
        }
        struct LayerDumpInfo layerInfo = {
            .nodeId = solidLayer->GetLayerInfo()->GetNodeId(),
            .surfaceId = solidSurfaceId,
            .layer = solidLayer,
        };

        dumpLayerInfos.emplace_back(layerInfo);
    }

    std::sort(dumpLayerInfos.begin(), dumpLayerInfos.end(), Cmp);
}

int HdiOutput::GetBufferCacheSize()
{
    return bufferCache_.size();
}

void HdiOutput::ClearBufferCache()
{
    if (bufferCache_.empty()) {
        return;
    }
    int32_t ret = device_->ClearClientBuffer(screenId_);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGD("Call hdi ClearClientBuffer failed, ret is %{public}d", ret);
    }
    bufferCache_.clear();
}

void HdiOutput::SetActiveRectSwitchStatus(bool flag)
{
    isActiveRectSwitching_ = flag;
}

void HdiOutput::InitLoadOptParams(LoadOptParamsForHdiOutput& loadOptParamsForHdiOutput)
{
    loadOptParamsForHdiOutput_ = loadOptParamsForHdiOutput;
    auto switchParams = loadOptParamsForHdiOutput_.switchParams;

    isMergeFenceSkipped_ = (switchParams.find(IS_MERGE_FENCE_SKIPPED) != switchParams.end())
                               ? switchParams.at(IS_MERGE_FENCE_SKIPPED)
                               : false;
    HLOGD("[%{public}s] %{public}s is %{public}d", __func__, IS_MERGE_FENCE_SKIPPED.c_str(), isMergeFenceSkipped_);
}

void HdiOutput::ANCOTransactionOnComplete(const LayerInfoPtr& layerInfo, const sptr<SyncFence>& previousReleaseFence)
{
    if (layerInfo == nullptr) {
        return;
    }
    if (layerInfo->IsAncoNative()) {
        auto consumer = layerInfo->GetSurface();
        auto curBuffer = layerInfo->GetBuffer();
        if (consumer == nullptr || curBuffer == nullptr) {
            return;
        }
        consumer->ReleaseBuffer(curBuffer, previousReleaseFence);
    }
}
} // namespace Rosen
} // namespace OHOS
