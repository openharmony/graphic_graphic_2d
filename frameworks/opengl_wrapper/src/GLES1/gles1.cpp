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
#include "hook.h"
#include "thread_private_data_ctl.h"
#include "wrapper_log.h"

#undef CALL_HOOK_API
#define CALL_HOOK_API(api, ...)                                                         \
    do {                                                                                \
        OHOS::GlHookTable const *table = OHOS::ThreadPrivateDataCtl::GetGlHookTable();  \
        if (table && table->table1.api) {                                               \
            table->table1.api(__VA_ARGS__);                                             \
        } else {                                                                        \
            WLOGE("%{public}s is invalid.", #api);                                      \
        }                                                                               \
    } while (0);                                                                        \
}

#undef CALL_HOOK_API_RET
#define CALL_HOOK_API_RET(api, ...) do {                                                \
        OHOS::GlHookTable const *table = OHOS::ThreadPrivateDataCtl::GetGlHookTable();  \
        if (table && table->table1.api) {                                               \
            return table->table1.api(__VA_ARGS__);                                      \
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
#include "gl1_hook_entries.in"
#pragma GCC diagnostic warning "-Wunused-parameter"
}
