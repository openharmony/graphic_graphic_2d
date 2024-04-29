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
#include <chrono>
#include <thread>
#include <unistd.h>
#include <gtest/gtest.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>

#include <surface.h>
#include <window.h>

#include "iconsumer_surface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;

class BufferProcTest : public IBufferConsumerListenerClazz {
    virtual void OnBufferAvailable() override;
};

void BufferProcTest::OnBufferAvailable()
{
}

class OpenglWrapperApiTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    static inline EGLConfig eglConfig_ = EGL_NO_CONFIG_KHR;
    static inline EGLContext eglContext_ = EGL_NO_CONTEXT;
    static inline EGLSurface eglSurface_ = EGL_NO_SURFACE;
    static inline sptr<OHOS::IConsumerSurface> cSurface_ = nullptr;
    static inline sptr<OHOS::Surface> pSurface_ = nullptr;
    static inline struct NativeWindow *gWindow_ = nullptr;
};

void OpenglWrapperApiTest::SetUpTestCase()
{
    eglDisplay_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
}

void OpenglWrapperApiTest::TearDownTestCase()
{
    eglDisplay_ = EGL_NO_DISPLAY;
}

/*
* Function: eglGetDisplay
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglGetDisplay
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglGetDisplay, TestSize.Level0)
{
    // normal
    EGLDisplay disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EXPECT_NE(disp, EGL_NO_DISPLAY);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);

    // abnormal
    disp = eglGetDisplay((EGLNativeDisplayType)1);
    EXPECT_EQ(disp, EGL_NO_DISPLAY);

    error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_PARAMETER);
}

/*
* Function: eglInitialize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglInitialize
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglInitialize, TestSize.Level0)
{
    EGLint major, minor;
    // abnormal
    EGLBoolean ret = eglInitialize(nullptr, &major, &minor);
    EXPECT_EQ(ret, EGL_FALSE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_DISPLAY);

    // normal
    ret = eglInitialize(eglDisplay_, &major, &minor);
    EXPECT_EQ(ret, EGL_TRUE);
    EXPECT_GE(major, 0);
    EXPECT_GE(minor, 0);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
}

/*
* Function: eglBindAPI
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglBindAPI
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglBindAPI, TestSize.Level0)
{
    // abnormal
    EGLBoolean ret = eglBindAPI(1);
    EXPECT_EQ(ret, EGL_FALSE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_PARAMETER);

    // normal
    ret = eglBindAPI(EGL_OPENGL_ES_API);
    EXPECT_EQ(ret, EGL_TRUE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
}

/*
* Function: eglChooseConfig
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglChooseConfig
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglChooseConfig, TestSize.Level0)
{
    EGLint count;
    EGLint configAttribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };
    // abnormal
    EGLBoolean ret = eglChooseConfig(nullptr, configAttribs, &eglConfig_, 1, &count);
    EXPECT_EQ(ret, EGL_FALSE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_DISPLAY);

    // normal
    ret = eglChooseConfig(eglDisplay_, configAttribs, &eglConfig_, 1, &count);
    EXPECT_EQ(ret, EGL_TRUE);
    EXPECT_GE(count, 1);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
}

/*
* Function: eglCreateContext
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglCreateContext
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglCreateContext, TestSize.Level0)
{
    const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION,
        EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };
    
    // abnormal
    eglContext_ = eglCreateContext(nullptr, eglConfig_, EGL_NO_CONTEXT, contextAttribs);
    EXPECT_EQ(eglContext_, EGL_NO_CONTEXT);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_DISPLAY);

    // normal
    eglContext_ = eglCreateContext(eglDisplay_, eglConfig_, EGL_NO_CONTEXT, contextAttribs);
    EXPECT_NE(eglContext_, EGL_NO_CONTEXT);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
}

/*
* Function: eglCreateWindowSurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglCreateWindowSurface
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglCreateWindowSurface, TestSize.Level0)
{
    static BufferProcTest test;
    cSurface_ = IConsumerSurface::Create("test");
    EXPECT_NE(cSurface_, nullptr);
    cSurface_->RegisterConsumerListener(&test);

    auto producer = cSurface_->GetProducer();
    EXPECT_NE(producer, nullptr);

    pSurface_ = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface_, nullptr);

    gWindow_ = CreateNativeWindowFromSurface(&pSurface_);
    EXPECT_NE(gWindow_, nullptr);

    int32_t height = 0x100;
    int32_t width = 0x100;
    cSurface_->SetDefaultWidthAndHeight(height, width);
    cSurface_->SetDefaultUsage(BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);

    NativeWindowHandleOpt(gWindow_, SET_USAGE, BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);
    NativeWindowHandleOpt(gWindow_, SET_BUFFER_GEOMETRY, height, width);
    NativeWindowHandleOpt(gWindow_, SET_COLOR_GAMUT, GRAPHIC_COLOR_GAMUT_SRGB);

    EGLint winAttribs[] = { EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE };
    eglSurface_ = eglCreateWindowSurface(nullptr, eglConfig_, gWindow_, winAttribs);
    EXPECT_EQ(eglSurface_, EGL_NO_SURFACE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_DISPLAY);

    eglSurface_ = eglCreateWindowSurface(eglDisplay_, eglConfig_, gWindow_, winAttribs);
    EXPECT_NE(eglSurface_, EGL_NO_SURFACE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
}

/*
* Function: eglMakeCurrent
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglMakeCurrent
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglMakeCurrent, TestSize.Level0)
{
    // abnormal
    EGLBoolean ret = eglMakeCurrent(nullptr, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    EXPECT_EQ(ret, EGL_FALSE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_DISPLAY);

    ret = eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, EGL_NO_CONTEXT);
    EXPECT_EQ(ret, EGL_FALSE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_MATCH);

    ret = eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, eglContext_);
    EXPECT_EQ(ret, EGL_TRUE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);

    ret = eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    EXPECT_EQ(ret, EGL_TRUE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);

    ret = eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, eglContext_);
    EXPECT_EQ(ret, EGL_TRUE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
}

/*
* Function: eglQuerySurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglQuerySurface
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglQuerySurface, TestSize.Level0)
{
    EGLint value;
    // abnormal
    EGLBoolean ret = eglQuerySurface(nullptr, eglSurface_, EGL_WIDTH, &value);
    EXPECT_EQ(ret, EGL_FALSE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_DISPLAY);

    ret = eglQuerySurface(eglDisplay_, nullptr, EGL_WIDTH, &value);
    EXPECT_EQ(ret, EGL_FALSE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_SURFACE);

    ret = eglQuerySurface(eglDisplay_, eglSurface_, EGL_WIDTH, &value);
    EXPECT_EQ(ret, EGL_TRUE);
    EXPECT_GE(value, 0);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
}

/*
* Function: glClearColor
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call glClearColor
 */
HWTEST_F(OpenglWrapperApiTest, glClearColor, TestSize.Level0)
{
    for (GLint error = glGetError(); error != GL_NO_ERROR; error = glGetError()) {
        continue;
    }

    glClearColor(0, 0, 0, 1.0);

    GLint error = glGetError();
    EXPECT_EQ(error, GL_NO_ERROR);
}

/*
* Function: glClear
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call glClear
 */
HWTEST_F(OpenglWrapperApiTest, glClear, TestSize.Level0)
{
    for (GLint error = glGetError(); error != GL_NO_ERROR; error = glGetError()) {
        continue;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    GLint error = glGetError();
    EXPECT_EQ(error, GL_NO_ERROR);
}

/*
* Function: eglDestroySurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglDestroySurface
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglDestroySurface, TestSize.Level0)
{
    // abnormal
    EGLBoolean ret = eglDestroySurface(nullptr, eglSurface_);
    EXPECT_EQ(ret, EGL_FALSE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_DISPLAY);

    ret = eglDestroySurface(eglDisplay_, nullptr);
    EXPECT_EQ(ret, EGL_FALSE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_SURFACE);

    ret = eglDestroySurface(eglDisplay_, eglSurface_);
    EXPECT_EQ(ret, EGL_TRUE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
    eglSurface_ = EGL_NO_SURFACE;
}

/*
* Function: eglDestroyContext
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglDestroyContext
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglDestroyContext, TestSize.Level0)
{
    // abnormal
    EGLBoolean ret = eglDestroyContext(nullptr, eglContext_);
    EXPECT_EQ(ret, EGL_FALSE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_DISPLAY);

    ret = eglDestroyContext(eglDisplay_, nullptr);
    EXPECT_EQ(ret, EGL_FALSE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_CONTEXT);

    ret = eglDestroyContext(eglDisplay_, eglContext_);
    EXPECT_EQ(ret, EGL_TRUE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
    eglContext_ = EGL_NO_CONTEXT;
}

/*
* Function: eglTerminate
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglTerminate
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglTerminate, TestSize.Level0)
{
    // abnormal
    EGLBoolean ret = eglTerminate(nullptr);
    EXPECT_EQ(ret, EGL_FALSE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_BAD_DISPLAY);

    ret = eglTerminate(eglDisplay_);
    EXPECT_EQ(ret, EGL_TRUE);

    error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
    eglDisplay_ = EGL_NO_DISPLAY;
}

/*
* Function: eglReleaseThread
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call eglReleaseThread
*                  2. check ret
 */
HWTEST_F(OpenglWrapperApiTest, eglReleaseThread, TestSize.Level0)
{
    // abnormal
    EGLBoolean ret = eglReleaseThread();
    EXPECT_EQ(ret, EGL_TRUE);

    EGLint error = eglGetError();
    EXPECT_EQ(error, EGL_SUCCESS);
}
}
