/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <EGL/egl.h>

#include "parameters.h"
#include "EGL/egl_wrapper_custom.h"
#include "EGL/egl_wrapper_display.h"
#include "egl_defs.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class EglWrapperCustomTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp()
    {
        mockParameter = std::make_shared<OHOS::system::ParameterMock>();
        OHOS::system::SetParameterGlobalMock(mockParameter);
    }
    void TearDown()
    {
        mockParameter = nullptr;
        OHOS::system::SetParameterGlobalMock(nullptr);
    }

    std::shared_ptr<OHOS::system::ParameterMock> mockParameter;
};

#ifdef OPENGL_WRAPPER_ENABLE_GL4
/**
 * @tc.name: EglGetProcAddressCustomImplTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, EglGetProcAddressCustomImplTest001, Level1)
{
    auto result = EglGetProcAddressCustomImpl("OH_Graphics_QueryGL");
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: EglGetProcAddressCustomImplTest002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, EglGetProcAddressCustomImplTest002, Level1)
{
    auto result = EglGetProcAddressCustomImpl("");
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: OHGraphicsQueryGLImplTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, OHGraphicsQueryGLImplTest001, Level1)
{
    EXPECT_CALL(*mockParameter, GetBoolParameter(_, _)).WillOnce(testing::Return(true));
    EXPECT_EQ(OHGraphicsQueryGLImpl(), EGL_TRUE);
}

/**
 * @tc.name: OHGraphicsQueryGLImplTest002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, OHGraphicsQueryGLImplTest002, Level1)
{
    EXPECT_CALL(*mockParameter, GetBoolParameter(_, _)).WillOnce(testing::Return(false));
    EXPECT_EQ(OHGraphicsQueryGLImpl(), EGL_FALSE);
}
#endif

namespace {
typedef EGLBoolean (*SetEngineNameFunc)(EGLDisplay dpy, const char* name);
} // namespace

/**
 * @tc.name: EglSetEngineNameImplTest001
 * @tc.desc: Test EglSetEngineNameImpl with nullptr display
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, EglSetEngineNameImplTest001, Level1)
{
    auto func = reinterpret_cast<SetEngineNameFunc>(eglGetProcAddress("eglSetEngineName"));
    ASSERT_NE(nullptr, func);

    auto result = func(EGL_NO_DISPLAY, "test_engine");
    EXPECT_EQ(result, EGL_FALSE);
}

/**
 * @tc.name: EglSetEngineNameImplTest002
 * @tc.desc: Test EglSetEngineNameImpl with invalid display (not initialized)
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, EglSetEngineNameImplTest002, Level1)
{
    auto func = reinterpret_cast<SetEngineNameFunc>(eglGetProcAddress("eglSetEngineName"));
    ASSERT_NE(nullptr, func);

    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay((EGLDisplay)&EglWrapperDisplay::wrapperDisp_);
    ASSERT_NE(nullptr, eglWrapperDisplay);
    auto tmp = eglWrapperDisplay->refCnt_;
    eglWrapperDisplay->refCnt_ = 0; // make display invalid

    auto result = func((EGLDisplay)eglWrapperDisplay, "test_engine");
    EXPECT_EQ(result, EGL_FALSE);

    eglWrapperDisplay->refCnt_ = tmp; // restore display for future test
}

/**
 * @tc.name: EglSetEngineNameImplTest003
 * @tc.desc: Test EglSetEngineNameImpl when driver not loaded
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, EglSetEngineNameImplTest003, Level1)
{
    auto func = reinterpret_cast<SetEngineNameFunc>(eglGetProcAddress("eglSetEngineName"));
    ASSERT_NE(nullptr, func);

    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major, minor;
    EGLBoolean ret = eglInitialize(dpy, &major, &minor);
    ASSERT_EQ(ret, EGL_TRUE);

    auto eglWrapperDisplay = EglWrapperDisplay::GetWrapperDisplay(dpy);
    ASSERT_NE(nullptr, eglWrapperDisplay);

    auto temp = gWrapperHook.isLoad;
    gWrapperHook.isLoad = false;

    auto result = func(dpy, "test_engine");
    EXPECT_EQ(result, EGL_FALSE);

    gWrapperHook.isLoad = temp;
    eglTerminate(dpy);
}

/**
 * @tc.name: EglSetEngineNameImplTest004
 * @tc.desc: Test EglSetEngineNameImpl when eglGetProcAddress is null
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, EglSetEngineNameImplTest004, Level1)
{
    auto func = reinterpret_cast<SetEngineNameFunc>(eglGetProcAddress("eglSetEngineName"));
    ASSERT_NE(nullptr, func);

    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major, minor;
    EGLBoolean ret = eglInitialize(dpy, &major, &minor);
    ASSERT_EQ(ret, EGL_TRUE);

    auto temp = gWrapperHook.egl.eglGetProcAddress;
    gWrapperHook.egl.eglGetProcAddress = nullptr;

    auto result = func(dpy, "test_engine");
    EXPECT_EQ(result, EGL_FALSE);

    gWrapperHook.egl.eglGetProcAddress = temp;
    eglTerminate(dpy);
}

/**
 * @tc.name: EglSetEngineNameImplTest005
 * @tc.desc: Test EglSetEngineNameImpl when driver not support eglSetEngineName
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, EglSetEngineNameImplTest005, Level1)
{
    auto func = reinterpret_cast<SetEngineNameFunc>(eglGetProcAddress("eglSetEngineName"));
    ASSERT_NE(nullptr, func);

    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major, minor;
    EGLBoolean ret = eglInitialize(dpy, &major, &minor);
    ASSERT_EQ(ret, EGL_TRUE);

    auto tempIsLoad = gWrapperHook.isLoad;
    auto tempGetProcAddr = gWrapperHook.egl.eglGetProcAddress;

    gWrapperHook.isLoad = true;
    gWrapperHook.egl.eglGetProcAddress = +[](const char*) -> __eglMustCastToProperFunctionPointerType { return nullptr; };

    auto result = func(dpy, "test_engine");
    EXPECT_EQ(result, EGL_FALSE);

    gWrapperHook.isLoad = tempIsLoad;
    gWrapperHook.egl.eglGetProcAddress = tempGetProcAddr;
    eglTerminate(dpy);
}

/**
 * @tc.name: EglSetEngineNameImplTest006
 * @tc.desc: Test EglSetEngineNameImpl success path
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperCustomTest, EglSetEngineNameImplTest006, Level1)
{
    auto func = reinterpret_cast<SetEngineNameFunc>(eglGetProcAddress("eglSetEngineName"));
    ASSERT_NE(nullptr, func);

    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major, minor;
    EGLBoolean ret = eglInitialize(dpy, &major, &minor);
    ASSERT_EQ(ret, EGL_TRUE);

    auto tempIsLoad = gWrapperHook.isLoad;
    auto tempGetProcAddr = gWrapperHook.egl.eglGetProcAddress;

    gWrapperHook.isLoad = true;
    gWrapperHook.egl.eglGetProcAddress = +[](const char*) -> __eglMustCastToProperFunctionPointerType {
        return reinterpret_cast<__eglMustCastToProperFunctionPointerType>(
            +[](EGLDisplay, const char*) -> EGLBoolean { return EGL_TRUE; }
        );
    };

    auto result = func(dpy, "test_engine");
    EXPECT_EQ(result, EGL_TRUE);

    gWrapperHook.isLoad = tempIsLoad;
    gWrapperHook.egl.eglGetProcAddress = tempGetProcAddr;
    eglTerminate(dpy);
}
} // OHOS::Rosen