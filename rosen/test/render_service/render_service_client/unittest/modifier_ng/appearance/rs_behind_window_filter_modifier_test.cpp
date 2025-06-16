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

#include "modifier_ng/appearance/rs_behind_window_filter_modifier.h"

using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen::ModifierNG {
class RSBehindWindowFilterModifierTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(RSBehindWindowFilterModifierTest, SetAndGetBehindWindowFilterRadiusTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBehindWindowFilterModifier> modifier =
        std::make_shared<ModifierNG::RSBehindWindowFilterModifier>();
    float radius = 6.6f;
    modifier->SetBehindWindowFilterRadius(radius);
    ASSERT_FLOAT_EQ(modifier->GetBehindWindowFilterRadius(), radius);
}

HWTEST_F(RSBehindWindowFilterModifierTest, SetAndGetBehindWindowFilterSaturationTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBehindWindowFilterModifier> modifier =
        std::make_shared<ModifierNG::RSBehindWindowFilterModifier>();
    float saturation = 0.88f;
    modifier->SetBehindWindowFilterSaturation(saturation);
    ASSERT_FLOAT_EQ(modifier->GetBehindWindowFilterSaturation(), saturation);
}

HWTEST_F(RSBehindWindowFilterModifierTest, SetAndGetBehindWindowFilterBrightnessTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBehindWindowFilterModifier> modifier =
        std::make_shared<ModifierNG::RSBehindWindowFilterModifier>();
    float saturation = 0.88f;
    modifier->SetBehindWindowFilterSaturation(saturation);
    ASSERT_FLOAT_EQ(modifier->GetBehindWindowFilterSaturation(), saturation);
}

HWTEST_F(RSBehindWindowFilterModifierTest, SetAndGetBehindWindowFilterMaskColorTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBehindWindowFilterModifier> modifier =
        std::make_shared<ModifierNG::RSBehindWindowFilterModifier>();
    Color testColor(100, 150, 200, 255);
    modifier->SetBehindWindowFilterMaskColor(testColor);
    Color result = modifier->GetBehindWindowFilterMaskColor();
    ASSERT_EQ(result, testColor);
}

HWTEST_F(RSBehindWindowFilterModifierTest, GetTypeTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBehindWindowFilterModifier> modifier =
        std::make_shared<ModifierNG::RSBehindWindowFilterModifier>();
    ASSERT_EQ(modifier->GetType(), RSModifierType::BEHIND_WINDOW_FILTER);
}

} // namespace OHOS::Rosen