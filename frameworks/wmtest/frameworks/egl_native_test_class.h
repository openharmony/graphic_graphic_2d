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

#ifndef FRAMEWORKS_WMTEST_FRAMEWORKS_EGL_NATIVE_TEST_CLASS
#define FRAMEWORKS_WMTEST_FRAMEWORKS_EGL_NATIVE_TEST_CLASS

#include <cstdint>
#include <functional>
#include <optional>

#include <refbase.h>
#include <window_manager.h>

#include <egl_surface.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace OHOS {
typedef struct {
    GLuint program;
    GLuint pos;
    GLuint color;
    GLuint offsetUniform;
} GLContext;

using EGLDrawFunc = std::function<void(GLContext *,
    sptr<EglSurface> &psurface, uint32_t width, uint32_t height)>;

class EGLNativeTestSync : public RefBase {
public:
    static sptr<EGLNativeTestSync> CreateSync(EGLDrawFunc drawFunc,
        sptr<EglSurface> &psurface, uint32_t width, uint32_t height, void *data = nullptr);

private:
    void Sync(int64_t, void *);
    bool GLContextInit();

    sptr<EglSurface> eglsurface = nullptr;
    EGLDrawFunc draw = nullptr;
    GLContext glCtx;
    bool bInit = false;
    SurfaceError sret = SURFACE_ERROR_OK;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t count = 0;
};

class EGLNativeTestDraw {
public:
    static void FlushDraw(GLContext *ctx, sptr<EglSurface> &eglsurface, uint32_t width, uint32_t height);
};
} // namespace OHOS

#endif // FRAMEWORKS_WMTEST_FRAMEWORKS_EGL_NATIVE_TEST_CLASS
