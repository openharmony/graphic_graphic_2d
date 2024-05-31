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

#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_SURFACE_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_SURFACE_H

#include <EGL/egl.h>
#include "egl_wrapper_object.h"
namespace OHOS {
class EglWrapperDisplay;

class EglWrapperSurface : public EglWrapperObject {
public:
    EglWrapperSurface(EglWrapperDisplay *disp, EGLSurface surf, NativeWindowType window = nullptr);
    static EglWrapperSurface *GetWrapperSurface(EGLSurface surf);
    inline EGLSurface GetEglSurface() const
    {
        return surf_;
    };

    inline NativeWindowType GetNativeWindow() const
    {
        return window_;
    };

protected:
    ~EglWrapperSurface() override;
private:
    EGLSurface surf_;
    NativeWindowType window_;
};
} // namespace OHOS
#endif // FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_SURFACE_H
