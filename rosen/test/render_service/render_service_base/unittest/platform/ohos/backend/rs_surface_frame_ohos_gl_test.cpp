/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "render_context/render_context.h"
#include "render_context/new_render_context/render_context_gl.h"

#include "platform/ohos/backend/rs_surface_frame_ohos_gl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceFrameOhosGlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceFrameOhosGlTest::SetUpTestCase() {}
void RSSurfaceFrameOhosGlTest::TearDownTestCase() {}
void RSSurfaceFrameOhosGlTest::SetUp() {}
void RSSurfaceFrameOhosGlTest::TearDown() {}

/**
 * @tc.name: SetDamageRegion001
 * @tc.desc: test results of SetDamageRegion
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, SetDamageRegion001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
    auto renderContext = std::make_shared<RenderContextGL>();
    EXPECT_NE(renderContext, nullptr);
    rsSurface.SetRenderContext(renderContext);
    rsSurface.SetDamageRegion(0, 0, 1, 1);
}

/**
 * @tc.name: GetCanvas001
 * @tc.desc: test results of GetCanvas
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetCanvas001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
#ifdef RS_ENABLE_GPU
    auto renderContext = std::make_shared<RenderContextGL>();
    EXPECT_NE(renderContext, nullptr);
    if (renderContext) {
        renderContext->Init();
        rsSurface.SetRenderContext(renderContext);
        EXPECT_NE(rsSurface.GetCanvas(), nullptr);
    }
#endif
}

/**
 * @tc.name: GetCanvas002
 * @tc.desc: test results of GetCanvas
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetCanvas002, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
    auto renderContext = std::make_shared<RenderContextGL>();
    ASSERT_NE(renderContext, nullptr);
    rsSurface.SetRenderContext(renderContext);
    ASSERT_NE(rsSurface.GetCanvas(), nullptr);
}

/**
 * @tc.name: GetSurface001
 * @tc.desc: test results of GetSurface
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetSurface001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
#ifdef RS_ENABLE_GPU
    auto renderContext = std::make_shared<RenderContextGL>();
    EXPECT_NE(renderContext, nullptr);
    if (renderContext) {
        renderContext->Init();
        rsSurface.SetRenderContext(renderContext);
        EXPECT_NE(rsSurface.GetSurface(), nullptr);
    }
#endif
}

/**
 * @tc.name: GetSurface002
 * @tc.desc: test results of GetSurface
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetSurface002, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
    auto context = std::make_shared<RenderContextGL>();
    ASSERT_NE(context, nullptr);
    rsSurface.SetRenderContext(context);
    ASSERT_NE(rsSurface.GetSurface(), nullptr);
    ASSERT_NE(rsSurface.GetSurface(), nullptr);
}

/**
 * @tc.name: GetReleaseFence001
 * @tc.desc: test results of GetReleaseFence
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetReleaseFence001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
    ASSERT_EQ(rsSurface.GetReleaseFence(), 0);
}

/**
 * @tc.name: SetReleaseFence001
 * @tc.desc: test results of SetReleaseFence
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, SetReleaseFence001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    int32_t fence = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
    rsSurface.SetReleaseFence(fence);
    EXPECT_EQ(rsSurface.GetReleaseFence(), 1);
}

/**
 * @tc.name: GetBufferAge001
 * @tc.desc: test results of GetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetBufferAge001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
    auto context = std::make_shared<RenderContextGL>();
    ASSERT_NE(context, nullptr);
    rsSurface.SetRenderContext(context);
    ASSERT_EQ(rsSurface.GetBufferAge(), -1);
}

/**
 * @tc.name: GetCanvasAndGetSurface001
 * @tc.desc: test results of GetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9VW90
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetCanvasAndGetSurface001, TestSize.Level1)
{
    RSSurfaceFrameOhosGl rsSurfaceTest1(1, 1);
    rsSurfaceTest1.surface_ = nullptr;
    auto renderContextTest1 = std::make_shared<RenderContextGL>();
    std::shared_ptr<Drawing::GPUContext> drGPUContextTest1 = std::make_shared<Drawing::GPUContext>();
    EXPECT_NE(drGPUContextTest1, nullptr);
    renderContextTest1->SetDrGPUContext(drGPUContextTest1);
    rsSurfaceTest1.renderContext_ = renderContextTest1;
    EXPECT_EQ(rsSurfaceTest1.GetCanvas(), NULL);
    std::shared_ptr<Drawing::Surface> surface = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surface, nullptr);
    rsSurfaceTest1.surface_ = surface;
    EXPECT_EQ(rsSurfaceTest1.GetCanvas(), nullptr);

    RSSurfaceFrameOhosGl rsSurfaceTest2(1, 1);
    rsSurfaceTest2.surface_ = nullptr;
    auto renderContextTest2 = std::make_shared<RenderContextGL>();
    std::shared_ptr<Drawing::GPUContext> drGPUContextTest2 = std::make_shared<Drawing::GPUContext>();
    EXPECT_NE(drGPUContextTest2, nullptr);
    renderContextTest2->SetDrGPUContext(drGPUContextTest2);
    rsSurfaceTest2.renderContext_ = renderContextTest2;
    EXPECT_EQ(rsSurfaceTest2.GetSurface(), nullptr);
    rsSurfaceTest2.surface_ = surface;
    EXPECT_NE(rsSurfaceTest2.GetSurface(), nullptr);
}
} // namespace Rosen
} // namespace OHOS