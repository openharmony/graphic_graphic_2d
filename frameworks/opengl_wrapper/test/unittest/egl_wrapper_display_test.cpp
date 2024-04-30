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

#include "EGL/egl_wrapper_display.h"

#include "egl_defs.h"
#include "thread_private_data_ctl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class EglWrapperDisplayTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: GetWrapperDisplay001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, GetWrapperDisplay001, Level1)
{
    auto result = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);
    ASSERT_NE(nullptr, result);
}

/**
 * @tc.name: GetWrapperDisplay002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, GetWrapperDisplay002, Level2)
{
    auto result = EglWrapperDisplay::GetWrapperDisplay(nullptr);
    ASSERT_EQ(nullptr, result);
}

/**
 * @tc.name: MakeCurrent001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, MakeCurrent001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);
    EGLSurface draw = EGL_NO_SURFACE;
    EGLSurface read = EGL_NO_SURFACE;
    EGLContext ctx = EGL_NO_CONTEXT;

    auto result = eglWrapperDisplay->MakeCurrent(draw, read, ctx);
    ASSERT_EQ(EGL_TRUE, result);
}

/**
 * @tc.name: CreateEglContext001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreateEglContext001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->CreateEglContext(nullptr, nullptr, 0);
    ASSERT_EQ(EGL_NO_CONTEXT, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: CreateEglContext002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreateEglContext002, Level2)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->CreateEglContext(nullptr, EGL_NO_CONTEXT, 0);
    ASSERT_EQ(EGL_NO_CONTEXT, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: CreateEglSurface001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreateEglSurface001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->CreateEglSurface(nullptr, 0, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: CreateEglSurface002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreateEglSurface002, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    NativeWindowType window = nullptr;
    auto result = eglWrapperDisplay->CreateEglSurface(nullptr, window, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: CreatePixmapSurface001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreatePixmapSurface001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->CreatePixmapSurface(nullptr, 0, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: CreatePixmapSurface002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreatePixmapSurface002, Level2)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    EGLNativePixmapType window = nullptr;
    auto result = eglWrapperDisplay->CreatePixmapSurface(nullptr, window, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: CreatePbufferFromClientBuffer001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreatePbufferFromClientBuffer001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->CreatePbufferFromClientBuffer(0, nullptr, nullptr, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: CreateImage001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreateImage001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->CreateImage(EGL_NO_CONTEXT, 0, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_IMAGE_KHR, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: CreatePlatformWindowSurface001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreatePlatformWindowSurface001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->CreatePlatformWindowSurface(nullptr, nullptr, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: CreatePlatformWindowSurface002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreatePlatformWindowSurface002, Level2)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->CreatePlatformWindowSurface(nullptr, &gWrapperHook, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: CreatePlatformPixmapSurface001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreatePlatformPixmapSurface001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->CreatePlatformPixmapSurface(nullptr, nullptr, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}

/**
 * @tc.name: CreatePlatformPixmapSurface002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreatePlatformPixmapSurface002, Level2)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->CreatePlatformPixmapSurface(nullptr, &gWrapperHook, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: LockSurfaceKHR001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, LockSurfaceKHR001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->LockSurfaceKHR(nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: UnlockSurfaceKHR001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, UnlockSurfaceKHR001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->UnlockSurfaceKHR(nullptr);
    ASSERT_EQ(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: CreateImageKHR001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreateImageKHR001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->CreateImageKHR(nullptr, 0, nullptr, nullptr);
    ASSERT_EQ(EGL_NO_IMAGE_KHR, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: DestroyImageKHR001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, DestroyImageKHR001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->DestroyImageKHR(nullptr);
    ASSERT_EQ(EGL_FALSE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: CreateStreamProducerSurfaceKHR001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreateStreamProducerSurfaceKHR001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = eglWrapperDisplay->CreateStreamProducerSurfaceKHR(nullptr, nullptr, 0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
    gWrapperHook.isLoad = temp;
}

/**
 * @tc.name: SwapBuffersWithDamageKHR001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, SwapBuffersWithDamageKHR001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->SwapBuffersWithDamageKHR(nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: SetDamageRegionKHR001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, SetDamageRegionKHR001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->SetDamageRegionKHR(nullptr, 0, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: GetCompositorTimingSupportedANDROID001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, GetCompositorTimingSupportedANDROID001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->GetCompositorTimingSupportedANDROID(nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: GetFrameTimestampSupportedANDROID001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, GetFrameTimestampSupportedANDROID001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->GetFrameTimestampSupportedANDROID(nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: PresentationTimeANDROID001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, PresentationTimeANDROID001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->PresentationTimeANDROID(nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: CreatePlatformWindowSurfaceEXT001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreatePlatformWindowSurfaceEXT001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->CreatePlatformWindowSurfaceEXT(nullptr, nullptr, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: CreatePlatformPixmapSurfaceEXT001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, CreatePlatformPixmapSurfaceEXT001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->CreatePlatformPixmapSurfaceEXT(nullptr, nullptr, nullptr);
    ASSERT_EQ(EGL_FALSE, result);
}

/**
 * @tc.name: SwapBuffersWithDamageEXT001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperDisplayTest, SwapBuffersWithDamageEXT001, Level1)
{
    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);

    auto result = eglWrapperDisplay->SwapBuffersWithDamageEXT(nullptr, nullptr, 0);
    ASSERT_EQ(EGL_FALSE, result);
}
} // OHOS::Rosen