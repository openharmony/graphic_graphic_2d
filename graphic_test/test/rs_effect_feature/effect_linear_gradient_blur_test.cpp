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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#include "render/rs_blur_filter.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class LinearGradientBlurTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_6)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_7)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::TOP;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_8)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::TOP;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_9)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::TOP;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_10)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::TOP;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_11)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::TOP;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_12)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::TOP;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_13)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_14)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_15)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_16)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_17)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_18)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_19)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::BOTTOM;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_20)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::BOTTOM;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_21)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::BOTTOM;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_22)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::BOTTOM;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_23)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::BOTTOM;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_24)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::BOTTOM;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_25)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_TOP;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_26)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_TOP;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_27)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_TOP;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_28)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_TOP;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_29)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_TOP;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_30)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_TOP;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_31)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_32)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_33)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_34)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_35)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_36)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::LEFT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_37)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_TOP;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_38)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_TOP;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_39)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_TOP;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_40)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_TOP;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_41)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_TOP;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_42)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_TOP;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_43)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_44)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_45)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_46)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_47)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_48)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::RIGHT_BOTTOM;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_49)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::NONE;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_50)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::NONE;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_51)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::NONE;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_52)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::NONE;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_53)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::NONE;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_54)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::NONE;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_55)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::START_TO_END;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_56)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::START_TO_END;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_57)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::START_TO_END;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_58)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::START_TO_END;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_59)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::START_TO_END;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_60)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::START_TO_END;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_61)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::END_TO_START;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_62)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::END_TO_START;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_63)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::END_TO_START;
    std::vector<std::pair<float, float>> fractionStops = { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_64)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::END_TO_START;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_65)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::END_TO_START;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LinearGradientBlurTest, EFFECT_TEST, Set_Linear_Gradient_Blur_Para_Test_66)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float blurRadius = 100;
    GradientDirection direction = GradientDirection::END_TO_START;
    std::vector<std::pair<float, float>> fractionStops =
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    testNode->SetFrame({ x, y, sizeX, sizeY });
    std::shared_ptr<RSLinearGradientBlurPara> param(
        std::make_shared<Rosen::RSLinearGradientBlurPara>(blurRadius, fractionStops, direction));
    testNode->SetLinearGradientBlurPara(param);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

} // namespace OHOS::Rosen
