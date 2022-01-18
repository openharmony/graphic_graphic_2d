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

#ifndef GRAPHIC_STANDARD_COMPOSER_HDIDEVICE_MOCK_H
#define GRAPHIC_STANDARD_COMPOSER_HDIDEVICE_MOCK_H

#include "hdi_device.h"

#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
namespace MockSys {
class HdiDevice : public Base::HdiDevice {
public:
    HdiDevice();
    ~HdiDevice() override;

    static HdiDevice* GetInstance();

    /* set & get device screen info begin */
    MOCK_METHOD2(RegHotPlugCallback, int32_t(HotPlugCallback, void*));
    MOCK_METHOD3(RegScreenVBlankCallback, int32_t(uint32_t, VBlankCallback, void*));
    MOCK_METHOD2(GetScreenCapability, int32_t(uint32_t, DisplayCapability&));
    MOCK_METHOD2(GetScreenSuppportedModes, int32_t(uint32_t, std::vector<DisplayModeInfo>&));
    MOCK_METHOD2(GetScreenMode, int32_t(uint32_t, uint32_t&));
    MOCK_METHOD2(SetScreenMode, int32_t(uint32_t, uint32_t));
    MOCK_METHOD2(GetScreenPowerStatus, int32_t(uint32_t, DispPowerStatus&));
    MOCK_METHOD2(SetScreenPowerStatus, int32_t(uint32_t, DispPowerStatus));
    MOCK_METHOD2(GetScreenBacklight, int32_t(uint32_t, uint32_t&));
    MOCK_METHOD2(SetScreenBacklight, int32_t(uint32_t, uint32_t));
    MOCK_METHOD2(PrepareScreenLayers, int32_t(uint32_t, bool&));
    MOCK_METHOD3(GetScreenCompChange, int32_t(uint32_t, std::vector<uint32_t>&, std::vector<int32_t>&));
    MOCK_METHOD3(SetScreenClientBuffer, int32_t(uint32_t, const BufferHandle*, const sptr<SyncFence>&));
    MOCK_METHOD3(SetScreenClientDamage, int32_t(uint32_t, uint32_t, IRect&));
    MOCK_METHOD2(SetScreenVsyncEnabled, int32_t(uint32_t, bool));
    MOCK_METHOD3(GetScreenReleaseFence, int32_t(uint32_t, std::vector<uint32_t>&, std::vector<sptr<SyncFence>>&));
    MOCK_METHOD2(Commit, int32_t(uint32_t, sptr<SyncFence>&));
    /* set & get device screen info end */

    /* set & get device layer info begin */
    MOCK_METHOD3(SetLayerAlpha, int32_t(uint32_t, uint32_t, LayerAlpha&));
    MOCK_METHOD3(SetLayerSize, int32_t(uint32_t, uint32_t, IRect&));
    MOCK_METHOD3(SetTransformMode, int32_t(uint32_t, uint32_t, TransformType));
    MOCK_METHOD4(SetLayerVisibleRegion, int32_t(uint32_t, uint32_t, uint32_t, IRect&));
    MOCK_METHOD3(SetLayerDirtyRegion, int32_t(uint32_t, uint32_t, IRect&));
    MOCK_METHOD4(SetLayerBuffer, int32_t(uint32_t, uint32_t, const BufferHandle*, const sptr<SyncFence>&));
    MOCK_METHOD3(SetLayerCompositionType, int32_t(uint32_t, uint32_t, CompositionType));
    MOCK_METHOD3(SetLayerBlendType, int32_t(uint32_t, uint32_t, BlendType));
    MOCK_METHOD3(SetLayerCrop, int32_t(uint32_t, uint32_t, IRect&));
    MOCK_METHOD3(SetLayerZorder, int32_t(uint32_t, uint32_t, uint32_t));
    MOCK_METHOD3(SetLayerPreMulti, int32_t(uint32_t, uint32_t, bool));
    /* set & get device layer info end */

    MOCK_METHOD3(CreateLayer, int32_t(uint32_t, const LayerInfo&, uint32_t&));
    MOCK_METHOD2(CloseLayer, int32_t(uint32_t, uint32_t));
};
} // namespace MockSys
} // namespace Rosen
} // namespace OHOS
#endif // GRAPHIC_STANDARD_COMPOSER_HDIDEVICE_MOCK_H