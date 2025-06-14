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

class MagnifierEffectTest : public RSGraphicTest {
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

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    std::string testImagePath = "/data/local/tmp/Images/3200x2000.jpg";
    auto testNodeBackGround = SetUpNodeBgImage(testImagePath, { x, y, sizeX, sizeY });
    auto magnifierParams = std::make_shared<Rosen::RSMagnifierParams>();
    magnifierParams->factor_ = 2;
    magnifierParams->width_ = 100;
    magnifierParams->height_ = 100;
    magnifierParams->cornerRadius_ = 10;
    magnifierParams->borderWidth_ = 2;
    magnifierParams->offsetX_ = 10;
    magnifierParams->offsetY_ = 10;
    testNodeBackGround->SetMagnifierParams(magnifierParams);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    std::string testImagePath = "/data/local/tmp/Images/3200x2000.jpg";
    auto testNodeBackGround = SetUpNodeBgImage(testImagePath, { x, y, sizeX, sizeY });
    auto magnifierParams = std::make_shared<Rosen::RSMagnifierParams>();
    magnifierParams->factor_ = 3;
    magnifierParams->width_ = 150;
    magnifierParams->height_ = 150;
    magnifierParams->cornerRadius_ = 15;
    magnifierParams->borderWidth_ = 3;
    magnifierParams->offsetX_ = 15;
    magnifierParams->offsetY_ = 15;
    testNodeBackGround->SetMagnifierParams(magnifierParams);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    std::string testImagePath = "/data/local/tmp/Images/3200x2000.jpg";
    auto testNodeBackGround = SetUpNodeBgImage(testImagePath, { x, y, sizeX, sizeY });
    auto magnifierParams = std::make_shared<Rosen::RSMagnifierParams>();
    magnifierParams->factor_ = 5;
    magnifierParams->width_ = 500;
    magnifierParams->height_ = 500;
    magnifierParams->cornerRadius_ = 50;
    magnifierParams->borderWidth_ = 10;
    magnifierParams->offsetX_ = 50;
    magnifierParams->offsetY_ = 50;
    testNodeBackGround->SetMagnifierParams(magnifierParams);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    std::string testImagePath = "/data/local/tmp/Images/3200x2000.jpg";
    auto testNodeBackGround = SetUpNodeBgImage(testImagePath, { x, y, sizeX, sizeY });
    auto magnifierParams = std::make_shared<Rosen::RSMagnifierParams>();
    magnifierParams->factor_ = 10;
    magnifierParams->width_ = 1000;
    magnifierParams->height_ = 1000;
    magnifierParams->cornerRadius_ = 100;
    magnifierParams->borderWidth_ = 10;
    magnifierParams->offsetX_ = 100;
    magnifierParams->offsetY_ = 100;
    testNodeBackGround->SetMagnifierParams(magnifierParams);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

} // namespace OHOS::Rosen
