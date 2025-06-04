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

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_render_bezier_warp_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderBezierWarpFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderBezierWarpFilterTest::SetUpTestCase() {}
void RSRenderBezierWarpFilterTest::TearDownTestCase() {}
void RSRenderBezierWarpFilterTest::SetUp() {}
void RSRenderBezierWarpFilterTest::TearDown() {}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    std::string out;
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    rsRenderBezierWarpFilterPara->GetDescription(out);
    EXPECT_EQ(out, "RSRenderBezierWarpFilterPara");
}

/**
 * @tc.name: WriteToParcelTest001
 * @tc.desc: Verify function WriteToParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, WriteToParcelTest001, TestSize.Level1)
{
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderBezierWarpFilterPara->WriteToParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ReadFromParcelTest001
 * @tc.desc: Verify function ReadFromParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, ReadFromParcelTest001, TestSize.Level1)
{
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderBezierWarpFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CreateRenderPropertyTest001
 * @tc.desc: Verify function CreateRenderProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, CreateRenderPropertyTest001, TestSize.Level1)
{
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    int point0  = static_cast<int>(RSUIFilterType::BEZIER_CONTROL_POINT0);
    int point11 = static_cast<int>(RSUIFilterType::BEZIER_CONTROL_POINT11);

    auto rsRenderPropertyBase1 = rsRenderBezierWarpFilterPara->CreateRenderProperty(RSUIFilterType::NONE);
    EXPECT_EQ(rsRenderPropertyBase1, nullptr);

    for (int i = point0; i <= point11; i++) {
        auto tempType = RSUIFilterType(i);
        auto rsRenderPropertyBase = rsRenderBezierWarpFilterPara->CreateRenderProperty(tempType);
        EXPECT_NE(rsRenderPropertyBase, nullptr);
    }
}

/**
 * @tc.name: GetLeafRenderPropertiesTest001
 * @tc.desc: Verify function CreateRenderProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, GetLeafRenderPropertiesTest001, TestSize.Level1)
{
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    auto out = rsRenderBezierWarpFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(out.empty());
}

/**
 * @tc.name: GetBezierWarpPointsTest001
 * @tc.desc: Verify function GetBezierWarpPoints
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, GetBezierWarpPointsTest001, TestSize.Level1)
{
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> points = {{
        {1.0f, 2.0f},
        {3.0f, 4.0f},
        {5.0f, 6.0f},
        {7.0f, 8.0f},
        {9.0f, 10.0f},
        {11.0f, 12.0f},
        {13.0f, 14.0f},
        {15.0f, 16.0f},
        {17.0f, 18.0f},
        {19.0f, 20.0f},
        {21.0f, 22.0f},
        {23.0f, 24.0f}
    }};
    auto rsBezierWarpFilter = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    rsBezierWarpFilter->destinationPatch_ = points;
    auto bezierWarpPoints = rsBezierWarpFilter->GetBezierWarpPoints();
    EXPECT_EQ(bezierWarpPoints.size(), BEZIER_WARP_POINT_NUM);
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, GenerateGEVisualEffectTest001, TestSize.Level1)
{
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> points = {{
        {1.0f, 2.0f},
        {3.0f, 4.0f},
        {5.0f, 6.0f},
        {7.0f, 8.0f},
        {9.0f, 10.0f},
        {11.0f, 12.0f},
        {13.0f, 14.0f},
        {15.0f, 16.0f},
        {17.0f, 18.0f},
        {19.0f, 20.0f},
        {21.0f, 22.0f},
        {23.0f, 24.0f}
    }};
    auto rsBezierWarpFilter = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    rsBezierWarpFilter->destinationPatch_ = points;
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsBezierWarpFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: ParseFilterValuesTest001
 * @tc.desc: Verify function ParseFilterValues
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, ParseFilterValuesTest001, TestSize.Level1)
{
    auto rsBezierWarpFilter = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    EXPECT_FALSE(rsBezierWarpFilter->ParseFilterValues());

    std::array<RSUIFilterType, BEZIER_WARP_POINT_NUM> ctrlPointsType = {
        RSUIFilterType::BEZIER_CONTROL_POINT0,
        RSUIFilterType::BEZIER_CONTROL_POINT1,
        RSUIFilterType::BEZIER_CONTROL_POINT2,
        RSUIFilterType::BEZIER_CONTROL_POINT3,
        RSUIFilterType::BEZIER_CONTROL_POINT4,
        RSUIFilterType::BEZIER_CONTROL_POINT5,
        RSUIFilterType::BEZIER_CONTROL_POINT6,
        RSUIFilterType::BEZIER_CONTROL_POINT7,
        RSUIFilterType::BEZIER_CONTROL_POINT8,
        RSUIFilterType::BEZIER_CONTROL_POINT9,
        RSUIFilterType::BEZIER_CONTROL_POINT10,
        RSUIFilterType::BEZIER_CONTROL_POINT11,
    };
    for (int i = 0; i < BEZIER_WARP_POINT_NUM; i++) {
        auto renderProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
            Vector2f(0.f, 0.f), 0, RSRenderPropertyType::PROPERTY_VECTOR2F);
        rsBezierWarpFilter->Setter(ctrlPointsType[i], renderProperty);
    }
    EXPECT_TRUE(rsBezierWarpFilter->ParseFilterValues());
}

/**
 * @tc.name: ParseFilterValuesTest002
 * @tc.desc: Verify function ParseFilterValues
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, ParseFilterValuesTest002, TestSize.Level1)
{
    auto rsBezierWarpFilter = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    EXPECT_FALSE(rsBezierWarpFilter->ParseFilterValues());

    std::array<RSUIFilterType, BEZIER_WARP_POINT_NUM> ctrlPointsType = {
        RSUIFilterType::BEZIER_CONTROL_POINT0,
        RSUIFilterType::BEZIER_CONTROL_POINT1,
        RSUIFilterType::BEZIER_CONTROL_POINT2,
        RSUIFilterType::BEZIER_CONTROL_POINT3,
        RSUIFilterType::BEZIER_CONTROL_POINT4,
        RSUIFilterType::BEZIER_CONTROL_POINT5,
        RSUIFilterType::BEZIER_CONTROL_POINT6,
        RSUIFilterType::BEZIER_CONTROL_POINT7,
        RSUIFilterType::BEZIER_CONTROL_POINT8,
        RSUIFilterType::BEZIER_CONTROL_POINT9,
        RSUIFilterType::BEZIER_CONTROL_POINT10,
        RSUIFilterType::BEZIER_CONTROL_POINT11,
    };
    for (int i = 0; i < BEZIER_WARP_POINT_NUM - 1; i++) {
        auto renderProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
            Vector2f(0.f, 0.f), 0, RSRenderPropertyType::PROPERTY_VECTOR2F);
        rsBezierWarpFilter->Setter(ctrlPointsType[i], renderProperty);
    }
    EXPECT_FALSE(rsBezierWarpFilter->ParseFilterValues());
}
} // namespace OHOS::Rosen