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
#include "rs_graphic_test_text.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AppearanceTest : public RSGraphicTest {
private:
    int screenWidth = 1260;
    int screenHeight = 2720;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
    }
};

// foreground background color, background img alpha
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Test_1)
{
    uint32_t colorList[] = { 0xffff0000, 0x80ff0000 };
    float alphaList[] = { 0, 0.2, 1 };

    // set alpha, set foreground color(alpha or not)
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            int x = (j % 3) * 410;
            int y = i * 410;
            auto testNodeFg = RSCanvasNode::Create();
            testNodeFg->SetAlpha(alphaList[j]);
            testNodeFg->SetBounds({ x, y, 400, 400 });
            testNodeFg->SetForegroundColor(colorList[i]);
            GetRootNode()->AddChild(testNodeFg);
            RegisterNode(testNodeFg);
        }
    }

    // set alpha, set background color(alpha or not)
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            int x = (j % 3) * 410;
            int y = (i + 2) * 410;
            auto testNodeBg = RSCanvasNode::Create();
            testNodeBg->SetAlpha(alphaList[j]);
            testNodeBg->SetBounds({ x, y, 400, 400 });
            testNodeBg->SetBackgroundColor(colorList[i]);
            GetRootNode()->AddChild(testNodeBg);
            RegisterNode(testNodeBg);
        }
    }

    // set background image alpha
    for (int j = 0; j < 3; j++) {
        int x = (j % 3) * 410;
        int y = 4 * 410;
        auto testNodeBgImg = SetUpNodeBgImage("/data/local/tmp/appearance_test.jpg", { x, y, 400, 400 });
        testNodeBgImg->SetAlpha(alphaList[j]);
        GetRootNode()->AddChild(testNodeBgImg);
        RegisterNode(testNodeBgImg);
    }
}

// content img, text alpha
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Test_2)
{
    float alphaList[] = { 0, 0.2, 1 };

    // set content image alpha
    for (int i = 0; i < 3; i++) {
        int x = (i % 3) * 410;
        int y = 0;
        auto testNodeImg = RSCanvasNode::Create();
        testNodeImg->SetBounds({ x, y, 400, 400 });
        testNodeImg->SetAlpha(alphaList[i]);
        auto imageModifier = std::make_shared<ImageCustomModifier>();
        imageModifier->SetWidth(400);
        imageModifier->SetHeight(400);
        imageModifier->SetPixelMapPath("/data/local/tmp/appearance_test.jpg");
        testNodeImg->AddModifier(imageModifier);
        GetRootNode()->AddChild(testNodeImg);
        RegisterNode(testNodeImg);
    }

    // set content text alpha
    for (int i = 0; i < 3; i++) {
        int x = (i % 3) * 410;
        int y = 410;
        auto testNodeImg = RSCanvasNode::Create();
        testNodeImg->SetBounds({ x, y, 400, 400 });
        testNodeImg->SetTranslate(0, 100, 0);
        testNodeImg->SetAlpha(alphaList[i]);
        auto textModifier = std::make_shared<TextCustomModifier>();
        textModifier->SetFontSize(500);
        textModifier->SetText("TEST TEXT ALPHA");
        testNodeImg->AddModifier(textModifier);
        GetRootNode()->AddChild(testNodeImg);
        RegisterNode(testNodeImg);
    }
}

// offscreen
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Offscreen_Test_1)
{
    uint32_t colorList[] = { 0xff0000ff, 0xffff0000 };
    float alphaList[] = { 0, 0.8, 1 };

    // set alpha not offscreen, set background color
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            int x = (j % 3) * 410;
            int y = i * 460;
            auto testNodeParent = RSCanvasNode::Create();
            testNodeParent->SetAlpha(alphaList[j]);
            testNodeParent->SetBounds({ x, y, 400, 400 });
            testNodeParent->SetBackgroundColor(colorList[0]);
            testNodeParent->SetAlphaOffscreen(false);
            GetRootNode()->AddChild(testNodeParent);
            RegisterNode(testNodeParent);

            auto testNodeChild = RSCanvasNode::Create();
            testNodeChild->SetBounds({ 0, 0, 400, 400 });
            testNodeChild->SetTranslate(0, 50, 0);
            testNodeChild->SetAlpha(0.2);
            testNodeChild->SetBackgroundColor(colorList[i]);
            testNodeParent->AddChild(testNodeChild);
            RegisterNode(testNodeChild);
        }
    }

    // set alpha offscreen, set background color
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            int x = (j % 3) * 410;
            int y = (i + 2) * 460;
            auto testNodeParent = RSCanvasNode::Create();
            testNodeParent->SetAlpha(alphaList[j]);
            testNodeParent->SetBounds({ x, y, 400, 400 });
            testNodeParent->SetBackgroundColor(colorList[0]);
            testNodeParent->SetAlphaOffscreen(true);
            GetRootNode()->AddChild(testNodeParent);
            RegisterNode(testNodeParent);

            auto testNodeChild = RSCanvasNode::Create();
            testNodeChild->SetBounds({ 0, 0, 400, 400 });
            testNodeChild->SetTranslate(0, 50, 0);
            testNodeChild->SetAlpha(0.2);
            testNodeChild->SetBackgroundColor(colorList[i]);
            testNodeParent->AddChild(testNodeChild);
            RegisterNode(testNodeChild);
        }
    }
}

} // namespace OHOS::Rosen