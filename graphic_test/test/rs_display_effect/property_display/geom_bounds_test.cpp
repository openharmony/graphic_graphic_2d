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

class GeometryTest : public RSGraphicTest {
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

/* SetBounds: normal bounds */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 300, 300 });
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds: different positions */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_2)
{
    std::vector<Vector4f> boundsList = {
        { 0, 0, 200, 200 },
        { 200, 0, 200, 200 },
        { 0, 200, 200, 200 },
        { 200, 200, 200, 200 },
        { 400, 400, 400, 400 }
    };

    for (const auto& bounds : boundsList) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds: different sizes */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_3)
{
    std::vector<Vector4f> sizeList = {
        { 100, 100, 100, 100 },
        { 250, 100, 200, 150 },
        { 100, 300, 300, 200 },
        { 500, 100, 400, 400 },
        { 100, 600, 600, 300 }
    };

    for (const auto& bounds : sizeList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds: width == 0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_4)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 0, 300 }); // width = 0, not visible
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds: height == 0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_5)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 300, 0 }); // height = 0, not visible
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds: negative width (treated as 0) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_6)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, -100, 300 }); // negative width
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds: large bounds */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_7)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 0, 0, 1200, 2000 });
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds: bounds at screen edge */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_8)
{
    Vector4f boundsList[] = {
        { 0, 0, 200, 200 },           // top-left
        { 1000, 0, 200, 200 },         // top-right
        { 0, 1800, 200, 200 },         // bottom-left
        { 1000, 1800, 200, 200 }       // bottom-right
    };

    for (const auto& bounds : boundsList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetBackgroundColor(0xff00ff00);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds: partial off-screen */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_9)
{
    Vector4f boundsList[] = {
        { -100, 100, 200, 200 },      // left off-screen
        { 1100, 100, 200, 200 },      // right off-screen
        { 100, -100, 200, 200 },      // top off-screen
        { 100, 1900, 200, 200 }       // bottom off-screen
    };

    for (const auto& bounds : boundsList) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds: bounds overlap test */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_10)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 100, 100, 400, 400 });
    testNode1->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({ 300, 300, 400, 400 });
    testNode2->SetBackgroundColor(0xff0000ff);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);
}

/* SetBounds: update bounds dynamically */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Test_11)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 200, 200 });
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update bounds
    testNode->SetBounds({ 200, 200, 300, 300 });
}

/* SetBounds: extreme position values */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Extreme_Position)
{
    std::vector<Vector4f> extremePositions = {
        { -10000, -10000, 200, 200 },  // 极端负位置
        { 10000, 10000, 200, 200 },     // 极端正位置
        { 0, 0, 200, 200 },             // 原点
        { 1100, 1900, 200, 200 }        // 屏幕右下角
    };

    for (const auto& bounds : extremePositions) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds: extreme size values */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Extreme_Size)
{
    std::vector<Vector4f> extremeSizes = {
        { 100, 100, 1, 1 },           // 最小尺寸
        { 100, 250, 2, 2 },           // 极小尺寸
        { 100, 400, 5000, 5000 },     // 超大尺寸
        { 100, 600, 1200, 2000 }      // 全屏尺寸
    };

    for (const auto& bounds : extremeSizes) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds: negative width/height combinations */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Negative_Combo)
{
    std::vector<Vector4f> boundsList = {
        { 100, 100, -100, -100 },  // 两者都负
        { 100, 250, -200, 100 },   // width负
        { 100, 400, 100, -200 },   // height负
        { 100, 550, -300, -300 }   // 两者都大负值
    };

    for (const auto& bounds : boundsList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds: matrix test - position x size (4x3) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Matrix_4x3)
{
    std::vector<Vector4f> positions = {
        { 0, 0, 0, 0 }, { 800, 0, 0, 0 }, { 0, 1500, 0, 0 }, { 800, 1500, 0, 0 }
    };
    std::vector<int> sizes = { 200, 300, 400 };

    for (size_t row = 0; row < positions.size(); row++) {
        for (size_t col = 0; col < sizes.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            int x = positions[row].x_ + (int)col * 280;
            int y = positions[row].y_ + (int)row * 50;
            testNode->SetBounds({ x, y, sizes[col], sizes[col] });
            testNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetBounds: bounds with all zero components */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_All_Zero)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 0, 0 }); // 全零
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds: rapid bounds changes */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Rapid_Change)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // 快速变化
    testNode->SetBounds({ 100, 100, 100, 100 });
    testNode->SetBounds({ 200, 200, 200, 200 });
    testNode->SetBounds({ 300, 300, 300, 300 });
    testNode->SetBounds({ 400, 400, 400, 400 });
}

/* SetBounds: off-screen positions matrix */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Bounds_Offscreen_Matrix)
{
    // 完全在屏幕外
    std::vector<Vector4f> offscreenBounds = {
        { -500, 100, 200, 200 },    // 左侧外
        { 1300, 100, 200, 200 },    // 右侧外
        { 100, -500, 200, 200 },    // 上方外
        { 100, 2100, 200, 200 }     // 下方外
    };

    for (const auto& bounds : offscreenBounds) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
