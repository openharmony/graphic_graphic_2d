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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslationXY_Test_1)
{
    float transList[] = { 0, 250, 500 };
    for (int i = 0; i < 3; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, 500, 500 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetTranslate(Vector2f(transList[i], transList[i]));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_TranslationZ_Test_2)
{
    float transList[] = { 0, 250, 500 };
    for (int i = 0; i < 3; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { x, y, 500, 500 });
        testNode->SetPivot(Vector2f(0, 0));
        testNode->SetTranslate(Vector2f(transList[i], transList[i]));
        testNode->SetTranslateZ(transList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen