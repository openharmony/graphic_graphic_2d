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

#include "render_context.h"

#include "iconsumer_surface.h"
#include "platform/common/rs_system_properties.h"
#include "render_context/new_render_context/render_context_gl.h"
#include "surface/ibuffer_consumer_listener.h"
#include "window.h"
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

class RenderContextGLTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static GpuApiType gpuType;
};

void RenderContextGLTest::SetUpTestCase() {}
void RenderContextGLTest::TearDownTestCase() {}
void RenderContextGLTest::SetUp()
{
    const_cast<GpuApiType&>(RSSystemProperties::systemGpuApiType_) = GpuApiType::OPENGL;
}
void RenderContextGLTest::TearDown()
{
    const_cast<GpuApiType&>(RSSystemProperties::systemGpuApiType_) = gpuType;
}

GpuApiType RenderContextGLTest::gpuType = RSSystemProperties::GetGpuApiType();

/**
 * @tc.name: CleanAllShaderCache
 * @tc.desc: Verify the CleanAllShaderCache of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, CleanAllShaderCache, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    std::string result = renderContext->CleanAllShaderCache();
    EXPECT_EQ(result, "");

    renderContext->SetUpGpuContext();
    result = renderContext->CleanAllShaderCache();
    EXPECT_NE(result, "");
}

/**
 * @tc.name: GetShaderCacheSize
 * @tc.desc: Verify the GetShaderCacheSize of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, GetShaderCacheSize, TestSize.Level1)
{
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
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->drGPUContext_ = nullptr;
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
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    EXPECT_NE(nullptr, renderContext->GetEGLDisplay());
    renderContext->DamageFrame({});
    RectI rect { 0, 0, 1, 1 };
    auto rects = { rect };
    renderContext->DamageFrame(rects);
    EXPECT_NE(nullptr, renderContext->GetEGLDisplay());

    auto context = std::make_shared<RenderContextGL>();
    context->Init();
    renderContext->SetUpGpuContext();
    auto cSurface = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    auto window = CreateNativeWindowFromSurface(&pSurface);
    context->CreateEGLSurface(window);
    context->DamageFrame({});
    EXPECT_NE(nullptr, context->GetEGLDisplay());

    context->DamageFrame(rects);
    EXPECT_NE(nullptr, context->GetEGLDisplay());
}

/**
 * @tc.name: QueryEglBufferAgeTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, QueryEglBufferAgeTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    renderContext->MakeCurrent(renderContext->GetEGLSurface(), renderContext->GetEGLContext());
    auto res = renderContext->QueryEglBufferAge();
    EXPECT_EQ(EGL_UNKNOWN, res);

    auto context = std::make_shared<RenderContextGL>();
    context->Init();
    context->SetUpGpuContext();
    auto cSurface = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    auto window = CreateNativeWindowFromSurface(&pSurface);
    context->CreateEGLSurface(window);
    res = context->QueryEglBufferAge();
    EXPECT_EQ(res, EGL_UNKNOWN);
}

/**
 * @tc.name: RenderFrameTest
 * @tc.desc: Verify the SetPixelFormatTest and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, RenderFrameTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->surface_ = nullptr;
    renderContext->RenderFrame();
    renderContext->surface_ = std::make_shared<Drawing::Surface>();
    EXPECT_NE(renderContext->surface_, nullptr);
    renderContext->RenderFrame();
    renderContext->surface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>(0, 0);
    renderContext->RenderFrame();
    EXPECT_NE(renderContext, nullptr);
}

/**
 * @tc.name: AcquireSurfaceTest
 * @tc.desc: Verify the AcquireSurfaceTest of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, AcquireSurfaceTest, Function | SmallTest | Level2)
{
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

    renderContext->Init();
    renderContext->SetUpGpuContext();
    renderContext->SetPixelFormat(GRAPHIC_PIXEL_FMT_YCBCR_P010);
    surface = renderContext->AcquireSurface(400, 800);
    EXPECT_TRUE(surface == nullptr);
}

/**
 * @tc.name: DestroyEGLSurface
 * @tc.desc: Verify the DestroyEGLSurface of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, DestroyEGLSurfaceTest, Function | SmallTest | Level2)
{
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
    auto renderContext = std::make_shared<RenderContextGL>();
    EGLSurface eglSurface = renderContext->CreateEGLSurface(nullptr);
    EXPECT_EQ(eglSurface, EGL_NO_SURFACE);

    renderContext->Init();
    eglSurface = renderContext->CreateEGLSurface(nullptr);
    EXPECT_EQ(eglSurface, EGL_NO_SURFACE);

    auto context = std::make_shared<RenderContextGL>();
    context->Init();
    context->SetUpGpuContext();
    EXPECT_NE(context->eglContext_, EGL_NO_CONTEXT);
    auto cSurface = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    auto window = CreateNativeWindowFromSurface(&pSurface);
    eglSurface = context->CreateEGLSurface(window);
    EXPECT_NE(eglSurface, EGL_NO_SURFACE);
}

/**
 * @tc.name: SwapBuffersTest
 * @tc.desc: Verify the SwapBuffers of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, SwapBuffersTest, Function | SmallTest | Level2)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    auto res = renderContext->SwapBuffers(EGL_NO_SURFACE);
    EXPECT_EQ(res, false);
    renderContext->Init();
    renderContext->SetUpGpuContext();
    auto cSurface = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    auto window = CreateNativeWindowFromSurface(&pSurface);
    renderContext->CreateEGLSurface(window);

    auto context = std::make_shared<RenderContextGL>();
    context->Init();
    context->SetUpGpuContext();
    auto cSurface1 = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener1 = new BufferConsumerListener();
    cSurface1->RegisterConsumerListener(listener1);
    auto producer1 = cSurface1->GetProducer();
    auto pSurface1 = Surface::CreateSurfaceAsProducer(producer1);
    auto window1 = CreateNativeWindowFromSurface(&pSurface1);
    auto eglSurface1 = context->CreateEGLSurface(window1);
    res = renderContext->SwapBuffers(eglSurface1);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: MakeCurrentTest
 * @tc.desc: Verify the MakeCurrent of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, MakeCurrentTest, Function | SmallTest | Level2)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->MakeCurrent(EGL_NO_SURFACE, EGL_NO_CONTEXT);
    EXPECT_EQ(renderContext->GetEGLSurface(), EGL_NO_SURFACE);
    renderContext->MakeCurrent(static_cast<EGLSurface>((void*)(0x1)), static_cast<EGLContext>((void*)(0x1)));
    EXPECT_NE(renderContext->GetEGLSurface(), EGL_NO_SURFACE);
}

/**
 * @tc.name: InitializeEglContextTest
 * @tc.desc: Verify the InitializeEglContext of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, InitializeEglContextTest, Function | SmallTest | Level2)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    auto res = renderContext->Init();
    EXPECT_EQ(res, true);

    res = renderContext->Init();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: CreatePbufferSurfaceTest001
 * @tc.desc: Verify the CreatePbufferSurface and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, CreatePbufferSurfaceTest001, Level1)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->CreatePbufferSurface();
    EXPECT_EQ(renderContext->pbufferSurface_, EGL_NO_SURFACE);

    renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    renderContext->CreatePbufferSurface();
    EXPECT_EQ(renderContext->pbufferSurface_, EGL_NO_SURFACE);
}

/**
 * @tc.name: SetUpGpuContextTest001
 * @tc.desc: Verify the SetUpGpuContext and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, SetUpGpuContextTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    EXPECT_NE(renderContext, nullptr);
    bool res = renderContext->SetUpGpuContext();
    EXPECT_EQ(res, true);
    renderContext = std::make_shared<RenderContextGL>();
    renderContext->isUniRenderMode_ = true;
    res = renderContext->SetUpGpuContext();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: AbandonContextTest001
 * @tc.desc: Verify the AbandonContext and GetPixelFormat of RenderContextGLTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, AbandonContextTest, Level1)
{
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
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->CreateShareContext();
    EXPECT_EQ(renderContext->eglShareContext_, EGL_NO_CONTEXT);

    renderContext->Init();
    renderContext->SetUpGpuContext();
    renderContext->CreateShareContext();
    EXPECT_NE(renderContext->eglShareContext_, EGL_NO_CONTEXT);
}

/**
 * @tc.name: DestroyShareContextTest001
 * @tc.desc: Verify DestroyShareContext
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, DestroyShareContextTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->DestroyShareContext();
    EXPECT_NE(renderContext, nullptr);
}

/**
 * @tc.name: ColorTypeTest
 * @tc.desc: Verify ColorTypeToGLFormat
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, ColorTypeTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    Drawing::ColorType type = Drawing::ColorType::COLORTYPE_RGBA_8888;
    auto res = renderContext->ColorTypeToGLFormat(type);
    EXPECT_EQ(res, GL_RGBA8);
    type = Drawing::ColorType::COLORTYPE_ALPHA_8;
    res = renderContext->ColorTypeToGLFormat(type);
    EXPECT_EQ(res, GL_R8);
    type = Drawing::ColorType::COLORTYPE_RGB_565;
    res = renderContext->ColorTypeToGLFormat(type);
    EXPECT_EQ(res, GL_RGB565);
    type = Drawing::ColorType::COLORTYPE_ARGB_4444;
    res = renderContext->ColorTypeToGLFormat(type);
    EXPECT_EQ(res, GL_RGBA4);
    type = Drawing::ColorType::COLORTYPE_RGBA_1010102;
    res = renderContext->ColorTypeToGLFormat(type);
    EXPECT_EQ(res, GL_RGBA8);
}

/**
 * @tc.name: CheckEglExtensionTest
 * @tc.desc: Verify CheckEglExtension
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextGLTest, CheckEglExtensionTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    EXPECT_TRUE(renderContext->CheckEglExtension("EGL_EXT_platform_wayland",
        "EGL_EXT_platform_wayland"));
    EXPECT_FALSE(renderContext->CheckEglExtension("EGL_EXT_platform_x11",
        "EGL_EXT_platform_wayland"));
    EXPECT_TRUE(renderContext->CheckEglExtension("  EGL_EXT_platform_wayland  EGL_OTHER",
        "EGL_EXT_platform_wayland"));
    EXPECT_FALSE(renderContext->CheckEglExtension("EGL_EXT_platform_wayland2",
        "EGL_EXT_platform_wayland"));
    EXPECT_FALSE(renderContext->CheckEglExtension("", "EGL_EXT_platform_wayland"));
    EXPECT_TRUE(renderContext->CheckEglExtension("EGL_FIRST EGL_EXT_platform_wayland EGL_LAST",
        "EGL_EXT_platform_wayland"));
}
} // namespace OHOS::Rosen