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

#include "render/rs_bezier_warp_filter.h"
#include "common/rs_common_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBezierWarpFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBezierWarpFilterTest::SetUpTestCase() {}
void RSBezierWarpFilterTest::TearDownTestCase() {}
void RSBezierWarpFilterTest::SetUp() {}
void RSBezierWarpFilterTest::TearDown() {}

/**
 * @tc.name: GetBezierWarpPointsTest001
 * @tc.desc: Verify function GetBezierWarpPoints
 * @tc.type:FUNC
 */
HWTEST_F(RSBezierWarpFilterTest, GetBezierWarpPointsTest001, TestSize.Level1)
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
    auto rsBezierWarpFilter = std::make_shared<RSBezierWarpFilter>(points);
    auto bezierWarpPoints = rsBezierWarpFilter->GetBezierWarpPoints();
    EXPECT_EQ(bezierWarpPoints.size(), BEZIER_WARP_POINT_NUM);
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 */
HWTEST_F(RSBezierWarpFilterTest, GenerateGEVisualEffectTest001, TestSize.Level1)
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
    auto rsBezierWarpFilter = std::make_shared<RSBezierWarpFilter>(points);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsBezierWarpFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}
} // namespace OHOS::Rosen