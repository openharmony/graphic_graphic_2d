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
            fileName += testInfo->test_case_name() + std::string("_");
            fileName += testInfo->name() + std::string(".png");
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
} // namespace OHOS::Rosen