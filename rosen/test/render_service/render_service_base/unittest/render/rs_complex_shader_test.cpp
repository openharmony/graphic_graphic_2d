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

#include "render/rs_complex_shader.h"
#include "common/rs_common_def.h"
#include "effect/runtime_shader_builder.h"
#include "ge_visual_effect_impl.h"
#include "platform/common/rs_log.h"
#include "ge_external_dynamic_loader.h"
#include "ext/gex_complex_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSComplexShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSComplexShaderTest::SetUpTestCase() {}
void RSComplexShaderTest::TearDownTestCase() {}
void RSComplexShaderTest::SetUp() {}
void RSComplexShaderTest::TearDown() {}

/**
 * @tc.name: MakeDrawingShaderTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSComplexShaderTest, MakeDrawingShaderTest001, TestSize.Level0)
{
    auto rsComplexShader = std::make_shared<RSComplexShader>();
    const OHOS::Rosen::RectF boundsRect(0.0f, 0.0f, 1.0f, 1.0f);
    std::vector<float> tempVec = {0.5f, 0.5f, 0.5f, 0.5f};
    rsComplexShader->MakeDrawingShader(boundsRect, tempVec);
    EXPECT_EQ(rsComplexShader->shaderEffect_, nullptr);
}

/**
 * @tc.name: GetDrawingShaderTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSComplexShaderTest, GetDrawingShaderTest001, TestSize.Level1)
{
    auto rsComplexShader = std::make_shared<RSComplexShader>();
    EXPECT_EQ(rsComplexShader->GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MarshallingTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSComplexShaderTest, MarshallingTest001, TestSize.Level1)
{
    auto rsComplexShader = std::make_shared<RSComplexShader>();
    Parcel parcel;
    EXPECT_TRUE(rsComplexShader->Marshalling(parcel));
}

/**
 * @tc.name: UnmarshallingTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSComplexShaderTest, UnmarshallingTest001, TestSize.Level1)
{
    auto rsComplexShader = std::make_shared<RSComplexShader>();
    bool needReset = false;
    Parcel parcel;
    EXPECT_TRUE(rsComplexShader->Unmarshalling(parcel, needReset));
}

/**
 * @tc.name: GetShaderEffectTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSComplexShaderTest, GetShaderEffectTest001, TestSize.Level1)
{
    auto rsComplexShader = std::make_shared<RSComplexShader>();
    std::vector<float> effectParam = {0.0f, 1.0f};
    const Drawing::RectF boundsRect(0.0f, 0.0f, 1.0f, 1.0f);
    auto shaderEffect = rsComplexShader->GetShaderEffect(effectParam, boundsRect);
    EXPECT_EQ(shaderEffect, nullptr);
}

/**
 * @tc.name: GetShaderTypeTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSComplexShaderTest, GetShaderTypeTest001, TestSize.Level3)
{
    auto rsComplexShader = std::make_shared<RSComplexShader>(GexComlexShaderType::COLOR_GRADIENT);
    EXPECT_EQ(rsComplexShader->GetShaderType(), GexComlexShaderType::COLOR_GRADIENT);
}

} // namespace OHOS::Rosen
