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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ClipRadiusTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetClipRRect: normal values - matrix 3x3 (rect x radius) */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Normal_Matrix_3x3)
{
    std::vector<Vector4f> rects = {
        {0, 0, 200, 200},
        {50, 50, 150, 150},
        {100, 100, 100, 100}
    };
    std::vector<Vector4f> radii = {
        {0, 0, 0, 0},
        {20, 20, 20, 20},
        {50, 50, 50, 50}
    };

    for (size_t row = 0; row < rects.size(); row++) {
        for (size_t col = 0; col < radii.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 50),
                 static_cast<float>(row * 300 + 50),
                 250, 250});
            testNode->SetClipRRect(rects[row], radii[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetClipRRect: boundary values - zero radius */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Boundary_ZeroRadius)
{
    std::vector<Vector4f> zeroRadii = {
        {0, 0, 0, 0},
        {0, 0, 10, 10},
        {10, 10, 0, 0},
        {0, 10, 0, 10},
        {10, 0, 10, 0}
    };

    for (size_t i = 0; i < zeroRadii.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 50),
             static_cast<float>((i / 2) * 300 + 50),
             200, 200});
        testNode->SetClipRRect({0, 0, 150, 150},
                                        zeroRadii[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: boundary values - zero rect */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Boundary_ZeroRect)
{
    std::vector<Vector4f> zeroRects = {
        {0, 0, 0, 0},
        {50, 50, 0, 0},
        {0, 0, 0, 100},
        {0, 0, 100, 0}
    };

    for (size_t i = 0; i < zeroRects.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 50),
             static_cast<float>((i / 2) * 300 + 50),
             200, 200});
        testNode->SetClipRRect(zeroRects[i], {20, 20, 20, 20});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: normal - uniform corner radius */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Uniform_Radius)
{
    std::vector<float> uniformRadii = {
        0, 5, 10, 20, 30, 50, 100
    };

    for (size_t i = 0; i < uniformRadii.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 150 + 50), 50,
             150, 150});
        float r = uniformRadii[i];
        testNode->SetClipRRect({0, 0, 100, 100},
                                        {r, r, r, r});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: normal - asymmetric corner radius */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Asymmetric_Radius)
{
    std::vector<Vector4f> asymmetricRadii = {
        {0, 0, 10, 10},           // Top: 0, Bottom: 10
        {10, 10, 0, 0},           // Top: 10, Bottom: 0
        {0, 10, 0, 10},           // Left: 0, Right: 10
        {10, 0, 10, 0},           // Left: 10, Right: 0
        {0, 10, 20, 30},      // All different
        {50, 40, 30, 20}    // Descending
    };

    for (size_t i = 0; i < asymmetricRadii.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 50),
             static_cast<float>((i / 2) * 300 + 50),
             200, 200});
        testNode->SetClipRRect({0, 0, 150, 150},
                                        asymmetricRadii[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: extreme values - large radius */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Extreme_LargeRadius)
{
    std::vector<Vector4f> largeRadii = {
        {100, 100, 100, 100},
        {200, 200, 200, 200},
        {500, 500, 500, 500},
        {1000, 1000, 1000, 1000},
        {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
         std::numeric_limits<float>::max(), std::numeric_limits<float>::max()}
    };

    for (size_t i = 0; i < largeRadii.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 50),
             static_cast<float>((i / 2) * 300 + 50),
             200, 200});
        testNode->SetClipRRect({0, 0, 100, 100},
                                        largeRadii[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: extreme values - negative radius */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Extreme_NegativeRadius)
{
    std::vector<Vector4f> negativeRadii = {
        {-10, -10, -10, -10},
        {-10, 0, 0, 0},
        {0, -10, 0, 0},
        {0, 0, -10, 0},
        {0, 0, 0, -10},
        {-50, -20, -30, -40}
    };

    for (size_t i = 0; i < negativeRadii.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 50),
             static_cast<float>((i / 2) * 300 + 50),
             200, 200});
        testNode->SetClipRRect({0, 0, 150, 150},
                                        negativeRadii[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: invalid float - infinity and NaN */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Invalid_Float)
{
    std::vector<Vector4f> invalidRadii = {
        {std::numeric_limits<float>::infinity(), 0, 0, 0},
        {0, std::numeric_limits<float>::infinity(), 0, 0},
        {0, 0, std::numeric_limits<float>::infinity(), 0},
        {0, 0, 0, std::numeric_limits<float>::infinity()},
        {std::numeric_limits<float>::quiet_NaN(), 0, 0, 0},
        {0, std::numeric_limits<float>::quiet_NaN(), 0, 0},
        {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(),
         std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()}
    };

    for (size_t i = 0; i < invalidRadii.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 2) * 400 + 50);
        float y = static_cast<float>((i / 2) * 400 + 50);
        testNode->SetBounds({x, y, 200, 200});
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetClipRRect({0, 0, 100, 100},
                                        invalidRadii[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: rect offset variations */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Rect_Offset)
{
    std::vector<Vector4f> rectOffsets = {
        {-50, -50, 100, 100},   // Negative offset
        {0, 0, 100, 100},        // At origin
        {50, 50, 100, 100},      // Positive offset
        {100, 100, 100, 100},  // Large offset
        {-25, 25, 150, 150}  // Mixed offset
    };

    for (size_t i = 0; i < rectOffsets.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             200, 200});
        testNode->SetClipRRect(rectOffsets[i], {30, 30, 30, 30});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: rect size variations */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Rect_Size)
{
    std::vector<Vector4f> rectSizes = {
        {0, 0, 50, 50},
        {0, 0, 100, 100},
        {0, 0, 150, 150},
        {0, 0, 200, 200},
        {0, 0, 300, 300}
    };

    for (size_t i = 0; i < rectSizes.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 310 + 50), 50,
             250, 250});
        testNode->SetClipRRect(rectSizes[i], {25, 25, 25, 25});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: radius vs size relationship */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Radius_Size_Relationship)
{
    struct TestCase {
        Vector4f rect;
        Vector4f radius;
        const char* description;
    };

    std::vector<TestCase> testCases = {
        {{0, 0, 100, 100}, {10, 10, 10, 10}, "radius much smaller than size"},
        {{0, 0, 100, 100}, {50, 50, 50, 50}, "radius half of size"},
        {{0, 0, 100, 100}, {100, 100, 100, 100}, "radius equal to size (circle)"},
        {{0, 0, 100, 100}, {150, 150, 150, 150}, "radius larger than size"},
        {{0, 0, 100, 200}, {50, 50, 50, 50}, "radius matches half width"},
        {{0, 0, 200, 100}, {50, 50, 50, 50}, "radius matches half height"}
    };

    for (size_t i = 0; i < testCases.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             250, 250});
        testNode->SetClipRRect(testCases[i].rect, testCases[i].radius);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: rapid sequential updates */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Rapid_Updates)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 300, 300});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    testNode->SetClipRRect({0, 0, 200, 200}, {0, 0, 0, 0});
    testNode->SetClipRRect({0, 0, 200, 200}, {20, 20, 20, 20});
    testNode->SetClipRRect({0, 0, 200, 200}, {50, 50, 50, 50});
    testNode->SetClipRRect({50, 50, 150, 150}, {30, 30, 30, 30});
    testNode->SetClipRRect({0, 0, 100, 100}, {100, 100, 100, 100});
    testNode->SetClipRRect({0, 0, 200, 200}, {0, 0, 0, 0});
}

/* SetClipRRect: mixed corner radius - matrix 2x2 */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_MixedCorners_Matrix_2x2)
{
    std::vector<Vector4f> cornerRadii = {
        {0, 0, 10, 10},        // Top: 0, Bottom: 10
        {10, 10, 0, 0},        // Top: 10, Bottom: 0
        {0, 10, 0, 10},        // Left: 0, Right: 10
        {10, 0, 10, 0}         // Left: 10, Right: 0
    };

    for (size_t i = 0; i < cornerRadii.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             200, 200});
        testNode->SetClipRRect({0, 0, 150, 150},
                                        cornerRadii[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: fractional radius values */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Fractional_Radius)
{
    std::vector<Vector4f> fractionalRadii = {
        {0.5f, 0.5f, 0.5f, 0.5f},
        {1.5f, 1.5f, 1.5f, 1.5f},
        {10.5f, 10.5f, 10.5f, 10.5f},
        {20.5f, 30.5f, 40.5f, 50.5f}
    };

    for (size_t i = 0; i < fractionalRadii.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 50),
             static_cast<float>((i / 2) * 300 + 50),
             200, 200});
        testNode->SetClipRRect({0, 0, 150, 150},
                                        fractionalRadii[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: elliptical corners (different x and y) */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Elliptical_Corners)
{
    std::vector<Vector4f> ellipticalRadii = {
        {20, 10, 20, 10},           // Top-left: 20x10
        {10, 20, 10, 20},           // Top-right: 10x20
        {30, 15, 45, 25},  // All elliptical
        {50, 25, 75, 37.5f}  // Mixed elliptical
    };

    for (size_t i = 0; i < ellipticalRadii.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             200, 200});
        testNode->SetClipRRect({0, 0, 150, 150},
                                        ellipticalRadii[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: complex asymmetric pattern - matrix 3x3 */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Asymmetric_Matrix_3x3)
{
    std::vector<float> topLeft = {0, 10, 20};
    std::vector<float> bottomRight = {0, 10, 20};

    for (size_t row = 0; row < topLeft.size(); row++) {
        for (size_t col = 0; col < bottomRight.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 250 + 50),
                 static_cast<float>(row * 250 + 50),
                 200, 200});
            testNode->SetClipRRect({0, 0, 150, 150},
                                            {topLeft[row], topLeft[row],
                                             bottomRight[col], bottomRight[col]});
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetClipRRect: negative rect with positive radius */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Negative_Rect_Positive_Radius)
{
    std::vector<Vector4f> negativeRects = {
        {-50, -50, 100, 100},
        {-100, 0, 150, 150},
        {0, -100, 150, 150},
        {-25, -25, 200, 200}
    };

    for (size_t i = 0; i < negativeRects.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             200, 200});
        testNode->SetClipRRect(negativeRects[i], {30, 30, 30, 30});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetClipRRect: minimum and maximum epsilon */
GRAPHIC_TEST(ClipRadiusTest, CONTENT_DISPLAY_TEST, ClipRectWithRadius_Epsilon_Values)
{
    std::vector<Vector4f> epsilonRadii = {
        {std::numeric_limits<float>::min(), std::numeric_limits<float>::min(),
         std::numeric_limits<float>::min(), std::numeric_limits<float>::min()},
        {std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::epsilon(),
         std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::epsilon()},
        {1, 1, 1, 1},
        {100, 100, 100, 100}
    };

    for (size_t i = 0; i < epsilonRadii.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 50),
             static_cast<float>((i / 2) * 300 + 50),
             200, 200});
        testNode->SetClipRRect({0, 0, 100, 100},
                                        epsilonRadii[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
