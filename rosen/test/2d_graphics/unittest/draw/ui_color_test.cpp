/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "draw/ui_color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class UIColorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void UIColorTest::SetUpTestCase() {}
void UIColorTest::TearDownTestCase() {}
void UIColorTest::SetUp() {}
void UIColorTest::TearDown() {}

/**
 * @tc.name: DefaultConstructor001
 * @tc.desc: Test Default Constructor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, DefaultConstructor001, TestSize.Level1)
{
    UIColor color;
    EXPECT_FLOAT_EQ(color.GetRed(), 0.0f);
    EXPECT_FLOAT_EQ(color.GetGreen(), 0.0f);
    EXPECT_FLOAT_EQ(color.GetBlue(), 0.0f);
    EXPECT_FLOAT_EQ(color.GetAlpha(), 1.0f);
    EXPECT_FLOAT_EQ(color.GetHeadroom(), 1.0f);
}

/**
 * @tc.name: ParameterizedConstructor001
 * @tc.desc: Test Parameterized Constructor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, ParameterizedConstructor001, TestSize.Level1)
{
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    EXPECT_FLOAT_EQ(color.GetRed(), 0.5f);
    EXPECT_FLOAT_EQ(color.GetGreen(), 0.6f);
    EXPECT_FLOAT_EQ(color.GetBlue(), 0.7f);
    EXPECT_FLOAT_EQ(color.GetAlpha(), 0.8f);
    EXPECT_FLOAT_EQ(color.GetHeadroom(), 2.0f);

    UIColor copy(color);
    EXPECT_FLOAT_EQ(color.GetRed(), 0.5f);
    EXPECT_FLOAT_EQ(color.GetGreen(), 0.6f);
    EXPECT_FLOAT_EQ(color.GetBlue(), 0.7f);
    EXPECT_FLOAT_EQ(color.GetAlpha(), 0.8f);
    EXPECT_FLOAT_EQ(color.GetHeadroom(), 2.0f);
}

/**
 * @tc.name: SetRed001
 * @tc.desc: Test SetRed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, SetRed001, TestSize.Level1)
{
    UIColor color;
    color.SetRed(0.7f);
    EXPECT_FLOAT_EQ(color.GetRed(), 0.7f);
    color.SetRed(-0.5f);
    EXPECT_FLOAT_EQ(color.GetRed(), 0.0f);
}

/**
 * @tc.name: SetGreen001
 * @tc.desc: Test SetGreen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, SetGreen001, TestSize.Level1)
{
    UIColor color;
    color.SetGreen(0.8f);
    EXPECT_FLOAT_EQ(color.GetGreen(), 0.8f);
    color.SetGreen(-0.3f);
    EXPECT_FLOAT_EQ(color.GetGreen(), 0.0f);
}

/**
 * @tc.name: SetBlue001
 * @tc.desc: Test SetBlue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, SetBlue001, TestSize.Level1)
{
    UIColor color;
    color.SetBlue(0.9f);
    EXPECT_FLOAT_EQ(color.GetBlue(), 0.9f);
    color.SetBlue(-0.2f);
    EXPECT_FLOAT_EQ(color.GetBlue(), 0.0f);
}

/**
 * @tc.name: SetAlpha001
 * @tc.desc: Test SetAlpha
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, SetAlpha001, TestSize.Level1)
{
    UIColor color;
    color.SetAlpha(0.6f);
    EXPECT_FLOAT_EQ(color.GetAlpha(), 0.6f);
    color.SetAlpha(-0.4f);
    EXPECT_FLOAT_EQ(color.GetAlpha(), 0.0f);
}

/**
 * @tc.name: SetHeadroom001
 * @tc.desc: Test SetHeadroom
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, SetHeadroom001, TestSize.Level1)
{
    UIColor color;
    color.SetHeadroom(3.0f);
    EXPECT_FLOAT_EQ(color.GetHeadroom(), 3.0f);
    color.SetHeadroom(-1.0f);
    EXPECT_FLOAT_EQ(color.GetHeadroom(), 1.0f);
}

/**
 * @tc.name: SetRgb001
 * @tc.desc: Test SetRgb
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, SetRgb001, TestSize.Level1)
{
    UIColor color;
    color.SetRgb(0.4f, 0.5f, 0.6f);
    EXPECT_FLOAT_EQ(color.GetRed(), 0.4f);
    EXPECT_FLOAT_EQ(color.GetGreen(), 0.5f);
    EXPECT_FLOAT_EQ(color.GetBlue(), 0.6f);
}

/**
 * @tc.name: SetRgbF001
 * @tc.desc: Test SetRgbF
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, SetRgbF001, TestSize.Level1)
{
    UIColor color;
    color.SetRgbF(0.3f, 0.4f, 0.5f, 0.7f);
    EXPECT_FLOAT_EQ(color.GetRed(), 0.3f);
    EXPECT_FLOAT_EQ(color.GetGreen(), 0.4f);
    EXPECT_FLOAT_EQ(color.GetBlue(), 0.5f);
    EXPECT_FLOAT_EQ(color.GetAlpha(), 0.7f);
}

/**
 * @tc.name: OperatorEqual001
 * @tc.desc: Test OperatorEqual
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, OperatorEqual001, TestSize.Level1)
{
    UIColor color1(0.5f, 0.6f, 0.7f, 0.8f, 1.0f);
    UIColor color2(0.5f, 0.6f, 0.7f, 0.8f, 1.0f);
    EXPECT_TRUE(color1 == color2);
    UIColor color3(0.5f, 0.6f, 0.7f, 0.8f, 1.0f);
    UIColor color4(0.5f, 0.6f, 0.7f, 0.9f, 1.0f);
    EXPECT_FALSE(color3 == color4);
}

/**
 * @tc.name: OperatorNotEqual001
 * @tc.desc: Test OperatorNotEqual
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, OperatorNotEqual001, TestSize.Level1)
{
    UIColor color1(0.5f, 0.6f, 0.7f, 0.8f, 1.0f);
    UIColor color2(0.5f, 0.6f, 0.7f, 0.9f, 1.0f);
    EXPECT_TRUE(color1 != color2);
    UIColor color3(0.5f, 0.6f, 0.7f, 0.8f, 1.0f);
    UIColor color4(0.5f, 0.6f, 0.7f, 0.8f, 1.0f);
    EXPECT_FALSE(color3 != color4);
}

/**
 * @tc.name: Dump001
 * @tc.desc: Test Dump
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIColorTest, Dump001, TestSize.Level1)
{
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    std::string out;
    color.Dump(out);
    EXPECT_FALSE(out.empty());
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
