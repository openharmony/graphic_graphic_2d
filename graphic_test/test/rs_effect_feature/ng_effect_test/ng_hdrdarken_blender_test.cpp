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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui_effect/effect/include/hdr_darken_blender.h"
#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
const RSHdrDarkenBlenderPara HDRDARKEN_DEFAULT_PARAMS = {
    1.0f,
    {0.299f, 0.587f, 0.114f}
};
const RSHdrDarkenBlenderPara HDRDARKEN_ONLYRATIO_PARAMS = {
    1.0f
};
const RSHdrDarkenBlenderPara HDRDARKEN_PARAMS_ONE = {
    1.0f,
    {0.2f, 0.3f, 0.4f}
};
const RSHdrDarkenBlenderPara HDRDARKEN_PARAMS_TWO = {
    2.0f,
    {0.2f, 0.3f, 0.4f}
};
const RSHdrDarkenBlenderPara HDRDARKEN_PARAMS_THREE = {
    0.5f,
    {0.2f, 0.3f, 0.4f}
};
const RSHdrDarkenBlenderPara HDRDARKEN_PARAMS_FOUR = {
    2.0f,
    {1.0f, 2.0f, 3.0f}
};
} // namespace

class NGHdrDarkenBlenderTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    RSCanvasNode::SharedPtr SetUpEffectNode()
    {
        auto effectNode = RSCanvasNode::Create();
        effectNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        effectNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        effectNode->SetBackgroundColor(0xFF333333);
        return effectNode;
    }
};

GRAPHIC_TEST(NGHdrDarkenBlenderTest, EFFECT_TEST, Set_NG_HdrDarken_Blender_Test001)
{
    int columnCount = 6;
    int rowCount = 5;
    int count = 4;
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT / rowCount;

    for (int i = 0; i < count; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetAlpha(0.3f);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.4f);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5f);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7f);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX, sizeY });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetHdrDarkenBlenderParams(HDRDARKEN_DEFAULT_PARAMS);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(NGHdrDarkenBlenderTest, EFFECT_TEST, Set_NG_HdrDarken_Blender_Test002)
{
    int columnCount = 6;
    int rowCount = 5;
    int count = 3;
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT / rowCount;

    for (int i = 0; i < count; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 20, sizeY - 20 });
        testNodeBackGround->SetAlpha(0.4f);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        childNode1->SetAlpha(0.4f);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::White()));
        childNode2->SetAlpha(0.1f);
        RegisterNode(childNode2);
        testNodeBackGround->SetBounds({ x, y, sizeX, sizeY });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);

        testNodeBackGround->SetHdrDarkenBlenderParams(HDRDARKEN_ONLYRATIO_PARAMS);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(NGHdrDarkenBlenderTest, EFFECT_TEST, Set_NG_HdrDarken_Blender_Test003)
{
    int columnCount = 5;
    int rowCount = 4;
    int count = 3;
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT / rowCount;

    for (int i = 0; i < count; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetAlpha(0.4f);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::White()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        childNode1->SetAlpha(0.2f);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        childNode2->SetAlpha(0.9f);
        RegisterNode(childNode2);
        testNodeBackGround->SetBounds({ x, y, sizeX, sizeY });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);

        testNodeBackGround->SetHdrDarkenBlenderParams(HDRDARKEN_PARAMS_ONE);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(NGHdrDarkenBlenderTest, EFFECT_TEST, Set_NG_HdrDarken_Blender_Test004)
{
    int columnCount = 5;
    int rowCount = 4;
    int count = 4;
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT / rowCount;

    for (int i = 0; i < count; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 20 });
        testNodeBackGround->SetAlpha(0.3f);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        childNode1->SetAlpha(0.3f);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::DarkGray()));
        childNode2->SetAlpha(0.3f);
        RegisterNode(childNode2);
        testNodeBackGround->SetBounds({ x, y, sizeX, sizeY });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);

        testNodeBackGround->SetHdrDarkenBlenderParams(HDRDARKEN_PARAMS_TWO);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(NGHdrDarkenBlenderTest, EFFECT_TEST, Set_NG_HdrDarken_Blender_Test005)
{
    int columnCount = 6;
    int rowCount = 5;
    int count = 2;
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT / rowCount;

    for (int i = 0; i < count; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetAlpha(0.7f);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Black()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Black()));
        childNode1->SetAlpha(0.7f);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Black()));
        childNode2->SetAlpha(0.7f);
        RegisterNode(childNode2);
        testNodeBackGround->SetBounds({ x, y, sizeX, sizeY });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);

        testNodeBackGround->SetHdrDarkenBlenderParams(HDRDARKEN_PARAMS_THREE);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(NGHdrDarkenBlenderTest, EFFECT_TEST, Set_NG_HdrDarken_Blender_Test006)
{
    int columnCount = 6;
    int rowCount = 4;
    int count = 2;
    auto sizeX = SCREEN_WIDTH / columnCount;
    auto sizeY = SCREEN_HEIGHT / rowCount;

    for (int i = 0; i < count; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 20, sizeY - 20 });
        testNodeBackGround->SetAlpha(0.5f);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        childNode1->SetAlpha(0.5f);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        childNode2->SetAlpha(0.5f);
        RegisterNode(childNode2);
        testNodeBackGround->SetBounds({ x, y, sizeX, sizeY });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);

        testNodeBackGround->SetHdrDarkenBlenderParams(HDRDARKEN_PARAMS_THREE);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

} // namespace OHOS::Rosen
