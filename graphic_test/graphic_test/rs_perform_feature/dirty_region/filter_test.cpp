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

Vector2f DEFAULT_TRANSLATE = { 600, 0 };
Vector4f DEFAULT_BOUNDS = { 0, 100, 400, 400 };
} //namespace

class DirtyRegionTest04 : public RSGraphicTest {
public:
    const Vector2f screenSize = GetScreenSize();
    // called before each tests
    void BeforeEach() override
    {
        SetSurfaceBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    }

    void TestCaseCapture()
    {
        auto pixelMap =
            DisplayManager::GetInstance().GetScreenshot(DisplayManager::GetInstance().GetDefaultDisplayId());
        pixelMap->crop({ 0, 0, 1200, 2000 });
        if (pixelMap) {
            const ::testing::TestInfo* const testInfo =
            ::testing::UnitTest::GetInstance()->current_test_info();
            std::string fileName = "/data/local/graphic_test/rs_perform_feature/dirty_region/";
            namespace fs = std::filesystem;
            if (!fs::exists(fileName)) {
                if (!fs::create_directories(fileName)) {
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
        int duration = 1000; // the animation lasts for 1 second
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
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds);
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
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds);
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
 * @tc.desc: test filter case, foreground blur with two different node
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter03)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds);
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
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds);
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
 * @tc.desc: test filter case, with dirty region lies between two background blur
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter05)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
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
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
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
 * @tc.desc: test filter case, with dirty region lies beyond two background blur
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter06)
{
    Vector4f bounds = { 800, 800, 400, 400 };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds);
    RegisterNode(testNode);
    testNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 800, 1500, 400, 400 };
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
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
 * @tc.desc: test filter case, with dirty region intersect one foreground blur and out of another
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter07)
{
    Vector4f bounds = { 0, 0, 1000, 1000 };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 800, 1500, 400, 400 };
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
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
 * @tc.desc: test filter case, with dirty region in one foreground blur and out of another
 * @tc.type: FUNC
 * @tc.require: issueICP02F
 */
GRAPHIC_N_TEST(DirtyRegionTest04, CONTENT_DISPLAY_TEST, Filter08)
{
    Vector4f bounds = { 0, 0, 1000, 1000 };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds);
    RegisterNode(testNode);
    testNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_BOUNDS);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBackgroundColor(COLOR_RED);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 800, 1500, 400, 400 };
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(testNode3);
    testNode3->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    GetRootNode()->AddChild(testNode);
    testNode->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}
} // namespace OHOS::Rosen