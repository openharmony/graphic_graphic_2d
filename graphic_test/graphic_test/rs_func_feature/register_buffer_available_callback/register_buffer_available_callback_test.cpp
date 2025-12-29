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
#include "ui/rs_root_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;


class RSRegisterBufferAvailableCallbackTest : public RSGraphicTest {
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
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
 * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 300, 300 });
        canvasNode->SetFrame({ 0, 0, 300, 300 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        rootNode->AddChild(canvasNode, -1);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);

        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_02
 * @tc.desc: test RegisterBufferAvailableCallback function of node set alpha
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_02)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 300, 300 });
        canvasNode->SetFrame({ 0, 0, 300, 300 });
        canvasNode->SetBackgroundColor(SK_ColorGREEN);
        canvasNode->SetAlpha(0.5f);
        rootNode->AddChild(canvasNode, -1);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_03
 * @tc.desc: test RegisterBufferAvailableCallback function of node setblendmode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_03)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 100, 100, 500, 700 });
        canvasNode->SetFrame({ 100, 100, 500, 700 });
        canvasNode->SetBackgroundColor(0xc1cbd700);
        canvasNode->SetColorBlendMode(RSColorBlendMode::XOR);
        rootNode->AddChild(canvasNode, -1);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_04
 * @tc.desc: test RegisterBufferAvailableCallback function of node setrotation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_04)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 100, 100, 300, 300 });
        canvasNode->SetFrame({ 100, 100, 300, 300 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        canvasNode->SetRotation(45);
        rootNode->AddChild(canvasNode, -1);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_05
 * @tc.desc: test RegisterBufferAvailableCallback function of node drawpixelmap and blur
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_05)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 700, 700 });
        canvasNode->SetFrame({ 0, 0, 700, 700 });
        canvasNode->SetBackgroundColor(0xc1cbd700);
        canvasNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));
        rootNode->AddChild(canvasNode, -1);

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

        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_06
 * @tc.desc: test RegisterBufferAvailableCallback function of node drawTextblob
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_06)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 800, 800 });
        canvasNode->SetFrame({ 0, 0, 800, 800 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        rootNode->AddChild(canvasNode, -1);
        auto recordingCanvas = canvasNode->BeginRecording(500, 700);
        Drawing::Font font = Drawing::Font();
        font.SetSize(30);
        Drawing::Brush brush;
        brush.SetColor(SK_ColorGREEN);
        brush.SetAntiAlias(true);
        recordingCanvas->AttachBrush(brush);
        std::shared_ptr<Drawing::TextBlob> textblob =
            Drawing::TextBlob::MakeFromString("Hello World", font, Drawing::TextEncoding::UTF8);
        recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
        recordingCanvas->DetachBrush();
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_07
 * @tc.desc: test RegisterBufferAvailableCallback function of node set translate
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_07)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        canvasNode->SetCornerRadius({10.0, 10.0, 10.0, 10.0});
        rootNode->AddChild(canvasNode, -1);

        auto allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
        std::shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test_trans.jpg", allocatorType);
        if (bgpixelmap == nullptr) {
            return;
        }
        canvasNode->SetBgImageWidth(500);
        canvasNode->SetBgImageHeight(600);
        canvasNode->SetBgImagePositionX(0);
        canvasNode->SetBgImagePositionY(0);

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetPixelMap(bgpixelmap);
        rosenImage->SetImageFit(static_cast<int>(ImageFit::FILL));
        canvasNode->SetBgImage(rosenImage);
        canvasNode->SetTranslateX(50);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_08
 * @tc.desc: test RegisterBufferAvailableCallback function of node set scale
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_08)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        canvasNode->SetScale(0.5);
        rootNode->AddChild(canvasNode, -1);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_09
 * @tc.desc: test RegisterBufferAvailableCallback function of node dma image
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_09)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 300, 300 });
        canvasNode->SetFrame({ 0, 0, 300, 300 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        rootNode->AddChild(canvasNode, -1);

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
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_10
 * @tc.desc: test RegisterBufferAvailableCallback function of node set border
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_10)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 300, 300 });
        canvasNode->SetFrame({ 0, 0, 300, 300 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        canvasNode->SetBorderStyle(0, 0, 0, 0);
        canvasNode->SetBorderColor(SK_ColorGREEN);
        canvasNode->SetBorderWidth({8, 8, 8, 8});
        rootNode->AddChild(canvasNode, -1);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_11
 * @tc.desc: test RegisterBufferAvailableCallback function of node :2 node set for parent
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_11)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 600, 600 });
        canvasNode->SetFrame({ 0, 0, 600, 600 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        rootNode->AddChild(canvasNode, -1);
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ 100, 100, 300, 300 });
        childNode->SetFrame({ 100, 100, 300, 300 });
        childNode->SetBackgroundColor(SK_ColorBLUE);
        canvasNode->AddChild(childNode);
        canvasNode->SetBackgroundColor(SK_ColorYELLOW);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(childNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_12
 * @tc.desc: test RegisterBufferAvailableCallback function of node set property after flush
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_12)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 600, 600 });
        canvasNode->SetFrame({ 0, 0, 600, 600 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        rootNode->AddChild(canvasNode, -1);
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ 100, 100, 300, 300 });
        childNode->SetFrame({ 100, 100, 300, 300 });
        childNode->SetBackgroundColor(SK_ColorBLUE);
        canvasNode->AddChild(childNode);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);

        canvasNode->SetBackgroundColor(SK_ColorGREEN);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(childNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_13
 * @tc.desc: test RegisterBufferAvailableCallback function of node effect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_13)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        rootNode->AddChild(canvasNode, -1);

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
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(effectNode);
        RegisterNode(childNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_14
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur brigtness 1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_14)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBackgroundBlurBrightness(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_15
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur brigtness 200
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_15)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(8, 8, 8, 8);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(200);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_16
 * @tc.desc: test RegisterBufferAvailableCallback function of node bluemask blue
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_16)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0x9ca8b800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(0.6);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_17
 * @tc.desc: test RegisterBufferAvailableCallback function of node bluemask green
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_17)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0x7b8b6f00);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(0.6);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(0);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_18
 * @tc.desc: test RegisterBufferAvailableCallback function of node bluemask yellow
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_18)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(0.6);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_19
 * @tc.desc: test RegisterBufferAvailableCallback function of node material filter
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_19)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 700, 700 });
        canvasNode->SetFrame({ 0, 0, 700, 700 });
        canvasNode->SetBackgroundColor(0xc1cbd700);
        canvasNode->SetBackgroundFilter(RSFilter::CreateMaterialFilter(6, 6, 6, 0xb5c4b100, BLUR_COLOR_MODE::AVERAGE));
        rootNode->AddChild(canvasNode, -1);

        auto allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
        std::shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test_bg.jpg", allocatorType);
        if (bgpixelmap == nullptr) {
            return;
        }
        canvasNode->SetBgImageWidth(700);
        canvasNode->SetBgImageHeight(700);
        canvasNode->SetBgImagePositionX(0);
        canvasNode->SetBgImagePositionY(0);

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetPixelMap(bgpixelmap);
        rosenImage->SetImageRepeat(1);
        rosenImage->SetImageFit(static_cast<int>(ImageFit::FILL));
        canvasNode->SetBgImage(rosenImage);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);

        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_20
 * @tc.desc: test RegisterBufferAvailableCallback function of node set parent property2
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_20)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 600, 600 });
        canvasNode->SetFrame({ 0, 0, 600, 600 });
        canvasNode->SetBackgroundColor(SK_ColorRED);
        rootNode->AddChild(canvasNode, -1);
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ 100, 100, 300, 300 });
        childNode->SetFrame({ 100, 100, 300, 300 });
        childNode->SetBackgroundColor(SK_ColorBLUE);
        canvasNode->AddChild(childNode);
        canvasNode->SetBackgroundColor(SK_ColorYELLOW);
        auto childNode1 = RSCanvasNode::Create();
        childNode1->SetBounds({ 100, 100, 200, 200 });
        childNode1->SetFrame({ 100, 100, 200, 200 });
        childNode1->SetBackgroundColor(SK_ColorGREEN);
        childNode->AddChild(childNode1);
        childNode->SetBackgroundColor(SK_ColorWHITE);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(childNode);
        RegisterNode(childNode1);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_21
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur saturation 0.2
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_21)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(0.2);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_22
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur saturation 0.4
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_22)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(0.4);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_23
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur saturation 0.8
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_23)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(0.8);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_24
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur saturation 1.0
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_24)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(1.0);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_25
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur saturation 1.2
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_25)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(1.2);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_26
 * @tc.desc: test RegisterBufferAvailableCallback function of node alpha 0.2
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_26)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(1.2);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);
        testNodeBackGround->SetAlpha(0.2);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_27
 * @tc.desc: test RegisterBufferAvailableCallback function of node alpha 0.0
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_27)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(1.2);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);
        testNodeBackGround->SetAlpha(0.0);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_28
 * @tc.desc: test RegisterBufferAvailableCallback function of node alpha 0.5
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_28)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(1.2);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);
        testNodeBackGround->SetAlpha(0.5);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_29
 * @tc.desc: test RegisterBufferAvailableCallback function of node alpha 0.8
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_29)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        Color color = Color(0xf8ebd800);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurBrightness(0.6);
        testNodeBackGround->SetBackgroundBlurSaturation(1.2);
        testNodeBackGround->SetBackgroundBlurMaskColor(color);
        testNodeBackGround->SetBackgroundBlurColorMode(1);
        testNodeBackGround->SetAlpha(0.8);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_30
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radius
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_30)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadius(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_31
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radius 10
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_31)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadius(10);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_32
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radius40
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_32)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadius(40);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_33
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radius100
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_33)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadius(100);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_34
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radiusXY
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_34)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadiusX(1);
        testNodeBackGround->SetBackgroundBlurRadiusY(10);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_35
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radiusXY1 100
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_35)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadiusX(1);
        testNodeBackGround->SetBackgroundBlurRadiusY(100);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_36
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radiusXY100 1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_36)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadiusX(100);
        testNodeBackGround->SetBackgroundBlurRadiusY(1);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_37
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radiusXY10 50
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_37)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadiusX(10);
        testNodeBackGround->SetBackgroundBlurRadiusY(50);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_38
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radiusXY50 10
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_38)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadiusX(50);
        testNodeBackGround->SetBackgroundBlurRadiusY(10);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_39
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radiusXY20 100
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_39)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadiusX(20);
        testNodeBackGround->SetBackgroundBlurRadiusY(100);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_40
 * @tc.desc: test RegisterBufferAvailableCallback function of node blur radiusXY100 20
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_40)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetBackgroundBlurRadiusX(100);
        testNodeBackGround->SetBackgroundBlurRadiusY(20);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_41
 * @tc.desc: test RegisterBufferAvailableCallback function of node set sepia
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_41)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetSepia(0.2);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_42
 * @tc.desc: test RegisterBufferAvailableCallback function of node set sepia
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_42)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetSepia(0.4);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_43
 * @tc.desc: test RegisterBufferAvailableCallback function of node set sepia
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_43)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetSepia(0.6);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_44
 * @tc.desc: test RegisterBufferAvailableCallback function of node set sepia
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_44)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetSepia(0.8);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_45
 * @tc.desc: test RegisterBufferAvailableCallback function of node set sepia
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_45)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetSepia(1.0);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_46
 * @tc.desc: test RegisterBufferAvailableCallback function of node set invert
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_46)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetInvert(0.2);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_47
 * @tc.desc: test RegisterBufferAvailableCallback function of node set invert
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_47)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetInvert(0.4);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_48
 * @tc.desc: test RegisterBufferAvailableCallback function of node set invert
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_48)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetInvert(0.6);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_49
 * @tc.desc: test RegisterBufferAvailableCallback function of node set invert
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_49)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetInvert(0.8);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_50
 * @tc.desc: test RegisterBufferAvailableCallback function of node set invert
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_50)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetInvert(1.0);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_51
 * @tc.desc: test RegisterBufferAvailableCallback function of node set cornerradius
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_51)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetCornerRadius(10);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_52
 * @tc.desc: test RegisterBufferAvailableCallback function of node set cornerradius
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_52)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetCornerRadius(40);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_53
 * @tc.desc: test RegisterBufferAvailableCallback function of node set cornerradius
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_53)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetCornerRadius(60);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_54
 * @tc.desc: test RegisterBufferAvailableCallback function of node set cornerradius
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_54)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetCornerRadius(80);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

/*
 * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_55
 * @tc.desc: test RegisterBufferAvailableCallback function of node set cornerradius
 * @tc.type: FUNC
 */
GRAPHIC_TEST(
    RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_55)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();

    auto surfaceNode = CreateTestSurfaceNode();
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

    auto cb = [this, surfaceNode, rootNode]() {
        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({ 0, 0, 900, 900 });
        canvasNode->SetFrame({ 0, 0, 900, 900 });
        canvasNode->SetBackgroundColor(SK_ColorWHITE);
        rootNode->AddChild(canvasNode, -1);

        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/test_bg.jpg", { 0, 0, 800, 800 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        testNodeBackGround->SetCornerRadius(100);

        canvasNode->AddChild(testNodeBackGround);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
        
        // created node should be registered to preserve ref_count
        RegisterNode(canvasNode);
        RegisterNode(testNodeBackGround);
    };
    surfaceNode->SetBufferAvailableCallback(cb);

    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AttachRSSurfaceNode(surfaceNode);

    GetRootNode()->SetTestSurface(surfaceNode);
}

}
} //OHOS::Rosen