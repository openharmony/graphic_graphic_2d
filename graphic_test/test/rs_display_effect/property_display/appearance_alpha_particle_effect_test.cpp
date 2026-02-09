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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AppearanceTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* ParticleEffect + Alpha: alpha = 0.5 */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_ParticleEffect_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetAlpha(0.5f);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* ParticleEffect + Alpha: different alpha values */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_ParticleEffect_Test_2)
{
    std::vector<float> alphaList = { 0.2f, 0.4f, 0.6f, 0.8f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { (int)i * 260 + 50, 100, 250, 250 });
        testNode->SetAlpha(alphaList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* ParticleEffect + Alpha: with border */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_ParticleEffect_Test_3)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetAlpha(0.7f);
    testNode->SetBorderWidth(20);
    testNode->SetBorderColor(0xff00ff00);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* ParticleEffect + Alpha: change alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_ParticleEffect_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 500, 500 });
    testNode->SetAlpha(0.9f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    testNode->SetAlpha(0.4f);
}

/* ParticleEffect + Alpha: boundary values */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_ParticleEffect_Boundary)
{
    std::vector<float> boundaryAlphas = { 0.0f, 0.001f, 0.999f, 1.0f };

    for (size_t i = 0; i < boundaryAlphas.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { (int)i * 280 + 50, 100, 250, 250 });
        testNode->SetAlpha(boundaryAlphas[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* ParticleEffect + Alpha: abnormal values */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_ParticleEffect_Abnormal)
{
    std::vector<float> abnormalAlphas = { -1.5f, -0.5f, 1.5f, 2.0f };

    for (size_t i = 0; i < abnormalAlphas.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 100, 250, 250 });
        testNode->SetAlpha(abnormalAlphas[i]);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
