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

/* SetScaleX: 1.0f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleX_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleX(1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleX: 0.5f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleX_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleX(0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleX: 2.0 f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleX_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleX(2.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleY: 1.0f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleY_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleY(1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleY: 0.5f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleY_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleY(0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleY: 2.0 f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleY_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleY(2.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScale1f: 1.0f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale1f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale(1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScale1f: 0.5f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale1f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale(0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScale1f: 2.0 f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale1f_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale(2.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScale2f: 0.5f, 2.0f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale2f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale(0.5f, 2.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScale2f: 0.5f, 0.5f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale2f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale(0.5f, 0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScale2f: 2.0f, 0.5f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale2f_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale(2.0f, 0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScale2f: 2.0f, 2.0f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale2f_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale(2.0f, 2.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleV2f: 0.5f, 2.0f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleV2f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale({ 0.5f, 2.0f });
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleV2f: 0.5f, 0.5f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleV2f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale({ 0.5f, 0.5f });
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleV2f: 2.0f, 0.5f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleV2f_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale({ 2.0f, 0.5f });
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScale2f: 2.0f, 2.0f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleV2f_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScale({ 2.0f, 2.0f });
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleX and SetScaleY: 0.5f, 2.0f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleX_And_SetScaleY_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleX(0.5f);
    testNode->SetScaleY(2.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleV2f: 0.5f, 0.5f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleX_And_SetScaleY_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleX(0.5f);
    testNode->SetScaleY(0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleV2f: 2.0f, 0.5f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleX_And_SetScaleY_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleX(2.0f);
    testNode->SetScaleY(0.5f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScale2f: 2.0f, 2.0f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleX_And_SetScaleY_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetScaleX(2.0f);
    testNode->SetScaleY(2.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetScaleZ */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleZ_Test_1)
{
    vector<float> vecs = { -0.5, 0.0, 0.5, 1, 100.0 };
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetScaleZ(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case1 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_1)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetScale({ scaleData[i], 0.5 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case2 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_2)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetRotation(0.0, 0, 45.0);
        testNode->SetScale({ scaleData[i], 0.5 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case3 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_3)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetRotation(45.0, 0, 0.0);
        testNode->SetScale({ scaleData[i], 0.5 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case4 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_4)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetScaleZ(0.5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case5 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_5)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetScaleZ(0.5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case6 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_6)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = { screenWidth / col, screenHeight / row };
    int nodeSpace[2] = { 10, 10 }; // col space , row space
    float scale = 1.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1] });
            testNode->SetPivot(Vector2f(0, 0));
            testNode->SetScaleX(scale);
            testNode->SetScaleY(scale);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            scale -= 0.06;
        }
    }
}

/* SetScale: composition case7 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_7)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = { screenWidth / col, screenHeight / row };
    int nodeSpace[2] = { 10, 10 }; // col space , row space
    float scale = 0.99;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1] });
            testNode->SetPivot(Vector2f(0, 0));
            testNode->SetScaleZ(scale);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            scale -= 0.06;
        }
    }
}

/* SetScale: composition case8 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_8)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(1, 1));
        testNode->SetRotation(45.0, 0, 45.0);
        testNode->SetScale({ scaleData[i], scaleData[i]});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case9 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_9)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(1, 1));
        testNode->SetRotation(45.0, 0, 0.0);
        testNode->SetScale({ scaleData[i], 0.1 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case10 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_10)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(1, 1));
        testNode->SetScale({ scaleData[i], scaleData[i] });
        testNode->SetScaleZ(0.5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case11 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_11)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(1, 1));
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetScale({ scaleData[i], scaleData[i] });
        testNode->SetScaleZ(0.5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetScale: composition case12 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_12)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = { screenWidth / col, screenHeight / row };
    int nodeSpace[2] = { 10, 10 }; // col space , row space
    float scale = 1.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1] });
            testNode->SetPivot(Vector2f(1, 1));
            testNode->SetScaleX(scale);
            testNode->SetScaleY(scale);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            scale -= 0.06;
        }
    }
}

/* SetScale: composition case13 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_13)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = { screenWidth / col, screenHeight / row };
    int nodeSpace[2] = { 10, 10 }; // col space , row space
    float scale = 0.99;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1] });
            testNode->SetPivot(Vector2f(1, 1));
            testNode->SetScaleZ(scale);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            scale -= 0.06;
        }
    }
}

} // namespace OHOS::Rosen