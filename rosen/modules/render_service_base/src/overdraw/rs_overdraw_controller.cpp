/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "overdraw/rs_overdraw_controller.h"

#include <hilog/log.h>
#include <parameter.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char *switchText = "debug.graphic.overdraw";
constexpr const char *switchEnableText = "true";
} // namespace

RSOverdrawController &RSOverdrawController::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard lock(mutex);
        if (instance == nullptr) {
            instance = std::unique_ptr<RSOverdrawController>(new RSOverdrawController());
        }
    }

    return *instance;
}

RSOverdrawController::RSOverdrawController()
{
    char value[0x20];
    GetParameter(switchText, "false", value, sizeof(value));
    SwitchFunction(switchText, value, this);
    WatchParameter(switchText, SwitchFunction, this);
}

void RSOverdrawController::SwitchFunction(const char *key, const char *value, void *context)
{
    auto &that = *reinterpret_cast<RSOverdrawController *>(context);
    if (strncmp(value, switchEnableText, strlen(switchEnableText)) == 0) {
        that.enabled_ = true;
        ROSEN_LOGI("%{public}s enable", key);
    } else {
        that.enabled_ = false;
        ROSEN_LOGI("%{public}s disable", key);
    }
}
} // namespace Rosen
} // namespace OHOS
