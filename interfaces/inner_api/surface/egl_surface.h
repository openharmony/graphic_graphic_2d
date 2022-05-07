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

#ifndef INTERFACES_INNERKITS_SURFACE_EGL_RENDER_SURFACE_H
#define INTERFACES_INNERKITS_SURFACE_EGL_RENDER_SURFACE_H

#include <refbase.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "ibuffer_producer.h"
#include "surface_type.h"

namespace OHOS {
class EglSurface : public RefBase {
public:
    static sptr<EglSurface> CreateEglSurfaceAsProducer(sptr<IBufferProducer>& producer);
    virtual ~EglSurface() = default;

    virtual GSError InitContext(EGLContext context = EGL_NO_CONTEXT) = 0;
    virtual EGLDisplay GetEglDisplay() const = 0;
    virtual EGLContext GetEglContext() const = 0;
    virtual EGLSurface GetEglSurface() const = 0;
    virtual GLuint GetEglFbo() const = 0;
    virtual GSError SwapBuffers() = 0;
    virtual GSError SetWidthAndHeight(int32_t width, int32_t height) = 0;

protected:
    EglSurface() = default;
};
typedef EglSurface EglRenderSurface;
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_EGL_RENDER_SURFACE_H
