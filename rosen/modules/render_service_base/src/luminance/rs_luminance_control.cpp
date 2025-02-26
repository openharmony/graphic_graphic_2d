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

#include "luminance/rs_luminance_control.h"

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
    CloseLibrary();
}

void RSLuminanceControl::CloseLibrary()
{
    if (extLibHandle_ != nullptr) {
        dlclose(extLibHandle_);
        extLibHandle_ = nullptr;
    }
    setHdrStatus_ = nullptr;
    isHdrOn_ = nullptr;
    isDimmingOn_ = nullptr;
    dimmingIncrease_ = nullptr;
    setSdrLuminance_ = nullptr;
    getNewHdrLuminance_ = nullptr;
    setNowHdrLuminance_ = nullptr;
    isNeedUpdateLuminance_ = nullptr;
    getHdrTmoNits_ = nullptr;
    getSdrDisplayNits_ = nullptr;
    getHdrDisplayNits_ = nullptr;
    getDisplayNits_ = nullptr;
    getNonlinearRatio_ = nullptr;
    calScaler_ = nullptr;
    isHdrPictureOn_ = nullptr;
}

void RSLuminanceControl::Init()
{
    initStatus_ = LoadLibrary();
}

bool RSLuminanceControl::LoadLibrary()
{
    if (UNLIKELY(extLibHandle_ != nullptr)) {
        return false;
    }
    bool loadResult{true};
    extLibHandle_ = dlopen(EXT_LIB_PATH.data(), RTLD_NOW);
    if (extLibHandle_ == nullptr) {
        RS_LOGI("LumCtr dlopen error:%{public}s", dlerror());
        loadResult = false;
    }
    if (loadResult && !LoadStatusControl()) {
        CloseLibrary();
        loadResult = false;
    }
    if (loadResult && !LoadLumControl()) {
        CloseLibrary();
        loadResult = false;
    }
    if (loadResult && !LoadTmoControl()) {
        CloseLibrary();
        loadResult = false;
    }
    RS_LOGI("LumCtr init status:%{public}d", loadResult);
    return loadResult;
}

bool RSLuminanceControl::LoadStatusControl()
{
    if (UNLIKELY(extLibHandle_ == nullptr)) {
        return false;
    }
    setHdrStatus_ = reinterpret_cast<SetHdrStatusFunc>(dlsym(extLibHandle_, "SetHdrStatus"));
    if (setHdrStatus_ == nullptr) {
        RS_LOGE("LumCtr link SetHdrStatusFunc error!");
        return false;
    }
    isHdrOn_ = reinterpret_cast<IsHdrOnFunc>(dlsym(extLibHandle_, "IsHdrOn"));
    if (isHdrOn_ == nullptr) {
        RS_LOGE("LumCtr link IsHdrOn error!");
        return false;
    }
    isDimmingOn_ = reinterpret_cast<IsDimmingOnFunc>(dlsym(extLibHandle_, "IsDimmingOn"));
    if (isDimmingOn_ == nullptr) {
        RS_LOGE("LumCtr link IsDimmingOn error!");
        return false;
    }
    dimmingIncrease_ = reinterpret_cast<DimmingIncreaseFunc>(dlsym(extLibHandle_, "DimmingIncrease"));
    if (dimmingIncrease_ == nullptr) {
        RS_LOGE("LumCtr link IsDimmingOn error!");
        return false;
    }
    isHdrPictureOn_ = reinterpret_cast<IsHdrPictureOnFunc>(dlsym(extLibHandle_, "IsHdrPictureOn"));
    if (isHdrPictureOn_ == nullptr) {
        RS_LOGE("LumCtr link IsHdrPictureOn error!");
        return false;
    }
    isForceCloseHdr_ = reinterpret_cast<IsForceCloseHdrFunc>(dlsym(extLibHandle_, "IsForceCloseHdr"));
    if (isForceCloseHdr_ == nullptr) {
        RS_LOGE("LumCtr link IsForceCloseHdr error!");
        return false;
    }
    forceCloseHdr_ = reinterpret_cast<ForceCloseHdrFunc>(dlsym(extLibHandle_, "ForceCloseHdr"));
    if (forceCloseHdr_ == nullptr) {
        RS_LOGE("LumCtr link ForceCloseHdr error!");
        return false;
    }
    return true;
}

bool RSLuminanceControl::LoadLumControl()
{
    if (UNLIKELY(extLibHandle_ == nullptr)) {
        return false;
    }
    setSdrLuminance_ = reinterpret_cast<SetSdrLuminanceFunc>(dlsym(extLibHandle_, "SetSdrLuminance"));
    if (setSdrLuminance_ == nullptr) {
        RS_LOGE("LumCtr link SetSdrLuminance error!");
        return false;
    }
    getNewHdrLuminance_ = reinterpret_cast<GetNewHdrLuminanceFunc>(dlsym(extLibHandle_, "GetNewHdrLuminance"));
    if (getNewHdrLuminance_ == nullptr) {
        RS_LOGE("LumCtr link GetNewHdrLuminance error!");
        return false;
    }
    setNowHdrLuminance_ = reinterpret_cast<SetNowHdrLuminanceFunc>(dlsym(extLibHandle_, "SetNowHdrLuminance"));
    if (setNowHdrLuminance_ == nullptr) {
        RS_LOGE("LumCtr link SetNowHdrLuminance error!");
        return false;
    }
    isNeedUpdateLuminance_ = reinterpret_cast<IsNeedUpdateLuminanceFunc>(dlsym(extLibHandle_, "IsNeedUpdateLuminance"));
    if (isNeedUpdateLuminance_ == nullptr) {
        RS_LOGE("LumCtr link IsNeedUpdateLuminance error!");
        return false;
    }
    return true;
}

bool RSLuminanceControl::LoadTmoControl()
{
    if (UNLIKELY(extLibHandle_ == nullptr)) {
        return false;
    }
    getHdrTmoNits_ = reinterpret_cast<GetHdrTmoNitsFunc>(dlsym(extLibHandle_, "GetHdrTmoNits"));
    if (getHdrTmoNits_ == nullptr) {
        RS_LOGE("LumCtr link GetHdrTmoNits error!");
        return false;
    }
    getSdrDisplayNits_ = reinterpret_cast<GetSdrDisplayNitsFunc>(dlsym(extLibHandle_, "GetSdrDisplayNits"));
    if (getSdrDisplayNits_ == nullptr) {
        RS_LOGE("LumCtr link GetSdrDisplayNits error!");
        return false;
    }
    getHdrDisplayNits_ = reinterpret_cast<GetHdrDisplayNitsFunc>(dlsym(extLibHandle_, "GetHdrDisplayNits"));
    if (getHdrDisplayNits_ == nullptr) {
        RS_LOGE("LumCtr link GetHdrDisplayNits error!");
        return false;
    }
    getDisplayNits_ = reinterpret_cast<GetDisplayNitsFunc>(dlsym(extLibHandle_, "GetDisplayNits"));
    if (getDisplayNits_ == nullptr) {
        RS_LOGE("LumCtr link GetDisplayNits error!");
        return false;
    }
    getNonlinearRatio_ = reinterpret_cast<GetNonlinearRatioFunc>(dlsym(extLibHandle_, "GetNonlinearRatio"));
    if (getNonlinearRatio_ == nullptr) {
        RS_LOGE("LumCtr link GetHdrBrightnessRatio error!");
        return false;
    }
    calScaler_ = reinterpret_cast<CalScalerFunc>(dlsym(extLibHandle_, "CalScaler"));
    if (calScaler_ == nullptr) {
        RS_LOGE("LumCtr link CalScaler error!");
        return false;
    }
    return true;
}

bool RSLuminanceControl::SetHdrStatus(ScreenId screenId, HdrStatus hdrstatus)
{
    return (initStatus_ && setHdrStatus_ != nullptr) ? setHdrStatus_(screenId, hdrstatus) : false;
}

bool RSLuminanceControl::IsHdrOn(ScreenId screenId)
{
    return (initStatus_ && isHdrOn_ != nullptr) ? isHdrOn_(screenId) : false;
}

bool RSLuminanceControl::IsDimmingOn(ScreenId screenId)
{
    return (initStatus_ && isDimmingOn_ != nullptr) ? isDimmingOn_(screenId) : false;
}

void RSLuminanceControl::DimmingIncrease(ScreenId screenId)
{
    if (initStatus_ && dimmingIncrease_ != nullptr) {
        dimmingIncrease_(screenId);
    }
}

void RSLuminanceControl::SetSdrLuminance(ScreenId screenId, uint32_t level)
{
    if (initStatus_ && setSdrLuminance_ != nullptr) {
        setSdrLuminance_(screenId, level);
    }
}

uint32_t RSLuminanceControl::GetNewHdrLuminance(ScreenId screenId)
{
    return (initStatus_ && getNewHdrLuminance_ != nullptr) ? getNewHdrLuminance_(screenId) : DEFAULT_LEVEL;
}

void RSLuminanceControl::SetNowHdrLuminance(ScreenId screenId, uint32_t level)
{
    if (initStatus_ && setNowHdrLuminance_ != nullptr) {
        setNowHdrLuminance_(screenId, level);
    }
}

bool RSLuminanceControl::IsNeedUpdateLuminance(ScreenId screenId)
{
    return (initStatus_ && isNeedUpdateLuminance_ != nullptr) ? isNeedUpdateLuminance_(screenId) : false;
}

float RSLuminanceControl::GetHdrTmoNits(ScreenId screenId, int32_t mode)
{
    return (initStatus_ && getHdrTmoNits_ != nullptr) ? getHdrTmoNits_(screenId, mode) : HDR_DEFAULT_TMO_NIT;
}

float RSLuminanceControl::GetSdrDisplayNits(ScreenId screenId)
{
    return (initStatus_ && getSdrDisplayNits_ != nullptr) ? getSdrDisplayNits_(screenId) : HDR_DEFAULT_TMO_NIT;
}

float RSLuminanceControl::GetHdrDisplayNits(ScreenId screenId)
{
    return (initStatus_ && getHdrDisplayNits_ != nullptr) ? getHdrDisplayNits_(screenId) : HDR_DEFAULT_TMO_NIT;
}

float RSLuminanceControl::GetDisplayNits(ScreenId screenId)
{
    return (initStatus_ && getDisplayNits_ != nullptr) ? getDisplayNits_(screenId) : HDR_DEFAULT_TMO_NIT;
}

double RSLuminanceControl::GetHdrBrightnessRatio(ScreenId screenId, int32_t mode)
{
    return (initStatus_ && getNonlinearRatio_ != nullptr) ? getNonlinearRatio_(screenId, mode) : 1.0;
}

float RSLuminanceControl::CalScaler(const float& maxContentLightLevel, int32_t dynamicMetadataSize, const float& ratio)
{
    return (initStatus_ && calScaler_ != nullptr) ? calScaler_(maxContentLightLevel, dynamicMetadataSize, ratio) :
        HDR_DEFAULT_SCALER * ratio;
}

bool RSLuminanceControl::IsHdrPictureOn()
{
    return (initStatus_ && isHdrPictureOn_ != nullptr) ? isHdrPictureOn_() : false;
}

bool RSLuminanceControl::IsForceCloseHdr()
{
    return (initStatus_ && isForceCloseHdr_ != nullptr) ? isForceCloseHdr_() : false;
}

void RSLuminanceControl::ForceCloseHdr(uint32_t closeHdrSceneId, bool forceCloseHdr)
{
    if (initStatus_ && forceCloseHdr_ != nullptr) {
        forceCloseHdr_(closeHdrSceneId, forceCloseHdr);
    }
}
} // namespace Rosen
} // namespace OHOS
