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
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
    }
};

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslationXY_Test_1)
{
    float transList[] = { 0, 250, 500 };
    for (int i = 0; i < 3; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, 500, 500 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetTranslate(Vector2f(transList[i], transList[i]));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslationZ_Test_2)
{
    float transList[] = { 0, 250, 500 };
    for (int i = 0; i < 3; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, 500, 500 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetTranslateX(transList[i]);
        testNode->SetTranslateY(transList[i]);
        testNode->SetTranslateZ(transList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SetPositionZ_Test_1)
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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SetPositionX_Test_1)
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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SetPositionX_Test_2)
{
    std::array<float, 3> translateData = { 0.f, 20.f, -20.f};
    for (int i = 0; i < translateData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(0.0, 0, 45.0);
        testNode->SetTranslate(translateData[i], 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SetPositionX_Test_3)
{
    std::array<float, 3> translateData = { 0.f, 20.f, -20.f};
    for (int i = 0; i < translateData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 0.0);
        testNode->SetTranslate(translateData[i], 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_SetPosition_linear_Test_1)
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
} // namespace OHOS::Rosen