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
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIShaderBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIShaderBaseTest::SetUpTestCase() {}
void RSUIShaderBaseTest::TearDownTestCase() {}
void RSUIShaderBaseTest::SetUp() {}
void RSUIShaderBaseTest::TearDown() {}

/**
 * @tc.name: RSNGShaderBaseCreate001
 * @tc.desc: Calling Create(RSNGEffectType::AIBAR_GLOW)
 *           should return a non-null RSNGAIBarGlow instance
 * @tc.type:FUNC
 */
HWTEST_F(RSUIShaderBaseTest, RSNGShaderBaseCreate001, TestSize.Level1)
{
    auto shader = RSNGShaderBase::Create(RSNGEffectType::AIBAR_GLOW);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::AIBAR_GLOW);
}

/**
 * @tc.name: RSNGShaderBaseCreate002
 * @tc.desc: Calling Create(RSNGEffectType::ROUNDED_RECT_FLOWLIGHT)
 *           should return a non-null RSNGRoundedRectFlowlight instance
 * @tc.type:FUNC
 */
HWTEST_F(RSUIShaderBaseTest, RSNGShaderBaseCreate001, TestSize.Level1)
{
    auto shader = RSNGShaderBase::Create(RSNGEffectType::ROUNDED_RECT_FLOWLIGHT);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::ROUNDED_RECT_FLOWLIGHT);
}


/**
 * @tc.name: RSNGShaderBaseCreate003
 * @tc.desc: Calling Create(RSNGEffectType::GRADIENT_FLOW_COLORS)
 *           should return a non-null RSNGGradientFlowColors instance
 * @tc.type:FUNC
 */
HWTEST_F(RSUIShaderBaseTest, RSNGShaderBaseCreate003, TestSize.Level1)
{
    auto shader = RSNGShaderBase::Create(RSNGEffectType::GRADIENT_FLOW_COLORS);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::GRADIENT_FLOW_COLORS);
}

} // namespace OHOS::Rosen
