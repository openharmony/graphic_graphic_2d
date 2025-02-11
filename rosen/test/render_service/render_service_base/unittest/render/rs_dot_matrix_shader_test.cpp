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

#include "render/rs_dot_matrix_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDotMatrixShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDotMatrixShaderTest::SetUpTestCase() {}
void RSDotMatrixShaderTest::TearDownTestCase() {}
void RSDotMatrixShaderTest::SetUp() {}
void RSDotMatrixShaderTest::TearDown() {}

/**
 * @tc.name: CreateRSDotMatrixShaderTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, CreateRSDotMatrixShaderTest001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    EXPECT_EQ(testShader.GetShaderType(), RSShader::ShaderType::DOT_MATRIX);
}

/**
 * @tc.name: CreateRSDotMatrixShaderTest002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, CreateRSDotMatrixShaderTest002, TestSize.Level1)
{
    Drawing::Color color;
    RSDotMatrixShader testShader {color, 0.f, 0.f, color};
    EXPECT_EQ(testShader.GetShaderType(), RSShader::ShaderType::DOT_MATRIX);
}

/**
 * @tc.name: SetNormalParams001
 * @tc.desc: Verify function SetNormalParams
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, SetNormalParams001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    Drawing::Color color;
    testShader.SetNormalParams(color, 1.f, 1.f, color);
    EXPECT_NE(testShader.params_, nullptr);
}

/**
 * @tc.name: SetRotateEffect001
 * @tc.desc: Verify function SetRotateEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, SetRotateEffect001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RotateEffectParams params;
    testShader.SetRotateEffect(params);
    EXPECT_NE(testShader.params_, nullptr);
    EXPECT_EQ(testShader.params_->effectType_, DotMatrixEffectType::ROTATE);
}

/**
 * @tc.name: SetRippleEffect001
 * @tc.desc: Verify function SetRippleEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, SetRippleEffect001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RippleEffectParams params;
    testShader.SetRippleEffect(params);
    EXPECT_NE(testShader.params_, nullptr);
    EXPECT_EQ(testShader.params_->effectType_, DotMatrixEffectType::RIPPLE);
}

/**
 * @tc.name: SetNoneEffect001
 * @tc.desc: Verify function SetRippleEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, SetNoneEffect001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RippleEffectParams params;
    testShader.SetRippleEffect(params);
    EXPECT_NE(testShader.params_, nullptr);
    EXPECT_EQ(testShader.params_->effectType_, DotMatrixEffectType::RIPPLE);
    testShader.SetNoneEffect();
    EXPECT_NE(testShader.params_, nullptr);
    EXPECT_EQ(testShader.params_->effectType_, DotMatrixEffectType::NONE);
}

/**
 * @tc.name: MakeAndGetDrawingShader001
 * @tc.desc: Verify function MakeDrawingShader and GetDrawingShader
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeAndGetDrawingShader001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RippleEffectParams params;
    testShader.SetRippleEffect(params);
    RectF rect;
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MarshallingAndUnmarshalling001
 * @tc.desc: Verify function Marshalling and Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MarshallingAndUnmarshalling001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RippleEffectParams params;
    testShader.SetRippleEffect(params);
    Parcel parcel;
    EXPECT_TRUE(testShader.Marshalling(parcel));
    bool needReset = true;
    EXPECT_TRUE(testShader.Unmarshalling(parcel, needReset));
    EXPECT_FALSE(needReset);
}

} // namespace OHOS::Rosen
