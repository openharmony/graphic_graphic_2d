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

#ifndef HDI_BACKEND_HDI_DEVICE_H
#define HDI_BACKEND_HDI_DEVICE_H

#include <vector>
#include <refbase.h>

#include <sync_fence.h>

#include "hdi_log.h"
#include "display_device.h"
#include "display_layer.h"

namespace OHOS {
namespace Rosen {
namespace Base {
class HdiDevice {
public:
    HdiDevice() = default;
    virtual ~HdiDevice() = default;

    /* set & get device screen info begin */
    virtual int32_t RegHotPlugCallback(HotPlugCallback callback, void *data) = 0;
    virtual int32_t RegScreenVBlankCallback(uint32_t screenId, VBlankCallback callback, void *data) = 0;
    virtual int32_t GetScreenCapability(uint32_t screenId, DisplayCapability &info) = 0;
    virtual int32_t GetScreenSupportedModes(uint32_t screenId, std::vector<DisplayModeInfo> &modes) = 0;
    virtual int32_t GetScreenMode(uint32_t screenId, uint32_t &modeId) = 0;
    virtual int32_t SetScreenMode(uint32_t screenId, uint32_t modeId) = 0;
    virtual int32_t GetScreenPowerStatus(uint32_t screenId, DispPowerStatus &status) = 0;
    virtual int32_t SetScreenPowerStatus(uint32_t screenId, DispPowerStatus status) = 0;
    virtual int32_t GetScreenBacklight(uint32_t screenId, uint32_t &level) = 0;
    virtual int32_t SetScreenBacklight(uint32_t screenId, uint32_t level) = 0;
    virtual int32_t PrepareScreenLayers(uint32_t screenId, bool &needFlushFb) = 0;
    virtual int32_t GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                std::vector<int32_t> &types) = 0;
    virtual int32_t SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer,
                                  const sptr<SyncFence> &fence) = 0;
    virtual int32_t SetScreenClientDamage(uint32_t screenId, uint32_t num, IRect &damageRect) = 0;
    virtual int32_t SetScreenVsyncEnabled(uint32_t screenId, bool enabled) = 0;
    virtual int32_t GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layersId,
                                  std::vector<sptr<SyncFence>> &fences) = 0;
    virtual int32_t GetScreenSupportedColorGamuts(uint32_t screenId, std::vector<ColorGamut> &gamuts) = 0;
    virtual int32_t SetScreenColorGamut(uint32_t screenId, ColorGamut gamut) = 0;
    virtual int32_t GetScreenColorGamut(uint32_t screenId, ColorGamut &gamut) = 0;
    virtual int32_t SetScreenGamutMap(uint32_t screenId, GamutMap gamutMap) = 0;
    virtual int32_t GetScreenGamutMap(uint32_t screenId, GamutMap &gamutMap) = 0;
    virtual int32_t SetScreenColorTransform(uint32_t screenId, const float *matrix) = 0;
    virtual int32_t GetHDRCapabilityInfos(uint32_t screenId, HDRCapability &info) = 0;
    virtual int32_t GetSupportedMetaDataKey(uint32_t screenId, std::vector<HDRMetadataKey> &keys) = 0;
    virtual int32_t Commit(uint32_t screenId, sptr<SyncFence> &fence) = 0;
    /* set & get device screen info end */

    /* set & get device layer info begin */
    virtual int32_t SetLayerAlpha(uint32_t screenId, uint32_t layerId, LayerAlpha &alpha) = 0;
    virtual int32_t SetLayerSize(uint32_t screenId, uint32_t layerId, IRect &layerRect) = 0;
    virtual int32_t SetTransformMode(uint32_t screenId, uint32_t layerId, TransformType type) = 0;
    virtual int32_t SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId, uint32_t num, IRect &visable) = 0;
    virtual int32_t SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId, IRect &dirty) = 0;
    virtual int32_t SetLayerBuffer(uint32_t screenId, uint32_t layerId, const BufferHandle *handle,
                           const sptr<SyncFence> &acquireFence) = 0;
    virtual int32_t SetLayerCompositionType(uint32_t screenId, uint32_t layerId, CompositionType type) = 0;
    virtual int32_t SetLayerBlendType(uint32_t screenId, uint32_t layerId, BlendType type) = 0;
    virtual int32_t SetLayerCrop(uint32_t screenId, uint32_t layerId, IRect &crop) = 0;
    virtual int32_t SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder) = 0;
    virtual int32_t SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti) = 0;
    virtual int32_t SetLayerColorTransform(uint32_t screenId, uint32_t layerId, const float *matrix) = 0;
    virtual int32_t SetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, ColorDataSpace colorSpace) = 0;
    virtual int32_t GetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, ColorDataSpace &colorSpace) = 0;
    virtual int32_t SetLayerMetaData(uint32_t screenId, uint32_t layerId, const std::vector<HDRMetaData> &metaData) = 0;
    virtual int32_t SetLayerMetaDataSet(uint32_t screenId, uint32_t layerId, HDRMetadataKey key,
                                        const std::vector<uint8_t> &metaData) = 0;
    /* set & get device layer info end */

    virtual int32_t CreateLayer(uint32_t screenId, const LayerInfo &layerInfo, uint32_t &layerId) = 0;
    virtual int32_t CloseLayer(uint32_t screenId, uint32_t layerId) = 0;

private:
    HdiDevice(const HdiDevice& rhs) = delete;
    HdiDevice& operator=(const HdiDevice& rhs) = delete;
    HdiDevice(HdiDevice&& rhs) = delete;
    HdiDevice& operator=(HdiDevice&& rhs) = delete;
};
} // namespace Base

class HdiDevice : public Base::HdiDevice {
public:
    HdiDevice();
    virtual ~HdiDevice();

    static HdiDevice* GetInstance();

    /* set & get device screen info begin */
    int32_t RegHotPlugCallback(HotPlugCallback callback, void *data) override;
    int32_t RegScreenVBlankCallback(uint32_t screenId, VBlankCallback callback, void *data) override;
    int32_t GetScreenCapability(uint32_t screenId, DisplayCapability &info) override;
    int32_t GetScreenSupportedModes(uint32_t screenId, std::vector<DisplayModeInfo> &modes) override;
    int32_t GetScreenMode(uint32_t screenId, uint32_t &modeId) override;
    int32_t SetScreenMode(uint32_t screenId, uint32_t modeId) override;
    int32_t GetScreenPowerStatus(uint32_t screenId, DispPowerStatus &status) override;
    int32_t SetScreenPowerStatus(uint32_t screenId, DispPowerStatus status) override;
    int32_t GetScreenBacklight(uint32_t screenId, uint32_t &level) override;
    int32_t SetScreenBacklight(uint32_t screenId, uint32_t level) override;
    int32_t PrepareScreenLayers(uint32_t screenId, bool &needFlushFb) override;
    int32_t GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                std::vector<int32_t> &types) override;
    int32_t SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer,
                                  const sptr<SyncFence> &fence) override;
    int32_t SetScreenClientDamage(uint32_t screenId, uint32_t num, IRect &damageRect) override;
    int32_t SetScreenVsyncEnabled(uint32_t screenId, bool enabled) override;
    int32_t GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layersId,
                                  std::vector<sptr<SyncFence>> &fences) override;
    int32_t GetScreenSupportedColorGamuts(uint32_t screenId, std::vector<ColorGamut> &gamuts) override;
    int32_t SetScreenColorGamut(uint32_t screenId, ColorGamut gamut) override;
    int32_t GetScreenColorGamut(uint32_t screenId, ColorGamut &gamut) override;
    int32_t SetScreenGamutMap(uint32_t screenId, GamutMap gamutMap) override;
    int32_t GetScreenGamutMap(uint32_t screenId, GamutMap &gamutMap) override;
    int32_t SetScreenColorTransform(uint32_t screenId, const float *matrix) override;
    int32_t GetHDRCapabilityInfos(uint32_t screenId, HDRCapability &info) override;
    int32_t GetSupportedMetaDataKey(uint32_t screenId, std::vector<HDRMetadataKey> &keys) override;
    int32_t Commit(uint32_t screenId, sptr<SyncFence> &fence) override;
    /* set & get device screen info end */

    /* set & get device layer info begin */
    int32_t SetLayerAlpha(uint32_t screenId, uint32_t layerId, LayerAlpha &alpha) override;
    int32_t SetLayerSize(uint32_t screenId, uint32_t layerId, IRect &layerRect) override;
    int32_t SetTransformMode(uint32_t screenId, uint32_t layerId, TransformType type) override;
    int32_t SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId, uint32_t num, IRect &visable) override;
    int32_t SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId, IRect &dirty) override;
    int32_t SetLayerBuffer(uint32_t screenId, uint32_t layerId, const BufferHandle *handle,
                           const sptr<SyncFence> &acquireFence) override;
    int32_t SetLayerCompositionType(uint32_t screenId, uint32_t layerId, CompositionType type) override;
    int32_t SetLayerBlendType(uint32_t screenId, uint32_t layerId, BlendType type) override;
    int32_t SetLayerCrop(uint32_t screenId, uint32_t layerId, IRect &crop) override;
    int32_t SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder) override;
    int32_t SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti) override;
    int32_t SetLayerColorTransform(uint32_t screenId, uint32_t layerId, const float *matrix) override;
    int32_t SetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, ColorDataSpace colorSpace) override;
    int32_t GetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, ColorDataSpace &colorSpace) override;
    int32_t SetLayerMetaData(uint32_t screenId, uint32_t layerId, const std::vector<HDRMetaData> &metaData) override;
    int32_t SetLayerMetaDataSet(uint32_t screenId, uint32_t layerId, HDRMetadataKey key,
                                const std::vector<uint8_t> &metaData) override;
    /* set & get device layer info end */

    int32_t CreateLayer(uint32_t screenId, const LayerInfo &layerInfo, uint32_t &layerId) override;
    int32_t CloseLayer(uint32_t screenId, uint32_t layerId) override;

private:
    HdiDevice(const HdiDevice& rhs) = delete;
    HdiDevice& operator=(const HdiDevice& rhs) = delete;
    HdiDevice(HdiDevice&& rhs) = delete;
    HdiDevice& operator=(HdiDevice&& rhs) = delete;

    DeviceFuncs *deviceFuncs_ = nullptr;
    LayerFuncs *layerFuncs_ = nullptr;

    RosenError Init();
    void Destory();
};

} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_DEVICE_H