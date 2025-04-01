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

#include "display_engine/rs_color_temperature.h"

#include <dlfcn.h>
#include <string_view>

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace {
constexpr std::string_view EXT_LIB_PATH = "system/lib64/libcct_ext.z.so";
}

namespace OHOS {
namespace Rosen {
RSColorTemperature& RSColorTemperature::Get()
{
    static RSColorTemperature instance;
    return instance;
}

RSColorTemperature::~RSColorTemperature()
{
    if (initStatus_ && destroy_ != nullptr) {
        destroy_();
    }
    CloseLibrary();
}

void RSColorTemperature::CloseLibrary()
{
    if (extLibHandle_ != nullptr) {
        dlclose(extLibHandle_);
        extLibHandle_ = nullptr;
    }
    create_ = nullptr;
    destroy_ = nullptr;
}

void RSColorTemperature::Init()
{
    initStatus_ = LoadLibrary();
    if (!initStatus_) {
        CloseLibrary();
    }
    if (create_ != nullptr) {
        rSCctInterface_ = create_();
        if (rSCctInterface_ == nullptr) {
            CloseLibrary();
        }
    }
}

bool RSColorTemperature::LoadLibrary()
{
    if (UNLIKELY(extLibHandle_ != nullptr)) {
        return false;
    }
    extLibHandle_ = dlopen(EXT_LIB_PATH.data(), RTLD_NOW);
    if (extLibHandle_ == nullptr) {
        RS_LOGI("ColorTemp dlopen error:%{public}s", dlerror());
        return false;
    }
    create_ = reinterpret_cast<CreateFunc>(dlsym(extLibHandle_, "Create"));
    if (create_ == nullptr) {
        RS_LOGE("ColorTemp link create error!");
        return false;
    }
    destroy_ = reinterpret_cast<DestroyFunc>(dlsym(extLibHandle_, "Destroy"));
    if (destroy_ == nullptr) {
        RS_LOGE("ColorTemp link destroy error!");
        return false;
    }
    RS_LOGI("ColorTemp LoadLibrary success");
    return true;
}

void RSColorTemperature::RegisterRefresh(std::function<void()>&& refresh)
{
    if (rSCctInterface_ != nullptr) {
        rSCctInterface_->RegisterRefresh(std::forward<std::function<void()>>(refresh));
    }
}

void RSColorTemperature::UpdateScreenStatus(ScreenId screenId, ScreenPowerStatus status)
{
    if (rSCctInterface_ != nullptr) {
        rSCctInterface_->UpdateScreenStatus(screenId, status);
    }
}

bool RSColorTemperature::IsDimmingOn(ScreenId screenId)
{
    return (rSCctInterface_ != nullptr) ? rSCctInterface_->IsDimmingOn(screenId) : false;
}

void RSColorTemperature::DimmingIncrease(ScreenId screenId)
{
    if (rSCctInterface_ != nullptr) {
        rSCctInterface_->DimmingIncrease(screenId);
    }
}

std::vector<float> RSColorTemperature::GetNewLinearCct(ScreenId screenId)
{
    return (rSCctInterface_ != nullptr) ?
        rSCctInterface_->GetNewLinearCct(screenId) :
            std::vector<float>{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

std::vector<float> RSColorTemperature::GetLayerLinearCct(ScreenId screenId, const std::vector<uint8_t>& metadata,
    const CM_Matrix srcColorMatrix)
{
    return (rSCctInterface_ != nullptr) ?
        rSCctInterface_->GetLayerLinearCct(screenId, metadata, srcColorMatrix) :
            std::vector<float>{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}
} // Rosen
} // OHOS