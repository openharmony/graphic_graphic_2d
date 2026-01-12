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

#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ForegroundTest02 : public RSGraphicTest {
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

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, LightIntensity_Test_1)
{
    float intensityList[] = { 0, 1, 10, 100 };
    for (int i = 0; i < 12; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(intensityList[i / 3]);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetLightPosition(200, 200, 0);
            testNode->SetIlluminatedBorderWidth(5);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, LightColor_Test_1)
{
    uint32_t colorList[] = { 0xffffffff, 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff, 0x7dffffff };
    for (int i = 0; i < 18; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(colorList[i / 3]);
            testNode->SetIlluminatedType(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetLightPosition(200, 200, 0);
            testNode->SetIlluminatedBorderWidth(5);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, IlluminatedType_Test_1)
{
    uint32_t typeList[] = { -1, 0, 1, 2, 3, 4, 5 };
    for (int i = 0; i < 21; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 380;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 30, y + 10, 360, 360 });
        testNode->SetClipRRect({ 0, 0, 360, 360 }, { 180, 180, 180, 180 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(typeList[i / 3]);
            testNode->SetLightColor(0xffffffff);
            testNode->SetLightPosition(200, 200, 0);
            testNode->SetIlluminatedBorderWidth(5);
            if (i / 3 >= 5)
                testNode->SetBloom(1);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, LightPosition_Test_1)
{
    float positionList[] = { 0, 200, 400 };
    float zList[] = { 0, 0.5, 1 };
    for (int i = 0; i < 9; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetLightPosition(positionList[i / 3], positionList[i / 3], zList[i / 3]);
            testNode->SetIlluminatedBorderWidth(5);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, IlluminatedBorderWidth_Test_1)
{
    float widthList[] = { 0, 5, 10 };
    for (int i = 0; i < 9; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetLightPosition(200, 200, 0);
            testNode->SetIlluminatedBorderWidth(widthList[i / 3]);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, Fg_Light_Bloom_Test_1)
{
    float bloomList[] = { 0, 0.5, 1, 10 };
    for (int i = 0; i < 12; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(4);
            testNode->SetLightColor(0xffffffff);
            testNode->SetLightPosition(200, 200, 0);
            testNode->SetIlluminatedBorderWidth(5);
            testNode->SetBloom(bloomList[i / 3]);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen