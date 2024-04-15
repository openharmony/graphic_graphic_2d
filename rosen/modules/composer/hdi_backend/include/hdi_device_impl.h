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
#include "graphic_error.h"
#include "v1_1/include/idisplay_composer_interface.h"
#include "v1_2/include/idisplay_composer_interface.h"

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
    bool RegHwcDeadCallback(OnHwcDeadCallback callback, void *data) override;
    int32_t GetScreenCapability(uint32_t screenId, GraphicDisplayCapability &info) override;
    int32_t GetScreenSupportedModes(uint32_t screenId, std::vector<GraphicDisplayModeInfo> &modes) override;
    int32_t GetScreenMode(uint32_t screenId, uint32_t &modeId) override;
    int32_t SetScreenMode(uint32_t screenId, uint32_t modeId) override;
    int32_t SetScreenOverlayResolution(uint32_t screenId, uint32_t width, uint32_t height) override;
    int32_t GetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus &status) override;
    int32_t SetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus status) override;
    int32_t GetScreenBacklight(uint32_t screenId, uint32_t &level) override;
    int32_t SetScreenBacklight(uint32_t screenId, uint32_t level) override;
    int32_t PrepareScreenLayers(uint32_t screenId, bool &needFlushFb) override;
    int32_t GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                std::vector<int32_t> &types) override;
    int32_t SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer, uint32_t cacheIndex,
                                  const sptr<SyncFence> &fence) override;
    int32_t SetScreenClientBufferCacheCount(uint32_t screen, uint32_t count) override;
    int32_t SetScreenClientDamage(uint32_t screenId, const std::vector<GraphicIRect> &damageRect) override;
    int32_t SetScreenVsyncEnabled(uint32_t screenId, bool enabled) override;
    int32_t GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layersId,
                                  std::vector<sptr<SyncFence>> &fences) override;
    int32_t GetScreenSupportedColorGamuts(uint32_t screenId, std::vector<GraphicColorGamut> &gamuts) override;
    int32_t SetScreenColorGamut(uint32_t screenId, GraphicColorGamut gamut) override;
    int32_t GetScreenColorGamut(uint32_t screenId, GraphicColorGamut &gamut) override;
    int32_t SetScreenGamutMap(uint32_t screenId, GraphicGamutMap gamutMap) override;
    int32_t GetScreenGamutMap(uint32_t screenId, GraphicGamutMap &gamutMap) override;
    int32_t SetScreenColorTransform(uint32_t screenId, const std::vector<float>& matrix) override;
    int32_t GetHDRCapabilityInfos(uint32_t screenId, GraphicHDRCapability &info) override;
    int32_t GetSupportedMetaDataKey(uint32_t screenId, std::vector<GraphicHDRMetadataKey> &keys) override;
    int32_t Commit(uint32_t screenId, sptr<SyncFence> &fence) override;
    int32_t CommitAndGetReleaseFence(uint32_t screenId, sptr<SyncFence> &fence, int32_t &skipState, bool &needFlush,
        std::vector<uint32_t>& layers, std::vector<sptr<SyncFence>>& fences) override;
    /* set & get device screen info end */

    /* set & get device layer info begin */
    int32_t SetLayerAlpha(uint32_t screenId, uint32_t layerId, const GraphicLayerAlpha &alpha) override;
    int32_t SetLayerSize(uint32_t screenId, uint32_t layerId, const GraphicIRect &layerRect) override;
    int32_t SetTransformMode(uint32_t screenId, uint32_t layerId, GraphicTransformType type) override;
    int32_t SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId,
                                  const std::vector<GraphicIRect> &visibles) override;
    int32_t SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId,
                                const std::vector<GraphicIRect> &dirtyRegions) override;
    int32_t SetLayerBuffer(uint32_t screenId, uint32_t layerId, const GraphicLayerBuffer &layerBuffer) override;
    int32_t SetLayerCompositionType(uint32_t screenId, uint32_t layerId, GraphicCompositionType type) override;
    int32_t SetLayerBlendType(uint32_t screenId, uint32_t layerId, GraphicBlendType type) override;
    int32_t SetLayerCrop(uint32_t screenId, uint32_t layerId, const GraphicIRect &crop) override;
    int32_t SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder) override;
    int32_t SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti) override;
    int32_t SetLayerColor(uint32_t screenId, uint32_t layerId, GraphicLayerColor layerColor) override;
    int32_t SetLayerColorTransform(uint32_t screenId, uint32_t layerId, const std::vector<float> &matrix) override;
    int32_t SetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace colorSpace) override;
    int32_t GetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace &colorSpace) override;
    int32_t SetLayerMetaData(uint32_t screenId, uint32_t layerId,
                             const std::vector<GraphicHDRMetaData> &metaData) override;
    int32_t SetLayerMetaDataSet(uint32_t screenId, uint32_t layerId, GraphicHDRMetadataKey key,
                                const std::vector<uint8_t> &metaData) override;
    int32_t GetSupportedLayerPerFrameParameterKey(std::vector<std::string>& keys) override;
    int32_t SetLayerPerFrameParameter(uint32_t devId, uint32_t layerId, const std::string& key,
                                      const std::vector<int8_t>& value) override;
    int32_t SetLayerTunnelHandle(uint32_t screenId, uint32_t layerId, GraphicExtDataHandle *handle) override;
    int32_t GetSupportedPresentTimestampType(uint32_t screenId, uint32_t layerId,
                                             GraphicPresentTimestampType &type) override;
    int32_t GetPresentTimestamp(uint32_t screenId, uint32_t layerId, GraphicPresentTimestamp &timestamp) override;
    int32_t SetLayerMaskInfo(uint32_t screenId, uint32_t layerId, uint32_t maskInfo) override;
    /* set & get device layer info end */

    int32_t CreateLayer(uint32_t screenId, const GraphicLayerInfo &layerInfo, uint32_t cacheCount,
                        uint32_t &layerId) override;
    int32_t CloseLayer(uint32_t screenId, uint32_t layerId) override;
    void Destroy() override;

private:
    HdiDeviceImpl(const HdiDeviceImpl& rhs) = delete;
    HdiDeviceImpl& operator=(const HdiDeviceImpl& rhs) = delete;
    HdiDeviceImpl(HdiDeviceImpl&& rhs) = delete;
    HdiDeviceImpl& operator=(HdiDeviceImpl&& rhs) = delete;
};

} // namespace Rosen
} // namespace OHOS
#endif // HDI_BACKEND_HDI_DEVICE_IMPL_H