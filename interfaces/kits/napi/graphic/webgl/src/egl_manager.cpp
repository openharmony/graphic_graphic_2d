/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "util/egl_manager.h"

#include <cstddef>
#include "EGL/egl.h"
#include "EGL/eglplatform.h"
#include "__config"
#include "util/log.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
namespace {
    const int OPENGL_ES2_VERSION = 2;
    const int OPENGL_ES3_VERSION = 3;
}
using namespace std;

EGLConfig EglManager::GetConfig(int version, EGLDisplay eglDisplay)
{
    EGLint renderableType = EGL_OPENGL_ES2_BIT;
    if (version == OPENGL_ES3_VERSION) {
        renderableType = EGL_OPENGL_ES3_BIT;
    }
    int attribList[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT | EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, renderableType,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
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

void EglManager::MakeCurrentIfNeeded(EGLSurface newEGLSurface)
{
    if (currentSurface_ != newEGLSurface) {
        currentSurface_ = newEGLSurface;
        eglMakeCurrent(eglDisplay_, currentSurface_, currentSurface_, eglContext_);
    }
}

EGLSurface EglManager::CreateSurface(NativeWindow* window)
{
    EGLSurface eglSurface = nullptr;
    if (window) {
        LOGD("EglManager CreateSurface from eglWindow");
        eglSurface = eglCreateWindowSurface(eglDisplay_, eglConfig_, window, NULL);
    } else {
        LOGD("EglManager CreateSurface from PBuffer width = %{public}d, height = %{public}d",
            eglWidth_, eglHeight_);
        int surfaceAttributes[] = {
            EGL_WIDTH, eglWidth_,
            EGL_HEIGHT, eglHeight_,
            EGL_NONE
        };
        eglSurface = eglCreatePbufferSurface(eglDisplay_, eglConfig_, surfaceAttributes);
    }
    if (eglSurface == NULL) {
        LOGE("EglManager CreateSurface eglSurface = null error %{public}d", eglGetError());
    }
    return eglSurface;
}

void EglManager::Init()
{
    if (initialized_) {
        return;
    }
    initialized_ = true;
    LOGD("EglManag Init.");
    if (eglContext_ != nullptr) {
        LOGE("EglManager Init mEGLContext is already init.");
        return;
    }

    eglDisplay_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        LOGE("EglManager Init unable to get EGL display.");
        return;
    }

    EGLint eglMajVers, eglMinVers;
    if (!eglInitialize(eglDisplay_, &eglMajVers, &eglMinVers)) {
        eglDisplay_ = EGL_NO_DISPLAY;
        LOGE("EglManager Init unable to initialize display");
        return;
    }

    int version = OPENGL_ES3_VERSION;
    eglConfig_ = EglManager::GetConfig(version, eglDisplay_);
    if (eglConfig_ == NULL) {
        LOGE("EglManager Init config ERROR, try again");
        version = OPENGL_ES2_VERSION;
        eglConfig_ = EglManager::GetConfig(version, eglDisplay_);
        if (eglConfig_ == NULL) {
            LOGE("EglManager Init config ERROR again");
            return;
        }
    }

    if (eglWindow_) {
        LOGD("EglManager Init eglSurface from eglWindow");
        currentSurface_ = eglCreateWindowSurface(eglDisplay_, eglConfig_, eglWindow_, NULL);
        if (currentSurface_ == NULL) {
            LOGE("EglManager Init eglSurface = null");
            return;
        }
    } else {
        LOGD("EglManager Init eglSurface from PBuffer width = %{public}d, height = %{public}d",
            eglWidth_, eglHeight_);
        int surfaceAttributes[] = {
            EGL_WIDTH, eglWidth_,
            EGL_HEIGHT, eglHeight_,
            EGL_NONE
        };
        currentSurface_ = eglCreatePbufferSurface(eglDisplay_, eglConfig_, surfaceAttributes);
        if (currentSurface_ == NULL) {
            LOGE("EglManager Init eglCreateContext eglSurface = null");
            return;
        }
    }

    int attrib3List[] = {
        EGL_CONTEXT_CLIENT_VERSION, version,
        EGL_NONE
    };
    eglContext_ = eglCreateContext(eglDisplay_, eglConfig_, nullptr, attrib3List);
    int error = eglGetError();
    if (error == EGL_SUCCESS) {
        LOGD("EglManager Init Create mEGLContext ok");
    } else {
        LOGE("EglManager Init eglCreateContext error %{public}x", error);
    }
    eglMakeCurrent(eglDisplay_, currentSurface_, currentSurface_, eglContext_);
}
} // namespace Rosen
} // namespace OHOS
#ifdef __cplusplus
}
#endif
