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

#ifndef ROSENRENDER_ROSEN_WEBGL_EGL_MANAGER
#define ROSENRENDER_ROSEN_WEBGL_EGL_MANAGER

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
        mCurrentSurface = eglSurface;
    }

    void SetPbufferAttributes(int eglWidth, int eglHeight)
    {
        mEglWidth = eglWidth;
        mEglHeight = eglHeight;
    }

    EGLSurface GetCurrentSurface() const
    {
        return mCurrentSurface;
    }

    EGLDisplay GetEGLDisplay() const
    {
        return mEGLDisplay;
    }

    EGLContext GetEGLContext() const
    {
        return mEGLContext;
    }

    EGLConfig GetConfig(int version, EGLDisplay eglDisplay);

    void MakeCurrentIfNeeded(EGLSurface newEGLSurface);

    void Init();

    EGLSurface CreateSurface(NativeWindow* window);

private:
    EglManager() : mEGLDisplay(EGL_NO_DISPLAY), mEGLConfig(nullptr), mEGLContext(EGL_NO_CONTEXT),
        mCurrentSurface(nullptr) {}
    EglManager(const EglManager&) = delete;
    EglManager& operator=(const EglManager&) = delete;
    EGLDisplay mEGLDisplay;
    EGLConfig mEGLConfig;
    EGLContext mEGLContext;
    EGLSurface mCurrentSurface;
    NativeWindow *mEglWindow = nullptr;
    bool initialized = false;
    int mEglWidth = 0;
    int mEglHeight = 0;
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // ROSENRENDER_ROSEN_WEBGL_EGL_MANAGER
