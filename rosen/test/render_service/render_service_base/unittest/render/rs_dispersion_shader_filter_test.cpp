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
#include "render/rs_dispersion_shader_filter.h"
#include "common/rs_vector2.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDispersionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDispersionFilterTest::SetUpTestCase() {}
void RSDispersionFilterTest::TearDownTestCase() {}
void RSDispersionFilterTest::SetUp() {}
void RSDispersionFilterTest::TearDown() {}

/**
 * @tc.name: CreateDispersionShaderFilter
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDispersionFilterTest, CreateDispersionShaderFilter, TestSize.Level1)
{
    DispersionShaderFilterParams para = { nullptr, 0.5f, Vector2f(0.5f, 0.5f), Vector2f(0.5f, 0.5f),
        Vector2f(0.5f, 0.5f) };
    auto filter = std::make_shared<RSDispersionShaderFilter>(para);
    EXPECT_EQ(filter->mask_, nullptr);

    para.mask = std::make_shared<RSShaderMask>(std::make_shared<RSRenderMaskPara>(RSUIFilterType::PIXEL_MAP_MASK));
    filter = std::make_shared<RSDispersionShaderFilter>(para);
    EXPECT_EQ(filter->mask_, para.mask);
}


/**
 * @tc.name: GenerateGEVisualEffect001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDispersionFilterTest, GenerateGEVisualEffect001, TestSize.Level1)
{
    DispersionShaderFilterParams para = { nullptr, 0.5f, Vector2f(0.5f, 0.5f), Vector2f(0.5f, 0.5f),
        Vector2f(0.5f, 0.5f) };

    auto filter = std::make_shared<RSDispersionShaderFilter>(para);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    filter->GenerateGEVisualEffect(nullptr);
    EXPECT_TRUE(visualEffectContainer->filterVec_.empty());

    filter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    para.mask = std::make_shared<RSShaderMask>(std::make_shared<RSRenderMaskPara>(RSUIFilterType::PIXEL_MAP_MASK));
    filter = std::make_shared<RSDispersionShaderFilter>(para);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GetMask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDispersionFilterTest, GetMask001, TestSize.Level1)
{
    auto mask = std::make_shared<RSShaderMask>(std::make_shared<RSRenderMaskPara>(RSUIFilterType::PIXEL_MAP_MASK));
    DispersionShaderFilterParams para = { mask, 0.5f, Vector2f(0.5f, 0.5f), Vector2f(0.5f, 0.5f),
        Vector2f(0.5f, 0.5f) };

    auto filter = std::make_shared<RSDispersionShaderFilter>(para);
    EXPECT_EQ(filter->GetMask(), mask);
}
} // namespace OHOS::Rosen