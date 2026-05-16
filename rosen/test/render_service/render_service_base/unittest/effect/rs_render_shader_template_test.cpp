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


#include "gtest/gtest.h"

#include "effect/rs_render_effect_template.h"
#include "effect/rs_render_shape_base.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "ge_visual_effect_impl.h"
#include "render/rs_path.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderShaderTemplateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderShaderTemplateTest::SetUpTestCase() {}
void RSRenderShaderTemplateTest::TearDownTestCase() {}
void RSRenderShaderTemplateTest::SetUp() {}
void RSRenderShaderTemplateTest::TearDown() {}

/**
 * @tc.name: UpdateVisualEffectParamImplPathNull
 * @tc.desc: Test that the UpdateVisualEffectParamImpl with path is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderShaderTemplateTest, UpdateVisualEffectParamImplPathNull, TestSize.Level1)
{
    auto testEffect = std::make_shared<Drawing::GEVisualEffect>(
        RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_PATH_SHAPE));
    EXPECT_NE(testEffect, nullptr);
    std::shared_ptr<RSPath> testValue = nullptr;
    RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(*testEffect,
        Drawing::GE_SHAPE_SDF_PATH_SHAPE_PATH, testValue);
    auto impl = testEffect->GetImpl();
    ASSERT_NE(impl->GetSDFPathShapeParams(), nullptr);
    EXPECT_FALSE(impl->GetSDFPathShapeParams()->path.IsValid());
}

/**
 * @tc.name: UpdateVisualEffectParamImplPathValid
 * @tc.desc: Test that the UpdateVisualEffectParamImpl with path is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderShaderTemplateTest, UpdateVisualEffectParamImplPathValid, TestSize.Level1)
{
    auto testEffect = std::make_shared<Drawing::GEVisualEffect>(
        RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_PATH_SHAPE));
    EXPECT_NE(testEffect, nullptr);
    std::string pathStr = "M0 55 L50 10 L100 55 L50 200 L0 55Z"; // test path
    auto testValue = RSPath::CreateRSPath(pathStr);
    ASSERT_NE(testValue, nullptr);
    RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(*testEffect,
        Drawing::GE_SHAPE_SDF_PATH_SHAPE_PATH, testValue);
    auto impl = testEffect->GetImpl();
    ASSERT_NE(impl->GetSDFPathShapeParams(), nullptr);
    EXPECT_TRUE(impl->GetSDFPathShapeParams()->path.IsValid());
}

/**
 * @tc.name: CalculatePropTagHashImplPathNull
 * @tc.desc: Test that the CalculatePropTagHashImpl with path is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderShaderTemplateTest, CalculatePropTagHashImplPathNull, TestSize.Level1)
{
    uint32_t hash = 0;
    uint32_t originalHash = hash;
    std::shared_ptr<RSPath> value = nullptr;
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, value);
    EXPECT_EQ(hash, originalHash);
}

/**
 * @tc.name: CalculatePropTagHashImplPathValid
 * @tc.desc: Test that the CalculatePropTagHashImpl with path is valid
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderShaderTemplateTest, CalculatePropTagHashImplPathValid, TestSize.Level1)
{
    uint32_t hash = 0;
    uint32_t originalHash = hash;
    std::string pathStr = "M0 55 L50 10 L100 55 L50 200 L0 55Z"; // test path
    auto value = RSPath::CreateRSPath(pathStr);
    ASSERT_NE(value, nullptr);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, value);
    EXPECT_NE(hash, originalHash);
}
} // namespace OHOS::Rosen