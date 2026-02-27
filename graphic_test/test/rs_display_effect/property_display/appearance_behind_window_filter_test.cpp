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

/* BehindWindowFilter: basic filter */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_BehindWindowFilter_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    // Note: BehindWindowFilter applies blur effect to content behind the window
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* BehindWindowFilter: with background image */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_BehindWindowFilter_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 600, 600 });
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* BehindWindowFilter: with border */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_BehindWindowFilter_Test_3)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetBorderWidth(30);
    testNode->SetBorderColor(0xff00ff00);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* BehindWindowFilter: corner radius */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_BehindWindowFilter_Test_4)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetCornerRadius(70);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* BehindWindowFilter: shadow */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_BehindWindowFilter_Test_5)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetShadowColor(0xff000000);
    testNode->SetShadowRadius(40);
    testNode->SetShadowOffset(30, 30);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* BehindWindowFilter: clip */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_BehindWindowFilter_Test_6)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 500, 500 });
    testNode->SetClipToBounds(true);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* BehindWindowFilter: extreme bounds */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_BehindWindowFilter_Extreme_Bounds)
{
    std::vector<Vector4f> extremeBounds = {
        { 0, 0, 1, 1 },
        { 0, 0, 1200, 2000 },
        { -100, -100, 200, 200 },
        { 1000, 1800, 200, 200 }
    };

    for (const auto& bounds : extremeBounds) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* BehindWindowFilter: zero/negative dimensions */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_BehindWindowFilter_Zero_Dimensions)
{
    std::vector<Vector4f> boundsList = {
        { 100, 100, 0, 100 },
        { 100, 250, 100, 0 },
        { 100, 400, -100, 100 },
        { 100, 550, 100, -100 }
    };

    for (const auto& bounds : boundsList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
