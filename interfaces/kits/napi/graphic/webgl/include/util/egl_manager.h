/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef WEBGL_EGL_MANAGER_H
#define WEBGL_EGL_MANAGER_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class EglManager {
public:
    static EglManager& GetInstance()
    {
        static EglManager manager;
        return manager;
    }

    ~EglManager() {}

    void SetCurrentSurface(EGLSurface eglSurface)
    {
        currentSurface_ = eglSurface;
    }

    void SetPbufferAttributes(int eglWidth, int eglHeight)
    {
        eglWidth_ = eglWidth;
        eglHeight_ = eglHeight;
    }

    EGLSurface GetCurrentSurface() const
    {
        return currentSurface_;
    }

    EGLDisplay GetEGLDisplay() const
    {
        return eglDisplay_;
    }

    EGLContext GetEGLContext() const
    {
        return eglContext_;
    }

    EGLConfig GetConfig(int version, EGLDisplay eglDisplay);

    void MakeCurrentIfNeeded(EGLSurface newEGLSurface);

    void Init();

    EGLSurface CreateSurface(NativeWindow* window);

private:
    EglManager() : eglDisplay_(EGL_NO_DISPLAY), eglConfig_(nullptr), eglContext_(EGL_NO_CONTEXT),
        currentSurface_(nullptr) {}
    EglManager(const EglManager&) = delete;
    EglManager& operator=(const EglManager&) = delete;
    EGLDisplay eglDisplay_;
    EGLConfig eglConfig_;
    EGLContext eglContext_;
    EGLSurface currentSurface_;
    NativeWindow *eglWindow_ = nullptr;
    bool initialized_ = false;
    int32_t eglWidth_ = 0;
    int32_t eglHeight_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#ifdef __cplusplus
}
#endif
#endif // WEBGL_EGL_MANAGER_H
