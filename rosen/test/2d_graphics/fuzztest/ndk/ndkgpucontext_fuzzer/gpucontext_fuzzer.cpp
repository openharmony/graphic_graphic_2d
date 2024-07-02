/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gpucontext_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"

#include "drawing_gpu_context.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void GpuContextFuzzTest000(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    EGLDisplay eglDisplay = EGL_NO_DISPLAY;
    EGLConfig eglConfig = EGL_NO_CONFIG_KHR;
    EGLContext eglContext = EGL_NO_CONTEXT;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    OH_Drawing_GpuContext* gpuContext = nullptr;

    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint eglMajVers;
    EGLint eglMinVers;
    EGLBoolean ret = eglInitialize(eglDisplay, &eglMajVers, &eglMinVers);

    EGLint count;
    EGLint configAttribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };
    ret = eglChooseConfig(eglDisplay, configAttribs, &eglConfig, 1, &count);

    const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs);

    EGLint attribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
    eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, attribs);

    ret = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    OH_Drawing_GpuContextOptions options;
    options.allowPathMaskCaching = GetObject<bool>();
    gpuContext = OH_Drawing_GpuContextCreateFromGL(options);
    OH_Drawing_GpuContextDestroy(gpuContext);

    ret = eglDestroySurface(eglDisplay, eglSurface);
    ret = eglDestroyContext(eglDisplay, eglContext);
    ret = eglTerminate(eglDisplay);

    eglSurface = EGL_NO_SURFACE;
    eglContext = EGL_NO_CONTEXT;
    eglDisplay = EGL_NO_DISPLAY;
    return;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::GpuContextFuzzTest000(data, size);
    return 0;
}