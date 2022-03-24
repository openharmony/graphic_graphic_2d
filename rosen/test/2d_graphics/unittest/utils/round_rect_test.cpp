/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "utils/rect.h"
#include "utils/scalar.h"
#include "utils/round_rect.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RoundRectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RoundRectTest::SetUpTestCase() {}
void RoundRectTest::TearDownTestCase() {}
void RoundRectTest::SetUp() {}
void RoundRectTest::TearDown() {}

/**
 * @tc.name: RoundRectCreateAndDestory001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectCreateAndDestory001, TestSize.Level1)
{
    // The best way to create RoundRect.
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRect != nullptr);
}

/**
 * @tc.name: RoundRectCreateAndDestory002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectCreateAndDestory002, TestSize.Level1)
{
    // The best way to create RoundRect.
    const RoundRect roundRect1;
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>(roundRect1);
    ASSERT_TRUE(roundRect != nullptr);
}

/**
 * @tc.name: RoundRectCreateAndDestory003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectRectCreateAndDestory003, TestSize.Level1)
{
    // The best way to create RoundRect.
    Rect rect;
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>(rect, 12.6f, 77.4f);
    ASSERT_TRUE(roundRect != nullptr);
}

/**
 * @tc.name: RoundRectCreateAndDestory004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectRectCreateAndDestory004, TestSize.Level1)
{
    // The best way to create RoundRect.
    Rect rect;
    std::vector<Point> radiusXY = {{1, 3}};
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>(rect, radiusXY);
    ASSERT_TRUE(roundRect != nullptr);
}

/**
 * @tc.name: RoundRectSetAndGetCornerRadius001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectRectSetAndGetCornerRadius001, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRect != nullptr);
    roundRect->SetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS, 111.3f, 84.5f);
    ASSERT_EQ(roundRect->GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetX(), 111.3f);
    ASSERT_EQ(roundRect->GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS).GetY(), 84.5f);
}

/**
 * @tc.name: RoundRectSetAndGetCornerRadius002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectSetAndGetCornerRadius002, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRect != nullptr);
    roundRect->SetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS, 120.0f, 90.5f);
    ASSERT_EQ(roundRect->GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetX(), 120.0f);
    ASSERT_EQ(roundRect->GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS).GetY(), 90.5f);
}

/**
 * @tc.name: RoundRectSetAndGetRect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectSetAndGetRect001, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRect != nullptr);
    Rect rect1;
    roundRect->SetRect(rect1);
    Rect rect2 = roundRect->GetRect();
    EXPECT_EQ(rect2, rect1);
}

/**
 * @tc.name: RoundRectSetAndGetRect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectSetAndGetRect002, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRect != nullptr);
    const Rect rect2;
    roundRect->SetRect(rect2);
    EXPECT_EQ(rect2, roundRect->GetRect());
}

/**
 * @tc.name: RoundRectRoundRectOffsetTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectRoundRectOffsetTest001, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRect != nullptr);
    roundRect->Offset(54.6f, 432.8f);
}

/**
 * @tc.name: RoundRectRoundRectOffsetTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RoundRectTest, RoundRectRoundRectOffsetTest002, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRect = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRect != nullptr);
    roundRect->Offset(200.0f, 40.8f);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS