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

#include "parameters_defination.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#include "render/rs_blur_filter.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ForegroundTest : public RSGraphicTest {
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

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, Blur_ForegroundEffectRadius_Test_1)
{
    float radiusList[] = { 0, 10.0, 100.0 };
    for (int i = 0; i < THREE_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetForegroundEffectRadius(radiusList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, Blur_Saturation_Test_1)
{
    float radiusList[] = { 10.0, 0.0, 10.0, 10.0 };
    float degreeList[] = { 0.0, 0.5, 0.5, 5.0 };
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetForegroundBlurRadius(radiusList[i]);
        testNode->SetForegroundBlurSaturation(degreeList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, Blur_Brightness_Test_1)
{
    float radiusList[] = { 10.0, 0.0, 10.0, 10.0 };
    float degreeList[] = { 0.0, 0.5, 0.5, 1.5 };
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetForegroundBlurRadius(radiusList[i]);
        testNode->SetForegroundBlurBrightness(degreeList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, Blur_Color_Test_1)
{
    float radiusList[] = { 10.0, 0.0, 10.0 };
    uint32_t colorList[] = { 0x00ffffff, 0x0000ff00, 0x7d00ff00 };
    for (int i = 0; i < THREE_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetForegroundBlurRadius(radiusList[i]);
        testNode->SetForegroundBlurMaskColor(RSColor::FromArgbInt(colorList[i]));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, Blur_ColorMode_Test_1)
{
    for (int i = 0; i < THREE_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetForegroundBlurRadius(TEN_);
        testNode->SetForegroundBlurMaskColor(RSColor::FromArgbInt(0x7dff0000));
        testNode->SetForegroundBlurColorMode(i);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, Blur_RadiusXY_Test_1)
{
    float radiusX[] = { 0.0, 1.1, 10.0, 10.0 };
    float radiusY[] = { 0.0, 10.0, 1.1, 10.0 };
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * SIX_HUNDRED_TEN_;
        int y = (i / TWO_) * SIX_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, SIX_HUNDRED_, SIX_HUNDRED_ });
        testNode->SetForegroundBlurRadiusX(radiusX[i]);
        testNode->SetForegroundBlurRadiusY(radiusY[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, Blur_LinearGradientBlur_Test_1)
{
    GradientDirection direction[] = {
        GradientDirection::RIGHT,
        GradientDirection::RIGHT,
        GradientDirection::BOTTOM,
        GradientDirection::RIGHT,
        GradientDirection::RIGHT,
        GradientDirection::RIGHT,
    };
    std::vector<std::pair<float, float>> fractionStops[] = {
        { std::make_pair(0.0, 0.0), std::make_pair(0.0, 1.0) }, { std::make_pair(1.0, 0.0), std::make_pair(1.0, 1.0) },
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0) },
        { std::make_pair(0.1, 0.1), std::make_pair(1.0, 0.5), std::make_pair(0.5, 0.7) },
        { std::make_pair(0.0, 0.0), std::make_pair(1.0, 0.5), std::make_pair(0.0, 1.0) },
        { std::make_pair(0.0, 0.1), std::make_pair(0.5, 0.7), std::make_pair(0.9, 0.3) }, // 对应模糊位置不严格递增
    };
    for (int i = 0; i < SIX_; i++) {
        int x = (i % TWO_) * SIX_HUNDRED_TEN_;
        int y = (i / TWO_) * SIX_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, SIX_HUNDRED_, SIX_HUNDRED_ });
        testNode->SetFrame({ x, y, SIX_HUNDRED_, SIX_HUNDRED_ });
        std::shared_ptr<RSLinearGradientBlurPara> param(
            std::make_shared<Rosen::RSLinearGradientBlurPara>(ONE_HUNDRED_, fractionStops[i], direction[i]));
        testNode->SetLinearGradientBlurPara(param);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, Foreground_Filter_Test)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    // Three nodes, 2 columns * 2 rows
    // First image, index = 0
    int x = (0 % columnCount) * sizeX;
    int y = (0 / columnCount) * sizeY;
    auto testNodeForeground =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeForeground->SetFilter(nullptr);
    GetRootNode()->AddChild(testNodeForeground);
    RegisterNode(testNodeForeground);

    // Second image, index = 1
    x = (1 % columnCount) * sizeX;
    y = (1 / columnCount) * sizeY;
    auto testNodeForeground2 =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    MaterialParam materialParam = { 50, 0.8, 0.8, Color(0x2dff0000), false };
    std::shared_ptr<RSMaterialFilter> matFilter =
        std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::PRE_DEFINED);
    testNodeForeground2->SetFilter(matFilter);
    GetRootNode()->AddChild(testNodeForeground2);
    RegisterNode(testNodeForeground2);

    // Third image, index = 2
    x = (2 % columnCount) * sizeX;
    y = (2 / columnCount) * sizeY;
    auto testNodeForeground3 =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    std::shared_ptr<RSBlurFilter> blurFilter = std::make_shared<RSBlurFilter>(30, 30, false);
    testNodeForeground3->SetFilter(blurFilter);
    GetRootNode()->AddChild(testNodeForeground3);
    RegisterNode(testNodeForeground3);

    // Forth image, index = 3, with all material filter value invalid
    x = (3 % columnCount) * sizeX;
    y = (3 / columnCount) * sizeY;
    auto testNodeForeground4 =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    MaterialParam materialParam2 = { -50, -0.5, -0.5, Color(0x9fff0000), false };
    std::shared_ptr<RSMaterialFilter> matFilter2 =
        std::make_shared<RSMaterialFilter>(materialParam2, BLUR_COLOR_MODE::PRE_DEFINED);
    testNodeForeground4->SetFilter(matFilter2);
    GetRootNode()->AddChild(testNodeForeground4);
    RegisterNode(testNodeForeground4);
}

} // namespace OHOS::Rosen