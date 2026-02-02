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

#include <cstdlib>
#include <filesystem>
#include <parameter.h>
#include <parameters.h>

#include "../../../../rosen/modules/render_service/core/system/rs_system_parameters.h"
#include "display_manager.h"
#include "param/sys_param.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"
#include "symbol_engine/hm_symbol_run.h"
#include "symbol_engine/hm_symbol_txt.h"

#include "draw/path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr uint32_t FONT_SIZE = 50;
constexpr uint32_t DEFAULT_SCALAR_X = 50;
constexpr uint32_t DEFAULT_SCALAR_Y = 50;
constexpr uint32_t RECORDING_WIDHT = 500;
constexpr uint32_t RECORDING_HEIGHT = 700;
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_CYAN = 0xFF00FFFF;
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;
Vector4f DEFAULT_BOUNDS = { 0, 0, RECORDING_WIDHT, RECORDING_HEIGHT };
Vector4f DEFAULT_FRAME = { 0, 0, RECORDING_WIDHT, RECORDING_HEIGHT };
} //namespace

class GpuComposerTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    const Vector2f screenSize = GetScreenSize();
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        SetForceClientComposer(true);
    }
    void AfterEach() override
    {
        SetForceClientComposer(false);
    }
    void SetForceClientComposer(bool isForceRedraw)
    {
        if (isForceRedraw) {
            system::SetParameter("rosen.client_composition.enabled", "1");
        } else {
            system::SetParameter("rosen.client_composition.enabled", "0");
        }
    }
    void TestCaseCapture()
    {
        auto pixelMap =
            DisplayManager::GetInstance().GetScreenshot(DisplayManager::GetInstance().GetDefaultDisplayId());
        if (pixelMap) {
            pixelMap->crop({ 0, 0, 1200, 2000 });
            const ::testing::TestInfo* const testInfo =
            ::testing::UnitTest::GetInstance()->current_test_info();
            std::string fileName = "/data/local/graphic_test/rs_perform_feature/gpu_composition/";
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

    void DoAnimation(std::shared_ptr<RSCanvasNode>& canvasNode, Vector2f translate, int duration = 500)
    {
        RSAnimationTimingProtocol protocol;
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
 * @tc.name: GpuComposer_TestText_1
 * @tc.desc: Test the textblob type for gpu composer without text tag.
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestText_1)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Font font = Font();
    font.SetSize(FONT_SIZE);
    Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<TextBlob> textblob = TextBlob::MakeFromString("GpuComposerTest",
        font, TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), DEFAULT_SCALAR_X, DEFAULT_SCALAR_Y);
    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GpuComposer_TestText_2
 * @tc.desc: Test the textblob type for gpu composer with text tag.
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestText_2)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Font font = Font();
    font.SetSize(FONT_SIZE);
    Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<TextBlob> textblob = TextBlob::MakeFromString("GpuComposerTest",
        font, TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), DEFAULT_SCALAR_X, DEFAULT_SCALAR_Y);
    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GpuComposer_TestText_3
 * @tc.desc: Test the textblob including multi lines for gpu composer with text tag.
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestText_3)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Font font = Font();
    font.SetSize(FONT_SIZE);
    Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<TextBlob> textblob = TextBlob::MakeFromString("Hello\nGpuComposerTest",
        font, TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), DEFAULT_SCALAR_X, DEFAULT_SCALAR_Y);
    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GpuComposer_TestSymbol_1
 * @tc.desc: Test the HMSymbol type for gpu composer render without HMSYMBOL tag.
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestSymbol_1)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Font font = Font();
    font.SetSize(FONT_SIZE);
    Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);

    RSPoint paint = {100, 100};
    SPText::HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    std::shared_ptr<TextBlob> textblob = TextBlob::MakeFromString("GpuComposerTest",
        font, TextEncoding::UTF8);
    SPText::HMSymbolRun hmSymbolRun = SPText::HMSymbolRun(1, symbolTxt, textblob, animationFunc);
    hmSymbolRun.DrawSymbol(recordingCanvas, paint);

    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GpuComposer_TestSymbol_2
 * @tc.desc: Test the HMSymbol for gpu composer with HMSYMBOL tag.
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestSymbol_2)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Font font = Font();
    font.SetSize(FONT_SIZE);
    Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);

    RSPoint paint = {100, 100};
    SPText::HMSymbolTxt symbolTxt;
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        [](const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig) {
            return true;
        };
    std::shared_ptr<TextBlob> textblob = TextBlob::MakeFromString("GpuComposerTest",
        font, TextEncoding::UTF8);
    SPText::HMSymbolRun hmSymbolRun = SPText::HMSymbolRun(1, symbolTxt, textblob, animationFunc);
    hmSymbolRun.DrawSymbol(recordingCanvas, paint);

    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}
/*
 * @tc.name: GpuComposer_TestSVG_1
 * @tc.desc: Test the svg for gpu composer without SVG tag.
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestSVG_1)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Font font = Font();
    font.SetSize(FONT_SIZE);
    Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);

    Path path;
    path.AddRect(Rect(RECORDING_WIDHT, 0, RECORDING_WIDHT, RECORDING_HEIGHT));
    recordingCanvas->DrawPath(path);
    Pen pen(Color::COLOR_RED);
    recordingCanvas->AttachPen(pen);
    recordingCanvas->DrawPath(path);

    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GpuComposer_TestSVG_2
 * @tc.desc: Test the svg for gpu composer with SVG tag.
 * @tc.type: FUNC
 * @tc.require: issueICRTWV
 */
GRAPHIC_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestSVG_2)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Font font = Font();
    font.SetSize(FONT_SIZE);
    Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);

    Path path;
    path.AddRect(Rect(RECORDING_WIDHT, 0, RECORDING_WIDHT, RECORDING_HEIGHT));
    recordingCanvas->DrawPath(path);
    Pen pen(Color::COLOR_RED);
    recordingCanvas->AttachPen(pen);
    recordingCanvas->DrawPath(path);

    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GpuComposer_TestSubTreeSkip01
 * @tc.desc: test canvas remove sub canvasNodes with weight = 0, height = 0
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestSubTreeSkip01)
{
    // bounds arrangement simulating the scenario of China Media Group's website
    Vector4f bounds = { 0, 2188, 1316, 210 };
    Vector4f bounds2 = { 0, 0, 1316, 210 };

    auto canvasNode1 = RSCanvasNode::Create();
    RegisterNode(canvasNode1);
    canvasNode1->SetBounds(bounds);
    canvasNode1->SetFrame(bounds);
    canvasNode1->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds3);
    backgroundNode->SetFrame(bounds3);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds4 = { 100, 1500, 200, 200 };
    auto imgNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds4);
    RegisterNode(imgNode);

    auto canvasNode2 = RSCanvasNode::Create();
    RegisterNode(canvasNode2);
    canvasNode2->SetBounds(bounds2);
    canvasNode2->SetFrame(bounds2);
    canvasNode2->SetTranslate({ 0, 0 });

    Vector4f bounds5 = { 800, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds5);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate3 = { -800, 0 };
    int duration = 10000;
    DoAnimation(animateNode, translate3, duration);

    auto pureCanvas = RSCanvasNode::Create();
    RegisterNode(pureCanvas);
    pureCanvas->SetBounds(bounds2);
    pureCanvas->SetFrame(bounds2);

    Vector4f bounds6 = { 0, 0, 200, 200 };
    auto canvasNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds6);
    RegisterNode(canvasNode3);
    canvasNode3->SetTranslate({ 0, 0 });

    auto canvasNode4 = RSCanvasNode::Create();
    RegisterNode(canvasNode4);
    canvasNode4->SetBounds(bounds2);
    canvasNode4->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(imgNode);
    backgroundNode->RSNode::AddChild(canvasNode1);
    canvasNode1->RSNode::AddChild(canvasNode2);
    canvasNode2->RSNode::AddChild(pureCanvas);
    canvasNode2->RSNode::AddChild(animateNode);
    pureCanvas->RSNode::AddChild(canvasNode4);
    pureCanvas->RSNode::AddChild(canvasNode3);

    Vector2f translate = { 0, -1419 };
    DoAnimation(canvasNode2, translate);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    Vector2f translate2 = { 800, 0 };
    DoAnimation(canvasNode3, translate2);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    Vector4f bounds7 = { 0, 2398, 0, 0 };
    canvasNode1->SetBounds(bounds7);
    canvasNode1->SetFrame(bounds7);
    pureCanvas->RemoveChild(canvasNode4);
    canvasNode2->RemoveChild(pureCanvas);
    canvasNode1->RemoveChild(canvasNode2);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: GpuComposer_TestSubTreeSkip02
 * @tc.desc: test sub canvasNode with parent alpha 1.0f to 0.1f
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestSubTreeSkip02)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    float alpha = 0.1f;
    parentCanvasNode->SetAlpha(alpha);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}

/*
 * @tc.name: GpuComposer_TestSubTreeSkip03
 * @tc.desc: test sub canvasNode with parent visible true to false
 * @tc.type: FUNC
 * @tc.require: issue20873
 */
GRAPHIC_N_TEST(GpuComposerTest, GPU_COMPOSER_TEST, GpuComposer_TestSubTreeSkip03)
{
    Vector4f bounds = { 0, 0, screenSize.x_, screenSize.y_ };
    auto backgroundNode = RSCanvasNode::Create();
    RegisterNode(backgroundNode);
    backgroundNode->SetBounds(bounds);
    backgroundNode->SetFrame(bounds);
    backgroundNode->SetBackgroundColor(COLOR_CYAN);

    Vector4f bounds2 = { 300, 300, 800, 800 };
    auto parentCanvasNode = RSCanvasNode::Create();
    RegisterNode(parentCanvasNode);
    parentCanvasNode->SetBounds(bounds2);
    parentCanvasNode->SetBackgroundColor(COLOR_BLUE);

    Vector4f bounds3 = { 0, 0, 200, 200 };
    auto animateNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(animateNode);
    animateNode->SetTranslate({ 0, 0 });

    Vector2f translate = { 0, 600 };
    int duration = 10000;
    DoAnimation(animateNode, translate, duration);

    Vector4f bounds4 = { 500, 200, 1200, 1200 };
    auto childCanvasNode = RSCanvasNode::Create();
    RegisterNode(childCanvasNode);
    childCanvasNode->SetBounds(bounds4);
    childCanvasNode->SetBackgroundColor(COLOR_YELLOW);

    GetRootNode()->AddChild(backgroundNode);
    backgroundNode->RSNode::AddChild(parentCanvasNode);
    parentCanvasNode->RSNode::AddChild(animateNode);
    parentCanvasNode->RSNode::AddChild(childCanvasNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    parentCanvasNode->SetVisible(false);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    TestCaseCapture();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS