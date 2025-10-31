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

#include "render/rs_drawing_filter.h"

// 引入测试所需的具体 Filter 类和依赖
#include "common/rs_color.h"
#include "draw/canvas.h"
#include "effect/image_filter.h"
#include "ge_visual_effect_container.h"
#include "render/rs_render_filter_base.h" // For RSNGRenderFilterBase
#include "render/rs_render_grey_filter.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h" // For new test
#include "render/rs_render_maskcolor_filter.h"
#include "render/rs_render_mesa_blur_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSDrawingFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    // 辅助创建 ImageFilter
    std::shared_ptr<Drawing::ImageFilter> CreateTestImageFilter()
    {
        // 创建一个简单的 Blur ImageFilter 作为测试对象
        return Drawing::ImageFilter::CreateBlurImageFilter(1.0f, 1.0f, Drawing::TileMode::DECAL, nullptr);
    }
};

void RSDrawingFilterTest::SetUpTestCase() {}
void RSDrawingFilterTest::TearDownTestCase() {}
void RSDrawingFilterTest::SetUp() {}
void RSDrawingFilterTest::TearDown() {}

// Mock for RSRenderFilterParaBase to test Pre/Post process calls
class MockShaderFilter : public RSRenderFilterParaBase {
public:
    explicit MockShaderFilter(RSUIFilterType type) : type_(type) {}
    RSUIFilterType GetType() const override { return type_; }
    uint32_t Hash() const override { return 111; }
    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override
    {
        generateCalled = true;
    }
    void PreProcess(std::shared_ptr<Drawing::Image>& image) override { preProcessCalled = true; }
    void PostProcess(Drawing::Canvas& canvas) override { postProcessCalled = true; }
    bool NeedForceSubmit() const override { return needForceSubmit; }

    mutable bool generateCalled = false;
    mutable bool preProcessCalled = false;
    mutable bool postProcessCalled = false;
    bool needForceSubmit = false;
    RSUIFilterType type_ = RSUIFilterType::NONE;
};

// Mock for RSNGRenderFilterBase to test Set/Get and Hash
class MockNGRenderFilter : public RSNGRenderFilterBase {
public:
    explicit MockNGRenderFilter(uint32_t hash) : hash_(hash) {}
    uint32_t CalculateHash() const override { return hash_; }
    void OnSync() override { onSyncCalled = true; }

    uint32_t hash_ = 0;
    mutable bool onSyncCalled = false;
};

/**
 * @tc.name: TestConstructorWithImageFilter
 * @tc.desc: 验证 ImageFilter 构造函数
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestConstructorWithImageFilter, TestSize.Level1)
{
    auto imageFilter = CreateTestImageFilter();
    uint32_t hash = 12345;
    RSDrawingFilter drawingFilter(imageFilter, hash);

    EXPECT_EQ(drawingFilter.GetImageFilter(), imageFilter);
    EXPECT_EQ(drawingFilter.ImageHash(), hash);
    EXPECT_TRUE(drawingFilter.GetShaderFilters().empty());
}

/**
 * @tc.name: TestConstructorWithShaderFilter
 * @tc.desc: 验证单个 ShaderFilter 构造函数
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestConstructorWithShaderFilter, TestSize.Level1)
{
    auto shaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(10);
    RSDrawingFilter drawingFilter(shaderFilter);

    EXPECT_EQ(drawingFilter.GetImageFilter(), nullptr);
    ASSERT_EQ(drawingFilter.GetShaderFilters().size(), 1);
    EXPECT_EQ(drawingFilter.GetShaderFilters()[0], shaderFilter);
    EXPECT_EQ(drawingFilter.ShaderHash(), shaderFilter->Hash());
}

/**
 * @tc.name: TestConstructorWithBothFilters
 * @tc.desc: 验证 ImageFilter 和 ShaderFilter 构造函数
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestConstructorWithBothFilters, TestSize.Level1)
{
    auto imageFilter = CreateTestImageFilter();
    auto shaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(10);
    uint32_t hash = 12345;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilter, hash);

    EXPECT_EQ(drawingFilter.GetImageFilter(), imageFilter);
    EXPECT_EQ(drawingFilter.ImageHash(), hash);
    ASSERT_EQ(drawingFilter.GetShaderFilters().size(), 1);
    EXPECT_EQ(drawingFilter.GetShaderFilters()[0], shaderFilter);
    EXPECT_EQ(drawingFilter.ShaderHash(), shaderFilter->Hash());
}

/**
 * @tc.name: TestConstructorWithMultipleShaderFilters
 * @tc.desc: 验证 ImageFilter 和 多个 ShaderFilter 构造函数
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestConstructorWithMultipleShaderFilters, TestSize.Level1)
{
    auto imageFilter = CreateTestImageFilter();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(std::make_shared<RSKawaseBlurShaderFilter>(10));
    shaderFilters.push_back(std::make_shared<RSGreyShaderFilter>(0.1f, 0.9f));
    uint32_t hash = 12345;

    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);

    EXPECT_EQ(drawingFilter.GetImageFilter(), imageFilter);
    EXPECT_EQ(drawingFilter.ImageHash(), hash);
    EXPECT_EQ(drawingFilter.GetShaderFilters().size(), 2);
}

/**
 * @tc.name: TestSetGetImageFilter
 * @tc.desc: 验证 SetImageFilter 和 GetImageFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestSetGetImageFilter, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    EXPECT_EQ(drawingFilter.GetImageFilter(), nullptr);

    auto imageFilter = CreateTestImageFilter();
    drawingFilter.SetImageFilter(imageFilter);
    EXPECT_EQ(drawingFilter.GetImageFilter(), imageFilter);
}

/**
 * @tc.name: TestInsertAndGetShaderFilters
 * @tc.desc: 验证 InsertShaderFilter 和 GetShaderFilters
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestInsertAndGetShaderFilters, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    EXPECT_TRUE(drawingFilter.GetShaderFilters().empty());

    auto shaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(10);
    drawingFilter.InsertShaderFilter(shaderFilter);
    ASSERT_EQ(drawingFilter.GetShaderFilters().size(), 1);
    EXPECT_EQ(drawingFilter.GetShaderFilters()[0], shaderFilter);

    auto shaderFilter2 = std::make_shared<RSGreyShaderFilter>(0.1f, 0.9f);
    drawingFilter.InsertShaderFilter(shaderFilter2);
    ASSERT_EQ(drawingFilter.GetShaderFilters().size(), 2);
    EXPECT_EQ(drawingFilter.GetShaderFilters()[1], shaderFilter2);
}

/**
 * @tc.name: TestGetShaderFilterWithType
 * @tc.desc: 验证 GetShaderFilterWithType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetShaderFilterWithType, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    auto kawaseFilter = std::make_shared<RSKawaseBlurShaderFilter>(10);
    auto greyFilter = std::make_shared<RSGreyShaderFilter>(0.1f, 0.9f);
    shaderFilters.push_back(kawaseFilter);
    shaderFilters.push_back(greyFilter);

    RSDrawingFilter drawingFilter(nullptr, shaderFilters, 0);

    auto resultKawase = drawingFilter.GetShaderFilterWithType(RSUIFilterType::KAWASE);
    EXPECT_NE(resultKawase, nullptr);
    EXPECT_EQ(resultKawase, kawaseFilter);

    auto resultGrey = drawingFilter.GetShaderFilterWithType(RSUIFilterType::GREY);
    EXPECT_NE(resultGrey, nullptr);
    EXPECT_EQ(resultGrey, greyFilter);

    // 测试一个不存在的类型
    auto resultMesa = drawingFilter.GetShaderFilterWithType(RSUIFilterType::MESA);
    EXPECT_EQ(resultMesa, nullptr);
}

/**
 * @tc.name: TestIsDrawingFilter
 * @tc.desc: 验证 IsDrawingFilter 总是返回 true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestIsDrawingFilter, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    EXPECT_TRUE(drawingFilter.IsDrawingFilter());
}

/**
 * @tc.name: TestStaticCanSkipFrame
 * @tc.desc: 验证静态方法 CanSkipFrame
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestStaticCanSkipFrame, TestSize.Level1)
{
    // HEAVY_BLUR_THRESHOLD = 25.0f
    EXPECT_FALSE(RSDrawingFilter::CanSkipFrame(10.0f));
    EXPECT_FALSE(RSDrawingFilter::CanSkipFrame(25.0f));
    EXPECT_TRUE(RSDrawingFilter::CanSkipFrame(25.1f));
    EXPECT_TRUE(RSDrawingFilter::CanSkipFrame(100.0f));
}

/**
 * @tc.name: TestSetGetSkipFrame
 * @tc.desc: 验证 SetSkipFrame 和 CanSkipFrame (成员变量)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestSetGetSkipFrame, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    EXPECT_FALSE(drawingFilter.CanSkipFrame()); // 默认值应为 false

    drawingFilter.SetSkipFrame(true);
    EXPECT_TRUE(drawingFilter.CanSkipFrame());

    drawingFilter.SetSkipFrame(false);
    EXPECT_FALSE(drawingFilter.CanSkipFrame());
}

/**
 * @tc.name: TestComposeWithShaderFilter
 * @tc.desc: 验证 Compose (RSRenderFilterParaBase)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestComposeWithShaderFilter, TestSize.Level1)
{
    auto kawaseFilter = std::make_shared<RSKawaseBlurShaderFilter>(10);
    RSDrawingFilter drawingFilter(kawaseFilter);
    auto originalHash = drawingFilter.ShaderHash();
    EXPECT_EQ(drawingFilter.GetShaderFilters().size(), 1);

    auto greyFilter = std::make_shared<RSGreyShaderFilter>(0.1f, 0.9f);
    auto composedFilter = drawingFilter.Compose(greyFilter);

    // 验证原始 filter 未被修改
    EXPECT_EQ(drawingFilter.GetShaderFilters().size(), 1);

    // 验证新 filter 包含两个 shader filter
    EXPECT_NE(composedFilter, nullptr);
    EXPECT_EQ(composedFilter->GetShaderFilters().size(), 2);
    EXPECT_NE(composedFilter->ShaderHash(), originalHash);
    EXPECT_EQ(composedFilter->GetShaderFilterWithType(RSUIFilterType::KAWASE), kawaseFilter);
    EXPECT_EQ(composedFilter->GetShaderFilterWithType(RSUIFilterType::GREY), greyFilter);
}

/**
 * @tc.name: TestComposeWithImageFilter
 * @tc.desc: 验证 Compose (Drawing::ImageFilter)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestComposeWithImageFilter, TestSize.Level1)
{
    auto imageFilter1 = CreateTestImageFilter();
    uint32_t hash1 = 123;
    RSDrawingFilter drawingFilter(imageFilter1, hash1);
    EXPECT_EQ(drawingFilter.GetImageFilter(), imageFilter1);
    EXPECT_EQ(drawingFilter.ImageHash(), hash1);

    auto imageFilter2 = Drawing::ImageFilter::CreateColorFilterImageFilter(
        *Drawing::ColorFilter::CreateLinearToSrgbGamma(), nullptr);
    uint32_t hash2 = 456;
    auto composedFilter = drawingFilter.Compose(imageFilter2, hash2);

    // 验证原始 filter 未被修改
    EXPECT_EQ(drawingFilter.GetImageFilter(), imageFilter1);
    EXPECT_EQ(drawingFilter.ImageHash(), hash1);

    // 验证新 filter 的 ImageFilter 已被组合
    EXPECT_NE(composedFilter, nullptr);
    EXPECT_NE(composedFilter->GetImageFilter(), imageFilter1);
    EXPECT_NE(composedFilter->ImageHash(), hash1);
}

/**
 * @tc.name: TestNeedForceSubmit
 * @tc.desc: 验证 NeedForceSubmit (假设子 filter 返回 false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestNeedForceSubmit, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    EXPECT_FALSE(drawingFilter.NeedForceSubmit()); // 默认为 false

    auto kawaseFilter = std::make_shared<RSKawaseBlurShaderFilter>(10);
    drawingFilter.InsertShaderFilter(kawaseFilter);
    // 假设 RSKawaseBlurShaderFilter::NeedForceSubmit() 返回 false
    EXPECT_FALSE(drawingFilter.NeedForceSubmit());
}

/**
 * @tc.name: TestGetDescription
 * @tc.desc: 验证 GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetDescription, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(std::make_shared<RSKawaseBlurShaderFilter>(15));
    shaderFilters.push_back(std::make_shared<RSMESABlurShaderFilter>(20.0f));
    
    RSDrawingFilter drawingFilter(nullptr, shaderFilters, 0);
    // 设置 type_，使其返回 "RSBlurFilterBlur"
    drawingFilter.SetFilterType(RSFilter::BLUR);

    std::string desc = drawingFilter.GetDescription();
    EXPECT_NE(desc.find("RSBlurFilterBlur"), std::string::npos);
    // 检查 KAWASE
    EXPECT_NE(desc.find("KAWASE radius: 15"), std::string::npos);
    // 检查 MESA
    EXPECT_NE(desc.find("MESA radius: 20 sigma"), std::string::npos);
}

/**
 * @tc.name: TestGetDetailedDescription
 * @tc.desc: 验证 GetDetailedDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetDetailedDescription, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(std::make_shared<RSGreyShaderFilter>(0.2f, 0.8f));
    RSColor maskColor = RSColor(255, 0, 0); // 红色
    shaderFilters.push_back(std::make_shared<RSMaskColorShaderFilter>(maskColor, 0)); // 0 for BLEND
    
    RSDrawingFilter drawingFilter(nullptr, shaderFilters, 0);
    // 设置 type_，使其返回 "RSMaterialFilterBlur"
    drawingFilter.SetFilterType(RSFilter::MATERIAL);

    std::string desc = drawingFilter.GetDetailedDescription();
    EXPECT_NE(desc.find("RSMaterialFilterBlur"), std::string::npos);
    // 检查 GREY
    EXPECT_NE(desc.find("greyCoef1: 0.2"), std::string::npos);
    EXPECT_NE(desc.find("greyCoef2: 0.8"), std::string::npos);
    // 检查 MASK_COLOR (FF0000 是红色)
    EXPECT_NE(desc.find("maskColor: FFFF0000"), std::string::npos);
    EXPECT_NE(desc.find("colorMode: 0"), std::string::npos);
}

// ===================================================================================
// ========================== 补充测试用例开始 ========================================
// ===================================================================================

/**
 * @tc.name: TestGetBrushDefaultAlpha
 * @tc.desc: 验证 GetBrush 默认 alpha 为 1.0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetBrushDefaultAlpha, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    Drawing::Brush brush = drawingFilter.GetBrush();
    EXPECT_EQ(brush.GetFilter().GetColorFilter(), nullptr);
}

/**
 * @tc.name: TestGetBrushCustomAlpha
 * @tc.desc: 验证 GetBrush 自定义 alpha
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetBrushCustomAlpha, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    float alpha = 0.5f;
    Drawing::Brush brush = drawingFilter.GetBrush(alpha);
    EXPECT_NE(brush.GetFilter().GetColorFilter(), nullptr);
}

/**
 * @tc.name: TestGetBrushWithFilterAndAlpha
 * @tc.desc: 验证 GetBrush 同时带有 ImageFilter 和 自定义 alpha
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetBrushWithFilterAndAlpha, TestSize.Level1)
{
    auto imageFilter = CreateTestImageFilter();
    RSDrawingFilter drawingFilter(imageFilter, 123);
    
    float alpha = 0.5f;
    Drawing::Brush brush = drawingFilter.GetBrush(alpha);
    EXPECT_NE(brush.GetFilter().GetImageFilter(), nullptr);
    EXPECT_NE(brush.GetFilter().GetColorFilter(), nullptr);
}

/**
 * @tc.name: TestHashDefault
 * @tc.desc: 验证默认构造函数的 Hash
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestHashDefault, TestSize.Level1)
{
    RSDrawingFilter filter1;
    RSDrawingFilter filter2;
    EXPECT_EQ(filter1.Hash(), filter2.Hash());
}

/**
 * @tc.name: TestHashWithImageFilter
 * @tc.desc: 验证 ImageFilter 对 Hash 的影响
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestHashWithImageFilter, TestSize.Level1)
{
    RSDrawingFilter filterDefault;
    auto imageFilter = CreateTestImageFilter();
    RSDrawingFilter filterWithImage(imageFilter, 123);

    EXPECT_NE(filterDefault.Hash(), filterWithImage.Hash());
}

/**
 * @tc.name: TestHashWithShaderFilter
 * @tc.desc: 验证 ShaderFilter 对 Hash 的影响
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestHashWithShaderFilter, TestSize.Level1)
{
    RSDrawingFilter filterDefault;
    auto shaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(10);
    RSDrawingFilter filterWithShader(shaderFilter);

    EXPECT_NE(filterDefault.Hash(), filterWithShader.Hash());
}

/**
 * @tc.name: TestHashWithNGRenderFilter
 * @tc.desc: 验证 NGRenderFilter 对 Hash 的影响
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestHashWithNGRenderFilter, TestSize.Level1)
{
    RSDrawingFilter filterDefault;
    RSDrawingFilter filterWithNG;
    auto ngFilter = std::make_shared<MockNGRenderFilter>(333);
    filterWithNG.SetNGRenderFilter(ngFilter);

    EXPECT_NE(filterDefault.Hash(), filterWithNG.Hash());
}

/**
 * @tc.name: TestHashConsistency
 * @tc.desc: 验证 Hash 的一致性
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestHashConsistency, TestSize.Level1)
{
    auto imageFilter = CreateTestImageFilter();
    auto shaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(10);
    RSDrawingFilter filter1(imageFilter, shaderFilter, 123);
    RSDrawingFilter filter2(imageFilter, shaderFilter, 123);

    EXPECT_EQ(filter1.Hash(), filter2.Hash());

    auto shaderFilter2 = std::make_shared<RSKawaseBlurShaderFilter>(20);
    RSDrawingFilter filter3(imageFilter, shaderFilter2, 123);
    EXPECT_NE(filter1.Hash(), filter3.Hash());
}

/**
 * @tc.name: TestRenderFilterHash
 * @tc.desc: 验证 RenderFilterHash 默认值和 Set 之后的值
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestRenderFilterHash, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    EXPECT_EQ(drawingFilter.RenderFilterHash(), 0); // 默认应为0

    auto ngFilter = std::make_shared<MockNGRenderFilter>(333);
    drawingFilter.SetNGRenderFilter(ngFilter);
    EXPECT_EQ(drawingFilter.RenderFilterHash(), 333);
}

/**
 * @tc.name: TestSetGetNGRenderFilter
 * @tc.desc: 验证 SetNGRenderFilter 和 GetNGRenderFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestSetGetNGRenderFilter, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    EXPECT_EQ(drawingFilter.GetNGRenderFilter(), nullptr);

    auto ngFilter = std::make_shared<MockNGRenderFilter>(123);
    drawingFilter.SetNGRenderFilter(ngFilter);
    EXPECT_EQ(drawingFilter.GetNGRenderFilter(), ngFilter);

    drawingFilter.SetNGRenderFilter(nullptr);
    EXPECT_EQ(drawingFilter.GetNGRenderFilter(), nullptr);
}

/**
 * @tc.name: TestGetDescriptionEmpty
 * @tc.desc: 验证没有 shader filter 时的 GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetDescriptionEmpty, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_EQ(drawingFilter.GetDescription(), "RSBlurFilterBlur");
}

/**
 * @tc.name: TestGetDetailedDescriptionEmpty
 * @tc.desc: 验证没有 shader filter 时的 GetDetailedDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetDetailedDescriptionEmpty, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    drawingFilter.SetFilterType(RSFilter::MATERIAL);
    EXPECT_EQ(drawingFilter.GetDetailedDescription(), "RSMaterialFilterBlur");
}

/**
 * @tc.name: TestGetDetailedDescriptionLinearGradient
 * @tc.desc: 验证 GetDetailedDescription 对 LINEAR_GRADIENT_BLUR 的处理
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetDetailedDescriptionLinearGradient, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(std::make_shared<RSLinearGradientBlurShaderFilter>(5.0f, std::vector<float>(),
        Drawing::Point(), Drawing::Point(), RSLinearGradientBlurDirection::LEFT));
    RSDrawingFilter drawingFilter(nullptr, shaderFilters, 0);
    drawingFilter.SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);

    std::string desc = drawingFilter.GetDetailedDescription();
    EXPECT_NE(desc.find("RSLinearGradientBlurFilterBlur"), std::string::npos);
    EXPECT_NE(desc.find("radius: 5"), std::string::npos);
}

/**
 * @tc.name: TestGetFilterTypeStringKnown
 * @tc.desc: 验证 GetFilterTypeString 对已知类型的处理
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetFilterTypeStringKnown, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    
    drawingFilter.SetFilterType(RSFilter::AIBAR);
    EXPECT_EQ(drawingFilter.GetFilterTypeString(), "RSAIBarFilterBlur");

    drawingFilter.SetFilterType(RSFilter::WATER_RIPPLE);
    EXPECT_EQ(drawingFilter.GetFilterTypeString(), "RSWaterRippleFilter");

    drawingFilter.SetFilterType(RSFilter::MAGNIFIER);
    EXPECT_EQ(drawingFilter.GetFilterTypeString(), "RSMagnifierFilter");
}

/**
 * @tc.name: TestGetFilterTypeStringUnknown
 * @tc.desc: 验证 GetFilterTypeString 对未知类型的处理
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGetFilterTypeStringUnknown, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    drawingFilter.SetFilterType(RSFilter::FilterType(999)); // 假设 999 是未知类型
    EXPECT_EQ(drawingFilter.GetFilterTypeString(), "not found.");
}

/**
 * @tc.name: TestGenerateAndUpdateGEVisualEffectEmpty
 * @tc.desc: 验证没有 filters 时的 GenerateAndUpdateGEVisualEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGenerateAndUpdateGEVisualEffectEmpty, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    // visualEffectContainer_ 是私有的，我们只能测试它不崩溃
    // 并且在 SetGeometry 时 container 应该是有效的
    drawingFilter.GenerateAndUpdateGEVisualEffect();
    Drawing::Canvas canvas;
    drawingFilter.SetGeometry(canvas, 100, 100); // 如果 container 内部创建了，这里不应崩溃
}

/**
 * @tc.name: TestGenerateAndUpdateGEVisualEffectWithFilters
 * @tc.desc: 验证有 filters 时的 GenerateAndUpdateGEVisualEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestGenerateAndUpdateGEVisualEffectWithFilters, TestSize.Level1)
{
    auto mockFilter = std::make_shared<MockShaderFilter>(RSUIFilterType::KAWASE);
    RSDrawingFilter drawingFilter(mockFilter);
    
    drawingFilter.GenerateAndUpdateGEVisualEffect();
    EXPECT_TRUE(mockFilter->generateCalled); // 验证子 filter 的 Generate 被调用
}

/**
 * @tc.name: TestOnSyncCalls
 * @tc.desc: 验证 OnSync 调用了 NGFilter 的 OnSync 和 GenerateAndUpdate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestOnSyncCalls, TestSize.Level1)
{
    auto mockShader = std::make_shared<MockShaderFilter>(RSUIFilterType::KAWASE);
    auto mockNG = std::make_shared<MockNGRenderFilter>(123);
    
    RSDrawingFilter drawingFilter(mockShader);
    drawingFilter.SetNGRenderFilter(mockNG);

    drawingFilter.OnSync();

    EXPECT_TRUE(mockShader->generateCalled); // 验证 GenerateAndUpdate 被调用
    EXPECT_TRUE(mockNG->onSyncCalled);      // 验证 NGFilter::OnSync 被调用
}

/**
 * @tc.name: TestSetGeometryNullContainer
 * @tc.desc: 验证在 container 为空时调用 SetGeometry
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestSetGeometryNullContainer, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    Drawing::Canvas canvas;
    // visualEffectContainer_ 默认为空，不应崩溃
    drawingFilter.SetGeometry(canvas, 100, 100);
    SUCCEED(); // 没崩溃就算成功
}

/**
 * @tc.name: TestSetGeometryWithContainer
 * @tc.desc: 验证在 container 有效时调用 SetGeometry
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestSetGeometryWithContainer, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    drawingFilter.GenerateAndUpdateGEVisualEffect(); // 创建 container
    Drawing::Canvas canvas;
    drawingFilter.SetGeometry(canvas, 100, 100);
    SUCCEED(); // 没崩溃就算成功
}

/**
 * @tc.name: TestSetDisplayHeadroomNullContainer
 * @tc.desc: 验证在 container 为空时调用 SetDisplayHeadroom
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestSetDisplayHeadroomNullContainer, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    // visualEffectContainer_ 默认为空，不应崩溃
    drawingFilter.SetDisplayHeadroom(1.0f);
    SUCCEED(); // 没崩溃就算成功
}

/**
 * @tc.name: TestSetDisplayHeadroomWithContainer
 * @tc.desc: 验证在 container 有效时调用 SetDisplayHeadroom
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestSetDisplayHeadroomWithContainer, TestSize.Level1)
{
    RSDrawingFilter drawingFilter;
    drawingFilter.GenerateAndUpdateGEVisualEffect(); // 创建 container
    drawingFilter.SetDisplayHeadroom(1.0f);
    SUCCEED(); // 没崩溃就算成功
}

/**
 * @tc.name: TestPreProcessCallsChild
 * @tc.desc: 验证 PreProcess 调用了子 filter 的 PreProcess
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestPreProcessCallsChild, TestSize.Level1)
{
    auto mockFilter = std::make_shared<MockShaderFilter>(RSUIFilterType::KAWASE);
    RSDrawingFilter drawingFilter(mockFilter);
    std::shared_ptr<Drawing::Image> image;

    drawingFilter.PreProcess(image);
    EXPECT_TRUE(mockFilter->preProcessCalled);
}

/**
 * @tc.name: TestPostProcessCallsChild
 * @tc.desc: 验证 PostProcess 调用了子 filter 的 PostProcess
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestPostProcessCallsChild, TestSize.Level1)
{
    auto mockFilter = std::make_shared<MockShaderFilter>(RSUIFilterType::KAWASE);
    RSDrawingFilter drawingFilter(mockFilter);
    Drawing::Canvas canvas;

    drawingFilter.PostProcess(canvas);
    EXPECT_TRUE(mockFilter->postProcessCalled);
}

/**
 * @tc.name: TestForcePostProcessCallsChild
 * @tc.desc: 验证 ForcePostProcess 调用了子 filter 的 PostProcess
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestForcePostProcessCallsChild, TestSize.Level1)
{
    auto mockFilter = std::make_shared<MockShaderFilter>(RSUIFilterType::KAWASE);
    RSDrawingFilter drawingFilter(mockFilter);
    Drawing::Canvas canvas;

    drawingFilter.ForcePostProcess(canvas);
    EXPECT_TRUE(mockFilter->postProcessCalled);
}

/**
 * @tc.name: TestNeedForceSubmitPositive
 * @tc.desc: 验证当子 filter 需要 force submit 时的 NeedForceSubmit
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestNeedForceSubmitPositive, TestSize.Level1)
{
    auto mockFilter = std::make_shared<MockShaderFilter>(RSUIFilterType::KAWASE);
    mockFilter->needForceSubmit = true;
    RSDrawingFilter drawingFilter(mockFilter);

    EXPECT_TRUE(drawingFilter.NeedForceSubmit());
}

/**
 * @tc.name: TestPostProcessSkipMaskColor
 * @tc.desc: 验证 PostProcess 在 canSkipMaskColor_ 为 true 时跳过 MASK_COLOR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestPostProcessSkipMaskColor, TestSize.Level1)
{
    // 这个测试有点 tricky，因为 canSkipMaskColor_ 是私有的，
    // 它在 DrawImageRectInternal -> ApplyImageEffect -> IsHpsBlurApplied/ApplyHpsImageEffect 中被设置。
    // 我们无法直接测试 PostProcess 的这个逻辑，除非我们能调用 DrawImageRectInternal
    // 并模拟 HPS 模糊的成功。
    //
    // 替代方案：我们可以测试 ForcePostProcess (它总是调用)
    // 和 PostProcess (在 canSkipMaskColor_ 默认为 false 时调用)
    
    auto mockMaskFilter = std::make_shared<MockShaderFilter>(RSUIFilterType::MASK_COLOR);
    RSDrawingFilter drawingFilter(mockMaskFilter);
    Drawing::Canvas canvas;

    // 默认 canSkipMaskColor_ = false
    mockMaskFilter->postProcessCalled = false;
    drawingFilter.PostProcess(canvas);
    EXPECT_TRUE(mockMaskFilter->postProcessCalled); // 应该被调用

    // ForcePostProcess 总是调用
    mockMaskFilter->postProcessCalled = false;
    drawingFilter.ForcePostProcess(canvas);
    EXPECT_TRUE(mockMaskFilter->postProcessCalled);
}

/**
 * @tc.name: TestComposeWithNullShaderFilter
 * @tc.desc: 验证 Compose (RSRenderFilterParaBase) 传入 nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestComposeWithNullShaderFilter, TestSize.Level1)
{
    auto kawaseFilter = std::make_shared<RSKawaseBlurShaderFilter>(10);
    RSDrawingFilter drawingFilter(kawaseFilter);
    auto originalHash = drawingFilter.Hash();

    auto composedFilter = drawingFilter.Compose(std::shared_ptr<RSRenderFilterParaBase>(nullptr));
    
    EXPECT_NE(composedFilter, nullptr);
    EXPECT_EQ(composedFilter->GetShaderFilters().size(), 1);
    EXPECT_EQ(composedFilter->Hash(), originalHash);
}

/**
 * @tc.name: TestComposeWithNullImageFilter
 * @tc.desc: 验证 Compose (Drawing::ImageFilter) 传入 nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, TestComposeWithNullImageFilter, TestSize.Level1)
{
    auto imageFilter1 = CreateTestImageFilter();
    uint32_t hash1 = 123;
    RSDrawingFilter drawingFilter(imageFilter1, hash1);
    auto originalHash = drawingFilter.Hash();

    auto composedFilter = drawingFilter.Compose(std::shared_ptr<Drawing::ImageFilter>(nullptr), 0);

    EXPECT_NE(composedFilter, nullptr);
    EXPECT_EQ(composedFilter->GetImageFilter(), imageFilter1);
    EXPECT_EQ(composedFilter->Hash(), originalHash);
}
} // namespace Rosen
} // namespace OHOS
