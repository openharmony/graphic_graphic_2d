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

#include "common/rs_common_def.h"
#include "display_engine/rs_luminance_control.h"
#include <dlfcn.h>
#include "platform/common/rs_log.h"
#include <string_view>

namespace {
constexpr float HDR_DEFAULT_TMO_NIT = 1000.0f;
constexpr float HDR_DEFAULT_SCALER = 1000.0f / 203.0f;
constexpr int32_t DEFAULT_LEVEL = 255;
constexpr std::string_view EXT_LIB_PATH = "system/lib64/libluminance_ext.z.so";
}

namespace OHOS {
namespace Rosen {
RSLuminanceControl& RSLuminanceControl::Get()
{
    static RSLuminanceControl instance;
    return instance;
}

RSLuminanceControl::~RSLuminanceControl()
{
    if (initStatus_ && destroy_ != nullptr) {
        destroy_();
    }
    CloseLibrary();
}

void RSLuminanceControl::CloseLibrary()
{
    if (extLibHandle_ != nullptr) {
        dlclose(extLibHandle_);
        extLibHandle_ = nullptr;
    }
    initStatus_ = false;
    create_ = nullptr;
    destroy_ = nullptr;
}

void RSLuminanceControl::Init()
{
    initStatus_ = LoadLibrary();
    if (!initStatus_) {
        CloseLibrary();
    }
    if (create_ != nullptr) {
        rSLuminanceControlInterface_ = create_();
        if (rSLuminanceControlInterface_ == nullptr) {
            CloseLibrary();
        }
    }
}

bool RSLuminanceControl::LoadLibrary()
{
    if (UNLIKELY(extLibHandle_ != nullptr)) {
        return false;
    }
    extLibHandle_ = dlopen(EXT_LIB_PATH.data(), RTLD_NOW);
    if (extLibHandle_ == nullptr) {
        RS_LOGI("LumControl dlopen error:%{public}s", dlerror());
        return false;
    }
    create_ = reinterpret_cast<CreateFunc>(dlsym(extLibHandle_, "Create"));
    if (create_ == nullptr) {
        RS_LOGE("LumControl link create error!");
        return false;
    }
    destroy_ = reinterpret_cast<DestroyFunc>(dlsym(extLibHandle_, "Destroy"));
    if (destroy_ == nullptr) {
        RS_LOGE("LumControl link destroy error!");
        return false;
    }
    RS_LOGI("LumControl LoadLibrary success");
    return true;
}

bool RSLuminanceControl::SetHdrStatus(ScreenId screenId, HdrStatus hdrStatus)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->SetHdrStatus(screenId, hdrStatus) : false;
}

bool RSLuminanceControl::IsHdrOn(ScreenId screenId)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->IsHdrOn(screenId) : false;
}

bool RSLuminanceControl::IsDimmingOn(ScreenId screenId)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->IsDimmingOn(screenId) : false;
}

void RSLuminanceControl::DimmingIncrease(ScreenId screenId)
{
    if (rSLuminanceControlInterface_ != nullptr) {
        rSLuminanceControlInterface_->DimmingIncrease(screenId);
    }
}

void RSLuminanceControl::SetSdrLuminance(ScreenId screenId, uint32_t level)
{
    if (rSLuminanceControlInterface_ != nullptr) {
        rSLuminanceControlInterface_->SetSdrLuminance(screenId, level);
    }
}

uint32_t RSLuminanceControl::GetNewHdrLuminance(ScreenId screenId)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->GetNewHdrLuminance(screenId) : DEFAULT_LEVEL;
}

void RSLuminanceControl::SetNowHdrLuminance(ScreenId screenId, uint32_t level)
{
    if (rSLuminanceControlInterface_ != nullptr) {
        rSLuminanceControlInterface_->SetNowHdrLuminance(screenId, level);
    }
}

bool RSLuminanceControl::IsNeedUpdateLuminance(ScreenId screenId)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->IsNeedUpdateLuminance(screenId) : false;
}

float RSLuminanceControl::GetSdrDisplayNits(ScreenId screenId)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->GetSdrDisplayNits(screenId) : HDR_DEFAULT_TMO_NIT;
}

float RSLuminanceControl::GetDisplayNits(ScreenId screenId)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->GetDisplayNits(screenId) : HDR_DEFAULT_TMO_NIT;
}

double RSLuminanceControl::GetHdrBrightnessRatio(ScreenId screenId, uint32_t mode)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->GetNonlinearRatio(screenId, mode) : 1.0;
}

float RSLuminanceControl::CalScaler(const float& maxContentLightLevel,
    const std::vector<uint8_t>& dynamicMetadata, const float& ratio)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->CalScaler(maxContentLightLevel,
            dynamicMetadata, ratio) : HDR_DEFAULT_SCALER * ratio;
}

bool RSLuminanceControl::IsHdrPictureOn()
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->IsHdrPictureOn() : false;
}

bool RSLuminanceControl::IsForceCloseHdr()
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->IsForceCloseHdr() : false;
}

void RSLuminanceControl::ForceCloseHdr(uint32_t closeHdrSceneId, bool forceCloseHdr)
{
    if (rSLuminanceControlInterface_ != nullptr) {
        rSLuminanceControlInterface_->ForceCloseHdr(closeHdrSceneId, forceCloseHdr);
    }
}

bool RSLuminanceControl::IsCloseHardwareHdr()
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->IsCloseHardwareHdr() : false;
}

bool RSLuminanceControl::IsScreenNoHeadroom(ScreenId screenId)
{
    return (rSLuminanceControlInterface_ != nullptr) ?
        rSLuminanceControlInterface_->IsScreenNoHeadroom(screenId) : false;
}
} // namespace Rosen
} // namespace OHOS
