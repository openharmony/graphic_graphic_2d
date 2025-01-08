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

#include "render_context/render_context.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"

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
 * @tc.name: FlushFrameTest001
 * @tc.desc: test results of FlushFrame
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosGlTest, FlushFrameTest001, TestSize.Level1)
{
    sptr<Surface> producer1 = nullptr;
    RSSurfaceOhosGl rsSurface(producer1);
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    uint64_t uiTimestamp = 1;
    rsSurface.SetUiTimeStamp(frame, uiTimestamp);
    EXPECT_FALSE(rsSurface.FlushFrame(frame, uiTimestamp));
    {
        sptr<Surface> producer1 = nullptr;
        RSSurfaceOhosGl rsSurface(producer1);
        RenderContext renderContext;
        rsSurface.SetRenderContext(&renderContext);
        ASSERT_TRUE(rsSurface.FlushFrame(frame, uiTimestamp));
    }
}

/**
 * @tc.name: ClearBufferTest001
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosGlTest, ClearBufferTest001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface1 != nullptr);
    sptr<IBufferProducer> bp = cSurface1->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosGl rsSurface(pSurface);
#ifdef RS_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        rsSurface.SetRenderContext(renderContext);
        bool useAFBC = true;
        int32_t height = 1;
        int32_t width = 1;
        uint64_t uiTimestamp = 1;
        rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);
    }
#endif
    rsSurface.ClearBuffer();

    {
        sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("DisplayNode");
        ASSERT_TRUE(cSurface1 != nullptr);
        sptr<IBufferProducer> bp = cSurface1->GetProducer();
        sptr<Surface> pSurface1 = Surface::CreateSurfaceAsProducer(bp);
        RSSurfaceOhosGl rsSurface(pSurface1);
#ifdef RS_ENABLE_GPU
        RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
        if (renderContext) {
            renderContext->InitializeEglContext();
            rsSurface.SetRenderContext(renderContext);
            bool useAFBC = true;
            int32_t height = 1;
            int32_t width = 1;
            uint64_t uiTimestamp = 1;
            rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);
        }
#endif
        EGLSurface mEglSurface1 = EGL_NO_CONTEXT;
        rsSurface.mEglSurface = mEglSurface1;
        rsSurface.ClearBuffer();
        ASSERT_EQ(rsSurface.mEglSurface, EGL_NO_SURFACE);
    }
}

/**
 * @tc.name: RequestFrameTest001
 * @tc.desc: test results of RequestFrame
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosGlTest, RequestFrameTest001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface1 != nullptr);
    sptr<IBufferProducer> bp = cSurface1->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosGl rsSurface(pSurface);
    bool useAFBC = true;
    uint64_t uiTimestamp = 1;
    int32_t width = 1;
    int32_t height = 1;
    rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);

    RenderContext* renderContext1 = RenderContextFactory::GetInstance().CreateEngine();
    rsSurface.SetRenderContext(renderContext1);
    rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);

    rsSurface.mWindow = CreateNativeWindowFromSurface(&rsSurface.producer_);
    rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);

    rsSurface.mEglSurface = EGL_NO_CONTEXT;
    ASSERT_EQ(rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC), nullptr);
}

/**
 * @tc.name: ResetBufferAgeTest001
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosGlTest, ResetBufferAgeTest001, TestSize.Level1)
{
    sptr<Surface> producer1 = nullptr;
    RSSurfaceOhosGl rsSurface(producer1);
    rsSurface.ResetBufferAge();

    RenderContext* renderContext1 = RenderContextFactory::GetInstance().CreateEngine();
    rsSurface.SetRenderContext(renderContext1);
    rsSurface.ResetBufferAge();

    EGLSurface mEglSurface1 = EGL_NO_CONTEXT;
    rsSurface.mEglSurface = mEglSurface1;
    rsSurface.ResetBufferAge();
    ASSERT_EQ(rsSurface.mWindow, nullptr);
}

/**
 * @tc.name: ResetBufferAgeTest002
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosGlTest, ResetBufferAgeTest002, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface2 != nullptr);
    sptr<IBufferProducer> bp = cSurface2->GetProducer();
    sptr<Surface> pSurface2 = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosGl rsSurface(pSurface2);
#ifdef RS_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        rsSurface.SetRenderContext(renderContext);
        bool useAFBC = true;
        int32_t height = 1;
        int32_t width = 1;
        uint64_t uiTimestamp = 1;
        rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);
    }
#endif
    rsSurface.ResetBufferAge();
    EXPECT_EQ(rsSurface.mEglSurface, EGL_NO_SURFACE);
    
    {
        sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("DisplayNode");
        ASSERT_TRUE(cSurface2 != nullptr);
        sptr<IBufferProducer> bp = cSurface2->GetProducer();
        sptr<Surface> pSurface2 = Surface::CreateSurfaceAsProducer(bp);
        RSSurfaceOhosGl rsSurface(pSurface2);
#ifdef RS_ENABLE_GPU
        RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
        if (renderContext) {
            renderContext->InitializeEglContext();
            rsSurface.SetRenderContext(renderContext);
            bool useAFBC = true;
            int32_t height = 1;
            int32_t width = 1;
            uint64_t uiTimestamp = 1;
            rsSurface.RequestFrame(width, height, uiTimestamp, useAFBC);
        }
#endif
        EGLSurface mEglSurface2 = EGL_NO_CONTEXT;
        rsSurface.mEglSurface = mEglSurface2;
        rsSurface.ResetBufferAge();
        EXPECT_EQ(rsSurface.mEglSurface, EGL_NO_SURFACE);
    }
}
} // namespace Rosen
} // namespace OHOS