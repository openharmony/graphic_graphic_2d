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

#include "feature/color_picker/rs_color_picker_utils.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorPickerUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorPickerUtilsTest::SetUpTestCase() {}
void RSColorPickerUtilsTest::TearDownTestCase() {}
void RSColorPickerUtilsTest::SetUp() {}
void RSColorPickerUtilsTest::TearDown() {}

/**
 * @tc.name: GetColorForPlaceholder
 * @tc.desc: test GetColorForPlaceholder
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, GetColorForPlaceholder, TestSize.Level1)
{
    auto colorPair = RSColorPickerUtils::GetColorForPlaceholder(ColorPlaceholder::FOREGROUND);
    EXPECT_EQ(colorPair.first, Drawing::Color::COLOR_BLACK);
    colorPair = RSColorPickerUtils::GetColorForPlaceholder(ColorPlaceholder::ACCENT);
    EXPECT_EQ(colorPair.first, Drawing::Color::COLOR_BLACK);
}

/**
 * @tc.name: InterpolateColor
 * @tc.desc: test InterpolateColor
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, InterpolateColor, TestSize.Level1)
{
    auto color = RSColorPickerUtils::InterpolateColor(0xFF000000, 0xFFFFFFFF, -1.f);
    EXPECT_EQ(color, 0xFFFFFFFF);
    color = RSColorPickerUtils::InterpolateColor(0xFF000000, 0xFFFFFFFF, 0.2f);
    EXPECT_EQ(color, 0xFF333333);
}

} // namespace OHOS::Rosen
