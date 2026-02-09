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

/* SetBounds + Alpha: alpha < 0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetAlpha(-1.0f); // treated as 0.0f
    testNode->SetBounds({ 100, 100, 300, 300 });
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds + Alpha: alpha = 0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_2)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetAlpha(0.0f);
    testNode->SetBounds({ 200, 200, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds + Alpha: alpha = 0.5 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 400, 400 });
    testNode->SetAlpha(0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds + Alpha: alpha = 1.0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 400, 400 });
    testNode->SetAlpha(1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds + Alpha: alpha > 1.0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 400, 400 });
    testNode->SetAlpha(1.5f); // treated as 1.0f
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds + Alpha: different alpha values */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_6)
{
    std::vector<float> alphaList = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { i * 220 + 50, 100, 200, 200 });
        testNode->SetAlpha(alphaList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds + Alpha: overlapping nodes with different alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_7)
{
    // Bottom node
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 100, 100, 400, 400 });
    testNode1->SetBackgroundColor(0xffff0000);
    testNode1->SetAlpha(1.0f);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Top node
    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({ 200, 200, 400, 400 });
    testNode2->SetBackgroundColor(0xff0000ff);
    testNode2->SetAlpha(0.5f);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);
}

/* SetBounds + Alpha: bounds position with alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_8)
{
    std::vector<Vector4f> boundsList = {
        { 0, 0, 300, 300 },
        { 300, 0, 300, 300 },
        { 0, 300, 300, 300 },
        { 300, 300, 300, 300 }
    };

    float alpha = 0.7f;
    for (const auto& bounds : boundsList) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetAlpha(alpha);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds + Alpha: update alpha dynamically */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_9)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetAlpha(0.8f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update alpha
    testNode->SetAlpha(0.3f);
}

/* SetBounds + Alpha: different bounds sizes with same alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_10)
{
    std::vector<Vector4f> sizeList = {
        { 100, 100, 100, 100 },
        { 250, 100, 200, 100 },
        { 100, 250, 100, 200 },
        { 250, 250, 200, 200 }
    };

    for (const auto& bounds : sizeList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetBackgroundColor(0xff00ff00);
        testNode->SetAlpha(0.6f);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds + Alpha: blend mode test */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Test_11)
{
    // Background node
    auto bgNode = RSCanvasNode::Create();
    bgNode->SetBounds({ 0, 0, 600, 600 });
    bgNode->SetBackgroundColor(0xffffffff);
    GetRootNode()->AddChild(bgNode);

    // Foreground node with alpha
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 400, 400 });
    testNode->SetAlpha(0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds + Alpha: matrix test - alpha x bounds size (5x4) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Matrix_5x4)
{
    std::vector<float> alphaList = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
    std::vector<int> sizeList = { 100, 150, 200, 250 };

    for (size_t row = 0; row < alphaList.size(); row++) {
        for (size_t col = 0; col < sizeList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 240 + 50, (int)row * 180 + 50,
                sizeList[col], sizeList[col] });
            testNode->SetAlpha(alphaList[row]);
            testNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetBounds + Alpha: extreme alpha values */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Extreme_Alpha)
{
    std::vector<float> extremeAlphas = { -100.0f, -1.0f, -0.1f, 0.0f, 0.001f,
        0.5f, 0.999f, 1.0f, 1.001f, 2.0f, 100.0f };

    for (size_t i = 0; i < extremeAlphas.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 100 + 50, 100, 90, 90 });
        testNode->SetAlpha(extremeAlphas[i]);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds + Alpha: extreme bounds values */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Extreme_Bounds)
{
    std::vector<Vector4f> extremeBounds = {
        { 0, 0, 1, 1 },           // 最小尺寸
        { 0, 0, 10, 10 },         // 极小尺寸
        { -1000, -1000, 100, 100 }, // 负位置
        { 1100, 1900, 100, 100 },  // 边界位置
        { 0, 0, 1200, 2000 },      // 全屏
        { 500, 500, 2000, 2000 }   // 超出屏幕
    };

    float alpha = 0.7f;
    for (const auto& bounds : extremeBounds) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetAlpha(alpha);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds + Alpha: zero/negative dimensions */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Zero_Negative)
{
    std::vector<Vector4f> boundsList = {
        { 100, 100, 0, 100 },     // width=0
        { 100, 250, 100, 0 },     // height=0
        { 100, 400, -100, 100 },  // negative width
        { 100, 550, 100, -100 },  // negative height
        { 100, 700, 0, 0 }        // both zero
    };

    for (const auto& bounds : boundsList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetAlpha(0.8f);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBounds + Alpha: conflict scenario - full alpha with zero bounds */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_Conflict_Test)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 0, 0 }); // 无尺寸
    testNode->SetAlpha(1.0f); // 完全不透明
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBounds + Alpha: parent-child bounds with alpha */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Alpha_Bounds_ParentChild)
{
    // 3x3 matrix: parent bounds size x child alpha
    std::vector<int> parentSizes = { 300, 400, 500 };
    std::vector<float> childAlphas = { 0.3f, 0.6f, 0.9f };

    for (size_t row = 0; row < parentSizes.size(); row++) {
        for (size_t col = 0; col < childAlphas.size(); col++) {
            auto parent = RSCanvasNode::Create();
            int size = parentSizes[row];
            parent->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, size, size });
            parent->SetAlpha(0.5f);
            parent->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(parent);

            auto child = RSCanvasNode::Create();
            child->SetBounds({ 50, 50, size - 100, size - 100 });
            child->SetAlpha(childAlphas[col]);
            child->SetBackgroundColor(0xff0000ff);
            parent->AddChild(child);
            RegisterNode(parent);
        }
    }
}

} // namespace OHOS::Rosen
