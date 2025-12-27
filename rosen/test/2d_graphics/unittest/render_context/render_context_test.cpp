/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include <memory>

#include "platform/common/rs_system_properties.h"
#include "render_context.h"
#include "new_render_context/render_context_gl.h"
#include "new_render_context/render_context_vk.h"

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
    std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
    EGLSurface eglSurface = renderContext->CreateEGLSurface(nullptr);
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
    std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    EGLSurface eglSurface = renderContext->CreateEGLSurface(nullptr);
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
    std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
    bool grContext = renderContext->SetUpGpuContext();
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
    std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
    auto surface = renderContext->AcquireSurface(0, 0);
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
    std::shared_ptr<RenderContextGL> renderContextGL = std::make_shared<RenderContextGL>();
    EGLint bufferAge = renderContextGL->QueryEglBufferAge();
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
    std::shared_ptr<RenderContextGL> renderContextGL = std::make_shared<RenderContextGL>();
    EGLint bufferAge = renderContextGL->QueryEglBufferAge();
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
    std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    EXPECT_NE(renderContext->GetDrGPUContext(), nullptr);
    renderContext->ClearRedundantResources();
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
    std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
    renderContext->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    ASSERT_EQ(renderContext->GetColorSpace(), GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
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
    std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
    renderContext->SetPixelFormat(GRAPHIC_PIXEL_FMT_RGBA_1010102);
    ASSERT_EQ(renderContext->GetPixelFormat(), GRAPHIC_PIXEL_FMT_RGBA_1010102);
#endif
}

/**
 * @tc.name: DiscodingTest001
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, DiscodingTest001, Level1)
{
    auto renderContext = RenderContext::Create();
    EXPECT_NE(renderContext, nullptr);
}

/**
 * @tc.name: ClearRedundantTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, ClearRedundantTest, Level1)
{
    auto renderContext = RenderContext::Create();
    EXPECT_NE(renderContext, nullptr);
    renderContext->SetDrGPUContext(nullptr);
    renderContext->ClearRedundantResources();
    renderContext->SetDrGPUContext(std::make_shared<Drawing::GPUContext>());
    renderContext->ClearRedundantResources();
}

/**
 * @tc.name: DamageFrameTest003
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, DamageFrameTest003, Level1)
{
    auto renderContext = RenderContext::Create();
    EXPECT_NE(renderContext, nullptr);
    auto renderContextGL = std::make_shared<RenderContextGL>();
    renderContextGL->eglDisplay_ = nullptr;
    renderContextGL->DamageFrame({});
}

/**
 * @tc.name: RenderFrameTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, RenderFrameTest, Level1)
{
    std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->surface_ = nullptr;
    renderContext->RenderFrame();
    renderContext->surface_ = std::make_shared<Drawing::Surface>();
    EXPECT_NE(renderContext->surface_, nullptr);
    renderContext->RenderFrame();
}

/**
 * @tc.name: QueryEglBufferAgeTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, QueryEglBufferAgeTest, Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
        renderContext->eglDisplay_ = nullptr;
        EXPECT_EQ(EGL_UNKNOWN, renderContext->QueryEglBufferAge());
    }
}

/**
 * @tc.name: SetUpGpuContextTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, SetUpGpuContextTest, Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->SetDrGPUContext(std::make_shared<Drawing::GPUContext>());
    EXPECT_TRUE(renderContext->SetUpGpuContext());
}

/**
 * @tc.name: CreateEGLSurfaceTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, CreateEGLSurfaceTest, Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->SetDrGPUContext(std::make_shared<Drawing::GPUContext>());
    EXPECT_TRUE(renderContext->SetUpGpuContext());
}

/**
 * @tc.name: AbandonContextTest001
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, AbandonContextTest001, Level1)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto renderContext = RenderContext::Create();
        auto renderContextVK = static_pointer_cast<RenderContextVK>(renderContext);
        renderContextVK->SetDrGPUContext(nullptr);
        renderContextVK->AbandonContext();
        renderContextVK->SetDrGPUContext(std::make_shared<Drawing::GPUContext>());
        renderContextVK->AbandonContext();
    } else {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
    }
#endif
}


/**
 * @tc.name: QueryEglBufferAgeTest003
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, QueryEglBufferAgeTest003, Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    std::shared_ptr<RenderContextGL> renderContextGL = std::make_shared<RenderContextGL>();
    EXPECT_EQ(EGL_NO_DISPLAY, renderContextGL->GetEGLDisplay());
    renderContextGL->QueryEglBufferAge();
}

/**
 * @tc.name: DamageFrameTest002
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextTest, DamageFrameTest002, Level1)
{
    std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    EXPECT_NE(nullptr, renderContext->GetEGLDisplay());
    renderContext->DamageFrame({});
    RectI rect{0, 0, 1, 1};
    auto rects = {rect};
    renderContext->DamageFrame(rects);
}
} // namespace OHOS::Rosen