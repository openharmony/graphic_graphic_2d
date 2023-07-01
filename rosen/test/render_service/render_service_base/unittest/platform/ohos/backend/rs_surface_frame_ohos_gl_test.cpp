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

#include "render_context/render_context.h"

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
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, SetDamageRegion001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
    std::unique_ptr<RenderContext> renderContext = std::make_unique<RenderContext>();
    rsSurface.SetRenderContext(renderContext.get());
    rsSurface.SetDamageRegion(0, 0, 1, 1);
}

/**
 * @tc.name: GetCanvas001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetCanvas001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
#ifdef ACE_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        rsSurface.SetRenderContext(renderContext);
        rsSurface.GetCanvas();
    }
#endif
}

/**
 * @tc.name: GetCanvas002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetCanvas002, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
#ifdef ACE_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        rsSurface.SetRenderContext(renderContext);
        rsSurface.GetCanvas();
        rsSurface.GetCanvas();
    }
#endif
}

/**
 * @tc.name: GetSurface001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetSurface001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
#ifdef ACE_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        rsSurface.SetRenderContext(renderContext);
        rsSurface.GetSurface();
    }
#endif
}

/**
 * @tc.name: GetSurface002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetSurface002, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
#ifdef ACE_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        rsSurface.SetRenderContext(renderContext);
        rsSurface.GetSurface();
        rsSurface.GetSurface();
    }
#endif
}

/**
 * @tc.name: GetReleaseFence001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, GetReleaseFence001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
    rsSurface.GetReleaseFence();
}

/**
 * @tc.name: SetReleaseFence001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceFrameOhosGlTest, SetReleaseFence001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    int32_t fence = 1;
    RSSurfaceFrameOhosGl rsSurface(width, height);
    rsSurface.SetReleaseFence(fence);
}
} // namespace Rosen
} // namespace OHOS