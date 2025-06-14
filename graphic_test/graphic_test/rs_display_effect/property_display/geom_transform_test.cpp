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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class TestGeomTransModifier : public RSGeometryTransModifier {
public:
    TestGeomTransModifier() = default;
    ~TestGeomTransModifier() = default;

    Drawing::Matrix GeometryEffect(float width, float height) const override
    {
        Drawing::Matrix matrix;
        matrix.PreTranslate(width, height);
        return matrix;
    }
};

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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test)
{
    int columnCount = 1;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector2f> transList = { { 0.1, 0.5 }, { 0.5, 0.2 } };

    for (int i = 0; i < 2; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        auto geomTransModifier = std::make_shared<TestGeomTransModifier>();
        geomTransModifier->GeometryEffect(transList[i].x_, transList[i].y_);
        testNodeBackGround->AddModifier(geomTransModifier);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test_2)
{
    const int dataCounts = 3;
    std::array<float, dataCounts> scaleData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> skewData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> perspData = { 1.0f, 2.0f, 0.5f };
    for (int i = 0; i < dataCounts; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetSkew(skewData[i], 0.5, 0.0);
        testNode->SetPersp(0.0, 0.0, perspData[i], 1.0);
        testNode->SetTranslate(0.5, 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test_3)
{
    const int dataCounts = 3;
    std::array<float, dataCounts> scaleData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> skewData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> perspData = { 1.0f, 2.0f, 0.5f };
    for (int i = 0; i < dataCounts; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(0.0, 0, 45.0);
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetSkew(skewData[i], 0.5, 0.0);
        testNode->SetPersp(0.0, 0.0, perspData[i], 1.0);
        testNode->SetTranslate(0.5, 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test_4)
{
    const int dataCounts = 3;
    std::array<float, dataCounts> scaleData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> skewData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> perspData = { 1.0f, 2.0f, 0.5f };
    for (int i = 0; i < dataCounts; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 0.0);
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetSkew(skewData[i], 0.5, 0.0);
        testNode->SetPersp(0.0, 0.0, perspData[i], 1.0);
        testNode->SetTranslate(0.5, 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test_5)
{
    const int dataCounts = 3;
    std::array<float, dataCounts> rorationData = { 45.0f, 45.0f, 45.0f };
    std::array<float, dataCounts> scaleData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> skewData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> perspData = { 1.0f, 2.0f, 0.5f };
    for (int i = 0; i < dataCounts; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(rorationData[i], 0, 0.0);
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetSkew(skewData[i], 0.5, 0.0);
        testNode->SetPersp(0.0, 0.0, perspData[i], 1.0);
        testNode->SetTranslate(0.5, 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test_6)
{
    const int dataCounts = 3;
    std::array<float, dataCounts> rorationData = { 45.0f, 45.0f, 45.0f };
    std::array<float, dataCounts> scaleData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> skewData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> perspData = { 1.0f, 2.0f, 0.5f };
    std::array<float, dataCounts> translateData = { -5.0f, 2.0f, 5.0f };
    for (int i = 0; i < dataCounts; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(rorationData[i], 0, 0.0);
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetSkew(skewData[i], 0.5, 0.0);
        testNode->SetPersp(0.0, 0.0, perspData[i], 1.0);
        testNode->SetTranslate(translateData[i], 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test_7)
{
    const int dataCounts = 3;
    std::array<float, dataCounts> pivotData = { 0.0f, 0.5f, 1.0f};
    std::array<float, dataCounts> rorationData = { 45.0f, 45.0f, 45.0f };
    std::array<float, dataCounts> scaleData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> skewData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> perspData = { 1.0f, 2.0f, 0.5f };
    std::array<float, dataCounts> translateData = { -5.0f, 2.0f, 5.0f };
    for (int i = 0; i < dataCounts; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(pivotData[i], 0.5));
        testNode->SetRotation(rorationData[i], 0, 0.0);
        testNode->SetScale({ scaleData[i], 0.5 });
        testNode->SetSkew(skewData[i], 0.5, 0.0);
        testNode->SetPersp(0.0, 0.0, perspData[i], 1.0);
        testNode->SetTranslate(translateData[i], 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test_8)
{
    const int dataCounts = 3;
    std::array<float, dataCounts> rorationData = { 45.0f, 45.0f, 45.0f };
    std::array<float, dataCounts> scaleData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> skewData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> perspData = { 1.0f, 2.0f, 0.5f };
    for (int i = 0; i < dataCounts; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(rorationData[i], rorationData[i], 0.0);
        testNode->SetScale({ 0.5, scaleData[i] });
        testNode->SetSkew(0.5, skewData[i], 0.0);
        testNode->SetPersp(0.0, perspData[i], 0.0, 1.0);
        testNode->SetTranslate(0.5, 0.5, 0.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test_9)
{
    const int dataCounts = 3;
    std::array<float, dataCounts> rorationData = { 45.0f, 45.0f, 45.0f };
    std::array<float, dataCounts> scaleData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> skewData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> perspData = { 1.0f, 2.0f, 0.5f };
    std::array<float, dataCounts> translateData = { -5.0f, 2.0f, 5.0f };
    for (int i = 0; i < dataCounts; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(rorationData[i], rorationData[i], rorationData[i]);
        testNode->SetScale({ scaleData[i], scaleData[i] });
        testNode->SetSkew(skewData[i], skewData[i], skewData[i]);
        testNode->SetPersp(perspData[i], perspData[i], perspData[i], 1.0);
        testNode->SetTranslate(translateData[i], translateData[i], translateData[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test_10)
{
    const int dataCounts = 3;
    std::array<float, dataCounts> pivotData = { 0.0f, 0.5f, 1.0f};
    std::array<float, dataCounts> rorationData = { 45.0f, 45.0f, 45.0f };
    std::array<float, dataCounts> scaleData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> skewData = { 0.f, 0.5f, -0.5f};
    std::array<float, dataCounts> perspData = { 1.0f, 2.0f, 0.5f };
    std::array<float, dataCounts> translateData = { -5.0f, 2.0f, 5.0f };
    for (int i = 0; i < dataCounts; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {380, i * 350 + 20, 300, 300});
        testNode->SetPivot(Vector2f(pivotData[i], pivotData[i]));
        testNode->SetRotation(rorationData[i], rorationData[i], rorationData[i]);
        testNode->SetScale({ scaleData[i], scaleData[i] });
        testNode->SetSkew(skewData[i], skewData[i], skewData[i]);
        testNode->SetPersp(perspData[i], perspData[i], perspData[i], 1.0);
        testNode->SetTranslate(translateData[i], translateData[i], translateData[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen