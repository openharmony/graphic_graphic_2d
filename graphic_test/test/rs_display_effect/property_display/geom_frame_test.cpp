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

/* SetFrame: normal frame */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetFrame({ 50, 50, 300, 300 });
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame: frame offset from bounds */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 400, 400 });
    testNode->SetFrame({ 100, 100, 200, 200 }); // offset from bounds origin
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame: frame at different positions */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_3)
{
    std::vector<Vector4f> frameList = {
        { 0, 0, 200, 200 },
        { 50, 0, 200, 200 },
        { 0, 50, 200, 200 },
        { 50, 50, 200, 200 },
        { 100, 100, 200, 200 }
    };

    for (const auto& frame : frameList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 100, 100, 400, 400 });
        testNode->SetFrame({ frame.x_, frame.y_, frame.z_, frame.w_ });
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: frame larger than bounds */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 300, 300 });
    testNode->SetFrame({ 0, 0, 400, 400 }); // frame larger than bounds
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame: frame with zero width/height */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_5)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetFrame({ 50, 50, 0, 0 }); // zero size frame
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame: negative frame position */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_6)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 200, 200, 400, 400 });
    testNode->SetFrame({ -50, -50, 300, 300 }); // negative offset
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame: different frame sizes */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_7)
{
    std::vector<Vector4f> frameList = {
        { 0, 0, 100, 100 },
        { 0, 0, 200, 200 },
        { 0, 0, 300, 300 },
        { 0, 0, 400, 400 }
    };

    for (const auto& frame : frameList) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { 100, 100, 400, 400 });
        testNode->SetFrame({ frame.x_, frame.y_, frame.z_, frame.w_ });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: frame aspect ratio different from bounds */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_8)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 400, 200 });
    testNode->SetFrame({ 0, 0, 300, 300 }); // different aspect ratio
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame: update frame dynamically */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_9)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetFrame({ 50, 50, 300, 300 });
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update frame
    testNode->SetFrame({ 100, 100, 200, 200 });
}

/* SetFrame: multiple nodes with frames */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Test_10)
{
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { (i % 2) * 450 + 100, (i / 2) * 450 + 100, 350, 350 });
        testNode->SetFrame({ 25, 25, 300, 300 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: extreme frame values */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Extreme_Values)
{
    std::vector<Vector4f> extremeFrames = {
        { -5000, -5000, 100, 100 },  // 极端负偏移
        { 5000, 5000, 100, 100 },     // 极端正偏移
        { 0, 0, 0, 0 },               // 全零
        { 0, 0, 1, 1 },               // 最小尺寸
        { 0, 0, 5000, 5000 }          // 超大尺寸
    };

    for (const auto& frame : extremeFrames) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 100, 100, 400, 400 });
        testNode->SetFrame({ frame.x_, frame.y_, frame.z_, frame.w_ });
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: negative frame dimensions */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Negative_Dimensions)
{
    std::vector<Vector4f> frameList = {
        { 50, 50, -100, 100 },   // negative width
        { 50, 50, 100, -100 },   // negative height
        { 50, 50, -200, -200 },  // both negative
        { 50, 50, -50, -50 }     // small negative
    };

    for (const auto& frame : frameList) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { 100, 100, 400, 400 });
        testNode->SetFrame({ frame.x_, frame.y_, frame.z_, frame.w_ });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: matrix test - frame position x size (4x3) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Matrix_4x3)
{
    std::vector<int> positions = { 0, 50, 100, 150 };
    std::vector<int> sizes = { 100, 200, 300 };

    for (size_t row = 0; row < positions.size(); row++) {
        for (size_t col = 0; col < sizes.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ 100, 100, 400, 400 });
            testNode->SetFrame({ positions[row], positions[row],
                sizes[col], sizes[col] });
            testNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFrame: frame-bounds size relationship matrix */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Bounds_Relationship)
{
    // frame smaller than bounds
    auto testNode1 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode1->SetFrame({ 50, 50, 200, 200 });
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // frame equal to bounds
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 550, 100, 400, 400 });
    testNode2->SetFrame({ 0, 0, 400, 400 });
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);

    // frame larger than bounds
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 550, 400, 400 });
    testNode3->SetFrame({ -50, -50, 500, 500 });
    GetRootNode()->AddChild(testNode3);
    RegisterNode(testNode3);

    // frame extends beyond bounds
    auto testNode4 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 550, 550, 400, 400 });
    testNode4->SetFrame({ -100, -100, 600, 600 });
    GetRootNode()->AddChild(testNode4);
    RegisterNode(testNode4);
}

/* SetFrame: zero frame edge cases */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Zero_Edges)
{
    std::vector<Vector4f> zeroEdgeFrames = {
        { 0, 0, 0, 300 },      // left edge, width=0
        { 0, 0, 300, 0 },      // top edge, height=0
        { 300, 0, 0, 300 },    // right edge, width=0
        { 0, 300, 300, 0 }     // bottom edge, height=0
    };

    for (const auto& frame : zeroEdgeFrames) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 100, 100, 400, 400 });
        testNode->SetFrame({ frame.x_, frame.y_, frame.z_, frame.w_ });
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: rapid frame changes */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Frame_Rapid_Changes)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode->SetFrame({ 0, 0, 400, 400 });
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Rapid changes
    testNode->SetFrame({ 50, 50, 300, 300 });
    testNode->SetFrame({ 100, 100, 200, 200 });
    testNode->SetFrame({ 150, 150, 100, 100 });
    testNode->SetFrame({ 0, 0, 400, 400 });
}

} // namespace OHOS::Rosen
