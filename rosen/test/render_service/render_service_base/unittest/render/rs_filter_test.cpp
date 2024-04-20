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

#include "render/rs_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFilterTest::SetUpTestCase() {}
void RSFilterTest::TearDownTestCase() {}
void RSFilterTest::SetUp() {}
void RSFilterTest::TearDown() {}

/**
 * @tc.name: CreateBlurFilter
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSFilterTest, CreateBlurFilter, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto filter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);

    float dipScale = 1.0f;
    float ratio = 1.0f;
    auto filter2 = RSFilter::CreateMaterialFilter(0, dipScale, BLUR_COLOR_MODE::DEFAULT, ratio);
    ASSERT_NE(filter2, nullptr);
    
    float radius = 1.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    auto filter3 = RSFilter::CreateMaterialFilter(radius, saturation, brightness, 0, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(filter3, nullptr);
}

/**
 * @tc.name: GetDescriptionTest
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSFilterTest, GetDescriptionTest, TestSize.Level1)
{
    RSFilter rSFilter;
    auto filter = rSFilter.GetDescription();
    EXPECT_EQ(filter, "RSFilter 0");
}

/**
 * @tc.name: CreateMaterialFilterTest001
 * @tc.desc: Verify function CreateMaterialFilter
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSFilterTest, CreateMaterialFilterTest001, TestSize.Level1)
{
    float dipScale = 1.0f;
    float ratio = 1.0f;
    auto filter = RSFilter::CreateMaterialFilter(0, dipScale, BLUR_COLOR_MODE::DEFAULT, ratio);
    ASSERT_NE(filter, nullptr);
}

/**
 * @tc.name: CreateMaterialFilterTest002
 * @tc.desc: Verify function CreateMaterialFilter
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSFilterTest, CreateMaterialFilterTest002, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = RSFilter::CreateLightUpEffectFilter(lightUpDegree);
    ASSERT_NE(filter, nullptr);
}

/**
 * @tc.name: operatorTest001
 * @tc.desc: Verify function operator+
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSFilterTest, operatorTest001, TestSize.Level1)
{
    std::shared_ptr<RSFilter> lhs;
    std::shared_ptr<RSFilter> rhs;
    EXPECT_EQ(lhs + rhs, rhs);
    lhs = std::make_shared<RSFilter>();
    EXPECT_EQ(lhs + rhs, lhs);
    rhs = std::make_shared<RSFilter>();
    EXPECT_EQ(lhs + rhs, nullptr);
}

/**
 * @tc.name: operatorTest002
 * @tc.desc: Verify function operator-
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSFilterTest, operatorTest002, TestSize.Level1)
{
    std::shared_ptr<RSFilter> lhs;
    std::shared_ptr<RSFilter> rhs;
    std::shared_ptr<RSFilter> lhss;
    lhs = std::make_shared<RSFilter>();
    EXPECT_EQ(lhs - rhs, lhs);
    rhs = std::make_shared<RSFilter>();
    EXPECT_EQ(lhss - rhs, lhss);
    EXPECT_EQ(lhs - rhs, nullptr);
}

/**
 * @tc.name: operatorTest003
 * @tc.desc: Verify function operator*
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSFilterTest, operatorTest003, TestSize.Level1)
{
    std::shared_ptr<RSFilter> lhs;
    EXPECT_EQ(lhs * 1.0f, nullptr);
    lhs = std::make_shared<RSFilter>();
    EXPECT_EQ(lhs * 1.0f, nullptr);
}
} // namespace OHOS::Rosen