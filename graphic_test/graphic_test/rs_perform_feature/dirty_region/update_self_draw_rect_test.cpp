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
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
constexpr uint32_t COLOR_DKGRAY = 0xFF444444;
constexpr uint32_t COLOR_CYAN = 0xFF00FFFF;
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000;
constexpr float DEFAULT_ALPHA = 0.5f;
constexpr float DEFAULT_FOREGROUND_RADIUS = 10.0f;
constexpr float DEFAULT_RADIUS = 50.0f;

Vector2f DEFAULT_TRANSLATE = { 600, 0 };
Vector4f DEFAULT_BOUNDS = { 0, 500, 400, 400 };
Vector4f DEFAULT_OUTLINE_WIDTH = { 10, 10, 10, 10 };
Vector4f DEFAULT_BORDER_WIDTH = { 15, 15, 15, 15 };
Vector4f DEFAULT_PIXEL_STRETCH = { 30, 30, 30, 30 };
Vector4<BorderStyle> STYLE_SOLID = Vector4<BorderStyle>(BorderStyle::SOLID);
Vector4<BorderStyle> STYLE_DASHED = Vector4<BorderStyle>(BorderStyle::DASHED);
Vector4<BorderStyle> STYLE_DOTTED = Vector4<BorderStyle>(BorderStyle::DOTTED);
Vector4<BorderStyle> STYLE_NONE = Vector4<BorderStyle>(BorderStyle::NONE);
} //namespace

class DirtyRegionTest03 : public RSGraphicTest {
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

    void DrawLine(std::shared_ptr<RSCanvasNode>& canvasNode, Drawing::PointF point1, Drawing::PointF point2)
    {
        auto recordingCanvas = canvasNode->BeginRecording(700, 700);
        Drawing::Brush brush;
        brush.SetColor(COLOR_RED);
        brush.SetAntiAlias(true);
        recordingCanvas->AttachBrush(brush);
        recordingCanvas->DrawLine(point1, point2);
        recordingCanvas->DetachBrush();
        canvasNode->FinishRecording();
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
 * @tc.name: Image01
 * @tc.desc: test self draw rect changed, normal case
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Image01)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: DrawRgeion01
 * @tc.desc: test self draw rect changed, set draw region
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, DrawRgeion01)
{
    std::shared_ptr<RectF> drawRegion = std::make_shared<RectF>(-100, 400, 700, 700);
    Drawing::PointF point1 = { 0, 450 };
    Drawing::PointF point2 = { 400, 450 };

    auto testNode = RSCanvasNode::Create();
    RegisterNode(testNode);
    testNode->SetBounds(DEFAULT_BOUNDS);
    testNode->SetBackgroundColor(COLOR_YELLOW);
    testNode->SetDrawRegion(drawRegion);
    testNode->SetTranslate({ 0, 0 });
    DrawLine(testNode, point1, point2);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Shader01
 * @tc.desc: test self draw rect changed, add shader
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Shader01)
{
    std::shared_ptr<OHOS::Rosen::Drawing::ShaderEffect> shaderEffect =
        OHOS::Rosen::Drawing::ShaderEffect::CreateLinearGradient({ 10, 10 }, { 100, 100 },
        { Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_RED }, { 0.0f, 0.5f, 1.0f },
        Drawing::TileMode::MIRROR);
    auto testNode = RSCanvasNode::Create();
    RegisterNode(testNode);
    testNode->SetBounds(DEFAULT_BOUNDS);
    testNode->SetTranslate({ 0, 0 });
    auto currentShader = RSShader::CreateRSShader(shaderEffect);
    testNode->SetBackgroundShader(currentShader);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Shadow01
 * @tc.desc: test self draw rect changed, add shadow
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Shadow01)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetShadowColor(COLOR_DKGRAY);
    testNode->SetShadowRadius(DEFAULT_RADIUS);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Shadow02
 * @tc.desc: test self draw rect changed, add shadow with different color
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Shadow02)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetShadowColor(COLOR_CYAN);
    testNode->SetShadowRadius(DEFAULT_RADIUS);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    Vector4f bounds2 = { 0, 1200, 400, 400 };
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds2);
    RegisterNode(testNode2);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetShadowColor(COLOR_BLUE);
    testNode2->SetShadowRadius(DEFAULT_RADIUS);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Shadow03
 * @tc.desc: test self draw rect changed, add shadow with offset
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Shadow03)
{
    float offSetX = -100.f;
    float offSetY = -100.f;
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetShadowColor(COLOR_DKGRAY);
    testNode->SetShadowRadius(DEFAULT_RADIUS);
    testNode->SetShadowOffset(offSetX, offSetY);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Shadow04
 * @tc.desc: test self draw rect changed, add shadow with different alpha
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Shadow04)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetShadowColor(COLOR_DKGRAY);
    testNode->SetShadowRadius(DEFAULT_RADIUS);
    testNode->SetShadowAlpha(DEFAULT_ALPHA);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Shadow05
 * @tc.desc: test self draw rect changed, add shadow with different radius
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Shadow05)
{
    float radius = 80.0f;
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetShadowColor(COLOR_DKGRAY);
    testNode->SetShadowRadius(radius);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Shadow06
 * @tc.desc: test self draw rect changed, add shadow with filled
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Shadow06)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetShadowColor(COLOR_DKGRAY);
    testNode->SetShadowRadius(DEFAULT_RADIUS);
    testNode->SetShadowIsFilled(0);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    Vector4f bounds2 = { 0, 1200, 400, 400 };
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds2);
    RegisterNode(testNode2);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetShadowColor(COLOR_DKGRAY);
    testNode2->SetShadowRadius(DEFAULT_RADIUS);
    testNode2->SetShadowIsFilled(1);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Shadow07
 * @tc.desc: test self draw rect changed, add shadow with different color strategy
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Shadow07)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetShadowColor(COLOR_BLUE);
    testNode->SetShadowRadius(DEFAULT_RADIUS);
    testNode->SetShadowColorStrategy(0);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    Vector4f bounds2 = { 0, 1200, 400, 400 };
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds2);
    RegisterNode(testNode2);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetShadowColor(COLOR_BLUE);
    testNode2->SetShadowRadius(DEFAULT_RADIUS);
    testNode2->SetShadowColorStrategy(1);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 0, 1900, 400, 400 };
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(testNode3);
    testNode3->SetTranslate({ 0, 0 });
    testNode3->SetShadowColor(COLOR_BLUE);
    testNode3->SetShadowRadius(DEFAULT_RADIUS);
    testNode3->SetShadowColorStrategy(2);
    DoAnimation(testNode3, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Outline01
 * @tc.desc: test self draw rect changed, add outline
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Outline01)
{
    Color color(0, 0, 0);
    Vector4<Color> outlineColor = { color, color, color, color };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetOutlineStyle(STYLE_SOLID);
    testNode->SetOutlineWidth(DEFAULT_OUTLINE_WIDTH);
    testNode->SetOutlineColor(outlineColor);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Outline02
 * @tc.desc: test self draw rect changed, add outline with different width
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Outline02)
{
    Color color(0, 0, 0);
    Vector4<Color> outlineColor = { color, color, color, color };
    Vector4f outlineWidth = { 5, 10, 15, 20 };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetOutlineStyle(STYLE_SOLID);
    testNode->SetOutlineWidth(outlineWidth);
    testNode->SetOutlineColor(outlineColor);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Outline03
 * @tc.desc: test self draw rect changed, add outline with different style
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Outline03)
{
    Color color(0, 0, 0);
    Vector4<Color> outlineColor = { color, color, color, color };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetOutlineStyle(STYLE_DASHED);
    testNode->SetOutlineWidth(DEFAULT_OUTLINE_WIDTH);
    testNode->SetOutlineColor(outlineColor);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    Vector4f bounds2 = { 0, 1200, 400, 400 };
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds2);
    RegisterNode(testNode2);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetOutlineStyle(STYLE_DOTTED);
    testNode2->SetOutlineWidth(DEFAULT_OUTLINE_WIDTH);
    testNode2->SetOutlineColor(outlineColor);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 0, 1900, 400, 400 };
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(testNode3);
    testNode3->SetTranslate({ 0, 0 });
    testNode3->SetOutlineStyle(STYLE_NONE);
    testNode3->SetOutlineWidth(DEFAULT_OUTLINE_WIDTH);
    testNode3->SetOutlineColor(outlineColor);
    DoAnimation(testNode3, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Border01
 * @tc.desc: test self draw rect changed, add border
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Border01)
{
    Color color(0, 0, 0);
    Vector4<Color> borderColor = { color, color, color, color };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetBorderStyle(STYLE_SOLID);
    testNode->SetBorderWidth(DEFAULT_BORDER_WIDTH);
    testNode->SetBorderColor(borderColor);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Border02
 * @tc.desc: test self draw rect changed, add border with different style
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Border02)
{
    Color color(0, 0, 0);
    Vector4<Color> borderColor = { color, color, color, color };
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetBorderStyle(STYLE_DASHED);
    testNode->SetBorderWidth(DEFAULT_BORDER_WIDTH);
    testNode->SetBorderColor(borderColor);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    Vector4f bounds2 = { 0, 1200, 400, 400 };
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds2);
    RegisterNode(testNode2);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetBorderStyle(STYLE_DOTTED);
    testNode2->SetBorderWidth(DEFAULT_BORDER_WIDTH);
    testNode2->SetBorderColor(borderColor);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 0, 1900, 400, 400 };
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(testNode3);
    testNode3->SetTranslate({ 0, 0 });
    testNode3->SetBorderStyle(STYLE_NONE);
    testNode3->SetBorderWidth(DEFAULT_BORDER_WIDTH);
    testNode3->SetBorderColor(borderColor);
    DoAnimation(testNode3, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PixelStretch01
 * @tc.desc: test self draw rect changed, add pixel stretch
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, PixelStretch01)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH, Drawing::TileMode::CLAMP);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PixelStretch02
 * @tc.desc: test self draw rect changed, add pixel stretch with different size
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, PixelStretch02)
{
    std::vector<Vector4f> pixelStretchList = { { 0, 20, 0, 0 }, { 0, 0, 40, 0 }, { 0, 0, 0, 60 } };
    
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetPixelStretch(pixelStretchList[0], Drawing::TileMode::CLAMP);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    Vector4f bounds2 = { 0, 1200, 400, 400 };
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds2);
    RegisterNode(testNode2);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetPixelStretch(pixelStretchList[1], Drawing::TileMode::CLAMP);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 0, 1900, 400, 400 };
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(testNode3);
    testNode3->SetTranslate({ 0, 0 });
    testNode3->SetPixelStretch(pixelStretchList[2], Drawing::TileMode::CLAMP);
    DoAnimation(testNode3, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: PixelStretch03
 * @tc.desc: test self draw rect changed, add pixel stretch with different mode
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, PixelStretch03)
{
    std::vector<Drawing::TileMode> modeList = { Drawing::TileMode::REPEAT,
        Drawing::TileMode::MIRROR, Drawing::TileMode::DECAL };
    
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetPixelStretch(DEFAULT_PIXEL_STRETCH, modeList[0]);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    Vector4f bounds2 = { 0, 1200, 400, 400 };
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds2);
    RegisterNode(testNode2);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetPixelStretch(DEFAULT_PIXEL_STRETCH, modeList[1]);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    Vector4f bounds3 = { 0, 1900, 400, 400 };
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds3);
    RegisterNode(testNode3);
    testNode3->SetTranslate({ 0, 0 });
    testNode3->SetPixelStretch(DEFAULT_PIXEL_STRETCH, modeList[2]);
    DoAnimation(testNode3, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);
    GetRootNode()->AddChild(testNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Foreground01
 * @tc.desc: test self draw rect changed, add foreground
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Foreground01)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetForegroundEffectRadius(DEFAULT_FOREGROUND_RADIUS);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}

/*
 * @tc.name: Foreground02
 * @tc.desc: test self draw rect changed, add foreground with different radius
 * @tc.type: FUNC
 * @tc.require: issueICLUP1
 */
GRAPHIC_N_TEST(DirtyRegionTest03, CONTENT_DISPLAY_TEST, Foreground02)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", DEFAULT_BOUNDS);
    RegisterNode(testNode);
    testNode->SetTranslate({ 0, 0 });
    testNode->SetForegroundEffectRadius(DEFAULT_FOREGROUND_RADIUS);
    testNode->SetForegroundColor(COLOR_BLUE);
    DoAnimation(testNode, DEFAULT_TRANSLATE);

    Vector4f bounds2 = { 0, 1200, 400, 400 };
    float radius = 50.0f;
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/dr_test.jpg", bounds2);
    RegisterNode(testNode2);
    testNode2->SetTranslate({ 0, 0 });
    testNode2->SetForegroundEffectRadius(radius);
    testNode2->SetForegroundColor(COLOR_BLUE);
    DoAnimation(testNode2, DEFAULT_TRANSLATE);

    GetRootNode()->AddChild(testNode);
    GetRootNode()->AddChild(testNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    TestCaseCapture();
}
} // namespace OHOS::Rosen