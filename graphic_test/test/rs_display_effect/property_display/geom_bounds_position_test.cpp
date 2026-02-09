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

class BoundsPositionTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetBoundsPosition: normal values - matrix 4x3 (x x y) */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Normal_Matrix_4x3)
{
    std::vector<float> positionsX = {0, 50, 100, 150};
    std::vector<float> positionsY = {0, 50, 100};

    for (size_t row = 0; row < positionsY.size(); row++) {
        for (size_t col = 0; col < positionsX.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {50, 50, 200, 200});
            testNode->SetBoundsPosition({positionsX[col], positionsY[row]});
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetBoundsPosition: boundary values - zero and negative */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Boundary_Zero_Negative)
{
    std::vector<Vector2f> positions = {
        {0, 0},
        {-100, 0},
        {0, -100},
        {-100, -100},
        {-200, -200},
        {-500, -500}
    };

    for (size_t i = 0; i < positions.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 200),
             static_cast<float>((i / 2) * 300 + 200),
             200, 200});
        testNode->SetBoundsPosition(positions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPosition: extreme values */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Extreme_Values)
{
    std::vector<Vector2f> extremePositions = {
        {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()},
        {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()},
        {1000, 1000},
        {-1000, -1000},
        {5000, 5000},
        {-5000, -5000},
        {std::numeric_limits<float>::min(), std::numeric_limits<float>::min()}
    };

    for (size_t i = 0; i < extremePositions.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 100 + 50),
             static_cast<float>((i / 2) * 100 + 50),
             100, 100});
        testNode->SetBoundsPosition(extremePositions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPosition: infinity and NaN */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Invalid_Float)
{
    std::vector<Vector2f> invalidPositions = {
        {std::numeric_limits<float>::infinity(), 0},
        {0, std::numeric_limits<float>::infinity()},
        {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()},
        {-std::numeric_limits<float>::infinity(), 0},
        {0, -std::numeric_limits<float>::infinity()},
        {-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()},
        {std::numeric_limits<float>::quiet_NaN(), 0},
        {0, std::numeric_limits<float>::quiet_NaN()},
        {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()}
    };

    for (size_t i = 0; i < invalidPositions.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 3) * 300 + 50);
        float y = static_cast<float>((i / 3) * 300 + 50);
        testNode->SetBounds({x, y, 100, 100});
        testNode->SetBoundsPosition(invalidPositions[i]);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPositionX: normal values - 1x5 matrix */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPositionX_Normal_Matrix_1x5)
{
    std::vector<float> positions = {
        -100, -50, 0, 50, 100
    };

    for (size_t i = 0; i < positions.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>(i * 200 + 50);
        float y = 100;
        testNode->SetBounds({x, y, 150, 150});
        testNode->SetBoundsPositionX(positions[i]);
        testNode->SetBackgroundColor(0xff00ff00);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPositionX: extreme values */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPositionX_Extreme_Values)
{
    std::vector<float> extremePositions = {
        0,
        1,
        -1,
        1000,
        -1000,
        5000,
        -5000,
        std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max(),
        std::numeric_limits<float>::min(),
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::quiet_NaN()
    };

    for (size_t i = 0; i < extremePositions.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 200 + 100),
             static_cast<float>((i / 2) * 200 + 100),
             100, 100});
        testNode->SetBoundsPositionX(extremePositions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPositionY: normal values - 5x1 matrix */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPositionY_Normal_Matrix_5x1)
{
    std::vector<float> positions = {
        -100, -50, 0, 50, 100
    };

    for (size_t i = 0; i < positions.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = 100;
        float y = static_cast<float>(i * 200 + 50);
        testNode->SetBounds({x, y, 150, 150});
        testNode->SetBoundsPositionY(positions[i]);
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPositionY: extreme values */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPositionY_Extreme_Values)
{
    std::vector<float> extremePositions = {
        0,
        1,
        -1,
        1000,
        -1000,
        5000,
        -5000,
        std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max(),
        std::numeric_limits<float>::min(),
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::quiet_NaN()
    };

    for (size_t i = 0; i < extremePositions.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 200 + 100),
             static_cast<float>((i / 2) * 200 + 100),
             100, 100});
        testNode->SetBoundsPositionY(extremePositions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPositionX/Y: combined test matrix */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPositionXY_Combined_Matrix_3x3)
{
    std::vector<float> positionsX = {-100, 0, 100};
    std::vector<float> positionsY = {-100, 0, 100};

    for (size_t row = 0; row < positionsY.size(); row++) {
        for (size_t col = 0; col < positionsX.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 100),
                 static_cast<float>(row * 300 + 100),
                 200, 200});
            testNode->SetBoundsPositionX(positionsX[col]);
            testNode->SetBoundsPositionY(positionsY[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetBoundsPosition: position vs bounds size relationship */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Size_Relationship)
{
    // Position at origin
    auto testNode1 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 200, 200});
    testNode1->SetBoundsPosition({0, 0});
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Positive offset
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {400, 100, 200, 200});
    testNode2->SetBoundsPosition({50, 50});
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);

    // Large positive offset
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 400, 200, 200});
    testNode3->SetBoundsPosition({200, 200});
    GetRootNode()->AddChild(testNode3);
    RegisterNode(testNode3);

    // Negative offset
    auto testNode4 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {400, 400, 200, 200});
    testNode4->SetBoundsPosition({-100, -100});
    GetRootNode()->AddChild(testNode4);
    RegisterNode(testNode4);
}

/* SetBoundsPosition: rapid sequential updates */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Rapid_Updates)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 300, 300});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    testNode->SetBoundsPosition({0, 0});
    testNode->SetBoundsPosition({50, 50});
    testNode->SetBoundsPosition({100, 100});
    testNode->SetBoundsPosition({-50, -50});
    testNode->SetBoundsPosition({0, 50});
    testNode->SetBoundsPosition({50, 0});
    testNode->SetBoundsPosition({150, 150});
    testNode->SetBoundsPosition({-100, -100});
}

/* SetBoundsPosition/PositionX/PositionY: interaction test */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Interaction)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 300, 300});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // SetBoundsPosition followed by SetBoundsPositionX
    testNode->SetBoundsPosition({100, 100});
    testNode->SetBoundsPositionX(200);

    // SetBoundsPositionY followed by SetBoundsPosition
    testNode->SetBoundsPositionY(200);
    testNode->SetBoundsPosition({50, 50});

    // SetBoundsPositionX followed by SetBoundsPositionY
    testNode->SetBoundsPositionX(150);
    testNode->SetBoundsPositionY(150);

    // Verify final state
    testNode->SetBackgroundColor(0xffffff00);
}

/* SetBoundsPosition: combined with size - matrix 3x2 */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Size_Matrix_3x2)
{
    std::vector<Vector2f> positions = {
        {0, 0},
        {50, 50},
        {100, 100}
    };
    std::vector<Vector2f> sizes = {
        {100, 100},
        {200, 200}
    };

    for (size_t row = 0; row < positions.size(); row++) {
        for (size_t col = 0; col < sizes.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 50),
                 static_cast<float>(row * 250 + 50),
                 200, 200});
            testNode->SetBoundsPosition(positions[row]);
            testNode->SetBoundsSize(sizes[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetBoundsPosition: quadrant positions - matrix 2x2 */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Quadrant_Matrix_2x2)
{
    std::vector<Vector2f> quadrantPositions = {
        {-100, -100},  // Top-left
        {100, -100},   // Top-right
        {-100, 100},   // Bottom-left
        {100, 100}     // Bottom-right
    };

    for (size_t i = 0; i < quadrantPositions.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 400 + 200),
             static_cast<float>((i / 2) * 400 + 200),
             300, 300});
        testNode->SetBoundsPosition(quadrantPositions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPosition: diagonal positions */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Diagonal)
{
    std::vector<Vector2f> diagonalPositions = {
        {-200, -200},
        {-100, -100},
        {0, 0},
        {100, 100},
        {200, 200}
    };

    for (size_t i = 0; i < diagonalPositions.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>(i * 200 + 50);
        float y = 100;
        testNode->SetBounds({x, y, 200, 200});
        testNode->SetBoundsPosition(diagonalPositions[i]);
        testNode->SetBackgroundColor(0xffff00ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPosition: interaction with SetBounds */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_SetBounds_Interaction)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 300, 300});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // SetBounds followed by SetBoundsPosition
    testNode->SetBounds({50, 50, 200, 200});
    testNode->SetBoundsPosition({100, 100});

    // SetBoundsPosition followed by SetBounds
    testNode->SetBoundsPosition({150, 150});
    testNode->SetBounds({100, 100, 100, 100});
}

/* SetBoundsPosition: fractional positions */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Fractional)
{
    std::vector<Vector2f> fractionalPositions = {
        {0.5f, 0.5f},
        {1.5f, 1.5f},
        {10.5f, 10.5f},
        {50.5f, 50.5f},
        {100.5f, 100.5f}
    };

    for (size_t i = 0; i < fractionalPositions.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 3) * 300 + 50);
        float y = static_cast<float>((i / 3) * 200 + 50);
        testNode->SetBounds({x, y, 200, 200});
        testNode->SetBoundsPosition(fractionalPositions[i]);
        testNode->SetBackgroundColor(0xffccff00);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetBoundsPosition: hierarchical positioning */
GRAPHIC_TEST(BoundsPositionTest, CONTENT_DISPLAY_TEST, BoundsPosition_Hierarchical)
{
    // Parent node
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({50, 50, 500, 500});
    parentNode->SetBackgroundColor(0xffcccccc);
    GetRootNode()->AddChild(parentNode);
    RegisterNode(parentNode);

    // Child nodes with different positions
    std::vector<Vector2f> childPositions = {
        {0, 0},
        {50, 50},
        {100, 100},
        {150, 150}
    };

    for (size_t i = 0; i < childPositions.size(); i++) {
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({childPositions[i].x_, childPositions[i].y_,
                             100, 100});
        childNode->SetBackgroundColor(0xffff0000);
        parentNode->AddChild(childNode);
        RegisterNode(childNode);
    }
}

} // namespace OHOS::Rosen
