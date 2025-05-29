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

#include "boot_animation_strategy.h"

#include <dlfcn.h>
#include "log.h"
#include <parameters.h>
#include "util.h"

namespace OHOS {
namespace {
    constexpr const char* DUE_UPDATE_TYPE_PARAM = "persist.dupdate_engine.update_type";
    constexpr const char* HMOS_UPDATE_PARAM = "persist.update.hmos_to_next_flag";
    const std::string DUE_UPDATE_TYPE_MANUAL = "manual";
    const std::string DUE_UPDATE_TYPE_NIGHT = "night";
}

bool BootAnimationStrategy::CheckExitAnimation()
{
    if (!isAnimationEnd_) {
        LOGI("boot animation is end");
        if (!system::GetBoolParameter(BOOT_ANIMATION_STARTED, false)) {
            system::SetParameter(BOOT_ANIMATION_STARTED, "true");
        }
        system::SetParameter(BOOT_ANIMATION_FINISHED, "true");
        isAnimationEnd_ = true;
    }
    bool bootEventCompleted = system::GetBoolParameter(BOOT_COMPLETED, false);
    if (bootEventCompleted) {
        LOGI("read boot completed is true");
#ifdef FEATURE_CHECK_EXIT_ANIMATION_EXT
        return CheckExitAnimationExt();
#else
        return true;
#endif
    }
    return false;
}

#ifdef FEATURE_CHECK_EXIT_ANIMATION_EXT
#define CHECK_EXIT_ANIMATION_EXT_PATH "libwatch_bootanimation_ext.z.so"
#define CHECK_EXIT_ANIMATION_EXT_FUNC_NAME "CheckExitAnimationExt"
typedef bool(*Func)();
bool BootAnimationStrategy::CheckExitAnimationExt()
{
    LOGI("CheckExitAnimationExt");
    void *handler = dlopen(CHECK_EXIT_ANIMATION_EXT_PATH, RTLD_LAZY | RTLD_NODELETE);
    if (handler == nullptr) {
        LOGI("CheckExitAnimationExt Dlopen failed, reason: %{public}s", dlerror());
        dlclose(handler);
        return true;
    }

    Func CheckExitAnimationExtFunc = (Func)dlsym(handler, CHECK_EXIT_ANIMATION_EXT_FUNC_NAME);
    if (CheckExitAnimationExtFunc == nullptr) {
        LOGI("CheckExitAnimationExt find function failed, reason: %{public}s", dlerror());
        dlclose(handler);
        return true;
    }

    bool resCode = CheckExitAnimationExtFunc();
    dlclose(handler);
    return resCode;
}
#endif

bool BootAnimationStrategy::IsOtaUpdate() const
{
    std::string dueUpdateType = system::GetParameter(DUE_UPDATE_TYPE_PARAM, "");
    LOGI("dueUpdateType is: %{public}s", dueUpdateType.c_str());
    bool isSingleUpdate = dueUpdateType == DUE_UPDATE_TYPE_MANUAL || dueUpdateType == DUE_UPDATE_TYPE_NIGHT;
    bool isHmosUpdate = system::GetIntParameter(HMOS_UPDATE_PARAM, -1) == 1;
    LOGI("isSingleUpdate: %{public}d, isHmosUpdate: %{public}d", isSingleUpdate, isHmosUpdate);
    return isSingleUpdate || isHmosUpdate;
}
} // namespace OHOS
