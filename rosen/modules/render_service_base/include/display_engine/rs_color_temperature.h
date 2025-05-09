/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_COLOR_TEMPERATURE_H
#define RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_COLOR_TEMPERATURE_H

#include <cinttypes>
#include <functional>
#include <vector>

#include "common/rs_macros.h"
#include "screen_manager/screen_types.h"
#ifndef ROSEN_CROSS_PLATFORM
#include <v1_0/cm_color_space.h>
#endif


namespace OHOS {
namespace Rosen {
#ifndef ROSEN_CROSS_PLATFORM
using namespace OHOS::HDI::Display::Graphic::Common::V1_0;
#endif

class RSColorTemperatureInterface {
public:
    virtual ~RSColorTemperatureInterface() = default;
    virtual bool Init() = 0;
    virtual void RegisterRefresh(std::function<void()>&& refreshFunc) = 0;
    virtual void UpdateScreenStatus(ScreenId screenId, ScreenPowerStatus powerStatus) = 0;
    virtual bool IsDimmingOn(ScreenId screenId) = 0;
    virtual void DimmingIncrease(ScreenId screenId) = 0;
    virtual bool IsColorTemperatureOn() const = 0;
    virtual std::vector<float> GetNewLinearCct(ScreenId screenId) = 0;
#ifndef ROSEN_CROSS_PLATFORM
    virtual std::vector<float> GetLayerLinearCct(ScreenId screenId,
        const std::vector<uint8_t>& dynamicMetadata, const CM_Matrix srcColorMatrix) = 0;
#endif
};

class RSB_EXPORT RSColorTemperature {
public:
    RSColorTemperature(const RSColorTemperature&) = delete;
    RSColorTemperature& operator=(const RSColorTemperature&) = delete;
    RSColorTemperature(RSColorTemperature&) = delete;
    RSColorTemperature& operator=(RSColorTemperature&&) = delete;

    RSB_EXPORT static RSColorTemperature& Get();
    RSB_EXPORT void Init();

    RSB_EXPORT void RegisterRefresh(std::function<void()>&& refreshFunc);
    RSB_EXPORT void UpdateScreenStatus(ScreenId screenId, ScreenPowerStatus powerStatus);
    RSB_EXPORT bool IsDimmingOn(ScreenId screenId);
    RSB_EXPORT void DimmingIncrease(ScreenId screenId);
    RSB_EXPORT bool IsColorTemperatureOn() const;
    RSB_EXPORT std::vector<float> GetNewLinearCct(ScreenId screenId);
#ifndef ROSEN_CROSS_PLATFORM
    RSB_EXPORT std::vector<float> GetLayerLinearCct(ScreenId screenId,
        const std::vector<uint8_t>& dynamicMetadata, const CM_Matrix srcColorMatrix = CM_Matrix::MATRIX_P3);
#endif

private:
    RSColorTemperature() = default;
    ~RSColorTemperature();

#ifdef ROSEN_OHOS
    bool LoadLibrary();
    void CloseLibrary();

    bool initStatus_{false};
    void *extLibHandle_{nullptr};
    RSColorTemperatureInterface* rSColorTemperatureInterface_{nullptr};

    using CreateFunc = RSColorTemperatureInterface*(*)();
    using DestroyFunc = void(*)();

    CreateFunc create_{nullptr};
    DestroyFunc destroy_{nullptr};
#endif
};

} // Rosen
} // OHOS

#endif // RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_COLOR_TEMPERATURE_H