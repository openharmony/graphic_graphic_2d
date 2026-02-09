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

class FramePositionTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetFramePosition: normal values - matrix 4x3 (x x y) */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Normal_Matrix_4x3)
{
    std::vector<float> positionsX = {0, 50, 100, 150};
    std::vector<float> positionsY = {0, 50, 100};

    for (size_t row = 0; row < positionsY.size(); row++) {
        for (size_t col = 0; col < positionsX.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {50, 50, 200, 200});
            testNode->SetFramePosition({positionsX[col], positionsY[row]});
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFramePosition: boundary values - zero and negative */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Boundary_Zero_Negative)
{
    std::vector<Vector2f> positions = {
        {0, 0},
        {-100, 0},
        {0, -100},
        {-100, -100},
        {-200, -200}
    };

    for (size_t i = 0; i < positions.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 300 + 200),
             static_cast<float>((i / 2) * 300 + 200),
             200, 200});
        testNode->SetFramePosition(positions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFramePosition: extreme values */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Extreme_Values)
{
    std::vector<Vector2f> extremePositions = {
        {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()},
        {std::numeric_limits<float>::min(), std::numeric_limits<float>::min()},
        {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()},
        {1000, 1000},
        {-1000, -1000},
        {5000, 5000},
        {-5000, -5000}
    };

    for (size_t i = 0; i < extremePositions.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 500 + 100),
             static_cast<float>((i / 2) * 500 + 100),
             300, 300});
        testNode->SetFramePosition(extremePositions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFramePosition: infinity and NaN */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Invalid_Float)
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
        testNode->SetBounds({x, y, 200, 200});
        testNode->SetFramePosition(invalidPositions[i]);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFramePositionX: normal values - 1x5 matrix */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePositionX_Normal_Matrix_1x5)
{
    std::vector<float> positions = {
        -100, -50, 0, 50, 100
    };

    for (size_t i = 0; i < positions.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>(i * 200 + 50);
        float y = 100;
        testNode->SetBounds({x, y, 150, 150});
        testNode->SetFramePositionX(positions[i]);
        testNode->SetBackgroundColor(0xff00ff00);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFramePositionX: extreme values */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePositionX_Extreme_Values)
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
            {static_cast<float>((i % 2) * 500 + 100),
             static_cast<float>((i / 2) * 200 + 100),
             200, 200});
        testNode->SetFramePositionX(extremePositions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFramePositionY: normal values - 5x1 matrix */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePositionY_Normal_Matrix_5x1)
{
    std::vector<float> positions = {
        -100, -50, 0, 50, 100
    };

    for (size_t i = 0; i < positions.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = 100;
        float y = static_cast<float>(i * 200 + 50);
        testNode->SetBounds({x, y, 150, 150});
        testNode->SetFramePositionY(positions[i]);
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFramePositionY: extreme values */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePositionY_Extreme_Values)
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
             static_cast<float>((i / 2) * 500 + 100),
             200, 200});
        testNode->SetFramePositionY(extremePositions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFramePositionX/Y: combined test matrix */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePositionXY_Combined_Matrix_3x3)
{
    std::vector<float> positionsX = {-100, 0, 100};
    std::vector<float> positionsY = {-100, 0, 100};

    for (size_t row = 0; row < positionsY.size(); row++) {
        for (size_t col = 0; col < positionsX.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 100),
                 static_cast<float>(row * 300 + 100),
                 200, 200});
            testNode->SetFramePositionX(positionsX[col]);
            testNode->SetFramePositionY(positionsY[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFramePosition: position vs bounds relationship */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Bounds_Relationship)
{
    // Position at origin
    auto testNode1 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 200, 200});
    testNode1->SetFramePosition({0, 0});
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Position offset within bounds
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {400, 100, 200, 200});
    testNode2->SetFramePosition({50, 50});
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);

    // Position offset beyond bounds
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 400, 200, 200});
    testNode3->SetFramePosition({200, 200});
    GetRootNode()->AddChild(testNode3);
    RegisterNode(testNode3);

    // Negative position
    auto testNode4 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {400, 400, 200, 200});
    testNode4->SetFramePosition({-100, -100});
    GetRootNode()->AddChild(testNode4);
    RegisterNode(testNode4);
}

/* SetFramePosition: rapid sequential updates */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Rapid_Updates)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 300, 300});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    testNode->SetFramePosition({0, 0});
    testNode->SetFramePosition({50, 50});
    testNode->SetFramePosition({100, 100});
    testNode->SetFramePosition({-50, -50});
    testNode->SetFramePosition({0, 50});
    testNode->SetFramePosition({50, 0});
    testNode->SetFramePosition({150, 150});
}

/* SetFramePosition/PositionX/PositionY: interaction test */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Interaction)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 300, 300});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // SetFramePosition followed by SetFramePositionX
    testNode->SetFramePosition({100, 100});
    testNode->SetFramePositionX(200);

    // SetFramePositionY followed by SetFramePosition
    testNode->SetFramePositionY(200);
    testNode->SetFramePosition({50, 50});

    // SetFramePositionX followed by SetFramePositionY
    testNode->SetFramePositionX(150);
    testNode->SetFramePositionY(150);

    // Verify final state
    testNode->SetBackgroundColor(0xffffff00);
}

/* SetFramePosition: quadrant positions - matrix 2x2 */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Quadrant_Matrix_2x2)
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
        testNode->SetFramePosition(quadrantPositions[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFramePosition: diagonal positions */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Diagonal)
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
        testNode->SetFramePosition(diagonalPositions[i]);
        testNode->SetBackgroundColor(0xffff00ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFramePosition: combined with size - matrix 3x2 */
GRAPHIC_TEST(FramePositionTest, CONTENT_DISPLAY_TEST, FramePosition_Size_Matrix_3x2)
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
                 static_cast<float>(row * 300 + 50),
                 250, 250});
            testNode->SetFramePosition(positions[row]);
            testNode->SetFrameSize(sizes[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

} // namespace OHOS::Rosen
