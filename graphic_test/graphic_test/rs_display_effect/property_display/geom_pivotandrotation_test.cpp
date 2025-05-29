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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Pivot_0_Rotation_float_Test_1)
{
    float degreeList[][3] = {
        { 45, 0, 0 },
        { 0, 45, 0 },
        { 0, 0, 45 },
        { 45, 45, 45 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetRotation(degreeList[i][0], degreeList[i][1], degreeList[i][2]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Pivot_05_Rotation_float_Test_1)
{
    float degreeList[][3] = {
        { 45, 0, 0 },
        { 0, 45, 0 },
        { 0, 0, 45 },
        { 45, 45, 45 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(degreeList[i][0], degreeList[i][1], degreeList[i][2]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Pivot_1_Rotation_float_Test_1)
{
    float degreeList[][3] = {
        { 45, 0, 0 },
        { 0, 45, 0 },
        { 0, 0, 45 },
        { 45, 45, 45 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetPivot(Vector2f(1, 1));
        testNode->SetRotation(degreeList[i][0], degreeList[i][1], degreeList[i][2]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Pivot_0_Rotation_quaternion_Test_1)
{
    Quaternion degreeList[] = {
        { 0.383, 0, 0, 0.962 },
        { 0, 0.383, 0, 0.962 },
        { 0, 0, 0.383, 0.962 },
        { 0.462, 0.191, 0.462, 0.733 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetPivotX(0);
        testNode->SetPivotY(0);
        testNode->SetRotation(degreeList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Pivot_05_Rotation_quaternion_Test_1)
{
    Quaternion degreeList[] = {
        { 0.383, 0, 0, 0.962 },
        { 0, 0.383, 0, 0.962 },
        { 0, 0, 0.383, 0.962 },
        { 0.462, 0.191, 0.462, 0.733 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetPivotX(0.5);
        testNode->SetPivotY(0.5);
        testNode->SetRotation(degreeList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Pivot_1_Rotation_quaternion_Test_1)
{
    Quaternion degreeList[] = {
        { 0.383, 0, 0, 0.962 },
        { 0, 0.383, 0, 0.962 },
        { 0, 0, 0.383, 0.962 },
        { 0.462, 0.191, 0.462, 0.733 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetPivotX(1);
        testNode->SetPivotY(1);
        testNode->SetRotation(degreeList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, PivotZ_Rotation_Test_1)
{
    float zList[] = { 0.0, 0.5, 1.0, 100.0 };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetPivotX(0.5);
        testNode->SetPivotY(0.5);
        testNode->SetPivotZ(zList[i]);
        testNode->SetRotation(0, 45.0, 0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, PivotZ_Rotation_Test_2)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float rotationZ = 0.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetRotation(0.0, 0.0, rotationZ);
            testNode->SetPerspW(2.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            rotationZ += 5.0;
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, PivotZ_Rotation_Test_3)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float rotation = 0.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetRotation(rotation, rotation, 0.0);
            testNode->SetPerspW(2.0);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            rotation += 5.0;
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PositionZ_Test_01)
{
    vector<float> vecs = {-50, -49.8, -49.4, 0, 5, 9.8, 10, 10.2, 50};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {i * 45 + 45, i * 45 + 45, 450, 450});
        testNode->SetPositionZ(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Pivot_Test_1)
{
    vector<Vector2f> vecs = {{-1.25, 0}, {-0.75, 0}, {0, 0}, {0.25, 0}, {1, 0}, {1.25, 0}};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(vecs[i]);
        testNode->SetRotation(30);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Pivot_Test_2)
{
    vector<Vector2f> vecs = {{0, -1.25}, {0, -0.75}, {0, 0}, {0, 0.25}, {0, 1}, {0, 1.25}};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(vecs[i]);
        testNode->SetRotation(30);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Pivot_Test_3)
{
    vector<Vector2f> vecs = {{0.5, -1.25}, {-1.25, 0.75}, {0.5, 0.5}, {0.5, 0.25}, {-0.5, -0.5}, {1.25, 1.25}};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(vecs[i]);
        testNode->SetRotation(30);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Pivot_Test_4)
{
    vector<Vector2f> vecs = {{0.5, -1.25}, {-1.25, 0.75}, {0.5, 0.5}, {0.5, 0.25}, {-0.5, -0.5}, {1.25, 1.25}};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(vecs[i]);
        testNode->SetRotationY(30);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Pivot_Test_5)
{
    vector<Vector2f> vecs = {{0.5, -1.25}, {-1.25, 0.75}, {0.5, 0.5}, {0.5, 0.25}, {-0.5, -0.5}, {1.25, 1.25}};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(vecs[i]);
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PositionZApplicableCamera3D_Test_1)
{
    vector<bool> vecs = {true, false};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotationY(30);
        testNode->SetPositionZ(100);
        testNode->SetPositionZApplicableCamera3D(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PivotZ_Test_1)
{
    vector<float> vecs = {-100, -1, -0.1, 0, 0.1, 1, 100};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivotX(0.5);
        testNode->SetPivotY(0.5);
        testNode->SetPivotZ(vecs[i]);
        testNode->SetRotation(0, 45.0, 0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation_Test_1)
{
    vector<float> vecs = {-405, -90, 0, 30, 90, 180, 405};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation_Test_2)
{
    vector<float> vecs = {-405, -90, 0, 30, 90, 180, 405};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotationX(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation_Test_3)
{
    vector<float> vecs = {-405, -90, 0, 30, 90, 180, 405};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotationY(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation_Test_4)
{
    vector<vector<float>> vecs = {{-45, -45, -45}, {-30, 30, -30}, {10, 20, 30}, {30, 30, 0},
        {30, 0, 30}, {0, 30, 30}, {90, 90, 90}, {30, 150, 45}, {405, 405, 405}, };
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(vecs[i][0], vecs[i][1], vecs[i][2]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen