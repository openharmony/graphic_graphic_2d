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

static void SavePixelToFile(std::shared_ptr<Media::PixelMap> pixelMap)
{
    const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string fileName = "/data/local/graphic_test/solonode_surface_capture/";
    namespace fs = std::filesystem;
    if (!fs::exists(fileName)) {
        if (!fs::create_directories(fileName)) {
            LOGE("CustomizedSurfaceCapture::OnSurfaceCapture create dir failed");
        }
    } else {
        if (!fs::is_directory(fileName)) {
            LOGE("CustomizedSurfaceCapture::OnSurfaceCapture path is not dir");
            return;
        }
    }
    fileName += testInfo->test_case_name() + std::string("_");
    fileName += testInfo->name() + std::string(".png");
    if (std::filesystem::exists(fileName)) {
        LOGW("CustomizedSurfaceCapture::OnSurfaceCapture file exists %{public}s", fileName.c_str());
    }
    if (!WriteToPngWithPixelMap(fileName, *pixelMap)) {
        LOGE("CustomizedSurfaceCapture::OnSurfaceCapture write image failed %{public}s-%{public}s",
            testInfo->test_case_name(), testInfo->name());
    }
}

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
        std::string filename = "/data/local/graphic_test/solonode_surface_capture/";
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

class RSSoloNodeSurfaceCaptureTest : public RSGraphicTest {
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
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_01
 * @tc.desc: test solonode surface capture function of node drawTextblob
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_01)
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
    std::shared_ptr<Drawing::TextBlob> textblob = Drawing::TextBlob::MakeFromString("Hello World",
        font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), 50, 50);
    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_02
 * @tc.desc: test solonode surface capture function of node set alpha and scale
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_02)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 500, 700});
    canvasNode->SetFrame({0, 0, 500, 700});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    canvasNode->SetAlpha(0.5f);
    surfaceNode->AddChild(canvasNode, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_03
 * @tc.desc: test solonode surface capture function of node set blendMode
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_03)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_04
 * @tc.desc: test solonode surface capture function of node draw pixelmap and blur
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_04)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_05
 * @tc.desc: test solonode surface capture function of node set translate
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_05)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetTranslate({ 100, 100 });
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetTranslate({100, 100});
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_06
 * @tc.desc: test solonode surface capture function of node set rotation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_06)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetRotation(45);
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({100, 100, 150, 150});
    childNode->SetFrame({100, 100, 150, 150});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetRotation(45);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_07
 * @tc.desc: test solonode surface capture function of node set scale
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_07)
{
    auto surfaceNode = CreateTestSurfaceNode();
    surfaceNode->SetRotation(45);
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetScale(0.5);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_08
 * @tc.desc: test solonode surface capture function of parent node set translate
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_08)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_9
 * @tc.desc: test solonode surface capture function of node draw dma pixelmap
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_9)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_10
 * @tc.desc: test solonode surface capture function of node draw astc pixelmap
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_10)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(0x5500ff00);
    surfaceNode->AddChild(canvasNode, -1);

    auto allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    std::shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test.astc", allocatorType);
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_11
 * @tc.desc: test solonode surface capture function of node is over parent node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_11)
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig1;
    rsSurfaceNodeConfig1.SurfaceNodeName = "AppWindow";
    rsSurfaceNodeConfig1.isSync = false;
    RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig1, rsSurfaceNodeType);
    surfaceNode->SetBounds({ 0, 0, 400, 500 });
    surfaceNode->SetFrame({ 0, 0, 400, 500 });
    surfaceNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({100, 100, 600, 600});
    childNode->SetFrame({100, 100, 600, 600});
    childNode->SetBackgroundColor(SK_ColorBLUE);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_12
 * @tc.desc: test solonode surface capture function of node set properties before takeUICapture
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_12)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 200, 300});
    childNode->SetFrame({0, 0, 200, 300});
    childNode->SetBackgroundColor(SK_ColorBLUE);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    childNode->SetBackgroundColor(SK_ColorGREEN);
    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_13
 * @tc.desc: test solonode surface capture function of effectNode and child set use effect
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_13)
{
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto effectNode = RSEffectNode::Create();
    effectNode->SetBounds({0, 0, 400, 500});
    effectNode->SetFrame({0, 0, 400, 500});
    effectNode->SetBackgroundColor(SK_ColorYELLOW);
    effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 200, 300});
    childNode->SetFrame({0, 0, 200, 300});
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
    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(childNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(childNode);
    RegisterNode(effectNode);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_14
 * @tc.desc: test solonode surface capture function of node of blur of brightness
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_14)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_15
 * @tc.desc: test solonode surface capture function of node of blur of brightness 1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_15)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_16
 * @tc.desc: test solonode surface capture function of node of blur of brightness 200
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_16)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_17
 * @tc.desc: test solonode surface capture function of node of background blur color node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_17)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_18
 * @tc.desc: test solonode surface capture function of node of background blur color node 2
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_18)
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
    int colorMode = 0;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_19
 * @tc.desc: test solonode surface capture function of node of background green color node 3
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_19)
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
    Color color = Color(0x4d00ff00); // Green()
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_20
 * @tc.desc: test solonode surface capture function of node of background green color node 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_20)
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
    Color color = Color(0x4d00ff00); // Green()
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_21
 * @tc.desc: test solonode surface capture function of node of background green color node 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_21)
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
    Color color = Color(0x4d00ff00); // Green()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 0;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_22
 * @tc.desc: test solonode surface capture function of node of background green color node 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_22)
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
    Color color = Color(0x4d00ff00); // Green()
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_23
 * @tc.desc: test solonode surface capture function of node of background green color node 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_23)
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
    Color color = Color(0x4d00ff00); // Green()
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_24
 * @tc.desc: test solonode surface capture function of node of background green color node 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_24)
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
    Color color = Color(0x4d00ff00); // Green()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 3;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_25
 * @tc.desc: test solonode surface capture function of node of background green color node 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_25)
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
    Color color = Color(0x4d00ff00); // Green()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 100;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_26
 * @tc.desc: test solonode surface capture function of node of background green color node 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_26)
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
    Color color = Color(0x6d0000ff); // Blue()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 100;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_27
 * @tc.desc: test solonode surface capture function of node of blur and mask color
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_27)
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
    Color color = Color(0xff000000); // Black()
    int radius = 10;
    float saturation = 0.2;
    float brightness = 0.2;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_28
 * @tc.desc: test solonode surface capture function of node of blur and mask color 2
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_28)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_29
 * @tc.desc: test solonode surface capture function of node of blur and mask color 3
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_29)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_30
 * @tc.desc: test solonode surface capture function of node of blur and mask color 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_30)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_31
 * @tc.desc: test solonode surface capture function of node of blur and mask color 5
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_31)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_32
 * @tc.desc: test solonode surface capture function of node of blur and mask color 5
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_32)
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
    Color color = Color(0x8d00ffff); // Cyan
    int radius = 60;
    float saturation = 1.2;
    float brightness = 1.2;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_33
 * @tc.desc: test solonode surface capture function of node of blur and mask color 5
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_33)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_34
 * @tc.desc: test solonode surface capture function of node of blur mask color and alpha
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_34)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_35
 * @tc.desc: test solonode surface capture function of node of blur mask color and alpha 2
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_35)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_36
 * @tc.desc: test solonode surface capture function of node of blur mask color and alpha 3
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_36)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_37
 * @tc.desc: test solonode surface capture function of node of blur mask color and alpha 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_37)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_38
 * @tc.desc: test solonode surface capture function of node of blur mask color and alpha 5
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_38)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_39
 * @tc.desc: test solonode surface capture function of node of blur mask color and alpha 6
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_39)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_40
 * @tc.desc: test solonode surface capture function of node of blur mask color and alpha 7
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_40)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_41
 * @tc.desc: test solonode surface capture function of node of blur mask color and alpha 8
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_41)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_42
 * @tc.desc: test solonode surface capture function of node of blackground blur radius
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_42)
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_43
 * @tc.desc: test solonode surface capture function of node of blackground blur radius 2
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_43)
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
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_44
 * @tc.desc: test solonode surface capture function of node of blackground blur radius 3
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_44)
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
    float radius = 10;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_45
 * @tc.desc: test solonode surface capture function of node of blackground blur radius 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_45)
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
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_46
 * @tc.desc: test solonode surface capture function of node of blackground blur radius 5
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_46)
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
    float radius = 30;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_47
 * @tc.desc: test solonode surface capture function of node of blackground blur radius 6
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_47)
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
    float radius = 50;
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

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_48
 * @tc.desc: test solonode surface capture function of node of blackground blur radius X
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_48)
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
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_49
 * @tc.desc: test solonode surface capture function of node of blackground blur radius X 2
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_49)
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
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_50
 * @tc.desc: test solonode surface capture function of node of blackground blur radius X 3
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_50)
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
    float radius = 10;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_51
 * @tc.desc: test solonode surface capture function of node of blackground blur radius X 4
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_51)
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
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_52
 * @tc.desc: test solonode surface capture function of node of blackground blur radius Y
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_52)
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
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_53
 * @tc.desc: test solonode surface capture function of node of blackground blur radius Y 2
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_53)
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
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_54
 * @tc.desc: test solonode surface capture function of node of blackground blur radius Y 2
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_54)
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
    float radius = 1;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_55
 * @tc.desc: test solonode surface capture function of node of blackground blur radius Y 3
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSSoloNodeSurfaceCaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_SOLONODE_SURFACE_CAPTURE_55)
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
    float radius = 10;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX, sizeY });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    surfaceNode->AddChild(canvasNode, -1);
    canvasNode->AddChild(testNodeBackGround);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto list = RSInterfaces::GetInstance().TakeSurfaceCaptureSoloNodeList(canvasNode);
    for (auto pixel : list) {
        SavePixelToFile(pixel.second);
    }
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(testNodeBackGround);
}
}