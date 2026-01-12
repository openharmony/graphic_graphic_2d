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

/* SetSkewX： -30° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewX_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewX(-30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewX： 0° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewX_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewX(0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewX： 30° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewX_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewX(30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewY： -30° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewY_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewY(-30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewY： 0° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewY_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewY(0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewY： 30° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewY_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewY(30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewX AND SetSkewY：(-30°, -30°)*/
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewX_And_SkewY_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewX(-30.0f);
    testNode->SetSkewY(-30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewX AND SetSkewY：(-30°, 30°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewX_And_SkewY_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewX(-30.0f);
    testNode->SetSkewY(30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewX AND SetSkewY：(30°, -30°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewX_And_SkewY_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewX(30.0f);
    testNode->SetSkewY(-30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewX AND SetSkewY：(30°, 30°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewX_And_SkewY_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewX(30.0f);
    testNode->SetSkewY(30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewX AND SetSkewY AND SetSkewZ：(15°, 0°, -15°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SkewX_And_SkewY_And_SkewZ_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkewX(15.0f);
    testNode->SetSkewY(0.0f);
    testNode->SetSkewZ(-15.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew1f：(-30°, -30°, -30°)*/
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew1f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(-30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew1f：(0, 0, 0)*/
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew1f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew1f：(30°, 30°, 30°)*/
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew1f_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew2f：(-30°, -30°)*/
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew2f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(-30.0f, -30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew2f：(-30°, 30°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew2f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(-30.0f, 30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew2f：(30°, -30°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew2f_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(30.0f, -30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew2f：(30°, 30°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew2f_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(30.0f, 30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew3f：(-30°, 0°, 30°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew3f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(-30.0f, 0.0f, 30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew3f：(0, 30, -30) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew3f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(0.0f, 30.0, -30.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew3f：(30°, -30°, 0°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew3f_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew(30.0f, -30.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewV3f：(-30°, 30°, 0°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew3f_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew({-30.0f, 30.0f, 0.0f});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewV3f：(0, -30, 30) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew3f_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew({0.0f, -30.0, 30.0f});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkewV3f：(30°, 0°, -30°) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew3f_Test_6)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetSkew({30.0f, 0.0f, -30.0f});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetSkew： composition case1 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_1)
{
    std::vector<std::vector<float> >  skewValues= {
        { 0.5, 0},
        { 0, 0.5},
        { 0.5, 0.5},
    };
    for (int i = 0; i < skewValues.size(); ++i) {
        auto testNode =
            SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 110, i * 420 + 110, 200, 200 });
        testNode->SetPivot(Vector2f(0.0, 0.0));
        testNode->SetSkew(skewValues[i][0], skewValues[i][1], 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetSkew： composition case2 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_2)
{
    std::array<float, 3> skewData = { 0.f, 0.5f, -0.5f};
    for (int i = 0; i < skewData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetSkew(skewData[i], 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetSkew： composition case3 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_3)
{
    std::array<float, 3> skewData = { 0.f, 0.5f, -0.5f};
    for (int i = 0; i < skewData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(0.0, 0, 45.0);
        testNode->SetSkew(skewData[i], 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetSkew： composition case4 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_4)
{
    std::array<float, 3> skewData = { 0.f, 0.5f, -0.5f};
    for (int i = 0; i < skewData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 0.0);
        testNode->SetSkew(skewData[i], 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetSkew： composition case5 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_5)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float skewX = 0.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetSkewX(skewX);
            testNode->SetPerspW(2.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            skewX += 0.1;
        }
    }
}

/* SetSkew： composition case6 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_6)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float skewY = 0.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetSkewY(skewY);
            testNode->SetPerspW(2.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            skewY += 0.1;
        }
    }
}

/* SetSkew： composition case7 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_7)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float skewZ = 0.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetRotation(45.0, 0, 0.0);
            testNode->SetSkew({0.f, 0.f, skewZ});
            testNode->SetPerspW(2.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            skewZ += 0.1;
        }
    }
}

/* SetSkew： composition case8 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_8)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<std::vector<float>> skewValues = {{-0.5, -0.1, -0.5}, {0.1, 0.5, 0.5}, {0.2, 0.3, 0.2},
        {1.5, 0.5, 1}, {0.5, 1.5, 1}, {10, 20, 10}, {0.5, 0.5, 0.5}, {1.2, 1.2, 1.2}};
    for (int i = 0; i < skewValues.size(); ++i) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetPivot(Vector2f(0.0, 0.0));
        testNode->SetSkew(skewValues[i][0], skewValues[i][1], skewValues[i][2]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetSkew： composition case9 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_9)
{
    std::vector<std::vector<float> >  skewValues= {
        { 0.5, 0, 0.3},
        { 0, 0.5, 0.3},
        { 0.5, 0.5, 0.4},
    };
    for (int i = 0; i < skewValues.size(); ++i) {
        auto testNode =
            SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 110, i * 420 + 110, 200, 200 });
        testNode->SetPivot(Vector2f(1.0, 1.0));
        testNode->SetSkew(skewValues[i][0], skewValues[i][1], skewValues[i][2]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetSkew： composition case10 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_10)
{
    std::array<float, 3> skewData = { 0.f, 0.5f, -0.5f};
    for (int i = 0; i < skewData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetSkew(skewData[i], skewData[i], skewData[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetSkew： composition case11 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_11)
{
    std::array<float, 3> skewData = { 0.f, 0.5f, -0.5f};
    for (int i = 0; i < skewData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.0, 0.0));
        testNode->SetRotation(0.0, 0, 45.0);
        testNode->SetSkew(skewData[i], 0.5, skewData[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetSkew： composition case12 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_12)
{
    std::array<float, 3> skewData = { 0.f, 0.5f, -0.5f};
    for (int i = 0; i < skewData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(1.0, 1.0));
        testNode->SetRotation(45.0, 0, 0.0);
        testNode->SetSkew(skewData[i], skewData[i], 0.2);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetSkew： composition case13 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_13)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float skew = 0.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(2.0, 2.0));
            testNode->SetSkewX(skew);
            testNode->SetSkewY(skew);
            testNode->SetPerspW(2.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            skew += 0.1;
        }
    }
}

/* SetSkew： composition case14 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_14)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float skewY = 0.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(1.0, 1.0));
            testNode->SetSkewY(skewY);
            testNode->SetPerspW(2.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            skewY += 0.1;
        }
    }
}

/* SetSkew： composition case15 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_15)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float skewZ = 0.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(0.0, 0.3));
            testNode->SetRotation(45.0, 0, 0.0);
            testNode->SetSkew({0.1f, 0.2f, skewZ});
            testNode->SetPerspW(2.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            skewZ += 0.1;
        }
    }
}

/* SetSkew： composition case16 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_16)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<std::vector<float>> skewValues = {{-0.5, -0.1, -0.5}, {0.1, 0.5, 0.5}, {0.2, 0.3, 0.2},
        {1.5, 0.5, 1}, {0.5, 1.5, 1}, {10, 20, 10}, {0.5, 0.5, 0.5}, {1.2, 1.2, 1.2}};
    for (int i = 0; i < skewValues.size(); ++i) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetPivot(Vector2f(0.4, 0.8));
        testNode->SetSkew(skewValues[i][0], skewValues[i][1], skewValues[i][2]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen