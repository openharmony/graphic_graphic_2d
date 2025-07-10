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

/* SetPerspX: > 0, {-1.0f, 0.0f, 0.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PerspX_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 200, 200 });
    testNode->SetPerspX(-1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPerspX: < 0, {1.0f, 0.0f, 0.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PerspX_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 200, 200, 400, 400 });
    testNode->SetPerspX(1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPerspY: > 0, {0.0f, -1.0f, 0.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PerspY_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 600, 600 });
    testNode->SetPerspY(-1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPerspY: < 0, {0.0f, 1.0f, 0.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PerspY_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 800, 800 });
    testNode->SetPerspY(1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPerspZ: > 0, {0.0f, 0.0f, -1.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PerspZ_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 800, 800, 400, 400 });
    testNode->SetPerspZ(-1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPerspZ: < 0, {0.0f, 0.0f, 1.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PerspZ_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 600, 600, 300, 300 });
    testNode->SetPerspZ(1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPerspW: = 0, {0.0f, 0.0f, 0.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PerspW_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPerspW(0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPerspW: < 0, {0.0f, 0.0f, 0.0f, -1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_PerspW_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 200, 200, 100, 100 });
    testNode->SetPerspW(-1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp1f: > 0， {-1.0f, -1.0f, 0.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp1f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp1f: < 0， {1.0f, 1.0f, 0.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp1f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 200, 200, 100, 100 });
    testNode->SetPersp(1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp2f: > 0  < 0， {-1.0f, 1.0f, 0.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp2f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f, 1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp2f: < 0 > 0， {1.0f, -1.0f, 0.0f, 1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp2f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 200, 200, 100, 100 });
    testNode->SetPersp(1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{-1.0f, -1.0f, -1.0f, -1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f, -1.0f, -1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{-1.0f, -1.0f, -1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f, -1.0f, -1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{-1.0f, -1.0f, 1.0f, -1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f, -1.0f, 1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{-1.0f, -1.0f, 1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f, -1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{-1.0f, 1.0f, 1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f, 1.0f, -1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{-1.0f, 1.0f, -1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_6)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f, 1.0f, -1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{-1.0f, 1.0f, 1.0f, -1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_7)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f, 1.0f, 1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{-1.0f, 1.0f, 1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_8)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(-1.0f, 1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{1.0f, -1.0f, -1.0f, -1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_9)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(1.0f, -1.0f, -1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{1.0f, -1.0f, -1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_10)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(1.0f, -1.0f, -1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{1.0f, -1.0f, 1.0f, -1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_11)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(1.0f, -1.0f, 1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{1.0f, -1.0f, 1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_12)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(1.0f, -1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{-1.0f, 1.0f, 1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_13)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(1.0f, 1.0f, -1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{1.0f, 1.0f, -1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_14)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(1.0f, 1.0f, -1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{1.0f, 1.0f, 1.0f, -1.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_15)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(1.0f, 1.0f, 1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp4f == SetPerspV4f：{1.0f, 1.0f, 1.0f, 0.0f} */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp4f_Test_16)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPersp(1.0f, 1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetPivot(0, 0) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 350, 650, 300, 400 });
    testNode->SetPivot(Vector2f(0.0f, 0.0f));
    testNode->SetPersp(1.0f, 1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetPivot(1, 1) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 350, 650, 300, 400 });
    testNode->SetPivot(Vector2f(1.0f, 1.0f));
    testNode->SetPersp(1.0f, 1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetRotation(-45, -45, -45) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 350, 650, 300, 400 });
    testNode->SetRotation(-45.0f, -45.0f, -45.0f);
    testNode->SetPersp(1.0f, 1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetRotation(45, 45, 45) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 350, 650, 300, 400 });
    testNode->SetRotation(45.0f, 45.0f, 45.0f);
    testNode->SetPersp(1.0f, 1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetRotation(-45, 0, 45) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 350, 650, 300, 400 });
    testNode->SetRotation(-45.0f, 0.0f, 45.0f);
    testNode->SetPersp(1.0f, 1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetRotation(0, -45, 45) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_6)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 350, 650, 300, 400 });
    testNode->SetRotation(0.0f, -45.0f, 45.0f);
    testNode->SetPersp(1.0f, 1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetRotation(45, -45, 0) */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_7)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 350, 650, 300, 400 });
    testNode->SetRotation(45.0f, -45.0f, 0.0f);
    testNode->SetPersp(1.0f, 1.0f, 1.0f, 0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetPivot、SetRotation case1 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_8)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 300, 300, 600, 600 });
    testNode->SetPivot(Vector2f(1.0f, 1.0f));
    testNode->SetRotation(45.0, 0.0f, 45.0);
    testNode->SetPersp(1.0f, 1.0f, 1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetPivot、SetRotation case2 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_9)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPivot(Vector2f(0.0f, 0.0f));
    testNode->SetRotation(0.0f, 0.0f, 45.0f);
    testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
    testNode->SetPersp(1.0f, -1.0f, -1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetPersp：compositon SetPivot、SetRotation case3 */
GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Comp_Test_10)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 400, 400, 200, 200 });
    testNode->SetPivot(Vector2f(1.0f, 1.0f));
    testNode->SetRotation(0.0, 0, 45.0);
    testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
    testNode->SetPersp(1.0f, -1.0f, -1.0f, -1.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

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