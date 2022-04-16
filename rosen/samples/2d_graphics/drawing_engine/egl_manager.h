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

#ifndef EGL_MANAGER_H
#define EGL_MANAGER_H

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
class EGLManager {
public:
    EGLManager() noexcept;
    virtual ~EGLManager();
    void Init();
    bool IsEglContextReady();
    void DestroySurface();
    void MakeCurrent();
    void SwapBuffers();
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height);
    EGLSurface CreateSurface(EGLNativeWindowType eglNativeWindow);
private:
    struct NativeWindow* nativeWindow_;
    EGLDisplay eglDisplay_;
    EGLContext eglContext_;
    EGLSurface eglSurface_;
    EGLConfig eglConfig_;
};
}
}
#endif