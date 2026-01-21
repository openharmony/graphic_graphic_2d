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

class BackgroundTest01 : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void setNode(std::shared_ptr<RSCanvasNode>& node, float alpha)
    {
        int radius = 30;
        int borderstyle = 0;
        int borderWidth = 5;
        node->SetBackgroundBlurRadius(radius);
        node->SetBorderStyle(borderstyle);
        node->SetBorderWidth(borderWidth);
        node->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        node->SetAlpha(alpha);
    }
};

GRAPHIC_TEST(BackgroundTest01, CONTENT_DISPLAY_TEST, Use_Effect_Test)
{
    int columnCount = 1;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    const bool useEffectList[] = { false, true, false };

    for (int i = 0; i < 2; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto effectNode = RSEffectNode::Create();
        effectNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        effectNode->SetFrame({ x, y, sizeX - 10, sizeY - 10 });
        effectNode->SetBackgroundColor(0xffff0000);
        effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));
        GetRootNode()->AddChild(effectNode);
        RegisterNode(effectNode);

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetFrame({ sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetUseEffect(useEffectList[i]);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        RegisterNode(childNode1);
        effectNode->AddChild(childNode1);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/appearance_test.jpg", { sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetFrame({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetUseEffect(useEffectList[i + 1]);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        RegisterNode(childNode2);
        effectNode->AddChild(childNode2);
    }
}

} // namespace OHOS::Rosen