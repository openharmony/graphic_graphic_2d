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

#include "gtest/gtest.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_render_aibar_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSAIBarShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAIBarShaderFilterTest::SetUpTestCase() {}
void RSAIBarShaderFilterTest::TearDownTestCase() {}
void RSAIBarShaderFilterTest::SetUp() {}
void RSAIBarShaderFilterTest::TearDown() {}

/**
 * @tc.name: DrawImageRectTest
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAIBarShaderFilterTest, DrawImageRectTest, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    EXPECT_FALSE(rsAIBarShaderFilter->GetAiInvertCoef().empty());
}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSAIBarShaderFilterTest, GenerateGEVisualEffectTest, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsAIBarShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: IsAiInvertCoefValidTest001
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest001, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_TRUE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest002
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest002, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 1.0, 0.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest003
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest003, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { -1.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest004
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest004, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 2.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest005
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest005, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, -1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest006
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest006, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 2.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest007
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest007, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, -0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest008
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest008, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 1.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest009
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest009, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, -0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest010
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest010, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 1.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest011
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest011, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, -1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest012
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest012, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 2.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: IsAiInvertCoefValidTest013
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTest013, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, -45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: DrawImageRectTest326
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAIBarShaderFilterTest, DrawImageRectTest326, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    ASSERT_NE(rsAIBarShaderFilter, nullptr);
    EXPECT_FALSE(rsAIBarShaderFilter->GetAiInvertCoef().empty());
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: DrawImageRectTestNONE326
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAIBarShaderFilterTest, DrawImageRectTestNONE326, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    ASSERT_NE(rsAIBarShaderFilter, nullptr);
    EXPECT_FALSE(rsAIBarShaderFilter->GetAiInvertCoef().empty());
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: DrawImageRectTestBRUSH326
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAIBarShaderFilterTest, DrawImageRectTestBRUSH326, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    ASSERT_NE(rsAIBarShaderFilter, nullptr);
    EXPECT_FALSE(rsAIBarShaderFilter->GetAiInvertCoef().empty());
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: DrawImageRectTestPEN326
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAIBarShaderFilterTest, DrawImageRectTestPEN326, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    ASSERT_NE(rsAIBarShaderFilter, nullptr);
    EXPECT_FALSE(rsAIBarShaderFilter->GetAiInvertCoef().empty());
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: DrawImageRectTestPAINT326
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAIBarShaderFilterTest, DrawImageRectTestPAINT326, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    ASSERT_NE(rsAIBarShaderFilter, nullptr);
    EXPECT_FALSE(rsAIBarShaderFilter->GetAiInvertCoef().empty());
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: DrawImageRectTestBRUSH_PEN326
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAIBarShaderFilterTest, DrawImageRectTestBRUSH_PEN326, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    ASSERT_NE(rsAIBarShaderFilter, nullptr);
    EXPECT_FALSE(rsAIBarShaderFilter->GetAiInvertCoef().empty());
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: GenerateGEVisualEffectTest327
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSAIBarShaderFilterTest, GenerateGEVisualEffectTest327, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    ASSERT_NE(rsAIBarShaderFilter, nullptr);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(visualEffectContainer, nullptr);
    rsAIBarShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE001
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE001, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_TRUE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE002
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE002, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 1.0, 0.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE003
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE003, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { -1.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE004
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE004, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 2.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE005
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE005, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, -1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE006
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE006, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 2.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE007
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE007, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, -0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE008
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE008, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 1.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE009
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE009, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, -0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE010
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE010, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 1.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE011
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE011, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, -1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE012
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE012, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 2.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestNONE013
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestNONE013, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, -45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::NONE;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH001
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH001, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_TRUE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH002
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH002, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 1.0, 0.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH003
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH003, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { -1.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH004
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH004, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 2.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH005
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH005, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, -1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH006
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH006, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 2.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH007
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH007, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, -0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH008
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH008, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 1.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH009
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH009, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, -0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH010
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH010, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 1.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH011
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH011, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, -1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH012
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH012, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 2.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH013
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH013, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, -45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN001
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN001, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_TRUE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN002
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN002, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 1.0, 0.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN003
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN003, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { -1.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN004
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN004, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 2.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN005
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN005, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, -1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN006
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN006, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 2.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN007
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN007, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, -0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN008
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN008, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 1.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN009
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN009, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, -0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN010
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN010, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 1.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN011
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN011, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, -1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN012
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN012, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 2.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPEN013
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPEN013, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, -45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT001
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT001, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_TRUE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT002
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT002, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 1.0, 0.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT003
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT003, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { -1.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT004
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT004, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 2.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT005
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT005, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, -1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT006
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT006, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 2.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT007
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT007, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, -0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT008
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT008, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 1.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT009
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT009, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, -0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT010
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT010, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 1.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT011
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT011, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, -1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT012
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT012, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 2.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestPAINT013
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestPAINT013, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, -45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::PAINT;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN001
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN001, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_TRUE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN002
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN002, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 1.0, 0.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN003
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN003, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { -1.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN004
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN004, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 2.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN005
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN005, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, -1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN006
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN006, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 2.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN007
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN007, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, -0.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));

    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN008
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN008, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 1.55, 0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN009
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN009, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, -0.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN010
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN010, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 1.4, 1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN011
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN011, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, -1.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN012
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN012, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 2.6, 45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}

/**
 * @tc.name: IsAiInvertCoefValidTestBRUSH_PEN013
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarShaderFilterTest, IsAiInvertCoefValidTestBRUSH_PEN013, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, -45.0 };
    EXPECT_FALSE(rsAIBarShaderFilter->IsAiInvertCoefValid(aiInvertCoef));
    std::shared_ptr<Drawing::GEVisualEffectContainer> container = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(container, nullptr);

    std::string name = "sfdnhpouigae wnvu=tr[oq8'53]xd[]=-[]";
    Drawing::DrawingPaintType type = Drawing::DrawingPaintType::BRUSH_PEN;
    auto visualEffect = std::make_shared<Drawing::GEVisualEffect>(name, type);
    ASSERT_NE(visualEffect, nullptr);
    container->AddToChainedFilter(visualEffect);
    rsAIBarShaderFilter->GenerateGEVisualEffect(container);
}
} // namespace Rosen
} // namespace OHOS