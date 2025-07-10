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
#include <vector>

namespace OHOS {
namespace Rosen {

namespace RSLuminanceConst {
    constexpr float DEFAULT_CAPTURE_HDR_NITS = 1000.0f;
    constexpr float DEFAULT_CAST_HDR_NITS = 1000.0f;
    constexpr float DEFAULT_CAST_SDR_NITS = 203.0f;
}

enum CLOSEHDR_SCENEID : uint32_t {
    MULTI_DISPLAY = 0,
    CLOSEHDR_SCENEID_MAX
};

enum HDRComponentType : uint32_t {
    IMAGE = 0,
    UICOMPONENT,
    EFFECT,
};

enum HdrStatus : uint32_t {
    NO_HDR = 0x0000,
    HDR_PHOTO = 0x0001,
    HDR_VIDEO = 0x0010,
    AI_HDR_VIDEO = 0x0100,
    HDR_EFFECT = 0x1000,
};

class RSLuminanceControlInterface {
public:
    virtual ~RSLuminanceControlInterface() = default;
    virtual bool Init() = 0;
    virtual bool SetHdrStatus(ScreenId screenId, HdrStatus hdrStatus) = 0;
    virtual bool IsHdrOn(ScreenId screenId) const = 0;
    virtual bool IsDimmingOn(ScreenId screenId) = 0;
    virtual void DimmingIncrease(ScreenId screenId) = 0;
    virtual void SetSdrLuminance(ScreenId screenId, uint32_t level) = 0;
    virtual uint32_t GetNewHdrLuminance(ScreenId screenId) = 0;
    virtual void SetNowHdrLuminance(ScreenId screenId, uint32_t level) = 0;
    virtual bool IsNeedUpdateLuminance(ScreenId screenId) = 0;
    virtual float GetSdrDisplayNits(ScreenId screenId) = 0;
    virtual float GetDisplayNits(ScreenId screenId) = 0;
    virtual double GetNonlinearRatio(ScreenId screenId, uint32_t mode) = 0;
    virtual float CalScaler(const float& maxContentLightLevel,
        const std::vector<uint8_t>& dynamicMetadata, const float& ratio) = 0;
    virtual bool IsHdrPictureOn() = 0;
    virtual bool IsForceCloseHdr() const = 0;
    virtual void ForceCloseHdr(uint32_t closeHdrSceneId, bool forceCloseHdr) = 0;
    virtual bool IsCloseHardwareHdr() const = 0;
    virtual bool IsScreenNoHeadroom(ScreenId) = 0;
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

    RSB_EXPORT float GetSdrDisplayNits(ScreenId screenId);
    RSB_EXPORT float GetDisplayNits(ScreenId screenId);
    RSB_EXPORT double GetHdrBrightnessRatio(ScreenId screenId, uint32_t mode);
    RSB_EXPORT float CalScaler(const float& maxContentLightLevel,
        const std::vector<uint8_t>& dynamicMetadata, const float& ratio = 1.0f);
    RSB_EXPORT bool IsHdrPictureOn();
    RSB_EXPORT bool IsForceCloseHdr();
    RSB_EXPORT void ForceCloseHdr(uint32_t closeHdrSceneId, bool forceCloseHdr);
    RSB_EXPORT bool IsCloseHardwareHdr();
    RSB_EXPORT bool IsScreenNoHeadroom(ScreenId screenId);

private:
    RSLuminanceControl() = default;
    ~RSLuminanceControl();
#ifdef ROSEN_OHOS
    bool LoadLibrary();
    void CloseLibrary();

    bool initStatus_{false};
    void *extLibHandle_{nullptr};
    RSLuminanceControlInterface* rSLuminanceControlInterface_{nullptr};

    using CreateFunc = RSLuminanceControlInterface*(*)();
    using DestroyFunc = void(*)();

    CreateFunc create_{nullptr};
    DestroyFunc destroy_{nullptr};
#endif
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_LUMINANCE_CONTROL_H
