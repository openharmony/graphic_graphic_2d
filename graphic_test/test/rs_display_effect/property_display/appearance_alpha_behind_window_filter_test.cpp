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

/* BehindWindowFilter + Alpha: alpha = 0.5 */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_BehindWindowFilter_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetAlpha(0.5f);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* BehindWindowFilter + Alpha: different alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_BehindWindowFilter_Test_2)
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

/* BehindWindowFilter + Alpha: with border */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_BehindWindowFilter_Test_3)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetAlpha(0.7f);
    testNode->SetBorderWidth(25);
    testNode->SetBorderColor(0xff00ff00);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* BehindWindowFilter + Alpha: update alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_BehindWindowFilter_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 500, 500 });
    testNode->SetAlpha(0.8f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    testNode->SetAlpha(0.4f);
}

/* BehindWindowFilter + Alpha: boundary and abnormal values */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_BehindWindowFilter_Extremes)
{
    std::vector<float> extremeAlphas = { 0.0f, 0.5f, 1.0f, -0.5f, 1.5f };

    for (size_t i = 0; i < extremeAlphas.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { (int)i * 230 + 50, 100, 200, 200 });
        testNode->SetAlpha(extremeAlphas[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* BehindWindowFilter + Alpha: matrix test - alpha x border (3x3) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_BehindWindowFilter_Matrix_3x3)
{
    std::vector<float> alphaList = { 0.3f, 0.6f, 0.9f };
    std::vector<int> borderWidths = { 0, 20, 40 };

    for (size_t row = 0; row < alphaList.size(); row++) {
        for (size_t col = 0; col < borderWidths.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 280 + 50, 330, 230 });
            testNode->SetAlpha(alphaList[row]);
            testNode->SetBorderWidth(borderWidths[col]);
            testNode->SetBorderColor(0xff00ff00);
            testNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

} // namespace OHOS::Rosen
