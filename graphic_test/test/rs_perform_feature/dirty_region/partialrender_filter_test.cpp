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
#include <filesystem>
#include "display_manager.h"

#include "foundation/graphic/graphic_2d/graphic_test/test/rs_effect_feature/ng_filter_test/ng_filter_test_utils.h"
#include "foundation/graphic/graphic_2d/graphic_test/test/rs_effect_feature/ui_filter_test/ui_filter_test_data_manager.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"
#include "ui/rs_effect_node.h"

#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t COLOR_YELLOW = 0xFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;
constexpr float DEFAULT_FOREGROUND_RADIUS = 10.0f;
constexpr float DEFAULT_RADIUS = 50.0f;
constexpr int ABILITY_BGALPHA = 255;

Vector2f DEFAULT_TRANSLATE = { 0, 600 };
Vector4f DEFAULT_BOUNDS = { 200, 200, 800, 800 };

const std::string TEST_IMG_PATH = "/data/local/tmp/dr_test.jpg";
} //namespace

class PartialRenderFilter : public RSGraphicTest {
public:
    const Vector2f screenSize = GetScreenSize();
    // called before each tests
    void BeforeEach() override
    {
        SetSurfaceBounds({ 0, 0, screenSize.x_, screenSize.y_ });
        GetRootNode()->testSurfaceNodes_.back()->SetAbilityBGAlpha(ABILITY_BGALPHA);
        UIFilterTestDataManager::Initialize();
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
        std::shared_ptr<RSUIContext> rsUIContext;
        RSNode::Animate(rsUIContext, protocol, timingCurve, [&]() {
            canvasNode->SetTranslate(translate);
        }, []() {
            std::cout << "animation finish callback" << std::endl;
        });
    }
};

/*
 * @tc.name: PartialRenderFilter01
 * @tc.desc: test PartialRenderFilter case, multi-window with background blur
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter01)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    auto mainNode = RSCanvasNode::Create();
    RegisterNode(mainNode);
    mainNode->SetBounds(DEFAULT_BOUNDS);
    mainNode->SetBackgroundColor(COLOR_YELLOW);
    mainNode->SetBackgroundBlurRadius(DEFAULT_RADIUS);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(mainNode);

    SubWindowOptions opts;
    opts.bounds = DEFAULT_BOUNDS;
    opts.order = SubWindowOrder::ABOVE_MAIN;
    opts.contentBgColor = 0x00FFFFFF;
    auto subId = CreateSubWindow(opts);

    auto subNode = RSCanvasNode::Create();
    RegisterNode(subNode);
    subNode->SetBounds({ 200, 0, 200, 200 });
    subNode->SetTranslate({ 0, 0 });
    subNode->SetBackgroundColor(COLOR_BLUE);
    GetRootNode()->AddChildToSubWindow(subId, subNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(subNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter02
 * @tc.desc: test PartialRenderFilter case, dirty above foreground blur
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter02)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    mainNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(mainNode);
    GetRootNode()->AddChild(translateNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter03
 * @tc.desc: test PartialRenderFilter case, dirty under foreground blur
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter03)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    mainNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(translateNode);
    GetRootNode()->AddChild(mainNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter04
 * @tc.desc: test PartialRenderFilter case, foreground blur dirty above another foreground blur
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter04)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    mainNode->SetForegroundBlurRadius(DEFAULT_FOREGROUND_RADIUS);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(mainNode);
    GetRootNode()->AddChild(translateNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter05
 * @tc.desc: test PartialRenderFilter case, dirty above UI foreground blur
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter05)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    vector<FilterPara::ParaType> testParaVec = {
        FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
        FilterPara::ParaType::BLUR
    };
    auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(
        testParaVec, TestDataGroupType::VALID_DATA2);
    mainNode->SetUIForegroundFilter(combinedFilter);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(mainNode);
    GetRootNode()->AddChild(translateNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter06
 * @tc.desc: test PartialRenderFilter case, dirty under UI foreground blur
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter06)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    vector<FilterPara::ParaType> testParaVec = {
        FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
        FilterPara::ParaType::BLUR
    };
    auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(
        testParaVec, TestDataGroupType::VALID_DATA2);
    mainNode->SetUIForegroundFilter(combinedFilter);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(translateNode);
    GetRootNode()->AddChild(mainNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter07
 * @tc.desc: test PartialRenderFilter case, dirty above frostedGlassBlur
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter07)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
    InitFrostedGlassBlurFilter(frostedGlassBlurFilter);
    mainNode->SetBackgroundNGFilter(frostedGlassBlurFilter);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(mainNode);
    GetRootNode()->AddChild(translateNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter08
 * @tc.desc: test PartialRenderFilter case, dirty under frostedGlassBlur
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter08)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
    InitFrostedGlassBlurFilter(frostedGlassBlurFilter);
    mainNode->SetBackgroundNGFilter(frostedGlassBlurFilter);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(translateNode);
    GetRootNode()->AddChild(mainNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter09
 * @tc.desc: test PartialRenderFilter case, dirty above background blur use EC
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter09)
{
    auto backgroundNode = SetUpNodeBgImage(TEST_IMG_PATH, { 0, 0, screenSize.x_, screenSize.y_ });
    RegisterNode(backgroundNode);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto effectNode = RSEffectNode::Create();
    RegisterNode(effectNode);
    effectNode->SetBounds(DEFAULT_BOUNDS);
    effectNode->SetFrame(DEFAULT_BOUNDS);
    effectNode->SetBackgroundColor(COLOR_YELLOW);
    effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));

    Vector4f contentRect = { 0, 0, 800, 800 };
    auto mainNode = RSEffectNode::Create();
    RegisterNode(mainNode);
    mainNode->SetBounds(contentRect);
    mainNode->SetUseEffect(true);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(effectNode);
    effectNode->AddChild(mainNode);
    GetRootNode()->AddChild(translateNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter10
 * @tc.desc: test PartialRenderFilter case, dirty under background blur use EC
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter10)
{
    auto backgroundNode = SetUpNodeBgImage(TEST_IMG_PATH, { 0, 0, screenSize.x_, screenSize.y_ });
    RegisterNode(backgroundNode);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto effectNode = RSEffectNode::Create();
    RegisterNode(effectNode);
    effectNode->SetBounds(DEFAULT_BOUNDS);
    effectNode->SetFrame(DEFAULT_BOUNDS);
    effectNode->SetBackgroundColor(COLOR_YELLOW);
    effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));

    Vector4f contentRect = { 0, 0, 800, 800 };
    auto mainNode = RSEffectNode::Create();
    RegisterNode(mainNode);
    mainNode->SetBounds(contentRect);
    mainNode->SetUseEffect(true);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(translateNode);
    GetRootNode()->AddChild(effectNode);
    effectNode->AddChild(mainNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter11
 * @tc.desc: test PartialRenderFilter case, dirty above forstedGlassBlur use EC
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter11)
{
    auto backgroundNode = SetUpNodeBgImage(TEST_IMG_PATH, { 0, 0, screenSize.x_, screenSize.y_ });
    RegisterNode(backgroundNode);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto effectNode = RSEffectNode::Create();
    RegisterNode(effectNode);
    effectNode->SetBounds(DEFAULT_BOUNDS);
    effectNode->SetFrame(DEFAULT_BOUNDS);
    effectNode->SetBackgroundColor(COLOR_YELLOW);
    auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
    InitFrostedGlassBlurFilter(frostedGlassBlurFilter);
    effectNode->SetBackgroundNGFilter(frostedGlassBlurFilter);

    Vector4f contentRect = { 0, 0, 800, 800 };
    auto mainNode = RSEffectNode::Create();
    RegisterNode(mainNode);
    mainNode->SetBounds(contentRect);
    mainNode->SetUseEffect(true);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(effectNode);
    effectNode->AddChild(mainNode);
    GetRootNode()->AddChild(translateNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter12
 * @tc.desc: test PartialRenderFilter case, dirty under forstedGlassBlur use EC
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter12)
{
    auto backgroundNode = SetUpNodeBgImage(TEST_IMG_PATH, { 0, 0, screenSize.x_, screenSize.y_ });
    RegisterNode(backgroundNode);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto effectNode = RSEffectNode::Create();
    RegisterNode(effectNode);
    effectNode->SetBounds(DEFAULT_BOUNDS);
    effectNode->SetFrame(DEFAULT_BOUNDS);
    effectNode->SetBackgroundColor(COLOR_YELLOW);
    auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
    InitFrostedGlassBlurFilter(frostedGlassBlurFilter);
    effectNode->SetBackgroundNGFilter(frostedGlassBlurFilter);

    Vector4f contentRect = { 0, 0, 800, 800 };
    auto mainNode = RSEffectNode::Create();
    RegisterNode(mainNode);
    mainNode->SetBounds(contentRect);
    mainNode->SetUseEffect(true);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(translateNode);
    GetRootNode()->AddChild(effectNode);
    effectNode->AddChild(mainNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter13
 * @tc.desc: test PartialRenderFilter case, dirty above node use avarage shadowColorStrategy
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter13)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    mainNode->SetShadowColor(COLOR_BLUE);
    mainNode->SetShadowRadius(DEFAULT_RADIUS);
    mainNode->SetShadowColorStrategy(1);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(mainNode);
    GetRootNode()->AddChild(translateNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter14
 * @tc.desc: test PartialRenderFilter case, dirty under node use avarage shadowColorStrategy
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter14)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    mainNode->SetShadowColor(COLOR_BLUE);
    mainNode->SetShadowRadius(DEFAULT_RADIUS);
    mainNode->SetShadowColorStrategy(1);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(translateNode);
    GetRootNode()->AddChild(mainNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter15
 * @tc.desc: test PartialRenderFilter case, dirty above node use default shadowColorStrategy
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter15)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    mainNode->SetShadowColor(COLOR_BLUE);
    mainNode->SetShadowRadius(DEFAULT_RADIUS);
    mainNode->SetShadowColorStrategy(0);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(mainNode);
    GetRootNode()->AddChild(translateNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PartialRenderFilter16
 * @tc.desc: test PartialRenderFilter case, dirty under node use default shadowColorStrategy
 * @tc.type: FUNC
 * @tc.require: issue23513
 */
GRAPHIC_N_TEST(PartialRenderFilter, CONTENT_DISPLAY_TEST, PartialRenderFilter16)
{
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds({ 0, 0, screenSize.x_, screenSize.y_ });
    backgroundNode->SetBackgroundColor(COLOR_RED);

    Vector4f translateBounds = { 400, 0, 200, 200 };
    auto translateNode = RSCanvasNode::Create();
    RegisterNode(translateNode);
    translateNode->SetBounds(translateBounds);
    translateNode->SetBackgroundColor(COLOR_BLUE);
    translateNode->SetTranslate({ 0, 0 });

    auto mainNode = SetUpNodeBgImage(TEST_IMG_PATH, DEFAULT_BOUNDS);
    RegisterNode(mainNode);
    mainNode->SetShadowColor(COLOR_BLUE);
    mainNode->SetShadowRadius(DEFAULT_RADIUS);
    mainNode->SetShadowColorStrategy(0);

    GetRootNode()->AddChild(backgroundNode);
    GetRootNode()->AddChild(translateNode);
    GetRootNode()->AddChild(mainNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    DoAnimation(translateNode, DEFAULT_TRANSLATE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}
} // namespace OHOS::Rosen