/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <unordered_map>
#include <sync_fence.h>
#include "hdi_display_type.h"

namespace OHOS {
namespace Rosen {
class HdiDevice {
public:
    static HdiDevice* GetInstance();
    HdiDevice() = default;
    virtual ~HdiDevice() = default;
    /* set & get device screen info begin */
    virtual int32_t RegHotPlugCallback(HotPlugCallback callback, void *data) = 0;
    virtual int32_t RegScreenVBlankCallback(uint32_t screenId, VBlankCallback callback, void *data) = 0;
    virtual bool RegHwcDeadCallback(OnHwcDeadCallback callback, void *data) = 0;
    virtual int32_t GetScreenCapability(uint32_t screenId, GraphicDisplayCapability &info) = 0;
    virtual int32_t GetScreenSupportedModes(uint32_t screenId, std::vector<GraphicDisplayModeInfo> &modes) = 0;
    virtual int32_t GetScreenMode(uint32_t screenId, uint32_t &modeId) = 0;
    virtual int32_t SetScreenMode(uint32_t screenId, uint32_t modeId) = 0;
    virtual int32_t SetScreenOverlayResolution(uint32_t screenId, uint32_t width, uint32_t height) = 0;
    virtual int32_t GetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus &status) = 0;
    virtual int32_t SetScreenPowerStatus(uint32_t screenId, GraphicDispPowerStatus status) = 0;
    virtual int32_t GetScreenBacklight(uint32_t screenId, uint32_t &level) = 0;
    virtual int32_t SetScreenBacklight(uint32_t screenId, uint32_t level) = 0;
    virtual int32_t PrepareScreenLayers(uint32_t screenId, bool &needFlushFb) = 0;
    virtual int32_t GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                        std::vector<int32_t> &types) = 0;
    virtual int32_t SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer, uint32_t cacheIndex,
                                          const sptr<SyncFence> &fence) = 0;
    virtual int32_t SetScreenClientBufferCacheCount(uint32_t screen, uint32_t count) = 0;
    virtual int32_t SetScreenClientDamage(uint32_t screenId, const std::vector<GraphicIRect> &damageRect) = 0;
    virtual int32_t SetScreenVsyncEnabled(uint32_t screenId, bool enabled) = 0;
    virtual int32_t GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layersId,
                                          std::vector<sptr<SyncFence>> &fences) = 0;
    virtual int32_t GetScreenSupportedColorGamuts(uint32_t screenId, std::vector<GraphicColorGamut> &gamuts) = 0;
    virtual int32_t SetScreenColorGamut(uint32_t screenId, GraphicColorGamut gamut) = 0;
    virtual int32_t GetScreenColorGamut(uint32_t screenId, GraphicColorGamut &gamut) = 0;
    virtual int32_t SetScreenGamutMap(uint32_t screenId, GraphicGamutMap gamutMap) = 0;
    virtual int32_t GetScreenGamutMap(uint32_t screenId, GraphicGamutMap &gamutMap) = 0;
    virtual int32_t SetScreenColorTransform(uint32_t screenId, const std::vector<float> &matrix) = 0;
    virtual int32_t GetHDRCapabilityInfos(uint32_t screenId, GraphicHDRCapability &info) = 0;
    virtual int32_t GetSupportedMetaDataKey(uint32_t screenId, std::vector<GraphicHDRMetadataKey> &keys) = 0;
    virtual int32_t Commit(uint32_t screenId, sptr<SyncFence> &fence) = 0;
    /* set & get device screen info end */

    /* set & get device layer info begin */
    virtual int32_t SetLayerAlpha(uint32_t screenId, uint32_t layerId, const GraphicLayerAlpha &alpha) = 0;
    virtual int32_t SetLayerSize(uint32_t screenId, uint32_t layerId, const GraphicIRect &layerRect) = 0;
    virtual int32_t SetTransformMode(uint32_t screenId, uint32_t layerId, GraphicTransformType type) = 0;
    virtual int32_t SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId,
                                          const std::vector<GraphicIRect> &visibles) = 0;
    virtual int32_t SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId,
                                        const std::vector<GraphicIRect> &dirtyRegions) = 0;
    virtual int32_t SetLayerBuffer(uint32_t screenId, uint32_t layerId, const GraphicLayerBuffer &layerBuffer) = 0;
    virtual int32_t SetLayerCompositionType(uint32_t screenId, uint32_t layerId, GraphicCompositionType type) = 0;
    virtual int32_t SetLayerBlendType(uint32_t screenId, uint32_t layerId, GraphicBlendType type) = 0;
    virtual int32_t SetLayerCrop(uint32_t screenId, uint32_t layerId, const GraphicIRect &crop) = 0;
    virtual int32_t SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder) = 0;
    virtual int32_t SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti) = 0;
    virtual int32_t SetLayerColor(uint32_t screenId, uint32_t layerId, GraphicLayerColor layerColor) = 0;
    virtual int32_t SetLayerColorTransform(uint32_t screenId, uint32_t layerId, const std::vector<float> &matrix) = 0;
    virtual int32_t SetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace colorSpace) = 0;
    virtual int32_t GetLayerColorDataSpace(uint32_t screenId, uint32_t layerId, GraphicColorDataSpace &colorSpace) = 0;
    virtual int32_t SetLayerMetaData(uint32_t screenId, uint32_t layerId,
                                     const std::vector<GraphicHDRMetaData> &graphicMetaData) = 0;
    virtual int32_t SetLayerMetaDataSet(uint32_t screenId, uint32_t layerId, GraphicHDRMetadataKey gkey,
                                        const std::vector<uint8_t> &metaData) = 0;
    virtual int32_t SetLayerTunnelHandle(uint32_t screenId, uint32_t layerId, GraphicExtDataHandle *handle) = 0;
    virtual int32_t GetSupportedPresentTimestampType(uint32_t screenId, uint32_t layerId,
                                                     GraphicPresentTimestampType &type) = 0;
    virtual int32_t GetPresentTimestamp(uint32_t screenId, uint32_t layerId, GraphicPresentTimestamp &timestamp) = 0;
    virtual int32_t SetLayerMaskInfo(uint32_t screenId, uint32_t layerId, uint32_t maskInfo) = 0;
    /* set & get device layer info end */

    virtual int32_t CreateLayer(uint32_t screenId, const GraphicLayerInfo &layerInfo, uint32_t cacheCount,
                                uint32_t &layerId) = 0;
    virtual int32_t CloseLayer(uint32_t screenId, uint32_t layerId) = 0;
    virtual void Destroy() = 0;

private:
    HdiDevice(const HdiDevice& rhs) = delete;
    HdiDevice& operator=(const HdiDevice& rhs) = delete;
    HdiDevice(HdiDevice&& rhs) = delete;
    HdiDevice& operator=(HdiDevice&& rhs) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // HDI_BACKEND_HDI_DEVICE_H