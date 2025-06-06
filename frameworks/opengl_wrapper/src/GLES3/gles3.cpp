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

#include <dlfcn.h>
#include <string>

#include "hook.h"
#include "thread_private_data_ctl.h"
#include "wrapper_log.h"

using GetGlHookTableFunc = OHOS::GlHookTable* (*)();
using GetGlHookTableKeyFunc = pthread_key_t(*)();
template<typename Func = void*>
Func GetEglApi(const char* procname)
{
    constexpr const char *libWrapperName = "libEGL.so";
    void* dlEglHandle = dlopen(libWrapperName, RTLD_NOW | RTLD_GLOBAL);
    if (!dlEglHandle) {
        WLOGE("Failed to load EGL library (%s) using dlopen", libWrapperName);
        return nullptr;
    }

    void* func = dlsym(dlEglHandle, procname);
    if (func) {
        return reinterpret_cast<Func>(func);
    };
    dlclose(dlEglHandle);
    return nullptr;
}
static GetGlHookTableFunc g_pfnGetGlHookTable = GetEglApi<GetGlHookTableFunc>("GetHookTable");
static GetGlHookTableKeyFunc g_pfnGetGlHookTableKey = GetEglApi<GetGlHookTableKeyFunc>("GetHookTableKey");

static pthread_key_t g_glHookTableKey = -1;

__attribute__((__always_inline__))__inline__ static OHOS::GlHookTable *GetHookTable()
{
    if (__builtin_expect(g_glHookTableKey != static_cast<pthread_key_t>(-1), 1)) {
        OHOS::GlHookTable *table = static_cast<OHOS::GlHookTable *>(pthread_getspecific(g_glHookTableKey));
        if (__builtin_expect(table != nullptr, 1)) {
            return table;
        }
    }

    OHOS::GlHookTable *table = g_pfnGetGlHookTable();
    g_glHookTableKey = g_pfnGetGlHookTableKey();
    return table;
}

#undef CALL_HOOK_API
#define CALL_HOOK_API(api, ...)                                                         \
    do {                                                                                \
        OHOS::GlHookTable *table = GetHookTable();                               \
        if (table && table->table3.api) {                                               \
            table->table3.api(__VA_ARGS__);                                             \
        } else {                                                                        \
            WLOGE("%{public}s is invalid.", #api);                                      \
        }                                                                               \
    } while (0);                                                                        \
}

#undef CALL_HOOK_API_RET
#define CALL_HOOK_API_RET(api, ...) do {                                                \
        OHOS::GlHookTable *table = GetHookTable();                               \
        if (table && table->table3.api) {                                               \
            return table->table3.api(__VA_ARGS__);                                      \
        } else {                                                                        \
            WLOGE("%{public}s is invalid.", #api);                                      \
            return 0;                                                                   \
        }                                                                               \
    } while (0);                                                                        \
}
#undef HOOK_API_ENTRY
#define HOOK_API_ENTRY(r, api, ...) r api(__VA_ARGS__) {                                \

extern "C" {
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "gl3_hook_entries.in"
#pragma GCC diagnostic warning "-Wunused-parameter"
}
