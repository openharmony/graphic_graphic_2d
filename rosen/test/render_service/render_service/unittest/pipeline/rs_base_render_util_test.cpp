/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/rs_base_render_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBaseRenderUtilTest::SetUpTestCase() {}
void RSBaseRenderUtilTest::TearDownTestCase() {}
void RSBaseRenderUtilTest::SetUp() {}
void RSBaseRenderUtilTest::TearDown() {}

/*
 * @tc.name: IsBufferValid
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, IsBufferValid, TestSize.Level1)
{
    ASSERT_EQ(false, RSBaseRenderUtil::IsBufferValid(nullptr));
}

/*
 * @tc.name: GetFrameBufferRequestConfig
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, GetFrameBufferRequestConfig, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.width = 480;
    BufferRequestConfig config = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true);
    ASSERT_EQ(static_cast<int32_t>(screenInfo.width), config.width);
}

/*
 * @tc.name: GetSurfaceTransformMatrix
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, GetSurfaceTransformMatrix, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    RectF bounds;
    (void)RSBaseRenderUtil::GetSurfaceTransformMatrix(rsSurfaceRenderNode, bounds);
}

/*
 * @tc.name: GetNodeGravityMatrix
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, GetNodeGravityMatrix, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    sptr<SurfaceBuffer> buffer;
    RectF bounds;
    (void)RSBaseRenderUtil::GetNodeGravityMatrix(rsSurfaceRenderNode, buffer, bounds);
}

/*
 * @tc.name: DropFrameProcess
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, DropFrameProcess, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceHandler surfaceHandler(id);
    RSBaseRenderUtil::DropFrameProcess(surfaceHandler);
}

/*
 * @tc.name: ConsumeAndUpdateBuffer
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, ConsumeAndUpdateBuffer, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceHandler surfaceHandler(id);
    ASSERT_EQ(true, RSBaseRenderUtil::ConsumeAndUpdateBuffer(surfaceHandler));
}

/*
 * @tc.name: ReleaseBuffer
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, ReleaseBuffer, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceHandler surfaceHandler(id);
    ASSERT_EQ(false, RSBaseRenderUtil::ReleaseBuffer(surfaceHandler));
}

/*
 * @tc.name: ConvertBufferToBitmap
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, ConvertBufferToBitmap, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer;
    std::vector<uint8_t> newBuffer;
    ColorGamut dstGamut = ColorGamut::COLOR_GAMUT_SRGB;
    SkBitmap bitmap;
    ASSERT_EQ(false, RSBaseRenderUtil::ConvertBufferToBitmap(buffer, newBuffer, dstGamut, bitmap));
}

/*
 * @tc.name: SetColorFilterModeToPaint
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, SetColorFilterModeToPaint, TestSize.Level1)
{
    ColorFilterMode colorFilterMode = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
    SkPaint paint;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
}

/*
 * @tc.name: IsColorFilterModeValid
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBaseRenderUtilTest, IsColorFilterModeValid, TestSize.Level1)
{
    ColorFilterMode colorFilterMode = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
    ASSERT_EQ(true, RSBaseRenderUtil::IsColorFilterModeValid(colorFilterMode));
}
} // namespace OHOS::Rosen
