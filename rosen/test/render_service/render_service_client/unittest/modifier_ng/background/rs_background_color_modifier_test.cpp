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
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/background/rs_background_color_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSBackgroundColorModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: RSBorderModifierTest
 * @tc.desc: Test Set/Get functions of RSBorderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSBackgroundColorModifierNGTypeTest, RSBackgroundColorModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBackgroundColorModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundColorModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::BACKGROUND_COLOR);

    Color testColor(15, 16, 14, 13);
    modifier->SetBackgroundColor(testColor);
    EXPECT_EQ(modifier->GetBackgroundColor(), testColor);

    Vector4f bgBrightnessRates(1.0f, 2.0f, 3.0f, 4.0f);
    modifier->SetBgBrightnessRates(bgBrightnessRates);
    EXPECT_EQ(modifier->GetBgBrightnessRates(), bgBrightnessRates);

    float bgBrightnessSaturation = 1.0f;
    modifier->SetBgBrightnessSaturation(bgBrightnessSaturation);
    EXPECT_EQ(modifier->GetBgBrightnessSaturation(), bgBrightnessSaturation);

    Vector4f bgBrightnessPosCoeff(1.0f, 2.0f, 3.0f, 4.0f);
    modifier->SetBgBrightnessPosCoeff(bgBrightnessPosCoeff);
    EXPECT_EQ(modifier->GetBgBrightnessPosCoeff(), bgBrightnessPosCoeff);

    Vector4f bgBrightnessNegCoeff(1.0f, 2.0f, 3.0f, 4.0f);
    modifier->SetBgBrightnessNegCoeff(bgBrightnessNegCoeff);
    EXPECT_EQ(modifier->GetBgBrightnessNegCoeff(), bgBrightnessNegCoeff);

    float bgBrightnessFract = 1.0f;
    modifier->SetBgBrightnessFract(bgBrightnessFract);
    EXPECT_EQ(modifier->GetBgBrightnessFract(), bgBrightnessFract);
}
} // namespace OHOS::Rosen