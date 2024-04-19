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

#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "ec_log.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class EglManager {
public:
    static constexpr int EGL_SUPPORT_VERSION = 3;
    static constexpr int EGL_LIMIT_VERSION = 4;
    static EglManager& GetInstance()
    {
        static EglManager manager;
        return manager;
    }

    ~EglManager() {}
    EGLConfig GetConfig(int version, EGLDisplay eglDisplay);
    void Init();

private:
    EglManager() : EGLDisplay_(EGL_NO_DISPLAY), EGLConfig_(nullptr), EGLContext_(EGL_NO_CONTEXT),
        currentSurface_(nullptr) {}
    EglManager(const EglManager&) = delete;
    EglManager& operator=(const EglManager&) = delete;
    EGLDisplay EGLDisplay_;
    EGLConfig EGLConfig_;
    EGLContext EGLContext_;
    EGLSurface currentSurface_;
    NativeWindow *EGLWindow_ = nullptr;
    bool initialized_ = false;
    int EGLWidth_ = 0;
    int EGLHeight_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // EGL_MANAGER_H
