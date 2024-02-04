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

#ifndef HDI_BACKEND_HDI_SCREEN_H
#define HDI_BACKEND_HDI_SCREEN_H

#include <functional>
#include <vector>
#include <refbase.h>

#include "hdi_layer.h"
#include "hdi_device.h"

namespace OHOS {
namespace Rosen {

using OnVsyncFunc = std::function<void()>;

class HdiScreen {
public:
    HdiScreen(uint32_t screenId);
    virtual ~HdiScreen();

    static std::unique_ptr<HdiScreen> CreateHdiScreen(uint32_t screenId);
    bool Init();

    int32_t GetScreenCapability(GraphicDisplayCapability &info) const;
    int32_t GetScreenSupportedModes(std::vector<GraphicDisplayModeInfo> &modes) const;
    int32_t GetScreenMode(uint32_t &modeId) const;
    int32_t SetScreenMode(uint32_t modeId) const;
    int32_t SetScreenOverlayResolution(uint32_t width, uint32_t height) const;
    int32_t GetScreenPowerStatus(GraphicDispPowerStatus &status) const;
    int32_t SetScreenPowerStatus(GraphicDispPowerStatus status) const;
    int32_t GetScreenBacklight(uint32_t &level) const;
    int32_t SetScreenBacklight(uint32_t level) const;
    int32_t SetScreenVsyncEnabled(bool enabled) const;

    int32_t GetScreenSupportedColorGamuts(std::vector<GraphicColorGamut> &gamuts) const;
    int32_t SetScreenColorGamut(GraphicColorGamut gamut) const;
    int32_t GetScreenColorGamut(GraphicColorGamut &gamut) const;
    int32_t SetScreenGamutMap(GraphicGamutMap gamutMap) const;
    int32_t GetScreenGamutMap(GraphicGamutMap &gamutMap) const;
    int32_t SetScreenColorTransform(const std::vector<float>& matrix) const;
    int32_t GetHDRCapabilityInfos(GraphicHDRCapability &info) const;
    int32_t GetSupportedMetaDataKey(std::vector<GraphicHDRMetadataKey> &keys) const;

    static void OnVsync(uint32_t sequence, uint64_t ns, void *data);

    /* only used for mock and fuzz tests */
    bool SetHdiDevice(HdiDevice* device);

private:
    uint32_t screenId_;
    HdiDevice *device_ = nullptr;

    void Destroy();
};

} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_SCREEN_H