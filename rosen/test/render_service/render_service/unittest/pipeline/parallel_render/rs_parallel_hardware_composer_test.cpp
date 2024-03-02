/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <memory>
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/parallel_render/rs_parallel_hardware_composer.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSParallelHardwareComposerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParallelHardwareComposerTest::SetUpTestCase() {}
void RSParallelHardwareComposerTest::TearDownTestCase() {}
void RSParallelHardwareComposerTest::SetUp() {}
void RSParallelHardwareComposerTest::TearDown() {}

/**
 * @tc.name: HardwareComposerEnabledTest
 * @tc.desc: Test RSParallelRenderExtTest.HardwareComposerEnabledTest
 * @tc.type: FUNC
 * @tc.require: issueI6COJS
 */
HWTEST_F(RSParallelHardwareComposerTest, HardwareComposerEnabledTest, TestSize.Level1)
{
    auto hardwareComposer = std::make_unique<RSParallelHardwareComposer>();
    hardwareComposer->Init(3);
    Drawing::Canvas tmpCanvas;
    RSPaintFilterCanvas canvas(&tmpCanvas);
    hardwareComposer->ClearTransparentColor(canvas, 1);
    RectF rect = {0.f, 0.f, 8.f, 8.f};
    Vector4f radius = {0.f, 0.f, 0.f, 0.f};
    auto shape1 = std::make_unique<RSParallelSelfDrawingSurfaceShape>(false, rect, radius);
    rect = {10.f, 10.f, 50.f, 50.f};
    radius = {4.f, 4.f, 4.f, 4.f};
    auto shape2 = std::make_unique<RSParallelSelfDrawingSurfaceShape>(true, rect, radius);
    rect = {60.f, 60.f, 80.f, 80.f};
    radius = {0.f, 0.f, 0.f, 0.f};
    auto shape3 = std::make_unique<RSParallelSelfDrawingSurfaceShape>(false, rect, radius);

    hardwareComposer->AddTransparentColorArea(0, std::move(shape1));
    hardwareComposer->AddTransparentColorArea(1, std::move(shape2));
    hardwareComposer->AddTransparentColorArea(2, std::move(shape3));

    hardwareComposer->ClearTransparentColor(canvas, 1);
    hardwareComposer->ClearTransparentColor(canvas, 2);
    
    rect = {200.f, 200.f, 300.f, 300.f};
    radius = {0.f, 0.f, 0.f, 0.f};
    auto shape4 = std::make_unique<RSParallelSelfDrawingSurfaceShape>(false, rect, radius);

    hardwareComposer->AddTransparentColorArea(100, std::move(shape4));
}

/**
 * @tc.name: HardwareComposerGetRRectTest
 * @tc.desc: Test RSParallelRenderExtTest.HardwareComposerGetRRectTest
 * @tc.type: FUNC
 * @tc.require: issueI6COJS
 */
HWTEST_F(RSParallelHardwareComposerTest, HardwareComposerGetRRectTest, TestSize.Level1)
{
    RectF rect = {0.f, 0.f, 8.f, 8.f};
    Vector4f radius = {0.f, 0.f, 0.f, 0.f};
    auto shape = std::make_unique<RSParallelSelfDrawingSurfaceShape>(false, rect, radius);
    Drawing::RoundRect rrect = shape->GetRRect();
    EXPECT_EQ(rrect.GetRect().right_, 8.f);
}
} // namespace OHOS::Rosen