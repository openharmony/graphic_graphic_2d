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

#include "render/rs_blur_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSSkiaFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSkiaFilterUnitTest::SetUpTestCase() {}
void RSSkiaFilterUnitTest::TearDownTestCase() {}
void RSSkiaFilterUnitTest::SetUp() {}
void RSSkiaFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestGetImageFiltere01
 * @tc.desc: Verify function GetImageFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaFilterUnitTest, TestGetImageFiltere01, TestSize.Level1)
{
    RSBlurFilter testFilter(0, 1);
    testFilter.GetBrush();

    testFilter.GetImageFilter();
    ASSERT_EQ(testFilter.blurRadiusX_, 0);
    ASSERT_EQ(testFilter.blurRadiusY_, 1);
}

/**
 * @tc.name: TestDrawImageRect01
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaFilterUnitTest, TestDrawImageRect01, TestSize.Level1)
{
    RSBlurFilter testFilter(0, 1);
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    testFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_NE(image, nullptr);
}
} // namespace Rosen
} // namespace OHOS