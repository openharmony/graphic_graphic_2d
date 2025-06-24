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

/* SetTranslateX： x[250, 750]-->[350, 850] */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateX_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 250, 250, 500, 500 });
    testNode->SetTranslateX(100);   // Pivot default:0.5f, 0.5f, 0.0f
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetTranslateX： x[250, 750]-->[-50, 450] < 0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateX_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 250, 250, 500, 500 });
    testNode->SetTranslateX(-300);  // Pivot default:0.5f, 0.5f, 0.0f
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetTranslateX： x[250, 750]-->[750, 1250] > 1200 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateX_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 250, 250, 500, 500 });
    testNode->SetTranslateX(500);   // Pivot default:0.5f, 0.5f, 0.0f
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetTranslateX： change Pivot */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateX_Test_4)
{
    std::vector<float> transList = { -250, 0, 250, 500 };
    for (int i = 0; i < transList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 500, 500 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetTranslateX(transList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetTranslateX： change RotationX */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateX_Test_5)
{
    vector<float> transList = {-250, 0, 250, 500};
    for (int i = 0; i < transList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotationX(30);
        testNode->SetTranslateX(transList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetTranslateY： y[500, 1000]-->[600, 1100] */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateY_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 500, 500 });
    testNode->SetTranslateY(100);   // Pivot default:0.5f, 0.5f, 0.0f
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetTranslateY： y[500, 1000]-->[-100, 900] < 0 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateY_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 500, 500 });
    testNode->SetTranslateY(-600);  // Pivot default:0.5f, 0.5f, 0.0f
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetTranslateY： y[500, 1000]-->[1600, 2100] > 2000 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateY_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 500, 500 });
    testNode->SetTranslateY(1100);   // Pivot default:0.5f, 0.5f, 0.0f
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetTranslateY： change Pivot */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateY_Test_4)
{
    std::vector<float> transList = { -250, 0, 250, 500 };
    for (int i = 0; i < transList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 500, 500 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetTranslateY(transList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetTranslateY： change RotationY */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslateY_Test_5)
{
    vector<float> transList = {-250, 0, 250, 500};
    for (int i = 0; i < transList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotationY(30);
        testNode->SetTranslateY(transList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetTranslateZ: SetRotation X or Y */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslationZ_Test_1)
{
    vector<float> transList = {-250, 0, 250};
    for (int i = 0; i < transList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotationY(30);
        testNode->SetTranslateZ(transList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetTranslateZ: SetRotation Quaternion */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslationZ_Test_2)
{
    vector<float> vecs = {-250, 0, 250};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetTranslateZ(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetPositionZApplicableCamera3D: SetRotation Quaternion */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslationZ_Test_3)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    float transList[] = { -100, 0, 250, 500 };

    for (int i = 0; i < columnCount * rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetPositionZ(transList[i / 2]);
        bool flag = i % 2 == 0 ? true : false;
        testNode->SetPositionZApplicableCamera3D(flag);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetTranslateXY: SetTranslateX and SetTranslateY */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslationXY_Test_1)
{
    vector<vector<float>> transList = {{0, 0}, {250, 250}, {500, 500}};
    for (int i = 0; i < 3; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, 500, 500 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetTranslateX(transList[i][0]);
        testNode->SetTranslateY(transList[i][1]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetTranslateXYZ: SetTranslateX、SetTranslateY、SetTranslateZ */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslationXYZ_Test_1)
{
    vector<vector<float>> transList = {{0, 0, 0}, {250, 250, 250}, {500, 500, 500}};
    for (int i = 0; i < 3; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, 500, 500 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetTranslateX(transList[i][0]);
        testNode->SetTranslateY(transList[i][1]);
        testNode->SetTranslateZ(transList[i][2]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetTranslate: SetTranslate(Vector2f) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Translation_Test_1)
{
    vector<vector<float>> transList = {{0, 0}, {250, 250}, {500, 500}, {-250, -250}};
    for (int i = 0; i < transList.size(); i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, 500, 500 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetTranslate(Vector2f(transList[i][0], transList[i][1]));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetTranslate: SetTranslate */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Translation_Test_2)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {150, 250}; // col space , row space
    float translates[3] = {0.0, 0.0, 0.0};  // {0.0, 0.0, 0.0} -> x, y, z
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetScaleY(0.5);
            testNode->SetScaleY(0.7);
            testNode->SetTranslate(translates[0], translates[1], translates[2]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
        translates[0] += (nodeSpace[0] / row);
        translates[1] += ((nodeSpace[1] / row) + 20);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Translate_Test_3)
{
    std::array<float, 3> translateData = { 0.f, 20.f, -20.f};
    for (int i = 0; i < translateData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetTranslate(translateData[i], 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Translate_Test_4)
{
    std::array<float, 3> translateData = { 0.f, 20.f, -20.f};
    for (int i = 0; i < translateData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(45.0, 0, 0.0);
        testNode->SetTranslate(translateData[i], 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_2D_Concat_Test_1)
{
    vector<float> vecs1 = {-30, 30, 45, 120, -30, 60};
    vector<vector<float>> vecs2 = {{-0.5, -0.1}, {-0.1, 0.5}, {0.2, 0.3}, {1.5, 0.5}, {0.5, 1.5}, {1.2, 1.2}};
    vector<vector<float>> vecs3 = {{-0.5, -0.1}, {-0.1, 0.5}, {0.2, 0.3}, {1.5, 0.5}, {0.5, 1.5}, {1.2, 1.2}};
    vector<vector<float>> vecs4 = {{0, 0.1, 0.1, 1}, {0.1, 0.1, 0, 1}, {0.1, 0, 0.1, 1},
        {-0.1, -0.1, -0.1, -0.1}, {1, 2, 3, 4}, {-1, -2, -3, -4}};
    vector<Vector2f> vecs5 = {{-100, -100}, {100, 100}, {-100, 100}, {100, -100}, {0, 100}, {100, 0}};
    for (int i = 0; i < vecs1.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(vecs1[i]);
        testNode->SetScale(vecs2[i][0], vecs2[i][1]);
        testNode->SetTranslate(vecs5[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_3D_Concat_Test_1)
{
    vector<Quaternion> vecs1 = {{0.383, 0, 0, 0.962}, {0, 0.383, 0, 0.962}, {0, 0, 0.383, 0.962},
        {0.462, 0.191, 0.462, 0.733}, {0.462, 0.191, 0.462, 0.733}, {0.462, 0.191, 0.462, 0.733}};
    vector<vector<float>> vecs2 = {{-0.5, -0.1, -0.5}, {0.1, 0.5, 0.5}, {0.2, 0.3, 0.3}, {1.5, 0.5, 1},
        {0.5, 1.5, 1}, {1.2, 1.2, 1.2}};
    vector<vector<float>> vecs3 = {{-0.5, -0.1, -0.5}, {0.1, 0.5, 0.5}, {0.2, 0.3, 0.3}, {1.5, 0.5, 1},
        {0.5, 1.5, 1}, {1.2, 1.2, 1.2}};
    vector<vector<float>> vecs4 = {{0, 0.1, 0.1, 1}, {0.1, 0.1, 0, 1}, {0.1, 0, 0.1, 1},
        {-0.1, -0.1, -0.1, -0.1}, {1, 2, 3, 4}, {-1, -2, -3, -4}};
    vector<vector<float>> vecs5 = {{-100, -100, -100}, {100, 100, 100}, {-100, 100, 0}, {100, -100, 0},
        {0, 100, -100}, {100, 0, 100}};
    for (int i = 0; i < vecs1.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(vecs1[i]);
        testNode->SetScale(vecs2[i][0], vecs2[i][1]);
        testNode->SetScaleZ(vecs2[i][2]);
        testNode->SetTranslate(vecs5[i][0], vecs5[i][1], vecs5[i][2]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_3D_Concat_Test_2)
{
    vector<vector<float>> vecs1 = {{30, 30, 30}, {45, 30, 45}, {60, -30, -30}, {-30, 60, -30},
        {120, 30, -30}, {-30, 120, 30}};
    vector<vector<float>> vecs2 = {{-0.5, -0.1, -0.5}, {0.1, 0.5, 0.5}, {0.2, 0.3, 0.3}, {1.5, 0.5, 1},
        {0.5, 1.5, 1}, {1.2, 1.2, 1.2}};
    vector<vector<float>> vecs3 = {{-0.5, -0.1, -0.5}, {0.1, 0.5, 0.5}, {0.2, 0.3, 0.3}, {1.5, 0.5, 1},
        {0.5, 1.5, 1}, {1.2, 1.2, 1.2}};
    vector<vector<float>> vecs4 = {{0, 0.1, 0.1, 1}, {0.1, 0.1, 0, 1}, {0.1, 0, 0.1, 1},
        {-0.1, -0.1, -0.1, -0.1}, {1, 2, 3, 4}, {-1, -2, -3, -4}};
    vector<vector<float>> vecs5 = {{-100, -100, -100}, {100, 100, 100}, {-100, 100, 0}, {100, -100, 0},
        {0, 100, -100}, {100, 0, 100}};
    vector<float> vecs6 = {-50, 50, 0, 100, 50, -50};
    for (int i = 0; i < vecs1.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(vecs1[i][0], vecs1[i][1], vecs1[i][2]);
        testNode->SetScale(vecs2[i][0], vecs2[i][1]);
        testNode->SetScaleZ(vecs2[i][2]);
        testNode->SetTranslate(vecs5[i][0], vecs5[i][1], vecs5[i][2]);
        testNode->SetCameraDistance(vecs6[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen