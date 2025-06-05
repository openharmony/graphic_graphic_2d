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

    RSLightUpEffectFilter rsLightUpEffectFilter = RSLightUpEffectFilter(lightUpDegree);
    rsLightUpEffectFilter.GetDescription();
}

/**
 * @tc.name: RSLightUpEffectFilterTestFLT_MAX
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, RSLightUpEffectFilterTestFLT_MAX, TestSize.Level1)
{
    float lightUpDegree = FLT_MAX;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);

    EXPECT_TRUE(filter->GetLightUpDegree() == FLT_MAX);
    filter->GetDescription();

    auto filter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto result = filter->Compose(filter2);
    EXPECT_TRUE(result != nullptr);

    RSLightUpEffectFilter rsLightUpEffectFilter = RSLightUpEffectFilter(lightUpDegree);
    rsLightUpEffectFilter.GetDescription();
}

/**
 * @tc.name: RSLightUpEffectFilterTestFLT_MIN
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, RSLightUpEffectFilterTestFLT_MIN, TestSize.Level1)
{
    float lightUpDegree = FLT_MIN;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);

    EXPECT_TRUE(filter->GetLightUpDegree() == FLT_MIN);
    filter->GetDescription();

    auto filter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto result = filter->Compose(filter2);
    EXPECT_TRUE(result != nullptr);

    RSLightUpEffectFilter rsLightUpEffectFilter = RSLightUpEffectFilter(lightUpDegree);
    rsLightUpEffectFilter.GetDescription();
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
} // namespace OHOS::Rosen
