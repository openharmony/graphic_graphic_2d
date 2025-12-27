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
}

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
        canvasNode->SetBounds({ 0, 0, 300, 300 });
        canvasNode->SetFrame({ 0, 0, 300, 300 });
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

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);

//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }

// /*
//  * @tc.name: GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01
//  * @tc.desc: test RegisterBufferAvailableCallback function of node setbackground
//  * @tc.type: FUNC
//  */
// GRAPHIC_TEST(
//     RSRegisterBufferAvailableCallbackTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_REGISTER_BUFFER_AVAILABLE_CALLBACK_01)
// {
//     std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
//     auto rsUIContext = rsUiDirector->GetRSUIContext();

//     auto surfaceNode = CreateTestSurfaceNode();
//     auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();

//     auto cb = [this, surfaceNode, rootNode]() {
//         auto canvasNode = RSCanvasNode::Create();
//         canvasNode->SetBounds({ 0, 0, 300, 300 });
//         canvasNode->SetFrame({ 0, 0, 300, 300 });
//         canvasNode->SetBackgroundColor(SK_ColorRED);
//         rootNode->AddChild(canvasNode, -1);
//         RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
//         usleep(SLEEP_TIME_FOR_PROXY);
        
//         // created node should be registered to preserve ref_count
//         RegisterNode(canvasNode);
//     };
//     surfaceNode->SetBufferAvailableCallback(cb);

//     rootNode->SetBounds(0, 0, 1000, 1000);
//     rootNode->SetFrame(0, 0, 1000, 1000);
//     rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

//     rootNode->AttachRSSurfaceNode(surfaceNode);

//     GetRootNode()->SetTestSurface(surfaceNode);
// }
}