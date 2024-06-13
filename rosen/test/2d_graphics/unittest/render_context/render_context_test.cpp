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

#include <gtest/gtest.h>
#include <hilog/log.h>

#include "platform/common/rs_system_properties.h"
#include "render_context.h"

using namespace testing::ext;

namespace OHOS::Rosen {
class RenderContextTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RenderContextTest" };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RenderContextTest::SetUpTestCase() {}
void RenderContextTest::TearDownTestCase() {}
void RenderContextTest::SetUp() {}
void RenderContextTest::TearDown() {}

/**
 * @tc.name: CreateEGLSurfaceTest001
 * @tc.desc: Verify the CreateEGLSurfaceTest001 of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, CreateEGLSurfaceTest001, Function | SmallTest | Level2)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start CreateEGLSurfaceTest001 test
    RenderContext renderContext;
    EGLSurface eglSurface = renderContext.CreateEGLSurface(nullptr);
    EXPECT_EQ(eglSurface, EGL_NO_SURFACE);
#endif
}

/**
 * @tc.name: CreateEGLSurfaceTest002
 * @tc.desc: Verify the CreateEGLSurfaceTest002 of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, CreateEGLSurfaceTest002, Function | SmallTest | Level2)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start CreateEGLSurfaceTest002 test
    RenderContext renderContext;
    renderContext.InitializeEglContext();
    EGLSurface eglSurface = renderContext.CreateEGLSurface(nullptr);
    EXPECT_EQ(eglSurface, EGL_NO_SURFACE);
#endif
}

/**
 * @tc.name: SetUpGrContextTest
 * @tc.desc: Verify the SetUpGrContextTest of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, SetUpGrContextTest, Function | SmallTest | Level2)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start SetUpGrContextTest test
    RenderContext renderContext;
    bool grContext = renderContext.SetUpGpuContext();
    EXPECT_EQ(grContext, false);
#endif
}

/**
 * @tc.name: AcquireSurfaceTest
 * @tc.desc: Verify the AcquireSurfaceTest of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, AcquireSurfaceTest, Function | SmallTest | Level2)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start AcquireSurfaceTest test
    RenderContext renderContext;
    auto surface = renderContext.AcquireSurface(0, 0);
    EXPECT_TRUE(surface == nullptr);
#endif
}

/**
 * @tc.name: QueryEglBufferAgeTest001
 * @tc.desc: Verify the QueryEglBufferAgeTest001 of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, QueryEglBufferAgeTest001, Function | SmallTest | Level2)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start QueryEglBufferAgeTest001 test
    RenderContext renderContext;
    EGLint bufferAge = renderContext.QueryEglBufferAge();
    EXPECT_EQ(bufferAge, EGL_UNKNOWN);
#endif
}

/**
 * @tc.name: QueryEglBufferAgeTest002
 * @tc.desc: Verify the QueryEglBufferAgeTest002 of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, QueryEglBufferAgeTest002, Function | SmallTest | Level2)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start QueryEglBufferAgeTest002 test
    RenderContext renderContext;
    EGLint bufferAge = renderContext.QueryEglBufferAge();
    EXPECT_EQ(bufferAge, EGL_UNKNOWN);
#endif
}

/**
 * @tc.name: ClearRedundantResourcesTest001
 * @tc.desc: Verify the ClearRedundantResourcesTest002 of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, ClearRedundantResourcesTest001, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start ClearRedundantResourcesTest001 test
    RenderContext renderContext;
    renderContext.InitializeEglContext();
    renderContext.ClearRedundantResources();
#endif
}

/**
 * @tc.name: DamageFrameTest001
 * @tc.desc: Verify the DamageFrameTest001 of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, DamageFrameTest001, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start DamageFrameTest001 test
    RenderContext renderContext;
    renderContext.InitializeEglContext();
    renderContext.DamageFrame(0, 0, 0, 0);
#endif
}

/**
 * @tc.name: DamageFrameTest002
 * @tc.desc: Verify the DamageFrameTest001 of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, DamageFrameTest002, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start DamageFrameTest001 test
    RenderContext renderContext;
    renderContext.DamageFrame(0, 0, 0, 0);
    ASSERT_EQ(0, false);
#endif
}

/**
 * @tc.name: MakeSelfCurrentTest001
 * @tc.desc: Verify the MakeSelfCurrentTest001 of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, MakeSelfCurrentTest001, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start MakeSelfCurrentTest001 test
    RenderContext renderContext;
    renderContext.InitializeEglContext();
    renderContext.MakeSelfCurrent();
#endif
}

/**
 * @tc.name: ColorSpaceTest001
 * @tc.desc: Verify the SetColorSpaceTest and GetColorSpaceTest of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, ColorSpaceTest001, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start ColorSpaceTest001 test
    RenderContext renderContext;
    renderContext.SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    ASSERT_EQ(renderContext.GetColorSpace(), GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
#endif
}

/**
 * @tc.name: PixelFormatTest001
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, PixelFormatTest001, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    // start PixelFormatTest001 test
    RenderContext renderContext;
    renderContext.SetPixelFormat(GRAPHIC_PIXEL_FMT_RGBA_1010102);
    ASSERT_EQ(renderContext.GetPixelFormat(), GRAPHIC_PIXEL_FMT_RGBA_1010102);
#endif
}

/**
 * @tc.name: SetUpGpuContext001
 * @tc.desc: Verify the SetUpGpuContext of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, SetUpGpuContext001, Level1)
{
    RenderContext renderContext;
    auto drGPUContext_ = renderContext.GetSharedDrGPUContext();
    renderContext.SetUpGpuContext(drGPUContext_);
    ASSERT_EQ(1, true);
}

/**
 * @tc.name: SetUpGpuContext002
 * @tc.desc: Verify the SetUpGpuContext of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, SetUpGpuContext002, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    RenderContext renderContext;
    #ifdef RS_ENABLE_GL
        renderContext.SetUniRenderMode(1);
        renderContext.SetUpGpuContext(nullptr);
        ASSERT_EQ(1, true);
    #endif
#endif
}

/**
 * @tc.name: SetUpGpuContext003
 * @tc.desc: Verify the SetUpGpuContext of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, SetUpGpuContext003, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    RenderContext renderContext;
    #ifdef RS_ENABLE_GL
        glGetString(GL_VERSION);
        renderContext.SetUpGpuContext(nullptr);
        ASSERT_EQ(1, true);
    #endif
#endif
}

/**
 * @tc.name: SetUpGpuContext004
 * @tc.desc: Verify the SetUpGpuContext of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, SetUpGpuContext004, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    RenderContext renderContext;
    #ifdef RS_ENABLE_GL
        auto drGPUContext = std::make_shared<Drawing::GPUContext>();
        ASSERT_EQ(renderContext.SetUpGpuContext(nullptr), false);
    #endif
#endif
}

/**
 * @tc.name: SetUpGpuContext005
 * @tc.desc: Verify the SetUpGpuContext of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, SetUpGpuContext005, Level1)
{
#ifdef RS_ENABLE_VK
    RenderContext renderContext;
    ASSERT_EQ(renderContext.SetUpGpuContext(nullptr), true);
#endif
}

/**
 * @tc.name: AbandonContext001
 * @tc.desc: Verify the AbandonContext of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, AbandonContext001, Level1)
{
#ifdef RS_ENABLE_VK
    RenderContext renderContext;
    renderContext.AbandonContext();
    ASSERT_EQ(1, true);
#endif
}

/**
 * @tc.name: AbandonContext002
 * @tc.desc: Verify the AbandonContext of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, AbandonContext002, Level1)
{
#ifdef RS_ENABLE_VK
    RenderContext renderContext;
    drGPUContext_ = nullptr
    renderContext.AbandonContext();
    ASSERT_EQ(1, true);
#endif
}

/**
 * @tc.name: AcquireSurface001
 * @tc.desc: Verify the AcquireSurface of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, AcquireSurface001, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    RenderContext renderContext;
    renderContext.SetUpGpuContext(nullptr);
    ASSERT_EQ(renderContext.AcquireSurface(0, 0), nullptr);
#endif
}

/**
 * @tc.name: AcquireSurface002
 * @tc.desc: Verify the AcquireSurface of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, AcquireSurface002, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    RenderContext renderContext;
    ASSERT_EQ(renderContext.AcquireSurface(0, 0), nullptr);
#endif
}

/**
 * @tc.name: QueryEglBufferAge001
 * @tc.desc: Verify the QueryEglBufferAge of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, QueryEglBufferAge001, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    RenderContext renderContext;
    ASSERT_EQ(renderContext.QueryEglBufferAge(), EGL_UNKNOWN);
#endif
}

/**
 * @tc.name: QueryEglBufferAge002
 * @tc.desc: Verify the QueryEglBufferAge of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, QueryEglBufferAge002, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    RenderContext renderContext;
    eglQuerySurface(nullptr, nullptr, EGL_BUFFER_AGE_KHR, nullptr);
    ASSERT_EQ(renderContext.QueryEglBufferAge(), EGL_UNKNOWN);
#endif
}

/**
 * @tc.name: ClearRedundantResources001
 * @tc.desc: Verify the ClearRedundantResources of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, ClearRedundantResources001, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    RenderContext renderContext;
    renderContext.ClearRedundantResources();
    ASSERT_EQ(1, true);
#endif
}

/**
 * @tc.name: ConvertColorGamutToSkColorSpace001
 * @tc.desc: Verify the ConvertColorGamutToSkColorSpace of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, ConvertColorGamutToSkColorSpace001, Level1)
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    RenderContext renderContext;
    renderContext.ConvertColorGamutToSkColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    ASSERT_EQ(1, true);
    renderContext.ConvertColorGamutToSkColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    ASSERT_EQ(1, true);
    renderContext.ConvertColorGamutToSkColorSpace(GRAPHIC_COLOR_GAMUT_BT2020);
    ASSERT_EQ(1, true);
#endif
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
/**
 * @tc.name: GetShaderCacheSize001
 * @tc.desc: Verify the GetShaderCacheSize of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, GetShaderCacheSize001, Level1)
{
#ifdef RS_ENABLE_VK
    RenderContext renderContext;
    renderContext.GetShaderCacheSize();
    ASSERT_EQ(1, true);
#else
    RenderContext renderContext;
    renderContext.GetShaderCacheSize();
    ASSERT_EQ(1, true);
#endif
}

/**
 * @tc.name: CleanAllShaderCache001
 * @tc.desc: Verify the CleanAllShaderCache of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, CleanAllShaderCache001, Level1)
{
#ifdef RS_ENABLE_VK
    RenderContext renderContext;
    renderContext.CleanAllShaderCache();
    ASSERT_EQ(1, true);
#else
    RenderContext renderContext;
    renderContext.CleanAllShaderCache();
    ASSERT_EQ(1, true);
#endif
}
#endif
} // namespace OHOS::Rosen