/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "iconsumer_surface.h"

#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceOhosGlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceOhosGlTest::SetUpTestCase() {}
void RSSurfaceOhosGlTest::TearDownTestCase() {}
void RSSurfaceOhosGlTest::SetUp() {}
void RSSurfaceOhosGlTest::TearDown() {}

/**
 * @tc.name: FlushFrame001
 * @tc.desc: test results of FlushFrame
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosGlTest, FlushFrame001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosGl rsSurface(producer);
    uint64_t uiTimestamp = 1;
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    rsSurface.SetUiTimeStamp(frame, uiTimestamp);
    EXPECT_FALSE(rsSurface.FlushFrame(frame, uiTimestamp));
    {
        sptr<Surface> producer = nullptr;
        RSSurfaceOhosGl rsSurface(producer);
        RenderContext renderContext;
        rsSurface.SetRenderContext(&renderContext);
        ASSERT_TRUE(rsSurface.FlushFrame(frame, uiTimestamp));
    }
}

/**
 * @tc.name: ClearBuffer001
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosGlTest, ClearBuffer001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosGl rsSurface(producer);
    rsSurface.ClearBuffer();

    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    rsSurface.SetRenderContext(renderContext);
    rsSurface.ClearBuffer();

    EGLSurface mEglSurface = EGL_NO_CONTEXT;
    rsSurface.mEglSurface = mEglSurface;
    rsSurface.ClearBuffer();
}

/**
 * @tc.name: ClearBuffer002
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosGlTest, ClearBuffer002, TestSize.Level1)
{
    RSSurfaceOhosGl rsSurface(IConsumerSurface::Create());
#ifdef ACE_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        rsSurface.SetRenderContext(renderContext);
        int32_t width = 1;
        int32_t height = 1;
        uint64_t uiTimestamp = 1;
        bool useAFBC = true;
        rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);
    }
#endif
    rsSurface.ClearBuffer();

    {
        RSSurfaceOhosGl rsSurface(IConsumerSurface::Create());
#ifdef ACE_ENABLE_GPU
        RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
        if (renderContext) {
            renderContext->InitializeEglContext();
            rsSurface.SetRenderContext(renderContext);
            int32_t width = 1;
            int32_t height = 1;
            uint64_t uiTimestamp = 1;
            bool useAFBC = true;
            rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);
        }
#endif
        EGLSurface mEglSurface = EGL_NO_CONTEXT;
        rsSurface.mEglSurface = mEglSurface;
        rsSurface.ClearBuffer();
        ASSERT_EQ(rsSurface.mEglSurface, EGL_NO_SURFACE);
    }
}

/**
 * @tc.name: ResetBufferAge001
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosGlTest, ResetBufferAge001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosGl rsSurface(producer);
    rsSurface.ResetBufferAge();

    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    rsSurface.SetRenderContext(renderContext);
    rsSurface.ResetBufferAge();

    EGLSurface mEglSurface = EGL_NO_CONTEXT;
    rsSurface.mEglSurface = mEglSurface;
    rsSurface.ResetBufferAge();
    ASSERT_EQ(rsSurface.mWindow, nullptr);
}

/**
 * @tc.name: ResetBufferAge002
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosGlTest, ResetBufferAge002, TestSize.Level1)
{
    RSSurfaceOhosGl rsSurface(IConsumerSurface::Create());
#ifdef ACE_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        rsSurface.SetRenderContext(renderContext);
        int32_t width = 1;
        int32_t height = 1;
        uint64_t uiTimestamp = 1;
        bool useAFBC = true;
        rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);
    }
#endif
    rsSurface.ResetBufferAge();
    EXPECT_EQ(rsSurface.mEglSurface, EGL_NO_SURFACE);
    
    {
        RSSurfaceOhosGl rsSurface(IConsumerSurface::Create());
#ifdef ACE_ENABLE_GPU
        RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
        if (renderContext) {
            renderContext->InitializeEglContext();
            rsSurface.SetRenderContext(renderContext);
            int32_t width = 1;
            int32_t height = 1;
            uint64_t uiTimestamp = 1;
            bool useAFBC = true;
            rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);
        }
#endif
        EGLSurface mEglSurface = EGL_NO_CONTEXT;
        rsSurface.mEglSurface = mEglSurface;
        rsSurface.ResetBufferAge();
        EXPECT_EQ(rsSurface.mEglSurface, EGL_NO_SURFACE);
    }
}

/**
 * @tc.name: RequestFrame001
 * @tc.desc: test results of RequestFrame
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosGlTest, RequestFrame001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosGl rsSurface(pSurface);
    int32_t width = 1;
    int32_t height = 1;
    uint64_t uiTimestamp = 1;
    bool useAFBC = true;
    rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);

    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    rsSurface.SetRenderContext(renderContext);
    rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);

    rsSurface.mWindow = CreateNativeWindowFromSurface(&rsSurface.producer_);
    rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);

    rsSurface.mEglSurface = EGL_NO_CONTEXT;
    ASSERT_EQ(rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC), nullptr);
}
} // namespace Rosen
} // namespace OHOS