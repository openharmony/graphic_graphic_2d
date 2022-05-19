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

#include "../hook.h"
#include "../thread_private_data.h"
#include "../wrapper_log.h"

using namespace OHOS;

#undef HOOK_API_ENTRY
#undef CALL_HOOK_API
#undef CALL_HOOK_API_RET

#define CALL_HOOK_API(api, ...)  \
    WLOGD("");\
    GlHookTable const *table = ThreadPrivateDataCtl::GetGlHookTable();\
    if (table && table->table1.api) {\
        return table->table1.api(__VA_ARGS__);\
    }\
    else {\
        WLOGE("%{public}s is invalid.", #api);\
    }\
    return;\
}

#define CALL_HOOK_API_RET(api, ...)  \
    WLOGD("");\
    GlHookTable const *table = ThreadPrivateDataCtl::GetGlHookTable();\
    if (table && table->table1.api) {\
        return table->table1.api(__VA_ARGS__);\
    }\
    else {\
        WLOGE("%{public}s is invalid.", #api);\
    }\
    return 0;\
}

#define HOOK_API_ENTRY(r, api, ...) r api(__VA_ARGS__) { \

extern "C" {
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "../gl1_hook_entries.in"
#pragma GCC diagnostic warning "-Wunused-parameter"
}

#undef HOOK_API_ENTRY
#undef CALL_HOOK_API
#undef CALL_HOOK_API_RET
