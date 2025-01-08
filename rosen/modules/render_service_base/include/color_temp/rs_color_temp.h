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

namespace OHOS {
namespace Rosen {

class RSCctInterface {
public:
    virtual void RegisterRefresh(std::function<void()>) = 0;
    virtual void UpdateScreenStatus(ScreenId, ScreenPowerStatus) = 0;
    virtual bool IsDimmingOn(ScreenId) = 0;
    virtual void DimmingIncrease(ScreenId) = 0;
    virtual bool IsNeedUpdateLinearCct(ScreenId) = 0;
    virtual std::vector<float> GetNewLinearCct(ScreenId) = 0;
    virtual std::vector<float> GetLayerLinearCct(ScreenId, const std::vector<uint8_t>&) = 0;
};

class RSB_EXPORT RSColorTemp {
public:
    RSColorTemp(const RSColorTemp&) = delete;
    RSColorTemp& operator=(const RSColorTemp&) = delete;
    RSColorTemp(RSColorTemp&) = delete;
    RSColorTemp& operator=(RSColorTemp&&) = delete;

    RSB_EXPORT static RSColorTemp& Instance();
    RSB_EXPORT void Init();

    RSB_EXPORT void RegisterRefresh(std::function<void()>);
    RSB_EXPORT void UpdateScreenStatus(ScreenId, ScreenPowerStatus);
    RSB_EXPORT bool IsDimmingOn(ScreenId);
    RSB_EXPORT void DimmingIncrease(ScreenId);
    RSB_EXPORT bool IsNeedUpdateLinearCct(ScreenId);
    RSB_EXPORT std::vector<float> GetNewLinearCct(ScreenId);
    RSB_EXPORT std::vector<float> GetLayerLinearCct(ScreenId, const std::vector<uint8_t>&);

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