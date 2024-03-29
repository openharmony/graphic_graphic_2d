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

#ifndef GRAPHIC_2D_COMPOSER_HDIDEVICE_MOCK_H
#define GRAPHIC_2D_COMPOSER_HDIDEVICE_MOCK_H

#include "hdi_device.h"

#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
namespace Mock {
class HdiDeviceMock : public HdiDevice {
public:
    HdiDeviceMock();
    ~HdiDeviceMock() override;

    static HdiDeviceMock* GetInstance();

    /* set & get device screen info begin */
    MOCK_METHOD2(RegHotPlugCallback, int32_t(HotPlugCallback, void*));
    MOCK_METHOD3(RegScreenVBlankCallback, int32_t(uint32_t, VBlankCallback, void*));
    MOCK_METHOD2(RegHwcDeadCallback, bool(OnHwcDeadCallback, void*));
    MOCK_METHOD2(GetScreenCapability, int32_t(uint32_t, GraphicDisplayCapability&));
    MOCK_METHOD2(GetScreenSupportedModes, int32_t(uint32_t, std::vector<GraphicDisplayModeInfo>&));
    MOCK_METHOD2(GetScreenMode, int32_t(uint32_t, uint32_t&));
    MOCK_METHOD2(SetScreenMode, int32_t(uint32_t, uint32_t));
    MOCK_METHOD3(SetScreenOverlayResolution, int32_t(uint32_t, uint32_t, uint32_t));
    MOCK_METHOD2(GetScreenPowerStatus, int32_t(uint32_t, GraphicDispPowerStatus&));
    MOCK_METHOD2(SetScreenPowerStatus, int32_t(uint32_t, GraphicDispPowerStatus));
    MOCK_METHOD2(GetScreenBacklight, int32_t(uint32_t, uint32_t&));
    MOCK_METHOD2(SetScreenBacklight, int32_t(uint32_t, uint32_t));
    MOCK_METHOD2(PrepareScreenLayers, int32_t(uint32_t, bool&));
    MOCK_METHOD3(GetScreenCompChange, int32_t(uint32_t, std::vector<uint32_t>&, std::vector<int32_t>&));
    MOCK_METHOD4(SetScreenClientBuffer, int32_t(uint32_t, const BufferHandle*, uint32_t, const sptr<SyncFence>&));
    MOCK_METHOD2(SetScreenClientBufferCacheCount, int32_t(uint32_t, uint32_t));
    MOCK_METHOD2(SetScreenClientDamage, int32_t(uint32_t, const std::vector<GraphicIRect> &damageRect));
    MOCK_METHOD2(SetScreenVsyncEnabled, int32_t(uint32_t, bool));
    MOCK_METHOD3(GetScreenReleaseFence, int32_t(uint32_t, std::vector<uint32_t>&, std::vector<sptr<SyncFence>>&));
    MOCK_METHOD2(GetScreenSupportedColorGamuts, int32_t(uint32_t, std::vector<GraphicColorGamut>&));
    MOCK_METHOD2(SetScreenColorGamut, int32_t(uint32_t, GraphicColorGamut));
    MOCK_METHOD2(GetScreenColorGamut, int32_t(uint32_t, GraphicColorGamut&));
    MOCK_METHOD2(SetScreenGamutMap, int32_t(uint32_t, GraphicGamutMap));
    MOCK_METHOD2(GetScreenGamutMap, int32_t(uint32_t, GraphicGamutMap&));
    MOCK_METHOD2(SetScreenColorTransform, int32_t(uint32_t, const std::vector<float> &matrix));
    MOCK_METHOD2(GetHDRCapabilityInfos, int32_t(uint32_t, GraphicHDRCapability&));
    MOCK_METHOD2(GetSupportedMetaDataKey, int32_t(uint32_t, std::vector<GraphicHDRMetadataKey>&));
    MOCK_METHOD2(Commit, int32_t(uint32_t, sptr<SyncFence>&));
    /* set & get device screen info end */

    /* set & get device layer info begin */
    MOCK_METHOD3(SetLayerAlpha, int32_t(uint32_t, uint32_t, const GraphicLayerAlpha&));
    MOCK_METHOD3(SetLayerSize, int32_t(uint32_t, uint32_t, const GraphicIRect&));
    MOCK_METHOD3(SetTransformMode, int32_t(uint32_t, uint32_t, GraphicTransformType));
    MOCK_METHOD3(SetLayerVisibleRegion, int32_t(uint32_t, uint32_t, const std::vector<GraphicIRect> &));
    MOCK_METHOD3(SetLayerDirtyRegion, int32_t(uint32_t, uint32_t, const std::vector<GraphicIRect> &));
    MOCK_METHOD3(SetLayerBuffer, int32_t(uint32_t, uint32_t, const GraphicLayerBuffer &));
    MOCK_METHOD3(SetLayerCompositionType, int32_t(uint32_t, uint32_t, GraphicCompositionType));
    MOCK_METHOD3(SetLayerBlendType, int32_t(uint32_t, uint32_t, GraphicBlendType));
    MOCK_METHOD3(SetLayerCrop, int32_t(uint32_t, uint32_t, const GraphicIRect&));
    MOCK_METHOD3(SetLayerZorder, int32_t(uint32_t, uint32_t, uint32_t));
    MOCK_METHOD3(SetLayerPreMulti, int32_t(uint32_t, uint32_t, bool));
    MOCK_METHOD3(SetLayerColor, int32_t(uint32_t, uint32_t, GraphicLayerColor));
    MOCK_METHOD3(SetLayerColorTransform, int32_t(uint32_t, uint32_t, const std::vector<float> &matrix));
    MOCK_METHOD3(SetLayerColorDataSpace, int32_t(uint32_t, uint32_t, GraphicColorDataSpace));
    MOCK_METHOD3(GetLayerColorDataSpace, int32_t(uint32_t, uint32_t, GraphicColorDataSpace&));
    MOCK_METHOD3(SetLayerMetaData, int32_t(uint32_t, uint32_t, const std::vector<GraphicHDRMetaData>&));
    MOCK_METHOD4(SetLayerMetaDataSet, int32_t(uint32_t, uint32_t, GraphicHDRMetadataKey, const std::vector<uint8_t>&));
    MOCK_METHOD3(SetLayerTunnelHandle, int32_t(uint32_t, uint32_t, GraphicExtDataHandle *));
    MOCK_METHOD3(GetSupportedPresentTimestampType, int32_t(uint32_t, uint32_t, GraphicPresentTimestampType&));
    MOCK_METHOD3(GetPresentTimestamp, int32_t(uint32_t, uint32_t, GraphicPresentTimestamp&));
    MOCK_METHOD3(SetLayerMaskInfo, int32_t(uint32_t, uint32_t, uint32_t));
    /* set & get device layer info end */

    MOCK_METHOD4(CreateLayer, int32_t(uint32_t, const GraphicLayerInfo&, uint32_t, uint32_t&));
    MOCK_METHOD2(CloseLayer, int32_t(uint32_t, uint32_t));

    MOCK_METHOD4(CommitAndGetReleaseFence, int32_t(uint32_t, sptr<SyncFence>&, int32_t&, bool&));
    MOCK_METHOD0(Destroy, void());
};
} // namespace Mock
} // namespace Rosen
} // namespace OHOS
#endif // GRAPHIC_2D_COMPOSER_HDIDEVICE_MOCK_H