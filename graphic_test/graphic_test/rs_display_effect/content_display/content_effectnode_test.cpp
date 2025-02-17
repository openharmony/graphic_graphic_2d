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

#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ContentEffectNodeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
    }

    void setNode(std::shared_ptr<RSCanvasNode>& node, float alpha)
    {
        int radius = 30;
        int borderstyle = 0;
        Vector4f borderWidth = { 5, 5, 5, 5 };
        node->SetBackgroundBlurRadius(radius);
        node->SetBorderStyle(borderstyle);
        node->SetBorderWidth(borderWidth);
        node->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        node->SetAlpha(alpha);
    };
};

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test)
{
    int columnCount = 1;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = 0; i < 2; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        testFaNode->SetBounds({ 0, 0, sizeX - 10, sizeY - 10 });
        RegisterNode(testFaNode);
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        setNode(childNode2, 0.5);
        childNode2->SetUseEffect(i);
        RegisterNode(childNode2);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->AddChild(childNode1);
        testFaNode->AddChild(childNode2);
        testNodeBackGround->AddChild(testFaNode);
        setNode(testNodeBackGround, 1);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

} // namespace OHOS::Rosen