/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

namespace OHOS::Rosen {
class RSBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBlurFilterTest::SetUpTestCase() {}
void RSBlurFilterTest::TearDownTestCase() {}
void RSBlurFilterTest::SetUp() {}
void RSBlurFilterTest::TearDown() {}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);

    float blurRadiusX1 = 1.2f;
    std::shared_ptr<RSFilter> other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);

    float threshold = 1.0f;
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
}

/**
 * @tc.name: IsNearZero001
 * @tc.desc: Verify function IsNearZero
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsNearZero001, TestSize.Level1)
{
    float blurRadiusX = 0.2f;
    float blurRadiusY = 0.2f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);

    float threshold = 0.5f;
    EXPECT_TRUE(filter->IsNearZero(threshold));
}
} // namespace OHOS::Rosen
