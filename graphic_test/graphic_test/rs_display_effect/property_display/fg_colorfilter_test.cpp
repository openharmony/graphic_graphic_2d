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

#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ForegroundTest01 : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        SetSurfaceColor(RSColor(0xff000000));
    }
};

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, GrayScale_Test_1)
{
    float grayScaleList[] = { 0.0, 0.5, 1.0, 2.0 };
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetGrayScale(grayScaleList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Brightness_Test_1)
{
    float brightnessList[] = { 0.0, 0.5, 1.0, 2.0, 5.0 };
    for (int i = 0; i < FIVE_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetBrightness(brightnessList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Contrast_Test_1)
{
    float contrastList[] = { 0.0, 0.5, 1.0, 5.0, 20.0 };
    for (int i = 0; i < FIVE_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetContrast(contrastList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Saturate_Test_1)
{
    float saturateList[] = { 0.0, 0.5, 1.0, 10.0, 100.0 };
    for (int i = 0; i < FIVE_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetSaturate(saturateList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Sepia_Test_1)
{
    float sepiaList[] = { 0.0, 0.5, 1.0, 2.0 };
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetSepia(sepiaList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Color_Invert_Test_1)
{
    float invertList[] = { 0.0, 0.5, 1.0, 2.0 };
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetInvert(invertList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, HueRotate_Test_1)
{
    float hueRotateList[] = { 0.0, 90.0, 180.0, 270.0, 360.0 };
    for (int i = 0; i < FIVE_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetHueRotate(hueRotateList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, ColorBlend_Test_1)
{
    uint32_t colorList[] = { 0xffffffff, 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff, 0x00000000 };
    for (int i = 0; i < SIX_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode->SetColorBlend(colorList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Color_AiInvert_Test_1)
{
    auto testFaNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, 1000, 1000 });
    GetRootNode()->AddChild(testFaNode);
    RegisterNode(testFaNode);
    Vector4f param_list[] = { { 0.0, 1.0, 0.5, 0.2 }, { 0.2, 0.5, 0.2, 0.2 }, { 0.0, 1.0, 0.5, 0.0 } };
    for (int i = 0; i < THREE_; i++) {
        int y = i * THREE_HUNDRED_TEN_;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 0.0, y, ONE_THOUSAND_, THREE_HUNDRED_TEN_ });
        testNode->SetAiInvert(param_list[i]);
        testFaNode->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Foreground_Filter_Test_1)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> radiusList = { 0.1, 0.5, 2.9, 100 };
    std::vector<float> saturateList = { 0.5, 0.6, 2.8, 100.0 };
    std::vector<float> brightnessList = { 0.1, 0.2, 2.9, 50.0 };
    std::vector<uint32_t> maskColorList = { 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff };

    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));

        MaterialParam materialParam;
        materialParam.brightness = brightnessList[i];
        materialParam.maskColor = Color::FromArgbInt(maskColorList[i]);
        materialParam.radius = radiusList[i];
        materialParam.saturation = saturateList[i];
        auto materialFilter = std::make_shared<RSMaterialFilter>(
            materialParam, BLUR_COLOR_MODE::AVERAGE); // BLUR_COLOR_MODE::AVERAGE == 1
        testNode->SetFilter(materialFilter);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen