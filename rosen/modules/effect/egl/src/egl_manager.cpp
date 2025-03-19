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

#include "egl_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;

EGLConfig EglManager::GetConfig(int version, EGLDisplay eglDisplay)
{
    int attribList[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    EGLConfig configs = NULL;
    int numConfigs;
    if (!eglChooseConfig(eglDisplay, attribList, &configs, 1, &numConfigs)) {
        LOGE("eglChooseConfig ERROR");
        return NULL;
    }
    return configs;
}

bool EglManager::RetryEGLContext()
{
    if (!IsEGLContextInCurrentThread(EGLDisplay_, EGLContext_)) {
        LOGW("retry eglMakeCurrent.");
        eglMakeCurrent(EGLDisplay_, currentSurface_, currentSurface_, EGLContext_);
        int error = eglGetError();
        if (error != EGL_SUCCESS) {
            Deinit();
            return false;
        }
        return true;
    }
    return true;
}

bool EglManager::InitializeEGLDisplay()
{
    EGLDisplay_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (EGLDisplay_ == EGL_NO_DISPLAY) {
        LOGE("EglManager Init unable to get EGL display.");
        return false;
    }

    EGLint eglMajVers;
    EGLint eglMinVers;
    if (!eglInitialize(EGLDisplay_, &eglMajVers, &eglMinVers)) {
        EGLDisplay_ = EGL_NO_DISPLAY;
        LOGE("EglManager Init unable to initialize display.");
        return false;
    }

    LOGI("EglManager Init eglMinVers = %{public}u", eglMinVers);
    int version = EGL_SUPPORT_VERSION;
    if (eglMinVers >= EGL_LIMIT_VERSION) {
        version = EGL_SUPPORT_VERSION;
    }

    EGLConfig_ = EglManager::GetConfig(version, EGLDisplay_);
    if (EGLConfig_ == NULL) {
        LOGE("EglManager Init config ERROR");
        return false;
    }

    return true;
}

bool EglManager::CreateEGLSurface()
{
#ifndef ANDROID_PLATFORM
    if (EGLWindow_ != nullptr) {
        LOGI("EglManager Init eglSurface from eglWindow");
        currentSurface_ = eglCreateWindowSurface(EGLDisplay_, EGLConfig_, EGLWindow_, NULL);
        if (currentSurface_ == NULL) {
            LOGE("EglManager Init eglSurface = null");
            return false;
        }
    } else {
        LOGI("EglManager Init eglSurface from PBuffer width = %{public}d, height = %{public}d", EGLWidth_, EGLHeight_);
        int surfaceAttributes[] = { EGL_WIDTH, EGLWidth_, EGL_HEIGHT, EGLHeight_, EGL_NONE };
        currentSurface_ = eglCreatePbufferSurface(EGLDisplay_, EGLConfig_, surfaceAttributes);
        if (currentSurface_ == NULL) {
            LOGE("EglManager Init eglCreateContext eglSurface = null");
            return false;
        }
    }
#else
    LOGI("EglManager Init eglSurface from PBuffer width = %{public}d, height = %{public}d", EGLWidth_, EGLHeight_);
    int surfaceAttributes[] = { EGL_WIDTH, EGLWidth_, EGL_HEIGHT, EGLHeight_, EGL_NONE };
    currentSurface_ = eglCreatePbufferSurface(EGLDisplay_, EGLConfig_, surfaceAttributes);
    if (currentSurface_ == NULL) {
        LOGE("EglManager Init eglCreateContext eglSurface = null");
        return false;
    }
#endif
    return true;
}

bool EglManager::CreateAndSetEGLContext()
{
    int attrib3List[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_SUPPORT_VERSION, EGL_NONE };
    EGLContext_ = eglCreateContext(EGLDisplay_, EGLConfig_, nullptr, attrib3List);
    int error = eglGetError();
    if (error == EGL_SUCCESS) {
        LOGI("EglManager Init Create EGLContext_ ok");
    } else {
        LOGE("EglManager Init eglCreateContext error %x", error);
        return false;
    }
    eglMakeCurrent(EGLDisplay_, currentSurface_, currentSurface_, EGLContext_);
    error = eglGetError();
    return error == EGL_SUCCESS;
}

EGLBoolean EglManager::Init()
{
    if (initialized_) {
        if (RetryEGLContext()) {
            return EGL_TRUE;
        }
    }

    initialized_ = true;
    LOGI("EglManager ----- Init.\n");

    if (EGLContext_ != EGL_NO_CONTEXT && EGLContext_ != nullptr) {
        LOGW("EglManager Init EGLContext_ is already init.\n");
    }

    if (!InitializeEGLDisplay()) {
        return EGL_FALSE;
    }

    if (!CreateEGLSurface()) {
        return EGL_FALSE;
    }

    if (!CreateAndSetEGLContext()) {
        return EGL_FALSE;
    }
    return EGL_TRUE;
}

void EglManager::Deinit()
{
    LOGW("Deinit");

    if (EGLDisplay_ == EGL_NO_DISPLAY) {
        return;
    }
 
    eglDestroyContext(EGLDisplay_, EGLContext_);
    if (currentSurface_ != EGL_NO_SURFACE) {
        eglDestroySurface(EGLDisplay_, currentSurface_);
    }
    eglMakeCurrent(EGLDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(EGLDisplay_);
    eglReleaseThread();
 
    EGLDisplay_ = EGL_NO_DISPLAY;
    EGLContext_ = EGL_NO_CONTEXT;
    currentSurface_ = EGL_NO_SURFACE;
    initialized_ = false;
}

EGLBoolean EglManager::IsEGLContextInCurrentThread(EGLDisplay display, EGLContext context)
{
    if (display == EGL_NO_DISPLAY) {
        LOGE("EglManager IsEGLContextInCurrentThread display is EGL_NO_DISPLAY");
        return EGL_FALSE;
    }

    if (context == EGL_NO_CONTEXT) {
        LOGE("EglManager IsEGLContextInCurrentThread context is EGL_NO_CONTEXT");
        return EGL_FALSE;
    }

    EGLBoolean isContextInCurrent = EGL_FALSE;
    EGLint isContextLost = 0;

    eglQueryContext(display, context, EGL_CONTEXT_LOST, &isContextLost);

    if (!isContextLost && eglGetCurrentContext() == context) {
        isContextInCurrent = EGL_TRUE;
    }

    return isContextInCurrent;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
