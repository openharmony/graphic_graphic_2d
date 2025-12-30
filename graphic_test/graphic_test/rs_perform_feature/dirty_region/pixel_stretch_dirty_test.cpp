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
    constexpr uint32_t COLOR_RED = 0xFFFF0000;
    constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;
    constexpr int ABILITY_BGALPHA = 255;

    Vector2f TRANSLATE_POSITIVE_X = { 600, 0 };
    Vector2f TRANSLATE_NEGATIVE_X = { -600, 0 };
    Vector2f TRANSLATE_POSITIVE_Y = { 0, 600 };
    Vector2f TRANSLATE_NEGATIVE_Y = { 0, -600 };
    Vector4f DEFAULT_SMALL_BOUNDS_TOP = { 0, 100, 400, 400 };
    Vector4f DEFAULT_SMALL_BOUNDS_BOTTOM = { 800, 900, 400, 400 };
    Vector4f DEFAULT_LARGE_BOUNDS = { 200, 200, 800, 800 };
    Vector4f DEFAULT_PIXEL_STRETCH_BOUNDS = { -50, -50, -50, -50 };

    const std::string TEST_IMG_PATH = "/data/local/tmp/dr_test.jpg";
} //namespace

class DirtyRegionTest06 : public RSGraphicTest {
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
        int duration = 500; // the animation lasts for 0.5 seconds
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
 * @tc.name: PixelStretch01
 * @tc.desc: test pixel stretch case, node moves to the other node's left stretched edge
 * @tc.type: FUNC
 * @tc.require: issue20109
 */
GRAPHIC_N_TEST(DirtyRegionTest06, CONTENT_DISPLAY_TEST, PixelStretch01)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_LARGE_BOUNDS);
    RegisterNode(testNode);
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH_BOUNDS, Drawing::TileMode::CLAMP);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_SMALL_BOUNDS_TOP);
    testNode2->SetBackgroundColor(COLOR_RED);
    testNode2->SetTranslate({ 0, 0 });
    DoAnimation(testNode2, TRANSLATE_POSITIVE_X);

    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PixelStretch02
 * @tc.desc: test pixel stretch case, node moves to the other node's top stretched edge
 * @tc.type: FUNC
 * @tc.require: issue21288
 */
GRAPHIC_N_TEST(DirtyRegionTest06, CONTENT_DISPLAY_TEST, PixelStretch02)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_LARGE_BOUNDS);
    RegisterNode(testNode);
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH_BOUNDS, Drawing::TileMode::CLAMP);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_SMALL_BOUNDS_TOP);
    testNode2->SetBackgroundColor(COLOR_RED);
    testNode2->SetTranslate({ 0, 0 });
    DoAnimation(testNode2, TRANSLATE_POSITIVE_Y);

    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PixelStretch03
 * @tc.desc: test pixel stretch case, node moves to the other node's right stretched edge
 * @tc.type: FUNC
 * @tc.require: issue21288
 */
GRAPHIC_N_TEST(DirtyRegionTest06, CONTENT_DISPLAY_TEST, PixelStretch03)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_LARGE_BOUNDS);
    RegisterNode(testNode);
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH_BOUNDS, Drawing::TileMode::CLAMP);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_SMALL_BOUNDS_BOTTOM);
    testNode2->SetBackgroundColor(COLOR_RED);
    testNode2->SetTranslate({ 0, 0 });
    DoAnimation(testNode2, TRANSLATE_NEGATIVE_X);

    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PixelStretch04
 * @tc.desc: test pixel stretch case, node moves to the other node's bottom stretched edge
 * @tc.type: FUNC
 * @tc.require: issue21288
 */
GRAPHIC_N_TEST(DirtyRegionTest06, CONTENT_DISPLAY_TEST, PixelStretch04)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_LARGE_BOUNDS);
    RegisterNode(testNode);
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH_BOUNDS, Drawing::TileMode::CLAMP);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_SMALL_BOUNDS_BOTTOM);
    testNode2->SetBackgroundColor(COLOR_RED);
    testNode2->SetTranslate({ 0, 0 });
    DoAnimation(testNode2, TRANSLATE_NEGATIVE_Y);

    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PixelStretch05
 * @tc.desc: test pixel stretch case, different pixel stretch tile mode with REPEAT
 * @tc.type: FUNC
 * @tc.require: issue21288
 */
GRAPHIC_N_TEST(DirtyRegionTest06, CONTENT_DISPLAY_TEST, PixelStretch05)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_LARGE_BOUNDS);
    RegisterNode(testNode);
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH_BOUNDS, Drawing::TileMode::REPEAT);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_SMALL_BOUNDS_TOP);
    testNode2->SetBackgroundColor(COLOR_RED);
    testNode2->SetTranslate({ 0, 0 });
    DoAnimation(testNode2, TRANSLATE_POSITIVE_Y);

    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PixelStretch06
 * @tc.desc: test pixel stretch case, different pixel stretch tile mode with MIRROR
 * @tc.type: FUNC
 * @tc.require: issue21288
 */
GRAPHIC_N_TEST(DirtyRegionTest06, CONTENT_DISPLAY_TEST, PixelStretch06)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_LARGE_BOUNDS);
    RegisterNode(testNode);
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH_BOUNDS, Drawing::TileMode::MIRROR);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_SMALL_BOUNDS_BOTTOM);
    testNode2->SetBackgroundColor(COLOR_RED);
    testNode2->SetTranslate({ 0, 0 });
    DoAnimation(testNode2, TRANSLATE_NEGATIVE_X);

    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PixelStretch07
 * @tc.desc: test pixel stretch case, different pixel stretch tile mode with DECAL
 * @tc.type: FUNC
 * @tc.require: issue21288
 */
GRAPHIC_N_TEST(DirtyRegionTest06, CONTENT_DISPLAY_TEST, PixelStretch07)
{
    auto testNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_LARGE_BOUNDS);
    RegisterNode(testNode);
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH_BOUNDS, Drawing::TileMode::DECAL);

    auto testNode2 = RSCanvasNode::Create();
    RegisterNode(testNode2);
    testNode2->SetBounds(DEFAULT_SMALL_BOUNDS_BOTTOM);
    testNode2->SetBackgroundColor(COLOR_RED);
    testNode2->SetTranslate({ 0, 0 });
    DoAnimation(testNode2, TRANSLATE_NEGATIVE_Y);

    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}
} // namespace OHOS::Rosen