/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "display_engine/rs_display_effect_swing_control.h"
 
#include <dlfcn.h>
#include <string_view>
 
#include "platform/common/rs_log.h"
 
namespace {
constexpr std::string_view SWING_EXT_LIB_PATH = "system/lib64/libdisplay_effect_swing_ext.z.so";
}
 
namespace OHOS {
namespace Rosen {
RSDisplayEffectSwingControl& RSDisplayEffectSwingControl::Get()
{
    static RSDisplayEffectSwingControl instance;
    return instance;
}
 
RSDisplayEffectSwingControl::~RSDisplayEffectSwingControl()
{
    Deinit();
}
 
void RSDisplayEffectSwingControl::Init()
{
    std::lock_guard<std::mutex> lock(mLibMutex);
    if (isInited_) {
        return;
    }
    if (!LoadLibrary()) {
        RS_LOGE("DisplayEffectSwingControl LoadLibrary failed");
        return;
    }
    if (create_ != nullptr) {
        swingControlInterface_ = create_();
        if (swingControlInterface_ == nullptr) {
            RS_LOGE("DisplayEffectSwingControl Create Interface failed");
            CloseLibrary();
            return;
        }
    }
    isInited_ = true;
    RS_LOGI("DisplayEffectSwingControl Init success");
}
 
void RSDisplayEffectSwingControl::Deinit()
{
    std::lock_guard<std::mutex> lock(mLibMutex);
    if (!isInited_) {
        return;
    }
    if (destroy_ != nullptr) {
        destroy_();
    }
    CloseLibrary();
    swingControlInterface_ = nullptr;
    isInited_ = false;
    RS_LOGI("DisplayEffectSwingControl Deinit success");
}
 
bool RSDisplayEffectSwingControl::LoadLibrary()
{
    if (UNLIKELY(extLibHandle_ != nullptr)) {
        return false;
    }
    extLibHandle_ = dlopen(SWING_EXT_LIB_PATH.data(), RTLD_NOW);
    if (extLibHandle_ == nullptr) {
        RS_LOGE("DisplayEffectSwingControl dlopen error:%{public}s", dlerror());
        return false;
    }
    create_ = reinterpret_cast<CreateFunc>(dlsym(extLibHandle_, "Create"));
    if (create_ == nullptr) {
        RS_LOGE("DisplayEffectSwingControl dlsym create error: %{public}s", dlerror());
        CloseLibrary();
        return false;
    }
    destroy_ = reinterpret_cast<DestroyFunc>(dlsym(extLibHandle_, "Destroy"));
    if (destroy_ == nullptr) {
        RS_LOGE("DisplayEffectSwingControl dlsym destroy error: %{public}s", dlerror());
        CloseLibrary();
        return false;
    }
    RS_LOGI("DisplayEffectSwingControl LoadLibrary success");
    return true;
}
 
void RSDisplayEffectSwingControl::CloseLibrary()
{
    if (extLibHandle_ != nullptr) {
        dlclose(extLibHandle_);
        extLibHandle_ = nullptr;
    }
    create_ = nullptr;
    destroy_ = nullptr;
    RS_LOGI("DisplayEffectSwingControl CloseLibrary success");
}
 
void RSDisplayEffectSwingControl::SetSwingEnabled(ScreenId screenId, bool enable)
{
    std::lock_guard<std::mutex> lock(mLibMutex);
    if (swingControlInterface_ != nullptr) {
        swingControlInterface_->SetSwingEnabled(screenId, enable);
    }
}
 
bool RSDisplayEffectSwingControl::IsSwingRegistered(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(mLibMutex);
    return (swingControlInterface_ != nullptr) ? swingControlInterface_->IsSwingRegistered(screenId) : false;
}
 
SwingData RSDisplayEffectSwingControl::GetSwingData(ScreenId screenId)
{
    std::lock_guard<std::mutex> lock(mLibMutex);
    return (swingControlInterface_ != nullptr) ?
        swingControlInterface_->GetSwingData(screenId) : SwingData{};
}
} // namespace Rosen
} // namespace OHOS