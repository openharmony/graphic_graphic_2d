/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <memory>
#include "platform/common/rs_system_properties.h"
#include "render_context.h"
#include "render_context/new_render_context/render_context_gl.h"
using namespace testing::ext;

namespace OHOS::Rosen {
class RenderContextGLTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RenderContextGLTest::SetUpTestCase() {}
void RenderContextGLTest::TearDownTestCase() {}
void RenderContextGLTest::SetUp() {}
void RenderContextGLTest::TearDown() {}

/**
 * @tc.name: CleanAllShaderCache
 * @tc.desc: Verify the CleanAllShaderCache of RenderContextGLTest
 * @tc.type: FUNC
*/
HWTEST_F(RenderContextGLTest, CleanAllShaderCache, TestSize.Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    std::string result = renderContext->CleanAllShaderCache();
    EXPECT_EQ(result, "");

    renderContext->SetUpGpuContext();
    result = renderContext->CleanAllShaderCache();
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: GetShaderCacheSize
 * @tc.desc: Verify the GetShaderCacheSize of RenderContextGLTest
 * @tc.type: FUNC
*/
HWTEST_F(RenderContextGLTest, GetShaderCacheSize, TestSize.Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    auto result = renderContext->GetShaderCacheSize();
    EXPECT_EQ(result, "");

    renderContext->SetUpGpuContext();
    result = renderContext->GetShaderCacheSize();
    EXPECT_NE(result, "");
}

/**
 * @tc.name: ClearRedundantResources
 * @tc.desc: Verify the ClearRedundantResources of RenderContextGLTest
 * @tc.type: FUNC
*/
HWTEST_F(RenderContextGLTest, ClearRedundantResources, TestSize.Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->ClearRedundantResources();

    renderContext->SetUpGpuContext();
    renderContext->ClearRedundantResources();
    EXPECT_EQ(renderContext->GetEGLSurface(), EGL_NO_SURFACE);
}

/**
 * @tc.name: DamageFrameTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
*/
HWTEST_F(RenderContextGLTest, DamageFrameTest, Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    EXPECT_NE(nullptr, renderContext->GetEGLDisplay());
    renderContext->DamageFrame({});
    RectI rect{0, 0, 1, 1};
    auto rects = {rect};
    renderContext->DamageFrame(rects);
    EXPECT_NE(nullptr, renderContext->GetEGLDisplay());
}

/**
 * @tc.name: QueryEglBufferAgeTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, QueryEglBufferAgeTest, Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    renderContext->MakeCurrent(renderContext->GetEGLSurface(), renderContext->GetEGLContext());
    auto res = renderContext->QueryEglBufferAge();
    EXPECT_EQ(EGL_UNKNOWN, res);
}

/**
 * @tc.name: RenderFrameTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, RenderFrameTest, Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->surface_ = nullptr;
    renderContext->RenderFrame();
    renderContext->surface_ = std::make_shared<Drawing::Surface>();
    EXPECT_NE(renderContext->surface_, nullptr);
    renderContext->RenderFrame();
}


/**
 * @tc.name: AcquireSurfaceTest
 * @tc.desc: Verify the AcquireSurfaceTest of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, AcquireSurfaceTest, Function | SmallTest | Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    auto surface = renderContext->AcquireSurface(0, 0);
    EXPECT_TRUE(surface == nullptr);
    renderContext->SetColorSpace(GRAPHIC_COLOR_GAMUT_BT2100_HLG);
    surface = renderContext->AcquireSurface(0, 0);
    EXPECT_TRUE(surface == nullptr);
    renderContext->SetColorSpace(GRAPHIC_COLOR_GAMUT_BT2020);
    surface = renderContext->AcquireSurface(0, 0);
    EXPECT_TRUE(surface == nullptr);
    renderContext->SetColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    surface = renderContext->AcquireSurface(0, 0);
    EXPECT_TRUE(surface == nullptr);

    renderContext->SetPixelFormat(GRAPHIC_PIXEL_FMT_RGBA_1010102);
    surface = renderContext->AcquireSurface(0, 0);
    EXPECT_TRUE(surface == nullptr);

    renderContext->SetPixelFormat(GRAPHIC_PIXEL_FMT_YCBCR_P010);
    surface = renderContext->AcquireSurface(400, 800);
    EXPECT_FALSE(surface == nullptr);
}

/**
 * @tc.name: DestroyEGLSurface
 * @tc.desc: Verify the DestroyEGLSurface of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, DestroyEGLSurfaceTest, Function | SmallTest | Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->DestroyEGLSurface(renderContext->GetEGLSurface());
    EXPECT_EQ(renderContext->GetEGLSurface(), nullptr);

    renderContext->Init();
    renderContext->MakeCurrent(renderContext->GetEGLSurface(), renderContext->GetEGLContext());
    renderContext->DestroyEGLSurface(renderContext->GetEGLSurface());
    EXPECT_EQ(renderContext->GetEGLSurface(), nullptr);
}

/**
 * @tc.name: CreateEGLSurfaceTest
 * @tc.desc: Verify the CreateEGLSurfaceTest of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, CreateEGLSurfaceTest, Function | SmallTest | Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    EGLSurface eglSurface = renderContext->CreateEGLSurface(nullptr);
    EXPECT_EQ(eglSurface, EGL_NO_SURFACE);

    renderContext->Init();
    eglSurface = renderContext->CreateEGLSurface(nullptr);
    EXPECT_EQ(eglSurface, EGL_NO_SURFACE);
    renderContext->SetUpGpuContext();
    eglSurface = renderContext->CreateEGLSurface(nullptr);
    EXPECT_EQ(eglSurface, EGL_NO_SURFACE);
}

/**
 * @tc.name: SwapBuffersTest
 * @tc.desc: Verify the SwapBuffers of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, SwapBuffersTest, Function | SmallTest | Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->SwapBuffers(EGL_NO_SURFACE);
    EXPECT_EQ(renderContext->GetEGLSurface(), EGL_NO_SURFACE);
}

/**
 * @tc.name: MakeCurrentTest
 * @tc.desc: Verify the MakeCurrent of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, MakeCurrentTest, Function | SmallTest | Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->MakeCurrent(EGL_NO_SURFACE, EGL_NO_CONTEXT);
    EXPECT_EQ(renderContext->GetEGLSurface(), EGL_NO_SURFACE);
    renderContext->MakeCurrent(static_cast<EGLSurface>((void*)(0x1)), static_cast<EGLContext>((void*)(0x1)));
    EXPECT_EQ(renderContext->GetEGLSurface(), EGL_NO_SURFACE);
}

/**
 * @tc.name: InitializeEglContextTest
 * @tc.desc: Verify the InitializeEglContext of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, InitializeEglContextTest, Function | SmallTest | Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    auto res = renderContext->Init();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: CreatePbufferSurfaceTest001
 * @tc.desc: Verify the CreatePbufferSurface and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, CreatePbufferSurfaceTest001, Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->CreatePbufferSurface();
    EXPECT_EQ(renderContext->GetEGLDisplay(), EGL_NO_DISPLAY);
}

/**
 * @tc.name: SetUpGpuContextTest001
 * @tc.desc: Verify the SetUpGpuContext and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, SetUpGpuContextTest, Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    EXPECT_NE(renderContext, nullptr);
    bool res = renderContext->SetUpGpuContext();
    EXPECT_EQ(res, false);
    res = renderContext->SetUpGpuContext();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: AbandonContextTest001
 * @tc.desc: Verify the AbandonContext and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, AbandonContextTest, Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    auto res = renderContext->AbandonContext();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: CreateShareContextTest001
 * @tc.desc: Verify CreateShareContext
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, CreateShareContextTest, Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->CreateShareContext();
    EXPECT_NE(renderContext, nullptr);
}

/**
 * @tc.name: DestroyShareContextTest001
 * @tc.desc: Verify DestroyShareContext
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, DestroyShareContextTest, Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->DestroyShareContext();
    EXPECT_NE(renderContext, nullptr);
}

/**
 * @tc.name: QueryMaxGpuBufferSize001
 * @tc.desc: Verify QueryMaxGpuBufferSize with initialized context
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, QueryMaxGpuBufferSize001, TestSize.Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    bool result = renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_TRUE(result);
    EXPECT_GT(maxWidth, 0);
    EXPECT_GT(maxHeight, 0);
}

/**
 * @tc.name: QueryMaxGpuBufferSize002
 * @tc.desc: Verify QueryMaxGpuBufferSize with uninitialized context
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, QueryMaxGpuBufferSize002, TestSize.Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    bool result = renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_NE(result, -2);
}

/**
 * @tc.name: QueryMaxGpuBufferSize003
 * @tc.desc: Verify QueryMaxGpuBufferSize with preset values
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, QueryMaxGpuBufferSize003, TestSize.Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    uint32_t maxWidth = 2048;
    uint32_t maxHeight = 2048;
    bool result = renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_TRUE(result);
    EXPECT_GE(maxWidth, 2048);
    EXPECT_GE(maxHeight, 2048);
}

/**
 * @tc.name: QueryMaxGpuBufferSize004
 * @tc.desc: Verify QueryMaxGpuBufferSize consistency across multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, QueryMaxGpuBufferSize004, TestSize.Level1)
{
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    uint32_t maxWidth1 = 0;
    uint32_t maxHeight1 = 0;
    bool result1 = renderContext->QueryMaxGpuBufferSize(maxWidth1, maxHeight1);

    uint32_t maxWidth2 = 0;
    uint32_t maxHeight2 = 0;
    bool result2 = renderContext->QueryMaxGpuBufferSize(maxWidth2, maxHeight2);

    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    EXPECT_EQ(maxWidth1, maxWidth2);
    EXPECT_EQ(maxHeight1, maxHeight2);
}
} // namespace OHOS::Rosen