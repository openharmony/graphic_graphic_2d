/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
class RSShadowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShadowTest::SetUpTestCase() {}
void RSShadowTest::TearDownTestCase() {}
void RSShadowTest::SetUp() {}
void RSShadowTest::TearDown() {}

/**
 * @tc.name: SetColorTest001
 * @tc.desc: Verify function SetColor
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, SetColorTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetColor(Color(1, 1, 1));
    EXPECT_EQ(rsShadow->color_.GetBlue(), 1);
}

/**
 * @tc.name: SetOffsetXTest001
 * @tc.desc: Verify function SetOffsetX
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, SetOffsetXTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetOffsetX(1.0f);
    EXPECT_EQ(rsShadow->offsetX_, 1.0f);
}

/**
 * @tc.name: SetOffsetYTest001
 * @tc.desc: Verify function SetOffsetY
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, SetOffsetYTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetOffsetY(1.0f);
    EXPECT_EQ(rsShadow->offsetY_, 1.0f);
}

/**
 * @tc.name: SetAlphaTest001
 * @tc.desc: Verify function SetAlpha
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, SetAlphaTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetAlpha(1.0f);
    EXPECT_EQ(rsShadow->color_.GetBlue(), 0);
}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    std::shared_ptr<RSShadow> rsShadow = std::make_shared<RSShadow>();
    ASSERT_FALSE(rsShadow->IsValid());
    rsShadow->SetElevation(5.f);
    ASSERT_FALSE(rsShadow->IsValid());
}

/**
 * @tc.name: TestRSShadow001
 * @tc.desc: IsValid test.
 * @tc.type: FUNC
 */
HWTEST_F(RSShadowTest, TestRSShadow001, TestSize.Level1)
{
    RSShadow shadow;
    shadow.SetRadius(0.1);
    shadow.IsValid();
}

/**
 * @tc.name: SetPathTest001
 * @tc.desc: Verify function SetPath
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, SetPathTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    auto path = std::make_shared<RSPath>();
    rsShadow->SetPath(path);
    EXPECT_NE(rsShadow->path_, nullptr);
}

/**
 * @tc.name: SetMaskTest001
 * @tc.desc: Verify function SetMask
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, SetMaskTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetMask(true);
    EXPECT_TRUE(rsShadow->imageMask_);
}

/**
 * @tc.name: SetIsFilledTest001
 * @tc.desc: Verify function SetIsFilled
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, SetIsFilledTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetIsFilled(true);
    EXPECT_TRUE(rsShadow->isFilled_);
}

/**
 * @tc.name: SetColorStrategyTest001
 * @tc.desc: Verify function SetColorStrategy
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, SetColorStrategyTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetColorStrategy(1);
    EXPECT_EQ(rsShadow->colorStrategy_, 1);
}

/**
 * @tc.name: GetColorTest001
 * @tc.desc: Verify function GetColor
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetColorTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_EQ(rsShadow->GetColor().GetBlue(), 0);
}

/**
 * @tc.name: GetOffsetXTest001
 * @tc.desc: Verify function GetOffsetX
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetOffsetXTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetOffsetX(), 1.0f);
}

/**
 * @tc.name: GetOffsetYTest001
 * @tc.desc: Verify function GetOffsetY
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetOffsetYTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetOffsetY(), 1.0f);
}

/**
 * @tc.name: GetAlphaTest001
 * @tc.desc: Verify function GetAlpha
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetAlphaTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetOffsetY(), 1.0f);
}

/**
 * @tc.name: GetElevationTest001
 * @tc.desc: Verify function GetElevation
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetElevationTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetElevation(), 1.0f);
}

/**
 * @tc.name: GetRadiusTest001
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetRadiusTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetRadius(), 1.0f);
}

/**
 * @tc.name: GetPathTest001
 * @tc.desc: Verify function GetPath
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetPathTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_EQ(rsShadow->GetPath(), nullptr);
}

/**
 * @tc.name: GetMaskTest001
 * @tc.desc: Verify function GetMask
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetMaskTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_FALSE(rsShadow->GetMask());
}

/**
 * @tc.name: GetIsFilledTest001
 * @tc.desc: Verify function GetIsFilled
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetIsFilledTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_FALSE(rsShadow->GetIsFilled());
}

/**
 * @tc.name: GetColorStrategyTest001
 * @tc.desc: Verify function GetColorStrategy
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, GetColorStrategyTest001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetColorStrategy(), 1);
}

/**
 * @tc.name: TestRSShadow002
 * @tc.desc: IsValid test.
 * @tc.type: FUNC
 */
HWTEST_F(RSShadowTest, TestRSShadow002, TestSize.Level1)
{
    RSShadow shadow;
    shadow.SetElevation(0.1);
    shadow.IsValid();
}
} // namespace OHOS::Rosen
