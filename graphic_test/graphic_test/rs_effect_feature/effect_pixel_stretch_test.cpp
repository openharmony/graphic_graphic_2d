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

#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class PixelStretchTest : public RSGraphicTest {
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

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test1)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 0, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test2)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 20, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test3)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 0, 40, 0 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test4)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 0, 0, 60 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test5)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 30, 30, 30, 30 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test6)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 0, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test7)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 20, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test8)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 0, 40, 0 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test9)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 0, 0, 60 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test10)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 30, 30, 30, 30 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test11)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 0, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test12)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 20, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test13)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 0, 40, 0 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test14)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 0, 0, 0, 60 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test15)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 30, 30, 30, 30 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Test16)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretch = { 30, 30, 30, 30 };
    Drawing::TileMode mode = Drawing::TileMode::DECAL;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretch(pixelStretch, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test1)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test2)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0.1, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test3)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0, 0.5, 0 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test4)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0, 0, 1.0 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test5)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0.1, 0.2, 0.3, 0.4 };
    Drawing::TileMode mode = Drawing::TileMode::CLAMP;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test6)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test7)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0.1, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test8)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0, 0.5, 0 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test9)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0, 0, 1.0 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test10)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0.1, 0.2, 0.3, 0.4 };
    Drawing::TileMode mode = Drawing::TileMode::REPEAT;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test11)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test12)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0.1, 0, 0 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test13)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0, 0.5, 0 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test14)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0, 0, 0, 1.0 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test15)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0.1, 0.2, 0.3, 0.4 };
    Drawing::TileMode mode = Drawing::TileMode::MIRROR;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(PixelStretchTest, EFFECT_TEST, Set_Pixel_Stretch_Percent_Test16)
{
    auto sizeX = screenWidth / 3;
    auto sizeY = screenHeight / 3;
    int x = screenWidth / 3;
    int y = screenHeight / 3;
    Vector4f pixelStretchPercent = { 0.1, 0.2, 0.3, 0.4 };
    Drawing::TileMode mode = Drawing::TileMode::DECAL;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetPixelStretchPercent(pixelStretchPercent, mode);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

} // namespace OHOS::Rosen
