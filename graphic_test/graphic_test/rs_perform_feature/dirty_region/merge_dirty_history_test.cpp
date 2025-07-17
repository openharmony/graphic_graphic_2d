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
constexpr int DEFAULT_TRANS_Y_2 = 1500;
constexpr int DEFAULT_TRANS_Y_3 = 2000;
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
constexpr uint32_t COLOR_DKGRAY = 0xFF444444;
constexpr uint32_t COLOR_GREEN = 0xFF00FF00;

class DirtyRegionTest02 : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
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
 * @tc.name: Update_Dirty_Test01
 * @tc.desc: test updateDirty with 0 buffer age, draw dirtyRegion_
 * @tc.type: FUNC
 * @tc.require: issueICF6LO
 */
GRAPHIC_TEST(DirtyRegionTest02, CONTENT_DISPLAY_TEST, Update_Dirty_Test01)
{
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 200, 300, 300};

    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetBufferAge(0);
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();
    RectI dirtyRegion = rsDirtyManager->GetDirtyRegion();

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds(
        {dirtyRegion.left_, dirtyRegion.top_, dirtyRegion.width_, dirtyRegion.height_});
    testNode1->SetBackgroundColor(COLOR_GREEN);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode2->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode2);

    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode3->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode3);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
}

/*
 * @tc.name: Update_Dirty_Test02
 * @tc.desc: test updateDirty with 1 buffer age, draw dirtyRegion_
 * @tc.type: FUNC
 * @tc.require: issueICF6LO
 */
GRAPHIC_TEST(DirtyRegionTest02, CONTENT_DISPLAY_TEST, Update_Dirty_Test02)
{
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 200, 300, 300};
    RectI rect3 = {900, 300, 300, 300};

    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetBufferAge(1);
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();
    RectI dirtyRegion = rsDirtyManager->GetDirtyRegion();
    rsDirtyManager->Clear();

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
    testNode4->SetBounds(
        {dirtyRegion.left_, dirtyRegion.top_ + DEFAULT_TRANS_Y, dirtyRegion.width_, dirtyRegion.height_});
    testNode4->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode4);

    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->UpdateDirty();
    dirtyRegion = rsDirtyManager->GetDirtyRegion();
    rsDirtyManager->Clear();

    auto testNode5 = RSCanvasNode::Create();
    testNode5->SetBounds(
        {dirtyRegion.left_, dirtyRegion.top_ + DEFAULT_TRANS_Y_2, dirtyRegion.width_, dirtyRegion.height_});
    testNode5->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode5);

    rsDirtyManager->MergeDirtyRect(rect3);
    rsDirtyManager->UpdateDirty();
    dirtyRegion = rsDirtyManager->GetDirtyRegion();
    rsDirtyManager->Clear();

    auto testNode6 = RSCanvasNode::Create();
    testNode6->SetBounds(
        {dirtyRegion.left_, dirtyRegion.top_ + DEFAULT_TRANS_Y_3, dirtyRegion.width_, dirtyRegion.height_});
    testNode6->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode6);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
    RegisterNode(testNode4);
    RegisterNode(testNode5);
    RegisterNode(testNode6);
}

/*
 * @tc.name: Update_Dirty_Test03
 * @tc.desc: test updateDirty with 2 buffer age, draw dirtyRegion_
 * @tc.type: FUNC
 * @tc.require: issueICF6LO
 */
GRAPHIC_TEST(DirtyRegionTest02, CONTENT_DISPLAY_TEST, Update_Dirty_Test03)
{
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 200, 300, 300};
    RectI rect3 = {900, 300, 300, 300};

    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetBufferAge(2);
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();
    RectI dirtyRegion = rsDirtyManager->GetDirtyRegion();
    rsDirtyManager->Clear();

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds(
        {dirtyRegion.left_, dirtyRegion.top_, dirtyRegion.width_, dirtyRegion.height_});
    testNode1->SetBackgroundColor(COLOR_GREEN);
    GetRootNode()->AddChild(testNode1);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({rect1.left_, rect1.top_, rect1.width_, rect1.height_});
    testNode2->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(testNode2);

    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetBounds({rect2.left_, rect2.top_, rect2.width_, rect2.height_});
    testNode3->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChild(testNode3);

    auto testNode4 = RSCanvasNode::Create();
    testNode4->SetBounds({rect3.left_, rect3.top_, rect3.width_, rect3.height_});
    testNode4->SetBackgroundColor(COLOR_DKGRAY);
    GetRootNode()->AddChild(testNode4);

    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->UpdateDirty();
    dirtyRegion = rsDirtyManager->GetDirtyRegion();
    rsDirtyManager->Clear();

    auto testNode5 = RSCanvasNode::Create();
    testNode5->SetBounds(
        {dirtyRegion.left_, dirtyRegion.top_ + DEFAULT_TRANS_Y_2, dirtyRegion.width_, dirtyRegion.height_});
    testNode5->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode5);

    rsDirtyManager->MergeDirtyRect(rect3);
    rsDirtyManager->UpdateDirty();
    dirtyRegion = rsDirtyManager->GetDirtyRegion();
    rsDirtyManager->Clear();

    auto testNode6 = RSCanvasNode::Create();
    testNode6->SetBounds(
        {dirtyRegion.left_, dirtyRegion.top_ + DEFAULT_TRANS_Y_3, dirtyRegion.width_, dirtyRegion.height_});
    testNode6->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChild(testNode6);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
    RegisterNode(testNode4);
    RegisterNode(testNode5);
    RegisterNode(testNode6);
}

/*
 * @tc.name: Update_Dirty_Test04
 * @tc.desc: test updateDirty with 2 buffer age, draw advancedDirtyRegion_
 * @tc.type: FUNC
 * @tc.require: issueICF6LO
 */
GRAPHIC_TEST(DirtyRegionTest02, CONTENT_DISPLAY_TEST, Update_Dirty_Test04)
{
    RectI rect1 = {100, 100, 300, 300};
    RectI rect2 = {500, 200, 300, 300};
    RectI rect3 = {900, 300, 300, 300};

    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto size = GetScreenSize();
    rsDirtyManager->SetBufferAge(2);
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    rsDirtyManager->SetAdvancedDirtyRegionType(AdvancedDirtyRegionType::SET_ADVANCED_SURFACE_AND_DISPLAY);
    rsDirtyManager->SetSurfaceRect({0, 0, size.x_, size.y_});
    rsDirtyManager->MergeDirtyRect(rect1);
    rsDirtyManager->UpdateDirty();
    std::vector<RectI> advancedDirtyRegion = rsDirtyManager->GetAdvancedDirtyRegion();
    rsDirtyManager->Clear();

    DrawAdvancedDirtyRegion(advancedDirtyRegion, COLOR_GREEN);

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

    rsDirtyManager->MergeDirtyRect(rect2);
    rsDirtyManager->UpdateDirty();
    advancedDirtyRegion = rsDirtyManager->GetAdvancedDirtyRegion();
    rsDirtyManager->Clear();

    DrawAdvancedDirtyRegion(advancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y_2);

    rsDirtyManager->MergeDirtyRect(rect3);
    rsDirtyManager->UpdateDirty();
    advancedDirtyRegion = rsDirtyManager->GetAdvancedDirtyRegion();
    rsDirtyManager->Clear();

    DrawAdvancedDirtyRegion(advancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y_3);
    
    RegisterNode(testNode1);
    RegisterNode(testNode2);
    RegisterNode(testNode3);
}
} // namespace OHOS::Rosen