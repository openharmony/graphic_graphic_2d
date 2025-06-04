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

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_utils.h"

#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200;
constexpr uint32_t SLEEP_TIME_IN_US = 10000; // 10ms
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms

class CustomizedSurfaceCapture : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelMap) override
    {
        if (pixelMap == nullptr) {
            LOGE("CustomizedSurfaceCapture::OnSurfaceCapture failed to get pixelMap, return nullptr!");
            return;
        }
        isCallbackCalled_ = true;
        const ::testing::TestInfo* const testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
        std::string fileName = "/data/local/graphic_test/screen_capture/";
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
    bool isCallbackCalled_ = false;
};
} // namespace

class RSScreenCaptureTest : public RSGraphicTest {
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
};

/*
 * @tc.name: DISPLAY_NODE_CAPTURE_TEST_001
 * @tc.desc: test display node capture
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenCaptureTest, CONTENT_DISPLAY_TEST, DISPLAY_NODE_CAPTURE_TEST_001)
{
    auto canvasNode0 = RSCanvasNode::Create();
    RegisterNode(canvasNode0);
    canvasNode0->SetBounds({0, 0, 100, 100});
    canvasNode0->SetFrame({0, 0, 100, 100});
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    RegisterNode(canvasNode1);
    canvasNode1->SetBounds({0, 0, 200, 200});
    canvasNode1->SetFrame({0, 0, 200, 200});
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    RegisterNode(surfaceNode0);
    surfaceNode0->SetBounds({0, 0, 100, 100});
    surfaceNode0->SetFrame({0, 0, 100, 100});

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    RegisterNode(surfaceNode1);
    surfaceNode1->SetBounds({0, 0, 200, 200});
    surfaceNode1->SetFrame({0, 0, 200, 200});

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen("TestVirtualScreen", width, height, psurface,
        INVALID_SCREEN_ID, -1, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig = {screenId, false, 0, true};
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    RegisterNode(displayNode);
    displayNode->SetBounds({0, 0, 1000, 1000});
    displayNode->SetFrame({0, 0, 1000, 1000});
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: DISPLAY_NODE_CAPTURE_TEST_002
 * @tc.desc: test display node capture with black list
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenCaptureTest, CONTENT_DISPLAY_TEST, DISPLAY_NODE_CAPTURE_TEST_002)
{
    auto canvasNode0 = RSCanvasNode::Create();
    RegisterNode(canvasNode0);
    canvasNode0->SetBounds({0, 0, 100, 100});
    canvasNode0->SetFrame({0, 0, 100, 100});
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    RegisterNode(canvasNode1);
    canvasNode1->SetBounds({0, 0, 200, 200});
    canvasNode1->SetFrame({0, 0, 200, 200});
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    RegisterNode(surfaceNode0);
    surfaceNode0->SetBounds({0, 0, 100, 100});
    surfaceNode0->SetFrame({0, 0, 100, 100});

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    RegisterNode(surfaceNode1);
    surfaceNode1->SetBounds({0, 0, 200, 200});
    surfaceNode1->SetFrame({0, 0, 200, 200});

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen("TestVirtualScreen", width, height, psurface,
        INVALID_SCREEN_ID, -1, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig = {screenId, false, 0, true};
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    RegisterNode(displayNode);
    displayNode->SetBounds({0, 0, 1000, 1000});
    displayNode->SetFrame({0, 0, 1000, 1000});
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.blackList.push_back(surfaceNode0->GetId());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback, captureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}
} // namespace OHOS::Rosen
