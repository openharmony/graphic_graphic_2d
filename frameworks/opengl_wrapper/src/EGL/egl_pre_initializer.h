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
#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_PRE_INITIALIZER_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_PRE_INITIALIZER_H
#include "egl_defs.h"
#include "thread_private_data_ctl.h"
#include "wrapper_log.h"

namespace OHOS {
class EglPreInitializer {
public:
    EglPreInitializer() noexcept
    {
        WLOGD("");
        int numApis = sizeof(gGlHookNoContext) / sizeof(EglWrapperFuncPointer);
        EglWrapperFuncPointer *iter = reinterpret_cast<EglWrapperFuncPointer*>(&gGlHookNoContext);
        for (int i = 0; i < numApis; ++i) {
            *(iter++) = reinterpret_cast<EglWrapperFuncPointer>(CallGlApiNoContext);
        }
        ThreadPrivateDataCtl::SetGlHookTable(&gGlHookNoContext);
        preInitFlag = true;
    }
    static void CallGlApiNoContext(void)
    {
        WLOGE("Call To OpenGL ES API With No Current Context.");
        return;
    }
    bool InitStat() const
    {
        return preInitFlag;
    }
private:
    bool preInitFlag = false;
};
}; // namespace OHOS
#endif // FRAMEWORKS_OPENGL_WRAPPER_EGL_PRE_INITIALIZER_H
