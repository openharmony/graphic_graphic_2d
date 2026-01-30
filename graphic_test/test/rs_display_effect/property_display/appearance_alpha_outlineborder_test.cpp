/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "parameters_defination.h"
#include "rs_graphic_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AppearanceAlphaOutlineBorderTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    // Helper function to create outline node with specified parameters
    std::shared_ptr<RSCanvasNode> CreateOutlineNode(
        const Vector4f& bounds,
        const Vector4<Color>& outlineColor,
        const Vector4f& outlineWidth,
        const Vector4f& outlineRadius,
        const Vector4<BorderStyle>& outlineStyle,
        const Vector4f& dashGap = {0, 0, 0, 0},
        const Vector4f& dashWidth = {0, 0, 0, 0},
        float alpha = 1.0f,
        uint32_t bgColor = 0x8FFF00FF)
    {
        auto node = RSCanvasNode::Create();
        node->SetAlpha(alpha);
        node->SetBounds(bounds);
        node->SetFrame(bounds);
        node->SetBackgroundColor(bgColor);
        node->SetOutlineColor(outlineColor);
        node->SetOutlineWidth(outlineWidth);
        node->SetOutlineRadius(outlineRadius);
        node->SetOutlineStyle(outlineStyle);
        node->SetOutlineDashGap(dashGap);
        node->SetOutlineDashWidth(dashWidth);
        node->SetTranslate(TWENTY_, TWENTY_, 0);
        return node;
    }
};

// ============================================================================
// Grid Layout Tests - Multiple scenarios in one test
// ============================================================================

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_Color_Grid_Test
 * @tc.desc: Test outline border with various colors including edge cases (alpha channel, white, black, multi-color)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_OutlineBorder_Color_Grid_Test)
{
    // Color scenarios: basic colors, alpha variations, multi-color per border
    Color colorList[] = {
        Color(0, 0, 0),           // Black
        Color(255, 0, 0),         // Red
        Color(0, 255, 0),         // Green
        Color(0, 0, 255),         // Blue
        Color(255, 255, 255),     // White
        Color(128, 128, 128),     // Gray
        Color(255, 255, 0),       // Yellow
        Color(255, 0, 255),       // Magenta
        Color(0, 255, 255),       // Cyan
        Color(0x7dffffff),        // Semi-transparent white
        Color(0x80ff0000),        // Semi-transparent red
        Color(0x330000ff),        // Low opacity blue
    };

    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 4;

    for (size_t i = 0; i < sizeof(colorList) / sizeof(colorList[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(colorList[i]),
            {FIVE_, FIVE_, FIVE_, FIVE_},
            {TEN_, TEN_, TEN_, TEN_},
            Vector4<BorderStyle>(BorderStyle::SOLID),
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            0.5f
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }

    // Four different colors on each border edge
    auto fourColorNode = CreateOutlineNode(
        {gap + (nodeWidth + gap) * 0, gap + (nodeHeight + gap) * 3, nodeWidth, nodeHeight},
        Vector4<Color>(Color(0, 0, 0), Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255)),
        {FIVE_, FIVE_, FIVE_, FIVE_},
        {TEN_, TEN_, TEN_, TEN_},
        Vector4<BorderStyle>(BorderStyle::SOLID),
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        0.7f
    );
    GetRootNode()->AddChild(fourColorNode);
    RegisterNode(fourColorNode);
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_Width_Grid_Test
 * @tc.desc: Test outline border width with edge cases (0, negative equivalent, very large)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_OutlineBorder_Width_Grid_Test)
{
    // Width scenarios: 0, small, normal, large, very large
    uint32_t widthList[] = {
        0,       // Zero width (no border)
        1,       // Minimal width
        5,       // Small width
        10,      // Normal width
        20,      // Medium width
        50,      // Large width
        100,     // Very large width
        200,     // Extremely large width
        120,     // Dash gap test width
        TWO_HUNDRED_FIFTY_, // Extra large width
    };

    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 5;

    for (size_t i = 0; i < sizeof(widthList) / sizeof(widthList[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(Color(0, 0, 0)),
            {static_cast<float>(widthList[i]), static_cast<float>(widthList[i]),
             static_cast<float>(widthList[i]), static_cast<float>(widthList[i])},
            {TEN_, TEN_, TEN_, TEN_},
            Vector4<BorderStyle>(BorderStyle::SOLID),
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            0.5f
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }

    // Four different widths on each border edge
    auto diffWidthNode = CreateOutlineNode(
        {gap, gap + (nodeHeight + gap) * 2, nodeWidth, nodeHeight},
        Vector4<Color>(Color(0, 0, 0)),
        {0, 10, 20, 40},  // 0, 1x, 2x, 4x multiplier
        {TEN_, TEN_, TEN_, TEN_},
        Vector4<BorderStyle>(BorderStyle::SOLID),
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        0.8f
    );
    GetRootNode()->AddChild(diffWidthNode);
    RegisterNode(diffWidthNode);
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_Alpha_Grid_Test
 * @tc.desc: Test alpha with edge cases (0.0, 1.0, negative equivalent, very small values)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_OutlineBorder_Alpha_Grid_Test)
{
    // Alpha scenarios: edge cases and normal values
    float alphaList[] = {
        0.0f,    // Fully transparent (should show nothing)
        0.1f,    // Very low opacity
        0.2f,    // Low opacity
        0.3f,    // Low-medium opacity
        0.4f,    // Medium-low opacity
        0.5f,    // Half opacity
        0.6f,    // Medium opacity
        0.7f,    // Medium-high opacity
        0.8f,    // High opacity
        0.9f,    // Very high opacity
        1.0f,    // Fully opaque
        0.25f,   // Quarter opacity
        0.75f,   // Three-quarter opacity
        0.05f,   // Near-transparent
        0.95f,   // Near-opaque
    };

    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 5;

    for (size_t i = 0; i < sizeof(alphaList) / sizeof(alphaList[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(Color(255, 0, 0)),  // Red border for visibility
            {FIVE_, FIVE_, FIVE_, FIVE_},
            {TEN_, TEN_, TEN_, TEN_},
            Vector4<BorderStyle>(BorderStyle::SOLID),
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            alphaList[i]
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_Style_Grid_Test
 * @tc.desc: Test border style (Solid, Dashed, Dotted, None) with various parameters
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_OutlineBorder_Style_Grid_Test)
{
    // Style: 0=Solid, 1=Dashed, 2=Dotted, 3=None
    struct StyleTestScenario {
        BorderStyle style;
        float dashWidth;
        float dashGap;
        const char* description;
    };

    StyleTestScenario scenarios[] = {
        {BorderStyle::SOLID, 0, 0, "Solid basic"},
        {BorderStyle::DASHED, 20, 20, "Dashed 20/20"},
        {BorderStyle::DASHED, 10, 10, "Dashed 10/10"},
        {BorderStyle::DASHED, 50, 50, "Dashed 50/50"},
        {BorderStyle::DOTTED, 0, 0, "Dotted basic"},
        {BorderStyle::DOTTED, 10, 20, "Dotted with gap"},
        {BorderStyle::NONE, 0, 0, "None (no border)"},
        {BorderStyle::DASHED, 5, 30, "Dashed small gap"},
        {BorderStyle::DASHED, 30, 5, "Dashed large dash"},
        {BorderStyle::SOLID, 100, 100, "Solid with params (ignored)"},
        {BorderStyle::DASHED, 0, 50, "Dashed zero width"},
        {BorderStyle::DASHED, 50, 0, "Dashed zero gap"},
    };

    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 4;

    for (size_t i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(Color(0, 0, 0)),
            {FIVE_, FIVE_, FIVE_, FIVE_},
            {TEN_, TEN_, TEN_, TEN_},
            Vector4<BorderStyle>(scenarios[i].style),
            {scenarios[i].dashGap, scenarios[i].dashGap, scenarios[i].dashGap, scenarios[i].dashGap},
            {scenarios[i].dashWidth, scenarios[i].dashWidth, scenarios[i].dashWidth, scenarios[i].dashWidth},
            0.7f
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }

    // Mixed styles on different edges
    auto mixedStyleNode = CreateOutlineNode(
        {gap + (nodeWidth + gap) * 0, gap + (nodeHeight + gap) * 3, nodeWidth, nodeHeight},
        Vector4<Color>(Color(0, 0, 0)),
        {FIVE_, FIVE_, FIVE_, FIVE_},
        {TEN_, TEN_, TEN_, TEN_},
        Vector4<BorderStyle>(BorderStyle::SOLID, BorderStyle::DASHED, BorderStyle::DOTTED, BorderStyle::NONE),
        {20, 20, 20, 20},
        {20, 20, 20, 20},
        0.8f
    );
    GetRootNode()->AddChild(mixedStyleNode);
    RegisterNode(mixedStyleNode);
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_Radius_Grid_Test
 * @tc.desc: Test outline radius with edge cases (0, very large, asymmetric)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_OutlineBorder_Radius_Grid_Test)
{
    // Radius scenarios
    uint32_t radiusList[] = {
        0,       // Sharp corners (no radius)
        5,       // Small radius
        10,      // Normal radius
        20,      // Medium radius
        50,      // Large radius
        80,      // Very large radius
        100,     // Extremely large (may exceed node size)
        200,     // Larger than node height
    };

    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 4;

    for (size_t i = 0; i < sizeof(radiusList) / sizeof(radiusList[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(Color(0, 0, 0)),
            {FIVE_, FIVE_, FIVE_, FIVE_},
            {static_cast<float>(radiusList[i]), static_cast<float>(radiusList[i]),
             static_cast<float>(radiusList[i]), static_cast<float>(radiusList[i])},
            Vector4<BorderStyle>(BorderStyle::SOLID),
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            0.7f
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }

    // Asymmetric radii
    struct AsymRadiusScenario {
        float r1, r2, r3, r4;
    };
    AsymRadiusScenario asymScenarios[] = {
        {0, 10, 20, 30},      // Increasing radius
        {50, 50, 10, 10},     // Large top, small bottom
        {100, 0, 100, 0},     // Alternating extreme
        {10, 20, 10, 20},     // Alternating pattern
    };

    for (size_t i = 0; i < sizeof(asymScenarios) / sizeof(asymScenarios[0]); i++) {
        int32_t col = i % 2;
        int32_t row = 2 + (i / 2);

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(Color(0, 0, 0)),
            {10, 10, 10, 10},
            {asymScenarios[i].r1, asymScenarios[i].r2, asymScenarios[i].r3, asymScenarios[i].r4},
            Vector4<BorderStyle>(BorderStyle::SOLID),
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            0.7f
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_DashWidth_Grid_Test
 * @tc.desc: Test dash width with edge cases (0, large, mixed styles)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST,
    Appearance_Alpha_OutlineBorder_DashWidth_Grid_Test)
{
    // Dash width scenarios for DASHED style
    uint32_t dashWidthList[] = {
        0,       // Zero (solid-like)
        5,       // Small dash
        10,      // Normal dash
        20,      // Medium dash
        50,      // Large dash
        100,     // Very large dash
        TWO_HUNDRED_FIFTY_, // Extra large dash
        120,     // Dash gap test width
    };

    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 4;

    for (size_t i = 0; i < sizeof(dashWidthList) / sizeof(dashWidthList[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(Color(0, 0, 0)),
            {FIVE_, FIVE_, FIVE_, FIVE_},
            {TEN_, TEN_, TEN_, TEN_},
            Vector4<BorderStyle>(BorderStyle::DASHED),
            {20, 20, 20, 20},  // Fixed gap
            {static_cast<float>(dashWidthList[i]), static_cast<float>(dashWidthList[i]),
             static_cast<float>(dashWidthList[i]), static_cast<float>(dashWidthList[i])},
            0.7f
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }

    // Different dash widths on each edge
    auto diffDashWidthNode = CreateOutlineNode(
        {gap + (nodeWidth + gap) * 0, gap + (nodeHeight + gap) * 2, nodeWidth, nodeHeight},
        Vector4<Color>(Color(0, 0, 0)),
        {FIVE_, FIVE_, FIVE_, FIVE_},
        {TEN_, TEN_, TEN_, TEN_},
        Vector4<BorderStyle>(BorderStyle::DASHED),
        {20, 20, 20, 20},
        {0, 20, 40, 80},  // 0, 1x, 2x, 4x multiplier
        0.7f
    );
    GetRootNode()->AddChild(diffDashWidthNode);
    RegisterNode(diffDashWidthNode);

    // Solid style with dash width (should be ignored)
    auto solidWithDashNode = CreateOutlineNode(
        {gap + (nodeWidth + gap) * 1, gap + (nodeHeight + gap) * 2, nodeWidth, nodeHeight},
        Vector4<Color>(Color(0, 0, 0)),
        {FIVE_, FIVE_, FIVE_, FIVE_},
        {TEN_, TEN_, TEN_, TEN_},
        Vector4<BorderStyle>(BorderStyle::SOLID),
        {20, 20, 20, 20},
        {20, 20, 20, 20},
        0.7f
    );
    GetRootNode()->AddChild(solidWithDashNode);
    RegisterNode(solidWithDashNode);
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_DashGap_Grid_Test
 * @tc.desc: Test dash gap with edge cases (0, large, mixed styles)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST,
    Appearance_Alpha_OutlineBorder_DashGap_Grid_Test)
{
    // Dash gap scenarios for DASHED style
    uint32_t dashGapList[] = {
        0,       // Zero (continuous line)
        5,       // Small gap
        10,      // Normal gap
        20,      // Medium gap
        50,      // Large gap
        100,     // Very large gap
        TWO_HUNDRED_FIFTY_, // Extra large gap
        120,     // Dash gap test gap
    };

    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 4;

    for (size_t i = 0; i < sizeof(dashGapList) / sizeof(dashGapList[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(Color(0, 0, 0)),
            {FIVE_, FIVE_, FIVE_, FIVE_},
            {TEN_, TEN_, TEN_, TEN_},
            Vector4<BorderStyle>(BorderStyle::DASHED),
            {static_cast<float>(dashGapList[i]), static_cast<float>(dashGapList[i]),
             static_cast<float>(dashGapList[i]), static_cast<float>(dashGapList[i])},
            {20, 20, 20, 20},  // Fixed dash width
            0.7f
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }

    // Different dash gaps on each edge
    auto diffDashGapNode = CreateOutlineNode(
        {gap + (nodeWidth + gap) * 0, gap + (nodeHeight + gap) * 2, nodeWidth, nodeHeight},
        Vector4<Color>(Color(0, 0, 0)),
        {FIVE_, FIVE_, FIVE_, FIVE_},
        {TEN_, TEN_, TEN_, TEN_},
        Vector4<BorderStyle>(BorderStyle::DASHED),
        {0, 20, 40, 80},  // 0, 1x, 2x, 4x multiplier
        {20, 20, 20, 20},
        0.7f
    );
    GetRootNode()->AddChild(diffDashGapNode);
    RegisterNode(diffDashGapNode);

    // Solid style with dash gap (should be ignored)
    auto solidWithGapNode = CreateOutlineNode(
        {gap + (nodeWidth + gap) * 1, gap + (nodeHeight + gap) * 2, nodeWidth, nodeHeight},
        Vector4<Color>(Color(0, 0, 0)),
        {FIVE_, FIVE_, FIVE_, FIVE_},
        {TEN_, TEN_, TEN_, TEN_},
        Vector4<BorderStyle>(BorderStyle::SOLID),
        {20, 20, 20, 20},
        {20, 20, 20, 20},
        0.7f
    );
    GetRootNode()->AddChild(solidWithGapNode);
    RegisterNode(solidWithGapNode);
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_Comprehensive_Grid_Test
 * @tc.desc: Comprehensive test combining various parameters (style, width, radius, color, alpha, dash)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST,
    Appearance_Alpha_OutlineBorder_Comprehensive_Grid_Test)
{
    struct ComprehensiveScenario {
        BorderStyle style;
        float width;
        float radius;
        uint32_t color;
        float alpha;
        float dashWidth;
        float dashGap;
        const char* description;
    };

    // Comprehensive test scenarios covering various combinations
    ComprehensiveScenario scenarios[] = {
        // Basic solid borders
        {BorderStyle::SOLID, 5, 10, 0xFF000000, 1.0f, 0, 0, "Solid normal"},
        {BorderStyle::SOLID, 10, 20, 0xFFFF0000, 0.8f, 0, 0, "Solid red"},
        {BorderStyle::SOLID, 20, 50, 0xFF00FF00, 0.6f, 0, 0, "Solid thick"},
        {BorderStyle::SOLID, 0, 10, 0xFF0000FF, 1.0f, 0, 0, "Solid zero width"},

        // Dashed borders
        {BorderStyle::DASHED, 5, 10, 0xFF000000, 1.0f, 10, 10, "Dashed 10/10"},
        {BorderStyle::DASHED, 5, 10, 0xFFFF0000, 0.7f, 20, 20, "Dashed 20/20"},
        {BorderStyle::DASHED, 5, 10, 0xFF00FF00, 0.5f, 50, 50, "Dashed 50/50"},
        {BorderStyle::DASHED, 5, 10, 0xFF0000FF, 0.9f, 100, 20, "Dashed long"},

        // Dotted borders
        {BorderStyle::DOTTED, 5, 10, 0xFF000000, 1.0f, 0, 0, "Dotted basic"},
        {BorderStyle::DOTTED, 10, 20, 0xFFFF0000, 0.8f, 0, 0, "Dotted thick"},
        {BorderStyle::DOTTED, 5, 50, 0xFF00FF00, 0.6f, 0, 0, "Dotted large radius"},
        {BorderStyle::DOTTED, 5, 0, 0xFF0000FF, 0.9f, 0, 0, "Dotted sharp"},

        // Mixed style combinations
        {BorderStyle::SOLID, 20, 50, 0xFF97001F, 0.9f, 0, 0, "Solid extreme"},
        {BorderStyle::DASHED, 100, 80, 0xFF93001F, 0.8f, 100, 100, "Dashed extreme"},
        {BorderStyle::DOTTED, 20, 0, 0xFF89001F, 0.7f, 0, 0, "Dotted sharp thick"},
        {BorderStyle::DASHED, 10, 100, 0xFF85001F, 0.6f, 50, 100, "Dashed gap>width"},

        // Edge cases
        {BorderStyle::SOLID, 200, 100, 0xFF81001F, 0.5f, 0, 0, "Width>size"},
        {BorderStyle::DASHED, 10, 10, 0xFF77001F, 0.4f, 0, 50, "Dashed zero width"},
        {BorderStyle::DASHED, 10, 10, 0xFF73001F, 0.3f, 50, 0, "Dashed zero gap"},
        {BorderStyle::NONE, 10, 10, 0xFF69001F, 0.2f, 0, 0, "None style"},

        // Alpha variations
        {BorderStyle::SOLID, 10, 10, 0xFF00001F, 0.1f, 0, 0, "Alpha 0.1"},
        {BorderStyle::DASHED, 10, 10, 0xFF00002F, 0.2f, 20, 20, "Alpha 0.2"},
        {BorderStyle::DOTTED, 10, 10, 0xFF00003F, 0.3f, 0, 0, "Alpha 0.3"},
        {BorderStyle::SOLID, 10, 10, 0xFF00004F, 0.0f, 0, 0, "Alpha 0.0"},
    };

    int32_t nodeWidth = 180;
    int32_t nodeHeight = 180;
    int32_t gap = 25;
    int32_t column = 5;

    for (size_t i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(Color(scenarios[i].color)),
            {scenarios[i].width, scenarios[i].width, scenarios[i].width, scenarios[i].width},
            {scenarios[i].radius, scenarios[i].radius, scenarios[i].radius, scenarios[i].radius},
            Vector4<BorderStyle>(scenarios[i].style),
            {scenarios[i].dashGap, scenarios[i].dashGap, scenarios[i].dashGap, scenarios[i].dashGap},
            {scenarios[i].dashWidth, scenarios[i].dashWidth, scenarios[i].dashWidth, scenarios[i].dashWidth},
            scenarios[i].alpha
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_MixedStyle_Per_Edge_Test
 * @tc.desc: Test different border styles on each edge (Solid/Dash/Dot/None combinations)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST,
    Appearance_Alpha_OutlineBorder_MixedStyle_Per_Edge_Test)
{
    struct MixedStyleScenario {
        BorderStyle top, right, bottom, left;
        float dashWidth;
        float dashGap;
        const char* description;
    };

    MixedStyleScenario scenarios[] = {
        {BorderStyle::SOLID, BorderStyle::DASHED, BorderStyle::DOTTED, BorderStyle::NONE, 20, 20, "S-D-D-N"},
        {BorderStyle::DASHED, BorderStyle::DOTTED, BorderStyle::NONE, BorderStyle::SOLID, 10, 10, "D-D-N-S"},
        {BorderStyle::DOTTED, BorderStyle::NONE, BorderStyle::SOLID, BorderStyle::DASHED, 15, 15, "D-N-S-D"},
        {BorderStyle::NONE, BorderStyle::SOLID, BorderStyle::DASHED, BorderStyle::DOTTED, 25, 25, "N-S-D-D"},
        {BorderStyle::SOLID, BorderStyle::SOLID, BorderStyle::DASHED, BorderStyle::DASHED, 20, 20, "S-S-D-D"},
        {BorderStyle::DASHED, BorderStyle::DASHED, BorderStyle::DOTTED, BorderStyle::DOTTED, 10, 10, "D-D-D-D"},
        {BorderStyle::SOLID, BorderStyle::DASHED, BorderStyle::SOLID, BorderStyle::DASHED, 30, 30, "S-D-S-D"},
        {BorderStyle::DOTTED, BorderStyle::SOLID, BorderStyle::DOTTED, BorderStyle::SOLID, 5, 5, "D-S-D-S"},
        {BorderStyle::SOLID, BorderStyle::NONE, BorderStyle::SOLID, BorderStyle::NONE, 20, 20, "S-N-S-N"},
        {BorderStyle::NONE, BorderStyle::SOLID, BorderStyle::NONE, BorderStyle::SOLID, 20, 20, "N-S-N-S"},
        {BorderStyle::DASHED, BorderStyle::NONE, BorderStyle::DASHED, BorderStyle::NONE, 50, 50, "D-N-D-N"},
        {BorderStyle::NONE, BorderStyle::DASHED, BorderStyle::NONE, BorderStyle::DASHED, 50, 50, "N-D-N-D"},
    };

    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 4;

    for (size_t i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        auto node = CreateOutlineNode(
            {gap + (nodeWidth + gap) * col, gap + (nodeHeight + gap) * row, nodeWidth, nodeHeight},
            Vector4<Color>(Color(0, 0, 0)),
            {FIVE_, FIVE_, FIVE_, FIVE_},
            {TEN_, TEN_, TEN_, TEN_},
            Vector4<BorderStyle>(scenarios[i].top, scenarios[i].right, scenarios[i].bottom, scenarios[i].left),
            {scenarios[i].dashGap, scenarios[i].dashGap, scenarios[i].dashGap, scenarios[i].dashGap},
            {scenarios[i].dashWidth, scenarios[i].dashWidth, scenarios[i].dashWidth, scenarios[i].dashWidth},
            0.7f
        );
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_Touch_Overlay_Test
 * @tc.desc: Test two nodes with outline borders overlapping and touching
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST,
    Appearance_Alpha_OutlineBorder_Touch_Overlay_Test)
{
    struct TouchOverlayScenario {
        BorderStyle style1, style2;
        Color color1, color2;
        float offsetX, offsetY;
        float alpha1, alpha2;
        const char* description;
    };

    TouchOverlayScenario scenarios[] = {
        {BorderStyle::SOLID, BorderStyle::SOLID, Color(255, 0, 0), Color(0, 0, 255), 50, 50, 0.5f, 0.5f, "Solid-Solid"},
        {BorderStyle::DASHED, BorderStyle::DASHED, Color(255, 0, 0), Color(0, 0, 255), 50, 50, 0.5f, 0.5f, "Dash-Dash"},
        {BorderStyle::DOTTED, BorderStyle::DOTTED, Color(255, 0, 0), Color(0, 0, 255), 50, 50, 0.5f, 0.5f, "Dot-Dot"},
        {BorderStyle::SOLID, BorderStyle::DASHED, Color(255, 0, 0), Color(0, 0, 255), 50, 50, 0.5f, 0.5f, "Solid-Dash"},
        {BorderStyle::DASHED, BorderStyle::DOTTED, Color(255, 0, 0), Color(0, 0, 255), 50, 50, 0.5f, 0.5f, "Dash-Dot"},
        {BorderStyle::SOLID, BorderStyle::DOTTED, Color(255, 0, 0), Color(0, 0, 255), 50, 50, 0.5f, 0.5f, "Solid-Dot"},
        {BorderStyle::SOLID, BorderStyle::SOLID, Color(255, 0, 0), Color(0, 0, 255), 20, 20, 0.3f, 0.7f, "Alpha diff"},
        {BorderStyle::DASHED, BorderStyle::DASHED, Color(255, 0, 0), Color(0, 0, 255),
         100, 20, 0.5f, 0.5f, "Large offset"},
    };

    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 2;

    for (size_t i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); i++) {
        int32_t row = i / column;
        int32_t col = i % column;

        int32_t baseX = gap + (nodeWidth + gap * 2 + 50) * col;
        int32_t baseY = gap + (nodeHeight + gap) * row;

        // First node (background)
        auto node1 = CreateOutlineNode(
            {baseX, baseY, nodeWidth, nodeHeight},
            Vector4<Color>(scenarios[i].color1),
            {TEN_, TEN_, TEN_, TEN_},
            {TEN_, TEN_, TEN_, TEN_},
            Vector4<BorderStyle>(scenarios[i].style1),
            {TWENTY_FIVE_, TWENTY_FIVE_, TWENTY_FIVE_, TWENTY_FIVE_},
            {0, 0, 0, 0},
            scenarios[i].alpha1,
            0xff000000  // Black background
        );
        GetRootNode()->AddChild(node1);
        RegisterNode(node1);

        // Second node (overlay)
        auto node2 = CreateOutlineNode(
            {baseX + scenarios[i].offsetX, baseY + scenarios[i].offsetY, nodeWidth, nodeHeight},
            Vector4<Color>(scenarios[i].color2),
            {TEN_, TEN_, TEN_, TEN_},
            {TEN_, TEN_, TEN_, TEN_},
            Vector4<BorderStyle>(scenarios[i].style2),
            {TWENTY_FIVE_, TWENTY_FIVE_, TWENTY_FIVE_, TWENTY_FIVE_},
            {0, 0, 0, 0},
            scenarios[i].alpha2,
            0xff000000  // Black background
        );
        GetRootNode()->AddChild(node2);
        RegisterNode(node2);
    }
}

/*
 * @tc.name: Appearance_Alpha_OutlineBorder_ParentChild_Test
 * @tc.desc: Test parent-child node relationship with outline borders
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AppearanceAlphaOutlineBorderTest, CONTENT_DISPLAY_TEST,
    Appearance_Alpha_OutlineBorder_ParentChild_Test)
{
    // Parent with border, child with different border
    auto parentNode = CreateOutlineNode(
        {50, 50, 400, 400},
        Vector4<Color>(Color(0, 0, 0)),  // Black border
        {20, 20, 20, 20},
        {50, 50, 50, 50},
        Vector4<BorderStyle>(BorderStyle::SOLID),
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        0.7f,
        0xff000000  // Black background
    );
    GetRootNode()->AddChild(parentNode);
    RegisterNode(parentNode);

    auto childNode = CreateOutlineNode(
        {50, 50, 200, 200},
        Vector4<Color>(Color(255, 255, 255)),  // White border
        {10, 10, 10, 10},
        {20, 20, 20, 20},
        Vector4<BorderStyle>(BorderStyle::DASHED),
        {20, 20, 20, 20},
        {20, 20, 20, 20},
        0.9f,
        0xffff0000  // Red foreground
    );
    parentNode->AddChild(childNode);
    RegisterNode(childNode);

    // Multiple nested children with different styles
    auto nestedParent = CreateOutlineNode(
        {500, 50, 400, 400},
        Vector4<Color>(Color(0, 0, 255)),  // Blue border
        {15, 15, 15, 15},
        {30, 30, 30, 30},
        Vector4<BorderStyle>(BorderStyle::SOLID),
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        0.6f,
        0xff00ff00  // Green background
    );
    GetRootNode()->AddChild(nestedParent);
    RegisterNode(nestedParent);

    auto nestedChild1 = CreateOutlineNode(
        {50, 50, 150, 150},
        Vector4<Color>(Color(255, 0, 0)),  // Red border
        {8, 8, 8, 8},
        {15, 15, 15, 15},
        Vector4<BorderStyle>(BorderStyle::DOTTED),
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        0.8f,
        0xffffff00  // Yellow background
    );
    nestedParent->AddChild(nestedChild1);
    RegisterNode(nestedChild1);

    auto nestedChild2 = CreateOutlineNode(
        {200, 200, 150, 150},
        Vector4<Color>(Color(255, 0, 255)),  // Magenta border
        {8, 8, 8, 8},
        {15, 15, 15, 15},
        Vector4<BorderStyle>(BorderStyle::DASHED),
        {15, 15, 15, 15},
        {15, 15, 15, 15},
        0.8f,
        0xff00ffff  // Cyan background
    );
    nestedParent->AddChild(nestedChild2);
    RegisterNode(nestedChild2);
}

} // namespace OHOS::Rosen