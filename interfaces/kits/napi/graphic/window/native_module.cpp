/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "native_module.h"

#include <hilog/log.h>
#include <napi/native_api.h>

#include "window.h"

namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WindowNAPILayer" };
#define GNAPI_LOG(fmt, ...) ::OHOS::HiviewDFX::HiLog::Info(LABEL, \
    "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)

static napi_value WindowModuleInit(napi_env env, napi_value exports)
{
    napi_status ret = WindowInit(env, exports);
    if (ret != napi_ok) {
        GNAPI_LOG("WindowInit failed");
    }

    return exports;
}
}

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module module = {
        .nm_version = 1, // NAPI v1
        .nm_flags = 0, // normal
        .nm_filename = nullptr,
        .nm_register_func = WindowModuleInit,
        .nm_modname = "window",
        .nm_priv = nullptr,
        .reserved = {}
    };
    napi_module_register(&module);
}
