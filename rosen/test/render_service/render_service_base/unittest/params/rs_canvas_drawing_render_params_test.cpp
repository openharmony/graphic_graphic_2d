/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "params/rs_canvas_drawing_render_params.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSCanvasDrawingRenderParamsTest::SetUpTestCase() {}
void RSCanvasDrawingRenderParamsTest::TearDownTestCase() {}
void RSCanvasDrawingRenderParamsTest::SetUp() {}
void RSCanvasDrawingRenderParamsTest::TearDown() {}
void RSCanvasDrawingRenderParamsTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSync_CanvasDrawingSurfaceChange_001
 * @tc.desc: Test OnSync with CanvasDrawing surface change
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSCanvasDrawingRenderParamsTest, OnSync_CanvasDrawingSurfaceChange_001, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    auto source = std::make_unique<RSCanvasDrawingRenderParams>(id);
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSCanvasDrawingRenderParams>(id + 1);

    source->canvasDrawingNodeSurfaceChanged_ = true;
    source->surfaceParams_.width = 2;
    source->surfaceParams_.height = 3;
    source->surfaceParams_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    source->canvasDrawingResetSurfaceIndex_ = 1;

    source->OnSync(target);
    auto targetParams = static_cast<RSCanvasDrawingRenderParams*>(target.get());
    EXPECT_EQ(targetParams->canvasDrawingNodeSurfaceChanged_, true);
    EXPECT_EQ(targetParams->surfaceParams_.width, 2);
    EXPECT_EQ(targetParams->surfaceParams_.height, 3);
    EXPECT_EQ(targetParams->surfaceParams_.colorSpace, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    EXPECT_EQ(targetParams->canvasDrawingResetSurfaceIndex_, 1);
    EXPECT_FALSE(source->canvasDrawingNodeSurfaceChanged_);
}

/**
 * @tc.name: OnSync_CanvasDrawingSurfaceChange_002
 * @tc.desc: Test OnSync without CanvasDrawing surface change
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSCanvasDrawingRenderParamsTest, OnSync_CanvasDrawingSurfaceChange_002, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    auto source = std::make_unique<RSCanvasDrawingRenderParams>(id);
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSCanvasDrawingRenderParams>(id + 1);

    source->canvasDrawingNodeSurfaceChanged_ = false;
    source->surfaceParams_.width = 2;
    source->surfaceParams_.height = 3;

    source->OnSync(target);
    auto targetParams = static_cast<RSCanvasDrawingRenderParams*>(target.get());
    EXPECT_FALSE(targetParams->canvasDrawingNodeSurfaceChanged_);
    EXPECT_EQ(targetParams->surfaceParams_.width, 0);
    EXPECT_EQ(targetParams->surfaceParams_.height, 0);

    source->canvasDrawingNodeSurfaceChanged_ = true;
    source->paramsType_ = RSRenderParamsType::RS_PARAM_OWNED_BY_DRAWABLE;
    source->OnSync(target);
    EXPECT_TRUE(targetParams->canvasDrawingNodeSurfaceChanged_);
}

/**
 * @tc.name: GetCanvasDrawingSurfaceChanged_001
 * @tc.desc: Test function GetCanvasDrawingSurfaceChanged
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSCanvasDrawingRenderParamsTest, GetCanvasDrawingSurfaceChanged_001, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    auto renderParams = std::make_unique<RSCanvasDrawingRenderParams>(id);
    renderParams->canvasDrawingNodeSurfaceChanged_ = false;
    EXPECT_FALSE(renderParams->GetCanvasDrawingSurfaceChanged());
}

/**
 * @tc.name: GetCanvasDrawingSurfaceParams_001
 * @tc.desc: Test function GetCanvasDrawingSurfaceParams
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSCanvasDrawingRenderParamsTest, GetCanvasDrawingSurfaceParams_001, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    auto renderParams = std::make_unique<RSCanvasDrawingRenderParams>(id);
    renderParams->surfaceParams_.height = 2;
    renderParams->surfaceParams_.width = 3;
    auto surfaceParams = renderParams->GetCanvasDrawingSurfaceParams();
    EXPECT_EQ(surfaceParams.height, renderParams->surfaceParams_.height);
    EXPECT_EQ(surfaceParams.width, renderParams->surfaceParams_.width);
}

/**
 * @tc.name: SetCanvasDrawingResetSurfaceIndexTest
 * @tc.desc: Test SetCanvasDrawingResetSurfaceIndex
 * @tc.type: FUNC
 * @tc.require:#issueICF7P6
 */
HWTEST_F(RSCanvasDrawingRenderParamsTest, SetCanvasDrawingResetSurfaceIndexTest, TestSize.Level2)
{
    auto renderParams = std::make_unique<RSCanvasDrawingRenderParams>(1);
    renderParams->canvasDrawingResetSurfaceIndex_ = 1;
    renderParams->SetCanvasDrawingResetSurfaceIndex(1);
    ASSERT_FALSE(renderParams->NeedSync());
    renderParams->SetCanvasDrawingResetSurfaceIndex(2);
    ASSERT_TRUE(renderParams->NeedSync());
}
} // namespace OHOS::Rosen