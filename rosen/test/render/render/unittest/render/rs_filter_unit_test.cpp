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

#include "render/rs_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFilterUnitTest::SetUpTestCase() {}
void RSFilterUnitTest::TearDownTestCase() {}
void RSFilterUnitTest::SetUp() {}
void RSFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestCreateBlurFilter
 * @tc.desc: Verify function CreateBlurFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestCreateBlurFilter, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);

    float dipScale = 1.0f;
    float ratio = 1.0f;
    auto materialFilter = RSFilter::CreateMaterialFilter(0, dipScale, BLUR_COLOR_MODE::DEFAULT, ratio);
    ASSERT_NE(materialFilter, nullptr);
    
    float radius = 1.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    auto materialFilter2 = RSFilter::CreateMaterialFilter(radius, saturation, brightness, 0, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(materialFilter2, nullptr);
}

/**
 * @tc.name: TestGetDescription
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestGetDescription, TestSize.Level1)
{
    RSFilter testFilter;
    auto filterDescription = testFilter.GetDescription();
    EXPECT_EQ(filterDescription, "RSFilter 0");
}

/**
 * @tc.name: TestCreateMaterialFilter01
 * @tc.desc: Verify function CreateMaterialFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestCreateMaterialFilter01, TestSize.Level1)
{
    float dipScale = 1.0f;
    float ratio = 1.0f;
    auto materialFilter = RSFilter::CreateMaterialFilter(0, dipScale, BLUR_COLOR_MODE::DEFAULT, ratio);
    ASSERT_NE(materialFilter, nullptr);
}

/**
 * @tc.name: TestCreateMaterialFilter02
 * @tc.desc: Verify function CreateMaterialFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestCreateMaterialFilter02, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto lightUpEffectFilter = RSFilter::CreateLightUpEffectFilter(lightUpDegreeValue);
    ASSERT_NE(lightUpEffectFilter, nullptr);
}

/**
 * @tc.name: TestOperator01
 * @tc.desc: Verify function operator+
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestOperator01, TestSize.Level1)
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
 * @tc.name: TestOperator02
 * @tc.desc: Verify function operator-
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestOperator02, TestSize.Level1)
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
 * @tc.name: TestOperator03
 * @tc.desc: Verify function operator*
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestOperator03, TestSize.Level1)
{
    std::shared_ptr<RSFilter> lhs;
    EXPECT_EQ(lhs * 1.0f, nullptr);
    lhs = std::make_shared<RSFilter>();
    EXPECT_EQ(lhs * 1.0f, nullptr);
}

/**
 * @tc.name: TestCreateLightUpEffectFilter
 * @tc.desc: Verify function CreateLightUpEffectFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestCreateLightUpEffectFilter, TestSize.Level1)
{
    float lightUpDegreeValue = 0.5f;
    auto lightUpEffectFilter = RSFilter::CreateLightUpEffectFilter(lightUpDegreeValue);
    ASSERT_NE(lightUpEffectFilter, nullptr);
}

/**
 * @tc.name: TestGetDetailedDescription
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestGetDetailedDescription, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSFilter>();
    EXPECT_EQ(testFilter->GetDetailedDescription(), "RSFilter 0");
}

/**
 * @tc.name: TestIsValid
 * @tc.desc: Verify function IsValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestIsValid, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSFilter>();
    EXPECT_FALSE(testFilter->IsValid());
}

/**
 * @tc.name: TestSetFilterType
 * @tc.desc: Verify function SetFilterType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestSetFilterType, TestSize.Level1)
{
    RSFilter testFilter;
    testFilter.SetFilterType(RSFilter::MATERIAL);
    EXPECT_EQ(testFilter.GetFilterType(), RSFilter::MATERIAL);
}

/**
 * @tc.name: TestRadiusVp2Sigma01
 * @tc.desc: Verify function RadiusVp2Sigma
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestRadiusVp2Sigma01, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSFilter>();
    EXPECT_EQ(rsFilter->RadiusVp2Sigma(0.f, 0.f), 0.0f);
}

/**
 * @tc.name: TestSetSnapshotOutset
 * @tc.desc: Verify function SetSnapshotOutset
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFilterUnitTest, TestSetSnapshotOutset, TestSize.Level1)
{
    RSFilter testFilter;
    testFilter.SetSnapshotOutset(1);
    EXPECT_EQ(testFilter.NeedSnapshotOutset(), 1);
}
} // namespace OHOS::Rosen