/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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
#include "display_manager.h"

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
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
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;
const std::string TEST_IMG_PATH = "/data/local/tmp/dr_test.jpg";
class DirtyRegionTest01 : public RSGraphicTest {
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

    void TestCaseCapture()
    {
        auto pixelMap =
            DisplayManager::GetInstance().GetScreenshot(DisplayManager::GetInstance().GetDefaultDisplayId());
        if (pixelMap == nullptr) {
            // NOT MODIFY THE COMMENTS
            std::cout << "[   FAILED   ] " << "image is null" << std::endl;
            return;
        }
        pixelMap->crop({ 0, 0, 1200, 2000 });
        const ::testing::TestInfo* const testInfo =
            ::testing::UnitTest::GetInstance()->current_test_info();
        std::string fileName = "/data/local/graphic_test/rs_perform_feature/dirty_region/";
        namespace fs = std::filesystem;
        if (fs::exists(fileName) && !fs::is_directory(fileName)) {
            std::cout << "path is not dir" << std::endl;
            return;
        }
        if (!fs::exists(fileName) && !fs::create_directories(fileName)) {
            std::cout << "create dir failed" << std::endl;
            return;
        }
        fileName += testInfo->test_case_name() + std::string("_") + testInfo->name() + std::string(".png");
        if (!WriteToPngWithPixelMap(fileName, *pixelMap)) {
            // NOT MODIFY THE COMMENTS
            std::cout << "[   FAILED   ] " << fileName << std::endl;
            return;
        }
        std::cout << "png write to " << fileName << std::endl;
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

/*
 * @tc.name: Merge_Dirty_Rect_MultiScreen01
 * @tc.desc: test dirty region merge with multiple windows
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_MultiScreen01)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    
    RectI mainRect = {100, 100, 400, 400};
    RectI subRect1 = {600, 200, 300, 300};
    RectI subRect2 = {900, 500, 200, 200};
    
    Vector4f mainBounds = {mainRect.left_, mainRect.top_, mainRect.width_, mainRect.height_};
    auto mainNode = RSCanvasNode::Create();
    mainNode->SetBounds(mainBounds);
    mainNode->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(mainNode);
    
    SubWindowOptions subOpts1;
    subOpts1.bounds = {subRect1.left_, subRect1.top_, subRect1.width_, subRect1.height_};
    subOpts1.order = SubWindowOrder::ABOVE_MAIN;
    subOpts1.contentBgColor = 0x80FF0000;
    auto subId1 = CreateSubWindow(subOpts1);
    
    auto subNode1 = RSCanvasNode::Create();
    subNode1->SetBounds({0, 0, 200, 200});
    subNode1->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChildToSubWindow(subId1, subNode1);
    
    SubWindowOptions subOpts2;
    subOpts2.bounds = {subRect2.left_, subRect2.top_, subRect2.width_, subRect2.height_};
    subOpts2.order = SubWindowOrder::BELOW_MAIN;
    subOpts2.contentBgColor = 0x800000FF;
    auto subId2 = CreateSubWindow(subOpts2);
    
    auto subNode2 = RSCanvasNode::Create();
    subNode2->SetBounds({0, 0, 150, 150});
    subNode2->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChildToSubWindow(subId2, subNode2);
    
    rsDirtyManager->MergeDirtyRect(mainRect);
    rsDirtyManager->MergeDirtyRect(subRect1);
    rsDirtyManager->MergeDirtyRect(subRect2);
    
    rsDirtyManager->ResetDirtyAsSurfaceSize();
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    
    auto testNode4 = RSCanvasNode::Create();
    testNode4->SetBounds(
        {curDirtyRect.left_, curDirtyRect.top_ + DEFAULT_TRANS_Y, curDirtyRect.width_, curDirtyRect.height_});
    testNode4->SetBackgroundColor(COLOR_CYAN);
    GetRootNode()->AddChild(testNode4);
    
    RegisterNode(mainNode);
    RegisterNode(subNode1);
    RegisterNode(subNode2);
    RegisterNode(testNode4);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Merge_Dirty_Rect_Rect_MultiScreen02
 * @tc.desc: test dirty region merge with overlapping sub-windows
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_MultiScreen02)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    
    RectI mainRect = {0, 0, 800, 800};
    RectI subRect1 = {400, 400, 500, 500};
    RectI subRect2 = {500, 500, 400, 400};
    
    Vector4f mainBounds = {mainRect.left_, mainRect.top_, mainRect.width_, mainRect.height_};
    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, mainBounds);
    RegisterNode(mainNode);
    GetRootNode()->AddChild(mainNode);
    
    SubWindowOptions subOpts1;
    subOpts1.bounds = {subRect1.left_, subRect1.top_, subRect1.width_, subRect1.height_};
    subOpts1.order = SubWindowOrder::ABOVE_MAIN;
    subOpts1.contentBgColor = 0x80000000;
    auto subId1 = CreateSubWindow(subOpts1);
    
    auto subNode1 = RSCanvasNode::Create();
    subNode1->SetBounds({0, 0, 300, 300});
    subNode1->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChildToSubWindow(subId1, subNode1);
    
    SubWindowOptions subOpts2;
    subOpts2.bounds = {subRect2.left_, subRect2.top_, subRect2.width_, subRect2.height_};
    subOpts2.order = SubWindowOrder::ABSOLUTE;
    subOpts2.absoluteZ = 1000001.0f;
    subOpts2.contentBgColor = 0x80000000;
    auto subId2 = CreateSubWindow(subOpts2);
    
    auto subNode2 = RSCanvasNode::Create();
    subNode2->SetBounds({0, 0, 250, 250});
    subNode2->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChildToSubWindow(subId2, subNode2);
    
    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    
    rsDirtyManager->MergeDirtyRect(mainRect);
    rsDirtyManager->MergeDirtyRect(subRect1);
    rsDirtyManager->MergeDirtyRect(subRect2);
    std::vector<RectI> curAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();
    
    DrawAdvancedDirtyRegion(curAdvancedDirtyRegion, COLOR_RED, DEFAULT_TRANS_Y);
    
    RegisterNode(subNode1);
    RegisterNode(subNode2);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Merge_Dirty_Rect_MultiScreen_Boundary01
 * @tc.desc: test dirty region when sub-window slides from screen boundary
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_MultiScreen_Boundary01)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    
    RectI mainRect = {0, 0, 800, 800};
    RectI subRect = {600, 200, 300, 300};
    
    Vector4f mainBounds = {mainRect.left_, mainRect.top_, mainRect.width_, mainRect.height_};
    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, mainBounds);
    RegisterNode(mainNode);
    GetRootNode()->AddChild(mainNode);
    
    SubWindowOptions subOpts;
    subOpts.bounds = {subRect.left_, subRect.top_, subRect.width_, subRect.height_};
    subOpts.order = SubWindowOrder::ABOVE_MAIN;
    subOpts.contentBgColor = 0x80FF0000;
    auto subId = CreateSubWindow(subOpts);
    
    auto subNode = RSCanvasNode::Create();
    subNode->SetBounds({0, 0, 200, 200});
    subNode->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChildToSubWindow(subId, subNode);
    
    rsDirtyManager->MergeDirtyRect(mainRect);
    rsDirtyManager->MergeDirtyRect(subRect);
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds(
        {curDirtyRect.left_, curDirtyRect.top_ + DEFAULT_TRANS_Y, curDirtyRect.width_, curDirtyRect.height_});
    testNode->SetBackgroundColor(COLOR_CYAN);
    GetRootNode()->AddChild(testNode);
    
    RegisterNode(subNode);
    RegisterNode(testNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Merge_Dirty_Rect_MultiScreen_Reset01
 * @tc.desc: test reset dirty region to full window size in multi-window scenario
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_MultiScreen_Reset01)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    
    RectI mainRect = {100, 100, 400, 400};
    RectI subRect1 = {600, 200, 300, 300};
    RectI subRect2 = {900, 500, 200, 200};
    
    auto mainNode = RSCanvasNode::Create();
    mainNode->SetBounds({mainRect.left_, mainRect.top_, mainRect.width_, mainRect.height_});
    mainNode->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(mainNode);
    
    SubWindowOptions subOpts1;
    subOpts1.bounds = {subRect1.left_, subRect1.top_, subRect1.width_, subRect1.height_};
    subOpts1.order = SubWindowOrder::ABOVE_MAIN;
    subOpts1.contentBgColor = 0x80FF0000;
    auto subId1 = CreateSubWindow(subOpts1);
    
    auto subNode1 = RSCanvasNode::Create();
    subNode1->SetBounds({0, 0, 200, 200});
    subNode1->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChildToSubWindow(subId1, subNode1);
    
    SubWindowOptions subOpts2;
    subOpts2.bounds = {subRect2.left_, subRect2.top_, subRect2.width_, subRect2.height_};
    subOpts2.order = SubWindowOrder::BELOW_MAIN;
    subOpts2.contentBgColor = 0x800000FF;
    auto subId2 = CreateSubWindow(subOpts2);
    
    auto subNode2 = RSCanvasNode::Create();
    subNode2->SetBounds({0, 0, 150, 150});
    subNode2->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChildToSubWindow(subId2, subNode2);
    
    rsDirtyManager->MergeDirtyRect(mainRect);
    rsDirtyManager->MergeDirtyRect(subRect1);
    rsDirtyManager->MergeDirtyRect(subRect2);
    
    rsDirtyManager->ResetDirtyAsSurfaceSize();
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds(
        {curDirtyRect.left_, curDirtyRect.top_ + DEFAULT_TRANS_Y, curDirtyRect.width_, curDirtyRect.height_});
    testNode->SetBackgroundColor(COLOR_CYAN);
    GetRootNode()->AddChild(testNode);
    
    RegisterNode(mainNode);
    RegisterNode(subNode1);
    RegisterNode(subNode2);
    RegisterNode(testNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Merge_Dirty_Rect_MultiScreen_Advanced01
 * @tc.desc: test advanced dirty region mode with multiple windows
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_MultiScreen_Advanced01)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    
    RectI mainRect = {0, 0, 800, 800};
    RectI subRect1 = {100, 100, 300, 300};
    RectI subRect2 = {500, 200, 300, 300};
    RectI subRect3 = {700, 600, 250, 250};

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, {0, 0, 800, 800});
    RegisterNode(mainNode);
    GetRootNode()->AddChild(mainNode);
    SubWindowOptions subOpts1;
    subOpts1.bounds = {subRect1.left_, subRect1.top_, subRect1.width_, subRect1.height_};
    subOpts1.order = SubWindowOrder::ABOVE_MAIN;
    subOpts1.contentBgColor = 0x80FF0000;
    auto subId1 = CreateSubWindow(subOpts1);
    
    auto subNode1 = RSCanvasNode::Create();
    subNode1->SetBounds({0, 0, 200, 200});
    subNode1->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChildToSubWindow(subId1, subNode1);
    
    SubWindowOptions subOpts2;
    subOpts2.bounds = {subRect2.left_, subRect2.top_, subRect2.width_, subRect2.height_};
    subOpts2.order = SubWindowOrder::BELOW_MAIN;
    subOpts2.contentBgColor = 0x800000FF;
    auto subId2 = CreateSubWindow(subOpts2);
    
    auto subNode2 = RSCanvasNode::Create();
    subNode2->SetBounds({0, 0, 200, 200});
    subNode2->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChildToSubWindow(subId2, subNode2);
    
    SubWindowOptions subOpts3;
    subOpts3.bounds = {subRect3.left_, subRect3.top_, subRect3.width_, subRect3.height_};
    subOpts3.order = SubWindowOrder::ABSOLUTE;
    subOpts3.absoluteZ = 1000003.0f;
    subOpts3.contentBgColor = 0x800000FF;
    auto subId3 = CreateSubWindow(subOpts3);
    
    auto subNode3 = RSCanvasNode::Create();
    subNode3->SetBounds({0, 0, 180, 180});
    subNode3->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChildToSubWindow(subId3, subNode3);

    rsDirtyManager->SetMaxNumOfDirtyRects(10);
    rsDirtyManager->MergeDirtyRect(mainRect);
    rsDirtyManager->MergeDirtyRect(subRect1);
    rsDirtyManager->MergeDirtyRect(subRect2);
    rsDirtyManager->MergeDirtyRect(subRect3);
    DrawAdvancedDirtyRegion(rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion(), COLOR_RED, DEFAULT_TRANS_Y);
    
    RegisterNode(subNode1);
    RegisterNode(subNode2);
    RegisterNode(subNode3);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Merge_Dirty_Rect_MultiScreen_Surface01
 * @tc.desc: test surface rect changes with multiple windows
 * @tc.type: FUNC
 * @tc.require: issueICF36K
 */
GRAPHIC_TEST(DirtyRegionTest01, CONTENT_DISPLAY_TEST, Merge_Dirty_Rect_MultiScreen_Surface01)
{
    auto rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    
    RectI mainRect = {100, 100, 400, 400};
    RectI subRect1 = {600, 200, 300, 300};
    RectI subRect2 = {900, 500, 200, 200};
    
    auto mainNode = RSCanvasNode::Create();
    mainNode->SetBounds({mainRect.left_, mainRect.top_, mainRect.width_, mainRect.height_});
    mainNode->SetBackgroundColor(COLOR_YELLOW);
    GetRootNode()->AddChild(mainNode);
    
    SubWindowOptions subOpts1;
    subOpts1.bounds = {subRect1.left_, subRect1.top_, subRect1.width_, subRect1.height_};
    subOpts1.order = SubWindowOrder::ABOVE_MAIN;
    subOpts1.contentBgColor = 0x80FF0000;
    auto subId1 = CreateSubWindow(subOpts1);
    
    auto subNode1 = RSCanvasNode::Create();
    subNode1->SetBounds({0, 0, 200, 200});
    subNode1->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChildToSubWindow(subId1, subNode1);
    
    SubWindowOptions subOpts2;
    subOpts2.bounds = {subRect2.left_, subRect2.top_, subRect2.width_, subRect2.height_};
    subOpts2.order = SubWindowOrder::BELOW_MAIN;
    subOpts2.contentBgColor = 0x800000FF;
    auto subId2 = CreateSubWindow(subOpts2);
    
    auto subNode2 = RSCanvasNode::Create();
    subNode2->SetBounds({0, 0, 150, 150});
    subNode2->SetBackgroundColor(COLOR_RED);
    GetRootNode()->AddChildToSubWindow(subId2, subNode2);
    
    rsDirtyManager->MergeDirtyRect(mainRect);
    rsDirtyManager->MergeDirtyRect(subRect1);
    rsDirtyManager->MergeDirtyRect(subRect2);
    
    rsDirtyManager->ResetDirtyAsSurfaceSize();
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds(
        {curDirtyRect.left_, curDirtyRect.top_ + DEFAULT_TRANS_Y, curDirtyRect.width_, curDirtyRect.height_});
    testNode->SetBackgroundColor(COLOR_CYAN);
    GetRootNode()->AddChild(testNode);
    
    RegisterNode(mainNode);
    RegisterNode(subNode1);
    RegisterNode(subNode2);
    RegisterNode(testNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}
} // namespace OHOS::Rosen
