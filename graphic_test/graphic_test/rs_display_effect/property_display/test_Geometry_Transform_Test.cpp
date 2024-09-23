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
    int screenWidth = 1260;
    int screenHeight = 2720;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
    }
};

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Transform_Test)
{
    int columnCount = 1;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector2f> transList = { { 1, 5 }, { 5, 2 } };

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

} // namespace OHOS::Rosen