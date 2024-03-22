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

#include "debug_layer_test.h"

#include <string>

#include "wrapper_log.h"

namespace {
constexpr const char *MY_LOG_TAG = "DebugLayerTest2";
}
static GetNextLayerAddr gGetNextLayerAddr = nullptr;
static void *gFuncTbl = nullptr;

static void *GetNextLayerProc(const char *name)
{
    if (gGetNextLayerAddr && gFuncTbl) {
        return gGetNextLayerAddr(gFuncTbl, name);
    }
    WLOGD("-%{public}s- GetNextLayerProc init error.", MY_LOG_TAG);
    return nullptr;
}

static EGLDisplay EglGetDisplay(EGLNativeDisplayType type)
{
    WLOGD("-%{public}s-", MY_LOG_TAG);
    PFNEGLGETDISPLAYPROC next = (PFNEGLGETDISPLAYPROC)GetNextLayerProc("eglGetDisplay");
    if (next) {
        return next(type);
    }
    return EGL_NO_DISPLAY;
}
static bool EglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
    WLOGE("-%{public}s-", MY_LOG_TAG);
    PFNEGLSWAPBUFFERSPROC next = (PFNEGLSWAPBUFFERSPROC)GetNextLayerProc("eglSwapBuffers");
    if (next) {
        return next(dpy, surface);
    }
    return EGL_NO_DISPLAY;
}

__eglMustCastToProperFunctionPointerType DebugLayerInitialize(const void *funcTbl, GetNextLayerAddr getAddr)
{
    WLOGD("-%{public}s-", MY_LOG_TAG);
    gFuncTbl = const_cast<void *>(funcTbl);
    gGetNextLayerAddr = getAddr;
    return nullptr;
}

__eglMustCastToProperFunctionPointerType DebugLayerGetProcAddr(const char *name,
    __eglMustCastToProperFunctionPointerType next)
{
    WLOGD("-%{public}s- ===name[%{public}s]===", MY_LOG_TAG, name);
    std::string func(name);
    if (func == "eglGetDisplay") {
        WLOGD("-%{public}s-", MY_LOG_TAG);
        return (__eglMustCastToProperFunctionPointerType)EglGetDisplay;
    }
    if (func == "eglSwapBuffers") {
        WLOGD("-%{public}s-", MY_LOG_TAG);
        return (__eglMustCastToProperFunctionPointerType)EglSwapBuffers;
    }

    return next;
}
