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
#include "ui_effect/property/include/rs_ui_bezier_warp_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIBezierWarpFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIBezierWarpFilterTest::SetUpTestCase() {}
void RSUIBezierWarpFilterTest::TearDownTestCase() {}
void RSUIBezierWarpFilterTest::SetUp() {}
void RSUIBezierWarpFilterTest::TearDown() {}

/**
 * @tc.name: Equal001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBezierWarpFilterTest, Equal001, TestSize.Level1)
{
    auto rsUIBezierWarpFilterPara1 = std::make_shared<RSUIBezierWarpFilterPara>();
    auto rsUIBezierWarpFilterPara2 = std::make_shared<RSUIBezierWarpFilterPara>();

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBezierWarpFilterPara2);

    EXPECT_TRUE(rsUIBezierWarpFilterPara1->Equals(rsUIFilterParaBase));
}

/**
 * @tc.name: SetProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBezierWarpFilterTest, SetProperty001, TestSize.Level0)
{
    std::array<Vector2f, BEZIER_WARP_POINT_NUM> points = {{
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
    auto bezierWarpPara = std::make_shared<BezierWarpPara>();
    bezierWarpPara->SetBezierControlPoints(points);

    auto rsUIBezierWarpFilterPara1 = std::make_shared<RSUIBezierWarpFilterPara>();
    auto rsUIBezierWarpFilterPara2 = std::make_shared<RSUIBezierWarpFilterPara>();
    rsUIBezierWarpFilterPara2->SetBezierWarp(bezierWarpPara);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBezierWarpFilterPara2);
    rsUIBezierWarpFilterPara1->SetProperty(rsUIFilterParaBase);

    auto iter = rsUIBezierWarpFilterPara1->properties_.find(RSUIFilterType::BEZIER_CONTROL_POINT0);
    ASSERT_NE(iter, rsUIBezierWarpFilterPara1->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), points[0]);
}

/**
 * @tc.name: SetBezierWarp001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBezierWarpFilterTest, SetBezierWarp001, TestSize.Level1)
{
    std::array<Vector2f, BEZIER_WARP_POINT_NUM> points = {{
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
    auto bezierWarpPara = std::make_shared<BezierWarpPara>();
    bezierWarpPara->SetBezierControlPoints(points);

    auto rsUIBezierWarpFilterPara = std::make_shared<RSUIBezierWarpFilterPara>();
    rsUIBezierWarpFilterPara->SetBezierWarp(bezierWarpPara);

    auto iter = rsUIBezierWarpFilterPara->properties_.find(RSUIFilterType::BEZIER_CONTROL_POINT0);
    ASSERT_NE(iter, rsUIBezierWarpFilterPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), points[0]);
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBezierWarpFilterTest, CreateRSRenderFilter001, TestSize.Level1)
{
    std::array<Vector2f, BEZIER_WARP_POINT_NUM> points = {{
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
    auto bezierWarpPara = std::make_shared<BezierWarpPara>();
    bezierWarpPara->SetBezierControlPoints(points);

    auto rsUIBezierWarpFilterPara = std::make_shared<RSUIBezierWarpFilterPara>();
    rsUIBezierWarpFilterPara->SetBezierWarp(bezierWarpPara);

    auto rsRenderFilterParaBase = rsUIBezierWarpFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);
}

/**
 * @tc.name: GetLeafProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIBezierWarpFilterTest, GetLeafProperties001, TestSize.Level1)
{
    std::array<Vector2f, BEZIER_WARP_POINT_NUM> points = {{
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
    auto bezierWarpPara = std::make_shared<BezierWarpPara>();
    bezierWarpPara->SetBezierControlPoints(points);

    auto rsUIBezierWarpFilterPara = std::make_shared<RSUIBezierWarpFilterPara>();
    rsUIBezierWarpFilterPara->SetBezierWarp(bezierWarpPara);

    auto rsPropertyBaseVec = rsUIBezierWarpFilterPara->GetLeafProperties();
    EXPECT_FALSE(rsPropertyBaseVec.empty());
}
} // namespace OHOS::Rosen