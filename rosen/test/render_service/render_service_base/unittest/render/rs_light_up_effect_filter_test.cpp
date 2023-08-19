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
