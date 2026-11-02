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

#include <gtest/gtest.h>

#include "effect/rs_render_effect_template.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

namespace OHOS {
namespace Rosen {

using namespace testing;
using namespace testing::ext;

class RSCosmosRingParaTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCosmosRingParaTest::SetUpTestCase() {}
void RSCosmosRingParaTest::TearDownTestCase() {}
void RSCosmosRingParaTest::SetUp() {}
void RSCosmosRingParaTest::TearDown() {}

/**
 * @tc.name: CreateByType
 * @tc.desc: Verify RSNGShaderBase::Create creates a CosmosRing shader by effect type
 * @tc.type: FUNC
 */
HWTEST_F(RSCosmosRingParaTest, CreateByType, TestSize.Level1)
{
    auto shader = RSNGShaderBase::Create(RSNGEffectType::COSMOS_RING);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::COSMOS_RING);

    auto cosmosRing = std::static_pointer_cast<RSNGCosmosRing>(shader);
    ASSERT_NE(cosmosRing, nullptr);
    EXPECT_FLOAT_EQ(cosmosRing->Getter<CosmosRingProgressTag>()->Get(), 0.0f);
    EXPECT_FLOAT_EQ(cosmosRing->Getter<CosmosRingRadiusTag>()->Get(), 150.0f);
    EXPECT_TRUE(cosmosRing->Getter<CosmosRingHlEnableTag>()->Get());
}

/**
 * @tc.name: SetterGetterRoundTrip
 * @tc.desc: Verify CosmosRing property tags store float, int, bool, Vector2f, Vector3f and Vector4f values
 * @tc.type: FUNC
 */
HWTEST_F(RSCosmosRingParaTest, SetterGetterRoundTrip, TestSize.Level1)
{
    auto shader = RSNGShaderBase::Create(RSNGEffectType::COSMOS_RING);
    ASSERT_NE(shader, nullptr);
    auto cosmosRing = std::static_pointer_cast<RSNGCosmosRing>(shader);
    ASSERT_NE(cosmosRing, nullptr);

    const Vector4f darkColor(0.1f, 0.2f, 0.3f, 0.4f);
    const Vector3f dispScales(0.8f, 0.0f, 1.4f);
    const Vector2f glowRadius(18.0f, 90.0f);

    cosmosRing->Setter<CosmosRingProgressTag>(0.5f);
    cosmosRing->Setter<CosmosRingRadiusTag>(256.0f);
    cosmosRing->Setter<CosmosRingNoiseSeedTag>(42);
    cosmosRing->Setter<CosmosRingHlEnableTag>(false);
    cosmosRing->Setter<CosmosRingDarkColorTag>(darkColor);
    cosmosRing->Setter<CosmosRingHlP2OuterDispScalesTag>(dispScales);
    cosmosRing->Setter<CosmosRingGlowRadiusTag>(glowRadius);

    EXPECT_FLOAT_EQ(cosmosRing->Getter<CosmosRingProgressTag>()->Get(), 0.5f);
    EXPECT_FLOAT_EQ(cosmosRing->Getter<CosmosRingRadiusTag>()->Get(), 256.0f);
    EXPECT_EQ(cosmosRing->Getter<CosmosRingNoiseSeedTag>()->Get(), 42);
    EXPECT_FALSE(cosmosRing->Getter<CosmosRingHlEnableTag>()->Get());
    EXPECT_EQ(cosmosRing->Getter<CosmosRingDarkColorTag>()->Get(), darkColor);
    EXPECT_EQ(cosmosRing->Getter<CosmosRingHlP2OuterDispScalesTag>()->Get(), dispScales);
    EXPECT_EQ(cosmosRing->Getter<CosmosRingGlowRadiusTag>()->Get(), glowRadius);
}

/**
 * @tc.name: EffectTypeString
 * @tc.desc: Verify CosmosRing maps to the expected effect type string
 * @tc.type: FUNC
 */
HWTEST_F(RSCosmosRingParaTest, EffectTypeString, TestSize.Level1)
{
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::COSMOS_RING), "CosmosRing");
}

/**
 * @tc.name: CreateRenderShaderByType
 * @tc.desc: Verify the server-side creator creates a CosmosRing render shader by effect type
 * @tc.type: FUNC
 */
HWTEST_F(RSCosmosRingParaTest, CreateRenderShaderByType, TestSize.Level1)
{
    auto renderShader = RSNGRenderShaderBase::Create(RSNGEffectType::COSMOS_RING);
    ASSERT_NE(renderShader, nullptr);
    EXPECT_EQ(renderShader->GetType(), RSNGEffectType::COSMOS_RING);
}

/**
 * @tc.name: AppendBuildsShaderChain
 * @tc.desc: Verify a CosmosRing shader can be appended to another shader and the chain is dumped
 * @tc.type: FUNC
 */
HWTEST_F(RSCosmosRingParaTest, AppendBuildsShaderChain, TestSize.Level1)
{
    auto head = RSNGShaderBase::Create(RSNGEffectType::COSMOS_RING);
    auto tail = RSNGShaderBase::Create(RSNGEffectType::COSMOS_RING);
    ASSERT_NE(head, nullptr);
    ASSERT_NE(tail, nullptr);

    EXPECT_TRUE(head->Append(tail));
    EXPECT_FALSE(head->Append(tail));

    auto renderEffect = head->GetRenderEffect();
    ASSERT_NE(renderEffect, nullptr);
    auto dump = renderEffect->Dump();
    EXPECT_NE(dump.find("CosmosRing"), std::string::npos);
    EXPECT_NE(dump.find("--"), std::string::npos);
}

} // namespace Rosen
} // namespace OHOS
