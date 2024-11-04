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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_1)
{
    std::vector<std::vector<float> >  skewValues= {
        { 0.5, 0},
        { 0, 0.5},
        { 0.5, 0.5},
    };
    for (int i = 0; i < skewValues.size(); ++i) {
        auto testNode =
            SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 110, i * 420 + 110, 200, 200 });
        testNode->SetPivot(Vector2f(0.0, 0.0));
        testNode->SetSkew(skewValues[i][0], skewValues[i][1], 1.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Skew_Test_2)
{
    std::vector<std::vector<float> >  skewValues= {
        { 1.5, 0},
        { 0, 1.5},
        { 1.5, 1.5},
    };
    for (int i = 0; i < skewValues.size(); ++i) {
        auto testNode =
            SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 110, i * 420 + 110, 200, 200 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetSkew(skewValues[i][0], skewValues[i][1], 1.0);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen