/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

/* SetRotationX: -90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationX_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationX(-90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationX: 0° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationX_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationX(-90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationX: 90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationX_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationX(-90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationX: -400° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationX_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationX(-400.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationX: 400° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationX_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationX(400.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationY: -90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationY_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationY(-90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationY: 0° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationY_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationY(0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationY: 90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationY_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationY(90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationY: -400° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationY_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationY(-400.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationY: 400° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationY_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationY(400.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationX And SetRotationY: -90°, -90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationX_And_RotationY_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationX(-90.0f);
    testNode->SetRotationY(-90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationX And SetRotationY: -90°, 90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationX_And_RotationY_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationX(-90.0f);
    testNode->SetRotationY(90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationX And SetRotationY: 90°, -90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationX_And_RotationY_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationX(90.0f);
    testNode->SetRotationY(-90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotationX And SetRotationY: 90°, 90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationX_And_RotationY_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 300, 300 });
    testNode->SetRotationX(90.0f);
    testNode->SetRotationY(90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotation1f */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation1f_Test_1)
{
    vector<float> ratation = {-405, -90, 0, 90, 405};
    for (int i = 0; i < ratation.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(ratation[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetRotation3f: -90°, 0°, 90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetRotation(-90.0f, 0.0f, 90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotation3f: 0°, 90°, -90° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetRotation(0.0f, 90.0f, -90.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotation3f: 90°, -90°, 0° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetRotation(90.0f, -90.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotation3f: 450°, 450°, 450° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetRotation(450.0f, 450.0f, 450.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotation3f: -450°, -450°, -450° */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 500, 500, 300, 300 });
    testNode->SetRotation(-450.0f, -450.0f, -450.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetRotation3f: SetPivotX、SetPivotY */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_6)
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

/* SetRotation3f: SetPivot({0, 0}) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_7)
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

/* SetRotation3f: SetPivot({0, 0}) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_8)
{
    float degreeList[][3] = {
        { 45, 0, 0 },
        { 0, 45, 0 },
        { 0, 0, 45 },
        { 45, 45, 45 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetPivot(Vector2f(0.0f, 0.0f));
        testNode->SetRotation(degreeList[i][2], degreeList[i][1], degreeList[i][0]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetRotation3f: SetPivot({1, 1}) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_9)
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

/* SetRotation3f: SetPivot({1, 1}) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Rotation3f_Test_10)
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
        testNode->SetRotation(degreeList[i][1], degreeList[i][2], degreeList[i][0]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetRotationQua: base */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationQua_Test_1)
{
    Quaternion degreeList[] = {
        { 0.383, 0, 0, 0.962 },
        { 0, 0.383, 0, 0.962 },
        { 0, 0, 0.383, 0.962 },
        { 0.462, 0.191, 0.462, 0.733 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetRotation(degreeList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetRotationQua: base */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationQua_Test_2)
{
    Quaternion degreeList[] = {
        { 0.383, 0, 0, 0.962 },
        { 0, 0.383, 0, 0.962 },
        { 0, 0, 0.383, 0.962 },
        { 0.462, 0.191, 0.462, 0.733 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetRotation(degreeList[4 - i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetRotationQua: SetPivotX、SetPivotY */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationQua_Test_3)
{
    Quaternion degreeList[] = {
        { 0.383, 0, 0, 0.962 },
        { 0, 0.383, 0, 0.962 },
        { 0, 0, 0.383, 0.962 },
        { 0.462, 0.191, 0.462, 0.733 },
    };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 280, i * 480 + 40, 400, 400 });
        testNode->SetPivot(0.0f, 0.0f);
        testNode->SetRotation(degreeList[4 - i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetRotationQua: SetPivotX、SetPivotY */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_RotationQua_Test_4)
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
} // namespace OHOS::Rosen