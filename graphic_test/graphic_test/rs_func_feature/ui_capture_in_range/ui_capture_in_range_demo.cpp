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

#include "image_source.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"

#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200;
constexpr uint32_t SLEEP_TIME_IN_US = 10000;      // 10ms
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;

class CustomizedUICaptureInRangeCallback : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelMap) override
    {
        if (pixelMap == nullptr) {
            LOGE("CustomizedUICaptureInRangeCallback::OnSurfaceCapture failed to get pixelMap, return nullptr!");
            return;
        }
        isCallbackCalled_ = true;
        const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
        std::string fileName = "/data/local/graphic_test/ui_capture_in_range/";
        namespace fs = std::filesystem;
        if (!fs::exists(fileName)) {
            if (!fs::create_directories(fileName)) {
                LOGE("CustomizedUICaptureInRangeCallback::OnSurfaceCapture create dir failed");
            }
        } else {
            if (!fs::is_directory(fileName)) {
                LOGE("CustomizedUICaptureInRangeCallback::OnSurfaceCapture path is not dir");
                return;
            }
        }
        fileName += testInfo->test_case_name() + std::string("_");
        fileName += testInfo->name() + std::string(".png");
        if (std::filesystem::exists(fileName)) {
            LOGW("CustomizedUICaptureInRangeCallback::OnSurfaceCapture file exists %{public}s", fileName.c_str());
        }
        if (!WriteToPngWithPixelMap(fileName, *pixelMap)) {
            LOGE("CustomizedUICaptureInRangeCallback::OnSurfaceCapture write image failed %{public}s-%{public}s",
                testInfo->test_case_name(), testInfo->name());
        }
    }
    void OnSurfaceCaptureHDR(
        std::shared_ptr<Media::PixelMap> pixelmap, std::shared_ptr<Media::PixelMap> pixelMapHDR) override
    {}
    bool isCallbackCalled_ = false;
};
} // namespace

class RSUICaptureInRangeTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        auto size = GetScreenSize();
        SetSurfaceBounds({ 0, 0, size.x_ / 2.0f, size.y_ / 2.0f });
        SetSurfaceColor(RSColor(0xffff0000));
    }

    // called after each tests
    void AfterEach() override {}

    bool CheckUICaptureInRangeCallback(std::shared_ptr<CustomizedUICaptureInRangeCallback> callback)
    {
        if (!callback) {
            return false;
        }

        uint32_t times = 0;
        while (times < MAX_TIME_WAITING_FOR_CALLBACK) {
            if (callback->isCallbackCalled_) {
                return true;
            }
            usleep(SLEEP_TIME_IN_US);
            ++times;
        }
        return false;
    }

    std::shared_ptr<Media::PixelMap> DecodePixelMap(
        const std::string& pathName, const Media::AllocatorType& allocatorType)
    {
        uint32_t errCode = 0;
        std::unique_ptr<Media::ImageSource> imageSource =
            Media::ImageSource::CreateImageSource(pathName, Media::SourceOptions(), errCode);
        if (imageSource == nullptr || errCode != 0) {
            return nullptr;
        }

        Media::DecodeOptions decodeOpt;
        decodeOpt.allocatorType = allocatorType;
        std::shared_ptr<Media::PixelMap> pixelmap = imageSource->CreatePixelMap(decodeOpt, errCode);
        if (pixelmap == nullptr || errCode != 0) {
            return nullptr;
        }
        return pixelmap;
    }

    RSSurfaceNode::SharedPtr CreateTestSurfaceNode()
    {
        struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
        rsSurfaceNodeConfig.SurfaceNodeName = "AppWindow";
        rsSurfaceNodeConfig.isSync = false;
        RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, rsSurfaceNodeType);
        surfaceNode->SetBounds({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        surfaceNode->SetFrame({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        surfaceNode->SetBackgroundColor(SK_ColorBLACK);
        return surfaceNode;
    }

    RSCanvasNode::SharedPtr CreateCanvasNode(const Vector4f& bounds)
    {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds(bounds);
        canvasNode->SetFrame(bounds);
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        return canvasNode;
    }

    RSEffectNode::SharedPtr CreateEffectNode(const Vector4f& bounds, float blurRadiusX, float blurRadiusY)
    {
        auto effectNode = RSEffectNode::Create();
        effectNode->SetBounds(bounds);
        effectNode->SetFrame(bounds);
        effectNode->SetBackgroundColor(SK_ColorBLUE);
        effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY));
        return effectNode;
    }

    void DrawTextBlob(ExtendRecordingCanvas* recordingCanvas, const char* str, Drawing::Color color, scalar textSize,
        scalar x, scalar y)
    {
        Drawing::Font font = Drawing::Font();
        font.SetSize(textSize);
        Drawing::Brush brush;
        brush.SetColor(color);
        brush.SetAntiAlias(true);
        recordingCanvas->AttachBrush(brush);
        std::shared_ptr<Drawing::TextBlob> textblob =
            Drawing::TextBlob::MakeFromString(str, font, Drawing::TextEncoding::UTF8);
        recordingCanvas->DrawTextBlob(textblob.get(), x, y);
        recordingCanvas->DetachBrush();
    }
};

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_001
 * @tc.desc: test ui capture in range function of node drawTextblob and alpha
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_001)
{
    auto surfaceNode = CreateTestSurfaceNode();

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 500, 700 });
    canvasNode1->SetFrame({ 0, 0, 500, 700 });
    canvasNode1->SetBackgroundColor(SK_ColorYELLOW);
    canvasNode1->SetAlpha(0.5f);
    surfaceNode->AddChild(canvasNode1, -1);

    auto canvasNode2 = RSCanvasNode::Create();
    canvasNode2->SetBounds({ 0, 0, 500, 700 });
    canvasNode2->SetFrame({ 0, 0, 500, 700 });
    canvasNode2->SetBackgroundColor(SK_ColorBLUE);
    canvasNode1->AddChild(canvasNode2, -1);
    auto recordingCanvas = canvasNode2->BeginRecording(500, 700);
    Drawing::Font font = Drawing::Font();
    font.SetSize(30);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob =
        Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    canvasNode2->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode2, true, callback, 1.f, 1.f, false);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_002
 * @tc.desc: test ui capture in range function of node drawTextblob and alpha
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_002)
{
    auto surfaceNode = CreateTestSurfaceNode();

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 500, 700 });
    canvasNode1->SetFrame({ 0, 0, 500, 700 });
    canvasNode1->SetBackgroundColor(SK_ColorYELLOW);
    canvasNode1->SetAlpha(0.5f);
    surfaceNode->AddChild(canvasNode1, -1);

    auto canvasNode2 = RSCanvasNode::Create();
    canvasNode2->SetBounds({ 0, 0, 500, 700 });
    canvasNode2->SetFrame({ 0, 0, 500, 700 });
    canvasNode2->SetBackgroundColor(SK_ColorBLUE);
    canvasNode1->AddChild(canvasNode2, -1);
    auto recordingCanvas = canvasNode2->BeginRecording(500, 700);
    Drawing::Font font = Drawing::Font();
    font.SetSize(30);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob =
        Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    canvasNode2->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode1, true, callback, 1.f, 1.f, false);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_003
 * @tc.desc: test ui capture in range function of node drawTextblob and alpha
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_003)
{
    auto surfaceNode = CreateTestSurfaceNode();

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 500, 700 });
    canvasNode1->SetFrame({ 0, 0, 500, 700 });
    canvasNode1->SetBackgroundColor(SK_ColorYELLOW);
    canvasNode1->SetAlpha(0.5f);
    surfaceNode->AddChild(canvasNode1, -1);

    auto canvasNode2 = RSCanvasNode::Create();
    canvasNode2->SetBounds({ 0, 0, 500, 700 });
    canvasNode2->SetFrame({ 0, 0, 500, 700 });
    canvasNode2->SetBackgroundColor(SK_ColorBLUE);
    canvasNode1->AddChild(canvasNode2, -1);
    auto recordingCanvas = canvasNode2->BeginRecording(500, 700);
    Drawing::Font font = Drawing::Font();
    font.SetSize(30);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob =
        Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    canvasNode2->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode1, true, callback, 1.2f, 1.2f, false);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_004
 * @tc.desc: test ui capture in range function of node drawTextblob and alpha
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_004)
{
    auto surfaceNode = CreateTestSurfaceNode();

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 500, 700 });
    canvasNode1->SetFrame({ 0, 0, 500, 700 });
    canvasNode1->SetBackgroundColor(SK_ColorYELLOW);
    canvasNode1->SetAlpha(0.5f);
    surfaceNode->AddChild(canvasNode1, -1);

    auto canvasNode2 = RSCanvasNode::Create();
    canvasNode2->SetBounds({ 0, 0, 500, 700 });
    canvasNode2->SetFrame({ 0, 0, 500, 700 });
    canvasNode2->SetBackgroundColor(SK_ColorBLUE);
    canvasNode1->AddChild(canvasNode2, -1);
    auto recordingCanvas = canvasNode2->BeginRecording(500, 700);
    Drawing::Font font = Drawing::Font();
    font.SetSize(30);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob =
        Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    canvasNode2->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode1, canvasNode2, false, callback, 1.f, 1.f, false);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_005
 * @tc.desc: test ui capture in range function of node draw pixelmap and blur
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_005)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 600, 600 });
    canvasNode->SetFrame({ 0, 0, 600, 600 });
    canvasNode->SetBackgroundColor(0x5500ff00);
    canvasNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));
    surfaceNode->AddChild(canvasNode, -1);

    auto allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    std::shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test_bg.jpg", allocatorType);
    if (bgpixelmap == nullptr) {
        return;
    }
    canvasNode->SetBgImageWidth(500);
    canvasNode->SetBgImageHeight(600);
    canvasNode->SetBgImagePositionX(0);
    canvasNode->SetBgImagePositionY(0);

    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(bgpixelmap);
    rosenImage->SetImageRepeat(3);
    rosenImage->SetImageFit(static_cast<int>(ImageFit::FILL));
    canvasNode->SetBgImage(rosenImage);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_006
 * @tc.desc: test ui capture in range function of node set rotation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_006)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetRotation(45);
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 600, 600 });
    canvasNode->SetFrame({ 0, 0, 600, 600 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 100, 100, 150, 150 });
    childNode->SetFrame({ 100, 100, 150, 150 });
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetRotation(45);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, childNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_007
 * @tc.desc: test ui capture in range function of node set rotation and draw text
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_007)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetRotation(45);
    GetRootNode()->SetTestSurface(surfaceNode);

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 600, 600 });
    canvasNode->SetFrame({ 0, 0, 600, 600 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    surfaceNode->AddChild(canvasNode, -1);

    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 100, 100, 150, 150 });
    childNode->SetFrame({ 100, 100, 150, 150 });
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetRotation(45);
    canvasNode->AddChild(childNode);

    auto textNode = RSCanvasNode::Create();
    textNode->SetBounds({ 0, 0, 500, 700 });
    textNode->SetFrame({ 0, 0, 500, 700 });
    textNode->SetBackgroundColor(SK_ColorBLUE);
    childNode->AddChild(textNode, -1);
    auto recordingCanvas = textNode->BeginRecording(500, 700);
    Drawing::Font font = Drawing::Font();
    font.SetSize(30);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob =
        Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    textNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, childNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
    RegisterNode(textNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_008
 * @tc.desc: test ui capture in range function of node set rotation and draw text
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_008)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetRotation(45);
    GetRootNode()->SetTestSurface(surfaceNode);

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 600, 600 });
    canvasNode->SetFrame({ 0, 0, 600, 600 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    surfaceNode->AddChild(canvasNode, -1);

    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 100, 100, 150, 150 });
    childNode->SetFrame({ 100, 100, 150, 150 });
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetRotation(45);
    canvasNode->AddChild(childNode);

    auto textNode = RSCanvasNode::Create();
    textNode->SetBounds({ 0, 0, 500, 700 });
    textNode->SetFrame({ 0, 0, 500, 700 });
    textNode->SetBackgroundColor(SK_ColorBLUE);
    childNode->AddChild(textNode, -1);
    auto recordingCanvas = textNode->BeginRecording(500, 700);
    Drawing::Font font = Drawing::Font();
    font.SetSize(30);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob =
        Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    textNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, textNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
    RegisterNode(textNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_009
 * @tc.desc: test ui capture in range function of node set rotation and draw text
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_009)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetRotation(45);
    GetRootNode()->SetTestSurface(surfaceNode);

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 600, 600 });
    canvasNode->SetFrame({ 0, 0, 600, 600 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    surfaceNode->AddChild(canvasNode, -1);

    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 100, 100, 150, 150 });
    childNode->SetFrame({ 100, 100, 150, 150 });
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetRotation(45);
    canvasNode->AddChild(childNode);

    auto textNode = RSCanvasNode::Create();
    textNode->SetBounds({ 0, 0, 500, 700 });
    textNode->SetFrame({ 0, 0, 500, 700 });
    textNode->SetBackgroundColor(SK_ColorBLUE);
    childNode->AddChild(textNode, -1);
    auto recordingCanvas = textNode->BeginRecording(500, 700);
    Drawing::Font font = Drawing::Font();
    font.SetSize(30);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob =
        Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    textNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode, textNode, false, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
    RegisterNode(textNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_010
 * @tc.desc: test ui capture in range function of node set rotation and draw text
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_010)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetRotation(45);
    GetRootNode()->SetTestSurface(surfaceNode);

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 600, 600 });
    canvasNode->SetFrame({ 0, 0, 600, 600 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    surfaceNode->AddChild(canvasNode, -1);

    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 100, 100, 150, 150 });
    childNode->SetFrame({ 100, 100, 150, 150 });
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetRotation(45);
    canvasNode->AddChild(childNode);

    auto textNode = RSCanvasNode::Create();
    textNode->SetBounds({ 0, 0, 500, 700 });
    textNode->SetFrame({ 0, 0, 500, 700 });
    textNode->SetBackgroundColor(SK_ColorBLUE);
    childNode->AddChild(textNode, -1);
    auto recordingCanvas = textNode->BeginRecording(500, 700);
    Drawing::Font font = Drawing::Font();
    font.SetSize(30);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob =
        Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    textNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode, textNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
    RegisterNode(textNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_011
 * @tc.desc: test ui capture in range function of node set rotation and draw text
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_011)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetRotation(45);
    GetRootNode()->SetTestSurface(surfaceNode);

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 600, 600 });
    canvasNode->SetFrame({ 0, 0, 600, 600 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    surfaceNode->AddChild(canvasNode, -1);

    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 100, 100, 150, 150 });
    childNode->SetFrame({ 100, 100, 150, 150 });
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetRotation(45);
    canvasNode->AddChild(childNode);

    auto textNode = RSCanvasNode::Create();
    textNode->SetBounds({ 0, 0, 500, 700 });
    textNode->SetFrame({ 0, 0, 500, 700 });
    textNode->SetBackgroundColor(SK_ColorBLUE);
    childNode->AddChild(textNode, -1);
    auto recordingCanvas = textNode->BeginRecording(500, 700);
    Drawing::Font font = Drawing::Font();
    font.SetSize(30);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob =
        Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    textNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode, childNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
    RegisterNode(textNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_012
 * @tc.desc: test ui capture in range function of parent node set translate
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_012)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetTranslate({ 50, 50 });
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 600, 600 });
    canvasNode->SetFrame({ 0, 0, 600, 600 });
    canvasNode->SetTranslate({ 50, 50 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 0, 0, 300, 300 });
    childNode->SetFrame({ 0, 0, 300, 300 });
    childNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, childNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_013
 * @tc.desc: test ui capture in range function of node draw dma pixelmap
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_013)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 600, 600 });
    canvasNode->SetFrame({ 0, 0, 600, 600 });
    canvasNode->SetBackgroundColor(0x5500ff00);
    surfaceNode->AddChild(canvasNode, -1);

    auto allocatorType = Media::AllocatorType::DMA_ALLOC;
    std::shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test_bg.jpg", allocatorType);
    if (bgpixelmap == nullptr) {
        return;
    }
    canvasNode->SetBgImageWidth(500);
    canvasNode->SetBgImageHeight(600);
    canvasNode->SetBgImagePositionX(0);
    canvasNode->SetBgImagePositionY(0);

    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(bgpixelmap);
    rosenImage->SetImageRepeat(3);
    rosenImage->SetImageFit(static_cast<int>(ImageFit::FILL));
    canvasNode->SetBgImage(rosenImage);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_014
 * @tc.desc: test ui capture in range function of effectNode and child set use effect
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_014)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto effectNode = CreateEffectNode({ 0, 0, 400, 500 }, 10, 10);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 0, 0, 200, 300 });
    childNode->SetFrame({ 0, 0, 200, 300 });
    childNode->SetBackgroundColor(SK_ColorBLUE);
    childNode->SetUseEffect(true);
    surfaceNode->AddChild(effectNode, -1);
    effectNode->AddChild(childNode);

    auto allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    std::shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test_bg.jpg", allocatorType);
    if (bgpixelmap == nullptr) {
        return;
    }
    childNode->SetBgImageWidth(200);
    childNode->SetBgImageHeight(300);
    childNode->SetBgImagePositionX(0);
    childNode->SetBgImagePositionY(0);

    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(bgpixelmap);
    rosenImage->SetImageRepeat(3);
    rosenImage->SetImageFit(static_cast<int>(ImageFit::FILL));
    childNode->SetBgImage(rosenImage);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    childNode->SetBackgroundColor(SK_ColorGREEN);
    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, effectNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(childNode);
    RegisterNode(effectNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_015
 * @tc.desc: test ui capture in range function of effectNode and child set use effect
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_015)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto effectNode = RSEffectNode::Create();
    effectNode->SetBounds({ 0, 0, 400, 500 });
    effectNode->SetFrame({ 0, 0, 400, 500 });
    effectNode->SetBackgroundColor(SK_ColorYELLOW);
    effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 0, 0, 200, 300 });
    childNode->SetFrame({ 0, 0, 200, 300 });
    childNode->SetBackgroundColor(SK_ColorBLUE);
    childNode->SetUseEffect(true);
    surfaceNode->AddChild(effectNode, -1);
    effectNode->AddChild(childNode);

    auto allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    std::shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test_bg.jpg", allocatorType);
    if (bgpixelmap == nullptr) {
        return;
    }
    childNode->SetBgImageWidth(200);
    childNode->SetBgImageHeight(300);
    childNode->SetBgImagePositionX(0);
    childNode->SetBgImagePositionY(0);

    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(bgpixelmap);
    rosenImage->SetImageRepeat(3);
    rosenImage->SetImageFit(static_cast<int>(ImageFit::FILL));
    childNode->SetBgImage(rosenImage);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    childNode->SetBackgroundColor(SK_ColorGREEN);
    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, childNode, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(childNode);
    RegisterNode(effectNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_016
 * @tc.desc: test ui capture in range function of effectNode and child set use effect
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_016)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto effectNode = RSEffectNode::Create();
    effectNode->SetBounds({ 0, 0, 400, 500 });
    effectNode->SetFrame({ 0, 0, 400, 500 });
    effectNode->SetBackgroundColor(SK_ColorYELLOW);
    effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 0, 0, 200, 300 });
    childNode->SetFrame({ 0, 0, 200, 300 });
    childNode->SetBackgroundColor(SK_ColorBLUE);
    childNode->SetUseEffect(true);
    surfaceNode->AddChild(effectNode, -1);
    effectNode->AddChild(childNode);

    auto allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    std::shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test_bg.jpg", allocatorType);
    if (bgpixelmap == nullptr) {
        return;
    }
    childNode->SetBgImageWidth(200);
    childNode->SetBgImageHeight(300);
    childNode->SetBgImagePositionX(0);
    childNode->SetBgImagePositionY(0);

    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(bgpixelmap);
    rosenImage->SetImageRepeat(3);
    rosenImage->SetImageFit(static_cast<int>(ImageFit::FILL));
    childNode->SetBgImage(rosenImage);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    childNode->SetBackgroundColor(SK_ColorGREEN);
    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, effectNode, true, callback, 1.2f, 1.5f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(effectNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_017
 * @tc.desc: test ui capture in range function of node of blur of brightness 1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_017)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ x, y, sizeX, sizeY });
    canvasNode->SetFrame({ x, y, sizeX, sizeY });
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    float brightness = 1;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, testNodeBackGround, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_018
 * @tc.desc: test ui capture in range function of node of blur of brightness 100
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_018)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ x, y, sizeX, sizeY });
    canvasNode->SetFrame({ x, y, sizeX, sizeY });
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    float brightness = 100;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, testNodeBackGround, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_019
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_019)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode1, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_020
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_020)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode2, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_021
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_021)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode3, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_022
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_022)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode4, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_023
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_023)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode1, canvasNode3, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_024
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_024)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode2, canvasNode4, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_025
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_025)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode2, testNodeBackGround, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_026
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_026)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, testNodeBackGround, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_027
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_027)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode3, true, callback, 1.f, 1.f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_028
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_028)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(
        surfaceNode, testNodeBackGround, false, callback, 1.5f, 1.5f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_029
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_029)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, testNodeBackGround, true, callback, 1.5f, 1.5f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_030
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_030)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    surfaceNode->AddChild(canvasNode2, -1);
    canvasNode1->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode2->AddChild(canvasNode4, -1);
    float brightness = 100;
    auto sizeX = SCREEN_WIDTH - 10;
    auto sizeY = SCREEN_HEIGHT - 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(
        surfaceNode, testNodeBackGround, false, callback, 2.0f, 2.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_031
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_031)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode5, false, callback, 2.0f, 2.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_032
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_032)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode5, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_033
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_033)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, testNodeBackGround, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_034
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_034)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode4, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_035
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_035)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode3, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_036
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_036)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode2, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_037
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_037)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode1, canvasNode2, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_038
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_038)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode1, canvasNode3, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_039
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_039)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode1, canvasNode4, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_040
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_040)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode1, testNodeBackGround, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_041
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_041)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode1, canvasNode5, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_042
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_042)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode1, canvasNode5, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_043
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_043)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode2, canvasNode5, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_044
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_044)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode2, canvasNode4, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_045
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_045)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode2, canvasNode3, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_046
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_046)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode2, testNodeBackGround, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_047
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_047)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode2, canvasNode2, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_048
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_048)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode3, canvasNode5, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_049
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_049)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode3, testNodeBackGround, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_050
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_050)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode3, canvasNode4, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_051
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_051)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    canvasNode2->SetBorderWidth(2);
    canvasNode2->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    canvasNode4->SetBackgroundBlurBrightness(15);
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode4, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_052
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_052)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    canvasNode2->SetBorderWidth(2);
    canvasNode2->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    canvasNode4->SetBackgroundBlurBrightness(15);
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode3, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_053
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_053)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    canvasNode2->SetBorderWidth(2);
    canvasNode2->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    canvasNode4->SetBackgroundBlurBrightness(15);
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, canvasNode5, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_054
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_054)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    canvasNode2->SetBorderWidth(2);
    canvasNode2->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    canvasNode4->SetBackgroundBlurBrightness(15);
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(surfaceNode, testNodeBackGround, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_055
 * @tc.desc: test ui capture in range function of node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureInRangeTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_IN_RANGE_055)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode1 = CreateCanvasNode({ 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20 });
    auto canvasNode2 = CreateCanvasNode({ 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100 });
    canvasNode2->SetBackgroundColor(SK_ColorGREEN);
    canvasNode2->SetRotation(60);
    canvasNode2->SetBorderWidth(2);
    canvasNode2->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
    auto canvasNode3 = CreateCanvasNode({ 0, 0, 500, 500 });
    auto recordingCanvas = canvasNode3->BeginRecording(500, 500);
    DrawTextBlob(recordingCanvas, "CanvasNode3", SK_ColorYELLOW, 20, 80, 80);
    canvasNode3->FinishRecording();
    surfaceNode->AddChild(canvasNode1, -1);
    canvasNode1->AddChild(canvasNode2, -1);
    canvasNode2->AddChild(canvasNode3, -1);
    auto canvasNode4 = CreateCanvasNode({ 0, 0, 100, 100 });
    canvasNode4->SetBackgroundBlurBrightness(15);
    recordingCanvas = canvasNode4->BeginRecording(100, 100);
    DrawTextBlob(recordingCanvas, "canvasNode4", SK_ColorGREEN, 10, 70, 70);
    canvasNode4->FinishRecording();
    canvasNode3->AddChild(canvasNode4, -1);
    float brightness = 30;
    auto sizeX = 200;
    auto sizeY = 200;
    int x = 60;
    int y = 60;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    canvasNode4->AddChild(testNodeBackGround, -1);
    auto canvasNode5 = CreateCanvasNode({ 0, 0, 300, 300 });
    recordingCanvas = canvasNode5->BeginRecording(300, 300);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    recordingCanvas->AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(150, 150);
    recordingCanvas->DrawLine(startPt, endPt);
    recordingCanvas->DetachPen();
    canvasNode5->FinishRecording();
    testNodeBackGround->AddChild(canvasNode5, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedUICaptureInRangeCallback>();
    RSInterfaces::GetInstance().TakeUICaptureInRange(canvasNode1, canvasNode5, true, callback, 1.0f, 1.0f, true);
    CheckUICaptureInRangeCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode1);
    RegisterNode(canvasNode2);
    RegisterNode(canvasNode3);
    RegisterNode(canvasNode4);
    RegisterNode(canvasNode5);
    RegisterNode(testNodeBackGround);
}
} // namespace OHOS::Rosen
