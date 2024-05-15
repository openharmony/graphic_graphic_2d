/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <memory>

#include "ge_render.h"
#include "ge_visual_effect.h"
#include "gtest/gtest.h"
#include "src/core/SkOpts.h"

#include "common/rs_optional_trace.h"
#include "draw/blend_mode.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_aibar_shader_filter.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_grey_shader_filter.h"
#include "render/rs_kawase_blur_shader_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"
#include "render/rs_maskcolor_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSDrawingFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrawingFilterTest::SetUpTestCase() {}
void RSDrawingFilterTest::TearDownTestCase() {}
void RSDrawingFilterTest::SetUp() {}
void RSDrawingFilterTest::TearDown() {}

/**
 * @tc.name: RSDrawingFilter001
 * @tc.desc: test results of RSDrawingFilter
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, RSDrawingFilter001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    EXPECT_TRUE(imageFilter != nullptr);
}

/**
 * @tc.name: GetDescription001
 * @tc.desc: test results of GetDescription
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, GetDescription001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto filterPtr = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSShaderFilter::NONE;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
}

/**
 * @tc.name: GetDetailedDescription001
 * @tc.desc: test results of GetDetailedDescription
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, GetDetailedDescription001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto filterPtr = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::GREY;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::MASK_COLOR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::NONE;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());
}

/**
 * @tc.name: Compose001
 * @tc.desc: test results of Compose
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, Compose001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    std::shared_ptr<RSDrawingFilter> other = nullptr;
    drawingFilter.Compose(other);
    EXPECT_TRUE(imageFilter != nullptr);

    other = std::make_shared<RSDrawingFilter>(imageFilter, shaderFilters, hash);
    drawingFilter.Compose(other);
    EXPECT_TRUE(other != nullptr);
}

/**
 * @tc.name: Compose002
 * @tc.desc: test results of Compose
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, Compose002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    std::shared_ptr<RSShaderFilter> other = nullptr;
    drawingFilter.Compose(other);
    EXPECT_TRUE(imageFilter != nullptr);

    other = std::make_shared<RSShaderFilter>();
    drawingFilter.Compose(other);
    EXPECT_TRUE(other != nullptr);
}

/**
 * @tc.name: Compose003
 * @tc.desc: test results of Compose
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, Compose003, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    std::shared_ptr<Drawing::ImageFilter> other = nullptr;
    drawingFilter.Compose(other, hash);
    EXPECT_TRUE(imageFilter != nullptr);

    other = std::make_shared<Drawing::ImageFilter>();
    drawingFilter.Compose(other, hash);
    EXPECT_TRUE(other != nullptr);
}

/**
 * @tc.name: ApplyColorFilter001
 * @tc.desc: test results of ApplyColorFilter
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, ApplyColorFilter001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    drawingFilter.ApplyColorFilter(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);
}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, DrawImageRect001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    drawingFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);

    filterPtr->type_ = RSShaderFilter::NONE;
    drawingFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);
}
} // namespace OHOS::Rosen
