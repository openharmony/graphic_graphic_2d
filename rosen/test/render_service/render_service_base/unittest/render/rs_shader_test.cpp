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

#include "render/rs_shader.h"
#include "effect/shader_effect_lazy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShaderTest::SetUpTestCase() {}
void RSShaderTest::TearDownTestCase() {}
void RSShaderTest::SetUp() {}
void RSShaderTest::TearDown() {}

/**
 * @tc.name: CreateRSShaderTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, CreateRSShaderTest001, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
}

/**
 * @tc.name: CreateRSShaderTest002
 * @tc.desc: Verify function CreateRSShader
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, CreateRSShaderTest002, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    EXPECT_EQ(rsShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: CreateRSShaderTest003
 * @tc.desc: Verify function CreateRSShader
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, CreateRSShaderTest003, TestSize.Level1)
{
    std::shared_ptr<Drawing::ShaderEffect> drShader;
    EXPECT_NE(RSShader::CreateRSShader(drShader), nullptr);
}

/**
 * @tc.name: CreateRSShaderTest004
 * @tc.desc: Verify function CreateRSShader
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, CreateRSShaderTest004, TestSize.Level1)
{
    std::vector<RSShader::ShaderType> shaderTypeVec;
    for (int i = 0; i < static_cast<int>(RSShader::ShaderType::COMPLEX); i++) {
        shaderTypeVec.push_back(static_cast<RSShader::ShaderType>(i));
    }
    for (const auto& shaderType : shaderTypeVec) {
        auto rsShader = RSShader::CreateRSShader(shaderType);
        EXPECT_NE(rsShader, nullptr);
    }
}

/**
 * @tc.name: SetSkShaderTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, SetSkShaderTest, TestSize.Level1)
{
    std::shared_ptr<RSShader> shaderPtr = RSShader::CreateRSShader();
    std::shared_ptr<Drawing::ShaderEffect> drawingShader;
    shaderPtr->SetDrawingShader(drawingShader);
    ASSERT_EQ(shaderPtr->drShader_, drawingShader);
}

/**
 * @tc.name: SetDrawingShaderLazyTest001
 * @tc.desc: Verify function SetDrawingShader with lazy shader effect
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, SetDrawingShaderLazyTest001, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
    auto normalDstShader = Drawing::ShaderEffect::CreateColorShader(0xFF0000FF);
    auto normalSrcShader = Drawing::ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader = Drawing::ShaderEffectLazy::CreateBlendShader(normalDstShader,
        normalSrcShader, Drawing::BlendMode::SRC_OVER);
    ASSERT_NE(lazyShader, nullptr);
    ASSERT_TRUE(lazyShader->IsLazy());

    // Set the lazy shader and verify it gets materialized
    rsShader->SetDrawingShader(lazyShader);

    // The stored shader should be the materialized version, not the lazy one
    auto storedShader = rsShader->GetDrawingShader();
    ASSERT_NE(storedShader, nullptr);
    ASSERT_FALSE(storedShader->IsLazy());
}

/**
 * @tc.name: MakeDrawingShaderWithProgressTest001
 * @tc.desc: Verify MakeDrawingShader(const RectF& rect, float progress) on base RSShader
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, MakeDrawingShaderWithProgressTest001, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
    RectF rect(0.0f, 0.0f, 100.0f, 100.0f);
    float progress = 0.5f;
    rsShader->MakeDrawingShader(rect, progress);
    EXPECT_EQ(rsShader->GetShaderType(), RSShader::ShaderType::DRAWING);
    EXPECT_EQ(rsShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderWithProgressTest002
 * @tc.desc: Verify MakeDrawingShader with zero rect and zero progress
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, MakeDrawingShaderWithProgressTest002, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
    RectF rect(0.0f, 0.0f, 0.0f, 0.0f);
    float progress = 0.0f;
    rsShader->MakeDrawingShader(rect, progress);
    EXPECT_EQ(rsShader->GetShaderType(), RSShader::ShaderType::DRAWING);
    EXPECT_EQ(rsShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderWithProgressTest003
 * @tc.desc: Verify MakeDrawingShader with negative progress
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, MakeDrawingShaderWithProgressTest003, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
    RectF rect(10.0f, 20.0f, 300.0f, 400.0f);
    float progress = -1.0f;
    rsShader->MakeDrawingShader(rect, progress);
    EXPECT_EQ(rsShader->GetShaderType(), RSShader::ShaderType::DRAWING);
    EXPECT_EQ(rsShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderWithProgressTest004
 * @tc.desc: Verify MakeDrawingShader does not modify existing shader
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, MakeDrawingShaderWithProgressTest004, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
    auto drShader = Drawing::ShaderEffect::CreateColorShader(0xFF0000FF);
    rsShader->SetDrawingShader(drShader);
    ASSERT_NE(rsShader->GetDrawingShader(), nullptr);

    RectF rect(0.0f, 0.0f, 50.0f, 50.0f);
    float progress = 0.5f;
    rsShader->MakeDrawingShader(rect, progress);
    EXPECT_NE(rsShader->GetDrawingShader(), nullptr);
    EXPECT_EQ(rsShader->GetShaderType(), RSShader::ShaderType::DRAWING);
}

/**
 * @tc.name: MakeDrawingShaderWithParamsTest001
 * @tc.desc: Verify MakeDrawingShader(const RectF& rect, const vector<float>& params) on base RSShader
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, MakeDrawingShaderWithParamsTest001, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
    RectF rect(0.0f, 0.0f, 100.0f, 100.0f);
    std::vector<float> params = { 0.5f, 0.5f, 0.5f, 0.5f };
    rsShader->MakeDrawingShader(rect, params);
    EXPECT_EQ(rsShader->GetShaderType(), RSShader::ShaderType::DRAWING);
    EXPECT_EQ(rsShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderWithParamsTest002
 * @tc.desc: Verify MakeDrawingShader with empty params vector
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, MakeDrawingShaderWithParamsTest002, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
    RectF rect(0.0f, 0.0f, 100.0f, 100.0f);
    std::vector<float> params;
    rsShader->MakeDrawingShader(rect, params);
    EXPECT_EQ(rsShader->GetShaderType(), RSShader::ShaderType::DRAWING);
    EXPECT_EQ(rsShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderWithParamsTest003
 * @tc.desc: Verify MakeDrawingShader with zero rect and single param
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, MakeDrawingShaderWithParamsTest003, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
    RectF rect(0.0f, 0.0f, 0.0f, 0.0f);
    std::vector<float> params = { 0.0f };
    rsShader->MakeDrawingShader(rect, params);
    EXPECT_EQ(rsShader->GetShaderType(), RSShader::ShaderType::DRAWING);
    EXPECT_EQ(rsShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderWithParamsTest004
 * @tc.desc: Verify MakeDrawingShader with params does not modify existing shader
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, MakeDrawingShaderWithParamsTest004, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
    auto drShader = Drawing::ShaderEffect::CreateColorShader(0xFF00FF00);
    rsShader->SetDrawingShader(drShader);
    ASSERT_NE(rsShader->GetDrawingShader(), nullptr);

    RectF rect(10.0f, 20.0f, 300.0f, 400.0f);
    std::vector<float> params = { 0.1f, 0.2f, 0.3f };
    rsShader->MakeDrawingShader(rect, params);
    EXPECT_NE(rsShader->GetDrawingShader(), nullptr);
    EXPECT_EQ(rsShader->GetShaderType(), RSShader::ShaderType::DRAWING);
}
} // namespace OHOS::Rosen
