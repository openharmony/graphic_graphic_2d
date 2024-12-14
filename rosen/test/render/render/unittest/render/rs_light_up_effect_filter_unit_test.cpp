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

#include "render/rs_light_up_effect_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLightUpEffectFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLightUpEffectFilterUnitTest::SetUpTestCase() {}
void RSLightUpEffectFilterUnitTest::TearDownTestCase() {}
void RSLightUpEffectFilterUnitTest::SetUp() {}
void RSLightUpEffectFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestRSLightUpEffectFilterUnit01
 * @tc.desc: Verify function RSLightUpEffectFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestRSLightUpEffectFilterUnit01, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_TRUE(testFilter != nullptr);

    EXPECT_TRUE(testFilter->GetLightUpDegree() == 1.0f);
    testFilter->GetDescription();

    auto testFilter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    auto result = testFilter->Compose(testFilter2);
    EXPECT_TRUE(result != nullptr);

    RSLightUpEffectFilter testFilter3 = RSLightUpEffectFilter(lightUpDegreeValue);
    testFilter3.GetDescription();
}

/**
 * @tc.name: TestRSLightUpEffectFilterUnit02
 * @tc.desc: Verify function RSLightUpEffectFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestRSLightUpEffectFilterUnit02, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_TRUE(testFilter != nullptr);

    auto testFilter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_TRUE(testFilter2 != nullptr);

    auto result = testFilter->Add(testFilter2);
    EXPECT_TRUE(result != nullptr);

    result = testFilter->Sub(testFilter2);
    EXPECT_TRUE(result != nullptr);

    result = testFilter->Multiply(1.0f);
    EXPECT_TRUE(result != nullptr);

    result = testFilter->Negate();
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: TestCreateLightUpEffectFilter01
 * @tc.desc: Verify function CreateLightUpEffectFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestCreateLightUpEffectFilter01, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_NE(testFilter->CreateLightUpEffectFilter(lightUpDegreeValue), nullptr);
}

/**
 * @tc.name: GetLightUpDegreeTest
 * @tc.desc: Verify function GetLightUpDegree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, GetLightUpDegreeTest, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_TRUE(testFilter != nullptr);
    testFilter->Add(testFilter);
    EXPECT_NE(testFilter->GetLightUpDegree(), 0.f);
}

/**
 * @tc.name: TestGetDescription01
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestGetDescription01, TestSize.Level1)
{
    float value = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(value);
    EXPECT_EQ(testFilter->GetDescription(), "RSLightUpEffectFilter light up degree is " + std::to_string(value));
}

/**
 * @tc.name: TestCompose
 * @tc.desc: Verify function Compose
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestCompose, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    auto testFilter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_NE(testFilter->Compose(testFilter2), nullptr);
}

/**
 * @tc.name: TestAdd01
 * @tc.desc: Verify function Add
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestAdd01, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    auto testFilter2 = std::make_shared<RSFilter>();
    EXPECT_NE(testFilter->Add(testFilter2), nullptr);
    testFilter2->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(testFilter->Add(testFilter2), nullptr);
}

/**
 * @tc.name: TestAdd02
 * @tc.desc: Verify function Add
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestAdd02, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_EQ(testFilter->Add(nullptr), testFilter);
}

/**
 * @tc.name: TestSub01
 * @tc.desc: Verify function Sub
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestSub01, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    auto testFilter2 = std::make_shared<RSFilter>();
    EXPECT_NE(testFilter->Sub(testFilter2), nullptr);
    testFilter2->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(testFilter->Sub(testFilter2), nullptr);
}

/**
 * @tc.name: TestSub02
 * @tc.desc: Verify function Sub
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestSub02, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_EQ(testFilter->Sub(nullptr), testFilter);
}

/**
 * @tc.name: TestIsEqual01
 * @tc.desc: Verify function IsEqual
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestIsEqual01, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    auto testFilter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_TRUE(testFilter->IsEqual(testFilter2));
}

/**
 * @tc.name: TestMultiply01
 * @tc.desc: Verify function Multiply
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestMultiply01, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_NE(testFilter->Multiply(1.0f), nullptr);
}

/**
 * @tc.name: TestNegate01
 * @tc.desc: Verify function Negate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestNegate01, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_NE(testFilter->Negate(), nullptr);
}

/**
 * @tc.name: TestIsNearEqual01
 * @tc.desc: Verify function IsNearEqual
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestIsNearEqual01, TestSize.Level1)
{
    float lightUpDegreeValue = 1.0f;
    float lightUpDegreeValue1 = 1.2f;
    float threshold = 0.5f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);

    std::shared_ptr<RSFilter> testFilter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue1);
    EXPECT_TRUE(testFilter->IsNearEqual(testFilter2, threshold));
}

/**
 * @tc.name: TestIsNearZero01
 * @tc.desc: Verify function IsNearZero
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterUnitTest, TestIsNearZero01, TestSize.Level1)
{
    float lightUpDegreeValue = 0.2f;
    float threshold = 0.5f;
    auto testFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegreeValue);
    EXPECT_TRUE(testFilter->IsNearZero(threshold));
}
} // namespace OHOS::Rosen
