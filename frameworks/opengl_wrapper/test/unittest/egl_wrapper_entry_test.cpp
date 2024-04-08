/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <string>

#include "EGL/egl_wrapper_entry.h"

#include "EGL/egl_wrapper_display.h"
#include "egl_defs.h"
#include "hook.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class EglWrapperEntryTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: FindEglWrapperApi001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, FindEglWrapperApi001, Level1)
{
    auto result = FindEglWrapperApi("eglChooseConfig");
    ASSERT_NE(nullptr, result);
}

/**
 * @tc.name: FindEglWrapperApi002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, FindEglWrapperApi002, Level2)
{
    auto result = FindEglWrapperApi("");
    ASSERT_EQ(nullptr, result);
}

/**
 * @tc.name: CheckEglWrapperApi001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, CheckEglWrapperApi001, Level1)
{
    auto result = CheckEglWrapperApi("eglChooseConfig");
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CheckEglWrapperApi002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, CheckEglWrapperApi002, Level2)
{
    auto result = CheckEglWrapperApi("");
    ASSERT_TRUE(!result);
}

/**
 * @tc.name: ValidateDisplay001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, ValidateDisplay001, Level1)
{
    // EglTerminateImpl calls ValidateDisplay
    auto result = eglTerminate(nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: ValidateDisplay002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, ValidateDisplay002, Level2)
{
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    // EglTerminateImpl calls ValidateDisplay
    auto result = eglTerminate(dpy);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglChooseConfigImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglChooseConfigImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglChooseConfig(dpy, nullptr, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglChooseConfigImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglChooseConfigImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglChooseConfig(dpy, nullptr, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglChooseConfigImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglChooseConfigImpl003, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);
    EGLint numConfig = 1;

    auto result = gWrapperHook.wrapper.eglChooseConfig(dpy, nullptr, nullptr, 0, &numConfig);
    ASSERT_NE(EGL_FALSE, result);
}

/**
 * @tc.name: EglCopyBuffersImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCopyBuffersImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCopyBuffers(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglCopyBuffersImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCopyBuffersImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCopyBuffers(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglCreateContextImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateContextImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreateContext(dpy, nullptr, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_CONTEXT, result);
}

/**
 * @tc.name: EglCreateContextImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateContextImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateContext(dpy, nullptr, nullptr, nullptr);
    ASSERT_NE(EGL_NO_CONTEXT, result);
}

/**
 * @tc.name: EglCreatePbufferSurfaceImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreatePbufferSurfaceImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreatePbufferSurface(dpy, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_CONTEXT, result);
}

/**
 * @tc.name: EglCreatePbufferSurfaceImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreatePbufferSurfaceImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreatePbufferSurface(dpy, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_CONTEXT, result);
}

/**
 * @tc.name: EglCreatePixmapSurfaceImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreatePixmapSurfaceImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreatePixmapSurface(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_NO_CONTEXT, result);
}

/**
 * @tc.name: EglCreatePixmapSurfaceImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreatePixmapSurfaceImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreatePixmapSurface(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_NO_CONTEXT, result);
}

/**
 * @tc.name: EglCreateWindowSurfaceImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateWindowSurfaceImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreateWindowSurface(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: EglCreateWindowSurfaceImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateWindowSurfaceImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateWindowSurface(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: EglDestroyContextImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroyContextImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglDestroyContext(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglDestroyContextImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroyContextImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglDestroyContext(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglDestroySurfaceImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroySurfaceImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglDestroySurface(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglDestroySurfaceImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroySurfaceImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglDestroySurface(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetConfigAttribImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetConfigAttribImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglGetConfigAttrib(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetConfigAttribImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetConfigAttribImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglGetConfigAttrib(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetConfigsImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetConfigsImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglGetConfigs(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetConfigsImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetConfigsImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglGetConfigs(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetConfigsImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetConfigsImpl003, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);
    EGLint numConfig = 1;

    auto result = gWrapperHook.wrapper.eglGetConfigs(dpy, nullptr, 0, &numConfig);
    ASSERT_NE(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetPlatformDisplayInternal001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetPlatformDisplayInternal001, Level1)
{
    auto type = 10;
    auto result = gWrapperHook.wrapper.eglGetDisplay(&type);
    ASSERT_EQ(EGL_NO_DISPLAY, result);
}

/**
 * @tc.name: EglGetPlatformDisplayInternal002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetPlatformDisplayInternal002, Level2)
{
    auto result = gWrapperHook.wrapper.eglGetDisplay(EGL_DEFAULT_DISPLAY);
    ASSERT_NE(EGL_NO_DISPLAY, result);
}

/**
 * @tc.name: EglGetPlatformDisplayInternal003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetPlatformDisplayInternal003, Level2)
{
    auto result = gWrapperHook.wrapper.eglGetPlatformDisplay(0, nullptr, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetErrorImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetErrorImpl001, Level1)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = gWrapperHook.wrapper.eglGetError();
    ASSERT_NE(EGL_SUCCESS, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglGetErrorImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetErrorImpl002, Level2)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = true;

    auto result = gWrapperHook.wrapper.eglGetError();
    ASSERT_EQ(EGL_SUCCESS, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglGetProcAddressImpL001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetProcAddressImpL001, Level1)
{
    std::string procname = "eglCreateImageKHR";
    auto result = gWrapperHook.wrapper.eglGetProcAddress(procname.c_str());
    ASSERT_NE(nullptr, result);
}

/**
 * @tc.name: EglGetProcAddressImpL002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetProcAddressImpL002, Level2)
{
    std::string procname = "nullptr";
    auto result = gWrapperHook.wrapper.eglGetProcAddress(procname.c_str());
    ASSERT_EQ(nullptr, result);
}

/**
 * @tc.name: EglInitializeImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglInitializeImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglInitialize(dpy, nullptr, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglInitializeImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglInitializeImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    auto result = gWrapperHook.wrapper.eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_NE(EGL_FALSE, result);
}

/**
 * @tc.name: EglMakeCurrentImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglMakeCurrentImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglMakeCurrent(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglMakeCurrentImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglMakeCurrentImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglMakeCurrent(dpy, nullptr, 0, nullptr);
    ASSERT_NE(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryContextImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryContextImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglQueryContext(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryContextImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryContextImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglQueryContext(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryContextImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryContextImpl003, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);
    EGLint value = 1;

    auto result = gWrapperHook.wrapper.eglQueryContext(dpy, nullptr, 0, &value);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryStringImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStringImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglQueryString(dpy, 0);
    ASSERT_EQ(nullptr, result);
}

/**
 * @tc.name: EglQueryStringImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStringImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglQueryString(dpy, 0);
    ASSERT_EQ(nullptr, result);
}

/**
 * @tc.name: EglQuerySurfaceImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQuerySurfaceImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglQuerySurface(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQuerySurfaceImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQuerySurfaceImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglQuerySurface(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQuerySurfaceImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQuerySurfaceImpl003, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);
    EGLint value = 1;

    auto result = gWrapperHook.wrapper.eglQuerySurface(dpy, nullptr, 0, &value);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSwapBuffersImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSwapBuffersImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglSwapBuffers(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSwapBuffersImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSwapBuffersImpl002, Level1)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglSwapBuffers(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglTerminateImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglTerminateImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglTerminate(dpy);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglTerminateImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglTerminateImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglTerminate(dpy);
    ASSERT_NE(EGL_FALSE, result);
}

/**
 * @tc.name: EglWaitGLImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitGLImpl001, Level1)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;
    auto result = gWrapperHook.wrapper.eglWaitGL();
    ASSERT_EQ(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}


/**
 * @tc.name: EglWaitGLImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitGLImpl002, Level2)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = true;
    auto result = gWrapperHook.wrapper.eglWaitGL();
    ASSERT_NE(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}


/**
 * @tc.name: EglWaitNativeImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitNativeImpl001, Level1)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;
    auto result = gWrapperHook.wrapper.eglWaitNative(1);
    ASSERT_EQ(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglWaitNativeImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitNativeImpl002, Level2)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = true;
    auto result = gWrapperHook.wrapper.eglWaitNative(1);
    ASSERT_NE(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglBindTexImageImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglBindTexImageImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglBindTexImage(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglBindTexImageImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglBindTexImageImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglBindTexImage(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglReleaseTexImageImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglReleaseTexImageImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglReleaseTexImage(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglReleaseTexImageImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglReleaseTexImageImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglReleaseTexImage(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSurfaceAttribImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSurfaceAttribImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglSurfaceAttrib(dpy, nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSurfaceAttribImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSurfaceAttribImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglSurfaceAttrib(dpy, nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSwapIntervalImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSwapIntervalImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglSwapInterval(dpy, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSwapIntervalImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSwapIntervalImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglSwapInterval(dpy, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglBindAPIImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglBindAPIImpl001, Level1)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;
    auto result = gWrapperHook.wrapper.eglBindAPI(1);
    ASSERT_EQ(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglBindAPIImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglBindAPIImpl002, Level2)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = true;
    auto result = gWrapperHook.wrapper.eglBindAPI(1);
    ASSERT_EQ(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglQueryAPIImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryAPIImpl001, Level1)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;
    auto result = gWrapperHook.wrapper.eglQueryAPI();
    ASSERT_EQ(EGL_OPENGL_ES_API, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglQueryAPIImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryAPIImpl002, Level2)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = true;
    auto result = gWrapperHook.wrapper.eglQueryAPI();
    ASSERT_EQ(EGL_OPENGL_ES_API, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglCreatePbufferFromClientBufferImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreatePbufferFromClientBufferImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreatePbufferFromClientBuffer(dpy, 0, nullptr, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: EglReleaseThreadImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglReleaseThreadImpl001, Level1)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;
    auto result = gWrapperHook.wrapper.eglReleaseThread();
    ASSERT_EQ(EGL_TRUE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglReleaseThreadImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglReleaseThreadImpl002, Level2)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = true;
    auto result = gWrapperHook.wrapper.eglReleaseThread();
    ASSERT_EQ(EGL_TRUE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglWaitClientImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitClientImpl001, Level1)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;
    auto result = gWrapperHook.wrapper.eglWaitClient();
    ASSERT_EQ(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglWaitClientImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitClientImpl002, Level2)
{
    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = true;
    auto result = gWrapperHook.wrapper.eglWaitClient();
    ASSERT_NE(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: EglCreateSyncImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateSyncImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreateSync(dpy, 0, nullptr);
    ASSERT_EQ(EGL_NO_SYNC, result);
}

/**
 * @tc.name: EglCreateSyncImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateSyncImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateSync(dpy, 0, nullptr);
    ASSERT_EQ(EGL_NO_SYNC, result);
}

/**
 * @tc.name: EglDestroySyncImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroySyncImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglDestroySync(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglDestroySyncImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroySyncImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglDestroySync(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglClientWaitSyncImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglClientWaitSyncImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglClientWaitSync(dpy, nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglClientWaitSyncImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglClientWaitSyncImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglClientWaitSync(dpy, nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetSyncAttribImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetSyncAttribImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglGetSyncAttrib(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetSyncAttribImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetSyncAttribImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglGetSyncAttrib(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetSyncAttribImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetSyncAttribImpl003, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);
    EGLAttrib value = 1;

    auto result = gWrapperHook.wrapper.eglGetSyncAttrib(dpy, nullptr, 0, &value);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglDupNativeFenceFDANDROIDImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDupNativeFenceFDANDROIDImpl001, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateSync(dpy, 0, nullptr);
    ASSERT_EQ(EGL_NO_SYNC, result);

    auto result1 = gWrapperHook.wrapper.eglDupNativeFenceFDANDROID(dpy, result);
    ASSERT_EQ(EGL_NO_NATIVE_FENCE_FD_ANDROID, result1);
}

/**
 * @tc.name: EglCreateImageImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateImageImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreateImage(dpy, nullptr, 0, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_IMAGE, result);
}

/**
 * @tc.name: EglCreateImageImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateImageImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateImage(dpy, nullptr, 0, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_IMAGE, result);
}

/**
 * @tc.name: EglDestroyImageImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroyImageImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglDestroyImage(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglDestroyImageImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroyImageImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglDestroyImage(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglCreatePlatformWindowSurfaceImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreatePlatformWindowSurfaceImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreatePlatformWindowSurface(dpy, nullptr, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: EglCreatePlatformWindowSurfaceImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreatePlatformWindowSurfaceImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreatePlatformWindowSurface(dpy, nullptr, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: EglCreatePlatformPixmapSurfaceImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreatePlatformPixmapSurfaceImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreatePlatformPixmapSurface(dpy, nullptr, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: EglCreatePlatformPixmapSurfaceImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreatePlatformPixmapSurfaceImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreatePlatformPixmapSurface(dpy, nullptr, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: EglWaitSyncImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitSyncImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglWaitSync(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglWaitSyncImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitSyncImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglWaitSync(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglLockSurfaceKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglLockSurfaceKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglLockSurfaceKHR(dpy, nullptr, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglLockSurfaceKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglLockSurfaceKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglLockSurfaceKHR(dpy, nullptr, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglUnlockSurfaceKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglUnlockSurfaceKHRImpl001, Level1)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglUnlockSurfaceKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglUnlockSurfaceKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglUnlockSurfaceKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglUnlockSurfaceKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglCreateImageKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateImageKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreateImageKHR(dpy, 0, 0, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_IMAGE_KHR, result);
}

/**
 * @tc.name: EglCreateImageKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateImageKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateImageKHR(dpy, 0, 0, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_IMAGE_KHR, result);
}

/**
 * @tc.name: EglDestroyImageKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroyImageKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglDestroyImageKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglDestroyImageKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroyImageKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglDestroyImageKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglCreateSyncKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateSyncKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreateSyncKHR(dpy, 0, nullptr);
    ASSERT_EQ(EGL_NO_SYNC_KHR, result);
}

/**
 * @tc.name: EglCreateSyncKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateSyncKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateSyncKHR(dpy, 0, nullptr);
    ASSERT_EQ(EGL_NO_SYNC_KHR, result);
}

/**
 * @tc.name: EglDestroySyncKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroySyncKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglDestroySyncKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglDestroySyncKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroySyncKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglDestroySyncKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglClientWaitSyncKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglClientWaitSyncKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglClientWaitSyncKHR(dpy, nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglClientWaitSyncKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglClientWaitSyncKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglClientWaitSyncKHR(dpy, nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetSyncAttribKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetSyncAttribKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglGetSyncAttribKHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetSyncAttribKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetSyncAttribKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglGetSyncAttribKHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetSyncAttribKHRImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetSyncAttribKHRImpl003, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);
    EGLint value = 1;

    auto result = gWrapperHook.wrapper.eglGetSyncAttribKHR(dpy, nullptr, 0, &value);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSignalSyncKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSignalSyncKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglSignalSyncKHR(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSignalSyncKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSignalSyncKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglSignalSyncKHR(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglCreateStreamKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateStreamKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreateStreamKHR(dpy, nullptr);
    ASSERT_EQ(EGL_NO_STREAM_KHR, result);
}

/**
 * @tc.name: EglCreateStreamKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateStreamKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateStreamKHR(dpy, nullptr);
    ASSERT_EQ(EGL_NO_STREAM_KHR, result);
}


/**
 * @tc.name: EglDestroyStreamKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroyStreamKHRImpl001, Level1)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglDestroyStreamKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglDestroyStreamKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglDestroyStreamKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglDestroyStreamKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglStreamAttribKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglStreamAttribKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglStreamAttribKHR(dpy, nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglStreamAttribKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglStreamAttribKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglStreamAttribKHR(dpy, nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryStreamKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStreamKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglQueryStreamKHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryStreamKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStreamKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglQueryStreamKHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryStreamKHRImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStreamKHRImpl003, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);
    EGLint value = 1;

    auto result = gWrapperHook.wrapper.eglQueryStreamKHR(dpy, nullptr, 0, &value);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryStreamu64KHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStreamu64KHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglQueryStreamu64KHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryStreamu64KHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStreamu64KHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglQueryStreamu64KHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryStreamu64KHRImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStreamu64KHRImpl003, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);
    EGLuint64KHR value = 1;

    auto result = gWrapperHook.wrapper.eglQueryStreamu64KHR(dpy, nullptr, 0, &value);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglStreamConsumerGLTextureExternalKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglStreamConsumerGLTextureExternalKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglStreamConsumerGLTextureExternalKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglStreamConsumerGLTextureExternalKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglStreamConsumerGLTextureExternalKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglStreamConsumerGLTextureExternalKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglStreamConsumerAcquireKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglStreamConsumerAcquireKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglStreamConsumerAcquireKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglStreamConsumerAcquireKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglStreamConsumerAcquireKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglStreamConsumerAcquireKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglStreamConsumerReleaseKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglStreamConsumerReleaseKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglStreamConsumerReleaseKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglStreamConsumerReleaseKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglStreamConsumerReleaseKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglStreamConsumerReleaseKHR(dpy, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglCreateStreamProducerSurfaceKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateStreamProducerSurfaceKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreateStreamProducerSurfaceKHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: EglCreateStreamProducerSurfaceKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateStreamProducerSurfaceKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateStreamProducerSurfaceKHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: EglQueryStreamTimeKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStreamTimeKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglQueryStreamTimeKHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryStreamTimeKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStreamTimeKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglQueryStreamTimeKHR(dpy, nullptr, 0, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglQueryStreamTimeKHRImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglQueryStreamTimeKHRImpl003, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);
    EGLTimeKHR value = 1;

    auto result = gWrapperHook.wrapper.eglQueryStreamTimeKHR(dpy, nullptr, 0, &value);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetStreamFileDescriptorKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetStreamFileDescriptorKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglGetStreamFileDescriptorKHR(dpy, nullptr);
    ASSERT_EQ(EGL_NO_FILE_DESCRIPTOR_KHR, result);
}

/**
 * @tc.name: EglGetStreamFileDescriptorKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetStreamFileDescriptorKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglGetStreamFileDescriptorKHR(dpy, nullptr);
    ASSERT_EQ(EGL_NO_FILE_DESCRIPTOR_KHR, result);
}


/**
 * @tc.name: EglCreateStreamFromFileDescriptorKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateStreamFromFileDescriptorKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglCreateStreamFromFileDescriptorKHR(dpy, EGL_NO_FILE_DESCRIPTOR_KHR);
    ASSERT_EQ(EGL_NO_STREAM_KHR, result);
}

/**
 * @tc.name: EglCreateStreamFromFileDescriptorKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglCreateStreamFromFileDescriptorKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglCreateStreamFromFileDescriptorKHR(dpy, EGL_NO_FILE_DESCRIPTOR_KHR);
    ASSERT_EQ(EGL_NO_STREAM_KHR, result);
}

/**
 * @tc.name: EglWaitSyncKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitSyncKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglWaitSyncKHR(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglWaitSyncKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglWaitSyncKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglWaitSyncKHR(dpy, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglGetPlatformDisplayEXTImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetPlatformDisplayEXTImpl001, Level1)
{
    auto result = gWrapperHook.wrapper.eglGetPlatformDisplayEXT(0, EGLNativeDisplayType(1), nullptr);
    ASSERT_EQ(EGL_NO_DISPLAY, result);
}

/**
 * @tc.name: EglGetPlatformDisplayEXTImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetPlatformDisplayEXTImpl002, Level2)
{
    auto result = gWrapperHook.wrapper.eglGetPlatformDisplayEXT(0, EGL_DEFAULT_DISPLAY, nullptr);
    ASSERT_EQ(EGL_NO_DISPLAY, result);
}

/**
 * @tc.name: EglGetPlatformDisplayEXTImpl003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglGetPlatformDisplayEXTImpl003, Level2)
{
    auto result = gWrapperHook.wrapper.eglGetPlatformDisplayEXT(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, 0);
    ASSERT_NE(EGL_NO_DISPLAY, result);
}

/**
 * @tc.name: EglSwapBuffersWithDamageKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSwapBuffersWithDamageKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglSwapBuffersWithDamageKHR(dpy, nullptr, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSwapBuffersWithDamageKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSwapBuffersWithDamageKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglSwapBuffersWithDamageKHR(dpy, nullptr, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSetDamageRegionKHRImpl001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSetDamageRegionKHRImpl001, Level1)
{
    EGLDisplay dpy = nullptr;
    auto result = gWrapperHook.wrapper.eglSetDamageRegionKHR(dpy, nullptr, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: EglSetDamageRegionKHRImpl002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperEntryTest, EglSetDamageRegionKHRImpl002, Level2)
{
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean ret = eglInitialize(dpy, &majorVersion, &minorVersion);
    ASSERT_EQ(ret, EGL_TRUE);

    auto result = gWrapperHook.wrapper.eglSetDamageRegionKHR(dpy, nullptr, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}
} // OHOS::Rosen