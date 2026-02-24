/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "ng_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
}  // namespace

class NGFilterBezierWarpTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

// Test BezierWarp with all control points
GRAPHIC_TEST(NGFilterBezierWarpTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_ControlPoints_Test)
{
    auto bezierWarpPtr = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(bezierWarpPtr);

    // Set all ControlPoint tags (ControlPoint0-11)
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f(0.0f, 0.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f(0.15f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f(0.35f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f(0.5f, 0.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f(0.15f, 0.33f));
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f(0.35f, 0.33f));
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f(0.5f, 0.33f));
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f(0.0f, 0.66f));
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f(0.15f, 0.66f));
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f(0.35f, 0.66f));
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f(0.5f, 0.66f));
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f(1.0f, 1.0f));

    int nodeWidth = 980;
    int nodeHeight = 1600;
    int start = 110;

    auto backgroundNode = SetUpNodeBgImage(BG_PATH,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

GRAPHIC_TEST(NGFilterBezierWarpTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_ControlPoints_Variant1_Test)
{
    auto bezierWarpPtr = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(bezierWarpPtr);

    // Test variant 1: different control point positions
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f(0.1f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f(0.2f, 0.15f));
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f(0.4f, 0.15f));
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f(0.5f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f(0.1f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f(0.3f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f(0.45f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f(0.5f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f(0.6f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f(0.75f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f(0.85f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f(0.9f, 0.9f));

    int nodeWidth = 980;
    int nodeHeight = 1600;
    int start = 110;

    auto backgroundNode = SetUpNodeBgImage(BG_PATH,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

GRAPHIC_TEST(NGFilterBezierWarpTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_ControlPoints_Variant2_Test)
{
    auto bezierWarpPtr = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(bezierWarpPtr);

    // Test variant 2: asymmetric warp
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f(0.0f, 0.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f(0.25f, 0.05f));
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f(0.45f, 0.05f));
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f(0.6f, 0.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f(0.05f, 0.35f));
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f(0.25f, 0.3f));
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f(0.4f, 0.35f));
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f(0.55f, 0.35f));
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f(0.7f, 0.3f));
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f(0.8f, 0.35f));
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f(0.9f, 0.35f));
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f(0.95f, 0.95f));

    int nodeWidth = 980;
    int nodeHeight = 1600;
    int start = 110;

    auto backgroundNode = SetUpNodeBgImage(BG_PATH,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

GRAPHIC_TEST(NGFilterBezierWarpTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_Multi_Test)
{
    // Test multiple BezierWarp configurations in one test
    int nodeWidth = 430;
    int nodeHeight = 700;
    int start = 120;
    int gap = 40;
    int row = 2;
    int col = 2;

    for (int j = 0; j < row; j++) {
        for (int i = 0; i < col; i++) {
            auto bezierWarpPtr = CreateFilter(RSNGEffectType::BEZIER_WARP);
            auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(bezierWarpPtr);

            // Different configuration for each instance
            float offset = (i + j * col) * 0.1f;
            bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f(0.0f + offset, 0.0f + offset));
            bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f(0.15f + offset, 0.1f));
            bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f(0.35f + offset, 0.1f));
            bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f(0.5f, 0.0f + offset));
            bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f(0.15f, 0.33f + offset));
            bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f(0.35f, 0.33f));
            bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f(0.5f - offset, 0.33f + offset));
            bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f(0.0f, 0.66f));
            bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f(0.15f + offset, 0.66f));
            bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f(0.35f + offset, 0.66f));
            bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f(0.5f, 0.66f - offset));
            bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f(1.0f - offset, 1.0f - offset));

            auto backgroundNode = SetUpNodeBgImage(BG_PATH,
                {start + (nodeWidth + gap) * i, start + (nodeHeight + gap) * j, nodeWidth, nodeHeight});
            backgroundNode->SetBackgroundNGFilter(bezierWarpFilter);
            GetRootNode()->AddChild(backgroundNode);
            RegisterNode(backgroundNode);
        }
    }
}

GRAPHIC_TEST(NGFilterBezierWarpTest, EFFECT_TEST, Set_NG_Filter_Bezier_Warp_Extreme_Test)
{
    auto bezierWarpPtr = CreateFilter(RSNGEffectType::BEZIER_WARP);
    auto bezierWarpFilter = std::static_pointer_cast<RSNGBezierWarpFilter>(bezierWarpPtr);

    // Test extreme warp values
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f(0.0f, 0.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f(0.5f, 0.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f(1.0f, 0.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f(0.0f, 0.5f));
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f(0.5f, 0.5f));
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f(1.0f, 0.5f));
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f(0.0f, 1.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f(0.5f, 1.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f(1.0f, 1.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f(0.0f, 0.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f(0.5f, 0.0f));
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f(1.0f, 1.0f));

    int nodeWidth = 980;
    int nodeHeight = 1600;
    int start = 110;

    auto backgroundNode = SetUpNodeBgImage(BG_PATH,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetBackgroundNGFilter(bezierWarpFilter);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen
