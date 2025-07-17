/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
        std::string filename = "/data/local/graphic_test/component_capture/";
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

    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
    bool isCallbackCalled_ = false;
};
}

class RSUICaptureTest : public RSGraphicTest {
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
};

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_01
 * @tc.desc: test ui capture function of node drawTextblob
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_01)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 500, 700});
    canvasNode->SetFrame({0, 0, 500, 700});
    canvasNode->SetBackgroundColor(SK_ColorBLUE);
    GetRootNode()->AddChild(canvasNode);
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

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_02
 * @tc.desc: test ui capture function of node set alpha and scale
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_02)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 500, 700});
    canvasNode->SetFrame({0, 0, 500, 700});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    canvasNode->SetAlpha(0.5f);
    GetRootNode()->AddChild(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback, 2, 2);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_03
 * @tc.desc: test ui capture function of node set blendMode
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_03)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(0x5500ff00);
    canvasNode->SetColorBlendMode(RSColorBlendMode::XOR);
    GetRootNode()->AddChild(canvasNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback, 2, 2);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_04
 * @tc.desc: test ui capture function of node draw pixelmap and blur
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_04)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(0x5500ff00);
    canvasNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(10, 10));
    GetRootNode()->AddChild(canvasNode);

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

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_05
 * @tc.desc: test ui capture function of node set translate
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_05)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetTranslate({100, 100});
    GetRootNode()->AddChild(canvasNode);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_06
 * @tc.desc: test ui capture function of node set rotation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_06)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({100, 100, 150, 150});
    childNode->SetFrame({100, 100, 150, 150});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetRotation(45);
    GetRootNode()->AddChild(canvasNode);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_07
 * @tc.desc: test ui capture function of node set scale
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_07)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    childNode->SetScale(0.5);
    GetRootNode()->AddChild(canvasNode);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_08
 * @tc.desc: test ui capture function of parent node set translate
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_08)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetTranslate({50, 50});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    GetRootNode()->AddChild(canvasNode);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_09
 * @tc.desc: test ui capture function of parent node set rotation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_09)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetRotation(45);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 300, 300});
    childNode->SetFrame({0, 0, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLACK);
    GetRootNode()->AddChild(canvasNode);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_10
 * @tc.desc: test ui capture function of node not on the tree
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_10)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetRotation(45);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_11
 * @tc.desc: test ui capture function of node draw dma pixelmap
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_11)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(0x5500ff00);
    GetRootNode()->AddChild(canvasNode);

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

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_12
 * @tc.desc: test ui capture function of node draw astc pixelmap
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_12)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 600, 600});
    canvasNode->SetFrame({0, 0, 600, 600});
    canvasNode->SetBackgroundColor(0x5500ff00);
    GetRootNode()->AddChild(canvasNode);

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

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_13
 * @tc.desc: test ui capture function of node is over parent node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_13)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({100, 100, 600, 600});
    childNode->SetFrame({100, 100, 600, 600});
    childNode->SetBackgroundColor(SK_ColorBLUE);
    GetRootNode()->AddChild(canvasNode);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_14
 * @tc.desc: test ui capture function of node set properties before takeUICapture
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_14)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({0, 0, 200, 300});
    childNode->SetFrame({0, 0, 200, 300});
    childNode->SetBackgroundColor(SK_ColorBLUE);
    GetRootNode()->AddChild(canvasNode);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    childNode->SetBackgroundColor(SK_ColorGREEN);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_15
 * @tc.desc: test ui capture function of effectNode and child set use effect
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_15)
{
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
    GetRootNode()->AddChild(effectNode);
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
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(childNode, callback);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(childNode);
    RegisterNode(effectNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_16
 * @tc.desc: test ui capture function of set sync
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_16)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback, 1, 1, true);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
}

/*
 * @tc.name: GRAPHIC_TEST_UI_CAPTURE_17
 * @tc.desc: test ui capture function of set sync 2
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSUICaptureTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_UI_CAPTURE_17)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, 400, 500});
    canvasNode->SetFrame({0, 0, 400, 500});
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({100, 100, 300, 300});
    childNode->SetFrame({100, 100, 300, 300});
    childNode->SetBackgroundColor(SK_ColorBLUE);
    GetRootNode()->AddChild(canvasNode);
    canvasNode->AddChild(childNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    childNode->SetBackgroundColor(SK_ColorGREEN);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(canvasNode, callback, 1, 1, true);
    CheckSurfaceCaptureCallback(callback);
    // created node should be registered to preserve ref_count
    RegisterNode(canvasNode);
    RegisterNode(childNode);
}

}
