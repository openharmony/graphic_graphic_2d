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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ForegroundTest01 : public RSGraphicTest {
private:
    int screenWidth = 1260;
    int screenHeight = 2720;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
        SetSurfaceColor(RSColor(0xff000000));
    }
};

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, GrayScale_Test_1)
{
    float grayScaleList[] = { 0.0, 0.5, 1.0, 2.0 };
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 500, 500 });
        testNode->SetGrayScale(grayScaleList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Brightness_Test_1)
{
    float brightnessList[] = { 0.0, 0.5, 1.0, 2.0, 5.0 };
    for (int i = 0; i < 5; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 500, 500 });
        testNode->SetBrightness(brightnessList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Contrast_Test_1)
{
    float contrastList[] = { 0.0, 0.5, 1.0, 5.0, 20.0 };
    for (int i = 0; i < 5; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 500, 500 });
        testNode->SetContrast(contrastList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Saturate_Test_1)
{
    float saturateList[] = { 0.0, 0.5, 1.0, 10.0, 100.0 };
    for (int i = 0; i < 5; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 500, 500 });
        testNode->SetSaturate(saturateList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Sepia_Test_1)
{
    float sepiaList[] = { 0.0, 0.5, 1.0, 2.0 };
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 500, 500 });
        testNode->SetSepia(sepiaList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, Color_Invert_Test_1)
{
    float invertList[] = { 0.0, 0.5, 1.0, 2.0 };
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 500, 500 });
        testNode->SetInvert(invertList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, HueRotate_Test_1)
{
    float hueRotateList[] = { 0.0, 90.0, 180.0, 270.0, 360.0 };
    for (int i = 0; i < 5; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 500, 500 });
        testNode->SetHueRotate(hueRotateList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest01, CONTENT_DISPLAY_TEST, ColorBlend_Test_1)
{
    uint32_t colorList[] = { 0xffffffff, 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff, 0x00000000 };
    for (int i = 0; i < 6; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 500, 500 });
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
    for (int i = 0; i < 3; i++) {
        int y = i * 310;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 0.0, y, 1000.0, 310.0 });
        testNode->SetAiInvert(param_list[i]);
        testFaNode->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen