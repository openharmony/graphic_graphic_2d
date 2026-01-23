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
constexpr uint32_t SLEEP_TIME_IN_US = 10000; // 10ms
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;

class CustomizedSurfaceCapture : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        if (pixelmap == nullptr) {
            LOGE("CustomizedSurfaceCapture::OnSurfaceCapture failed to get pixelmap, return nullptr!");
            return;
        }
        isCallbackCalled_ = true;
        const ::testing::TestInfo* const testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
        std::string filename = "/data/local/graphic_test/window_freeze_immediately/";
        namespace fs = std::filesystem;
        if (!fs::exists(filename)) {
            if (!fs::create_directories(filename)) {
                LOGE("CustomizedSurfaceCapture::OnSurfaceCapture create dir failed");
            }
        } else {
            if (!fs::is_directory(filename)) {
                LOGE("CustomizedSurfaceCapture::OnSurfaceCapture path is not dir");
                return;
            }
        }
        filename += testInfo->test_case_name() + std::string("_");
        filename += testInfo->name() + std::string(".png");
        if (std::filesystem::exists(filename)) {
            LOGW("CustomizedSurfaceCapture::OnSurfaceCapture file exists %{public}s", filename.c_str());
        }
        if (!WriteToPngWithPixelMap(filename, *pixelmap)) {
            LOGE("CustomizedSurfaceCapture::OnSurfaceCapture write image failed %{public}s-%{public}s",
                testInfo->test_case_name(), testInfo->name());
        }
    }

    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelmap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
    bool isCallbackCalled_ = false;
};
}

class RSFreezeWindowTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        auto size = GetScreenSize();
        SetSurfaceBounds({0, 0, size.x_ / 2.0f, size.y_ / 2.0f});
        SetSurfaceColor(RSColor(0xffff0000));
    }

    // called after each tests
    void AfterEach() override {}

    bool CheckSurfaceCaptureCallback(std::shared_ptr<CustomizedSurfaceCapture> callback)
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
        struct RSSurfaceNodeConfig rsSurfaceNodeConfig1;
        rsSurfaceNodeConfig1.SurfaceNodeName = "AppWindow";
        rsSurfaceNodeConfig1.isSync = false;
        RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig1, rsSurfaceNodeType);
        surfaceNode->SetBounds({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        surfaceNode->SetFrame({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        surfaceNode->SetBackgroundColor(SK_ColorBLACK);
        return surfaceNode;
    }
};

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_01
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_01)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 500, 700});
    canvasNode->SetFrame({0, 0, 500, 700});
    canvasNode->SetBackgroundColor(SK_ColorBLUE);
    surfaceNode->AddChild(canvasNode, -1);
    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_02
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_02)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({0, 0, 100, 100});
    canvasNode0->SetFrame({0, 0, 100, 100});
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({0, 0, 200, 200});
    canvasNode1->SetFrame({0, 0, 200, 200});
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({0, 0, 100, 100});
    surfaceNode0->SetFrame({0, 0, 100, 100});

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({0, 0, 200, 200});
    surfaceNode1->SetFrame({0, 0, 200, 200});
    surfaceNode->RSNode::AddChild(surfaceNode0);
    surfaceNode->RSNode::AddChild(surfaceNode1);
    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.blackList.push_back(surfaceNode0->GetId());
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode0);
    RegisterNode(canvasNode1);
}


/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_03
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_03)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 500, 700});
    canvasNode->SetFrame({0, 0, 500, 700});
    canvasNode->SetBackgroundColor(SK_ColorBLUE);
    surfaceNode->AddChild(canvasNode, -1);
    auto recordingCanvas = canvasNode->BeginRecording(500, 700);
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
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_04
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_04)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(0x5500ff00);
    canvasNode->SetColorBlendMode(RSColorBlendMode::XOR);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_05
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_05)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(0x5500ff00);
    canvasNode->SetColorBlendMode(RSColorBlendMode::XOR);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_06
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_06)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(0x5500ff00);
    canvasNode->SetColorBlendMode(RSColorBlendMode::XOR);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_07
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_07)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
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

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_08
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_08)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode02 = RSCanvasNode::Create();
    canvasNode02->SetBounds({0, 0, 200, 200});
    canvasNode02->SetFrame({0, 0, 200, 200});
    canvasNode02->SetBackgroundColor(SK_ColorBLACK);

    surfaceNode->AddChild(canvasNode, -1);
    surfaceNode->AddChild(canvasNode02, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
    RegisterNode(canvasNode02);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_09
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_09)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorRED);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.backGroundColor = Drawing::Color::COLOR_RED;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_10
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_10)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorRED);
    canvasNode->SetRotation(45);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_11
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_11)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode02 = RSCanvasNode::Create();
    canvasNode02->SetBounds({0, 0, 200, 200});
    canvasNode02->SetFrame({0, 0, 200, 200});
    canvasNode02->SetBackgroundColor(SK_ColorBLACK);
    canvasNode02->SetScale(0.5);
    surfaceNode->AddChild(canvasNode, -1);
    surfaceNode->AddChild(canvasNode02, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
    RegisterNode(canvasNode02);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_12
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_12)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode02 = RSCanvasNode::Create();
    canvasNode02->SetBounds({100, 100, 600, 600});
    canvasNode02->SetFrame({100, 100, 600, 600});
    canvasNode02->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(canvasNode02, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
    RegisterNode(canvasNode02);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_13
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_13)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode02 = RSCanvasNode::Create();
    canvasNode02->SetBounds({0, 0, 200, 300});
    canvasNode02->SetFrame({0, 0, 200, 300});
    canvasNode02->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(canvasNode02, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
    RegisterNode(canvasNode02);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_14
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_14)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto effectNode = RSEffectNode::Create();
    effectNode->SetBounds({0, 0, 400, 500});
    effectNode->SetFrame({0, 0, 400, 500});
    effectNode->SetBackgroundColor(SK_ColorYELLOW);
    effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 200, 300});
    canvasNode->SetFrame({0, 0, 200, 300});
    canvasNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(effectNode, -1);
    effectNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(effectNode);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_15
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_15)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto effectNode = RSEffectNode::Create();
    effectNode->SetBounds({0, 0, 400, 500});
    effectNode->SetFrame({0, 0, 400, 500});
    effectNode->SetBackgroundColor(SK_ColorYELLOW);
    effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 200, 300});
    canvasNode->SetFrame({0, 0, 200, 300});
    canvasNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(effectNode, -1);
    effectNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(effectNode);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_16
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_16)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorLTGRAY);

    auto canvasNode02 = RSCanvasNode::Create();
    canvasNode02->SetBounds({100, 100, 600, 600});
    canvasNode02->SetFrame({100, 100, 600, 600});
    canvasNode02->SetBackgroundColor(SK_ColorRED);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(canvasNode02, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
    RegisterNode(canvasNode02);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_17
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_17)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode02 = RSCanvasNode::Create();
    canvasNode02->SetBounds({100, 100, 600, 600});
    canvasNode02->SetFrame({100, 100, 600, 600});
    canvasNode02->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(canvasNode02, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.useDma = true;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
    RegisterNode(canvasNode02);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_18
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_18)
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
    float brightness = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_19
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_19)
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
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_20
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_20)
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
    float brightness = 200;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_21
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_21)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorRED);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.backGroundColor = Drawing::Color::COLOR_YELLOW;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}


/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_22
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_22)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetFreeze(true);
    canvasNode->SetBackgroundColor(SK_ColorRED);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    canvasNode->SetFreeze(false);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_23
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_23)
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
    Color color = Color(0x2dff0000); // Red
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = -1;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_24
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_24)
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
    Color color = Color(0x2dff0000); // Red
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 1;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_25
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_25)
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
    Color color = Color(0x2dff0000); // Red
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 2;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_26
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_26)
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
    Color color = Color(0x4d00ff00);
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_27
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_27)
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
    Color color = Color(0x4d00ff00);
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_28
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_28)
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
    Color color = Color(0x6d0000ff);
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_29
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_29)
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
    Color color = Color(0xff000000);
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_30
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_30)
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
    Color color = Color(0xffffffff);
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_31
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_31)
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
    Color color = Color(0x8d00ffff);
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_32
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_32)
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
    Color color = Color(0xad444444); // Cyan
    int radius = 70;
    float saturation = 1.4;
    float brightness = 1.4;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_33
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_33)
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
    Color color = Color(0xff000000); // BLACK
    int radius = 10;
    float saturation = 0.2;
    float brightness = 0.2;
    float alpha = 0.1;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_34
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_34)
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
    Color color = Color(0xffffffff); // WHITE
    int radius = 20;
    float saturation = 0.4;
    float brightness = 0.4;
    float alpha = 0.2;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_35
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_35)
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
    Color color = Color(0x2dff0000); // RED
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    float alpha = 0.3;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_36
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_36)
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
    Color color = Color(0x4d00ff00); // GREEN
    int radius = 40;
    float saturation = 0.8;
    float brightness = 0.8;
    float alpha = 0.4;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_37
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_37)
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
    Color color = Color(0x6d0000ff); // BLUE
    int radius = 50;
    float saturation = 1.0;
    float brightness = 1.0;
    float alpha = 0.5;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_38
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_38)
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
    Color color = Color(0x8d00ffff); // WHITE
    int radius = 60;
    float saturation = 1.2;
    float brightness = 1.2;
    float alpha = 0.6;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_39
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_39)
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
    Color color = Color(0xad444444); // WHITE
    int radius = 70;
    float saturation = 1.4;
    float brightness = 1.4;
    float alpha = 0.7;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_40
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_40)
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
    Color color = Color(0xcdff00ff); // WHITE
    int radius = 80;
    float saturation = 1.6;
    float brightness = 1.6;
    float alpha = 0.8;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);

    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_41
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_41)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 700, 700});
    canvasNode->SetFrame({0, 0, 700, 700});
    canvasNode->SetBackgroundColor(SK_ColorCYAN);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.backGroundColor = Drawing::Color::COLOR_BLACK;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_42
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_42)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 700, 700});
    canvasNode->SetFrame({0, 0, 700, 700});
    canvasNode->SetBackgroundColor(SK_ColorGREEN);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.backGroundColor = Drawing::Color::COLOR_LTGRAY;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_43
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_43)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 700, 700});
    canvasNode->SetFrame({0, 0, 700, 700});
    canvasNode->SetBackgroundColor(SK_ColorDKGRAY);
    surfaceNode->AddChild(canvasNode, -1);
    surfaceNode->SetRotation(45);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.backGroundColor = Drawing::Color::COLOR_MAGENTA;
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback, captureConfig);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_44
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_44)
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
    float radius = -1;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();

    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_45
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_45)
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
    float radius = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_46
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_46)
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
    canvasNode->SetBackgroundColor(SK_ColorGRAY);
    float radius = 10;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::White()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_47
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_47)
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
    float radius = 20;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::LightGray()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_48
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_48)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetTranslate({ 50, 50 });
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetTranslate({ 50, 50 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_49
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_49)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetTranslateX(50);
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetTranslateX(50);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_49
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_50)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetTranslateY(50);
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetTranslateY(50);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_51
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_51)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetTranslateZ(50);
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetTranslateZ(50);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_52
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_52)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetScale(1.5);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetScale(1.5);
    childNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_53
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_53)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetScale(1.5);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetScale(1.5);
    childNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_54
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_54)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetScaleX(1.5);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetScaleX(1.5);
    childNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}

/*
 * @tc.name: WINDOW_FREEZE_IMMEDIATELY_TEST_55
 * @tc.desc: test for SetWindowFreezeImmediately
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSFreezeWindowTest, CONTENT_DISPLAY_TEST, WINDOW_FREEZE_IMMEDIATELY_TEST_55)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetScaleX(1.5);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetScaleX(1.5);
    childNode->SetBackgroundColor(SK_ColorBLACK);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto surfaceCaptureCallback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().SetWindowFreezeImmediately(surfaceNode, true, surfaceCaptureCallback);
    CheckSurfaceCaptureCallback(surfaceCaptureCallback);
    RegisterNode(canvasNode);
}
}