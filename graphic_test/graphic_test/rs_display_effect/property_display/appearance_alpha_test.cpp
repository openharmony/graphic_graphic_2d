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
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

// foreground background color, background img alpha
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Test_1)
{
    uint32_t colorList[] = { 0xffff0000, 0x80ff0000 };
    std::vector<float> alphaList = { -0.5, 0, 0.2, 1, 1.5 };

    const int columnCount = 2;
    const int nodeGap = 10;
    const int rowCount = alphaList.size();
    const int nodeWidth = screenWidth / rowCount - nodeGap;
    const int nodeHeight = 400;
    // set alpha, set foreground color(alpha or not)
    for (int i = 0; i < columnCount; i++) {
        for (int j = 0; j < rowCount; j++) {
            int x = (j % rowCount) * (nodeWidth + nodeGap);
            int y = i * (nodeHeight + nodeGap);
            auto testNodeFg = RSCanvasNode::Create();
            testNodeFg->SetAlpha(alphaList[j]);
            testNodeFg->SetBounds({ x, y, nodeWidth, nodeHeight });
            testNodeFg->SetForegroundColor(colorList[i]);
            GetRootNode()->AddChild(testNodeFg);
            RegisterNode(testNodeFg);
        }
    }

    // set alpha, set background color(alpha or not)
    for (int i = 0; i < columnCount; i++) {
        for (int j = 0; j < rowCount; j++) {
            int x = (j % rowCount) * (nodeWidth + nodeGap);
            int y = (i + columnCount) * (nodeHeight + nodeGap);
            auto testNodeBg = RSCanvasNode::Create();
            testNodeBg->SetAlpha(alphaList[j]);
            testNodeBg->SetBounds({ x, y, nodeWidth, nodeHeight });
            testNodeBg->SetBackgroundColor(colorList[i]);
            GetRootNode()->AddChild(testNodeBg);
            RegisterNode(testNodeBg);
        }
    }

    // set background image alpha
    const std::string imgPath = "/data/local/tmp/appearance_test.jpg";
    for (int i = 0; i < rowCount; i++) {
        int x = (i % rowCount) * (nodeWidth + nodeGap);
        int y = (columnCount + columnCount) * (nodeHeight + nodeGap);
        auto testNodeBgImg = SetUpNodeBgImage(imgPath, { x, y, nodeWidth, nodeHeight });
        testNodeBgImg->SetAlpha(alphaList[i]);
        GetRootNode()->AddChild(testNodeBgImg);
        RegisterNode(testNodeBgImg);
    }
}

// content img, text alpha
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Test_2)
{
    std::vector<float> alphaList = { -0.5, 0, 0.2, 1, 1.5 };

    const int columnCount = alphaList.size();
    const int nodeGap = 10;
    const int nodeWidth = screenWidth / columnCount - nodeGap;
    const int nodeHeight = 400;

    const std::string imgPath = "/data/local/tmp/appearance_test.jpg";
    // set content image alpha
    for (int i = 0; i < columnCount; i++) {
        int x = (i % columnCount) * (nodeWidth + nodeGap);
        int y = 0;
        auto testNodeImg = RSCanvasNode::Create();
        testNodeImg->SetBounds({ x, y, nodeWidth, nodeHeight });
        testNodeImg->SetAlpha(alphaList[i]);
        auto imageModifier = std::make_shared<ImageCustomModifier>();
        imageModifier->SetWidth(nodeWidth);
        imageModifier->SetHeight(nodeHeight);
        imageModifier->SetPixelMapPath(imgPath);
        testNodeImg->AddModifier(imageModifier);
        GetRootNode()->AddChild(testNodeImg);
        RegisterNode(testNodeImg);
    }

    // set content text alpha
    const int translateY = 100;
    const int fontSize = 500;
    const std::string testText = "TEST TEXT ALPHA";
    for (int i = 0; i < columnCount; i++) {
        int x = (i % columnCount) * (nodeWidth + nodeGap);
        int y = nodeHeight + nodeGap;
        auto testNodeImg = RSCanvasNode::Create();
        testNodeImg->SetBounds({ x, y, nodeWidth, nodeHeight });
        testNodeImg->SetTranslate(0, translateY, 0);
        testNodeImg->SetAlpha(alphaList[i]);
        auto textModifier = std::make_shared<TextCustomModifier>();
        textModifier->SetFontSize(fontSize);
        textModifier->SetText(testText);
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

    const int columnCount = 2;
    const int rowCount = 3;
    const int nodeSize = 400;
    const int nodeGap = 10;
    const int nodeColumnPos = 460;
    // set alpha not offscreen, set background color
    for (int i = 0; i < columnCount; i++) {
        for (int j = 0; j < rowCount; j++) {
            int x = (j % rowCount) * (nodeSize + nodeGap);
            int y = i * nodeColumnPos;
            auto testNodeParent = RSCanvasNode::Create();
            testNodeParent->SetAlpha(alphaList[j]);
            testNodeParent->SetBounds({ x, y, nodeSize, nodeSize });
            testNodeParent->SetBackgroundColor(colorList[0]);
            testNodeParent->SetAlphaOffscreen(false);
            GetRootNode()->AddChild(testNodeParent);
            RegisterNode(testNodeParent);

            auto testNodeChild = RSCanvasNode::Create();
            testNodeChild->SetBounds({ 0, 0, nodeSize, nodeSize });
            testNodeChild->SetTranslate(0, 50, 0); // Set Y-Position to 50
            testNodeChild->SetAlpha(0.2); // Set alpha to 0.2
            testNodeChild->SetBackgroundColor(colorList[i]);
            testNodeParent->AddChild(testNodeChild);
            RegisterNode(testNodeChild);
        }
    }

    // set alpha offscreen, set background color
    for (int i = 0; i < columnCount; i++) {
        for (int j = 0; j < rowCount; j++) {
            int x = (j % rowCount) * (nodeSize + nodeGap);
            int y = (i + columnCount) * nodeColumnPos;
            auto testNodeParent = RSCanvasNode::Create();
            testNodeParent->SetAlpha(alphaList[j]);
            testNodeParent->SetBounds({ x, y, nodeSize, nodeSize });
            testNodeParent->SetBackgroundColor(colorList[0]);
            testNodeParent->SetAlphaOffscreen(true);
            GetRootNode()->AddChild(testNodeParent);
            RegisterNode(testNodeParent);

            auto testNodeChild = RSCanvasNode::Create();
            testNodeChild->SetBounds({ 0, 0, nodeSize, nodeSize });
            testNodeChild->SetTranslate(0, 50, 0); // Set Y-Position to 50
            testNodeChild->SetAlpha(0.2); // Set alpha = 0.2
            testNodeChild->SetBackgroundColor(colorList[i]);
            testNodeParent->AddChild(testNodeChild);
            RegisterNode(testNodeChild);
        }
    }
}

} // namespace OHOS::Rosen