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

#ifndef RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_LUMINANCE_CONTROL_H
#define RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_LUMINANCE_CONTROL_H

#include <cinttypes>

#include "common/rs_macros.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

enum CLOSEHDR_SCENEID : uint32_t {
    MULTI_DISPLAY = 0,
    CLOSEHDR_SCENEID_MAX
};

enum HdrStatus : uint32_t {
    NO_HDR = 0x0000,
    HDR_PHOTO = 0x0001,
    HDR_VIDEO = 0x0010,
    AI_HDR_VIDEO = 0x0100,
};

class RSB_EXPORT RSLuminanceControl {
public:
    RSLuminanceControl(const RSLuminanceControl&) = delete;
    RSLuminanceControl& operator=(const RSLuminanceControl&) = delete;
    RSLuminanceControl(RSLuminanceControl&&) = delete;
    RSLuminanceControl& operator=(RSLuminanceControl&&) = delete;

    RSB_EXPORT static RSLuminanceControl& Get();
    RSB_EXPORT void Init();

    RSB_EXPORT bool SetHdrStatus(ScreenId screenId, HdrStatus displayHdrstatus);
    RSB_EXPORT bool IsHdrOn(ScreenId screenId);
    RSB_EXPORT bool IsDimmingOn(ScreenId screenId);
    RSB_EXPORT void DimmingIncrease(ScreenId screenId);

    RSB_EXPORT void SetSdrLuminance(ScreenId screenId, uint32_t level);
    RSB_EXPORT uint32_t GetNewHdrLuminance(ScreenId screenId);
    RSB_EXPORT void SetNowHdrLuminance(ScreenId screenId, uint32_t level);
    RSB_EXPORT bool IsNeedUpdateLuminance(ScreenId screenId);

    RSB_EXPORT float GetHdrTmoNits(ScreenId screenId, int32_t mode);
    RSB_EXPORT float GetSdrDisplayNits(ScreenId screenId);
    RSB_EXPORT float GetHdrDisplayNits(ScreenId screenId);
    RSB_EXPORT float GetDisplayNits(ScreenId screenId);
    RSB_EXPORT double GetHdrBrightnessRatio(ScreenId screenId, int32_t mode);
    RSB_EXPORT float CalScaler(const float& maxContentLightLevel,
        int32_t dynamicMetadataSize, const float& ratio = 1.0f);
    RSB_EXPORT bool IsHdrPictureOn();

    RSB_EXPORT bool IsForceCloseHdr();
    RSB_EXPORT void ForceCloseHdr(uint32_t closeHdrSceneId, bool forceCloseHdr);

private:
    RSLuminanceControl() = default;
    ~RSLuminanceControl();
#ifdef ROSEN_OHOS
    bool LoadLibrary();
    bool LoadStatusControl();
    bool LoadLumControl();
    bool LoadTmoControl();
    void CloseLibrary();

    bool initStatus_{false};
    void *extLibHandle_{nullptr};

    using SetHdrStatusFunc = bool(*)(ScreenId, HdrStatus);
    using IsHdrOnFunc = bool(*)(ScreenId);
    using IsDimmingOnFunc = bool(*)(ScreenId);
    using DimmingIncreaseFunc = void(*)(ScreenId);
    using SetSdrLuminanceFunc = void(*)(ScreenId, uint32_t);
    using GetNewHdrLuminanceFunc = uint32_t(*)(ScreenId);
    using SetNowHdrLuminanceFunc = void(*)(ScreenId, uint32_t);
    using IsNeedUpdateLuminanceFunc = bool(*)(ScreenId);
    using GetHdrTmoNitsFunc = float(*)(ScreenId, int32_t);
    using GetSdrDisplayNitsFunc = float(*)(ScreenId);
    using GetHdrDisplayNitsFunc = float(*)(ScreenId);
    using GetDisplayNitsFunc = float(*)(ScreenId);
    using GetNonlinearRatioFunc = double(*)(ScreenId, int32_t);
    using CalScalerFunc = float(*)(const float&, int32_t, const float&);
    using IsHdrPictureOnFunc = bool(*)();
    using IsForceCloseHdrFunc = bool(*)();
    using ForceCloseHdrFunc = void(*)(uint32_t, bool);

    SetHdrStatusFunc setHdrStatus_{nullptr};
    IsHdrOnFunc isHdrOn_{nullptr};
    IsDimmingOnFunc isDimmingOn_{nullptr};
    DimmingIncreaseFunc dimmingIncrease_{nullptr};
    SetSdrLuminanceFunc setSdrLuminance_{nullptr};
    GetNewHdrLuminanceFunc getNewHdrLuminance_{nullptr};
    SetNowHdrLuminanceFunc setNowHdrLuminance_{nullptr};
    IsNeedUpdateLuminanceFunc isNeedUpdateLuminance_{nullptr};
    GetHdrTmoNitsFunc getHdrTmoNits_{nullptr};
    GetSdrDisplayNitsFunc getSdrDisplayNits_{nullptr};
    GetHdrDisplayNitsFunc getHdrDisplayNits_{nullptr};
    GetDisplayNitsFunc getDisplayNits_{nullptr};
    GetNonlinearRatioFunc getNonlinearRatio_{nullptr};
    CalScalerFunc calScaler_{nullptr};
    IsHdrPictureOnFunc isHdrPictureOn_{nullptr};
    IsForceCloseHdrFunc isForceCloseHdr_{nullptr};
    ForceCloseHdrFunc forceCloseHdr_{nullptr};
#endif
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_LUMINANCE_CONTROL_H
