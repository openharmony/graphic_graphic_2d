/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "render/rs_linear_gradient_blur_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class LinearGradientBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void LinearGradientBlurFilterTest::SetUpTestCase() {}
void LinearGradientBlurFilterTest::TearDownTestCase() {}
void LinearGradientBlurFilterTest::SetUp() {}
void LinearGradientBlurFilterTest::TearDown() {}

/**
 * @tc.name: testInterface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, testInterface, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara = std::make_shared<RSLinearGradientBlurPara>(
        16, fractionStops, GradientDirection::BOTTOM);
    float width = 100;
    float height = 100;
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, width, height);
    EXPECT_TRUE(filter != nullptr);

    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image;

    filter->DrawImageRect(canvas, image, src, dst);
}
} // namespace Rosen
} // namespace OHOS