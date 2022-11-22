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

#ifndef HDI_BACKEND_HDI_DEVICE_IMPL_H
#define HDI_BACKEND_HDI_DEVICE_IMPL_H

#include "hdi_device.h"
#include "hdi_display_type.h"
#include "display_device.h"
#include "display_layer.h"

namespace OHOS {
namespace Rosen {
class HdiDeviceImpl : public HdiDevice {
public:
    HdiDeviceImpl();
    virtual ~HdiDeviceImpl();
    RosenError Init();
    /* set & get device screen info begin */
    int32_t RegHotPlugCallback(HotPlugCallback callback, void *data) override;
    int32_t RegScreenVBlankCallback(uint32_t screenId, VBlankCallback callback, void *data) override;
    int32_t GetScreenCapability(uint32_t screenId, GraphicDisplayCapability &info) override;
    int32_t GetScreenSupportedModes(uint32_t screenId, std::vector<GraphicDisplayModeInfo> &modes) override;
    int32_t GetScreenMode(uint32_t screenId, uint32_t &modeId) override;
    int32_t SetScreenMode(uint32_t screenId, uint32_t modeId) override;
    int32_t GetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus &status) override;
    int32_t SetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus status) override;
    int32_t GetScreenBacklight(uint32_t screenId, uint32_t &level) override;
    int32_t SetScreenBacklight(uint32_t screenId, uint32_t level) override;
    int32_t PrepareScreenLayers(uint32_t screenId, bool &needFlushFb) override;
    int32_t GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                std::vector<int32_t> &types) override;
    int32_t SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer,
                                  const sptr<SyncFence> &fence) override;
    int32_t SetScreenClientDamage(uint32_t screenId, uint32_t num, GraphicIRect &damageRect) override;
    int32_t SetScreenVsyncEnabled(uint32_t screenId, bool enabled) override;
    int32_t GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layersId,
                                  std::vector<sptr<SyncFence>> &fences) override;
    int32_t GetScreenSupportedColorGamuts(uint32_t screenId, std::vector<GraphicColorGamut> &gamuts) override;
    int32_t SetScreenColorGamut(uint32_t screenId, GraphicColorGamut gamut) override;
    int32_t GetScreenColorGamut(uint32_t screenId, GraphicColorGamut &gamut) override;
    int32_t SetScreenGamutMap(uint32_t screenId, GraphicGamutMap gamutMap) override;
    int32_t GetScreenGamutMap(uint32_t screenId, GraphicGamutMap &gamutMap) override;
    int32_t SetScreenColorTransform(uint32_t screenId, const float *matrix) override;
    int32_t GetHDRCapabilityInfos(uint32_t screenId, GraphicHDRCapability &info) override;
    int32_t GetSupportedMetaDataKey(uint32_t screenId, std::vector<GraphicHDRMetadataKey> &keys) override;
    int32_t Commit(uint32_t screenId, sptr<SyncFence> &fence) override;
    /* set & get device screen info end */

    /* set & get device layer info begin */
    int32_t SetLayerAlpha(uint32_t screenId, uint32_t layerId, GraphicLayerAlpha &alpha) override;
    int32_t SetLayerSize(uint32_t screenId, uint32_t layerId, GraphicIRect &layerRect) override;
    int32_t SetTransformMode(uint32_t screenId, uint32_t layerId, GraphicTransformType type) override;
    int32_t SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId, uint32_t num, GraphicIRect &visible) override;
    int32_t SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId, GraphicIRect &dirty) override;
    int32_t SetLayerBuffer(uint32_t screenId, uint32_t layerId, const BufferHandle *handle,
                           const sptr<SyncFence> &acquireFence) override;
    int32_t SetLayerCompositionType(uint32_t screenId, uint32_t layerId, GraphicCompositionType type) override;
    int32_t SetLayerBlendType(uint32_t screenId, uint32_t layerId, GraphicBlendType type) override;
    int32_t SetLayerCrop(uint32_t screenId, uint32_t layerId, GraphicIRect &crop) override;
    int32_t SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder) override;
    int32_t SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti) override;
    int32_t SetLayerColorTransform(uint32_t screenId, uint32_t layerId, const float *matrix) override;
    int32_t SetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace colorSpace) override;
    int32_t GetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace &colorSpace) override;
    int32_t SetLayerMetaData(uint32_t screenId, uint32_t layerId,
                             const std::vector<GraphicHDRMetaData> &metaData) override;
    int32_t SetLayerMetaDataSet(uint32_t screenId, uint32_t layerId, GraphicHDRMetadataKey key,
                                const std::vector<uint8_t> &metaData) override;
    int32_t SetLayerTunnelHandle(uint32_t screenId, uint32_t layerId, OHExtDataHandle *handle) override;
    int32_t GetSupportedPresentTimestampType(uint32_t screenId, uint32_t layerId,
                                             GraphicPresentTimestampType &type) override;
    int32_t GetPresentTimestamp(uint32_t screenId, uint32_t layerId, GraphicPresentTimestamp &timestamp) override;
    /* set & get device layer info end */

    int32_t CreateLayer(uint32_t screenId, const GraphicLayerInfo &layerInfo, uint32_t &layerId) override;
    int32_t CloseLayer(uint32_t screenId, uint32_t layerId) override;

private:
    HdiDeviceImpl(const HdiDeviceImpl& rhs) = delete;
    HdiDeviceImpl& operator=(const HdiDeviceImpl& rhs) = delete;
    HdiDeviceImpl(HdiDeviceImpl&& rhs) = delete;
    HdiDeviceImpl& operator=(HdiDeviceImpl&& rhs) = delete;

    DeviceFuncs *deviceFuncs_ = nullptr;
    LayerFuncs *layerFuncs_ = nullptr;

    void Destroy();
};

template <typename DevicePtr, typename DeviceFuncPtr>
class CheckFunc {
public:
    CheckFunc(const DevicePtr device, const DeviceFuncPtr deviceFunc, const std::string& funcName)
    {
        if (device == nullptr || deviceFunc == nullptr) {
            HLOGD("can not find hdi func: %{public}s", funcName.c_str());
            hasFunc = false;
        }
    }
    ~CheckFunc() noexcept = default;
    bool operator()() const
    {
        return hasFunc;
    }
private:
    bool hasFunc = true;
};
} // namespace Rosen
} // namespace OHOS
#endif // HDI_BACKEND_HDI_DEVICE_IMPL_H