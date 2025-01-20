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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_1)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<std::vector<float>> skewValues = { { -0.5, -0.1 }, { 0.1, 0.5 }, { 0.2, 0.3 }, { 1.5, 0.5 },
        { 0.5, 1.5 }, { 10, 20 }, { 0.5, 0.5 }, { 1.2, 1.2 } };
    for (int i = 0; i < skewValues.size(); ++i) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetPivot(Vector2f(0.0, 0.0));
        // try to cover all SetSkew funcitons
        if (i < 2) {
            testNode->SetSkew({ skewValues[i][0], skewValues[i][1] });
        } else if (i < 4) {
            testNode->SetSkew(skewValues[i][0], skewValues[i][1]);
        } else if (i < 6) {
            testNode->SetSkewX(skewValues[i][0]);
            testNode->SetSkewY(skewValues[i][1]);
        } else {
            testNode->SetSkew(skewValues[i][0]);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen