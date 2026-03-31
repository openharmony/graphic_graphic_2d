/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSDrawCmdListUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrawCmdListUtilTest::SetUpTestCase() {}
void RSDrawCmdListUtilTest::TearDownTestCase() {}
void RSDrawCmdListUtilTest::SetUp() {}
void RSDrawCmdListUtilTest::TearDown() {}

/**
 * @tc.name: PlaybackTest
 * @tc.desc: Test the Playback
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawCmdListUtilTest, PlaybackTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest PlaybackTest start";
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintCanvas(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 1, 1);
    auto startValue = std::make_shared<Drawing::DrawCmdList>(0, 0);
    auto endValue = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto rsCmdList = std::make_shared<RSDrawCmdList>(startValue, endValue);
    rsCmdList->Playback(paintCanvas, rect.get());
    ASSERT_TRUE(rsCmdList->GetEndDrawCmdList());

    rsCmdList->startValue_.first = nullptr;
    rsCmdList->endValue_.first = nullptr;
    rsCmdList->Playback(paintCanvas, rect.get());
    ASSERT_FALSE(rsCmdList->GetEndDrawCmdList());
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest PlaybackTest end";
}

/**
 * @tc.name: GetHeightTest
 * @tc.desc: Test the GetHeight
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawCmdListUtilTest, GetHeightTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest GetHeightTest start";
    auto startValue = std::make_shared<Drawing::DrawCmdList>(0, 0);
    auto endValue = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto rsCmdList = std::make_shared<RSDrawCmdList>(startValue, endValue);
    ASSERT_EQ(rsCmdList->GetHeight(), rsCmdList->GetEndDrawCmdList()->GetHeight());

    rsCmdList->startValue_.first = nullptr;
    rsCmdList->endValue_.first = nullptr;
    auto cmdList = std::static_pointer_cast<Drawing::DrawCmdList>(rsCmdList);
    ASSERT_EQ(rsCmdList->GetHeight(), cmdList->GetHeight());
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest GetHeightTest end";
}

/**
 * @tc.name: IsEmptyTest
 * @tc.desc: Test the IsEmpty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawCmdListUtilTest, IsEmptyTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest IsEmptyTest start";
    auto startValue = std::make_shared<Drawing::DrawCmdList>(0, 0);
    auto endValue = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto rsCmdList = std::make_shared<RSDrawCmdList>(startValue, endValue);
    ASSERT_EQ(rsCmdList->IsEmpty(), rsCmdList->GetEndDrawCmdList()->IsEmpty());

    rsCmdList->startValue_.first = nullptr;
    rsCmdList->endValue_.first = nullptr;
    ASSERT_FALSE(rsCmdList->IsEmpty());
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest IsEmptyTest end";
}

/**
 * @tc.name: GetWidthTest
 * @tc.desc: Test the GetWidth
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawCmdListUtilTest, GetWidthTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest GetWidthTest start";
    auto startValue = std::make_shared<Drawing::DrawCmdList>(0, 0);
    auto endValue = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto rsCmdList = std::make_shared<RSDrawCmdList>(startValue, endValue);
    ASSERT_EQ(rsCmdList->GetWidth(), rsCmdList->GetEndDrawCmdList()->GetWidth());

    rsCmdList->startValue_.first = nullptr;
    rsCmdList->endValue_.first = nullptr;
    auto cmdList = std::static_pointer_cast<Drawing::DrawCmdList>(rsCmdList);
    ASSERT_EQ(rsCmdList->GetWidth(), cmdList->GetWidth());
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest GetWidthTest end";
}

/**
 * @tc.name: EstimateTest
 * @tc.desc: Test the Estimate
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawCmdListUtilTest, EstimateTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest EstimateTest start";
    auto startValue = std::make_shared<Drawing::DrawCmdList>(0, 0);
    auto endValue = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto rsCmdList = std::make_shared<RSDrawCmdList>(startValue, endValue);
    rsCmdList->Estimate(1.0f);
    ASSERT_EQ(rsCmdList->endValue_.second.opacity, 1.0f);
    rsCmdList->Estimate(0.1f); // fraction
    ASSERT_NE(rsCmdList->endValue_.second.opacity, 1.0f);
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest EstimateTest end";
}

/**
 * @tc.name: ToStringTest
 * @tc.desc: Test the ToString
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawCmdListUtilTest, ToStringTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest ToStringTest start";
    auto startValue = std::make_shared<Drawing::DrawCmdList>(0, 0);
    auto endValue = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto rsCmdList = std::make_shared<RSDrawCmdList>(startValue, endValue);
    ASSERT_FALSE(rsCmdList->ToString().empty());
    rsCmdList->CleanOpacity();
    ASSERT_FALSE(rsCmdList->ToString().empty());
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest ToStringTest end";
}

/**
 * @tc.name: RSDrawCmdListUtilTest
 * @tc.desc: Test the RSDrawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawCmdListUtilTest, RSDrawCmdListUtilTest, TestSize.Level1)
{
    auto startValue = std::make_shared<Drawing::DrawCmdList>(0, 0);
    auto endValue = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto rsCmdList = std::make_shared<RSDrawCmdList>(nullptr, endValue);
    EXPECT_TRUE(rsCmdList->startValue_.first == nullptr);

    auto startValue2 = std::make_shared<RSDrawCmdList>(startValue, endValue);
    auto rsCmdList2 = std::make_shared<RSDrawCmdList>(startValue2, endValue);
    EXPECT_TRUE(rsCmdList->GetEndDrawCmdList());
}

/**
 * @tc.name: DrawCmdListPtr+-*==
 * @tc.desc: Test the DrawCmdListPtr+-*==
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawCmdListUtilTest, DrawCmdListPtrTest, TestSize.Level1)
{
    auto startValue = std::make_shared<Drawing::DrawCmdList>(0, 0);
    auto endValue = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto retPlus = startValue + endValue;
    EXPECT_EQ(startValue.get(), retPlus.get());

    auto retMinus = startValue - endValue;
    EXPECT_EQ(startValue.get(), retMinus.get());

    auto retMultiply = startValue * 1.f;
    EXPECT_EQ(startValue.get(), retMultiply.get());

    EXPECT_FALSE(startValue == endValue);
}

/**
 * @tc.name: GetEndDrawCmdListTest
 * @tc.desc: Test the GetEndDrawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawCmdListUtilTest, GetEndDrawCmdListTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest GetEndDrawCmdListTest start";
    auto startValue = std::make_shared<Drawing::DrawCmdList>(0, 0);
    auto endValue = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto rsCmdList = std::make_shared<RSDrawCmdList>(startValue, endValue);
    ASSERT_TRUE(rsCmdList->GetEndDrawCmdList());
    GTEST_LOG_(INFO) << "RSDrawCmdListUtilTest GetEndDrawCmdListTest end";
}
} // namespace Rosen
} // namespace OHOS