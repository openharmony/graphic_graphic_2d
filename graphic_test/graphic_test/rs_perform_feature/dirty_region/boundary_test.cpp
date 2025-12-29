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
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;
constexpr int ABILITY_BGALPHA = 255;

Vector4f DEFAULT_BOUNDS = { 200, 200, 800, 800 };
const std::string TEST_IMG_PATH = "/data/local/tmp/dr_test.jpg";
} //namespace

class DirtyRegionTest08 : public RSGraphicTest {
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
 * @tc.name: ScreenBoundary01
 * @tc.desc: the node slides in the screen from the left
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary01)
{
    // node is left outside the screen
    Vector4f bounds = { -1000, 100, 800, 800 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { 800, 0 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary02
 * @tc.desc: the node slides in the screen from the top
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary02)
{
    // node is top outside the screen
    Vector4f bounds = { 100, -1000, 800, 800 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { 0, 800 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary03
 * @tc.desc: the node slides in the screen from the right
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary03)
{
    // node is bottom right the screen
    Vector4f bounds = { screenSize.x_ + 100, 100, 800, 800 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { -800, 0 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary04
 * @tc.desc: the node slides in the screen from the bottom
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary04)
{
    // node is bottom outside the screen
    Vector4f bounds = { 100, screenSize.y_ + 100, 800, 800 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { 0, -800 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary05
 * @tc.desc: slide the node from the middle of the screen to the left
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary05)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { -1200, 0 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary06
 * @tc.desc: slide the node from the middle of the screen to the top
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary06)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { 0, -1200 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary07
 * @tc.desc: slide the node from the middle of the screen to the right
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary07)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { screenSize.x_, 0 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary08
 * @tc.desc: slide the node from the middle of the screen to the bottom
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary08)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { 0, screenSize.y_ };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary09
 * @tc.desc: node is at the left edge of the screen, with part of it inside the screen and part outside
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary09)
{
    Vector4f bounds = { -400, 200, 800, 800 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { 0, 1200 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary10
 * @tc.desc: node is at the top edge of the screen, with part of it inside the screen and part outside
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary10)
{
    Vector4f bounds = { 200, -400, 800, 800 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { 1200, 0 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary11
 * @tc.desc: node is at the right edge of the screen, with part of it inside the screen and part outside
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary11)
{
    Vector4f bounds = { screenSize.x_ - 400, 200, 800, 800 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { 0, 1200 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: ScreenBoundary12
 * @tc.desc: node is at the bottom edge of the screen, with part of it inside the screen and part outside
 * @tc.type: FUNC
 * @tc.require: issue21381
 */
GRAPHIC_N_TEST(DirtyRegionTest08, CONTENT_DISPLAY_TEST, ScreenBoundary12)
{
    Vector4f bounds = { 200, screenSize.y_ - 400, 800, 800 };
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, bounds);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    Vector2f translate = { 1200, 0 };
    DoAnimation(testNode, translate);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}
} // namespace OHOS::Rosen