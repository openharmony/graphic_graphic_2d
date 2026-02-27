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
#include "ui_effect/property/include/rs_ui_filter_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIFilterBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIFilterBaseTest::SetUpTestCase() {}
void RSUIFilterBaseTest::TearDownTestCase() {}
void RSUIFilterBaseTest::SetUp() {}
void RSUIFilterBaseTest::TearDown() {}

/**
 * @tc.name: CreateNGBlurFilter
 * @tc.desc: test for RSNGFilterHelper::CreateNGBlurFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGBlurFilter, TestSize.Level1)
{
    auto blurFilter = std::static_pointer_cast<RSNGBlurFilter>(
        RSNGFilterHelper::CreateNGBlurFilter(10.f, 10.f));
    EXPECT_NE(blurFilter, nullptr);
    float blurRadiusX = blurFilter->Getter<BlurRadiusXTag>()->Get();
    float blurRadiusY = blurFilter->Getter<BlurRadiusYTag>()->Get();
    EXPECT_FLOAT_EQ(blurRadiusX, 10.f);
    EXPECT_FLOAT_EQ(blurRadiusY, 10.f);
}

/**
 * @tc.name: CreateNGMaterialBlurFilter
 * @tc.desc: test for RSNGFilterHelper::CreateNGMaterialBlurFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGMaterialBlurFilter, TestSize.Level1)
{
    MaterialParam materialParam = { -50, -0.5, 0.5, Color(0x9fff0000), false };
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(
        RSNGFilterHelper::CreateNGMaterialBlurFilter(materialParam));
    EXPECT_NE(materialBlurFilter, nullptr);
    float radius = materialBlurFilter->Getter<MaterialBlurRadiusTag>()->Get();
    float saturation = materialBlurFilter->Getter<MaterialBlurSaturationTag>()->Get();
    float brightness = materialBlurFilter->Getter<MaterialBlurBrightnessTag>()->Get();
    EXPECT_FLOAT_EQ(radius, -50.f);
    EXPECT_FLOAT_EQ(saturation, -0.5);
    EXPECT_FLOAT_EQ(brightness, -0.5);
}
} // namespace OHOS::Rosen
