/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ui_effect/filter/include/filter_blur_para.h"
#include "ui_effect/property/include/rs_ui_blur_filter.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
class RSUIBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void RSUIBlurFilterTest::SetUpTestCase() {}
void RSUIBlurFilterTest::TearDownTestCase() {}
void RSUIBlurFilterTest::SetUp() {}
void RSUIBlurFilterTest::TearDown() {}

/**
 * @tc.name: Equal001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBlurFilterTest, Equal001, TestSize.Level1)
{
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);

    auto rsUIBlurFilterPara1 = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara1->SetBlurPara(filterBlurPara);

    auto rsUIBlurFilterPara2 = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara2->SetBlurPara(filterBlurPara);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara2);
    
    EXPECT_TRUE(rsUIBlurFilterPara1->Equals(rsUIFilterParaBase));
}

/**
 * @tc.name: SetProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBlurFilterTest, SetProperty001, TestSize.Level1)
{
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);

    auto rsUIBlurFilterPara1 = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIBlurFilterPara2 = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara2->SetBlurPara(filterBlurPara);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara2);
    rsUIBlurFilterPara1->SetProperty(rsUIFilterParaBase);

    auto iter = rsUIBlurFilterPara1->properties_.find(RSUIFilterType::BLUR_RADIUS_X);
    ASSERT_NE(iter, rsUIBlurFilterPara1->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), radius);
}

/**
 * @tc.name: SetBlurPara001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBlurFilterTest, SetBlurPara001, TestSize.Level1)
{
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);

    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);

    auto iter = rsUIBlurFilterPara->properties_.find(RSUIFilterType::BLUR_RADIUS_X);
    ASSERT_NE(iter, rsUIBlurFilterPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), radius);
}

/**
 * @tc.name: SetBlurRadiusX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBlurFilterTest, SetBlurRadiusX001, TestSize.Level1)
{
    float radiusX = 1.0f;
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetRadiusX(radiusX);

    auto iter = rsUIBlurFilterPara->properties_.find(RSUIFilterType::BLUR_RADIUS_X);
    ASSERT_NE(iter, rsUIBlurFilterPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), radiusX);
}

/**
 * @tc.name: SetBlurRadiusY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBlurFilterTest, SetBlurRadiusY001, TestSize.Level1)
{
    float radiusY = 1.0f;
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetRadiusY(radiusY);

    auto iter = rsUIBlurFilterPara->properties_.find(RSUIFilterType::BLUR_RADIUS_Y);
    ASSERT_NE(iter, rsUIBlurFilterPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), radiusY);
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBlurFilterTest, CreateRSRenderFilter001, TestSize.Level1)
{
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);

    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);

    auto rsRenderFilterParaBase = rsUIBlurFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);
}

/**
 * @tc.name: GetLeafProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBlurFilterTest, GetLeafProperties001, TestSize.Level1)
{
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);

    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);

    auto rsPropertyBaseVector = rsUIBlurFilterPara->GetLeafProperties();
    EXPECT_NE(rsPropertyBaseVector.size(), 0);
}
} // namespace OHOS::Rosen