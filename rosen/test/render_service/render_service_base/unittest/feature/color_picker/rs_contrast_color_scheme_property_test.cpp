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

#include "feature/color_picker/rs_contrast_color_scheme_property.h"
#include "property/rs_color_picker_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSContrastColorSchemePropertyTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: DefaultConstructor
 * @tc.desc: Test default constructor initializes value to INVALID
 * @tc.type: FUNC
 */
HWTEST_F(RSContrastColorSchemePropertyTest, DefaultConstructor, TestSize.Level1)
{
    RSContrastColorSchemeProperty property;
    EXPECT_EQ(property.GetValue(), ContrastColorScheme::INVALID);
}

/**
 * @tc.name: MultipleInstancesIndependence
 * @tc.desc: Test multiple instances are independent
 * @tc.type: FUNC
 */
HWTEST_F(RSContrastColorSchemePropertyTest, MultipleInstancesIndependence, TestSize.Level1)
{
    RSContrastColorSchemeProperty property1(ContrastColorScheme::LIGHT);
    RSContrastColorSchemeProperty property2(ContrastColorScheme::DARK);
    RSContrastColorSchemeProperty property3;

    EXPECT_EQ(property1.GetValue(), ContrastColorScheme::LIGHT);
    EXPECT_EQ(property2.GetValue(), ContrastColorScheme::DARK);
    EXPECT_EQ(property3.GetValue(), ContrastColorScheme::INVALID);

    property1.SetValue(ContrastColorScheme::DARK);
    property2.SetValue(ContrastColorScheme::LIGHT);
    property3.SetValue(ContrastColorScheme::DARK);

    EXPECT_EQ(property1.GetValue(), ContrastColorScheme::DARK);
    EXPECT_EQ(property2.GetValue(), ContrastColorScheme::LIGHT);
    EXPECT_EQ(property3.GetValue(), ContrastColorScheme::DARK);
}

/**
 * @tc.name: AllEnumValuesCoverage
 * @tc.desc: Test all ContrastColorScheme enum values
 * @tc.type: FUNC
 */
HWTEST_F(RSContrastColorSchemePropertyTest, AllEnumValuesCoverage, TestSize.Level1)
{
    RSContrastColorSchemeProperty property;

    for (int i = 0; i <= static_cast<int>(ContrastColorScheme::DARK); ++i) {
        auto scheme = static_cast<ContrastColorScheme>(i);
        property.SetValue(scheme);
        EXPECT_EQ(property.GetValue(), scheme);
    }
}
} // namespace Rosen
} // namespace OHOS