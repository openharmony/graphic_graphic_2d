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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_COLOR_TEMP_H
#define ROSEN_RENDER_SERVICE_BASE_RS_COLOR_TEMP_H

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

class RSCctInterface {
public:
    virtual ~RSCctInterface() = default;
    virtual void RegisterRefresh(std::function<void()>) = 0;
    virtual void UpdateScreenStatus(ScreenId, ScreenPowerStatus) = 0;
    virtual bool IsDimmingOn(ScreenId) = 0;
    virtual void DimmingIncrease(ScreenId) = 0;
    virtual std::vector<float> GetNewLinearCct(ScreenId) = 0;
#ifndef ROSEN_CROSS_PLATFORM
    virtual std::vector<float> GetLayerLinearCct(ScreenId, const std::vector<uint8_t>&,
        const CM_Matrix targetMatrix) = 0;
#endif
};

class RSB_EXPORT RSColorTemp {
public:
    RSColorTemp(const RSColorTemp&) = delete;
    RSColorTemp& operator=(const RSColorTemp&) = delete;
    RSColorTemp(RSColorTemp&) = delete;
    RSColorTemp& operator=(RSColorTemp&&) = delete;

    RSB_EXPORT static RSColorTemp& Get();
    RSB_EXPORT void Init();

    RSB_EXPORT void RegisterRefresh(std::function<void()> refreshFunc);
    RSB_EXPORT void UpdateScreenStatus(ScreenId screenId, ScreenPowerStatus status);
    RSB_EXPORT bool IsDimmingOn(ScreenId screenId);
    RSB_EXPORT void DimmingIncrease(ScreenId screenId);
    RSB_EXPORT std::vector<float> GetNewLinearCct(ScreenId screenId);
#ifndef ROSEN_CROSS_PLATFORM
    RSB_EXPORT std::vector<float> GetLayerLinearCct(ScreenId, const std::vector<uint8_t>& metadata,
        const CM_Matrix targetMatrix = CM_Matrix::MATRIX_P3);
#endif

private:
    RSColorTemp() = default;
    ~RSColorTemp();

#ifdef ROSEN_OHOS
    bool LoadLibrary();
    void CloseLibrary();

    bool initStatus_{false};
    void *extLibHandle_{nullptr};
    RSCctInterface* rSCctInterface_{nullptr};

    using CreatFunc = RSCctInterface*(*)();
    using DestroyFunc = void(*)();

    CreatFunc create_{nullptr};
    DestroyFunc destroy_{nullptr};
#endif
};

} // Rosen
} // OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_COLOR_TEMP_H