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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"

#include "modifier_ng/appearance/rs_blend_modifier.h"

using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen::ModifierNG {
class RSBlendModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(RSBlendModifierNGTypeTest, RSBlendModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBlendModifier> modifier = std::make_shared<ModifierNG::RSBlendModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::BLENDER);

    modifier->SetColorBlendMode(static_cast<RSColorBlendMode>(1));
    EXPECT_EQ(modifier->GetColorBlendMode(), static_cast<RSColorBlendMode>(1));

    modifier->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(2));
    EXPECT_EQ(modifier->GetColorBlendApplyType(), static_cast<RSColorBlendApplyType>(2));

    Vector4f rates{1.f, 2.f, 3.f, 4.f};
    modifier->SetFgBrightnessRates(rates);
    EXPECT_EQ(modifier->GetFgBrightnessRates(), rates);

    modifier->SetFgBrightnessSaturation(0.5f);
    EXPECT_FLOAT_EQ(modifier->GetFgBrightnessSaturation(), 0.5f);

    Vector4f posCoeff{0.1f, 0.2f, 0.3f, 0.4f};
    modifier->SetFgBrightnessPosCoeff(posCoeff);
    EXPECT_EQ(modifier->GetFgBrightnessPosCoeff(), posCoeff);

    Vector4f negCoeff{-1.f, -2.f, -3.f, -4.f};
    modifier->SetFgBrightnessNegCoeff(negCoeff);
    EXPECT_EQ(modifier->GetFgBrightnessNegCoeff(), negCoeff);

    modifier->SetFgBrightnessFract(0.75f);
    EXPECT_FLOAT_EQ(modifier->GetFgBrightnessFract(), 0.75f);
}

} // namespace OHOS::Rosen