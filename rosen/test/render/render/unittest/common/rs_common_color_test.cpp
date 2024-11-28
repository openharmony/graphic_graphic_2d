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

#include <cmath>

#include "gtest/gtest.h"

#include "common/rs_color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCommonColorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCommonColorTest::SetUpTestCase() {}
void RSCommonColorTest::TearDownTestCase() {}
void RSCommonColorTest::SetUp() {}
void RSCommonColorTest::TearDown() {}

/**
 * @tc.name: Operator001
 * @tc.desc: test results of operator/
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCommonColorTest, Operator001, TestSize.Level1)
{
    int16_t red = 22;
    int16_t green = 15;
    int16_t blue = 33;
    int16_t alpha = 1;
    RSColor color(red, green, blue, alpha);
    // operator/ is overridden to do nothing when scale is equal to 0
    float scale = 0;
    RSColor scaledColor = color / scale;
    EXPECT_EQ(scaledColor.GetRed(), color.GetRed());
    EXPECT_EQ(scaledColor.GetGreen(), color.GetGreen());
    EXPECT_EQ(scaledColor.GetBlue(), color.GetBlue());
    EXPECT_EQ(scaledColor.GetAlpha(), color.GetAlpha());

    scale = 3;
    scaledColor = color / scale;
    int16_t scaledRed = 6;
    int16_t scaledGreen = 5;
    int16_t scaledBlue = 2;
    int16_t scaledAlpha = 9;
    EXPECT_EQ(scaledColor.GetRed(), scaledRed);
    EXPECT_EQ(scaledColor.GetGreen(), scaledGreen);
    EXPECT_EQ(scaledColor.GetBlue(), scaledBlue);
    EXPECT_EQ(scaledColor.GetAlpha(), scaledAlpha);
}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: verify function IsNearEqual
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCommonColorTest, IsNearEqual001, TestSize.Level1)
{
    int16_t red = 9;
    int16_t green = 10;
    int16_t blue = 11;
    int16_t alpha = 12;
    RSColor color(red, green, blue, alpha);

    int16_t threshold = 2;

    int16_t red1 = 10;
    RSColor ohterColor1(red1, green, blue, alpha);
    EXPECT_TRUE(color.IsNearEqual(ohterColor1, threshold));

    int16_t red2 = 14;
    RSColor ohterColor2(red2, green, blue, alpha);
    EXPECT_FALSE(color.IsNearEqual(ohterColor2, threshold));
}

/**
 * @tc.name: RSColorCreateTest
 * @tc.desc: Verify function RSColor
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, RSColorCreateTest, TestSize.Level1)
{
    std::shared_ptr<RSColor> color;
    color = std::make_shared<RSColor>();
    EXPECT_FALSE(color == nullptr);
    color = std::make_shared<RSColor>(0);
    EXPECT_FALSE(color == nullptr);
    // for test
    int16_t red = 9;
    int16_t green = 8;
    int16_t blue = 7;
    color = std::make_shared<RSColor>(red, green, blue);
    EXPECT_FALSE(color == nullptr);
}

/**
 * @tc.name: operatorTest002
 * @tc.desc: Verify function operator== operator+ operator- operator* operator*=
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, operatorTest002, TestSize.Level1)
{
    // for test
    int16_t red = 12;
    int16_t green = 11;
    int16_t blue = 10;
    int16_t alpha = 13;
    RSColor color(red, green, blue, alpha);
    RSColor colorTest;
    float scale = 0;
    EXPECT_FALSE(color == colorTest);
    EXPECT_TRUE(color == (color + colorTest));
    EXPECT_TRUE(color == (color - colorTest));
    EXPECT_FALSE(color == (color * scale));
    EXPECT_TRUE(color == (color *= scale));
}

/**
 * @tc.name: AsRgbaIntTest
 * @tc.desc: Verify function AsRgbaInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, AsRgbaIntTest, TestSize.Level1)
{
    RSColor color(1);
    EXPECT_EQ(color.AsRgbaInt(), 1);
}

/**
 * @tc.name: FromRgbaIntTest
 * @tc.desc: Verify function FromRgbaInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, FromRgbaIntTest, TestSize.Level1)
{
    auto color = RSColor::FromRgbaInt(1);
    EXPECT_EQ(color.alpha_, 1);
}

/**
 * @tc.name: AsArgbIntTest
 * @tc.desc: Verify function AsArgbInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, AsArgbIntTest, TestSize.Level1)
{
    RSColor color(1);
    EXPECT_EQ(color.AsArgbInt(), 16777216);
}

/**
 * @tc.name: FromArgbIntTest
 * @tc.desc: Verify function FromArgbInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, FromArgbIntTest, TestSize.Level1)
{
    auto color = RSColor::FromArgbInt(1);
    EXPECT_EQ(color.alpha_, 0);
}

/**
 * @tc.name: FromBgraIntTest
 * @tc.desc: Verify function FromBgraInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, FromBgraIntTest, TestSize.Level1)
{
    auto color = RSColor::FromBgraInt(1);
    EXPECT_EQ(color.alpha_, 0);
}


/**
 * @tc.name: AlphaTest
 * @tc.desc: Verify function GetAlpha SetAlpha
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, AlphaTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetAlpha(), 0);
    color.SetAlpha(1);
    EXPECT_EQ(color.GetAlpha(), 1);
}

/**
 * @tc.name: MultiplyAlphaTest
 * @tc.desc: Verify function MultiplyAlpha
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, MultiplyAlphaTest, TestSize.Level1)
{
    RSColor color;
    color.MultiplyAlpha(1.0f);
    EXPECT_EQ(color.GetAlpha(), 0);
}
} // namespace OHOS::Rosen

/**
 * @tc.name: AsBgraIntTest
 * @tc.desc: Verify function AsBgraInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, AsBgraIntTest, TestSize.Level1)
{
    RSColor color(1);
    EXPECT_EQ(color.AsBgraInt(), 1);
}

/**
 * @tc.name: BlueTest
 * @tc.desc: Verify function GetBlue SetBlue
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, BlueTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetBlue(), 0);
    color.SetBlue(1);
    EXPECT_EQ(color.GetBlue(), 1);
}

/**
 * @tc.name: GreenTest
 * @tc.desc: Verify function GetGreen SetGreen
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, GreenTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetGreen(), 0);
    color.SetGreen(1);
    EXPECT_EQ(color.GetGreen(), 1);
}

/**
 * @tc.name: RedTest
 * @tc.desc: Verify function GetRed SetRed
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonColorTest, RedTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetRed(), 0);
    color.SetRed(1);
    EXPECT_EQ(color.GetRed(), 1);
}