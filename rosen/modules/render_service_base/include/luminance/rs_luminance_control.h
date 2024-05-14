/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_LUMINANCE_CONTROL_H
#define ROSEN_RENDER_SERVICE_BASE_RS_LUMINANCE_CONTROL_H

#include <cinttypes>

#include "common/rs_macros.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSLuminanceControl {
public:
    RSB_EXPORT static RSLuminanceControl& Instance()
    {
        static RSLuminanceControl instance;
        return instance;
    }
    RSLuminanceControl(const RSLuminanceControl&) = delete;
    RSLuminanceControl& operator=(const RSLuminanceControl&) = delete;
    RSLuminanceControl(RSLuminanceControl&&) = delete;
    RSLuminanceControl& operator=(RSLuminanceControl&&) = delete;

    RSB_EXPORT void Init();

    RSB_EXPORT bool SetHdrStatus(ScreenId screenId, bool isHdrOn);
    RSB_EXPORT bool IsHdrOn(ScreenId screenId);
    RSB_EXPORT bool IsDimmingOn(ScreenId screenId);
    RSB_EXPORT void DimmingIncrease(ScreenId screenId);

    RSB_EXPORT void SetSdrLuminance(ScreenId screenId, uint32_t level);
    RSB_EXPORT uint32_t GetNewHdrLuminance(ScreenId screenId);
    RSB_EXPORT void SetNowHdrLuminance(ScreenId screenId, uint32_t level);
    RSB_EXPORT bool IsNeedUpdateLuminance(ScreenId screenId);

    RSB_EXPORT float GetHdrTmoNits(ScreenId screenId, int32_t mode);
    RSB_EXPORT float GetHdrDisplayNits(ScreenId screenId);
    RSB_EXPORT double GetHdrBrightnessRatio(ScreenId screenId, int32_t mode);

private:
    RSLuminanceControl() = default;
    ~RSLuminanceControl();
    bool LoadLibrary();
    bool LoadStatusControl();
    bool LoadLumControl();
    bool LoadTmoControl();
    void CloseLibrary();

    bool initStatus_{false};
    void *extLibHandle_{nullptr};

    using SetHdrStatusFunc = bool(*)(ScreenId, bool);
    using IsHdrOnFunc = bool(*)(ScreenId);
    using IsDimmingOnFunc = bool(*)(ScreenId);
    using DimmingIncreaseFunc = void(*)(ScreenId);
    using SetSdrLuminanceFunc = void(*)(ScreenId, uint32_t);
    using GetNewHdrLuminanceFunc = uint32_t(*)(ScreenId);
    using SetNowHdrLuminanceFunc = void(*)(ScreenId, uint32_t);
    using IsNeedUpdateLuminanceFunc = bool(*)(ScreenId);
    using GetHdrTmoNitsFunc = float(*)(ScreenId, int32_t);
    using GetHdrDisplayNitsFunc = float(*)(ScreenId);
    using GetNonlinearRatioFunc = double(*)(ScreenId, int32_t);

    SetHdrStatusFunc setHdrStatus_{nullptr};
    IsHdrOnFunc isHdrOn_{nullptr};
    IsDimmingOnFunc isDimmingOn_{nullptr};
    DimmingIncreaseFunc dimmingIncrease_{nullptr};
    SetSdrLuminanceFunc setSdrLuminance_{nullptr};
    GetNewHdrLuminanceFunc getNewHdrLuminance_{nullptr};
    SetNowHdrLuminanceFunc setNowHdrLuminance_{nullptr};
    IsNeedUpdateLuminanceFunc isNeedUpdateLuminance_{nullptr};
    GetHdrTmoNitsFunc getHdrTmoNits_{nullptr};
    GetHdrDisplayNitsFunc getHdrDisplayNits_{nullptr};
    GetNonlinearRatioFunc getNonlinearRatio_{nullptr};

};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_LUMINANCE_CONTROL_H
