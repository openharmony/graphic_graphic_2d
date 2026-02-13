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
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_utils.h"

#include "pipeline/rs_dirty_region_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int DEFAULT_TRANS_Y = 1000;
constexpr int DEFAULT_TRANS_Y_2 = 1500;
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
constexpr uint32_t COLOR_GREEN = 0xFF00FF00;
constexpr uint32_t COLOR_CYAN = 0xFF00FFFF;

class DirtyRegionTest09 : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        SetSurfaceColor(RSColor(0xFFFFFFFF));
    }

    void AfterEach() override {}

    // Helper function to draw RectI as colored rectangle
    void DrawRectI(const RectI& rect, uint32_t color, int transY = 0)
    {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({rect.left_, rect.top_ + transY, rect.width_, rect.height_});
        testNode->SetBackgroundColor(color);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }

    // Helper function to draw multiple RectI as colored rectangles
    void DrawAdvancedDirtyRegion(
        const std::vector<RectI>& advancedDirtyRegion, uint32_t backgroundColor, int transY = 0)
    {
        for (const RectI& rect : advancedDirtyRegion) {
            DrawRectI(rect, backgroundColor, transY);
        }
    }
};
} //namespace

/*
 * @tc.name: ExpandDamageRegionToSingleRect001
 * @tc.desc: test expand single dirty region - empty region
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, ExpandDamageRegionToSingleRect001)
{
    // Create empty dirty region scenario
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 200, 300, 300};

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode1->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode2->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode2);

    RegisterNode(testNode1);
    RegisterNode(testNode2);
}

/*
 * @tc.name: ExpandDamageRegionToSingleRect002
 * @tc.desc: test expand single dirty region - single rect no expansion
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, ExpandDamageRegionToSingleRect002)
{
    // Single dirty region - no expansion needed
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 400, 400};
    rsDirtyManager->MergeDirtyRect(rect1);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();

    // Draw original rect
    DrawRectI(rect1, COLOR_YELLOW);
    // Draw result (should be same)
    DrawRectI(curDirtyRect, COLOR_RED, DEFAULT_TRANS_Y);

    EXPECT_TRUE(curDirtyRect.width_ == rect1.width_);
    EXPECT_TRUE(curDirtyRect.height_ == rect1.height_);
}

/*
 * @tc.name: ExpandDamageRegionToSingleRect003
 * @tc.desc: test expand single dirty region - two overlapping rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, ExpandDamageRegionToSingleRect003)
{
    // Two overlapping rects - should expand to single rect
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 400, 400};
    RectI rect2 = {200, 200, 500, 500};

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();

    // Draw original rects
    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
    // Draw merged result
    DrawRectI(curDirtyRect, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: ExpandDamageRegionToSingleRect004
 * @tc.desc: test expand single dirty region - two separated rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, ExpandDamageRegionToSingleRect004)
{
    // Two non-overlapping rects
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 100, 300, 300};

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();

    // Draw original rects
    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
    // Draw merged result (expanded to cover both)
    DrawRectI(curDirtyRect, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: ExpandDamageRegionToSingleRect005
 * @tc.desc: test expand single dirty region - three scattered rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, ExpandDamageRegionToSingleRect005)
{
    // Three scattered rects
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 200, 200};
    RectI rect2 = {400, 100, 200, 200};
    RectI rect3 = {700, 100, 200, 200};

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->MergeDirtyRect(rect3);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();

    // Draw original rects
    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
    DrawRectI(rect3, COLOR_GREEN);
    // Draw expanded result
    DrawRectI(curDirtyRect, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyRects001
 * @tc.desc: test merge dirty rects - empty region
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects001)
{
    // Empty dirty region scenario
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(10);

    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();
    EXPECT_TRUE(curAdvancedDirtyRegion.empty());
}

/*
 * @tc.name: MergeDirtyRects002
 * @tc.desc: test merge dirty rects - single rect
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects002)
{
    // Single rect
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    RectI rect1 = {100, 100, 400, 400};

    DrawRectI(rect1, COLOR_YELLOW);

    rsDirtyManager->MergeDirtyRect(rect1);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();

    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyRects003
 * @tc.desc: test merge dirty rects - two overlapping rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects003)
{
    // Two overlapping rects
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    RectI rect1 = {100, 100, 400, 400};
    RectI rect2 = {200, 200, 500, 500};

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();

    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyRects004
 * @tc.desc: test merge dirty rects - two adjacent rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects004)
{
    // Two adjacent rects
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {400, 100, 300, 300};

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();

    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyRects005
 * @tc.desc: test merge dirty rects - multiple scattered rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects005)
{
    // Four scattered rects
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    RectI rect1 = {100, 100, 200, 200};
    RectI rect2 = {400, 100, 200, 200};
    RectI rect3 = {250, 400, 200, 200};
    RectI rect4 = {100, 700, 200, 200};

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
    DrawRectI(rect3, COLOR_GREEN);
    DrawRectI(rect4, COLOR_CYAN);

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->MergeDirtyRect(rect3);
    rsDirtyManager->MergeDirtyRect(rect4);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();

    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyRects006
 * @tc.desc: test merge dirty rects - three overlapping rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects006)
{
    // Three overlapping rects in diagonal
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    RectI rect1 = {100, 100, 400, 400};
    RectI rect2 = {200, 200, 400, 400};
    RectI rect3 = {300, 300, 400, 400};

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
    DrawRectI(rect3, COLOR_GREEN);

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->MergeDirtyRect(rect3);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();

    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyRects007
 * @tc.desc: test merge dirty rects - L-shape pattern
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects007)
{
    // L-shape pattern
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    RectI rect1 = {100, 100, 400, 200};
    RectI rect2 = {100, 300, 200, 400};

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();

    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyRects008
 * @tc.desc: test merge dirty rects with max limit
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects008)
{
    // Multiple rects but limit to 2
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(2);
    RectI rect1 = {100, 100, 200, 200};
    RectI rect2 = {400, 100, 200, 200};
    RectI rect3 = {250, 400, 200, 200};
    RectI rect4 = {100, 700, 200, 200};

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
    DrawRectI(rect3, COLOR_GREEN);
    DrawRectI(rect4, COLOR_CYAN);

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->MergeDirtyRect(rect3);
    rsDirtyManager->MergeDirtyRect(rect4);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();

    // Should be limited to 2 rects
    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyRects009
 * @tc.desc: test merge dirty rects - diagonal pattern
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects009)
{
    // Diagonal pattern
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 500, 300, 300};

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();

    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyRects010
 * @tc.desc: test merge dirty rects - cross pattern
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyRects010)
{
    // Cross pattern (horizontal + vertical)
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    RectI rect1 = {100, 300, 800, 200};
    RectI rect2 = {400, 100, 200, 800};

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();

    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyHistory001
 * @tc.desc: test merge dirty history with bufferAge 0 - full redraw
 * @tc.type: FUNC
 * @tc.require: issueICF6LO
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyHistory001)
{
    // bufferAge = 0 means full redraw
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetBufferAge(0);
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 300, 300};
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();
    RectI dirtyRegion = rsDirtyManager->GetDirtyRegion();

    // Should be full screen or larger
    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(dirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: MergeDirtyHistory002
 * @tc.desc: test merge dirty history with bufferAge 1 - current frame only
 * @tc.type: FUNC
 * @tc.require: issueICF6LO
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyHistory002)
{
    // bufferAge = 1 means only current frame dirty regions
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetBufferAge(1);
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 300, 300};
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();
    RectI dirtyRegion = rsDirtyManager->GetDirtyRegion();
    rsDirtyManager->Clear();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(dirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);

    // Add second dirty region
    RectI rect2 = {500, 200, 300, 300};
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->UpdateDirty();
    dirtyRegion = rsDirtyManager->GetDirtyRegion();

    DrawRectI(rect2, COLOR_BLUE);
    DrawRectI(dirtyRegion, COLOR_GREEN, DEFAULT_TRANS_Y_2);
}

/*
 * @tc.name: MergeDirtyHistory003
 * @tc.desc: test merge dirty history with bufferAge 2 - merge with previous
 * @tc.type: FUNC
 * @tc.require: issueICF6LO
 */
GRAPHIC_TEST(DirtyRegionTest09, CONTENT_DISPLAY_TEST, MergeDirtyHistory003)
{
    // bufferAge = 2 means merge current and previous frame dirty regions
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetBufferAge(2);
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 300, 300};
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();
    RectI dirtyRegion = rsDirtyManager->GetDirtyRegion();
    rsDirtyManager->Clear();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(dirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);

    // Second frame
    RectI rect2 = {500, 200, 300, 300};
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->UpdateDirty();
    dirtyRegion = rsDirtyManager->GetDirtyRegion();

    DrawRectI(rect2, COLOR_BLUE);
    DrawRectI(dirtyRegion, COLOR_GREEN, DEFAULT_TRANS_Y_2);
}

} // namespace OHOS::Rosen
