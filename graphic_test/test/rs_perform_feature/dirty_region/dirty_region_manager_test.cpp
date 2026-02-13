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

class DirtyRegionTest10 : public RSGraphicTest {
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
};
} //namespace

// ============================================================================
// IsCurrentFrameDirty tests
// ============================================================================

/*
 * @tc.name: IsCurrentFrameDirty001
 * @tc.desc: test IsCurrentFrameDirty - empty region
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsCurrentFrameDirty001)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    EXPECT_FALSE(rsDirtyManager->IsCurrentFrameDirty());
}

/*
 * @tc.name: IsCurrentFrameDirty002
 * @tc.desc: test IsCurrentFrameDirty - single dirty rect
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsCurrentFrameDirty002)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 400, 400};

    rsDirtyManager->MergeDirtyRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsCurrentFrameDirty());

    DrawRectI(rect1, COLOR_YELLOW);
}

/*
 * @tc.name: IsCurrentFrameDirty003
 * @tc.desc: test IsCurrentFrameDirty - after Clear()
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsCurrentFrameDirty003)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 400, 400};

    rsDirtyManager->MergeDirtyRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsCurrentFrameDirty());

    rsDirtyManager->Clear();
    EXPECT_FALSE(rsDirtyManager->IsCurrentFrameDirty());

    DrawRectI(rect1, COLOR_YELLOW);
}

/*
 * @tc.name: IsCurrentFrameDirty004
 * @tc.desc: test IsCurrentFrameDirty - after UpdateDirty()
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsCurrentFrameDirty004)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 400, 400};
    rsDirtyManager->MergeDirtyRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsCurrentFrameDirty());

    rsDirtyManager->UpdateDirty();
    EXPECT_TRUE(rsDirtyManager->IsCurrentFrameDirty());

    DrawRectI(rect1, COLOR_YELLOW);
}

// ============================================================================
// IsDirty tests
// ============================================================================

/*
 * @tc.name: IsDirty001
 * @tc.desc: test IsDirty - empty region
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsDirty001)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    EXPECT_FALSE(rsDirtyManager->IsDirty());
}

/*
 * @tc.name: IsDirty002
 * @tc.desc: test IsDirty - after MergeDirtyRect
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsDirty002)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 400, 400};

    rsDirtyManager->MergeDirtyRect(rect1);
    EXPECT_FALSE(rsDirtyManager->IsDirty()); // Not dirty until UpdateDirty()

    DrawRectI(rect1, COLOR_YELLOW);
}

/*
 * @tc.name: IsDirty003
 * @tc.desc: test IsDirty - after UpdateDirty
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsDirty003)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 400, 400};
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();
    EXPECT_TRUE(rsDirtyManager->IsDirty());

    DrawRectI(rect1, COLOR_YELLOW);
}

/*
 * @tc.name: IsDirty004
 * @tc.desc: test IsDirty - after Clear
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsDirty004)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 400, 400};
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();
    EXPECT_TRUE(rsDirtyManager->IsDirty());

    rsDirtyManager->Clear();
    EXPECT_FALSE(rsDirtyManager->IsDirty());

    DrawRectI(rect1, COLOR_YELLOW);
}

// ============================================================================
// UpdateDirtyByAligned tests
// ============================================================================

/*
 * @tc.name: UpdateDirtyByAligned001
 * @tc.desc: test UpdateDirtyByAligned - default alignment
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, UpdateDirtyByAligned001)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 350, 350}; // Not aligned to 32
    rsDirtyManager->MergeDirtyRect(rect1);

    RectI beforeAlign = rsDirtyManager->GetCurrentFrameDirtyRegion();
    rsDirtyManager->UpdateDirtyByAligned();

    RectI afterAlign = rsDirtyManager->GetCurrentFrameDirtyRegion();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(beforeAlign, COLOR_RED, DEFAULT_TRANS_Y);
    DrawRectI(afterAlign, COLOR_GREEN, DEFAULT_TRANS_Y_2);

    // After alignment, width and height should be multiples of 32
    EXPECT_EQ(afterAlign.width_ % 32, 0);
    EXPECT_EQ(afterAlign.height_ % 32, 0);
}

/*
 * @tc.name: UpdateDirtyByAligned002
 * @tc.desc: test UpdateDirtyByAligned - custom alignment bits
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, UpdateDirtyByAligned002)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 350, 350};
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirtyByAligned(64); // Align to 64

    RectI afterAlign = rsDirtyManager->GetCurrentFrameDirtyRegion();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(afterAlign, COLOR_RED, DEFAULT_TRANS_Y);

    EXPECT_EQ(afterAlign.width_ % 64, 0);
    EXPECT_EQ(afterAlign.height_ % 64, 0);
}

/*
 * @tc.name: UpdateDirtyByAligned003
 * @tc.desc: test UpdateDirtyByAligned - already aligned rect
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, UpdateDirtyByAligned003)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {96, 96, 320, 320}; // Already aligned to 32
    rsDirtyManager->MergeDirtyRect(rect1);

    RectI beforeAlign = rsDirtyManager->GetCurrentFrameDirtyRegion();
    rsDirtyManager->UpdateDirtyByAligned();

    RectI afterAlign = rsDirtyManager->GetCurrentFrameDirtyRegion();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(beforeAlign, COLOR_RED, DEFAULT_TRANS_Y);
    DrawRectI(afterAlign, COLOR_GREEN, DEFAULT_TRANS_Y_2);

    EXPECT_EQ(beforeAlign, afterAlign);
}

// ============================================================================
// IsActiveSurfaceRectChanged tests
// ============================================================================

/*
 * @tc.name: IsActiveSurfaceRectChanged001
 * @tc.desc: test IsActiveSurfaceRectChanged - initial state
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsActiveSurfaceRectChanged001)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    EXPECT_FALSE(rsDirtyManager->IsActiveSurfaceRectChanged());
}

/*
 * @tc.name: IsActiveSurfaceRectChanged002
 * @tc.desc: test IsActiveSurfaceRectChanged - after SetActiveSurfaceRect
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsActiveSurfaceRectChanged002)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 400, 400};

    rsDirtyManager->SetActiveSurfaceRect(rect1);
    EXPECT_FALSE(rsDirtyManager->IsActiveSurfaceRectChanged());

    DrawRectI(rect1, COLOR_YELLOW);
}

/*
 * @tc.name: IsActiveSurfaceRectChanged003
 * @tc.desc: test IsActiveSurfaceRectChanged - same rect twice
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsActiveSurfaceRectChanged003)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 400, 400};
    rsDirtyManager->SetActiveSurfaceRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsActiveSurfaceRectChanged());

    rsDirtyManager->SetActiveSurfaceRect(rect1);
    EXPECT_FALSE(rsDirtyManager->IsActiveSurfaceRectChanged());

    DrawRectI(rect1, COLOR_YELLOW);
}

/*
 * @tc.name: IsActiveSurfaceRectChanged004
 * @tc.desc: test IsActiveSurfaceRectChanged - different rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsActiveSurfaceRectChanged004)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 400, 400};
    RectI rect2 = {200, 200, 500, 500};

    rsDirtyManager->SetActiveSurfaceRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsActiveSurfaceRectChanged());

    rsDirtyManager->SetActiveSurfaceRect(rect2);
    EXPECT_TRUE(rsDirtyManager->IsActiveSurfaceRectChanged());

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
}

// ============================================================================
// IsSurfaceRectChanged tests
// ============================================================================

/*
 * @tc.name: IsSurfaceRectChanged001
 * @tc.desc: test IsSurfaceRectChanged - initial state
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsSurfaceRectChanged001)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    EXPECT_FALSE(rsDirtyManager->IsSurfaceRectChanged());
}

/*
 * @tc.name: IsSurfaceRectChanged002
 * @tc.desc: test IsSurfaceRectChanged - after SetSurfaceRect
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsSurfaceRectChanged002)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {0, 0, 1200, 2000};

    rsDirtyManager->SetSurfaceRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsSurfaceRectChanged());

    DrawRectI(rect1, COLOR_YELLOW);
}

/*
 * @tc.name: IsSurfaceRectChanged003
 * @tc.desc: test IsSurfaceRectChanged - same rect twice
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsSurfaceRectChanged003)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {0, 0, 1200, 2000};

    rsDirtyManager->SetSurfaceRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsSurfaceRectChanged());

    rsDirtyManager->SetSurfaceRect(rect1);
    EXPECT_FALSE(rsDirtyManager->IsSurfaceRectChanged());

    DrawRectI(rect1, COLOR_YELLOW);
}

/*
 * @tc.name: IsSurfaceRectChanged004
 * @tc.desc: test IsSurfaceRectChanged - different rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsSurfaceRectChanged004)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {0, 0, 1200, 2000};
    RectI rect2 = {0, 0, 1920, 1080};

    rsDirtyManager->SetSurfaceRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsSurfaceRectChanged());

    rsDirtyManager->SetSurfaceRect(rect2);
    EXPECT_TRUE(rsDirtyManager->IsSurfaceRectChanged());

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE, DEFAULT_TRANS_Y);
}

/*
 * @tc.name: IsSurfaceRectChanged005
 * @tc.desc: test IsSurfaceRectChanged - empty rect
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IsSurfaceRectChanged005)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {0, 0, 1200, 2000};
    RectI emptyRect = {0, 0, 0, 0};

    rsDirtyManager->SetSurfaceRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsSurfaceRectChanged());

    // Empty rect should not change surface rect
    bool result = rsDirtyManager->SetSurfaceRect(emptyRect);
    EXPECT_FALSE(result);

    DrawRectI(rect1, COLOR_YELLOW);
}

// ============================================================================
// ResetDirtyAsSurfaceSize tests
// ============================================================================

/*
 * @tc.name: ResetDirtyAsSurfaceSize001
 * @tc.desc: test ResetDirtyAsSurfaceSize - basic functionality
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, ResetDirtyAsSurfaceSize001)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 400, 400};
    rsDirtyManager->MergeDirtyRect(rect1);

    RectI beforeReset = rsDirtyManager->GetCurrentFrameDirtyRegion();
    rsDirtyManager->ResetDirtyAsSurfaceSize();

    RectI afterReset = rsDirtyManager->GetCurrentFrameDirtyRegion();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(beforeReset, COLOR_RED, DEFAULT_TRANS_Y);
    DrawRectI(afterReset, COLOR_GREEN, DEFAULT_TRANS_Y_2);

    // After reset, should be full surface size
    EXPECT_EQ(afterReset.width_, size.x_);
    EXPECT_EQ(afterReset.height_, size.y_);
}

/*
 * @tc.name: ResetDirtyAsSurfaceSize002
 * @tc.desc: test ResetDirtyAsSurfaceSize - multiple dirty rects
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, ResetDirtyAsSurfaceSize002)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 200, 400, 400};
    RectI rect3 = {800, 600, 200, 200};

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->MergeDirtyRect(rect3);

    rsDirtyManager->ResetDirtyAsSurfaceSize();
    RectI afterReset = rsDirtyManager->GetCurrentFrameDirtyRegion();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
    DrawRectI(rect3, COLOR_GREEN);
    DrawRectI(afterReset, COLOR_RED, DEFAULT_TRANS_Y);

    EXPECT_EQ(afterReset.width_, size.x_);
    EXPECT_EQ(afterReset.height_, size.y_);
}

/*
 * @tc.name: ResetDirtyAsSurfaceSize003
 * @tc.desc: test ResetDirtyAsSurfaceSize - empty region
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, ResetDirtyAsSurfaceSize003)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    EXPECT_FALSE(rsDirtyManager->IsCurrentFrameDirty());

    rsDirtyManager->ResetDirtyAsSurfaceSize();
    EXPECT_TRUE(rsDirtyManager->IsCurrentFrameDirty());

    RectI afterReset = rsDirtyManager->GetCurrentFrameDirtyRegion();
    DrawRectI(afterReset, COLOR_YELLOW);

    EXPECT_EQ(afterReset.width_, size.x_);
    EXPECT_EQ(afterReset.height_, size.y_);
}

// ============================================================================
// Combination tests for MergeDirtyRect + IntersectDirtyRect
// ============================================================================

/*
 * @tc.name: MergeAndIntersect001
 * @tc.desc: test combination of MergeDirtyRect and IntersectDirtyRect
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, MergeAndIntersect001)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();

    RectI rect1 = {100, 100, 400, 400};
    RectI rect2 = {200, 200, 300, 300};
    RectI intersect = {300, 300, 200, 200};

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    RectI beforeIntersect = rsDirtyManager->GetCurrentFrameDirtyRegion();

    rsDirtyManager->IntersectDirtyRect(intersect);
    RectI afterIntersect = rsDirtyManager->GetCurrentFrameDirtyRegion();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
    DrawRectI(beforeIntersect, COLOR_RED, DEFAULT_TRANS_Y);
    DrawRectI(afterIntersect, COLOR_GREEN, DEFAULT_TRANS_Y_2);
}

/*
 * @tc.name: MergeAndIntersect002
 * @tc.desc: test MergeDirtyRect followed by IntersectDirtyRect with no overlap
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, MergeAndIntersect002)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();

    RectI rect1 = {100, 100, 400, 400};
    RectI farRect = {1000, 1000, 200, 200}; // Far away, no overlap

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->IntersectDirtyRect(farRect);
    RectI result = rsDirtyManager->GetCurrentFrameDirtyRegion();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(farRect, COLOR_BLUE);
    DrawRectI(result, COLOR_RED, DEFAULT_TRANS_Y);

    // Result should be empty or very small due to no intersection
    EXPECT_TRUE(result.IsEmpty() || result.width_ < rect1.width_);
}

// ============================================================================
// Combination tests for UpdateDirty + IsDirty
// ============================================================================

/*
 * @tc.name: UpdateDirtyAndIsDirty001
 * @tc.desc: test UpdateDirty followed by IsDirty check
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, UpdateDirtyAndIsDirty001)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 400, 400};

    // Before any operation
    EXPECT_FALSE(rsDirtyManager->IsCurrentFrameDirty());
    EXPECT_FALSE(rsDirtyManager->IsDirty());

    // After merge
    rsDirtyManager->MergeDirtyRect(rect1);
    EXPECT_TRUE(rsDirtyManager->IsCurrentFrameDirty());
    EXPECT_FALSE(rsDirtyManager->IsDirty());

    // After update
    rsDirtyManager->UpdateDirty();
    EXPECT_TRUE(rsDirtyManager->IsCurrentFrameDirty());
    EXPECT_TRUE(rsDirtyManager->IsDirty());

    DrawRectI(rect1, COLOR_YELLOW);
}

/*
 * @tc.name: UpdateDirtyAndIsDirty002
 * @tc.desc: test UpdateDirty with multiple frames
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, UpdateDirtyAndIsDirty002)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});
    rsDirtyManager->SetBufferAge(2);

    // Frame 1
    RectI rect1 = {100, 100, 400, 400};
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();

    // Frame 2
    RectI rect2 = {500, 200, 300, 300};
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->UpdateDirty();

    // Should merge history due to bufferAge=2
    EXPECT_TRUE(rsDirtyManager->IsDirty());

    RectI dirtyRegion = rsDirtyManager->GetDirtyRegion();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(rect2, COLOR_BLUE);
    DrawRectI(dirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
}

// ============================================================================
// Combination tests for IntersectDirtyRect + ResetDirtyAsSurfaceSize
// ============================================================================

/*
 * @tc.name: IntersectAndReset001
 * @tc.desc: test IntersectDirtyRect followed by ResetDirtyAsSurfaceSize
 * @tc.type: FUNC
 * @tc.require: issueIC0Y3E
 */
GRAPHIC_TEST(DirtyRegionTest10, CONTENT_DISPLAY_TEST, IntersectAndReset001)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});

    RectI rect1 = {100, 100, 800, 800};
    RectI smallRect = {200, 200, 200, 200};

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->IntersectDirtyRect(smallRect);

    RectI beforeReset = rsDirtyManager->GetCurrentFrameDirtyRegion();
    rsDirtyManager->ResetDirtyAsSurfaceSize();

    RectI afterReset = rsDirtyManager->GetCurrentFrameDirtyRegion();

    DrawRectI(rect1, COLOR_YELLOW);
    DrawRectI(smallRect, COLOR_BLUE);
    DrawRectI(beforeReset, COLOR_RED, DEFAULT_TRANS_Y);
    DrawRectI(afterReset, COLOR_GREEN, DEFAULT_TRANS_Y_2);

    // Before reset should be small (intersection)
    EXPECT_TRUE(beforeReset.width_ <= smallRect.width_);

    // After reset should be full surface
    EXPECT_EQ(afterReset.width_, size.x_);
    EXPECT_EQ(afterReset.height_, size.y_);
}

} // namespace OHOS::Rosen
