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

#include "render/rs_light_up_effect_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLightUpEffectFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLightUpEffectFilterTest::SetUpTestCase() {}
void RSLightUpEffectFilterTest::TearDownTestCase() {}
void RSLightUpEffectFilterTest::SetUp() {}
void RSLightUpEffectFilterTest::TearDown() {}

/**
 * @tc.name: RSLightUpEffectFilterTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, RSLightUpEffectFilterTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);

    EXPECT_TRUE(filter->GetLightUpDegree() == 1.0f);
    filter->GetDescription();

    auto filter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto result = filter->Compose(filter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: RSLightUpEffectFilterTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, RSLightUpEffectFilterTest002, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);

    auto filter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter2 != nullptr);

    auto result = filter->Add(filter2);
    EXPECT_TRUE(result != nullptr);

    result = filter->Sub(filter2);
    EXPECT_TRUE(result != nullptr);

    result = filter->Multiply(1.0f);
    EXPECT_TRUE(result != nullptr);

    result = filter->Negate();
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: CreateLightUpEffectFilterTest001
 * @tc.desc: Verify function CreateLightUpEffectFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, CreateLightUpEffectFilterTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->CreateLightUpEffectFilter(lightUpDegree), nullptr);
}

/**
 * @tc.name: GetLightUpDegreeTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetLightUpDegreeTest, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);
    filter->Add(filter);
    EXPECT_NE(filter->GetLightUpDegree(), 0.f);
}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_EQ(filter->GetDescription(), "RSLightUpEffectFilter light up degree is " + std::to_string(lightUpDegree));
}

/**
 * @tc.name: ComposeTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, ComposeTest, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto filter_ = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->Compose(filter_), nullptr);
}

/**
 * @tc.name: AddTest001
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTest001ionTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilter::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: SubTest001
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilter::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: MultiplyTest001
 * @tc.desc: Verify function Multiply
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, MultiplyTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->Multiply(1.0f), nullptr);
}

/**
 * @tc.name: NegateTest001
 * @tc.desc: Verify function Negate
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, NegateTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->Negate(), nullptr);
}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, IsNearEqual001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    float lightUpDegree1 = 1.2f;
    float threshold = 0.5f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);

    std::shared_ptr<RSFilter> other1 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree1);
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
}

/**
 * @tc.name: IsNearZero001
 * @tc.desc: Verify function IsNearZero
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, IsNearZero001, TestSize.Level1)
{
    float lightUpDegree = 0.2f;
    float threshold = 0.5f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter->IsNearZero(threshold));
}
} // namespace OHOS::Rosen
