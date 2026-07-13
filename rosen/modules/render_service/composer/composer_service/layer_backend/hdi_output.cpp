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

#include <algorithm>
#include <cstdint>
#include <set>
#include <unistd.h>
#include "common/rs_common_hook.h"
#include "hdi_log.h"
#include "hdi_output.h"
#include "metadata_helper.h"
#include "rs_trace.h"
#include "string_utils.h"
// DISPLAYENGINE
#include "sync_fence_tracker.h"
#include "syspara/parameters.h"
#include "vsync_generator.h"
#include "vsync_sampler.h"

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
static constexpr uint32_t DEFAULT_TUNNEL_LAYER_THRESHOLD = 2;
static const std::string GENERIC_METADATA_KEY_ARSR_PRE_NEEDED = "ArsrDoEnhance";
static const std::string GENERIC_METADATA_KEY_COPYBIT_NEEDED = "TryToDoCopybit";
static constexpr size_t MATRIX_SIZE = 9;
static const std::string GENERIC_METADATA_KEY_DISPLAY_LINEAR_MATRIX = "DisplayLinearMatrix";

using TunnelLayerKey = std::pair<uint32_t, uintptr_t>;
std::mutex g_tunnelLayerMutex;
std::set<TunnelLayerKey> g_tunnelLayerKeys;

static bool IsTunnelHdiLayer(const std::shared_ptr<HdiLayer>& hdiLayer)
{
    return hdiLayer != nullptr &&
        hdiLayer->GetCreatedLayerType() == GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL;
}

static bool RegisterTunnelHdiLayer(uint32_t screenId, const std::shared_ptr<HdiLayer>& hdiLayer)
{
    if (!IsTunnelHdiLayer(hdiLayer)) {
        return true;
    }
    TunnelLayerKey key{screenId, reinterpret_cast<uintptr_t>(hdiLayer.get())};
    std::lock_guard<std::mutex> lock(g_tunnelLayerMutex);
    if (g_tunnelLayerKeys.count(key) > 0) {
        return true;
    }
    if (g_tunnelLayerKeys.size() >= DEFAULT_TUNNEL_LAYER_THRESHOLD) {
        return false;
    }
    g_tunnelLayerKeys.emplace(key);
    return true;
}

static void UnregisterTunnelHdiLayer(uint32_t screenId, const std::shared_ptr<HdiLayer>& hdiLayer)
{
    if (!IsTunnelHdiLayer(hdiLayer)) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_tunnelLayerMutex);
    g_tunnelLayerKeys.erase(TunnelLayerKey(screenId, reinterpret_cast<uintptr_t>(hdiLayer.get())));
}

static uint32_t GetTunnelLayerCount()
{
    std::lock_guard<std::mutex> lock(g_tunnelLayerMutex);
    return static_cast<uint32_t>(g_tunnelLayerKeys.size());
}

static void DemoteRSLayerFromTunnel(const std::shared_ptr<RSLayer>& rsLayer)
{
    if (rsLayer == nullptr) {
        return;
    }
    rsLayer->SetTunnelLayerId(0);
    rsLayer->SetTunnelLayerProperty(TUNNEL_PROP_INVALID);
    rsLayer->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
}

static std::shared_ptr<RSLayer> GetReplacedTunnelRSLayer(
    const std::shared_ptr<HdiLayer>& hdiLayer, bool fallbackRequested)
{
    if (fallbackRequested || !IsTunnelHdiLayer(hdiLayer)) {
        return nullptr;
    }
    return hdiLayer->GetRSLayer();
}

std::shared_ptr<HdiOutput> HdiOutput::CreateHdiOutput(uint32_t screenId)
{
    return std::make_shared<HdiOutput>(screenId);
}

HdiOutput::HdiOutput(uint32_t screenId) : screenId_(screenId)
{
    // DISPLAYENGINE ARSR_PRE FLAG
    HLOGI("HdiOutput(screenId_:%{public}" PRIu32 ")", screenId);
    arsrPreEnabled_ = system::GetBoolParameter("const.display.enable_arsr_pre", true);
    arsrPreEnabledForVm_ = system::GetBoolParameter("const.display.enable_arsr_pre_for_vm", false);
    vmArsrWhiteList_ = system::GetParameter("const.display.vmlayer.whitelist", "unknown");
}

HdiOutput::~HdiOutput()
{
    HLOGI("~HdiOutput screenId_:%{public}" PRIu32, screenId_);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        UnregisterGlobalTunnelLayersLocked();
    }
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

    {
        std::lock_guard<std::mutex> lock(backlightDeviceMutex_);
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

    std::lock_guard<std::mutex> lock(backlightDeviceMutex_);
    if (device_ != nullptr) {
        HLOGW("HdiDevice has been changed");
        return ROSEN_ERROR_OK;
    }
    device_ = device;
    return ROSEN_ERROR_OK;
}

void HdiOutput::SetRSLayers(const std::vector<std::shared_ptr<RSLayer>>& rsLayers)
{
    uint32_t solidLayerCount = 0;
    std::unique_lock<std::mutex> lock(mutex_);
    for (const auto& rsLayer : rsLayers) {
        UpdateRSLayerLocked(rsLayer, solidLayerCount);
    }

    DeletePrevLayersLocked();
    ResetLayerStatusLocked();
}

void HdiOutput::DestroyLayerBySurfaceIdLocked(uint64_t surfaceId)
{
    auto iter = surfaceIdMap_.find(surfaceId);
    if (iter == surfaceIdMap_.end()) {
        return;
    }

    auto hdiLayer = iter->second;
    surfaceIdMap_.erase(iter);
    if (hdiLayer != nullptr) {
        layerIdMap_.erase(hdiLayer->GetLayerId());
        layersTobeRelease_.remove(hdiLayer);
        UnregisterTunnelHdiLayer(screenId_, hdiLayer);
    }
}

void HdiOutput::UpdateRSLayerLocked(const std::shared_ptr<RSLayer>& rsLayer, uint32_t& solidLayerCount)
{
    if (rsLayer == nullptr) {
        HLOGE("current rsLayer is null");
        return;
    }
    if (rsLayer->GetIsMaskLayer()) {
        DirtyRegions(rsLayer);
        return;
    }
    if (UpdateSolidColorLayerLocked(rsLayer, solidLayerCount)) {
        return;
    }
    uint64_t surfaceId = rsLayer->GetSurfaceUniqueId();
    bool fallbackRequested = ShouldFallbackTunnelLayerLocked(surfaceId, rsLayer);
    if (fallbackRequested) {
        uint64_t failedGeneration = rsLayer->GetTunnelLayerGeneration();
        if (!IsTunnelDeclinedLocked(surfaceId, failedGeneration)) {
            OnLayerCreated(rsLayer->GetNodeId(), false, failedGeneration);
        }
        DemoteRSLayerFromTunnel(rsLayer);
        MarkTunnelDeclinedLocked(surfaceId, failedGeneration);
    }
    auto iter = surfaceIdMap_.find(surfaceId);
    if (invalidTunnelSurfaceIds_.find(surfaceId) != invalidTunnelSurfaceIds_.end() &&
        IsTunnelLayerRequestedLocked(rsLayer)) {
        DestroyLayerBySurfaceIdLocked(surfaceId);
        CreateLayerLocked(surfaceId, rsLayer);
        return;
    }
    if (iter != surfaceIdMap_.end()) {
        const std::shared_ptr<HdiLayer>& hdiLayer = iter->second;
        if (hdiLayer != nullptr && hdiLayer->GetCreatedLayerType() != rsLayer->GetType()) {
            auto previousRsLayer = GetReplacedTunnelRSLayer(hdiLayer, fallbackRequested);
            if (previousRsLayer != nullptr) {
                OnLayerCreated(previousRsLayer->GetNodeId(), false, previousRsLayer->GetTunnelLayerGeneration());
            }
            DestroyLayerBySurfaceIdLocked(surfaceId);
            CreateLayerLocked(surfaceId, rsLayer);
            return;
        }
        hdiLayer->UpdateRSLayer(rsLayer);
        layerIdMap_[hdiLayer->GetLayerId()] = hdiLayer;
        if (IsTunnelHdiLayer(hdiLayer) && IsTunnelLayerRequestedLocked(rsLayer)) {
            hdiLayer->MarkPendingTunnelLayerCreated(rsLayer->GetTunnelLayerGeneration());
        }
        return;
    }
    CreateLayerLocked(surfaceId, rsLayer);
}

bool HdiOutput::UpdateSolidColorLayerLocked(const std::shared_ptr<RSLayer>& rsLayer, uint32_t& solidLayerCount)
{
    if (rsLayer->GetCompositionType() != GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
        return false;
    }
    auto iter = solidSurfaceIdMap_.find(solidLayerCount);
    if (iter != solidSurfaceIdMap_.end()) {
        const std::shared_ptr<HdiLayer>& hdiLayer = iter->second;
        hdiLayer->UpdateRSLayer(rsLayer);
        solidLayerCount++;
        return true;
    }
    CreateLayerLocked(solidLayerCount++, rsLayer);
    return true;
}

void HdiOutput::DirtyRegions(const std::shared_ptr<RSLayer>& rsLayer)
{
    std::vector<GraphicIRect> dirtyRegions;
    if (maskLayer_) {
        dirtyRegions.emplace_back(GraphicIRect {0, 0, 0, 0});
        rsLayer->SetDirtyRegions(dirtyRegions);
        maskLayer_->UpdateRSLayer(rsLayer);
    } else {
        auto layerSize = rsLayer->GetLayerSize();
        dirtyRegions.emplace_back(layerSize);
        rsLayer->SetDirtyRegions(dirtyRegions);
        CreateLayerLocked(rsLayer->GetZorder(), rsLayer);
    }
}

void HdiOutput::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    RS_TRACE_NAME_FMT("HdiOutput::CleanLayerBufferById, screenId=%u, surfaceId=%lu", screenId_, surfaceId);
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = surfaceIdMap_.find(surfaceId);
    if (iter == surfaceIdMap_.end()) {
        return;
    }
    const std::shared_ptr<HdiLayer>& hdiLayer = iter->second;
    if (hdiLayer) {
        hdiLayer->ClearBufferCache();
    }
}

void HdiOutput::UnregisterGlobalTunnelLayersLocked() const
{
    for (const auto& [_, hdiLayer] : surfaceIdMap_) {
        UnregisterTunnelHdiLayer(screenId_, hdiLayer);
    }
}

void HdiOutput::MarkTunnelSurfaceInvalid(uint64_t surfaceId)
{
    if (surfaceId == 0) {
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    invalidTunnelSurfaceIds_.insert(surfaceId);
}

void HdiOutput::EraseTunnelSurfaceInvalid(uint64_t surfaceId)
{
    if (surfaceId == 0) {
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    invalidTunnelSurfaceIds_.erase(surfaceId);
}

uint64_t HdiOutput::GetNodeIdBySurfaceId(uint64_t surfaceId) const
{
    if (surfaceId == 0) {
        return 0;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = surfaceIdMap_.find(surfaceId);
    std::shared_ptr<HdiLayer> hdiLayer = iter == surfaceIdMap_.end() ? nullptr : iter->second;
    if (hdiLayer == nullptr) {
        return 0;
    }
    auto rsLayer = hdiLayer->GetRSLayer();
    if (rsLayer == nullptr) {
        return 0;
    }
    return rsLayer->GetNodeId();
}

uint64_t HdiOutput::GetTunnelLayerGenerationBySurfaceId(uint64_t surfaceId) const
{
    if (surfaceId == 0) {
        return 0;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = surfaceIdMap_.find(surfaceId);
    std::shared_ptr<HdiLayer> hdiLayer = iter == surfaceIdMap_.end() ? nullptr : iter->second;
    if (hdiLayer == nullptr) {
        return 0;
    }
    auto rsLayer = hdiLayer->GetRSLayer();
    if (rsLayer == nullptr) {
        return 0;
    }
    return rsLayer->GetTunnelLayerGeneration();
}

int32_t HdiOutput::DestroyLayerBySurfaceId(uint64_t surfaceId)
{
    if (surfaceId == 0) {
        return GRAPHIC_DISPLAY_SUCCESS;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    DestroyLayerBySurfaceIdLocked(surfaceId);
    return GRAPHIC_DISPLAY_SUCCESS;
}

int32_t HdiOutput::CommitTunnelLayerBySurfaceId(uint64_t surfaceId, uint64_t tunnelLayerId,
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, sptr<SyncFence>& releaseFence)
{
    CHECK_DEVICE_NULL(device_);
    if (surfaceId == 0 || tunnelLayerId == 0 || buffer == nullptr) {
        HLOGE("%{public}s invalid param, surfaceId:%{public}" PRIu64 ", tunnelLayerId:%{public}" PRIu64,
            __func__, surfaceId, tunnelLayerId);
        return GRAPHIC_DISPLAY_PARAM_ERR;
    }

    std::shared_ptr<HdiLayer> hdiLayer = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto iter = surfaceIdMap_.find(surfaceId);
        if (iter != surfaceIdMap_.end()) {
            hdiLayer = iter->second;
        }
        if (hdiLayer == nullptr) {
            HLOGE("%{public}s can not find hdiLayer, surfaceId:%{public}" PRIu64, __func__, surfaceId);
            return GRAPHIC_DISPLAY_FAILURE;
        }
    }
    const BufferHandle* handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        HLOGE("%{public}s get buffer handle failed, surfaceId:%{public}" PRIu64, __func__, surfaceId);
        return GRAPHIC_DISPLAY_FAILURE;
    }

    int32_t ret = device_->SetTunnelLayerBuffer(screenId_, tunnelLayerId, handle,
        acquireFence == nullptr ? -1 : acquireFence->Get());
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("%{public}s SetTunnelLayerBuffer failed, surfaceId:%{public}" PRIu64 ", ret:%{public}d",
            __func__, surfaceId, ret);
        return ret;
    }

    int32_t releaseFenceFd = -1;
    ret = device_->CommitTunnelLayer(screenId_, tunnelLayerId, releaseFenceFd);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("%{public}s CommitTunnelLayer failed, surfaceId:%{public}" PRIu64 ", ret:%{public}d",
            __func__, surfaceId, ret);
        return ret;
    }

    if (releaseFenceFd >= 0) {
        releaseFence = new SyncFence(releaseFenceFd);
    } else {
        releaseFence = SyncFence::InvalidFence();
    }
    if (IsTagEnabled(HITRACE_TAG_GRAPHIC_AGP)) {
        static SyncFenceTracker tracker("Tunnel Release Fence");
        tracker.TrackFence(releaseFence);
    }
    return GRAPHIC_DISPLAY_SUCCESS;
}

void HdiOutput::DeletePrevLayersLocked()
{
    if (maskLayer_ && !maskLayer_->GetLayerStatus()) {
        maskLayer_ = nullptr;
    }
    auto solidSurfaceIter = solidSurfaceIdMap_.begin();
    while (solidSurfaceIter != solidSurfaceIdMap_.end()) {
        const std::shared_ptr<HdiLayer>& hdiLayer = solidSurfaceIter->second;
        if (!hdiLayer->GetLayerStatus()) {
            solidSurfaceIdMap_.erase(solidSurfaceIter++);
            if (hdiLayer->GetRSLayer() != nullptr) {
                solidRSLayerIdMap_.erase(hdiLayer->GetRSLayer()->GetRSLayerId());
            }
        } else {
            ++solidSurfaceIter;
        }
    }

    auto surfaceIter = surfaceIdMap_.begin();
    while (surfaceIter != surfaceIdMap_.end()) {
        const std::shared_ptr<HdiLayer>& hdiLayer = surfaceIter->second;
        if (!hdiLayer->GetLayerStatus()) {
            UnregisterTunnelHdiLayer(screenId_, hdiLayer);
            ClearTunnelDeclinedLocked(surfaceIter->first);
            surfaceIdMap_.erase(surfaceIter++);
        } else {
            ++surfaceIter;
        }
    }

    auto layerIter = layerIdMap_.begin();
    while (layerIter != layerIdMap_.end()) {
        const std::shared_ptr<HdiLayer>& hdiLayer = layerIter->second;
        if (!hdiLayer->GetLayerStatus()) {
            layerIdMap_.erase(layerIter++);
        } else {
            ++layerIter;
        }
    }

    auto iter = layersTobeRelease_.begin();
    while (iter != layersTobeRelease_.end()) {
        const std::shared_ptr<HdiLayer>& hdiLayer = *iter;
        if (!hdiLayer->GetLayerStatus()) {
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
        auto rsLayer = iter->second->GetRSLayer();
        if (rsLayer != nullptr) {
            rsLayer->SetIsNeedComposition(false);
        }
    }
    for (auto iter = layersTobeRelease_.begin(); iter != layersTobeRelease_.end(); ++iter) {
        (*iter)->SetLayerStatus(false);
        auto rsLayer = (*iter)->GetRSLayer();
        if (rsLayer != nullptr) {
            rsLayer->SetIsNeedComposition(false);
        }
    }
    if (maskLayer_) {
        maskLayer_->SetLayerStatus(false);
        auto rsLayer = maskLayer_->GetRSLayer();
        if (rsLayer != nullptr) {
            rsLayer->SetIsNeedComposition(false);
        }
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

bool HdiOutput::FallbackTunnelLayerToGraphicLocked(const std::shared_ptr<HdiLayer>& hdiLayer,
    const std::shared_ptr<RSLayer>& rsLayer) const
{
    if (hdiLayer == nullptr || !IsTunnelLayerRequestedLocked(rsLayer)) {
        return false;
    }

    GraphicLayerType originalType = rsLayer->GetType();
    uint64_t originalTunnelLayerId = rsLayer->GetTunnelLayerId();
    uint32_t originalTunnelLayerProperty = rsLayer->GetTunnelLayerProperty();
    rsLayer->SetTunnelLayerId(0);
    rsLayer->SetTunnelLayerProperty(TUNNEL_PROP_INVALID);
    rsLayer->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
    if (hdiLayer->Init(rsLayer)) {
        return true;
    }

    rsLayer->SetTunnelLayerId(originalTunnelLayerId);
    rsLayer->SetTunnelLayerProperty(originalTunnelLayerProperty);
    rsLayer->SetType(originalType);
    return false;
}

void HdiOutput::RegisterCreatedLayerLocked(uint64_t surfaceId, const std::shared_ptr<HdiLayer>& hdiLayer,
    const std::shared_ptr<RSLayer>& rsLayer, bool shouldEmitTunnelCreated)
{
    hdiLayer->UpdateRSLayer(rsLayer);

    if (rsLayer->GetIsMaskLayer()) {
        maskLayer_ = hdiLayer;
    }

    uint32_t layerId = hdiLayer->GetLayerId();
    layerIdMap_[layerId] = hdiLayer;

    if (rsLayer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
        // solid hdiLayer's surfaceId is unique, use solidLayerCount as key, to avoid conflict with normal hdiLayer
        solidSurfaceIdMap_[surfaceId] = hdiLayer;
        solidRSLayerIdMap_[rsLayer->GetRSLayerId()] = hdiLayer;
    } else {
        surfaceIdMap_[surfaceId] = hdiLayer;
    }
    RegisterTunnelHdiLayer(screenId_, hdiLayer);
    if (IsTunnelHdiLayer(hdiLayer)) {
        ClearTunnelDeclinedLocked(surfaceId);
        if (shouldEmitTunnelCreated) {
            hdiLayer->MarkPendingTunnelLayerCreated(rsLayer->GetTunnelLayerGeneration());
        }
    }
}

void HdiOutput::SetLayerPerFrameParameters(uint32_t layerId, const std::shared_ptr<RSLayer>& rsLayer)
{
    if (device_ == nullptr) {
        HLOGE("[%{public}s]HdiDevice is nullptr.", __func__);
        return;
    }

    if ((arsrPreEnabledForVm_ && CheckSupportArsrPreMetadata() && CheckIfDoArsrPreForVm(rsLayer)) ||
        (arsrPreEnabled_ && CheckSupportArsrPreMetadata() && CheckIfDoArsrPre(rsLayer))) {
        const std::vector<int8_t> valueBlob{static_cast<int8_t>(1)};
        if (device_->SetLayerPerFrameParameter(screenId_,
            layerId, GENERIC_METADATA_KEY_ARSR_PRE_NEEDED, valueBlob) != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGE("SetLayerPerFrameParameter Fail!");
        }
    }

    if (rsLayer->GetLayerCopybit() && CheckSupportCopybitMetadata()) {
        const std::vector<int8_t> valueBlob{static_cast<int8_t>(1)};
        if (device_->SetLayerPerFrameParameter(screenId_,
            layerId, GENERIC_METADATA_KEY_COPYBIT_NEEDED, valueBlob) != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGE("SetLayerPerFrameParameter Fail!");
        }
    }
}

int32_t HdiOutput::CreateLayerLocked(uint64_t surfaceId, const std::shared_ptr<RSLayer>& rsLayer)
{
    uint64_t nodeId = rsLayer != nullptr ? rsLayer->GetNodeId() : 0;
    bool tunnelLayerRequested = IsTunnelLayerRequestedLocked(rsLayer);

    auto createFunc = createHdiLayerFunc_ ? createHdiLayerFunc_ : HdiLayer::CreateHdiLayer;
    std::shared_ptr<HdiLayer> hdiLayer = createFunc(screenId_);
    if (hdiLayer == nullptr) {
        if (tunnelLayerRequested) {
            OnLayerCreated(nodeId, false, rsLayer->GetTunnelLayerGeneration());
        }
        HLOGE("Create hdiLayer failed");
        return GRAPHIC_DISPLAY_FAILURE;
    }
    bool initSucceeded = hdiLayer->Init(rsLayer);
    bool tunnelReserved = initSucceeded && RegisterTunnelHdiLayer(screenId_, hdiLayer);
    bool tunnelCreationSucceeded = tunnelLayerRequested && initSucceeded && tunnelReserved;
    if (!initSucceeded || (IsTunnelHdiLayer(hdiLayer) && !tunnelReserved)) {
        bool fallbackSucceeded = FallbackTunnelLayerToGraphicLocked(hdiLayer, rsLayer);
        if (tunnelLayerRequested) {
            uint64_t failedGeneration = rsLayer->GetTunnelLayerGeneration();
            if (!IsTunnelDeclinedLocked(surfaceId, failedGeneration)) {
                OnLayerCreated(nodeId, false, failedGeneration);
            }
            MarkTunnelDeclinedLocked(surfaceId, failedGeneration);
        }
        if (!fallbackSucceeded) {
            if (!initSucceeded) {
                hdiLayer->UpdateRSLayer(rsLayer);
            }
            layersTobeRelease_.emplace_back(hdiLayer);
            HLOGE("Init hdiLayer failed");
            return GRAPHIC_DISPLAY_FAILURE;
        }
    }

    RegisterCreatedLayerLocked(surfaceId, hdiLayer, rsLayer, tunnelCreationSucceeded);
    SetLayerPerFrameParameters(hdiLayer->GetLayerId(), rsLayer);
    return GRAPHIC_DISPLAY_SUCCESS;
}

void HdiOutput::SetOutputDamages(const std::vector<GraphicIRect>& outputDamages)
{
    outputDamages_ = outputDamages;
}

const std::vector<GraphicIRect>& HdiOutput::GetOutputDamages()
{
    return outputDamages_;
}

void HdiOutput::GetComposeClientLayers(std::vector<std::shared_ptr<HdiLayer>>& clientLayers)
{
    std::unique_lock<std::mutex> lock(mutex_);
    for (const auto& [first, hdiLayer] : layerIdMap_) {
        if (hdiLayer == nullptr || hdiLayer->GetRSLayer() == nullptr) {
            continue;
        }
        if (hdiLayer->GetRSLayer()->GetHdiCompositionType() ==
                GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT ||
            hdiLayer->GetRSLayer()->GetHdiCompositionType() ==
                GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT_CLEAR ||
            hdiLayer->GetRSLayer()->GetHdiCompositionType() ==
                GraphicCompositionType::GRAPHIC_COMPOSITION_TUNNEL) {
            clientLayers.emplace_back(hdiLayer);
        }
    }
}

void HdiOutput::GetRSLayers(std::vector<std::shared_ptr<RSLayer>>& rsLayers)
{
    std::unique_lock<std::mutex> lock(mutex_);
    for (const auto& [_, hdiLayer] : layerIdMap_) {
        if (hdiLayer != nullptr) {
            rsLayers.emplace_back(hdiLayer->GetRSLayer());
        }
    }
}

void HdiOutput::UpdatePrevRSLayerLocked()
{
    RS_TRACE_NAME_FMT("HdiOutput::UpdatePrevRSLayerLocked, layerIdMap size %u", layerIdMap_.size());
    for (const auto& [_, hdiLayer] : layerIdMap_) {
        hdiLayer->SavePrevRSLayer();
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

    for (const auto& [_, hdiLayer] : layerIdMap_) {
        ret = hdiLayer->SetHdiLayerInfo(isActiveRectSwitching_);
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGE("Set hdi hdiLayer[id:%{public}d] info failed, ret %{public}u.", hdiLayer->GetLayerId(), ret);
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
            HLOGE("Invalid hdi hdiLayer id[%{public}u]", layersId[i]);
            continue;
        }

        const std::shared_ptr<HdiLayer>& hdiLayer = iter->second;
        hdiLayer->UpdateCompositionType(static_cast<GraphicCompositionType>(types[i]));
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
bool HdiOutput::CheckIfDoArsrPre(const std::shared_ptr<RSLayer>& rsLayer)
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
        "UnityPlayerSurface",
    };

    if (rsLayer == nullptr || rsLayer->GetBuffer() == nullptr) {
        return false;
    }

    if (((yuvFormats.count(static_cast<GraphicPixelFormat>(rsLayer->GetBuffer()->GetFormat())) > 0) ||
        (videoLayers.count(rsLayer->GetSurfaceName()) > 0)) && rsLayer->GetLayerArsr()) {
        return true;
    }

    return false;
}

bool HdiOutput::CheckIfDoArsrPreForVm(const std::shared_ptr<RSLayer>& rsLayer)
{
    char sep = ';';
    std::unordered_set<std::string> vmLayers;
    SplitString(vmArsrWhiteList_, vmLayers, sep);
    if (vmLayers.count(rsLayer->GetSurfaceName()) > 0) {
        return true;
    }
    return false;
}

int32_t HdiOutput::FlushScreen(std::vector<std::shared_ptr<HdiLayer>>& compClientLayers)
{
    auto fbEntry = GetFramebuffer();
    if (fbEntry == nullptr) {
        HLOGE("HdiBackend flush screen failed : GetFramebuffer failed!");
        return -1;
    }

    const auto& fbAcquireFence = fbEntry->acquireFence;
    for (auto& hdiLayer : compClientLayers) {
        if (hdiLayer != nullptr) {
            hdiLayer->MergeWithFramebufferFence(fbAcquireFence);
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

int32_t HdiOutput::Commit(sptr<SyncFence>& fbFence)
{
    CHECK_DEVICE_NULL(device_);
    return device_->Commit(screenId_, fbFence);
}

int32_t HdiOutput::CommitAndGetReleaseFence(
    sptr<SyncFence>& fbFence, int32_t& skipState, bool& needFlush, bool isValidated)
{
    CHECK_DEVICE_NULL(device_);
    layersId_.clear();
    fences_.clear();
    int32_t ret = device_->CommitAndGetReleaseFence(
        screenId_, fbFence, skipState, needFlush, layersId_, fences_, isValidated);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        for (const auto& [id, layer] : layerIdMap_) {
            if (layer) {
                layer->ClearBufferCache();
            }
        }
        ClearBufferCache();
    }
    return ret;
}

void HdiOutput::UpdateThirdFrameAheadPresentFence(sptr<SyncFence>& fbFence)
{
    RS_TRACE_NAME_FMT("presentFenceIndex_ = %d", presentFenceIndex_);
    if (historicalPresentfences_.size() == NUMBER_OF_HISTORICAL_FRAMES) {
        thirdFrameAheadPresentFence_ = historicalPresentfences_[presentFenceIndex_];
        historicalPresentfences_[presentFenceIndex_] = fbFence;
        presentFenceIndex_ = (presentFenceIndex_ + 1) % NUMBER_OF_HISTORICAL_FRAMES;
    } else {
        RS_TRACE_NAME("historicalPresentfences_ push_back");
        historicalPresentfences_.push_back(fbFence);
    }
}

int32_t HdiOutput::UpdateInfosAfterCommit(sptr<SyncFence> fbFence)
{
    RS_TRACE_NAME("HdiOutput::UpdateInfosAfterCommit");
    std::unique_lock<std::mutex> lock(mutex_);
    UpdatePrevRSLayerLocked();

    if (sampler_ == nullptr) {
        sampler_ = CreateVSyncSampler();
    }
    if (thirdFrameAheadPresentFence_ == nullptr) {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }
    thirdFrameAheadPresentFenceFd_ = -1;
    thirdFrameAheadPresentTime_ = SyncFence::FENCE_PENDING_TIMESTAMP;
    int64_t timestamp;
    {
        RS_TRACE_NAME("HdiOutput::SyncFileReadTimestamp");
        timestamp = thirdFrameAheadPresentFence_->SyncFileReadTimestamp();
    }
    bool startSample = false;
    if (timestamp != SyncFence::FENCE_PENDING_TIMESTAMP) {
        startSample = sampler_->GetVsyncSamplerEnabled() && sampler_->AddPresentFenceTime(screenId_, timestamp);
        RecordCompositionTime(timestamp);
        bool presentTimeUpdated = false;
        std::shared_ptr<HdiLayer> uniRenderLayer = nullptr;
        for (auto iter = layerIdMap_.begin(); iter != layerIdMap_.end(); ++iter) {
            const std::shared_ptr<HdiLayer>& hdiLayer = iter->second;
            RS_TRACE_NAME_FMT("HdiOutput::Iterate layerIdMap_ %u", iter->first);
            if (hdiLayer->RecordPresentTime(timestamp)) {
                presentTimeUpdated = true;
            }
            if (hdiLayer->GetRSLayer() && hdiLayer->GetRSLayer()->GetUniRenderFlag()) {
                uniRenderLayer = hdiLayer;
            }
        }
        if (presentTimeUpdated && uniRenderLayer) {
            RS_TRACE_NAME_FMT("HdiOutput::RecordMergedPresentTime %lld", timestamp);
            uniRenderLayer->RecordMergedPresentTime(timestamp);
        }
        thirdFrameAheadPresentFenceFd_ = thirdFrameAheadPresentFence_->Get();
        thirdFrameAheadPresentTime_ = timestamp;
    }

    int32_t ret = GRAPHIC_DISPLAY_SUCCESS;
    if (startSample) {
        ret = StartVSyncSampler();
    }
    UpdateThirdFrameAheadPresentFence(fbFence);
    if (fbFence != nullptr) {
        curPresentFd_ = fbFence->Get();
    }
    rsComposerJankStats.CalculateJankInfo(timestamp);
    return ret;
}

void HdiOutput::FinalizePostCommit(bool commitSucceeded)
{
    std::vector<LayerCreatedInfo> createdInfos;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (commitSucceeded) {
            ClearRecoveredInvalidTunnelSurfaceIdsLocked();
            createdInfos = CollectPendingLayerCreatedInfosLocked();
        }
    }
    for (const auto& info : createdInfos) {
        OnLayerCreated(info.nodeId, true, info.tunnelLayerGeneration);
    }
}

void HdiOutput::ClearRecoveredInvalidTunnelSurfaceIdsLocked()
{
    auto iter = invalidTunnelSurfaceIds_.begin();
    while (iter != invalidTunnelSurfaceIds_.end()) {
        auto layerIter = surfaceIdMap_.find(*iter);
        if (layerIter == surfaceIdMap_.end() || layerIter->second == nullptr) {
            ++iter;
            continue;
        }
        iter = invalidTunnelSurfaceIds_.erase(iter);
    }
}

std::vector<LayerCreatedInfo> HdiOutput::CollectPendingLayerCreatedInfosLocked()
{
    std::vector<LayerCreatedInfo> layerCreatedInfos;
    layerCreatedInfos.reserve(layerIdMap_.size());
    for (const auto& [_, hdiLayer] : layerIdMap_) {
        uint64_t tunnelLayerGeneration = 0;
        if (hdiLayer == nullptr || !hdiLayer->TakePendingTunnelLayerCreated(tunnelLayerGeneration)) {
            continue;
        }
        auto rsLayer = hdiLayer->GetRSLayer();
        if (rsLayer == nullptr || !IsTunnelLayerRequestedLocked(rsLayer)) {
            continue;
        }
        if (rsLayer->GetHdiCompositionType() != GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE) {
            continue;
        }
        invalidTunnelSurfaceIds_.erase(rsLayer->GetSurfaceUniqueId());
        LayerCreatedInfo layerCreatedInfo;
        layerCreatedInfo.nodeId = rsLayer->GetNodeId();
        layerCreatedInfo.tunnelLayerGeneration = tunnelLayerGeneration;
        layerCreatedInfos.emplace_back(layerCreatedInfo);
    }
    return layerCreatedInfos;
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

void HdiOutput::ReleaseLayers(ReleaseLayerBuffersInfo& releaseLayerInfo)
{
    std::unique_lock<std::mutex> lock(mutex_);
    // get present timestamp from and set present timestamp to cSurface
    for (const auto& [id, hdiLayer] : layerIdMap_) {
        if (hdiLayer == nullptr || hdiLayer->GetRSLayer() == nullptr) {
            HLOGW("%{public}s: hdiLayer or rsLayer is nullptr", __func__);
            continue;
        }
        releaseLayerInfo.timestampVec.push_back(std::tuple(hdiLayer->GetRSLayer()->GetRSLayerId(),
            hdiLayer->GetRSLayer()->GetIsSupportedPresentTimestamp(),
            hdiLayer->GetRSLayer()->GetPresentTimestamp()));
    }
    std::unordered_map<RSLayerId, sptr<SyncFence>> releaseBufferFenceMap;
    const auto layersReleaseFence = GetLayersReleaseFenceLocked();
    // lambda for releasing original buffer in device offline mode
    auto releaseOriginalBuffer = [&releaseLayerInfo](const std::shared_ptr<RSLayer>& rsLayer,
        const sptr<SyncFence>& fence) {
        if (!rsLayer->GetUseDeviceOffline()) {
            return;
        }
        auto origPreBuffer = rsLayer->GetHpaeOriginalInfo().originalPreBuffer;
        releaseLayerInfo.releaseBufferFenceVec.push_back(
            std::tuple(rsLayer->GetRSLayerId(), origPreBuffer, fence));
    };

    if (layersReleaseFence.size() == 0) {
        // When release fence's size is 0, the output may invalid, release all buffer
        // This situation may happen when killing composer_host
        for (const auto& [id, hdiLayer] : layerIdMap_) {
            if (hdiLayer == nullptr || hdiLayer->GetRSLayer() == nullptr) {
                HLOGD("%{public}s: hdiLayer or rsLayer is nullptr", __func__);
                continue;
            }
            releaseBufferFenceMap[hdiLayer->GetRSLayer()->GetRSLayerId()] = SyncFence::InvalidFence();
            releaseLayerInfo.releaseBufferFenceVec.push_back(std::tuple(hdiLayer->GetRSLayer()->GetRSLayerId(),
                hdiLayer->GetRSLayer()->GetPreBuffer(), SyncFence::InvalidFence()));
            // hpae_offline: release original buffer for device offline
            releaseOriginalBuffer(hdiLayer->GetRSLayer(), SyncFence::InvalidFence());
        }
        HLOGD("%{public}s: no layer needs to release", __func__);
    } else {
        for (const auto& [rsLayer, fence] : layersReleaseFence) {
            if (rsLayer != nullptr) {
                releaseBufferFenceMap[rsLayer->GetRSLayerId()] = fence;
                RS_OPTIONAL_TRACE_NAME_FMT("%s releaseBufferFenceVec bufferId %" PRIu64
                    " Fence %d", __func__, rsLayer->GetPreBuffer() ? rsLayer->GetPreBuffer()->GetBufferId() : 0,
                    fence ? fence->Get() : -1);
                if (rsLayer->IsAncoNative()) {
                    releaseLayerInfo.releaseBufferFenceVec.push_back(std::tuple(rsLayer->GetRSLayerId(),
                        rsLayer->GetBuffer(), fence));
                } else {
                    releaseLayerInfo.releaseBufferFenceVec.push_back(std::tuple(rsLayer->GetRSLayerId(),
                        rsLayer->GetPreBuffer(), fence));
                }
                // hpae_offline: release original buffer for device offline
                releaseOriginalBuffer(rsLayer, fence);
            }
        }
    }

    for (const auto& hdiLayer : layersTobeRelease_) {
        if (hdiLayer == nullptr || hdiLayer->GetRSLayer() == nullptr) {
            continue;
        }
        if (releaseBufferFenceMap.find(hdiLayer->GetRSLayer()->GetRSLayerId()) == releaseBufferFenceMap.end()) {
            releaseLayerInfo.releaseBufferFenceVec.push_back(std::tuple(hdiLayer->GetRSLayer()->GetRSLayerId(),
                hdiLayer->GetRSLayer()->GetPreBuffer(), SyncFence::InvalidFence()));
            // hpae_offline: release original buffer for device offline
            releaseOriginalBuffer(hdiLayer->GetRSLayer(), SyncFence::InvalidFence());
        }
    }
}

std::unordered_map<std::shared_ptr<RSLayer>, sptr<SyncFence>> HdiOutput::GetLayersReleaseFence()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return GetLayersReleaseFenceLocked();
}

std::unordered_map<std::shared_ptr<RSLayer>, sptr<SyncFence>> HdiOutput::GetLayersReleaseFenceLocked()
{
    std::unordered_map<std::shared_ptr<RSLayer>, sptr<SyncFence>> res;
    size_t layerNum = layersId_.size();
    for (size_t i = 0; i < layerNum; i++) {
        auto iter = layerIdMap_.find(layersId_[i]);
        if (iter == layerIdMap_.end()) {
            HLOGE("Invalid hdi hdiLayer id [%{public}u]", layersId_[i]);
            continue;
        }

        const std::shared_ptr<HdiLayer>& hdiLayer = iter->second;
        hdiLayer->MergeWithLayerFence(fences_[i]);
        res.emplace(hdiLayer->GetRSLayer(), hdiLayer->GetReleaseFence());
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
    RS_TRACE_NAME_FMT("VSyncSampler::SetPendingMode period:%" PRId64 ", timestamp:%" PRId64, period, timestamp);
    if (sampler_ == nullptr) {
        sampler_ = CreateVSyncSampler();
    }
    sampler_->SetPendingPeriod(period);
    CreateVSyncGenerator()->SetPendingMode(period, timestamp);
}

// only used for separate rendering
void HdiOutput::DumpLayerInfoForSplitRender(std::string& result) const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    ReorderLayerInfoLocked(dumpLayerInfos);

    result.append("\n");
    result.append("-- LayerInfo ScreenId: " + std::to_string(screenId_) + "\n");

    for (const LayerDumpInfo& layerInfo : dumpLayerInfos) {
        const std::shared_ptr<HdiLayer>& hdiLayer = layerInfo.hdiLayer;
        if (hdiLayer == nullptr || hdiLayer->GetRSLayer() == nullptr) {
            continue;
        }
        auto surface = hdiLayer->GetRSLayer()->GetSurface();
        const std::string& name = surface ? surface->GetName() :
            "Layer Without Surface" + std::to_string(hdiLayer->GetRSLayer()->GetZorder());
        auto info = hdiLayer->GetRSLayer();
        result += "\n surface [" + name + "] NodeId[" + std::to_string(layerInfo.nodeId) + "]";
        result += " LayerId[" + std::to_string(hdiLayer->GetLayerId()) + "]:\n";
        info->Dump(result);
    }
}

void HdiOutput::Dump(std::string& result, bool isSplitRender) const
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (isSplitRender) {
        DumpLayerInfoForSplitRender(result);
    }
    if (fbSurface_ != nullptr) {
        result += "\n";
        result += "FrameBufferSurface\n";
        fbSurface_->Dump(result);
    }
    CreateVSyncGenerator()->Dump(result);
    CreateVSyncSampler()->Dump(result);
}

// only used for separate rendering
void HdiOutput::DumpCurrentFrameLayers() const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    std::unique_lock<std::mutex> lock(mutex_);
    ReorderLayerInfoLocked(dumpLayerInfos);

    for (const LayerDumpInfo& layerInfo : dumpLayerInfos) {
        const std::shared_ptr<HdiLayer>& hdiLayer = layerInfo.hdiLayer;
        if (hdiLayer == nullptr || hdiLayer->GetRSLayer() == nullptr ||
            hdiLayer->GetRSLayer()->GetSurface() == nullptr) {
            continue;
        }
        auto info = hdiLayer->GetRSLayer();
        info->DumpCurrentFrameLayer();
    }
}

void HdiOutput::DumpFps(std::string& result, const std::string& arg) const
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

    for (const LayerDumpInfo& layerInfo : dumpLayerInfos) {
        const std::shared_ptr<HdiLayer>& hdiLayer = layerInfo.hdiLayer;
        if (arg == "UniRender") {
            if (hdiLayer->GetRSLayer()->GetUniRenderFlag()) {
                result += "\n surface [" + arg + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
                hdiLayer->DumpMergedResult(result);
                break;
            }
            continue;
        }
        const std::string& name = hdiLayer->GetRSLayer()->GetSurfaceName();
        if (name == arg) {
            result += "\n surface [" + name + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
            hdiLayer->Dump(result);
        }
        if (hdiLayer->GetRSLayer()->GetUniRenderFlag()) {
            const auto& windowsName = hdiLayer->GetRSLayer()->GetWindowsName();
            if (std::find(windowsName.begin(), windowsName.end(), arg) != windowsName.end()) {
                result += "\n window [" + arg + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
                hdiLayer->DumpByName(arg, result);
            }
        }
    }
}

void HdiOutput::DumpHitchs(std::string& result, const std::string& arg) const
{
    std::vector<LayerDumpInfo> dumpLayerInfos;
    std::unique_lock<std::mutex> lock(mutex_);
    ReorderLayerInfoLocked(dumpLayerInfos);
    result.append("\n");
    for (const LayerDumpInfo& layerInfo : dumpLayerInfos) {
        const std::shared_ptr<HdiLayer>& hdiLayer = layerInfo.hdiLayer;
        if (hdiLayer->GetRSLayer()->GetUniRenderFlag()) {
            result += "\n window [" + arg + "] Id[" + std::to_string(layerInfo.surfaceId) + "]:\n";
            hdiLayer->SelectHitchsInfo(arg, result);
        }
    }
}

void HdiOutput::ClearFpsDump(std::string& result, const std::string& arg)
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

    for (const LayerDumpInfo& layerInfo : dumpLayerInfos) {
        const std::shared_ptr<HdiLayer>& hdiLayer = layerInfo.hdiLayer;
        if (hdiLayer == nullptr || hdiLayer->GetRSLayer() == nullptr) {
            result += "layer is null.\n";
            return;
        }
        const std::string& name = hdiLayer->GetRSLayer()->GetSurfaceName();
        if (name == arg) {
            result += "\n The fps info of surface [" + name + "] Id["
                + std::to_string(layerInfo.surfaceId) + "] is cleared.\n";
            hdiLayer->ClearDump();
        }
    }
}

static inline bool Cmp(const LayerDumpInfo& layer1, const LayerDumpInfo& layer2)
{
    return layer1.hdiLayer->GetRSLayer()->GetZorder() < layer2.hdiLayer->GetRSLayer()->GetZorder();
}

void HdiOutput::ReorderLayerInfoLocked(std::vector<LayerDumpInfo>& dumpLayerInfos) const
{
    for (const auto& [surfaceId, hdiLayer] : surfaceIdMap_) {
        if (hdiLayer == nullptr || hdiLayer->GetRSLayer() == nullptr) {
            continue;
        }
        struct LayerDumpInfo layerInfo = {
            .nodeId = hdiLayer->GetRSLayer()->GetNodeId(),
            .surfaceId = surfaceId,
            .hdiLayer = hdiLayer,
        };
        dumpLayerInfos.emplace_back(layerInfo);
    }

    for (const auto& [solidSurfaceId, solidLayer] : solidSurfaceIdMap_) {
        if (solidLayer == nullptr || solidLayer->GetRSLayer() == nullptr) {
            continue;
        }
        struct LayerDumpInfo layerInfo = {
            .nodeId = solidLayer->GetRSLayer()->GetNodeId(),
            .surfaceId = solidSurfaceId,
            .hdiLayer = solidLayer,
        };

        dumpLayerInfos.emplace_back(layerInfo);
    }

    std::sort(dumpLayerInfos.begin(), dumpLayerInfos.end(), Cmp);
}

int HdiOutput::GetBufferCacheSize() const
{
    return bufferCache_.size();
}

void HdiOutput::ClearBufferCache()
{
    if (bufferCache_.empty()) {
        return;
    }
    bufferCache_.clear();
    if (device_ == nullptr) {
        HLOGE("HdiOutput ClearBufferCache failed : HdiDevice is null");
        return;
    }
    int32_t ret = device_->ClearClientBuffer(screenId_);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGD("Call hdi ClearClientBuffer failed, ret is %{public}d", ret);
    }
}

void HdiOutput::SetActiveRectSwitchStatus(bool flag, const GraphicIRect& activeRect)
{
    if (flag) {
        int32_t ret = device_->SetScreenActiveRect(screenId_, activeRect);
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGD("Call hdi SetScreenActiveRect failed, ret is %{public}d", ret);
        }
    }
    isActiveRectSwitching_ = flag;
}

RosenError HdiOutput::RegPrepareComplete(OnPrepareCompleteFunc func, void* data)
{
    if (func == nullptr) {
        HLOGE("OnPrepareCompleteFunc is null");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    onPrepareCompleteCb_ = func;
    onPrepareCompleteCbData_ = data;

    return ROSEN_ERROR_OK;
}

RosenError HdiOutput::RegLayerCreated(OnLayerCreatedFunc func, void* data)
{
    if (func == nullptr) {
        HLOGE("OnLayerCreatedFunc is null");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    onLayerCreatedCb_ = func;
    onLayerCreatedCbData_ = data;
    return ROSEN_ERROR_OK;
}

void HdiOutput::ClearLayerCreatedCallback()
{
    onLayerCreatedCb_ = nullptr;
    onLayerCreatedCbData_ = nullptr;
}

void HdiOutput::OnLayerCreated(uint64_t nodeId, bool success, uint64_t tunnelLayerGeneration)
{
    RS_TRACE_NAME_FMT("HdiOutput::OnLayerCreated screenId=%u nodeId=%" PRIu64
        " success=%d generation=%" PRIu64, screenId_, nodeId, static_cast<int32_t>(success),
        tunnelLayerGeneration);
    HLOGI("%{public}s screenId:%{public}" PRIu32 ", nodeId:%{public}" PRIu64
        ", success:%{public}d, generation:%{public}" PRIu64,
        __func__, screenId_, nodeId, static_cast<int32_t>(success), tunnelLayerGeneration);
    if (onLayerCreatedCb_ == nullptr) {
        return;
    }
    onLayerCreatedCb_(nodeId, success, tunnelLayerGeneration, onLayerCreatedCbData_);
}

bool HdiOutput::IsTunnelLayerRequestedLocked(const std::shared_ptr<RSLayer>& rsLayer) const
{
    return rsLayer != nullptr && rsLayer->GetType() == GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL &&
        rsLayer->GetTunnelLayerId() != 0;
}

bool HdiOutput::ShouldFallbackTunnelLayerLocked(uint64_t surfaceId, const std::shared_ptr<RSLayer>& rsLayer) const
{
    if (!IsTunnelLayerRequestedLocked(rsLayer)) {
        return false;
    }
    if (IsTunnelDeclinedLocked(surfaceId, rsLayer->GetTunnelLayerGeneration())) {
        return true;
    }
    auto iter = surfaceIdMap_.find(surfaceId);
    if (iter != surfaceIdMap_.end() && IsTunnelHdiLayer(iter->second)) {
        return false;
    }
    return GetTunnelLayerCount() >= DEFAULT_TUNNEL_LAYER_THRESHOLD;
}

bool HdiOutput::IsTunnelDeclinedLocked(uint64_t surfaceId, uint64_t generation) const
{
    auto iter = tunnelFallbackGenerations_.find(surfaceId);
    return iter != tunnelFallbackGenerations_.end() && iter->second == generation;
}

void HdiOutput::MarkTunnelDeclinedLocked(uint64_t surfaceId, uint64_t generation)
{
    tunnelFallbackGenerations_[surfaceId] = generation;
}

void HdiOutput::ClearTunnelDeclinedLocked(uint64_t surfaceId)
{
    tunnelFallbackGenerations_.erase(surfaceId);
}

static inline bool CmpZorder(const std::shared_ptr<RSLayer>& rsLayer1, const std::shared_ptr<RSLayer>& rsLayer2)
{
    if (rsLayer1 == nullptr || rsLayer2 == nullptr) {
        return false;
    }
    return rsLayer1->GetZorder() < rsLayer2->GetZorder();
}

void HdiOutput::ReorderRSLayers(std::vector<std::shared_ptr<RSLayer>>& newRSLayer)
{
    std::sort(newRSLayer.begin(), newRSLayer.end(), CmpZorder);
}

void HdiOutput::OnPrepareComplete(bool needFlush, std::vector<std::shared_ptr<RSLayer>>& newRSLayers)
{
    if (needFlush) {
        ReorderRSLayers(newRSLayers);
    }

    if (onPrepareCompleteCb_ != nullptr) {
        struct PrepareCompleteParam param = {
            .needFlushFramebuffer = needFlush,
            .layers = newRSLayers,
        };

        auto fbSurface = GetFrameBufferSurface();
        onPrepareCompleteCb_(fbSurface, param, onPrepareCompleteCbData_);
    }
}

int32_t HdiOutput::PrepareCompleteIfNeed(bool needFlush)
{
    std::vector<std::shared_ptr<HdiLayer>> compClientLayers;
    GetComposeClientLayers(compClientLayers);
    if (compClientLayers.size() > 0) {
        needFlush = true;
        HLOGD("Need flush framebuffer, client composition hdiLayer num is %{public}zu", compClientLayers.size());
    }

    std::vector<std::shared_ptr<RSLayer>> newRSLayers;
    GetRSLayers(newRSLayers);
    OnPrepareComplete(needFlush, newRSLayers);
    if (needFlush) {
        return FlushScreen(compClientLayers);
    }
    return GRAPHIC_DISPLAY_SUCCESS;
}

void HdiOutput::Repaint()
{
    RS_TRACE_NAME("Repaint");
    HLOGD("%{public}s: start", __func__);

    int32_t ret = PreProcessLayersComp();
    GraphicIRect activeRect = {0};
    SetActiveRectSwitchStatus(false, activeRect);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("PreProcessLayersComp failed, ret is %{public}d", ret);
        return;
    }

    sptr<SyncFence> fbFence = SyncFence::InvalidFence();
    bool needFlush = false;
    int32_t skipState = INT32_MAX;
    ret = CommitAndGetReleaseFence(fbFence, skipState, needFlush, false);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("first commit failed, ret is %{public}d, skipState is %{public}d", ret, skipState);
    }

    if (skipState != GRAPHIC_DISPLAY_SUCCESS) {
        ret = UpdateLayerCompType();
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            return;
        }
        ret = PrepareCompleteIfNeed(needFlush);
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            return;
        }
        skipState = INT32_MAX;
        ret = CommitAndGetReleaseFence(fbFence, skipState, needFlush, true);
        HLOGD("%{public}s: ValidateDisplay", __func__);
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGE("second commit failed, ret is %{public}d skipState is %{public}d", ret, skipState);
        }
    }

    if (IsTagEnabled(HITRACE_TAG_GRAPHIC_AGP)) {
        auto tracker = SyncFenceTrackerManager::GetSyncFenceTracker(
            "PresentFence_" + std::to_string(screenId_), screenId_);
        if (tracker) {
            tracker->TrackFence(fbFence);
        } else {
            HLOGD("SyncFenceTrackerManager::GetSyncFenceTracker failed!");
        }
    }

    bool commitSucceeded = ret == GRAPHIC_DISPLAY_SUCCESS;
    ret = UpdateInfosAfterCommit(fbFence);
    FinalizePostCommit(commitSucceeded);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return;
    }

    ret = ReleaseFramebuffer(fbFence);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return;
    }
    HLOGD("%{public}s: end", __func__);
}

void HdiOutput::SetScreenBacklight(uint32_t level)
{
    std::lock_guard<std::mutex> lock(backlightDeviceMutex_);
    if (device_ == nullptr) {
        HLOGW("%{public}s: device_ is nullptr, screenId:%{public}u", __func__, screenId_);
        return;
    }
    int32_t ret = device_->SetScreenBacklight(screenId_, level);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGW("%{public}s failed, screenId:%{public}u, ret:%{public}d", __func__, screenId_, ret);
    }
}

void HdiOutput::SetScreenLinearMatrix(const std::vector<float>& matrix)
{
    if (device_ == nullptr) {
        HLOGE("%{public}s failed : HdiDevice is null", __func__);
        return;
    }
    if (matrix.size() != MATRIX_SIZE) {
        HLOGE("%{public}s failed : size is invalid", __func__);
        return;
    }
    std::vector<int8_t> valueBlob(MATRIX_SIZE * sizeof(float));
    if (memcpy_s(valueBlob.data(), valueBlob.size(), matrix.data(), MATRIX_SIZE * sizeof(float)) != EOK) {
        return;
    }
    int32_t ret = device_->SetDisplayPerFrameParameterSmq(screenId_,
        GENERIC_METADATA_KEY_DISPLAY_LINEAR_MATRIX, valueBlob);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGD("Call hdi SetDisplayPerFrameParameterSmq failed, ret is %{public}d", ret);
    }
}

int32_t HdiOutput::GetDisplayClientTargetProperty(int32_t& pixelFormat, int32_t& dataspace)
{
    int32_t ret = device_->GetDisplayClientTargetProperty(screenId_, pixelFormat, dataspace);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGD("Call hdi GetDisplayClientTargetProperty failed, ret is %{public}d", ret);
    }
    return ret;
}

int32_t HdiOutput::GetLayerSolidFilledColor(uint64_t layerId, uint32_t& solidFilledColor)
{
    auto iter = solidRSLayerIdMap_.find(layerId);
    if (iter != solidRSLayerIdMap_.end()) {
        int32_t ret = device_->GetLayerSolidFilledColor(screenId_, iter->second->GetLayerId(), solidFilledColor);
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGD("Call hdi GetLayerSolidFilledColor failed, ret is %{public}d", ret);
        }
        return ret;
    }
    HLOGE("%{public}s failed, not find hdi layer", __func__);
    return GRAPHIC_DISPLAY_FAILURE;
}
} // namespace Rosen
} // namespace OHOS
