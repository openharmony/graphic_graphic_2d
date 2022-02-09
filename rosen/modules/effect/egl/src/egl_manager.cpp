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
    int renderableType = EGL_OPENGL_ES2_BIT;
    if (version >= EGL_SUPPORT_VERSION) {
        renderableType = EGL_OPENGL_ES3_BIT_KHR;
    }
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

void EglManager::Init()
{
    if (initialized_) {
        return;
    }
    initialized_ = true;
    LOGI("EglManager ----- Init.\n");
    if (EGLContext_ != nullptr) {
        LOGE("EglManager Init EGLContext_ is already init.\n");
        return;
    }

    EGLDisplay_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (EGLDisplay_ == EGL_NO_DISPLAY) {
        LOGE("EglManager Init unable to get EGL display.\n");
        return;
    }

    EGLint eglMajVers, eglMinVers;
    if (!eglInitialize(EGLDisplay_, &eglMajVers, &eglMinVers)) {
        EGLDisplay_ = EGL_NO_DISPLAY;
        LOGE("EglManager Init unable to initialize display");
        return;
    }

    LOGI("EglManager Init eglMinVers = %{public}u", eglMinVers);
    int version = EGL_SUPPORT_VERSION;
    if (eglMinVers >= EGL_LIMIT_VERSION) {
        version = EGL_SUPPORT_VERSION;
    }

    EGLConfig_ = EglManager::GetConfig(version, EGLDisplay_);
    if (EGLConfig_ == NULL) {
        LOGE("EglManager Init config ERROR");
        return;
    }

    if (EGLWindow_ != nullptr) {
        LOGI("EglManager Init eglSurface from eglWindow");
        currentSurface_ = eglCreateWindowSurface(EGLDisplay_, EGLConfig_, EGLWindow_, NULL);
        if (currentSurface_ == NULL) {
            LOGE("EglManager Init eglSurface = null");
            return;
        }
    } else {
        LOGI("EglManager Init eglSurface from PBuffer width = %{public}d, height = %{public}d",
            EGLWidth_, EGLHeight_);
        int surfaceAttributes[] = {
            EGL_WIDTH, EGLWidth_,
            EGL_HEIGHT, EGLHeight_,
            EGL_NONE
        };
        currentSurface_ = eglCreatePbufferSurface(EGLDisplay_, EGLConfig_, surfaceAttributes);
        if (currentSurface_ == NULL) {
            LOGE("EglManager Init eglCreateContext eglSurface = null");
            return;
        }
    }

    int attrib3List[] = {
        EGL_CONTEXT_CLIENT_VERSION, version,
        EGL_NONE
    };
    EGLContext_ = eglCreateContext(EGLDisplay_, EGLConfig_, nullptr, attrib3List);
    int error = eglGetError();
    if (error == EGL_SUCCESS) {
        LOGI("EglManager Init Create EGLContext_ ok");
    } else {
        LOGE("EglManager Init eglCreateContext error %x", error);
    }
    eglMakeCurrent(EGLDisplay_, currentSurface_, currentSurface_, EGLContext_);
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
