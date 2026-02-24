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
#include "draw/path.h"
#include "text/text_blob.h"
#include "transaction/rs_interfaces.h"

#include <functional>
#include <vector>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr uint32_t FONT_SIZE = 50;
constexpr uint32_t DEFAULT_SCALAR_X = 50;
constexpr uint32_t DEFAULT_SCALAR_Y = 50;
constexpr uint32_t RECORDING_WIDTH = 400;
constexpr uint32_t RECORDING_HEIGHT = 400;
constexpr uint32_t MAX_PIXELMAP_WIDTH = 5000;
constexpr uint32_t MAX_PIXELMAP_HEIGHT = 5000;

// Color schemes for different hybrid render types
constexpr uint32_t COLOR_NONE_BG = 0xFFFFF0E0;      // Light orange background
constexpr uint32_t COLOR_NONE_TEXT = 0xFF8B4513;    // Dark brown text

constexpr uint32_t COLOR_TEXT_BG = 0xFFE0F7FA;      // Light blue background
constexpr uint32_t COLOR_TEXT_TEXT = 0xFF01579B;    // Dark blue text

constexpr uint32_t COLOR_SVG_BG = 0xFFE8F5E9;       // Light green background
constexpr uint32_t COLOR_SVG_SHAPE = 0xFF2E7D32;    // Dark green shape

constexpr uint32_t COLOR_HMSYMBOL_BG = 0xFFF3E5F5;   // Light purple background
constexpr uint32_t COLOR_HMSYMBOL_TEXT = 0xFF6A1B9A; // Dark purple text

constexpr uint32_t COLOR_CANVAS_BG = 0xFFFFEBEE;    // Light red background
constexpr uint32_t COLOR_CANVAS_SHAPE = 0xFFC62828; // Dark red shape

// Auxiliary colors
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_CYAN = 0xFF00FFFF;
constexpr uint32_t COLOR_MAGENTA = 0xFFFF00FF;
} // namespace

/**
 * @class HybridRenderInterfacesTest
 * @brief Pixel-level comparison test class for hybrid render interfaces
 *        Uses different colors and graphics for visual comparison verification
 */
class HybridRenderInterfacesTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    // Helper function: Create colored CanvasNode
    std::shared_ptr<RSCanvasNode> CreateColoredNode(float x, float y, float w, float h,
        uint32_t bgColor = SK_ColorWHITE)
    {
        auto canvasNode = RSCanvasNode::Create();
        Vector4f bounds = {x, y, x + w, y + h};
        Vector4f frame = bounds;
        canvasNode->SetBounds(bounds);
        canvasNode->SetFrame(frame);
        canvasNode->SetBackgroundColor(bgColor);
        GetRootNode()->AddChild(canvasNode);
        return canvasNode;
    }

    // Helper function: Draw colored text
    void DrawColoredText(::OHOS::Rosen::ExtendRecordingCanvas* canvas, const std::string& text,
        uint32_t textColor, uint32_t offsetX = DEFAULT_SCALAR_X, uint32_t offsetY = DEFAULT_SCALAR_Y)
    {
        Font font = Font();
        font.SetSize(FONT_SIZE);
        Brush brush;
        brush.SetColor(textColor);
        brush.SetAntiAlias(true);
        canvas->AttachBrush(brush);
        std::shared_ptr<TextBlob> textblob = TextBlob::MakeFromString(text.c_str(), font, TextEncoding::UTF8);
        canvas->DrawTextBlob(textblob.get(), offsetX, offsetY);
        canvas->DetachBrush();
    }

    // Helper function: Draw colored rectangle
    void DrawColoredRect(::OHOS::Rosen::ExtendRecordingCanvas* canvas, float left, float top,
        float right, float bottom, uint32_t color)
    {
        Brush brush;
        brush.SetColor(color);
        canvas->AttachBrush(brush);
        canvas->DrawRect(Rect(left, top, right, bottom));
        canvas->DetachBrush();
    }

    // Helper function: Draw colored circle
    void DrawColoredCircle(::OHOS::Rosen::ExtendRecordingCanvas* canvas, float centerX, float centerY,
        float radius, uint32_t color)
    {
        Brush brush;
        brush.SetColor(color);
        brush.SetAntiAlias(true);
        canvas->AttachBrush(brush);
        canvas->DrawCircle(Point(centerX, centerY), radius);
        canvas->DetachBrush();
    }

    // Helper function: Draw colored path
    void DrawColoredPath(::OHOS::Rosen::ExtendRecordingCanvas* canvas, uint32_t color)
    {
        Path path;
        path.AddRect(RECORDING_WIDTH * 0.1f, RECORDING_HEIGHT * 0.6f,
                      RECORDING_WIDTH * 0.4f, RECORDING_HEIGHT * 0.9f);
        path.AddCircle(RECORDING_WIDTH * 0.7f, RECORDING_HEIGHT * 0.75f, RECORDING_WIDTH * 0.1f);

        Pen pen;
        pen.SetColor(color);
        pen.SetWidth(5.0f);
        pen.SetAntiAlias(true);
        canvas->AttachPen(pen);
        canvas->DrawPath(path);
        canvas->DetachPen();
    }
};

// ============================================================================
// SetHybridRenderType and GetHybridRenderType tests - Use different colors to distinguish types
// ============================================================================

/**
 * @tc.name: SetGetHybridRenderType_None_001
 * @tc.desc: Test NONE type - Light orange background + dark brown text + yellow decoration
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, SetGetHybridRenderType_None_001)
{
    auto canvasNode = CreateColoredNode(50, 50, RECORDING_WIDTH, RECORDING_HEIGHT, COLOR_NONE_BG);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    DrawColoredText(recordingCanvas, "NONE Type", COLOR_NONE_TEXT);
    DrawColoredRect(recordingCanvas, 50, 150, 350, 180, COLOR_YELLOW);
    DrawColoredCircle(recordingCanvas, 200, 280, 50, COLOR_YELLOW);

    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(DrawCmdList::HybridRenderType::NONE);
    EXPECT_EQ(recordingCanvas->GetDrawCmdList()->GetHybridRenderType(),
        DrawCmdList::HybridRenderType::NONE);

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: SetGetHybridRenderType_Text_002
 * @tc.desc: Test TEXT type - Light blue background + dark blue text + cyan decoration
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, SetGetHybridRenderType_Text_002)
{
    auto canvasNode = CreateColoredNode(50, 50, RECORDING_WIDTH, RECORDING_HEIGHT, COLOR_TEXT_BG);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    DrawColoredText(recordingCanvas, "TEXT Type", COLOR_TEXT_TEXT);
    DrawColoredRect(recordingCanvas, 50, 150, 350, 180, COLOR_CYAN);
    DrawColoredCircle(recordingCanvas, 200, 280, 50, COLOR_CYAN);

    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(DrawCmdList::HybridRenderType::TEXT);
    EXPECT_EQ(recordingCanvas->GetDrawCmdList()->GetHybridRenderType(),
        DrawCmdList::HybridRenderType::TEXT);

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: SetGetHybridRenderType_SVG_003
 * @tc.desc: Test SVG type - Light green background + dark green shapes + path drawing
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, SetGetHybridRenderType_SVG_003)
{
    auto canvasNode = CreateColoredNode(50, 50, RECORDING_WIDTH, RECORDING_HEIGHT, COLOR_SVG_BG);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    DrawColoredText(recordingCanvas, "SVG Type", COLOR_SVG_SHAPE);
    DrawColoredRect(recordingCanvas, 50, 150, 180, 250, COLOR_SVG_SHAPE);
    DrawColoredCircle(recordingCanvas, 280, 220, 60, COLOR_SVG_SHAPE);
    DrawColoredPath(recordingCanvas, COLOR_SVG_SHAPE);

    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(DrawCmdList::HybridRenderType::SVG);
    EXPECT_EQ(recordingCanvas->GetDrawCmdList()->GetHybridRenderType(),
        DrawCmdList::HybridRenderType::SVG);

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: SetGetHybridRenderType_HMSymbol_004
 * @tc.desc: Test HMSYMBOL type - Light purple background + dark purple text + magenta decoration
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, SetGetHybridRenderType_HMSymbol_004)
{
    auto canvasNode = CreateColoredNode(50, 50, RECORDING_WIDTH, RECORDING_HEIGHT, COLOR_HMSYMBOL_BG);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    DrawColoredText(recordingCanvas, "HMSYMBOL", COLOR_HMSYMBOL_TEXT);
    DrawColoredRect(recordingCanvas, 50, 150, 350, 180, COLOR_MAGENTA);
    DrawColoredCircle(recordingCanvas, 120, 280, 40, COLOR_MAGENTA);
    DrawColoredCircle(recordingCanvas, 280, 280, 40, COLOR_MAGENTA);

    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(DrawCmdList::HybridRenderType::HMSYMBOL);
    EXPECT_EQ(recordingCanvas->GetDrawCmdList()->GetHybridRenderType(),
        DrawCmdList::HybridRenderType::HMSYMBOL);

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: SetGetHybridRenderType_Canvas_005
 * @tc.desc: Test CANVAS type - Light red background + dark red shapes + combined forms
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, SetGetHybridRenderType_Canvas_005)
{
    auto canvasNode = CreateColoredNode(50, 50, RECORDING_WIDTH, RECORDING_HEIGHT, COLOR_CANVAS_BG);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    DrawColoredText(recordingCanvas, "CANVAS Type", COLOR_CANVAS_SHAPE);
    // 绘制多个形状形成对比
    DrawColoredRect(recordingCanvas, 50, 150, 150, 150, COLOR_CANVAS_SHAPE);
    DrawColoredCircle(recordingCanvas, 275, 225, 50, COLOR_YELLOW);
    DrawColoredPath(recordingCanvas, COLOR_CANVAS_SHAPE);

    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(DrawCmdList::HybridRenderType::CANVAS);
    EXPECT_EQ(recordingCanvas->GetDrawCmdList()->GetHybridRenderType(),
        DrawCmdList::HybridRenderType::CANVAS);

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: SetGetHybridRenderType_TypeMax_006
 * @tc.desc: Test boundary value TYPE_MAX - Gray background + black text
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, SetGetHybridRenderType_TypeMax_006)
{
    auto canvasNode = CreateColoredNode(500, 50, RECORDING_WIDTH, RECORDING_HEIGHT, 0xFFE0E0E0);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    DrawColoredText(recordingCanvas, "TYPE_MAX", SK_ColorBLACK);
    DrawColoredRect(recordingCanvas, 100, 200, 300, 50, SK_ColorDKGRAY);

    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(DrawCmdList::HybridRenderType::TYPE_MAX);
    EXPECT_EQ(recordingCanvas->GetDrawCmdList()->GetHybridRenderType(),
        DrawCmdList::HybridRenderType::TYPE_MAX);

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: SetGetHybridRenderType_Comparison_007
 * @tc.desc: Combination test: All types arranged horizontally for comparison
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, SetGetHybridRenderType_Comparison_007)
{
    struct TypeInfo {
        DrawCmdList::HybridRenderType type;
        uint32_t bgColor;
        uint32_t contentColor;
        std::string label;
    };

    std::vector<TypeInfo> types = {
        {DrawCmdList::HybridRenderType::NONE, COLOR_NONE_BG, COLOR_NONE_TEXT, "NONE"},
        {DrawCmdList::HybridRenderType::TEXT, COLOR_TEXT_BG, COLOR_TEXT_TEXT, "TEXT"},
        {DrawCmdList::HybridRenderType::SVG, COLOR_SVG_BG, COLOR_SVG_SHAPE, "SVG"},
        {DrawCmdList::HybridRenderType::HMSYMBOL, COLOR_HMSYMBOL_BG, COLOR_HMSYMBOL_TEXT, "HM"},
        {DrawCmdList::HybridRenderType::CANVAS, COLOR_CANVAS_BG, COLOR_CANVAS_SHAPE, "CV"}
    };

    for (size_t i = 0; i < types.size(); ++i) {
        float x = 50 + i * (RECORDING_WIDTH + 20);
        float y = 50;

        auto canvasNode = CreateColoredNode(x, y, RECORDING_WIDTH, RECORDING_HEIGHT, types[i].bgColor);
        auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

        DrawColoredText(recordingCanvas, types[i].label, types[i].contentColor, 50, 80);

        // Draw different shapes to enhance contrast
        if (types[i].type == DrawCmdList::HybridRenderType::SVG) {
            DrawColoredPath(recordingCanvas, types[i].contentColor);
        } else if (types[i].type == DrawCmdList::HybridRenderType::CANVAS) {
            DrawColoredRect(recordingCanvas, 100, 150, 300, 200, types[i].contentColor);
        } else {
            DrawColoredCircle(recordingCanvas, 200, 250, 80, types[i].contentColor);
        }

        recordingCanvas->GetDrawCmdList()->SetHybridRenderType(types[i].type);
        EXPECT_EQ(recordingCanvas->GetDrawCmdList()->GetHybridRenderType(), types[i].type);

        canvasNode->FinishRecording();
        RegisterNode(canvasNode);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

// ============================================================================
// GetBounds test - Use visual bounding boxes
// ============================================================================

/**
 * @tc.name: GetBounds_ValidRect_001
 * @tc.desc: Test valid bounds - Draw content and mark bounds with border
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, GetBounds_ValidRect_001)
{
    auto canvasNode = CreateColoredNode(50, 500, RECORDING_WIDTH, RECORDING_HEIGHT, 0xFFFFF8DC);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    // Draw content
    DrawColoredRect(recordingCanvas, 100, 100, 300, 250, 0xFFFF5722);
    DrawColoredText(recordingCanvas, "Bounds Test", 0xFFBF360C, 120, 200);

    // Get bounds
    Rect rect;
    recordingCanvas->GetDrawCmdList()->GetBounds(rect);

    // Draw border box (with dashed line effect)
    if (rect.IsValid()) {
        Pen pen;
        pen.SetColor(SK_ColorBLUE);
        pen.SetWidth(3.0f);
        recordingCanvas->AttachPen(pen);
        recordingCanvas->DrawRect(rect);
        recordingCanvas->DetachPen();
    }

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: GetBounds_EmptyContent_002
 * @tc.desc: Test empty content bounds - Only background without drawing
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, GetBounds_EmptyContent_002)
{
    auto canvasNode = CreateColoredNode(500, 500, RECORDING_WIDTH, RECORDING_HEIGHT, 0xFFEFEBE9);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    // Do not draw any content, only background
    Rect rect;
    recordingCanvas->GetDrawCmdList()->GetBounds(rect);
    EXPECT_FALSE(rect.IsValid());

    // Add explanatory text
    DrawColoredText(recordingCanvas, "Empty", 0xFFD32F2F, 150, 200);

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: GetBounds_MultiContent_003
 * @tc.desc: Test multiple content bounds - Multiple shape combination
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, GetBounds_MultiContent_003)
{
    auto canvasNode = CreateColoredNode(50, 950, RECORDING_WIDTH, RECORDING_HEIGHT, 0xFFE3F2FD);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    // Draw multiple scattered contents
    DrawColoredRect(recordingCanvas, 50, 50, 150, 100, 0xFF1976D2);
    DrawColoredCircle(recordingCanvas, 300, 100, 50, 0xFFE91E63);
    DrawColoredRect(recordingCanvas, 100, 250, 200, 80, 0xFF4CAF50);
    DrawColoredText(recordingCanvas, "Multi Bounds", 0xFF3F51B5, 100, 350);

    Rect rect;
    recordingCanvas->GetDrawCmdList()->GetBounds(rect);

    // Draw border box
    if (rect.IsValid()) {
        Pen pen(SK_ColorMAGENTA);
        pen.SetWidth(2.0f);
        recordingCanvas->AttachPen(pen);
        recordingCanvas->DrawRect(rect);
        recordingCanvas->DetachPen();
    }

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

// ============================================================================
// IsHybridRenderEnabled test - Enable/disable state visualization
// ============================================================================

/**
 * @tc.name: IsHybridRenderEnabled_Enabled_001
 * @tc.desc: Test enabled state - Green background indicates enabled
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, IsHybridRenderEnabled_Enabled_001)
{
    auto canvasNode = CreateColoredNode(50, 1400, RECORDING_WIDTH, RECORDING_HEIGHT, 0xFFC8E6C9);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(DrawCmdList::HybridRenderType::TEXT);
    bool isEnabled = recordingCanvas->GetDrawCmdList()->IsHybridRenderEnabled(
        MAX_PIXELMAP_WIDTH, MAX_PIXELMAP_HEIGHT);

    // Draw different content based on state
    if (isEnabled) {
        DrawColoredText(recordingCanvas, "ENABLED", 0xFF2E7D32, 130, 180);
        DrawColoredCircle(recordingCanvas, 200, 250, 60, 0xFF4CAF50);
    }

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: IsHybridRenderEnabled_SizeExceed_002
 * @tc.desc: Test size limit exceeded disable - Red background indicates disabled
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, IsHybridRenderEnabled_SizeExceed_002)
{
    auto canvasNode = CreateColoredNode(500, 1400, RECORDING_WIDTH, RECORDING_HEIGHT, 0xFFFFCDD2);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDTH, RECORDING_HEIGHT);

    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(DrawCmdList::HybridRenderType::CANVAS);
    recordingCanvas->ResetHybridRenderSize(6000.0f, 6000.0f);

    bool isEnabled = recordingCanvas->GetDrawCmdList()->IsHybridRenderEnabled(5000, 5000);

    // Draw different content based on state
    if (!isEnabled) {
        DrawColoredText(recordingCanvas, "DISABLED", 0xFFC62828, 110, 180);
        DrawColoredRect(recordingCanvas, 100, 250, 200, 50, 0xFFEF5350);
    }

    canvasNode->FinishRecording();
    RegisterNode(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

// ============================================================================
// ResetHybridRenderSize test - Size change visualization
// ============================================================================

/**
 * @tc.name: ResetHybridRenderSize_Compare_001
 * @tc.desc: Test size reset comparison - Left-right comparison of different sizes
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, ResetHybridRenderSize_Compare_001)
{
    // Left side: Normal size
    auto canvasNode1 = CreateColoredNode(50, 1850, 300, 300, 0xFFE1BEE7);
    auto recordingCanvas1 = canvasNode1->BeginRecording(300, 300);
    recordingCanvas1->ResetHybridRenderSize(200.0f, 200.0f);
    DrawColoredText(recordingCanvas1, "200x200", 0xFF6A1B9A, 70, 150);
    DrawColoredCircle(recordingCanvas1, 150, 220, 40, 0xFF8E24AA);
    canvasNode1->FinishRecording();
    RegisterNode(canvasNode1);

    // Right side: Large size
    auto canvasNode2 = CreateColoredNode(400, 1850, 300, 300, 0xFFB2DFDB);
    auto recordingCanvas2 = canvasNode2->BeginRecording(300, 300);
    recordingCanvas2->ResetHybridRenderSize(400.0f, 400.0f);
    DrawColoredText(recordingCanvas2, "400x400", 0xFF006064, 70, 150);
    DrawColoredCircle(recordingCanvas2, 150, 220, 40, 0xFF0097A7);
    canvasNode2->FinishRecording();
    RegisterNode(canvasNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/**
 * @tc.name: ResetHybridRenderSize_Grid_002
 * @tc.desc: Test multiple sizes grid layout display
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, ResetHybridRenderSize_Grid_002)
{
    struct SizeInfo {
        float w;
        float h;
        uint32_t color;
        std::string label;
    };

    std::vector<SizeInfo> sizes = {
        {100.0f, 100.0f, 0xFFFFCDD2, "100x100"},
        {200.0f, 150.0f, 0xFFF8BBD0, "200x150"},
        {250.0f, 250.0f, 0xFFE1BEE7, "250x250"},
        {300.0f, 200.0f, 0xFFD1C4E9, "300x200"}
    };

    for (size_t i = 0; i < sizes.size(); ++i) {
        float x = 50 + (i % 2) * 450;
        float y = 2200 + (i / 2) * 350;

        auto canvasNode = CreateColoredNode(x, y, 400, 350, sizes[i].color);
        auto recordingCanvas = canvasNode->BeginRecording(400, 350);

        recordingCanvas->ResetHybridRenderSize(sizes[i].w, sizes[i].h);
        DrawColoredText(recordingCanvas, sizes[i].label, 0xFF4A148C, 50, 100);

        // Draw size indicator box
        Pen pen;
        pen.SetColor(0xFF6A1B9A);
        pen.SetWidth(2.0f);
        recordingCanvas->AttachPen(pen);
        recordingCanvas->DrawRect(Rect(50, 150, 50 + sizes[i].w, 150 + sizes[i].h));
        recordingCanvas->DetachPen();

        canvasNode->FinishRecording();
        RegisterNode(canvasNode);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

// ============================================================================
// Combination test: Full display
// ============================================================================

/**
 * @tc.name: Combined_FullDisplay_001
 * @tc.desc: Full display: Comprehensive display of all types + states + sizes
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
GRAPHIC_TEST(HybridRenderInterfacesTest, HYBRID_RENDER_TEST, Combined_FullDisplay_001)
{
    struct DisplayInfo {
        DrawCmdList::HybridRenderType type;
        uint32_t bgColor;
        uint32_t contentColor;
        std::string label;
        float size;
    };

    std::vector<DisplayInfo> displays = {
        {DrawCmdList::HybridRenderType::NONE, COLOR_NONE_BG, COLOR_NONE_TEXT, "NONE", 100.0f},
        {DrawCmdList::HybridRenderType::TEXT, COLOR_TEXT_BG, COLOR_TEXT_TEXT, "TEXT", 150.0f},
        {DrawCmdList::HybridRenderType::SVG, COLOR_SVG_BG, COLOR_SVG_SHAPE, "SVG", 200.0f},
        {DrawCmdList::HybridRenderType::HMSYMBOL, COLOR_HMSYMBOL_BG, COLOR_HMSYMBOL_TEXT, "HM", 250.0f},
        {DrawCmdList::HybridRenderType::CANVAS, COLOR_CANVAS_BG, COLOR_CANVAS_SHAPE, "CV", 300.0f}
    };

    for (size_t i = 0; i < displays.size(); ++i) {
        float x = 50 + i * 270;
        float y = 3000;

        auto canvasNode = CreateColoredNode(x, y, 250, 350, displays[i].bgColor);
        auto recordingCanvas = canvasNode->BeginRecording(250, 350);

        DrawColoredText(recordingCanvas, displays[i].label, displays[i].contentColor, 80, 50);

        // Draw size indicator circle
        DrawColoredCircle(recordingCanvas, 125, 150, displays[i].size * 0.3f, displays[i].contentColor);

        // Draw state indicator
        recordingCanvas->GetDrawCmdList()->SetHybridRenderType(displays[i].type);
        bool isEnabled = recordingCanvas->GetDrawCmdList()->IsHybridRenderEnabled(5000, 5000);

        if (isEnabled) {
            DrawColoredRect(recordingCanvas, 50, 250, 200, 10, 0xFF4CAF50);
        } else {
            DrawColoredRect(recordingCanvas, 50, 250, 200, 10, 0xFFF44336);
        }

        canvasNode->FinishRecording();
        RegisterNode(canvasNode);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
