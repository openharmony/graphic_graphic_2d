/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "utils/rect.h"
#include "render/rs_high_performance_visual_engine.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHveFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHveFilterTest::SetUpTestCase() {}
void RSHveFilterTest::TearDownTestCase() {}
void RSHveFilterTest::SetUp() {}
void RSHveFilterTest::TearDown() {}

/**
 * @tc.name: ClearSurfaceNodeInfoTest
 * @tc.desc: Verify function ClearSurfaceNodeInfo
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, ClearSurfaceNodeInfoTest, TestSize.Level1)
{
    HveFilter filter;
    filter.ClearSurfaceNodeInfo();
    EXPECT_TRUE(filter.surfaceNodeInfo_.empty());
}

/**
 * @tc.name: PushSurfaceNodeInfoTest
 * @tc.desc: Verify function PushSurfaceNodeInfo
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, PushSurfaceNodeInfoTest, TestSize.Level1)
{
    HveFilter filter;
    SurfaceNodeInfo surfaceNodeInfo;
    filter.PushSurfaceNodeInfo(surfaceNodeInfo);
    EXPECT_NE(filter.surfaceNodeInfo_.size(), 0);
}

/**
 * @tc.name: GetSurfaceNodeInfoTest
 * @tc.desc: Verify function GetSurfaceNodeInfo
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, GetSurfaceNodeInfoTest, TestSize.Level1)
{
    HveFilter filter;
    EXPECT_TRUE(filter.GetSurfaceNodeInfo().empty());
}

/**
 * @tc.name: GetSurfaceNodeSizeTest
 * @tc.desc: Verify function GetSurfaceNodeSize
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, GetSurfaceNodeSizeTest, TestSize.Level1)
{
    HveFilter filter;
    EXPECT_EQ(filter.GetSurfaceNodeSize(), 0);
}

/**
 * @tc.name: SampleLayerTest
 * @tc.desc: Verify function SampleLayer
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, SampleLayerTest, TestSize.Level1)
{
    HveFilter filter;
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());
    EXPECT_EQ(paintFilterCanvas.GetSurface(), nullptr);
    const Drawing::RectI srcRect = Drawing::RectI(0, 0, 350, 20);
    auto outImage = filter.SampleLayer(paintFilterCanvas, srcRect);
    EXPECT_EQ(outImage, nullptr);

    std::shared_ptr<Drawing::Surface> surfacePtr = std::make_shared<Drawing::Surface>();
    std::shared_ptr<RSPaintFilterCanvas> canvasPtr = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    EXPECT_EQ(paintFilterCanvas.GetSurface(), surfacePtr.get());
    outImage = filter.SampleLayer(paintFilterCanvas, srcRect);
    EXPECT_EQ(outImage, nullptr);
}

} // namespace Rosen
} // namespace OHOS