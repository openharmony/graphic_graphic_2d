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

#include "parameters_defination.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AppearanceTest : public RSGraphicTest {
private:
    const int screenWidth = 1260;
    const int screenHeight = 2720;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
    }
};

// shadow color
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Color_Test_1)
{
    uint32_t colorList[] = { 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * SIX_HUNDRED_;
        int y = (i / TWO_) * SIX_HUNDRED_;
        auto testNodeColor = RSCanvasNode::Create();
        testNodeColor->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeColor->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        testNodeColor->SetShadowColor(colorList[i]);
        testNodeColor->SetShadowRadius(FIFTY_);
        GetRootNode()->AddChild(testNodeColor);
        RegisterNode(testNodeColor);
    }
}

// shadow offset
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Offset_Test_1)
{
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * SIX_HUNDRED_;
        int y = (i / TWO_) * SIX_HUNDRED_;
        auto testNodeOffset = RSCanvasNode::Create();
        testNodeOffset->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeOffset->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        testNodeOffset->SetShadowColor(0xff000000);
        switch (i) {
            case 0: {
                testNodeOffset->SetShadowOffset(-50, -50);
                break;
            }
            case 1: {
                testNodeOffset->SetShadowOffset(50, -50);
                break;
            }
            case 2: {
                testNodeOffset->SetShadowOffset(-50, 50);
                break;
            }
            case 3: {
                testNodeOffset->SetShadowOffset(50, 50);
                break;
            }
            default:
                break;
        }
        testNodeOffset->SetShadowRadius(FIFTY_);
        GetRootNode()->AddChild(testNodeOffset);
        RegisterNode(testNodeOffset);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Offset_Test_2)
{
    for (int i = 0; i < FIVE_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * SIX_HUNDRED_;
        auto testNodeOffset = RSCanvasNode::Create();
        testNodeOffset->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeOffset->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        testNodeOffset->SetBackgroundColor(0xffc0c0c0);
        testNodeOffset->SetShadowColor(0xff000000);
        testNodeOffset->SetShadowRadius(50);

        switch (i) {
            case 0: {
                testNodeOffset->SetShadowOffset(-250, -250);
                break;
            }
            case 1: {
                testNodeOffset->SetShadowOffset(250, 0);
                break;
            }
            case 2: {
                testNodeOffset->SetShadowOffset(0, 250);
                break;
            }
            case 3: {
                testNodeOffset->SetShadowOffset(250, 250);
                break;
            }
            case 4: {
                testNodeOffset->SetTranslate(0, TWO_HUNDRED_, 0);
                testNodeOffset->SetShadowOffset(FIVE_HUNDRED_, FIVE_HUNDRED_);
                break;
            }
            default:
                break;
        }
        GetRootNode()->AddChild(testNodeOffset);
        RegisterNode(testNodeOffset);
    }
}

// shadow alpha
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Alpha_Test_1)
{
    float alphaList[] = { 0, 0.2, 0.8, 1 };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * SIX_HUNDRED_;
        int y = (i / TWO_) * SIX_HUNDRED_;
        auto testNodeAlpha = RSCanvasNode::Create();
        testNodeAlpha->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeAlpha->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        testNodeAlpha->SetBackgroundColor(0xffc0c0c0);
        testNodeAlpha->SetShadowColor(0xff000000);
        testNodeAlpha->SetShadowAlpha(alphaList[i]);
        testNodeAlpha->SetShadowRadius(FIFTY_);
        GetRootNode()->AddChild(testNodeAlpha);
        RegisterNode(testNodeAlpha);
    }
}

// shadow radius
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Radius_Test_1)
{
    float radiusList[] = { 0, 50, 250 };

    for (int i = 0; i < THREE_; i++) {
        int x = (i % TWO_) * SIX_HUNDRED_;
        int y = (i / TWO_) * SIX_HUNDRED_;
        auto testNodeRadius = RSCanvasNode::Create();
        testNodeRadius->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeRadius->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        testNodeRadius->SetBackgroundColor(0xffc0c0c0);
        testNodeRadius->SetShadowColor(0xff000000);
        testNodeRadius->SetShadowRadius(radiusList[i]);
        GetRootNode()->AddChild(testNodeRadius);
        RegisterNode(testNodeRadius);
    }

    // ClipToBounds
    const int yPos = 1250;
    for (int i = 0; i < 2; i++) {
        auto testNodeParent = RSCanvasNode::Create();
        testNodeParent->SetBounds({ i * SIX_HUNDRED_, yPos, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeParent->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        testNodeParent->SetClipToBounds(i);
        GetRootNode()->AddChild(testNodeParent);
        RegisterNode(testNodeParent);

        auto testNodeChild = RSCanvasNode::Create();
        testNodeChild->SetBounds({ 0, 0, FOUR_HUNDRED_, FOUR_HUNDRED_ });
        testNodeChild->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        testNodeChild->SetBackgroundColor(0xffc0c0c0);
        testNodeChild->SetShadowColor(0xff000000);
        testNodeChild->SetShadowRadius(FIFTY_);
        testNodeParent->AddChild(testNodeChild);
        RegisterNode(testNodeChild);
    }
}

// shadow color strategy
// 0 -- None
// 1 -- Average
// 2 -- Primary
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Strategy_Test_1)
{
    float alphaList[] = { 1, 0.2 };
    float radiusList[] = { 50, 100 };
    float offsetList[] = { 0, 50 };

    for (int i = 0; i < SIX_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNodeParent = RSCanvasNode::Create();
        testNodeParent->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        auto imageModifier = std::make_shared<ImageCustomModifier>();
        imageModifier->SetWidth(FIVE_HUNDRED_);
        imageModifier->SetHeight(FIVE_HUNDRED_);
        imageModifier->SetPixelMapPath("/data/local/tmp/appearance_test.jpg");
        testNodeParent->AddModifier(imageModifier);
        GetRootNode()->AddChild(testNodeParent);
        RegisterNode(testNodeParent);

        auto testNodeChild = RSCanvasNode::Create();
        testNodeChild->SetBounds({ 0, 0, TWO_HUNDRED_FIFTY_, TWO_HUNDRED_FIFTY_ });
        testNodeChild->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        testNodeChild->SetBackgroundColor(0xffffffff);
        testNodeChild->SetShadowAlpha(alphaList[i % TWO_]);
        testNodeChild->SetShadowOffset(offsetList[i % TWO_], offsetList[i % TWO_]);
        // 1st, 2nd row, Average and Primary, radius
        if (i < 4) {
            testNodeChild->SetShadowColorStrategy((i / TWO_) + 1);
            testNodeChild->SetShadowRadius(radiusList[i % TWO_]);
        }
        // third row, Shadow Elevation, normal shadow
        else {
            testNodeChild->SetShadowColorStrategy(i / TWO_);
            testNodeChild->SetShadowElevation(radiusList[i % TWO_]);
        }
        testNodeParent->AddChild(testNodeChild);
        RegisterNode(testNodeChild);
    }
}

// shadow filled
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Filled_Test_1)
{
    float alphaList[] = { 1, 0.5 };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * SIX_HUNDRED_;
        int y = (i / TWO_) * SIX_HUNDRED_;
        auto testNodeFilled = RSCanvasNode::Create();
        testNodeFilled->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeFilled->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        auto imageModifier = std::make_shared<ImageCustomModifier>();
        imageModifier->SetWidth(FIVE_HUNDRED_);
        imageModifier->SetHeight(FIVE_HUNDRED_);
        imageModifier->SetPixelMapPath("/data/local/tmp/appearance_test.jpg");
        testNodeFilled->AddModifier(imageModifier);
        testNodeFilled->SetAlpha(0.5);
        testNodeFilled->SetShadowColor(0xff00ff00);
        testNodeFilled->SetShadowOffset(FIFTY_, FIFTY_);
        testNodeFilled->SetShadowRadius(FIFTY_);
        testNodeFilled->SetShadowIsFilled(i / TWO_);
        testNodeFilled->SetShadowAlpha(alphaList[i % TWO_]);
        GetRootNode()->AddChild(testNodeFilled);
        RegisterNode(testNodeFilled);
    }
}

// shadow mask
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_mask_Test_1)
{
    for (int i = 0; i < TWO_; i++) {
        int x = (i % TWO_) * SIX_HUNDRED_;
        int y = (i / TWO_) * SIX_HUNDRED_;
        auto testNodeMask = RSCanvasNode::Create();
        testNodeMask->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        auto imageModifier = std::make_shared<ImageCustomModifier>();
        imageModifier->SetWidth(FIVE_HUNDRED_);
        imageModifier->SetHeight(FIVE_HUNDRED_);
        imageModifier->SetPixelMapPath("/data/local/tmp/appearance_test.jpg");
        testNodeMask->AddModifier(imageModifier);
        testNodeMask->SetShadowMask(i);
        testNodeMask->SetShadowRadius(ONE_HUNDRED_);
        testNodeMask->SetShadowAlpha(0.5);
        GetRootNode()->AddChild(testNodeMask);
        RegisterNode(testNodeMask);
    }
}

// shadow touch
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Touch_Test_1)
{
    float alphaList[] = { 1, 0.5 };
    uint32_t colorList[] = { 0xff000000, 0xffff0000 };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * SIX_HUNDRED_;
        int y = (i / TWO_) * SIX_HUNDRED_;
        auto testNodeTouch = RSCanvasNode::Create();
        testNodeTouch->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeTouch->SetShadowColor(colorList[i]);
        testNodeTouch->SetShadowRadius(ONE_HUNDRED_);
        testNodeTouch->SetShadowAlpha(alphaList[i / TWO_]);
        GetRootNode()->AddChild(testNodeTouch);
        RegisterNode(testNodeTouch);
    }
}

} // namespace OHOS::Rosen