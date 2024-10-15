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
    int screenWidth = 1260;
    int screenHeight = 2720;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
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
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 680 + 40, 600, 600 });
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
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 680 + 40, 600, 600 });
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
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 680 + 40, 600, 600 });
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
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 680 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(0, 0));
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
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 680 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
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
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 680 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(1, 1));
        testNode->SetRotation(degreeList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, PivotZ_Rotation_Test_1)
{
    float zList[] = { 0.0, 0.5, 1.0, 100.0 };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 680 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetPivotZ(zList[i]);
        testNode->SetRotation(0, 45.0, 0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen