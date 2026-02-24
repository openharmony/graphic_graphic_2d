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

class BoundsSizeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetBoundsSize: normal values - matrix 3x3 (width x height) */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Normal_Matrix_3x3)
{
    std::vector<float> widths = {100, 200, 300};
    std::vector<float> heights = {100, 200, 300};

    for (size_t row = 0; row < heights.size(); row++) {
        for (size_t col = 0; col < widths.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            float x = static_cast<float>(col * 350 + 50);
            float y = static_cast<float>(row * 350 + 50);
            testNode->SetBounds({x, y, widths[col], heights[row]});
            testNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetBoundsSize: boundary values - zero size */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Boundary_Zero)
{
    std::vector<Vector2f> zeroSizes = {
        {0, 0},
        {0, 100},
        {100, 0}
    };

    for (size_t i = 0; i < zeroSizes.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {50, static_cast<float>(i * 300 + 50), 200, 200});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsSize: boundary values - minimum positive size */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Boundary_Minimum)
{
    std::vector<Vector2f> minSizes = {
        {1, 1},
        {std::numeric_limits<float>::min(), std::numeric_limits<float>::min()},
        {std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::epsilon()},
        {0.5f, 0.5f}
    };

    for (size_t i = 0; i < minSizes.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 2) * 600 + 50);
        float y = static_cast<float>((i / 2) * 200 + 50);
        testNode->SetBounds({x, y, 100, 100});
        testNode->SetBackgroundColor(0xff00ff00);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsSize: abnormal values - negative dimensions */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Abnormal_Negative)
{
    std::vector<Vector2f> negativeSizes = {
        {-100, 100},
        {100, -100},
        {-200, -200},
        {-500, 100},
        {100, -500}
    };

    for (size_t i = 0; i < negativeSizes.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 500 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             200, 200});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsSize: extreme values - large and max float */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Extreme_Large)
{
    std::vector<Vector2f> largeSizes = {
        {1000, 1000},
        {5000, 5000},
        {std::numeric_limits<float>::max(), 100},
        {100, std::numeric_limits<float>::max()},
        {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()}
    };

    for (size_t i = 0; i < largeSizes.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = 50;
        float y = static_cast<float>(i * 200 + 50);
        testNode->SetBounds({x, y, 100, 100});
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsSize: infinity and NaN */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Invalid_Float)
{
    std::vector<Vector2f> invalidSizes = {
        {std::numeric_limits<float>::infinity(), 100},
        {100, std::numeric_limits<float>::infinity()},
        {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()},
        {-std::numeric_limits<float>::infinity(), 100},
        {std::numeric_limits<float>::quiet_NaN(), 100},
        {100, std::numeric_limits<float>::quiet_NaN()},
        {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()}
    };

    for (size_t i = 0; i < invalidSizes.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 2) * 500 + 50);
        float y = static_cast<float>((i / 2) * 300 + 50);
        testNode->SetBounds({x, y, 100, 100});
        testNode->SetBackgroundColor(0xffff00ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsSize: aspect ratio variations - matrix 4x3 */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Aspect_Ratio_Matrix_4x3)
{
    std::vector<float> widths = {100, 200, 300, 400};
    std::vector<float> heights = {100, 150, 200};

    for (size_t row = 0; row < heights.size(); row++) {
        for (size_t col = 0; col < widths.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 50),
                 static_cast<float>(row * 250 + 50),
                 200, 200});
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetBoundsSize: rapid sequential updates */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Rapid_Updates)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 400, 400});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Rapid sequential size updates
    testNode->SetBounds({100, 100, 100, 100});
    testNode->SetBounds({100, 100, 200, 200});
    testNode->SetBounds({100, 100, 300, 300});
    testNode->SetBounds({100, 100, 400, 400});
}

/* SetBoundsSize: size before position */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Before_Position)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBackgroundColor(0xff00ffff);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetBoundsSize: combined with position - matrix 3x2 */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Position_Matrix_3x2)
{
    std::vector<Vector2f> sizes = {
        {100, 100},
        {150, 150},
        {200, 200}
    };
    std::vector<Vector2f> positions = {
        {0, 0},
        {50, 50}
    };

    for (size_t row = 0; row < sizes.size(); row++) {
        for (size_t col = 0; col < positions.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 50),
                 static_cast<float>(row * 250 + 50),
                 200, 200});
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetBoundsSize: interaction with SetBounds */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_SetBounds_Interaction)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 300, 300});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // SetBounds followed by SetBoundsSize
    testNode->SetBounds({50, 50, 200, 200});

    // SetBoundsSize followed by SetBounds
    testNode->SetBounds({100, 100, 100, 100});
}

/* SetBoundsSize: square vs rectangular sizes */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Shape_Variation)
{
    std::vector<Vector2f> shapes = {
        {100, 100},        // Square
        {100, 200},        // Portrait
        {200, 100},        // Landscape
        {100, 150},  // Slightly portrait
        {150, 100}   // Slightly landscape
    };

    for (size_t i = 0; i < shapes.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 3) * 300 + 50);
        float y = static_cast<float>((i / 3) * 300 + 50);
        testNode->SetBounds({x, y, 200, 200});
        testNode->SetBackgroundColor(0xffffff00);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsSize: hierarchical sizes */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Hierarchical)
{
    // Parent node
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({50, 50, 500, 500});
    parentNode->SetBackgroundColor(0xffcccccc);
    GetRootNode()->AddChild(parentNode);
    RegisterNode(parentNode);

    // Child nodes with different sizes
    std::vector<Vector2f> childSizes = {
        {100, 100},
        {200, 200},
        {300, 300},
        {400, 400}
    };

    for (size_t i = 0; i < childSizes.size(); i++) {
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({static_cast<float>(i * 110 + 10),
                             static_cast<float>(i * 110 + 10),
                             childSizes[i].x_, childSizes[i].y_});
        childNode->SetBackgroundColor(0xffff0000);
        parentNode->AddChild(childNode);
        RegisterNode(childNode);
    }
}

/* SetBoundsSize: overflow scenarios */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Overflow)
{
    std::vector<Vector2f> overflowSizes = {
        {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()},
        {1000, 1000},
        {5000, 100},
        {100, 5000}
    };

    for (size_t i = 0; i < overflowSizes.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 100 + 50),
             static_cast<float>((i / 2) * 100 + 50),
             100, 100});
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsSize: fractional sizes */
GRAPHIC_TEST(BoundsSizeTest, CONTENT_DISPLAY_TEST, BoundsSize_Fractional)
{
    std::vector<Vector2f> fractionalSizes = {
        {0.5f, 0.5f},
        {1.5f, 1.5f},
        {10.5f, 10.5f},
        {100.5f, 100.5f},
        {200.75f, 200.75f}
    };

    for (size_t i = 0; i < fractionalSizes.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 3) * 300 + 50);
        float y = static_cast<float>((i / 3) * 200 + 50);
        testNode->SetBounds({x, y, 200, 200});
        testNode->SetBackgroundColor(0xffffcc00);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
