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
#include "egl_core.h"
#include "egl_defs.h"
#include "../wrapper_log.h"
#include "../thread_private_data_ctl.h"

#undef CALL_HOOK_API
#define CALL_HOOK_API(api, ...)                                         \
    do {                                                                \
        if (OHOS::EglCoreInit() && OHOS::CheckEglWrapperApi(#api)) {    \
            OHOS::gWrapperHook.wrapper.api(__VA_ARGS__);                \
        } else {                                                        \
            WLOGE("%{public}s is invalid.", #api);                      \
        }                                                               \
    } while (0);                                                        \
}

#undef CALL_HOOK_API_RET
#define CALL_HOOK_API_RET(api, ...)                                     \
    do {                                                                \
        if (OHOS::EglCoreInit() && OHOS::CheckEglWrapperApi(#api)) {    \
            return OHOS::gWrapperHook.wrapper.api(__VA_ARGS__);         \
        } else {                                                        \
            WLOGE("%{public}s is invalid.", #api);                      \
            return 0;                                                   \
        }                                                               \
    } while (0);                                                        \
}

#undef HOOK_API_ENTRY
#define HOOK_API_ENTRY(r, api, ...) r api(__VA_ARGS__) {                \

extern "C" {
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "../egl_hook_entries.in"

OHOS::GlHookTable *GetHookTable()
{
    auto hookTable = OHOS::ThreadPrivateDataCtl::GetGlHookTable();
    if (hookTable == nullptr) {
        return &OHOS::gWrapperHook.gl;
    }
    return hookTable;
}

#pragma GCC diagnostic warning "-Wunused-parameter"
}

namespace OHOS {
const std::map<std::string, EglWrapperFuncPointer> gExtensionMap = {
    { "eglLockSurfaceKHR", (EglWrapperFuncPointer)&eglLockSurfaceKHR },
    { "eglUnlockSurfaceKHR", (EglWrapperFuncPointer)&eglUnlockSurfaceKHR },

    // EGL_KHR_image, EGL_KHR_image_base
    { "eglCreateImageKHR", (EglWrapperFuncPointer)&eglCreateImageKHR },
    { "eglDestroyImageKHR", (EglWrapperFuncPointer)&eglDestroyImageKHR },

    // EGL_KHR_reusable_sync, EGL_KHR_fence_sync
    { "eglCreateSyncKHR", (EglWrapperFuncPointer)&eglCreateSyncKHR },
    { "eglDestroySyncKHR", (EglWrapperFuncPointer)&eglDestroySyncKHR },
    { "eglClientWaitSyncKHR", (EglWrapperFuncPointer)&eglClientWaitSyncKHR },
    { "eglSignalSyncKHR", (EglWrapperFuncPointer)&eglSignalSyncKHR },
    { "eglGetSyncAttribKHR", (EglWrapperFuncPointer)&eglGetSyncAttribKHR },

    // EGL_KHR_wait_sync
    { "eglWaitSyncKHR", (EglWrapperFuncPointer)&eglWaitSyncKHR },

    // EGL_KHR_swap_buffers_with_damage
    { "eglSwapBuffersWithDamageKHR", (EglWrapperFuncPointer)&eglSwapBuffersWithDamageKHR },

    // EGL_KHR_partial_update
    { "eglSetDamageRegionKHR", (EglWrapperFuncPointer)&eglSetDamageRegionKHR },

    { "eglCreateStreamKHR", (EglWrapperFuncPointer)&eglCreateStreamKHR },
    { "eglDestroyStreamKHR", (EglWrapperFuncPointer)&eglDestroyStreamKHR },
    { "eglStreamAttribKHR", (EglWrapperFuncPointer)&eglStreamAttribKHR },
    { "eglQueryStreamKHR", (EglWrapperFuncPointer)&eglQueryStreamKHR },
    { "eglQueryStreamu64KHR", (EglWrapperFuncPointer)&eglQueryStreamu64KHR },
    { "eglQueryStreamTimeKHR", (EglWrapperFuncPointer)&eglQueryStreamTimeKHR },
    { "eglCreateStreamProducerSurfaceKHR", (EglWrapperFuncPointer)&eglCreateStreamProducerSurfaceKHR },
    { "eglStreamConsumerGLTextureExternalKHR", (EglWrapperFuncPointer)&eglStreamConsumerGLTextureExternalKHR },
    { "eglStreamConsumerAcquireKHR", (EglWrapperFuncPointer)&eglStreamConsumerAcquireKHR },
    { "eglStreamConsumerReleaseKHR", (EglWrapperFuncPointer)&eglStreamConsumerReleaseKHR },
    { "eglGetStreamFileDescriptorKHR", (EglWrapperFuncPointer)&eglGetStreamFileDescriptorKHR },
    { "eglCreateStreamFromFileDescriptorKHR", (EglWrapperFuncPointer)&eglCreateStreamFromFileDescriptorKHR },
};
}
