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
 * @tc.name: SetSurfaceBufferUsage001
 * @tc.desc: test results of SetSurfaceBufferUsage
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosRasterTest, SetSurfaceBufferUsage001, TestSize.Level1)
{
    sptr<Surface> pSurface = nullptr;
    RSSurfaceOhosRaster raster(pSurface);
    uint64_t usage = 0;
    EXPECT_FALSE(raster.IsValid());
    raster.SetSurfaceBufferUsage(usage);
    ASSERT_EQ(raster.bufferUsage_, 0);
}

/**
 * @tc.name: RequestFrame001
 * @tc.desc: test results of RequestFrame
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosRasterTest, RequestFrame001, TestSize.Level1)
{
    sptr<Surface> pSurface = nullptr;
    RSSurfaceOhosRaster raster(pSurface);
    int32_t width = 1;
    int32_t height = 1;
    uint64_t uiTimestamp = 1;
    bool useAFBC = true;
    EXPECT_FALSE(raster.IsValid());
    EXPECT_EQ(raster.RequestFrame(width, height, uiTimestamp, useAFBC), nullptr);
    raster.ClearBuffer();
}

/**
 * @tc.name: RequestFrame002
 * @tc.desc: test results of RequestFrame
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosRasterTest, RequestFrame002, TestSize.Level1)
{
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    RSSurfaceOhosRaster raster(surface);
    int32_t width = 1;
    int32_t height = 1;
    uint64_t uiTimestamp = 1;
    bool useAFBC = true;
    EXPECT_TRUE(raster.IsValid());
    EXPECT_EQ(raster.RequestFrame(width, height, uiTimestamp, useAFBC), nullptr);
    raster.ClearBuffer();
}

/**
 * @tc.name: FlushFrame001
 * @tc.desc: test results of FlushFrame
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosRasterTest, FlushFrame001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface);
    uint64_t uiTimestamp = 1;

    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    raster.SetUiTimeStamp(frame, uiTimestamp);
    EXPECT_FALSE(raster.FlushFrame(frame, uiTimestamp));

    int32_t width = 1;
    int32_t height = 1;
    frame = std::make_unique<RSSurfaceFrameOhosRaster>(width, height);
    raster.SetUiTimeStamp(frame, uiTimestamp);
    EXPECT_FALSE(raster.FlushFrame(frame, uiTimestamp));
}

/**
 * @tc.name: ResetBufferAge001
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosRasterTest, ResetBufferAge001, TestSize.Level1)
{
    sptr<Surface> pSurface = nullptr;
    RSSurfaceOhosRaster raster(pSurface);
    EXPECT_FALSE(raster.IsValid());
    raster.ResetBufferAge();
}

/**
 * @tc.name: ClearBuffer001
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosRasterTest, ClearBuffer001, TestSize.Level1)
{
    sptr<Surface> pSurface = nullptr;
    RSSurfaceOhosRaster raster(pSurface);
    raster.ClearBuffer();
    EXPECT_EQ(raster.producer_, nullptr);

    {
        sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
        ASSERT_TRUE(cSurface != nullptr);
        sptr<IBufferProducer> bp = cSurface->GetProducer();
        sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
        RSSurfaceOhosRaster raster(pSurface);
        raster.ClearBuffer();
        ASSERT_TRUE(raster.IsValid());
    }
}

/**
 * @tc.name: SetSurfacePixelFormat001
 * @tc.desc: test results of SetSurfacePixelFormat
 * @tc.type:FUNC
 * @tc.require: SR20240111372440
 */
HWTEST_F(RSSurfaceOhosRasterTest, SetSurfacePixelFormat001, TestSize.Level1)
{
    sptr<Surface> pSurface = nullptr;
    RSSurfaceOhosRaster raster(pSurface);
    int32_t pixelFormat = 1;
    raster.SetSurfacePixelFormat(pixelFormat);
    ASSERT_EQ(raster.pixelFormat_, pixelFormat);
}
} // namespace Rosen
} // namespace OHOS