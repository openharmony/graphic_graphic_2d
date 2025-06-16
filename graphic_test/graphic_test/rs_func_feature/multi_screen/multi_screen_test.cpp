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

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "pixel_map_from_surface.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_utils.h"
#include "token_setproc.h"

#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200;
constexpr uint32_t SLEEP_TIME_IN_US = 10000; // 10 ms
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
        const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
        std::string fileName = "/data/local/graphic_test/multi_screen/";
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

class CustomizedBufferConsumerListener : public IBufferConsumerListener {
public:
    CustomizedBufferConsumerListener(sptr<Surface> consumerSurface, sptr<Surface> pruducerSurface)
        : consumerSurface_(consumerSurface), pruducerSurface_(pruducerSurface)
    {}
    ~CustomizedBufferConsumerListener() {}

    void OnBufferAvailable() override
    {
        std::cout << "OnBufferAvailable" << std::endl;
        if (consumerSurface_ == nullptr) {
            std::cout << "consumerSurface is nullptr" << std::endl;
            return;
        }
        sptr<SurfaceBuffer> buffer = nullptr;
        int64_t timestamp = 0;
        Rect damage = { 0 };
        sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
        SurfaceError ret = consumerSurface_->AcquireBuffer(buffer, acquireFence, timestamp, damage);
        if (ret != SURFACE_ERROR_OK || buffer == nullptr) {
            std::cout << "AcquireBuffer failed, ret is " << ret << std::endl;
            return;
        }
        OHOS::Rect rect = { 0, 0, buffer->GetWidth(), buffer->GetHeight() };
        std::shared_ptr<Media::PixelMap> pixelMap =
            RSInterfaces::GetInstance().CreatePixelMapFromSurfaceId(pruducerSurface_->GetUniqueId(), rect);
        if (pixelMap == nullptr) {
            std::cout << "pixelMap is nullptr" << std::endl;
            return;
        }
        SurfaceError relaseRet = consumerSurface_->ReleaseBuffer(buffer, SyncFence::InvalidFence());
        if (relaseRet != SURFACE_ERROR_OK) {
            std::cout << "ReleaseBuffer failed ret is " << ret << std::endl;
        }

        const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
        std::string fileName = "/data/local/graphic_test/multi_screen/";
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

private:
    sptr<Surface> consumerSurface_ = nullptr;
    sptr<Surface> pruducerSurface_ = nullptr;
};
} // namespace

class RSMultiScreenTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        std::cout << "BeforeEach" << std::endl;
        uint64_t tokenId;
        const char* perms[1];
        perms[0] = "ohos.permission.CAPTURE_SCREEN";
        NativeTokenInfoParams infoInstance = {
            .dcapsNum = 0,
            .permsNum = 1,
            .aclsNum = 0,
            .dcaps = NULL,
            .perms = perms,
            .acls = NULL,
            .processName = "foundation",
            .aplStr = "system_basic",
        };
        tokenId = GetAccessTokenId(&infoInstance);
        SetSelfTokenID(tokenId);
        OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
        auto size = GetScreenSize();
        SetSurfaceBounds({ 0, 0, size.x_ / 2.0f, size.y_ / 2.0f });
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
 * @tc.name: MULTI_SCREEN_TEST_001
 * @tc.desc: test CreateVirtualScreen without mirrorScreenId 640*1000
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_001)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_001 screenId: " << screenId << "nodeId: " << displayNode->GetId() << std::endl;
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->SetBackgroundColor(SK_ColorBLUE);
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
 * @tc.name: MULTI_SCREEN_TEST_002
 * @tc.desc: test CreateVirtualScreen with mirrorScreenId 640*1000
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_002)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_002", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_002 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;
    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->SetBackgroundColor(SK_ColorBLUE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_002_2", width, height, psurface2, screenId1, -1, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_002 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_003
 * @tc.desc: test SetMirrorScreenVisibleRect with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_003)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_003", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_003 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;
    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->SetBackgroundColor(SK_ColorBLUE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_003_2", width, height, psurface2, screenId1, -1, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }
    RSInterfaces::GetInstance().SetMirrorScreenVisibleRect(screenId2, { 100, 100, 100, 500 });
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_003 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_004
 * @tc.desc: test SetVirtualMirrorScreenScaleMode with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_004)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_004", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_004 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;
    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->SetBackgroundColor(SK_ColorBLUE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_004_2", 2 * width, 5 * height, psurface2, screenId1, -1, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }

    RSInterfaces::GetInstance().SetVirtualMirrorScreenScaleMode(screenId2, ScreenScaleMode::FILL_MODE);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_004 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 2 * width, 5 * height });
    displayNode2->SetFrame({ 0, 0, 2 * width, 5 * height });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_005
 * @tc.desc: test SetVirtualScreenSurface without mirrorScreenId 手动 param set
 * rosen.uni.virtualexpandscreenskip.enabled 0
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_005)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_005", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    auto csurface = Surface::CreateSurfaceAsConsumer();
    csurface->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface, psurface);
    csurface->RegisterConsumerListener(listener);

    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_005 screenId: " << screenId << "nodeId: " << displayNode->GetId() << std::endl;

    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->SetBackgroundColor(SK_ColorBLUE);
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestVirtualScreen_005";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode->SetBounds({ 0, 0, 100, 100 });
    surfaceNode->SetFrame({ 0, 0, 100, 100 });
    surfaceNode->SetBackgroundColor(SK_ColorYELLOW);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_006
 * @tc.desc: test SetVirtualScreenSurface without mirrorScreenId 手动 param set
 * rosen.uni.virtualexpandscreenskip.enabled 0
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_006)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_006", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    auto csurface = Surface::CreateSurfaceAsConsumer();
    csurface->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface, psurface);
    csurface->RegisterConsumerListener(listener);

    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_006 screenId: " << screenId << "nodeId: " << displayNode->GetId() << std::endl;

    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->SetBackgroundColor(SK_ColorBLUE);
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestVirtualScreen_006";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode->SetBounds({ 0, 0, 100, 100 });
    surfaceNode->SetFrame({ 0, 0, 100, 100 });
    surfaceNode->SetBackgroundColor(SK_ColorYELLOW);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_007
 * @tc.desc: test CreateVirtualScreen without mirrorScreenId 640*640
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_007)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_007", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_007 screenId: " << screenId << "nodeId: " << displayNode->GetId() << std::endl;
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->SetBackgroundColor(SK_ColorBLUE);
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
 * @tc.name: MULTI_SCREEN_TEST_008
 * @tc.desc: test CreateVirtualScreen without mirrorScreenId 1280*640
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_008)
{
    uint32_t width = 640 * 2;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_008", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_008 screenId: " << screenId << "nodeId: " << displayNode->GetId() << std::endl;
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->SetBackgroundColor(SK_ColorBLUE);
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
 * @tc.name: MULTI_SCREEN_TEST_009
 * @tc.desc: test SetVirtualScreenBlackList with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_009)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_009", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_009 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });
    surfaceNode1->SetBackgroundColor(SK_ColorBLUE);

    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);
    displayNode1->SetBackgroundColor(SK_ColorBLACK);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_009_2", width, height, psurface2, screenId1, 0, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    std::cout << "MULTI_SCREEN_TEST_009 surfaceId: " << surfaceNode1->GetId() << std::endl;
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(screenId2, screenBlackList);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_009 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_010
 * @tc.desc: test CreateVirtualScreen with mirrorScreenId 640*640
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_010)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_010", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_010 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;
    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->SetBackgroundColor(SK_ColorBLUE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_010_2", width, height, psurface2, screenId1, -1, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_010 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_011
 * @tc.desc: test CreateVirtualScreen with mirrorScreenId 1280*640
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_011)
{
    uint32_t width = 2 * 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_011", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode1 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_011 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;
    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->SetBackgroundColor(SK_ColorBLUE);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_011_2", width, height, psurface2, screenId1, 0, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_011 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_012
 * @tc.desc: test SetVirtualScreenBlackList with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_012)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_012", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_012 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });
    surfaceNode1->SetBackgroundColor(SK_ColorBLUE);

    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);
    displayNode1->SetBackgroundColor(SK_ColorBLACK);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_012_2", width, height, psurface2, 0, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    std::cout << "MULTI_SCREEN_TEST_012 surfaceId: " << surfaceNode1->GetId() << std::endl;
    // 加黑名单到公共黑名单
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);
    // 屏幕黑名单无数据，两个都展示
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_012 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_013
 * @tc.desc: test SetCastScreenEnableSkipWindow with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_013)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_013", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_013 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });
    surfaceNode1->SetBackgroundColor(SK_ColorBLUE);

    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);
    displayNode1->SetBackgroundColor(SK_ColorBLACK);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_013_2", width, height, psurface2, screenId1, 0, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    std::cout << "MULTI_SCREEN_TEST_013 surfaceId: " << surfaceNode1->GetId() << std::endl;
    // 加黑名单到公共黑名单
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);
    // 公共黑名单打开，读取公共黑名单开关打开，只显示id1黄色
    RSInterfaces::GetInstance().SetCastScreenEnableSkipWindow(screenId2, true);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_013 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_014
 * @tc.desc: test SetCastScreenEnableSkipWindow with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_014)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_014", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_014 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });
    surfaceNode1->SetBackgroundColor(SK_ColorBLUE);

    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);
    displayNode1->SetBackgroundColor(SK_ColorBLACK);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_014_2", width, height, psurface2, screenId1, 0, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    std::cout << "MULTI_SCREEN_TEST_014 surfaceId: " << surfaceNode1->GetId() << std::endl;
    // 加黑名单到公共黑名单
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);
    // 公共黑名单不打开，显示全部
    RSInterfaces::GetInstance().SetCastScreenEnableSkipWindow(screenId2, false);
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_014 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_015
 * @tc.desc: test SetScreenCorrection without mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_015)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_015", width, height, psurface, INVALID_SCREEN_ID, 0, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_015 screenId: " << screenId << "nodeId1: " << displayNode->GetId() << std::endl;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });
    surfaceNode1->SetBackgroundColor(SK_ColorBLUE);

    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);
    displayNode->SetBackgroundColor(SK_ColorBLACK);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().SetScreenCorrection(screenId,ScreenRotation::ROTATION_0);  
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_016
 * @tc.desc: test SetScreenCorrection without mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_016)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_016", width, height, psurface, INVALID_SCREEN_ID, 0, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_016 screenId: " << screenId << "nodeId1: " << displayNode->GetId() << std::endl;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });
    surfaceNode1->SetBackgroundColor(SK_ColorBLUE);

    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);
    displayNode->SetBackgroundColor(SK_ColorBLACK);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().SetScreenCorrection(screenId,ScreenRotation::ROTATION_90);  
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}


/*
 * @tc.name: MULTI_SCREEN_TEST_017
 * @tc.desc: test SetScreenCorrection without mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_017)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_017", width, height, psurface, INVALID_SCREEN_ID, 0, {});
    if (screenId == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_017 screenId: " << screenId << "nodeId1: " << displayNode->GetId() << std::endl;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });
    surfaceNode1->SetBackgroundColor(SK_ColorBLUE);

    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);
    displayNode->SetBackgroundColor(SK_ColorBLACK);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().SetScreenCorrection(screenId,ScreenRotation::ROTATION_180);  
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_018
 * @tc.desc: test SetVirtualMirrorScreenCanvasRotation with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_018)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_018", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    if (screenId1 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen1 failed");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    if (!displayNode1) {
        LOGE("displayNode is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_018 screenId1: " << screenId1 << "nodeId1: " << displayNode1->GetId() << std::endl;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 200 });
    surfaceNode0->SetFrame({ 0, 0, 100, 200 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode1->SetBounds({ 0, 0, 300, 300 });
    surfaceNode1->SetFrame({ 0, 0, 400, 400 });
    surfaceNode1->SetBackgroundColor(SK_ColorBLUE);

    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);
 
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface2, psurface2);
    csurface2->RegisterConsumerListener(listener);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_018_2", width, height, psurface2, screenId1, 0, {});
    if (screenId2 == INVALID_SCREEN_ID) {
        LOGE("CreateVirtualScreen2 failed");
        return;
    }

    RSInterfaces::GetInstance().SetVirtualMirrorScreenCanvasRotation(screenId2, false);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    if (!displayNode2) {
        LOGE("displayNode2 is nullptr");
        return;
    }
    std::cout << "MULTI_SCREEN_TEST_018 screenId2: " << screenId2 << "nodeId2: " << displayNode2->GetId() << std::endl;
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    
    // 避免node1已经旋转完成了，才开始渲染node2 写在这里就是为了延迟旋转
    displayNode1->SetScreenRotation(static_cast<uint32_t>(ScreenRotation::ROTATION_90));
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}
} // namespace ::OHOS::Rosen
