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
#include "include/render/rs_shadow.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSShadowRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShadowRenderTest::SetUpTestCase() {}
void RSShadowRenderTest::TearDownTestCase() {}
void RSShadowRenderTest::SetUp() {}
void RSShadowRenderTest::TearDown() {}

/**
 * @tc.name: TestSetColor001
 * @tc.desc: Verify function TestSetColor001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestSetColor001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetColor(Color(1, 1, 1));
    EXPECT_EQ(rsShadow->color_.GetBlue(), 1);
}

/**
 * @tc.name: TestSetOffsetX001
 * @tc.desc: Verify function TestSetOffsetX001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestSetOffsetX001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetOffsetX(1.0f);
    EXPECT_EQ(rsShadow->offsetX_, 1.0f);
}

/**
 * @tc.name: TestSetOffsetY001
 * @tc.desc: Verify function TestSetOffsetY001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestSetOffsetY001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetOffsetY(1.0f);
    EXPECT_EQ(rsShadow->offsetY_, 1.0f);
}

/**
 * @tc.name: TestSetAlpha001
 * @tc.desc: Verify function TestSetAlpha001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestSetAlpha001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetAlpha(1.0f);
    EXPECT_EQ(rsShadow->color_.GetBlue(), 0);
}

/**
 * @tc.name: TestLifeCycle001
 * @tc.desc: Verify function TestLifeCycle001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestLifeCycle001, TestSize.Level1)
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
 * @tc.desc: IsValid test TestRSShadow001.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestRSShadow001, TestSize.Level1)
{
    RSShadow shadow;
    shadow.SetRadius(0.1);
    EXPECT_TRUE(shadow.IsValid());
}

/**
 * @tc.name: TestSetPath001
 * @tc.desc: Verify function TestSetPath001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestSetPath001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    auto path = std::make_shared<RSPath>();
    rsShadow->SetPath(path);
    EXPECT_NE(rsShadow->path_, nullptr);
}

/**
 * @tc.name: TestSetMask001
 * @tc.desc: Verify function TestSetMask001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestSetMask001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetMask(true);
    EXPECT_TRUE(rsShadow->imageMask_);
}

/**
 * @tc.name: TestSetIsFilled001
 * @tc.desc: Verify function TestSetIsFilled001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestSetIsFilled001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetIsFilled(true);
    EXPECT_TRUE(rsShadow->isFilled_);
}

/**
 * @tc.name: TestSetColorStrategy001
 * @tc.desc: Verify function TestSetColorStrategy001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestSetColorStrategy001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    rsShadow->SetColorStrategy(1);
    EXPECT_EQ(rsShadow->colorStrategy_, 1);
}

/**
 * @tc.name: TestGetColor001
 * @tc.desc: Verify function TestGetColor001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetColor001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_EQ(rsShadow->GetColor().GetBlue(), 0);
}

/**
 * @tc.name: TestGetOffsetX001
 * @tc.desc: Verify function TestGetOffsetX001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetOffsetX001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetOffsetX(), 1.0f);
}

/**
 * @tc.name: TestGetOffsetY001
 * @tc.desc: Verify function TestGetOffsetY001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetOffsetY001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetOffsetY(), 1.0f);
}

/**
 * @tc.name: TestGetAlpha001
 * @tc.desc: Verify function TestGetAlpha001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetAlpha001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetOffsetY(), 1.0f);
}

/**
 * @tc.name: TestGetElevation001
 * @tc.desc: Verify function TestGetElevation001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetElevation001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetElevation(), 1.0f);
}

/**
 * @tc.name: TestGetRadius001
 * @tc.desc: Verify function TestGetRadius001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetRadius001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetRadius(), 1.0f);
}

/**
 * @tc.name: TestGetPath001
 * @tc.desc: Verify function TestGetPath001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetPath001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_EQ(rsShadow->GetPath(), nullptr);
}

/**
 * @tc.name: TestGetMask001
 * @tc.desc: Verify function TestGetMask001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetMask001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_FALSE(rsShadow->GetMask());
}

/**
 * @tc.name: TestGetIsFilled001
 * @tc.desc: Verify function TestGetIsFilled001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetIsFilled001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_FALSE(rsShadow->GetIsFilled());
}

/**
 * @tc.name: TestGetColorStrategy001
 * @tc.desc: Verify function TestGetColorStrategy001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestGetColorStrategy001, TestSize.Level1)
{
    auto rsShadow = std::make_shared<RSShadow>();
    EXPECT_NE(rsShadow->GetColorStrategy(), 1);
}

/**
 * @tc.name: TestRSShadow002
 * @tc.desc: IsValid test TestRSShadow002.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestRSShadow002, TestSize.Level1)
{
    RSShadow shadow;
    shadow.SetElevation(0.1);
    EXPECT_FALSE(shadow.IsValid());
}

/**
 * @tc.name: TestRSShadow003
 * @tc.desc: GetColorStrategy test TestRSShadow003.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShadowRenderTest, TestRSShadow003, TestSize.Level1)
{
    RSShadow shadow;
    shadow.SetColorStrategy(SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE);
    EXPECT_EQ(shadow.GetColorStrategy(), 1);
}
} // namespace OHOS::Rosen
