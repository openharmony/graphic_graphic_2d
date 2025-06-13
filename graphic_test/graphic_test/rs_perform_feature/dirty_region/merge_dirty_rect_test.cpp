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
#include <filesystem>

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_utils.h"

#include "pipeline/rs_dirty_region_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int DEFAULT_TRANS_Y = 1000;
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
constexpr uint32_t COLOR_DKGRAY = 0xFF444444;
constexpr uint32_t COLOR_CYAN = 0xFF00FFFF;

class DirtyRegionTest01 : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        auto size = GetScreenSize();
        SetSurfaceBounds({0, 0, size.x_, size.y_});
        SetSurfaceColor(RSColor(0xFFFFFFFF));
    }

    void DrawAdvancedDirtyRegion(
        const std::vector<RectI>& advancedDirtyRegion, uint32_t backgroundColor, int transY = 0)
    {
        for (const RectI& rect : advancedDirtyRegion) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({rect.left_, rect.top_ + transY, rect.width_, rect.height_});
            testNode->SetBackgroundColor(backgroundColor);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
};
} //namespace

/*
 * @tc.name: Merge_Dirty_Rect_Test01
 * @tc.desc: test currentFrameDirtyRegion, rects are not intersected
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_Test01)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
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

    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();

    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds(
        {curDirtyRect.left_, curDirtyRect.top_ + DEFAULT_TRANS_Y, curDirtyRect.width_, curDirtyRect.height_});
    testNode3->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode3);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
}

/*
 * @tc.name: Merge_Dirty_Rect_Test02
 * @tc.desc: test currentFrameDirtyRegion, rects are intersected
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_Test02)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {300, 200, 300, 300};
    
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode1->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode2->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode2);
    
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    
    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds(
        {curDirtyRect.left_, curDirtyRect.top_ + DEFAULT_TRANS_Y, curDirtyRect.width_, curDirtyRect.height_});
    testNode3->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode3);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
}

/*
 * @tc.name: Merge_Dirty_Rect_Test03
 * @tc.desc: test currentFrameDirtyRegion, rects are out of surface
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_Test03)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {-100, 100, 300, 300};
    RectI rect2 = {500, 200, 300, 300};

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode1->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode2->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode2);
    
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    
    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds(
        {curDirtyRect.left_, curDirtyRect.top_ + DEFAULT_TRANS_Y, curDirtyRect.width_, curDirtyRect.height_});
    testNode3->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode3);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
}

/*
 * @tc.name: Merge_Dirty_Rect_Test04
 * @tc.desc: test currentFrameDirtyRegion, merge three rects
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_Test04)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 200, 300, 300};
    RectI rect3 = {900, 300, 300, 300};

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode1->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode2->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode2);

    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds({rect3.left_, rect3.top_, rect3.width_, rect3.height_});
    testNode3->SetBackgroundColor(COLOR_DKGRAY);
    GetRootNode()->AddChild(testNode3);
    
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->MergeDirtyRect(rect3);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    
    auto testNode4 = RSCanvasNode::Create();
    testNode4->SetBounds(
        {curDirtyRect.left_, curDirtyRect.top_ + DEFAULT_TRANS_Y, curDirtyRect.width_, curDirtyRect.height_});
    testNode4->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode4);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
    RegisterNode(testNode4);
}

/*
 * @tc.name: Merge_Dirty_Rect_Test05
 * @tc.desc: test currentFrameAdvancedDirtyRegion, merge three rects
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_Test05)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 200, 300, 300};
    RectI rect3 = {900, 300, 300, 300};

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode1->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode2->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode2);

    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds({rect3.left_, rect3.top_, rect3.width_, rect3.height_});
    testNode3->SetBackgroundColor(COLOR_DKGRAY);
    GetRootNode()->AddChild(testNode3);

    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->MergeDirtyRect(rect3);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();
    
    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
}

/*
 * @tc.name: Merge_Dirty_Rect_Test06
 * @tc.desc: test currentFrameAdvancedDirtyRegion, merge four rects but oversize
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_Test06)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 200, 300, 300};
    RectI rect3 = {900, 300, 300, 300};
    RectI rect4 = {100, 700, 300, 300};

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode1->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode2->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode2);

    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds({rect3.left_, rect3.top_, rect3.width_, rect3.height_});
    testNode3->SetBackgroundColor(COLOR_DKGRAY);
    GetRootNode()->AddChild(testNode3);

    auto testNode4 = RSCanvasNode::Create();
    testNode4->SetBounds({rect4.left_, rect4.top_, rect4.width_, rect4.height_});
    testNode4->SetBackgroundColor(COLOR_CYAN);
    GetRootNode()->AddChild(testNode4);

    rsDirtyManager->SetMaxNumOfDirtyRects(2);
    
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->MergeDirtyRect(rect3);
    rsDirtyManager->MergeDirtyRect(rect4);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();
    
    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
    RegisterNode(testNode4);
}

/*
 * @tc.name: Intersect_Dirty_Rect_Test01
 * @tc.desc: test currentFrameDirtyRegion, intersect two rects
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Intersect_Dirty_Rect_Test01)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 500, 500};
    RectI rect2 = {300, 100, 500, 500};

    Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
    Vector4f borderWidth = { 10, 10, 10, 10 };

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode1->SetBorderStyle(style);
    testNode1->SetBorderWidth(borderWidth);
    testNode1->SetBorderColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode2->SetBorderStyle(style);
    testNode2->SetBorderWidth(borderWidth);
    testNode2->SetBorderColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode2);
    
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->IntersectDirtyRect(rect2);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    
    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds(
        {curDirtyRect.left_, curDirtyRect.top_ + DEFAULT_TRANS_Y, curDirtyRect.width_, curDirtyRect.height_});
    testNode3->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode3);

    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
}

/*
 * @tc.name: Intersect_Dirty_Rect_Test02
 * @tc.desc: test currentFrameAdvancedDirtyRegion, intersect two rects
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Intersect_Dirty_Rect_Test02)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI rect1 = {100, 100, 500, 500};
    RectI rect2 = {700, 100, 500, 500};
    RectI rect3 = {400, 100, 500, 500};

    Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
    Vector4f borderWidth = { 10, 10, 10, 10 };

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode1->SetBorderStyle(style);
    testNode1->SetBorderWidth(borderWidth);
    testNode1->SetBorderColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode2->SetBorderStyle(style);
    testNode2->SetBorderWidth(borderWidth);
    testNode2->SetBorderColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode2);

    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds({rect3.left_, rect3.top_, rect3.width_, rect3.height_});
    testNode3->SetBorderStyle(style);
    testNode3->SetBorderWidth(borderWidth);
    testNode3->SetBorderColor(COLOR_DKGRAY);
    GetRootNode()->AddChild(testNode3);

    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->IntersectDirtyRect(rect3);

    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();
    
    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);

    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
}
} // namespace OHOS::Rosen