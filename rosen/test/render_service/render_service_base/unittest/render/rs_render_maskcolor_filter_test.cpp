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
#include <parameter.h>
#include <parameters.h>

#include "inner_event.h"
#include "gtest/gtest.h"

#include "render/rs_render_maskcolor_filter.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSMaskColorShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaskColorShaderFilterTest::SetUpTestCase() {}
void RSMaskColorShaderFilterTest::TearDownTestCase() {}
void RSMaskColorShaderFilterTest::SetUp() {}
void RSMaskColorShaderFilterTest::TearDown() {}

/**
 * @tc.name: RSMaskColorShaderFilterConstructorTest001
 * @tc.desc: Test RSMaskColorShaderFilter constructor with PRE_DEFINED mode
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSMaskColorShaderFilterTest, RSMaskColorShaderFilterConstructorTest001, TestSize.Level1)
{
    RSColor maskColor(0x80, 0x40, 0x20, 0xFF);
    auto filter = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, maskColor);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetColorMode(), PRE_DEFINED);
    EXPECT_EQ(filter->GetMaskColor().AsArgbInt(), maskColor.AsArgbInt());
}

/**
 * @tc.name: RSMaskColorShaderFilterConstructorTest002
 * @tc.desc: Test RSMaskColorShaderFilter constructor with AVERAGE mode
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSMaskColorShaderFilterTest, RSMaskColorShaderFilterConstructorTest002, TestSize.Level1)
{
    RSColor maskColor(0x10, 0x20, 0x30, 0xC0);
    auto filter = std::make_shared<RSMaskColorShaderFilter>(AVERAGE, maskColor);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetColorMode(), AVERAGE);
}

/**
 * @tc.name: RSMaskColorShaderFilterHashConsistencyTest001
 * @tc.desc: Same color and colorMode should produce same hash
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSMaskColorShaderFilterTest, RSMaskColorShaderFilterHashConsistencyTest001, TestSize.Level1)
{
    RSColor maskColor(0x12, 0x34, 0x56, 0x78);
    auto filter1 = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, maskColor);
    auto filter2 = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, maskColor);
    ASSERT_NE(filter1, nullptr);
    ASSERT_NE(filter2, nullptr);
    EXPECT_EQ(filter1->Hash(), filter2->Hash());
}

/**
 * @tc.name: RSMaskColorShaderFilterHashDifferenceTest001
 * @tc.desc: Different maskColor should produce different hash
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSMaskColorShaderFilterTest, RSMaskColorShaderFilterHashDifferenceTest001, TestSize.Level1)
{
    RSColor color1(0x12, 0x34, 0x56, 0x78);
    RSColor color2(0x87, 0x65, 0x43, 0x21);
    auto filter1 = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, color1);
    auto filter2 = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, color2);
    ASSERT_NE(filter1, nullptr);
    ASSERT_NE(filter2, nullptr);
    EXPECT_NE(filter1->Hash(), filter2->Hash());
}

/**
 * @tc.name: RSMaskColorShaderFilterHashDifferenceTest002
 * @tc.desc: Different colorMode should produce different hash
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSMaskColorShaderFilterTest, RSMaskColorShaderFilterHashDifferenceTest002, TestSize.Level1)
{
    RSColor maskColor(0x12, 0x34, 0x56, 0x78);
    auto filter1 = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, maskColor);
    auto filter2 = std::make_shared<RSMaskColorShaderFilter>(AVERAGE, maskColor);
    ASSERT_NE(filter1, nullptr);
    ASSERT_NE(filter2, nullptr);
    EXPECT_NE(filter1->Hash(), filter2->Hash());
}

/**
 * @tc.name: RSMaskColorShaderFilterHashAlphaDifferenceTest001
 * @tc.desc: Same AsArgbInt but different raw GetAlpha should produce different hash
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSMaskColorShaderFilterTest, RSMaskColorShaderFilterHashAlphaDifferenceTest001, TestSize.Level1)
{
    RSColor color1(100, 100, 100, 255);
    RSColor color2(100, 100, 100, 300);
    ASSERT_EQ(color1.AsArgbInt(), color2.AsArgbInt());
    ASSERT_NE(color1.GetAlpha(), color2.GetAlpha());
    auto filter1 = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, color1);
    auto filter2 = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, color2);
    ASSERT_NE(filter1, nullptr);
    ASSERT_NE(filter2, nullptr);
    EXPECT_NE(filter1->Hash(), filter2->Hash());
}

/**
 * @tc.name: RSMaskColorShaderFilterGetPostProcessAlphaTest001
 * @tc.desc: GetPostProcessAlpha returns alpha divided by MAX_ALPHA
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSMaskColorShaderFilterTest, RSMaskColorShaderFilterGetPostProcessAlphaTest001, TestSize.Level1)
{
    RSColor maskColor(0x12, 0x34, 0x56, 0x80);
    auto filter = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, maskColor);
    ASSERT_NE(filter, nullptr);
    float expected = static_cast<float>(0x80) / 255.0f;
    EXPECT_FLOAT_EQ(filter->GetPostProcessAlpha(), expected);
}

/**
 * @tc.name: RSMaskColorShaderFilterNeedForceSubmitTest001
 * @tc.desc: NeedForceSubmit returns true for AVERAGE mode
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSMaskColorShaderFilterTest, RSMaskColorShaderFilterNeedForceSubmitTest001, TestSize.Level1)
{
    RSColor maskColor(0xFF, 0xFF, 0xFF, 0xFF);
    auto filter = std::make_shared<RSMaskColorShaderFilter>(AVERAGE, maskColor);
    ASSERT_NE(filter, nullptr);
    EXPECT_TRUE(filter->NeedForceSubmit());
}

/**
 * @tc.name: RSMaskColorShaderFilterNeedForceSubmitTest002
 * @tc.desc: NeedForceSubmit returns false for PRE_DEFINED mode
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSMaskColorShaderFilterTest, RSMaskColorShaderFilterNeedForceSubmitTest002, TestSize.Level1)
{
    RSColor maskColor(0xFF, 0xFF, 0xFF, 0xFF);
    auto filter = std::make_shared<RSMaskColorShaderFilter>(PRE_DEFINED, maskColor);
    ASSERT_NE(filter, nullptr);
    EXPECT_FALSE(filter->NeedForceSubmit());
}

} // namespace Rosen
} // namespace OHOS
