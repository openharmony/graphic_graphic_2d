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

#include "color_temp/rs_color_temp.h"

#include <dlfcn.h>
#include <string_view>

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace {
constexpr std::string_view EXT_LIB_PATH = "system/lib64/libcct_ext.z.so";
}

namespace OHOS {
namespace Rosen {
RSColorTemp& RSColorTemp::Get()
{
    static RSColorTemp instance;
    return instance;
}

RSColorTemp::~RSColorTemp()
{
    if (initStatus_ && destroy_ != nullptr) {
        destroy_();
    }
    CloseLibrary();
}

void RSColorTemp::CloseLibrary()
{
    if (extLibHandle_ != nullptr) {
        dlclose(extLibHandle_);
        extLibHandle_ = nullptr;
    }
    create_ = nullptr;
    destroy_ = nullptr;
}

void RSColorTemp::Init()
{
    initStatus_ = LoadLibrary();
    if (initStatus_ && create_ != nullptr) {
        rSCctInterface_ = create_();
    }
}

bool RSColorTemp::LoadLibrary()
{
    if (UNLIKELY(extLibHandle_ != nullptr)) {
        return false;
    }
    bool loadResult{true};
    extLibHandle_ = dlopen(EXT_LIB_PATH.data(), RTLD_NOW);
    if (extLibHandle_ == nullptr) {
        RS_LOGI("ColorTemp dlopen error:%{public}s", dlerror());
        loadResult = false;
    }
    create_ = reinterpret_cast<CreatFunc>(dlsym(extLibHandle_, "Create"));
    if (create_ == nullptr) {
        RS_LOGE("ColorTemp link create error!");
        loadResult = false;
    }
    destroy_ = reinterpret_cast<DestroyFunc>(dlsym(extLibHandle_, "Destroy"));
    if (destroy_ == nullptr) {
        RS_LOGE("ColorTemp link destroy error!");
        loadResult = false;
    }
    RS_LOGI("ColorTemp init status:%{public}d", loadResult);
    return loadResult;
}

void RSColorTemp::RegisterRefresh(std::function<void()> refresh)
{
    if (rSCctInterface_ != nullptr) {
        rSCctInterface_->RegisterRefresh(refresh);
    }
}

void RSColorTemp::UpdateScreenStatus(ScreenId screenId, ScreenPowerStatus status)
{
    if (rSCctInterface_ != nullptr) {
        rSCctInterface_->UpdateScreenStatus(screenId, status);
    }
}

bool RSColorTemp::IsDimmingOn(ScreenId screenId)
{
    return (rSCctInterface_ != nullptr) ? rSCctInterface_->IsDimmingOn(screenId) : false;
}

void RSColorTemp::DimmingIncrease(ScreenId screenId)
{
    if (rSCctInterface_ != nullptr) {
        rSCctInterface_->DimmingIncrease(screenId);
    }
}

std::vector<float> RSColorTemp::GetNewLinearCct(ScreenId screenId)
{
    return (rSCctInterface_ != nullptr) ?
        rSCctInterface_->GetNewLinearCct(screenId) : std::vector<float>();
}

std::vector<float> RSColorTemp::GetLayerLinearCct(ScreenId screenId, const std::vector<uint8_t>& metadata,
    const CM_Matrix targetMatrix)
{
    return (rSCctInterface_ != nullptr) ?
        rSCctInterface_->GetLayerLinearCct(screenId, metadata, targetMatrix) : std::vector<float>();
}
} // Rosen
} // OHOS