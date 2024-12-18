/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "include/render/rs_shadow.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSShadowUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShadowUnitTest::SetUpTestCase() {}
void RSShadowUnitTest::TearDownTestCase() {}
void RSShadowUnitTest::SetUp() {}
void RSShadowUnitTest::TearDown() {}

/**
 * @tc.name: TestSetColor01
 * @tc.desc: Verify function SetColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetColor01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    testShadow->SetColor(Color(1, 1, 1));
    EXPECT_EQ(testShadow->color_.GetBlue(), 1);
}

/**
 * @tc.name: TestSetOffsetX01
 * @tc.desc: Verify function SetOffsetX
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetOffsetX01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    testShadow->SetOffsetX(1.0f);
    EXPECT_EQ(testShadow->offsetX_, 1.0f);
}

/**
 * @tc.name: TestSetOffsetY01
 * @tc.desc: Verify function SetOffsetY
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetOffsetY01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    testShadow->SetOffsetY(1.0f);
    EXPECT_EQ(testShadow->offsetY_, 1.0f);
}

/**
 * @tc.name: TestSetAlpha01
 * @tc.desc: Verify function SetAlpha
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetAlpha01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    testShadow->SetAlpha(1.0f);
    EXPECT_EQ(testShadow->color_.GetBlue(), 0);
}

/**
 * @tc.name: TestSetElevation01
 * @tc.desc: Verify function SetElevation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetElevation01, TestSize.Level1)
{
    std::shared_ptr<RSShadow> testShadow = std::make_shared<RSShadow>();
    ASSERT_FALSE(testShadow->IsValid());
    testShadow->SetElevation(5.f);
    ASSERT_FALSE(testShadow->IsValid());
}

/**
 * @tc.name: TestSetRadius01
 * @tc.desc: Verify function SetRadius
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetRadius01, TestSize.Level1)
{
    RSShadow testShadow;
    testShadow.SetRadius(0.1);
    ASSERT_FALSE(testShadow->IsValid());
}

/**
 * @tc.name: TestSetPath01
 * @tc.desc: Verify function SetPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetPath01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    auto path = std::make_shared<RSPath>();
    testShadow->SetPath(path);
    EXPECT_NE(testShadow->path_, nullptr);
}

/**
 * @tc.name: TestSetMask01
 * @tc.desc: Verify function SetMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetMask01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    testShadow->SetMask(true);
    EXPECT_TRUE(testShadow->imageMask_);
}

/**
 * @tc.name: TestSetIsFilled01
 * @tc.desc: Verify function SetIsFilled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetIsFilled01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    testShadow->SetIsFilled(true);
    EXPECT_TRUE(testShadow->isFilled_);
}

/**
 * @tc.name: TestSetColorStrategy01
 * @tc.desc: Verify function SetColorStrategy
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetColorStrategy01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    testShadow->SetColorStrategy(1);
    EXPECT_EQ(testShadow->colorStrategy_, 1);
}

/**
 * @tc.name: TestGetColor01
 * @tc.desc: Verify function GetColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetColor01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_EQ(testShadow->GetColor().GetBlue(), 0);
}

/**
 * @tc.name: TestGetOffsetX01
 * @tc.desc: Verify function GetOffsetX
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetOffsetX01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_NE(testShadow->GetOffsetX(), 1.0f);
}

/**
 * @tc.name: TestGetOffsetY01
 * @tc.desc: Verify function GetOffsetY
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetOffsetY01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_NE(testShadow->GetOffsetY(), 1.0f);
}

/**
 * @tc.name: TestGetAlpha01
 * @tc.desc: Verify function GetAlpha
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetAlpha01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_NE(testShadow->GetOffsetY(), 1.0f);
}

/**
 * @tc.name: TestGetElevation01
 * @tc.desc: Verify function GetElevation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetElevation01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_NE(testShadow->GetElevation(), 1.0f);
}

/**
 * @tc.name: TestGetRadius01
 * @tc.desc: Verify function GetRadius
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetRadius01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_NE(testShadow->GetRadius(), 1.0f);
}

/**
 * @tc.name: TestGetPath01
 * @tc.desc: Verify function GetPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetPath01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_EQ(testShadow->GetPath(), nullptr);
}

/**
 * @tc.name: TestGetMask01
 * @tc.desc: Verify function GetMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetMask01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_FALSE(testShadow->GetMask());
}

/**
 * @tc.name: TestGetIsFilled01
 * @tc.desc: Verify function GetIsFilled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetIsFilled01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_FALSE(testShadow->GetIsFilled());
}

/**
 * @tc.name: TestGetColorStrategy01
 * @tc.desc: Verify function GetColorStrategy
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestGetColorStrategy01, TestSize.Level1)
{
    auto testShadow = std::make_shared<RSShadow>();
    EXPECT_NE(testShadow->GetColorStrategy(), 1);
}

/**
 * @tc.name: TestSetElevation02
 * @tc.desc: Verify function SetElevation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetElevation02, TestSize.Level1)
{
    RSShadow testShadow;
    testShadow.SetElevation(0.1);
    EXPECT_FALSE(testShadow.IsValid());
}

/**
 * @tc.name: TestSetColorStrategy02
 * @tc.desc: Verify function SetColorStrategy
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowUnitTest, TestSetColorStrategy02, TestSize.Level1)
{
    RSShadow testShadow;
    testShadow.SetColorStrategy(SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE);
    EXPECT_EQ(testShadow.GetColorStrategy(), 1);
}
} // namespace OHOS::Rosen
