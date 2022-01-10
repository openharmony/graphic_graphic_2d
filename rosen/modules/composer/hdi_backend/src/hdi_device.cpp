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

#include "hdi_device.h"

#include <mutex>

#include <scoped_bytrace.h>

#define CHECK_FUNC(device, deviceFunc)                                 \
    do {                                                               \
        if (device == nullptr || deviceFunc == nullptr) {              \
            HLOGE("can not find hdi func: %{public}s", __FUNCTION__);  \
            return DISPLAY_NULL_PTR;                                   \
        }                                                              \
    } while(0)

namespace OHOS {
namespace Rosen {

HdiDevice* HdiDevice::GetInstance()
{
    static HdiDevice instance;

    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    {
        if (instance.Init() != ROSEN_ERROR_OK) {
            return nullptr;
        }
    }

    return &instance;
}

HdiDevice::HdiDevice()
{
}

HdiDevice::~HdiDevice()
{
    Destory();
}

RosenError HdiDevice::Init()
{
    if (deviceFuncs_ == nullptr) {
        int32_t ret = DeviceInitialize(&deviceFuncs_);
        if (ret != DISPLAY_SUCCESS || deviceFuncs_ == nullptr) {
            HLOGE("DeviceInitialize failed, ret is %{public}d", ret);
            return ROSEN_ERROR_NOT_INIT;
        }
    }

    if (layerFuncs_ == nullptr) {
        int32_t ret = LayerInitialize(&layerFuncs_);
        if (ret != DISPLAY_SUCCESS || layerFuncs_ == nullptr) {
            Destory();
            HLOGE("LayerInitialize failed, ret is %{public}d", ret);
            return ROSEN_ERROR_NOT_INIT;
        }
    }

    return ROSEN_ERROR_OK;
}

void HdiDevice::Destory()
{
    if (deviceFuncs_ != nullptr) {
        int32_t ret = DeviceUninitialize(deviceFuncs_);
        if (ret != DISPLAY_SUCCESS) {
            HLOGE("DeviceUninitialize failed, ret is %{public}d", ret);
        }
        deviceFuncs_ = nullptr;
    }

    if (layerFuncs_ != nullptr) {
        int32_t ret = LayerUninitialize(layerFuncs_);
        if (ret != DISPLAY_SUCCESS) {
            HLOGE("LayerUninitialize failed, ret is %{public}d", ret);
        }
        layerFuncs_ = nullptr;
    }
}

/* set & get device screen info begin */
int32_t HdiDevice::RegHotPlugCallback(HotPlugCallback callback, void *data)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->RegHotPlugCallback);
    return deviceFuncs_->RegHotPlugCallback(callback, data);
}

int32_t HdiDevice::RegScreenVBlankCallback(uint32_t screenId, VBlankCallback callback, void *data)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->RegDisplayVBlankCallback);
    return deviceFuncs_->RegDisplayVBlankCallback(screenId, callback, data);
}

int32_t HdiDevice::SetScreenVsyncEnabled(uint32_t screenId, bool enabled)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayVsyncEnabled);
    return deviceFuncs_->SetDisplayVsyncEnabled(screenId, enabled);
}

int32_t HdiDevice::GetScreenCapability(uint32_t screenId, DisplayCapability &info)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayCapability);
    return deviceFuncs_->GetDisplayCapability(screenId, &info);
}

int32_t HdiDevice::GetScreenSuppportedModes(uint32_t screenId, std::vector<DisplayModeInfo> &modes)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplaySuppportedModes);

    int32_t num = 0;
    int32_t ret = deviceFuncs_->GetDisplaySuppportedModes(screenId, &num, nullptr);
    if (ret != DISPLAY_SUCCESS) {
        return ret;
    }

    if (num > 0) {
        modes.resize(num);
        return deviceFuncs_->GetDisplaySuppportedModes(screenId, &num, modes.data());
    }

    return ret;
}

int32_t HdiDevice::GetScreenMode(uint32_t screenId, uint32_t &modeId)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayMode);
    return deviceFuncs_->GetDisplayMode(screenId, &modeId);
}

int32_t HdiDevice::SetScreenMode(uint32_t screenId, uint32_t modeId)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayMode);
    return deviceFuncs_->SetDisplayMode(screenId, modeId);
}

int32_t HdiDevice::GetScreenPowerStatus(uint32_t screenId, DispPowerStatus &status)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayPowerStatus);
    return deviceFuncs_->GetDisplayPowerStatus(screenId, &status);
}

int32_t HdiDevice::SetScreenPowerStatus(uint32_t screenId, DispPowerStatus status)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayPowerStatus);
    return deviceFuncs_->SetDisplayPowerStatus(screenId, status);
}

int32_t HdiDevice::GetScreenBacklight(uint32_t screenId, uint32_t &level)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayBacklight);
    return deviceFuncs_->GetDisplayBacklight(screenId, &level);
}

int32_t HdiDevice::SetScreenBacklight(uint32_t screenId, uint32_t level)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayBacklight);
    return deviceFuncs_->SetDisplayBacklight(screenId, level);
}

int32_t HdiDevice::PrepareScreenLayers(uint32_t screenId, bool &needFlush)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->PrepareDisplayLayers);
    return deviceFuncs_->PrepareDisplayLayers(screenId, &needFlush);
}

int32_t HdiDevice::GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                       std::vector<int32_t> &types)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayCompChange);

    uint32_t layerNum = 0;
    int32_t ret = deviceFuncs_->GetDisplayCompChange(screenId, &layerNum, nullptr, nullptr);
    if (ret != DISPLAY_SUCCESS) {
        return ret;
    }

    if (layerNum > 0) {
        layersId.resize(layerNum);
        types.resize(layerNum);
        ret = deviceFuncs_->GetDisplayCompChange(screenId, &layerNum, layersId.data(), types.data());
    }

    return ret;
}

int32_t HdiDevice::SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer,
                                         const sptr<SyncFence> &fence)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayClientBuffer);

    if (fence == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    int32_t fenceFd = fence->Get();
    return deviceFuncs_->SetDisplayClientBuffer(screenId, buffer, fenceFd);
}

int32_t HdiDevice::SetScreenClientDamage(uint32_t screenId, uint32_t num, IRect &damageRect)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayClientDamage);
    return deviceFuncs_->SetDisplayClientDamage(screenId, num, &damageRect);
}

int32_t HdiDevice::GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layers,
                                         std::vector<sptr<SyncFence>> &fences)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayReleaseFence);

    uint32_t layerNum = 0;
    int32_t ret = deviceFuncs_->GetDisplayReleaseFence(screenId, &layerNum, nullptr, nullptr);
    if (ret != DISPLAY_SUCCESS) {
        return ret;
    }

    if (layerNum > 0) {
        layers.resize(layerNum);
        fences.resize(layerNum);
        std::vector<int32_t> fenceFds;
        fenceFds.resize(layerNum);

        ret = deviceFuncs_->GetDisplayReleaseFence(screenId, &layerNum,
                                                   layers.data(), fenceFds.data());

        for (uint32_t i = 0; i < fenceFds.size(); i++) {
            if (fenceFds[i] >= 0) {
                fences[i] = new SyncFence(fenceFds[i]);
            } else {
                fences[i] = new SyncFence(-1);
            }
        }
    }

    return ret;
}

int32_t HdiDevice::Commit(uint32_t screenId, sptr<SyncFence> &fence)
{
    ScopedBytrace bytrace(__func__);
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->Commit);

    int32_t fenceFd = -1;
    int32_t ret = deviceFuncs_->Commit(screenId, &fenceFd);

    if (fenceFd >= 0) {
        fence = new SyncFence(fenceFd);
    } else {
        fence = new SyncFence(-1);
    }

    return ret;
}
/* set & get device screen info end */

/* set & get device layer info begin */
int32_t HdiDevice::SetLayerAlpha(uint32_t screenId, uint32_t layerId, LayerAlpha &alpha)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerAlpha);
    return layerFuncs_->SetLayerAlpha(screenId, layerId, &alpha);
}

int32_t HdiDevice::SetLayerSize(uint32_t screenId, uint32_t layerId, IRect &layerRect)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerSize);
    return layerFuncs_->SetLayerSize(screenId, layerId, &layerRect);
}

int32_t HdiDevice::SetTransformMode(uint32_t screenId, uint32_t layerId, TransformType type)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetTransformMode);
    return layerFuncs_->SetTransformMode(screenId, layerId, type);
}

int32_t HdiDevice::SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId,
                                         uint32_t num, IRect &visable)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerVisibleRegion);
    return layerFuncs_->SetLayerVisibleRegion(screenId, layerId, num, &visable);
}

int32_t HdiDevice::SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId, IRect &dirty)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerDirtyRegion);
    return layerFuncs_->SetLayerDirtyRegion(screenId, layerId, &dirty);
}

int32_t HdiDevice::SetLayerBuffer(uint32_t screenId, uint32_t layerId, const BufferHandle *handle,
                                  const sptr<SyncFence> &acquireFence)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerBuffer);

    if (acquireFence == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    int32_t fenceFd = acquireFence->Get();
    return layerFuncs_->SetLayerBuffer(screenId, layerId, handle, fenceFd);
}

int32_t HdiDevice::SetLayerCompositionType(uint32_t screenId, uint32_t layerId, CompositionType type)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerCompositionType);
    return layerFuncs_->SetLayerCompositionType(screenId, layerId, type);
}

int32_t HdiDevice::SetLayerBlendType(uint32_t screenId, uint32_t layerId, BlendType type)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerBlendType);
    return layerFuncs_->SetLayerBlendType(screenId, layerId, type);
}

int32_t HdiDevice::SetLayerCrop(uint32_t screenId, uint32_t layerId, IRect &crop)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerCrop);
    return layerFuncs_->SetLayerCrop(screenId, layerId, &crop);
}

int32_t HdiDevice::SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerZorder);
    return layerFuncs_->SetLayerZorder(screenId, layerId, zorder);
}

int32_t HdiDevice::SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerPreMulti);
    return layerFuncs_->SetLayerPreMulti(screenId, layerId, isPreMulti);
}
/* set & get device layer info end */

int32_t HdiDevice::CreateLayer(uint32_t screenId, const LayerInfo &layerInfo, uint32_t &layerId)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->CreateLayer);
    return layerFuncs_->CreateLayer(screenId, &layerInfo, &layerId);
}

int32_t HdiDevice::CloseLayer(uint32_t screenId, uint32_t layerId)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->CloseLayer);
    return layerFuncs_->CloseLayer(screenId, layerId);
}

} // namespace Rosen
} // namespace OHOS