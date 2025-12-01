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

#include "buffer_handle.h"
#include "render_context/render_context.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "surface_buffer_impl.h"

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
    auto renderContext = RenderContext::Create();
    raster.SetRenderContext(renderContext);
    raster.SetDamageRegion(0, 0, 2, 1);
    ASSERT_EQ(raster.flushConfig_.damage.x, 0);
}

/**
 * @tc.name: GetCanvas001
 * @tc.desc: test results of GetCanvas
 * @tc.type:FUNC
 * @tc.require: issueI9VW90
 */
HWTEST_F(RSSurfaceFrameOhosRasterTest, GetCanvas001, TestSize.Level1)
{
    RSSurfaceFrameOhosRaster raster(1, 1);
    EXPECT_EQ(raster.GetCanvas(), nullptr);

    auto bufferTest = new SurfaceBufferImpl();
    bufferTest->handle_ = nullptr;
    raster.buffer_ = bufferTest;
    EXPECT_EQ(raster.GetCanvas(), nullptr);

    auto handleTest = new BufferHandle();
    handleTest->width = 1;
    handleTest->height = 0;
    bufferTest->handle_ = handleTest;
    EXPECT_EQ(raster.GetCanvas(), nullptr);

    handleTest->width = 1;
    handleTest->height = 1;
    int data = 1;
    handleTest->virAddr = &data;
    EXPECT_NE(raster.GetCanvas(), nullptr);
    if (handleTest) {
        delete handleTest;
        bufferTest->handle_ = nullptr;
        handleTest = nullptr;
    }
    if (bufferTest) {
        delete bufferTest;
        raster.buffer_ = nullptr;
        bufferTest = nullptr;
    }
}

/**
 * @tc.name: GetSurface001
 * @tc.desc: test results of GetSurface
 * @tc.type:FUNC
 * @tc.require: issueI9VW90
 */
HWTEST_F(RSSurfaceFrameOhosRasterTest, GetSurface001, TestSize.Level1)
{
    RSSurfaceFrameOhosRaster raster(1, 1);
    EXPECT_EQ(raster.GetSurface(), nullptr);

    auto bufferTest = new SurfaceBufferImpl();
    bufferTest->handle_ = nullptr;
    raster.buffer_ = bufferTest;
    EXPECT_EQ(raster.GetSurface(), nullptr);

    auto handleTest = new BufferHandle();
    handleTest->width = 1;
    handleTest->height = 0;
    bufferTest->handle_ = handleTest;
    EXPECT_EQ(raster.GetSurface(), nullptr);

    handleTest->width = 1;
    handleTest->height = 1;
    EXPECT_EQ(raster.GetSurface(), nullptr);
    if (handleTest) {
        delete handleTest;
        bufferTest->handle_ = nullptr;
        handleTest = nullptr;
    }
    if (bufferTest) {
        delete bufferTest;
        raster.buffer_ = nullptr;
        bufferTest = nullptr;
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
 * @tc.name: CreateSurfaceTest
 * @tc.desc: test results of CreateSurface
 * @tc.type:FUNC
 * @tc.require: issueI9VW90
 */
HWTEST_F(RSSurfaceFrameOhosRasterTest, CreateSurfaceTest, TestSize.Level1)
{
    RSSurfaceFrameOhosRaster raster(1, 1);
    auto buffer = new SurfaceBufferImpl();
    buffer->handle_ = nullptr;
    raster.buffer_ = buffer;
    raster.CreateSurface();
    EXPECT_EQ(raster.surface_, nullptr);

    auto handle = new BufferHandle();
    handle->width = 1;
    handle->height = 0;
    int data = 1;
    handle->virAddr = &data;
    buffer->handle_ = handle;
    raster.CreateSurface();
    if (handle) {
        delete handle;
        buffer->handle_ = nullptr;
        handle = nullptr;
    }
    if (buffer) {
        delete buffer;
        raster.buffer_ = nullptr;
        buffer = nullptr;
    }
}
} // namespace Rosen
} // namespace OHOS