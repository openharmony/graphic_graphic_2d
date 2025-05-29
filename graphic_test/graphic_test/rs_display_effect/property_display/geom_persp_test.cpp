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
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_1)
{
    float xList[] = { -1.0, 0.0, 2.0 };
    float yList[] = { -1.0, 0.0, 1.0 };
    for (int i = 0; i < 1; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 680 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 45.0);
        testNode->SetPerspX(xList[i]);
        testNode->SetPerspY(yList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_2)
{
    float xList[] = { -1.0, 0.0, 1 };
    float yList[] = { -1.0, 0.0, 0 };
    for (int i = 1; i < 3; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, (i - 1) * 680 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 45.0);
        testNode->SetPersp(xList[i], yList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_3)
{
    float xList[] = { -1.0, 0.0, 1 };
    float yList[] = { -1.0, 0.0, 0 };
    for (int i = 1; i < 3; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, (i - 1) * 680 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 45.0);
        testNode->SetPersp({ xList[i], yList[i], 0.0, 1.0 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_4)
{
    float persp[] = { 0.8, 1.0 };
    for (int i = 0; i < 2; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 880 + 200, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(0.0, 0, 45.0);
        testNode->SetPersp(persp[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_5)
{
    std::array<float, 3> perspData = { 1.0f, 2.0f, 0.5f };
    for (int i = 0; i < perspData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(0.0, 0, 45.0);
        testNode->SetPerspW(perspData[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_6)
{
    std::array<float, 3> perspData = { 1.0f, 2.0f, 0.5f };
    for (int i = 0; i < perspData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 0.0);
        testNode->SetPerspW(perspData[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_7)
{
    std::array<float, 3> perspData = { 1.0f, 2.0f, 0.5f };
    for (int i = 0; i < perspData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 350 + 20, 300, 300 });
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetPerspZ(perspData[i]);
        testNode->SetPerspW(perspData[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_8)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = { screenWidth / col, screenHeight / row };
    int nodeSpace[2] = { 10, 10 }; // col space , row space
    float perspX = 0.001;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1] });
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetPerspX(perspX);
            testNode->SetPerspW(4.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            perspX += 0.001;
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_9)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = { screenWidth / col, screenHeight / row };
    int nodeSpace[2] = { 10, 10 }; // col space , row space
    float perspY = 0.001;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1] });
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetPerspY(perspY);
            testNode->SetPerspW(4.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            perspY += 0.001;
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_10)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = { screenWidth / col, screenHeight / row };
    int nodeSpace[2] = { 10, 10 }; // col space , row space
    float perspZ = 0.001;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1] });
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetPerspZ(perspZ);
            testNode->SetPerspW(4.0);
            testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            perspZ += 0.001;
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_11)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = { screenWidth / col, screenHeight / row };
    int nodeSpace[2] = { 10, 10 }; // col space , row space
    float perspW = 1.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1] });
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetPerspW(perspW);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            perspW += 0.3;
        }
    }
}
} // namespace OHOS::Rosen