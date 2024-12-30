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
#include "surface_buffer_impl.h"

#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceOhosRasterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceOhosRasterTest::SetUpTestCase() {}
void RSSurfaceOhosRasterTest::TearDownTestCase() {}
void RSSurfaceOhosRasterTest::SetUp() {}
void RSSurfaceOhosRasterTest::TearDown() {}

/**
 * @tc.name: SetSurfaceBufferUsageTest001
 * @tc.desc: test results of SetSurfaceBufferUsage
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosRasterTest, SetSurfaceBufferUsageTest001, TestSize.Level1)
{
    sptr<Surface> pSurface1 = nullptr;
    RSSurfaceOhosRaster raster(pSurface1);
    EXPECT_FALSE(raster.IsValid());
    uint64_t usage = 0;
    raster.SetSurfaceBufferUsage(usage);
    ASSERT_EQ(raster.bufferUsage_, 0);
}

/**
 * @tc.name: FlushFrameTest001
 * @tc.desc: test results of FlushFrame
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosRasterTest, FlushFrameTest001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface1->GetProducer();
    sptr<Surface> pSurface1 = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface1);
    uint64_t uiTimestamp = 1;

    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    raster.SetUiTimeStamp(frame, uiTimestamp);
    EXPECT_FALSE(raster.FlushFrame(frame, uiTimestamp));

    int32_t height = 1;
    int32_t width = 1;
    frame = std::make_unique<RSSurfaceFrameOhosRaster>(width, height);
    auto frameOhosRaster1 =  static_cast<RSSurfaceFrameOhosRaster *>(frame.get());
    frameOhosRaster1->buffer_ = new SurfaceBufferImpl();
    raster.SetUiTimeStamp(frame, uiTimestamp);
    EXPECT_FALSE(raster.FlushFrame(frame, uiTimestamp));
}

/**
 * @tc.name: RequestFrameTest001
 * @tc.desc: test results of RequestFrame
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosRasterTest, RequestFrameTest001, TestSize.Level1)
{
    sptr<Surface> pSurface1 = nullptr;
    RSSurfaceOhosRaster raster(pSurface1);
    EXPECT_FALSE(raster.IsValid());

    bool useAFBC = true;
    int32_t height = 1;
    int32_t width = 1;
    uint64_t uiTimestamp = 1;
    EXPECT_EQ(raster.RequestFrame(width, height, uiTimestamp, useAFBC), nullptr);
    raster.ClearBuffer();
}

/**
 * @tc.name: RequestFrameTest002
 * @tc.desc: test results of RequestFrame
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosRasterTest, RequestFrameTest002, TestSize.Level1)
{
    sptr<IConsumerSurface> consumer2 = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer2->GetProducer();
    sptr<Surface> surface2 = Surface::CreateSurfaceAsProducer(producer);
    RSSurfaceOhosRaster raster(surface2);
    EXPECT_TRUE(raster.IsValid());

    bool useAFBC = true;
    int32_t height = 1;
    int32_t width = 1;
    uint64_t uiTimestamp = 1;
    EXPECT_EQ(raster.RequestFrame(width, height, uiTimestamp, useAFBC), nullptr);
    raster.ClearBuffer();
}

/**
 * @tc.name: ClearBufferTest001
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosRasterTest, ClearBufferTest001, TestSize.Level1)
{
    sptr<Surface> pSurface1 = nullptr;
    RSSurfaceOhosRaster raster(pSurface1);
    raster.ClearBuffer();
    EXPECT_EQ(raster.producer_, nullptr);

    {
        sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("DisplayNode");
        ASSERT_TRUE(cSurface1 != nullptr);
        sptr<IBufferProducer> bp = cSurface1->GetProducer();
        sptr<Surface> pSurface1 = Surface::CreateSurfaceAsProducer(bp);
        RSSurfaceOhosRaster raster(pSurface1);
        raster.ClearBuffer();
        ASSERT_TRUE(raster.IsValid());
    }
}

/**
 * @tc.name: SetSurfacePixelFormatTest001
 * @tc.desc: test results of SetSurfacePixelFormat
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosRasterTest, SetSurfacePixelFormatTest001, TestSize.Level1)
{
    sptr<Surface> pSurface1 = nullptr;
    RSSurfaceOhosRaster raster(pSurface1);
    int32_t pixelFormat1 = 1;
    raster.SetSurfacePixelFormat(pixelFormat1);
    ASSERT_EQ(raster.pixelFormat_, pixelFormat1);
}

/**
 * @tc.name: ResetBufferAgeTest001
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9K9FU
 */
HWTEST_F(RSSurfaceOhosRasterTest, ResetBufferAgeTest001, TestSize.Level1)
{
    sptr<Surface> pSurface1 = nullptr;
    RSSurfaceOhosRaster raster(pSurface1);
    EXPECT_FALSE(raster.IsValid());
    raster.ResetBufferAge();
}
} // namespace Rosen
} // namespace OHOS