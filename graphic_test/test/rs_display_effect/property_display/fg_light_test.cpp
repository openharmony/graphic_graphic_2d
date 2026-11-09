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
#include "ui_effect/property/include/rs_ui_shader_base.h"

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

std::shared_ptr<Rosen::RSCanvasNode> SetUpNodeColor(uint32_t colorValue, const Rosen::Vector4f bounds)
{
    auto testNode1 = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    testNode1->SetBounds(bounds);
    testNode1->SetBackgroundColor(colorValue);
    return testNode1;
}

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
            testNode->SetLightPosition(200, 200, 200);
        }
        testNode->SetIlluminatedType(1);
        testNode->SetIlluminatedBorderWidth(5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, LightIntensity_Test_SetCoverageNGShader)
{
    float intensityList[] = { 0, 1, 10, 100 };
    for (int i = 0; i < 12; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        auto pointLightShader = std::make_shared<RSNGPointLight>();
        if (i % 3 == 1) {
            pointLightShader->Setter<PointLightLightColorTag>(Vector4f(255.0f, 255.0f, 255.0f, 255.0f));
            pointLightShader->Setter<PointLightLightIntensityTag>(intensityList[i / 3]);
            pointLightShader->Setter<PointLightLightPositionTag>(Vector4f(200, 200, 300, 0.0f));
        }
        pointLightShader->Setter<PointLightIlluminatedBorderWidthTag>(100.0f);
        pointLightShader->Setter<PointLightIlluminatedTypeTag>(intensityList[i / 3]);
        testNode->SetCoverageNGShader(pointLightShader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, LightIntensity_Test_SetBackgroundNGShader)
{
    float intensityList[] = { 0, 1, 10, 100 };
    for (int i = 0; i < 12; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeColor(0xffffffff, { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        auto pointLightShader = std::make_shared<RSNGPointLight>();
        if (i % 3 == 1) {
            pointLightShader->Setter<PointLightLightColorTag>(Vector4f(255.0f, 0.0f, 255.0f, 255.0f));
            pointLightShader->Setter<PointLightLightIntensityTag>(intensityList[i / 3]);
            pointLightShader->Setter<PointLightLightPositionTag>(Vector4f(200, 200, 300, 0.0f));
        }
        pointLightShader->Setter<PointLightIlluminatedBorderWidthTag>(100.0f);
        pointLightShader->Setter<PointLightIlluminatedTypeTag>(intensityList[i / 3]);
        testNode->SetBackgroundNGShader(pointLightShader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, LightIntensity_Test_SetOverlayNGShader)
{
    float intensityList[] = { 0, 1, 10, 100 };
    for (int i = 0; i < 12; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        auto pointLightShader = std::make_shared<RSNGPointLight>();
        if (i % 3 == 1) {
            pointLightShader->Setter<PointLightLightColorTag>(Vector4f(255.0f, 0.0f, 0.0f, 255.0f));
            pointLightShader->Setter<PointLightLightIntensityTag>(intensityList[i / 3]);
            pointLightShader->Setter<PointLightLightPositionTag>(Vector4f(200, 200, 300, 0.0f));
        }
        pointLightShader->Setter<PointLightIlluminatedBorderWidthTag>(100.0f);
        pointLightShader->Setter<PointLightIlluminatedTypeTag>(intensityList[i / 3]);
        testNode->SetOverlayNGShader(pointLightShader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, LightIntensity_Test_SetForegroundShader)
{
    float intensityList[] = { 0, 1, 10, 100 };
    for (int i = 0; i < 12; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        auto pointLightShader = std::make_shared<RSNGPointLight>();
        if (i % 3 == 1) {
            pointLightShader->Setter<PointLightLightColorTag>(Vector4f(0.0f, 255.0f, 0.0f, 255.0f));
            pointLightShader->Setter<PointLightLightIntensityTag>(intensityList[i / 3]);
            pointLightShader->Setter<PointLightLightPositionTag>(Vector4f(200, 200, 300, 0.0f));
        }
        pointLightShader->Setter<PointLightIlluminatedBorderWidthTag>(100.0f);
        pointLightShader->Setter<PointLightIlluminatedTypeTag>(intensityList[i / 3]);
        testNode->SetForegroundShader(pointLightShader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest02, CONTENT_DISPLAY_TEST, LightIntensity_Test_SetMaterialShader)
{
    float intensityList[] = { 0, 1, 10, 100 };
    for (int i = 0; i < 12; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        auto pointLightShader = std::make_shared<RSNGPointLight>();
        if (i % 3 == 1) {
            pointLightShader->Setter<PointLightLightColorTag>(Vector4f(0.0f, 0.0f, 255.0f, 255.0f));
            pointLightShader->Setter<PointLightLightIntensityTag>(intensityList[i / 3]);
            pointLightShader->Setter<PointLightLightPositionTag>(Vector4f(200, 200, 300, 0.0f));
        }
        pointLightShader->Setter<PointLightIlluminatedBorderWidthTag>(100.0f);
        pointLightShader->Setter<PointLightIlluminatedTypeTag>(intensityList[i / 3]);
        testNode->SetMaterialShader(pointLightShader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen