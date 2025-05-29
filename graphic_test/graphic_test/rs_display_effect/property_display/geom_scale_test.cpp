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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_1)
{
    float zList[] = { 1, 0.5, 2 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            auto testNode =
                SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { j * 420 + 110, i * 420 + 110, 200, 200 });
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetScaleX(zList[i]);
            testNode->SetScaleY(zList[j]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_2)
{
    float zList[] = { 1, 0.5, 2 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            auto testNode =
                SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { j * 420 + 110, i * 420 + 110, 200, 200 });
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetScale(zList[i], zList[j]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_3)
{
    float zList[] = { 1, 0.5, 2 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            auto testNode =
                SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { j * 420 + 110, i * 420 + 110, 200, 200 });
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetScale({ zList[i], zList[j] });
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_4)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f};
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetScale({ scaleData[i], 0.5 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_5)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(0.0, 0, 45.0);
        testNode->SetScale({ scaleData[i], 0.5 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_6)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 0.0);
        testNode->SetScale({ scaleData[i], 0.5 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_7)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetScaleZ(0.5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_8)
{
    std::array<float, 3> scaleData = { 0.f, 0.5f, -0.5f };
    for (int i = 0; i < scaleData.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetScaleZ(0.5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_9)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float scale = 1.0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetScaleX(scale);
            testNode->SetScaleY(scale);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            scale -= 0.06;
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_10)
{
    int row = 5;
    int col = 3;
    int nodeSize[2] = {screenWidth / col, screenHeight / row};
    int nodeSpace[2] = {10, 10}; // col space , row space
    float scale = 0.99;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {j * nodeSize[0], i * nodeSize[1], nodeSize[0] - nodeSpace[0], nodeSize[1] - nodeSpace[1]});
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetScaleZ(scale);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
            scale -= 0.06;
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ScaleZ_Test_1)
{
    vector<float> vecs = {-0.5, 0.0, 0.5, 1, 100.0};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetScaleZ(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen