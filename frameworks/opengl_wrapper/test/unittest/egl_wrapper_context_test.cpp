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

#include "EGL/egl_wrapper_context.h"

#include "EGL/egl_wrapper_display.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class EglWrapperContextTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: GetCurrentSurface001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglWrapperContextTest, GetCurrentSurface001, Level1)
{
    EGLDisplay display = nullptr;
    EglWrapperDisplay *disp = EglWrapperDisplay::GetWrapperDisplay(display);
    EGLContext context = nullptr;
    EglWrapperContext ctx(disp, context);

    EGLSurface draw = nullptr;
    EGLSurface read = nullptr;
    ctx.SetCurrentSurface(draw, read);

    auto result = ctx.GetCurrentSurface(EGL_READ);
    ASSERT_EQ(read, result);
    result = ctx.GetCurrentSurface(EGL_DRAW);
    ASSERT_EQ(draw, result);
    result = ctx.GetCurrentSurface(0);
    ASSERT_EQ(EGL_NO_SURFACE, result);
}
} // OHOS::Rosen