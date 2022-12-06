/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hdi_device_impl.h"
#include <cstdlib>
#include <mutex>
#include <scoped_bytrace.h>
#include <valarray>
#include <securec.h>
#define CHECK_FUNC(device, deviceFunc)                                 \
    do {                                                               \
        static CheckFunc checkFunc(device, deviceFunc, __FUNCTION__);  \
        if (!checkFunc()) {                                            \
            return GRAPHIC_DISPLAY_NULL_PTR;                           \
        }                                                              \
    } while(0) 

namespace OHOS {
namespace Rosen {
HdiDeviceImpl::HdiDeviceImpl()
{
}

HdiDeviceImpl::~HdiDeviceImpl()
{
    Destroy();
}

RosenError HdiDeviceImpl::Init()
{
    if (deviceFuncs_ == nullptr) {
        int32_t ret = DeviceInitialize(&deviceFuncs_);
        if (ret != GRAPHIC_DISPLAY_SUCCESS || deviceFuncs_ == nullptr) {
            HLOGE("DeviceInitialize failed, ret is %{public}d", ret);
            return ROSEN_ERROR_NOT_INIT;
        }
    }

    if (layerFuncs_ == nullptr) {
        int32_t ret = LayerInitialize(&layerFuncs_);
        if (ret != GRAPHIC_DISPLAY_SUCCESS || layerFuncs_ == nullptr) {
            Destroy();
            HLOGE("LayerInitialize failed, ret is %{public}d", ret);
            return ROSEN_ERROR_NOT_INIT;
        }
    }

    return ROSEN_ERROR_OK;
}

void HdiDeviceImpl::Destroy()
{
    if (deviceFuncs_ != nullptr) {
        int32_t ret = DeviceUninitialize(deviceFuncs_);
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGE("DeviceUninitialize failed, ret is %{public}d", ret);
        }
        deviceFuncs_ = nullptr;
    }

    if (layerFuncs_ != nullptr) {
        int32_t ret = LayerUninitialize(layerFuncs_);
        if (ret != GRAPHIC_DISPLAY_SUCCESS) {
            HLOGE("LayerUninitialize failed, ret is %{public}d", ret);
        }
        layerFuncs_ = nullptr;
    }
}

/* set & get device screen info begin */
int32_t HdiDeviceImpl::RegHotPlugCallback(HotPlugCallback callback, void *data)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->RegHotPlugCallback);
    return deviceFuncs_->RegHotPlugCallback(callback, data);
}

int32_t HdiDeviceImpl::RegScreenVBlankCallback(uint32_t screenId, VBlankCallback callback, void *data)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->RegDisplayVBlankCallback);
    return deviceFuncs_->RegDisplayVBlankCallback(screenId, callback, data);
}

int32_t HdiDeviceImpl::SetScreenVsyncEnabled(uint32_t screenId, bool enabled)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayVsyncEnabled);
    return deviceFuncs_->SetDisplayVsyncEnabled(screenId, enabled);
}

int32_t HdiDeviceImpl::GetScreenCapability(uint32_t screenId, GraphicDisplayCapability &info)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayCapability);
    DisplayCapability hdiInfo;
    int32_t ret = deviceFuncs_->GetDisplayCapability(screenId, &hdiInfo);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        if (strcpy_s(info.name, PROPERTY_NAME_LEN, hdiInfo.name)) {
            HLOGD("strcpy_s display capability info name failed.");
        }
        info.type = static_cast<GraphicInterfaceType>(hdiInfo.type);
        info.phyWidth = hdiInfo.phyWidth;
        info.phyHeight = hdiInfo.phyHeight;
        info.supportLayers = hdiInfo.supportLayers;
        info.virtualDispCount = hdiInfo.virtualDispCount;
        info.supportWriteBack = hdiInfo.supportWriteBack;
        info.propertyCount = hdiInfo.propertyCount;
        info.props = (GraphicPropertyObject*)malloc(info.propertyCount * sizeof(GraphicPropertyObject));
        for (uint32_t i = 0; i < info.propertyCount; i++) {
            if (strcpy_s(info.props[i].name, PROPERTY_NAME_LEN, hdiInfo.props[i].name)) {
                HLOGD("strcpy_s display capability props name failed.");
            }
            info.props[i].propId = hdiInfo.props[i].propId;
            info.props[i].value = hdiInfo.props[i].value;
        }
    }
    return ret;
}

int32_t HdiDeviceImpl::GetScreenSupportedModes(uint32_t screenId, std::vector<GraphicDisplayModeInfo> &modes)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplaySupportedModes);

    uint32_t num = 0;
    int32_t ret = deviceFuncs_->GetDisplaySupportedModes(screenId, &num, nullptr);
    if (ret != GRAPHIC_DISPLAY_SUCCESS || num <= 0) {
        return ret;
    }

    std::vector<DisplayModeInfo> hdiModes;
    hdiModes.resize(num);
    ret = deviceFuncs_->GetDisplaySupportedModes(screenId, &num, hdiModes.data());
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        modes.resize(num);
        for (uint32_t i = 0; i < num; i++) {
            modes[i].width = hdiModes[i].width;
            modes[i].height = hdiModes[i].height;
            modes[i].freshRate = hdiModes[i].freshRate;
            modes[i].id = hdiModes[i].id;
        }
    }

    return ret;
}

int32_t HdiDeviceImpl::GetScreenMode(uint32_t screenId, uint32_t &modeId)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayMode);
    return deviceFuncs_->GetDisplayMode(screenId, &modeId);
}

int32_t HdiDeviceImpl::SetScreenMode(uint32_t screenId, uint32_t modeId)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayMode);
    return deviceFuncs_->SetDisplayMode(screenId, modeId);
}

int32_t HdiDeviceImpl::GetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus &status)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayPowerStatus);
    DispPowerStatus hdiStatus;
    int32_t ret = deviceFuncs_->GetDisplayPowerStatus(screenId, &hdiStatus);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        status = static_cast<GraphicDispPowerStatus>(hdiStatus);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus status)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayPowerStatus);
    return deviceFuncs_->SetDisplayPowerStatus(screenId, static_cast<DispPowerStatus>(status));
}

int32_t HdiDeviceImpl::GetScreenBacklight(uint32_t screenId, uint32_t &level)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayBacklight);
    return deviceFuncs_->GetDisplayBacklight(screenId, &level);
}

int32_t HdiDeviceImpl::SetScreenBacklight(uint32_t screenId, uint32_t level)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayBacklight);
    return deviceFuncs_->SetDisplayBacklight(screenId, level);
}

int32_t HdiDeviceImpl::PrepareScreenLayers(uint32_t screenId, bool &needFlush)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->PrepareDisplayLayers);
    return deviceFuncs_->PrepareDisplayLayers(screenId, &needFlush);
}

int32_t HdiDeviceImpl::GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                           std::vector<int32_t> &types)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayCompChange);

    uint32_t layerNum = 0;
    int32_t ret = deviceFuncs_->GetDisplayCompChange(screenId, &layerNum, nullptr, nullptr);
    if (ret != GRAPHIC_DISPLAY_SUCCESS || layerNum <= 0) {
        return ret;
    }

    layersId.resize(layerNum);
    types.resize(layerNum);
    ret = deviceFuncs_->GetDisplayCompChange(screenId, &layerNum, layersId.data(), types.data());

    return ret;
}

int32_t HdiDeviceImpl::SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer,
                                             const sptr<SyncFence> &fence)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayClientBuffer);

    if (fence == nullptr) {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t fenceFd = fence->Get();
    return deviceFuncs_->SetDisplayClientBuffer(screenId, buffer, fenceFd);
}

int32_t HdiDeviceImpl::SetScreenClientDamage(uint32_t screenId, uint32_t num, GraphicIRect &damageRect)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayClientDamage);
    IRect hdiDamageRect = {
        .x = damageRect.x,
        .y = damageRect.y,
        .w = damageRect.w,
        .h = damageRect.h,
    };
    return deviceFuncs_->SetDisplayClientDamage(screenId, num, &hdiDamageRect);
}

int32_t HdiDeviceImpl::GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layers,
                                             std::vector<sptr<SyncFence>> &fences)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayReleaseFence);

    uint32_t layerNum = 0;
    int32_t ret = deviceFuncs_->GetDisplayReleaseFence(screenId, &layerNum, nullptr, nullptr);
    if (ret != GRAPHIC_DISPLAY_SUCCESS || layerNum <= 0) {
        return ret;
    }

    layers.resize(layerNum);
    std::vector<int32_t> fenceFds;
    fenceFds.resize(layerNum);
    ret = deviceFuncs_->GetDisplayReleaseFence(screenId, &layerNum, layers.data(), fenceFds.data());
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }

    fences.resize(layerNum);
    for (uint32_t i = 0; i < fenceFds.size(); i++) {
        if (fenceFds[i] >= 0) {
            fences[i] = new SyncFence(fenceFds[i]);
        } else {
            fences[i] = new SyncFence(-1);
        }
    }

    return ret;
}

int32_t HdiDeviceImpl::GetScreenSupportedColorGamuts(uint32_t screenId, std::vector<GraphicColorGamut> &gamuts)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplaySupportedColorGamuts);
    uint32_t num = 0;
    int32_t ret = deviceFuncs_->GetDisplaySupportedColorGamuts(screenId, &num, nullptr);
    if (ret != GRAPHIC_DISPLAY_SUCCESS || num <= 0) {
        return ret;
    }
    std::vector<ColorGamut> hdiGamuts;
    hdiGamuts.resize(num);
    ret = deviceFuncs_->GetDisplaySupportedColorGamuts(screenId, &num, hdiGamuts.data());
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        gamuts.resize(num);
        for (uint32_t i = 0; i < num; i++) {
            gamuts[i] = static_cast<GraphicColorGamut>(hdiGamuts[i]);
        }
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenColorGamut(uint32_t screenId, GraphicColorGamut gamut)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayColorGamut);
    return deviceFuncs_->SetDisplayColorGamut(screenId, static_cast<ColorGamut>(gamut));
}

int32_t HdiDeviceImpl::GetScreenColorGamut(uint32_t screenId, GraphicColorGamut &gamut)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayColorGamut);
    ColorGamut hdiGamut;
    int32_t ret = deviceFuncs_->GetDisplayColorGamut(screenId, &hdiGamut);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        gamut = static_cast<GraphicColorGamut>(hdiGamut);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenGamutMap(uint32_t screenId, GraphicGamutMap gamutMap)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayGamutMap);
    return deviceFuncs_->SetDisplayGamutMap(screenId, static_cast<GamutMap>(gamutMap));
}

int32_t HdiDeviceImpl::GetScreenGamutMap(uint32_t screenId, GraphicGamutMap &gamutMap)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetDisplayGamutMap);
    GamutMap hdiGamutMap;
    int32_t ret = deviceFuncs_->GetDisplayGamutMap(screenId, &hdiGamutMap);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        gamutMap = static_cast<GraphicGamutMap>(hdiGamutMap);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetScreenColorTransform(uint32_t screenId, const float *matrix)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->SetDisplayColorTransform);
    return deviceFuncs_->SetDisplayColorTransform(screenId, matrix);
}

int32_t HdiDeviceImpl::GetHDRCapabilityInfos(uint32_t screenId, GraphicHDRCapability &info)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetHDRCapabilityInfos);
    HDRCapability hdiInfo;
    int32_t ret = deviceFuncs_->GetHDRCapabilityInfos(screenId, &hdiInfo);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }
    uint32_t formatCount = hdiInfo.formatCount;
    info.formats = (GraphicHDRFormat*)std::malloc(formatCount * sizeof(GRAPHIC_NOT_SUPPORT_HDR));
    if (memset_s(info.formats, sizeof(info.formats), 0, sizeof(info.formats))) {
        HLOGD("memset_s failed when get HDRCapability format infos.");
    }
    for (uint32_t i = 0; i < formatCount; i++) {
        info.formats[i] = static_cast<GraphicHDRFormat>(hdiInfo.formats[i]);
    }
    info.maxLum = hdiInfo.maxLum;
    info.maxAverageLum = hdiInfo.maxAverageLum;
    info.minLum = hdiInfo.minLum;
    return ret;
}

int32_t HdiDeviceImpl::GetSupportedMetaDataKey(uint32_t screenId, std::vector<GraphicHDRMetadataKey> &keys)
{
    CHECK_FUNC(deviceFuncs_, deviceFuncs_->GetSupportedMetadataKey);
    uint32_t num = 0;
    int32_t ret = deviceFuncs_->GetSupportedMetadataKey(screenId, &num, nullptr);
    if (ret != GRAPHIC_DISPLAY_SUCCESS || num <= 0) {
        return ret;
    }
    std::vector<HDRMetadataKey> hdiKeys;
    hdiKeys.resize(num);
    ret = deviceFuncs_->GetSupportedMetadataKey(screenId, &num, hdiKeys.data());
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return ret;
    }

    keys.resize(num);
    for (uint32_t i = 0; i < num; i++) {
        keys[i] = static_cast<GraphicHDRMetadataKey>(hdiKeys[i]);
    }
    return ret;
}

int32_t HdiDeviceImpl::Commit(uint32_t screenId, sptr<SyncFence> &fence)
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
int32_t HdiDeviceImpl::SetLayerAlpha(uint32_t screenId, uint32_t layerId, GraphicLayerAlpha &alpha)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerAlpha);
    LayerAlpha hdiLayerAlpha = {
        .enGlobalAlpha = alpha.enGlobalAlpha,
        .enPixelAlpha = alpha.enPixelAlpha,
        .alpha0 = alpha.alpha0,
        .alpha1 = alpha.alpha1,
        .gAlpha = alpha.gAlpha,
    };
    return layerFuncs_->SetLayerAlpha(screenId, layerId, &hdiLayerAlpha);
}

int32_t HdiDeviceImpl::SetLayerSize(uint32_t screenId, uint32_t layerId, GraphicIRect &layerRect)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerSize);
    IRect hdiLayerRect = {
        .x = layerRect.x,
        .y = layerRect.y,
        .w = layerRect.w,
        .h = layerRect.h,
    };
    return layerFuncs_->SetLayerSize(screenId, layerId, &hdiLayerRect);
}

int32_t HdiDeviceImpl::SetTransformMode(uint32_t screenId, uint32_t layerId, GraphicTransformType type)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetTransformMode);
    TransformType hdiType = static_cast<TransformType>(type);
    return layerFuncs_->SetTransformMode(screenId, layerId, hdiType);
}

int32_t HdiDeviceImpl::SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId,
                                             uint32_t num, GraphicIRect &visible)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerVisibleRegion);
    IRect hdiVisibleRect = {
        .x = visible.x,
        .y = visible.y,
        .w = visible.w,
        .h = visible.h,
    };
    return layerFuncs_->SetLayerVisibleRegion(screenId, layerId, num, &hdiVisibleRect);
}

int32_t HdiDeviceImpl::SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId, GraphicIRect &dirty)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerDirtyRegion);
    IRect hdiDirtyRect = {
        .x = dirty.x,
        .y = dirty.y,
        .w = dirty.w,
        .h = dirty.h,
    };
    return layerFuncs_->SetLayerDirtyRegion(screenId, layerId, &hdiDirtyRect);
}

int32_t HdiDeviceImpl::SetLayerBuffer(uint32_t screenId, uint32_t layerId, const BufferHandle *handle,
                                      const sptr<SyncFence> &acquireFence)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerBuffer);

    if (acquireFence == nullptr) {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t fenceFd = acquireFence->Get();
    return layerFuncs_->SetLayerBuffer(screenId, layerId, handle, fenceFd);
}

int32_t HdiDeviceImpl::SetLayerCompositionType(uint32_t screenId, uint32_t layerId, GraphicCompositionType type)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerCompositionType);
    CompositionType hdiType = static_cast<CompositionType>(type);
    return layerFuncs_->SetLayerCompositionType(screenId, layerId, hdiType);
}

int32_t HdiDeviceImpl::SetLayerBlendType(uint32_t screenId, uint32_t layerId, GraphicBlendType type)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerBlendType);
    BlendType hdiBlendType = static_cast<BlendType>(type);
    return layerFuncs_->SetLayerBlendType(screenId, layerId, hdiBlendType);
}

int32_t HdiDeviceImpl::SetLayerCrop(uint32_t screenId, uint32_t layerId, GraphicIRect &crop)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerCrop);
    IRect hdiCropRect = {
        .x = crop.x,
        .y = crop.y,
        .w = crop.w,
        .h = crop.h,
    };
    return layerFuncs_->SetLayerCrop(screenId, layerId, &hdiCropRect);
}

int32_t HdiDeviceImpl::SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerZorder);
    return layerFuncs_->SetLayerZorder(screenId, layerId, zorder);
}

int32_t HdiDeviceImpl::SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerPreMulti);
    return layerFuncs_->SetLayerPreMulti(screenId, layerId, isPreMulti);
}

int32_t HdiDeviceImpl::SetLayerColorTransform(uint32_t screenId, uint32_t layerId, const float *matrix)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerColorTransform);
    return layerFuncs_->SetLayerColorTransform(screenId, layerId, matrix);
}

int32_t HdiDeviceImpl::SetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace colorSpace)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerColorDataSpace);
    ColorDataSpace hdiColorDataSpace = static_cast<ColorDataSpace>(colorSpace);
    return layerFuncs_->SetLayerColorDataSpace(screenId, layerId, hdiColorDataSpace);
}

int32_t HdiDeviceImpl::GetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace &colorSpace)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->GetLayerColorDataSpace);
    ColorDataSpace hdiColorDataSpace = COLOR_DATA_SPACE_UNKNOWN;
    int32_t ret = layerFuncs_->GetLayerColorDataSpace(screenId, layerId, &hdiColorDataSpace);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        colorSpace = static_cast<GraphicColorDataSpace>(hdiColorDataSpace);
    }
    return ret;
}

int32_t HdiDeviceImpl::SetLayerMetaData(uint32_t screenId, uint32_t layerId,
                                        const std::vector<GraphicHDRMetaData> &metaData)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerMetaData);
    std::vector<HDRMetaData> hdiMetaDatas;
    std::size_t metaDataSize = metaData.size();
    for (std::size_t i = 0; i < metaDataSize; i++) {
        HDRMetaData hdiMetaData = {
            .key = static_cast<HDRMetadataKey>(metaData[i].key),
            .value = metaData[i].value,
        };
        hdiMetaDatas.emplace_back(hdiMetaData);
    }
    return layerFuncs_->SetLayerMetaData(screenId, layerId, hdiMetaDatas.size(), hdiMetaDatas.data());
}

int32_t HdiDeviceImpl::SetLayerMetaDataSet(uint32_t screenId, uint32_t layerId, GraphicHDRMetadataKey key,
                                           const std::vector<uint8_t> &metaData)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerMetaDataSet);
    HDRMetadataKey hdiKey = static_cast<HDRMetadataKey>(key);
    return layerFuncs_->SetLayerMetaDataSet(screenId, layerId, hdiKey, metaData.size(), metaData.data());
}

int32_t HdiDeviceImpl::SetLayerTunnelHandle(uint32_t screenId, uint32_t layerId, OHExtDataHandle *handle)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->SetLayerTunnelHandle);
    return layerFuncs_->SetLayerTunnelHandle(screenId, layerId, reinterpret_cast<ExtDataHandle *>(handle));
}

int32_t HdiDeviceImpl::GetSupportedPresentTimestampType(uint32_t screenId, uint32_t layerId,
                                                        GraphicPresentTimestampType &type)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->GetSupportedPresentTimestamp);
    PresentTimestampType hdiType = PresentTimestampType::HARDWARE_DISPLAY_PTS_UNSUPPORTED;
    int32_t ret = layerFuncs_->GetSupportedPresentTimestamp(screenId, layerId, &hdiType);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        type = static_cast<GraphicPresentTimestampType>(hdiType);
    }
    return ret;
}

int32_t HdiDeviceImpl::GetPresentTimestamp(uint32_t screenId, uint32_t layerId, GraphicPresentTimestamp &timestamp)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->GetHwPresentTimestamp);
    PresentTimestamp hdiTimestamp = {HARDWARE_DISPLAY_PTS_UNSUPPORTED, 0};
    int32_t ret = layerFuncs_->GetHwPresentTimestamp(screenId, layerId, &hdiTimestamp);
    if (ret == GRAPHIC_DISPLAY_SUCCESS) {
        timestamp.time = hdiTimestamp.time;
        timestamp.type = static_cast<GraphicPresentTimestampType>(hdiTimestamp.type);
    }
    return ret;
}

/* set & get device layer info end */
int32_t HdiDeviceImpl::CreateLayer(uint32_t screenId, const GraphicLayerInfo &layerInfo, uint32_t &layerId)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->CreateLayer);
    LayerInfo hdiLayerInfo = {
        .width = layerInfo.width,
        .height = layerInfo.height,
        .type = static_cast<LayerType>(layerInfo.type),
        .pixFormat = static_cast<PixelFormat>(layerInfo.pixFormat),
    };
    return layerFuncs_->CreateLayer(screenId, &hdiLayerInfo, &layerId);
}

int32_t HdiDeviceImpl::CloseLayer(uint32_t screenId, uint32_t layerId)
{
    CHECK_FUNC(layerFuncs_, layerFuncs_->CloseLayer);
    return layerFuncs_->CloseLayer(screenId, layerId);
}

} // namespace Rosen
} // namespace OHOS