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

/* SetFrame + Alpha: alpha = 0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetFrame({ 50, 50, 300, 300 });
    testNode->SetAlpha(0.0f);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame + Alpha: alpha = 0.5 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 400, 400 });
    testNode->SetFrame({ 50, 50, 300, 300 });
    testNode->SetAlpha(0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame + Alpha: alpha = 1.0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 400, 400 });
    testNode->SetFrame({ 50, 50, 300, 300 });
    testNode->SetAlpha(1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame + Alpha: different alpha with same frame */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_4)
{
    std::vector<float> alphaList = { 0.2f, 0.4f, 0.6f, 0.8f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { (int)i * 260 + 50, 100, 250, 250 });
        testNode->SetFrame({ 25, 25, 200, 200 });
        testNode->SetAlpha(alphaList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame + Alpha: different frames with same alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_5)
{
    std::vector<Vector4f> frameList = {
        { 0, 0, 150, 150 },
        { 50, 0, 150, 150 },
        { 0, 50, 150, 150 },
        { 50, 50, 150, 150 }
    };

    float alpha = 0.7f;
    for (const auto& frame : frameList) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { 100, 100, 400, 400 });
        testNode->SetFrame({ frame.x_, frame.y_, frame.z_, frame.w_ });
        testNode->SetAlpha(alpha);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame + Alpha: frame size with alpha gradient */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_6)
{
    std::vector<Vector4f> frameList = {
        { 0, 0, 100, 100 },
        { 0, 0, 200, 200 },
        { 0, 0, 300, 300 }
    };

    for (size_t i = 0; i < frameList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { (int)i * 380 + 50, 100, 350, 350 });
        testNode->SetFrame({ frameList[i].x_, frameList[i].y_,
            frameList[i].z_, frameList[i].w_ });
        testNode->SetAlpha(0.3f + i * 0.2f);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame + Alpha: overlapping frames with alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_7)
{
    // Node 1
    auto testNode1 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode1->SetFrame({ 0, 0, 300, 300 });
    testNode1->SetAlpha(0.8f);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Node 2 - overlapping
    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({ 200, 200, 400, 400 });
    testNode2->SetFrame({ 50, 50, 300, 300 });
    testNode2->SetBackgroundColor(0xff0000ff);
    testNode2->SetAlpha(0.5f);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);
}

/* SetFrame + Alpha: update both frame and alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_8)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode->SetFrame({ 50, 50, 300, 300 });
    testNode->SetAlpha(0.6f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update frame and alpha
    testNode->SetFrame({ 100, 100, 200, 200 });
    testNode->SetAlpha(0.9f);
}

/* SetFrame + Alpha: frame offset with alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_9)
{
    std::vector<Vector4f> offsetList = {
        { 0, 0, 200, 200 },
        { 50, 0, 200, 200 },
        { 0, 50, 200, 200 },
        { 50, 50, 200, 200 }
    };

    for (const auto& offset : offsetList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 100, 100, 400, 400 });
        testNode->SetFrame({ offset.x_, offset.y_, offset.z_, offset.w_ });
        testNode->SetBackgroundColor(0xff00ff00);
        testNode->SetAlpha(0.7f);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame + Alpha: clip with alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Test_10)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode->SetFrame({ -50, -50, 500, 500 }); // frame extends beyond bounds
    testNode->SetAlpha(0.8f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame + Alpha: matrix test - alpha x frame offset (5x4) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Matrix_5x4)
{
    std::vector<float> alphaList = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
    std::vector<int> offsets = { 0, 25, 50, 75 };

    for (size_t row = 0; row < alphaList.size(); row++) {
        for (size_t col = 0; col < offsets.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { (int)col * 240 + 50, (int)row * 180 + 50, 200, 200 });
            testNode->SetFrame({ offsets[col], offsets[col], 150, 150 });
            testNode->SetAlpha(alphaList[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFrame + Alpha: extreme frame positions */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Extreme_Position)
{
    std::vector<Vector4f> extremeFrames = {
        { -1000, -1000, 100, 100 },  // 极端负位置
        { 1000, 1000, 100, 100 },     // 极端正位置
        { 0, 0, 1, 1 },               // 最小尺寸
        { 0, 0, 2000, 2000 }          // 超大尺寸
    };

    float alpha = 0.7f;
    for (const auto& frame : extremeFrames) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 100, 100, 400, 400 });
        testNode->SetFrame({ frame.x_, frame.y_, frame.z_, frame.w_ });
        testNode->SetAlpha(alpha);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame + Alpha: zero/negative frame dimensions */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Zero_Negative)
{
    std::vector<Vector4f> frameList = {
        { 50, 50, 0, 100 },      // width=0
        { 50, 50, 100, 0 },      // height=0
        { 50, 50, -100, 100 },   // negative width
        { 50, 50, 100, -100 },   // negative height
        { 50, 50, 0, 0 }         // both zero
    };

    for (const auto& frame : frameList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 100, 100, 400, 400 });
        testNode->SetFrame({ frame.x_, frame.y_, frame.z_, frame.w_ });
        testNode->SetAlpha(0.8f);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame + Alpha: frame larger than bounds with alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Larger_Than_Bounds)
{
    std::vector<float> alphaList = { 0.3f, 0.5f, 0.7f, 0.9f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { (int)i * 280 + 50, 100, 250, 250 });
        // Frame larger than bounds
        testNode->SetFrame({ -50, -50, 350, 350 });
        testNode->SetAlpha(alphaList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame + Alpha: conflict - zero frame with full alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Conflict_Test)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetFrame({ 100, 100, 0, 0 }); // zero frame
    testNode->SetAlpha(1.0f); // full alpha
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetFrame + Alpha: bounds-frame aspect ratio mismatch */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Frame_Aspect_Mismatch)
{
    // Test different combinations of bounds and frame aspect ratios
    std::vector<Vector4f> boundsList = {
        { 100, 100, 400, 200 },    // wide bounds
        { 100, 350, 200, 400 },    // tall bounds
        { 100, 800, 400, 400 },    // square bounds
        { 600, 100, 300, 300 }     // square bounds
    };

    std::vector<Vector4f> frameList = {
        { 0, 0, 400, 400 },        // square frame
        { 0, 0, 400, 400 },        // square frame
        { 0, 0, 200, 200 },        // small frame
        { 0, 0, 500, 500 }         // large frame
    };

    float alpha = 0.6f;
    for (size_t i = 0; i < boundsList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { boundsList[i].x_, boundsList[i].y_,
              boundsList[i].z_, boundsList[i].w_ });
        testNode->SetFrame({ frameList[i].x_, frameList[i].y_,
            frameList[i].z_, frameList[i].w_ });
        testNode->SetAlpha(alpha);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
