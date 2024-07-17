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
#include "utils/log.h"
#include <native_engine/native_engine.h>
#include "js_text_init.h"

static napi_module g_textModule = {
    .nm_filename = "libtext_napi.so/text.js",
    .nm_register_func = OHOS::Rosen::TextInit,
    .nm_modname = "graphics.text",
};

extern "C" __attribute__((constructor)) void NAPI_text_AutoRegister()
{
    napi_module_register(&g_textModule);
}