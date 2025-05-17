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

#include "render/rs_color_gradient_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSColorGradientShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorGradientShaderFilterTest::SetUpTestCase() {}
void RSColorGradientShaderFilterTest::TearDownTestCase() {}
void RSColorGradientShaderFilterTest::SetUp() {}
void RSColorGradientShaderFilterTest::TearDown() {}

/**
 * @tc.name: RSColorGradientShaderFilterTest001
 * @tc.desc: Verify function RSColorGradientShaderFilterTest
 * @tc.type:FUNC
 */
HWTEST_F(RSColorGradientShaderFilterTest, RSColorGradientShaderFilterTest001, TestSize.Level1)
{
    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0, 0.0, 0.0, 1.0 };
    std::vector<float> positions = { 1.0, 1.0 }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5 }; // 0.5 is strength params

    RSColorGradientShaderFilter filter = RSColorGradientShaderFilter(colors, positions, strengths, nullptr);
    EXPECT_EQ(filter.GetMask(), nullptr);
    EXPECT_FALSE(filter.GetColors().empty());
    EXPECT_FALSE(filter.GetPositions().empty());
    EXPECT_FALSE(filter.GetStrengths().empty());

    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    filter.GenerateGEVisualEffect(nullptr);
    filter.GenerateGEVisualEffect(container);
    EXPECT_FALSE(container->GetFilters().empty());
}
} // namespace Rosen
} // namespace OHOS