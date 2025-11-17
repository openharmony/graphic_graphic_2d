/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <map>
#include <string>

#include "egl_defs.h"
#include "egl_wrapper_loader.h"
#include "parameters.h"
#include "wrapper_log.h"
#ifdef OPENGL_WRAPPER_ENABLE_GL4
EGLBoolean OHGraphicsQueryGLImpl(void)
{
    EGLBoolean supportOpengl = OHOS::system::GetBoolParameter(OHOS::SUPPORT_GL_TO_VK, false) ? EGL_TRUE : EGL_FALSE;
    return supportOpengl;
}

__eglMustCastToProperFunctionPointerType EglGetProcAddressCustomImpl(const char *procname)
{
    void* func = nullptr;
    if (OHOS::gGetProcAddress && OHOS::gGetProcAddress != &EglGetProcAddressCustomImpl) {
        func = reinterpret_cast<void *>(OHOS::gGetProcAddress(procname));
    }

    if (func) {
        return __eglMustCastToProperFunctionPointerType(func);
    }

    auto it = OHOS::gCustomMap.find(procname);
    if (it != OHOS::gCustomMap.end()) {
        return __eglMustCastToProperFunctionPointerType(it->second);
    }
    WLOGD("EglGetProcAddressCustomImpl did not find an entry for %{public}s", procname);
    return nullptr;
}

const std::map<std::string, EglWrapperFuncPointer> OHOS::gCustomMap = {
    // OH_Graphics_QueryGL
    { "OH_Graphics_QueryGL", (EglWrapperFuncPointer)&OHGraphicsQueryGLImpl },
};

PFNEGLGETPROCADDRESSPROC OHOS::gGetProcAddress = &EglGetProcAddressCustomImpl;
#endif