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
#include "display_manager.h"

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"

#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;
constexpr float DEFAULT_ALPHA = 0.5f;
constexpr float DEFAULT_FOREGROUND_RADIUS = 10.0f;
constexpr float DEFAULT_RADIUS = 50.0f;
constexpr int ABILITY_BGALPHA = 255;

Vector2f DEFAULT_TRANSLATE = { 600, 0 };
Vector4f DEFAULT_BOUNDS = { 0, 100, 400, 400 };

const std::string TEST_IMG_PATH = "/data/local/tmp/dr_test.jpg";
} //namespace

class DirtyRegionTest04 : public RSGraphicTest {
public:
    const Vector2f screenSize = GetScreenSize();
    // called before each tests
    void BeforeEach() override
    {
        SetSurfaceBounds({ 0, 0, screenSize.x_, screenSize.y_ });
        GetRootNode()->testSurfaceNodes_.back()->SetAbilityBGAlpha(ABILITY_BGALPHA);
    }

    void TestCaseCapture()
    {
        auto pixelMap =
            DisplayManager::GetInstance().GetScreenshot(DisplayManager::GetInstance().GetDefaultDisplayId());
        if (pixelMap) {
            pixelMap->crop({ 0, 0, 1200, 2000 });
            const ::testing::TestInfo* const testInfo =
                ::testing::UnitTest::GetInstance()->current_test_info();
            std::string fileName = "/data/local/graphic_test/rs_perform_feature/dirty_region/";
            namespace fs = std::filesystem;
            if (!fs::exists(fileName)) {
                fs::create_directories(fileName);
                if (!std::filesystem::exists(fileName)) {
                    std::cout << "create dir failed" << std::endl;
                    return;
                }
            } else {
                if (!fs::is_directory(fileName)) {
                    std::cout << "path is not dir" << std::endl;
                    return;
                }
            }
            fileName += testInfo->test_case_name() + std::string("_") + testInfo->name() + std::string(".png");
            if (!WriteToPngWithPixelMap(fileName, *pixelMap)) {
                // NOT MODIFY THE COMMENTS
                std::cout << "[   FAILED   ] " << fileName << std::endl;
                return;
            }
            std::cout << "png write to " << fileName << std::endl;
        } else {
            // NOT MODIFY THE COMMENTS
            std::cout << "[   FAILED   ] " << "image is null" << std::endl;
        }
    }

    void DoAnimation(std::shared_ptr<RSCanvasNode>& canvasNode, Vector2f translate)
    {
        RSAnimationTimingProtocol protocol;
        int duration = 500; // the animation lasts for 0.5 second
        protocol.SetDuration(duration);
        auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
        RSNode::Animate(protocol, timingCurve, [&]() {
            canvasNode->SetTranslate(translate);
        }, []() {
            std::cout << "animation finish callback" << std::endl;
        });
    }
};

/*
 * @tc.name: Filter01
 * @tc.desc: test filter case, background blur
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter01)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    testNode->AddChild(testNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter02
 * @tc.desc: test filter case, foreground blur
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter02)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    testNode->AddChild(testNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter03
 * @tc.desc: two moving nodes above one full screen foreground blur
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter03)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 0, 800, 400, 400 };
    auto testNode3 = RSCanvasNode::Create();
    RegisterNode(testNode3);
    testNode3->SetBounds(bounds3);
    testNode3->SetTranslate({ 0, 0 });
    testNode3->SetBackgroundColor(COLOR_BLUE);
    DoAnimation(testNode3, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    testNode->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter04
 * @tc.desc: test filter case, background blur with alpha
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter04)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);
    testNode->SetAlpha(DEFAULT_ALPHA);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter05
 * @tc.desc: test filter case, check node's track clears after it shifting between two background blur views
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter05)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    Vector4f bounds2 = { 0, 800, 400, 400 };
    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(bounds2);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 600, 1500, 400, 400 };
    auto testNode3 = SetUpNodeBgImage(TEST_IMG_PATH, bounds3);
    RegisterNode(testNode3);
    testNode3->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter06
 * @tc.desc: test filter case, check node's track clears after it shifting above two background blur views
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter06)
{
    Vector4f bounds = { 800, 800, 400, 400 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 800, 1500, 400, 400 };
    auto testNode3 = SetUpNodeBgImage(TEST_IMG_PATH, bounds3);
    RegisterNode(testNode3);
    testNode3->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter07
 * @tc.desc: check node's track clears after it shifting intersects one foreground blur, not the other
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter07)
{
    Vector4f bounds = { 0, 0, 1000, 1000 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 800, 1500, 400, 400 };
    auto testNode3 = SetUpNodeBgImage(TEST_IMG_PATH, bounds3);
    RegisterNode(testNode3);
    testNode3->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter08
 * @tc.desc: check node's track clears after it shifting in one foreground blur, out of the other
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter08)
{
    Vector4f bounds = { 0, 0, 1000, 1000 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 800, 1500, 400, 400 };
    auto testNode3 = SetUpNodeBgImage(TEST_IMG_PATH, bounds3);
    RegisterNode(testNode3);
    testNode3->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    GetRootNode()->AddChild(testNode);
    testNode->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter09
 * @tc.desc: check node's track clears after it shifting between two background blur views on the Z-axis
 * @tc.type: FUNC
 * @tc.require: issue21265
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter09)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    auto testNode3 = RSCanvasNode::Create();
    RegisterNode(testNode3);
    testNode3->SetBounds(bounds);
    testNode3->SetBackgroundColor(COLOR_BLUE);
    testNode3->SetAlpha(DEFAULT_ALPHA);
    testNode3->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter10
 * @tc.desc: check node's track clears after it shifting between two foreground blur views on the Z-axis
 * @tc.type: FUNC
 * @tc.require: issue21265
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter10)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    auto testNode3 = RSCanvasNode::Create();
    RegisterNode(testNode3);
    testNode3->SetBounds(bounds);
    testNode3->SetForegroundColor(COLOR_BLUE);
    testNode3->SetAlpha(DEFAULT_ALPHA);
    testNode3->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter11
 * @tc.desc: two moving nodes above one full screen background blur
 * @tc.type: FUNC
 * @tc.require: issue21265
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter11)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 0, 800, 400, 400 };
    auto testNode3 = RSCanvasNode::Create();
    RegisterNode(testNode3);
    testNode3->SetBounds(bounds3);
    testNode3->SetTranslate({ 0, 0 });
    testNode3->SetBackgroundColor(COLOR_BLUE);
    DoAnimation(testNode3, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    testNode->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter12
 * @tc.desc: test filter case, background blur between two moving nodes on the Z-axis
 * @tc.type: FUNC
 * @tc.require: issue21265
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter12)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 0, 800, 400, 400 };
    auto testNode3 = RSCanvasNode::Create();
    RegisterNode(testNode3);
    testNode3->SetBounds(bounds3);
    testNode3->SetTranslate({ 0, 0 });
    testNode3->SetBackgroundColor(COLOR_BLUE);
    DoAnimation(testNode3, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode3);
    GetRootNode()->AddChild(testNode);
    testNode->AddChild(testNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Filter13
 * @tc.desc: test filter case, foreground blur between two moving nodes on the Z-axis
 * @tc.type: FUNC
 * @tc.require: issue21265
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter13)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 0, 800, 400, 400 };
    auto testNode3 = RSCanvasNode::Create();
    RegisterNode(testNode3);
    testNode3->SetBounds(bounds3);
    testNode3->SetTranslate({ 0, 0 });
    testNode3->SetBackgroundColor(COLOR_BLUE);
    DoAnimation(testNode3, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode3);
    GetRootNode()->AddChild(testNode);
    testNode->AddChild(testNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}
/*
 * @tc.name: MultiWindowFilter01
 * @tc.desc: sub-window with background blur occludes main window content
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, MultiWindowFilter01)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    GetRootNode()->AddChild(testNode);

    auto movingNode = RSCanvasNode::Create();
    RegisterNode(movingNode);
    movingNode->SetBounds(DEFAULT_BOUNDS);
    movingNode->SetTranslate({ 0, 0 });
    movingNode->SetBackgroundColor(COLOR_RED);
    DoAnimation(movingNode, DEFAULT_TRANSLATE);
    testNode->AddChild(movingNode);

    SubWindowOptions opts;
    opts.bounds = { 200, 200, 600, 600 }; // sub-window offset and size
    opts.order = SubWindowOrder::ABOVE_MAIN;
    opts.contentBgColor = 0x80000000; // semi-transparent black background
    auto subId = CreateSubWindow(opts);

    auto blurNode = RSCanvasNode::Create();
    RegisterNode(blurNode);
    blurNode->SetBounds({ 0, 0, 300, 300 }); // partial coverage within sub-window
    blurNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);
    blurNode->SetAlpha(DEFAULT_ALPHA);
    blurNode->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChildToSubWindow(subId, blurNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: MultiWindowFilter02
 * @tc.desc: main window foreground blur, sub-window behind main window
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, MultiWindowFilter02)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);
    GetRootNode()->AddChild(testNode);

    SubWindowOptions opts;
    opts.bounds = { 100, 100, 500, 500 }; // sub-window offset and size
    opts.order = SubWindowOrder::BELOW_MAIN;
    opts.contentBgColor = 0x80000000; // semi-transparent black background
    auto subId = CreateSubWindow(opts);

    auto subContent = RSCanvasNode::Create();
    RegisterNode(subContent);
    subContent->SetBounds({ 0, 0, 300, 300 }); // partial coverage within sub-window
    subContent->SetBackgroundColor(COLOR_BLUE);
    subContent->SetTranslate({ 0, 0 });
    DoAnimation(subContent, DEFAULT_TRANSLATE);
    GetRootNode()->AddChildToSubWindow(subId, subContent);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: MultiWindowFilter03
 * @tc.desc: two sub-windows with blur overlapping each other
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, MultiWindowFilter03)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    GetRootNode()->AddChild(testNode);

    SubWindowOptions opts1;
    opts1.bounds = { 0, 100, 600, 600 }; // sub-window 1 offset and size
    opts1.order = SubWindowOrder::ABOVE_MAIN;
    opts1.contentBgColor = 0x80000000; // semi-transparent black background
    auto subId1 = CreateSubWindow(opts1);

    auto blurNode1 = RSCanvasNode::Create();
    RegisterNode(blurNode1);
    blurNode1->SetBounds({ 0, 0, 300, 300 }); // partial coverage within sub-window
    blurNode1->SetBackgroundBlurRadius(DEFAULT_RADIUS);
    blurNode1->SetBackgroundColor(COLOR_RED);
    blurNode1->SetAlpha(DEFAULT_ALPHA);
    GetRootNode()->AddChildToSubWindow(subId1, blurNode1);

    SubWindowOptions opts2;
    opts2.bounds = { 300, 400, 600, 600 }; // sub-window 2 offset and size, overlaps with sub-window 1
    opts2.order = SubWindowOrder::ABSOLUTE;
    constexpr float subWindowZ = 1000002.0f;
    opts2.absoluteZ = subWindowZ;
    opts2.contentBgColor = 0x80000000; // semi-transparent black background
    auto subId2 = CreateSubWindow(opts2);

    auto blurNode2 = RSCanvasNode::Create();
    RegisterNode(blurNode2);
    blurNode2->SetBounds({ 0, 0, 300, 300 }); // partial coverage within sub-window
    blurNode2->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);
    blurNode2->SetBackgroundColor(COLOR_BLUE);
    blurNode2->SetAlpha(DEFAULT_ALPHA);
    GetRootNode()->AddChildToSubWindow(subId2, blurNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: MultiWindowFilter04
 * @tc.desc: moving node in sub-window crossing main window blur region
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, MultiWindowFilter04)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);
    GetRootNode()->AddChild(testNode);

    SubWindowOptions opts;
    opts.bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    opts.order = SubWindowOrder::ABOVE_MAIN;
    opts.contentBgColor = 0x80000000; // semi-transparent black background
    auto subId = CreateSubWindow(opts);

    auto movingNode = RSCanvasNode::Create();
    RegisterNode(movingNode);
    movingNode->SetBounds(DEFAULT_BOUNDS);
    movingNode->SetTranslate({ 0, 0 });
    movingNode->SetBackgroundColor(COLOR_RED);
    DoAnimation(movingNode, DEFAULT_TRANSLATE);
    GetRootNode()->AddChildToSubWindow(subId, movingNode);

    Vector4f bounds3 = { 0, 800, 400, 400 }; // second moving node, below the first one
    auto movingNode2 = RSCanvasNode::Create();
    RegisterNode(movingNode2);
    movingNode2->SetBounds(bounds3);
    movingNode2->SetTranslate({ 0, 0 });
    movingNode2->SetBackgroundColor(COLOR_BLUE);
    DoAnimation(movingNode2, DEFAULT_TRANSLATE);
    GetRootNode()->AddChildToSubWindow(subId, movingNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

} // namespace OHOS::Rosen