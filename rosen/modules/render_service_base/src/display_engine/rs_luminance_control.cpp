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

#include "display_engine/rs_luminance_control.h"

#include <dlfcn.h>
#include <string_view>

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

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
        rSLumInterface_ = create_();
        if (rSLumInterface_ == nullptr) {
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

bool RSLuminanceControl::SetHdrStatus(ScreenId screenId, HdrStatus hdrstatus)
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->SetHdrStatus(screenId, hdrstatus) : false;
}

bool RSLuminanceControl::IsHdrOn(ScreenId screenId)
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->IsHdrOn(screenId) : false;
}

bool RSLuminanceControl::IsDimmingOn(ScreenId screenId)
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->IsDimmingOn(screenId) : false;
}

void RSLuminanceControl::DimmingIncrease(ScreenId screenId)
{
    if (rSLumInterface_ != nullptr) {
        rSLumInterface_->DimmingIncrease(screenId);
    }
}

void RSLuminanceControl::SetSdrLuminance(ScreenId screenId, uint32_t level)
{
    if (rSLumInterface_ != nullptr) {
        rSLumInterface_->SetSdrLuminance(screenId, level);
    }
}

uint32_t RSLuminanceControl::GetNewHdrLuminance(ScreenId screenId)
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->GetNewHdrLuminance(screenId) : DEFAULT_LEVEL;
}

void RSLuminanceControl::SetNowHdrLuminance(ScreenId screenId, uint32_t level)
{
    if (rSLumInterface_ != nullptr) {
        rSLumInterface_->SetNowHdrLuminance(screenId, level);
    }
}

bool RSLuminanceControl::IsNeedUpdateLuminance(ScreenId screenId)
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->IsNeedUpdateLuminance(screenId) : false;
}

float RSLuminanceControl::GetSdrDisplayNits(ScreenId screenId)
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->GetSdrDisplayNits(screenId) : HDR_DEFAULT_TMO_NIT;
}

float RSLuminanceControl::GetDisplayNits(ScreenId screenId)
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->GetDisplayNits(screenId) : HDR_DEFAULT_TMO_NIT;
}

double RSLuminanceControl::GetHdrBrightnessRatio(ScreenId screenId, int32_t mode)
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->GetNonlinearRatio(screenId, mode) : 1.0;
}

float RSLuminanceControl::CalScaler(const float& maxContentLightLevel, const std::vector<uint8_t>& dyMetadata,
    const float& ratio)
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->CalScaler(maxContentLightLevel, dyMetadata, ratio) :
        HDR_DEFAULT_SCALER * ratio;
}

bool RSLuminanceControl::IsHdrPictureOn()
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->IsHdrPictureOn() : false;
}

bool RSLuminanceControl::IsForceCloseHdr()
{
    return (rSLumInterface_ != nullptr) ? rSLumInterface_->IsForceCloseHdr() : false;
}

void RSLuminanceControl::ForceCloseHdr(uint32_t closeHdrSceneId, bool forceCloseHdr)
{
    if (rSLumInterface_ != nullptr) {
        rSLumInterface_->ForceCloseHdr(closeHdrSceneId, forceCloseHdr);
    }
}

bool RSLuminanceControl::IsCloseHardwareHdr()
{
    return (rSLumInterface_ != nullptr) ?
        rSLumInterface_->IsCloseHardwareHdr() : false;
}
} // namespace Rosen
} // namespace OHOS
