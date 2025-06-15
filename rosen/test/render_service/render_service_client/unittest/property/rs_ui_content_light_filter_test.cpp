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
#include "ui_effect/filter/include/filter_content_light_para.h"
#include "ui_effect/property/include/rs_ui_blur_filter.h"
#include "ui_effect/property/include/rs_ui_content_light_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIContentLightFilterTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;
};

void RSUIContentLightFilterTest::SetUpTestCase() {}

void RSUIContentLightFilterTest::TearDownTestCase() {}

void RSUIContentLightFilterTest::SetUp() {}

void RSUIContentLightFilterTest::TearDown() {}

/**
 * @tc.name: Equals001
 * @tc.desc: Test function of Equals
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContentLightFilterTest, Equals001, TestSize.Level1)
{
    auto contentLightFilterPara1 = std::make_shared<RSUIContentLightFilterPara>();
    auto contentLightFilterPara2 = std::make_shared<RSUIContentLightFilterPara>();
    auto rsUIFilterParaBase1 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(contentLightFilterPara2);

    EXPECT_TRUE(contentLightFilterPara1->Equals(rsUIFilterParaBase1));
    EXPECT_FALSE(contentLightFilterPara1->Equals(nullptr));

    auto blurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase2 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(blurFilterPara);
    EXPECT_FALSE(contentLightFilterPara1->Equals(rsUIFilterParaBase2));
}

/**
 * @tc.name: SetProperty001
 * @tc.desc: Test function of SetProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContentLightFilterTest, SetProperty001, TestSize.Level1)
{
    auto contentLightFilterPara1 = std::make_shared<RSUIContentLightFilterPara>();
    contentLightFilterPara1->SetProperty(nullptr);

    auto blurFilterPara1 = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase1 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(blurFilterPara1);
    contentLightFilterPara1->SetProperty(rsUIFilterParaBase1);

    auto contentLightFilterPara2 = std::make_shared<RSUIContentLightFilterPara>();
    auto rsUIFilterParaBase2 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(contentLightFilterPara2);
    contentLightFilterPara1->SetProperty(rsUIFilterParaBase2);

    auto contentLightPara = std::make_shared<ContentLightPara>();
    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    contentLightPara->SetLightPosition(lightPosition);
    contentLightFilterPara2->SetContentLight(contentLightPara);
    rsUIFilterParaBase2 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(contentLightFilterPara2);
    contentLightFilterPara1->SetProperty(rsUIFilterParaBase2);

    Vector4f lightColor = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    contentLightPara->SetLightColor(lightColor);
    contentLightFilterPara2->SetContentLight(contentLightPara);
    rsUIFilterParaBase2 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(contentLightFilterPara2);
    contentLightFilterPara1->SetProperty(rsUIFilterParaBase2);

    float lightIntensity = 0.5f;
    contentLightPara->SetLightIntensity(lightIntensity);
    contentLightFilterPara2->SetContentLight(contentLightPara);
    rsUIFilterParaBase2 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(contentLightFilterPara2);
    contentLightFilterPara1->SetProperty(rsUIFilterParaBase2);

    auto iter = contentLightFilterPara1->properties_.find(RSUIFilterType::LIGHT_INTENSITY);
    ASSERT_NE(iter, contentLightFilterPara1->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), lightIntensity);
}

/**
 * @tc.name: SetContentLight001
 * @tc.desc: Test function of SetContentLight
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContentLightFilterTest, SetContentLight001, TestSize.Level1)
{
    auto contentLightFilterPara = std::make_shared<RSUIContentLightFilterPara>();
    contentLightFilterPara->SetContentLight(nullptr);
    auto rsRenderFilterParaBase1 = contentLightFilterPara->CreateRSRenderFilter();
    EXPECT_EQ(rsRenderFilterParaBase1, nullptr);

    auto contentLightPara = std::make_shared<ContentLightPara>();
    Vector4f lightColor = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    contentLightPara->SetLightColor(lightColor);

    contentLightFilterPara->SetContentLight(contentLightPara);
    auto rsRenderFilterParaBase2 = contentLightFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase2, nullptr);
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc: Test function of CreateRSRenderFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContentLightFilterTest, CreateRSRenderFilter001, TestSize.Level1)
{
    auto contentLightFilterPara = std::make_shared<RSUIContentLightFilterPara>();

    auto contentLightPara0 = std::make_shared<ContentLightPara>();
    contentLightFilterPara->SetContentLight(contentLightPara0);
    auto rsRenderFilterParaBase = contentLightFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);

    rsRenderFilterParaBase = nullptr;
    auto contentLightPara1 = std::make_shared<ContentLightPara>();
    Vector4f lightColor = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    contentLightPara1->SetLightColor(lightColor);
    contentLightFilterPara->SetContentLight(contentLightPara1);
    rsRenderFilterParaBase = contentLightFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);

    rsRenderFilterParaBase = nullptr;
    auto contentLightPara2 = std::make_shared<ContentLightPara>();
    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    contentLightPara2->SetLightPosition(lightPosition);
    contentLightFilterPara->SetContentLight(contentLightPara2);
    rsRenderFilterParaBase = contentLightFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);

    rsRenderFilterParaBase = nullptr;
    auto contentLightPara3 = std::make_shared<ContentLightPara>();
    float lightIntensity = 0.5f;
    contentLightPara3->SetLightIntensity(lightIntensity);
    contentLightFilterPara->SetContentLight(contentLightPara3);
    rsRenderFilterParaBase = contentLightFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);

    rsRenderFilterParaBase = nullptr;
    auto contentLightPara4 = std::make_shared<ContentLightPara>();
    contentLightPara4->SetLightColor(lightColor);
    contentLightPara4->SetLightPosition(lightPosition);
    contentLightPara4->SetLightIntensity(lightIntensity);
    contentLightFilterPara->SetContentLight(contentLightPara4);
    rsRenderFilterParaBase = contentLightFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);
}

/**
 * @tc.name: CreateRenderFilterLightPosition001
 * @tc.desc: Test function of CreateRenderFilterLightPosition
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContentLightFilterTest, CreateRenderFilterLightPosition001, TestSize.Level1)
{
    auto tempFilter = std::make_shared<RSUIContentLightFilterPara>();
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    auto lightPositionProperty = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(lightPosition);
    filterPara->Setter(RSUIFilterType::LIGHT_POSITION, lightPositionProperty);

    auto ret = tempFilter->CreateRenderFilterLightPosition(filterPara, RSUIFilterType::KAWASE);
    EXPECT_FALSE(ret);
    ret = tempFilter->CreateRenderFilterLightPosition(filterPara, RSUIFilterType::LIGHT_POSITION);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CreateRenderFilterLightColor001
 * @tc.desc: Test function of CreateRenderFilterLightColor
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContentLightFilterTest, CreateRenderFilterLightColor001, TestSize.Level1)
{
    auto tempFilter = std::make_shared<RSUIContentLightFilterPara>();
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    Vector4f lightColor = Vector4f(0.1f, 0.2f, 0.3f, 0.0f);
    auto lightColorProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(lightColor);
    filterPara->Setter(RSUIFilterType::LIGHT_COLOR, lightColorProperty);

    auto ret = tempFilter->CreateRenderFilterLightColor(filterPara, RSUIFilterType::KAWASE);
    EXPECT_FALSE(ret);
    ret = tempFilter->CreateRenderFilterLightColor(filterPara, RSUIFilterType::LIGHT_COLOR);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CreateRenderFilterLightIntensity001
 * @tc.desc: Test function of CreateRenderFilterLightIntensity
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContentLightFilterTest, CreateRenderFilterLightIntensity001, TestSize.Level1)
{
    auto tempFilter = std::make_shared<RSUIContentLightFilterPara>();
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    auto lightIntensityProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f);
    filterPara->Setter(RSUIFilterType::LIGHT_INTENSITY, lightIntensityProperty);

    auto ret = tempFilter->CreateRenderFilterLightIntensity(filterPara, RSUIFilterType::KAWASE);
    EXPECT_FALSE(ret);
    ret = tempFilter->CreateRenderFilterLightIntensity(filterPara, RSUIFilterType::LIGHT_INTENSITY);
    EXPECT_FALSE(ret);
}

} // namespace OHOS::Rosen