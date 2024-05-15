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
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceFrameOhosRasterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceFrameOhosRasterTest::SetUpTestCase() {}
void RSSurfaceFrameOhosRasterTest::TearDownTestCase() {}
void RSSurfaceFrameOhosRasterTest::SetUp() {}
void RSSurfaceFrameOhosRasterTest::TearDown() {}

/**
 * @tc.name: SetDamageRegion001
 * @tc.desc: test results of SetDamageRegion
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosRasterTest, SetDamageRegion001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosRaster raster(width, height);
    std::unique_ptr<RenderContext> renderContext = std::make_unique<RenderContext>();
    raster.SetRenderContext(renderContext.get());
    raster.SetDamageRegion(0, 0, 2, 1);
    ASSERT_EQ(raster.flushConfig_.damage.x, 0);
}

/**
 * @tc.name: GetCanvas001
 * @tc.desc: test results of GetCanvas
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosRasterTest, GetCanvas001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosRaster raster(width, height);
    EXPECT_EQ(raster.GetCanvas(), nullptr);
    {
        int32_t width = 1;
        int32_t height = 1;
        RSSurfaceFrameOhosRaster raster(width, height);
        raster.buffer_ = SurfaceBuffer::Create();
        EXPECT_EQ(raster.GetCanvas(), nullptr);
    }
}

/**
 * @tc.name: GetSurface001
 * @tc.desc: test results of GetSurface
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosRasterTest, GetSurface001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosRaster raster(width, height);
    EXPECT_EQ(raster.GetSurface(), nullptr);
    {
        int32_t width = 1;
        int32_t height = 1;
        RSSurfaceFrameOhosRaster raster(width, height);
        raster.buffer_ = SurfaceBuffer::Create();
        EXPECT_EQ(raster.GetSurface(), nullptr);
    }
}

/**
 * @tc.name: GetReleaseFence001
 * @tc.desc: test results of GetReleaseFence
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosRasterTest, GetReleaseFence001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    RSSurfaceFrameOhosRaster raster(width, height);
    ASSERT_EQ(raster.GetReleaseFence(), -1);
}

/**
 * @tc.name: SetReleaseFence001
 * @tc.desc: test results of SetReleaseFence
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosRasterTest, SetReleaseFence001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    int32_t fence = 1;
    RSSurfaceFrameOhosRaster raster(width, height);
    raster.SetReleaseFence(fence);
    ASSERT_EQ(raster.GetReleaseFence(), 1);
}

/**
 * @tc.name: GetSurfaceTest
 * @tc.desc: test results of GetSurface
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceFrameOhosRasterTest, GetSurfaceTest, TestSize.Level1)
{
    int32_t width = 0;
    int32_t height = 0;
    RSSurfaceFrameOhosRaster raster(width, height);
    EXPECT_EQ(raster.GetSurface(), nullptr);
    {
        int32_t width = 1;
        int32_t height = 1;
        RSSurfaceFrameOhosRaster raster(width, height);
        raster.buffer_ = SurfaceBuffer::Create();
        EXPECT_EQ(raster.GetSurface(), nullptr);
    }
}
} // namespace Rosen
} // namespace OHOS