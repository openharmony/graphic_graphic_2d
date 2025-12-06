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
constexpr uint32_t SLEEP_TIME_IN_US = 10000;      // 10 ms
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
constexpr float DEFAULT_BOUND_WIDTH = 100;
constexpr float DEFAULT_BOUND_HEIGHT = 100;
constexpr uint32_t DEFAULT_SCREEN_WIDTH = 640;
constexpr uint32_t DEFAULT_SCREEN_HEIGHT = 1000;

static void SavePixelToFile(std::shared_ptr<Media::PixelMap> pixelMap)
{
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

class CustomizedSurfaceCapture : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelMap) override
    {
        if (pixelMap == nullptr) {
            LOGE("CustomizedSurfaceCapture::OnSurfaceCapture failed to get pixelMap, return nullptr!");
            return;
        }
        isCallbackCalled_ = true;
        SavePixelToFile(pixelMap);
    }
    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
    bool isCallbackCalled_ = false;
};

class CustomizedBufferConsumerListener : public IBufferConsumerListener {
public:
    CustomizedBufferConsumerListener(sptr<Surface> consumerSurface, sptr<Surface> producerSurface)
        : consumerSurface_(consumerSurface), producerSurface_(producerSurface)
    {}
    ~CustomizedBufferConsumerListener() {}

    void OnBufferAvailable() override
    {
        LOGI("OnBufferAvailable");
        if (consumerSurface_ == nullptr) {
            LOGE("consumerSurface is nullptr");
            return;
        }
        sptr<SurfaceBuffer> buffer = nullptr;
        int64_t timestamp = 0;
        Rect damage = { 0 };
        sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
        SurfaceError ret = consumerSurface_->AcquireBuffer(buffer, acquireFence, timestamp, damage);
        if (ret != SURFACE_ERROR_OK || buffer == nullptr) {
            LOGE("AcquireBuffer failed ret is:");
            return;
        }
        OHOS::Rect rect = { 0, 0, buffer->GetWidth(), buffer->GetHeight() };
        std::shared_ptr<Media::PixelMap> pixelMap =
            RSInterfaces::GetInstance().CreatePixelMapFromSurfaceId(producerSurface_->GetUniqueId(), rect);
        if (pixelMap == nullptr) {
            LOGE("pixelMap is nullptr");
            return;
        }
        SurfaceError relaseRet = consumerSurface_->ReleaseBuffer(buffer, SyncFence::InvalidFence());
        if (relaseRet != SURFACE_ERROR_OK) {
            LOGE("ReleaseBuffer failed");
        }

        SavePixelToFile(pixelMap);
    }

private:
    sptr<Surface> consumerSurface_ = nullptr;
    sptr<Surface> producerSurface_ = nullptr;
};
} // namespace

class RSMultiScreenTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        LOGI("BeforeEach");
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

    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> CreateSurfaceNodeWithConfig(
        const string& name, const Vector4f& rect, uint32_t colorValue)
    {
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.isSync = true;
        surfaceNodeConfig.SurfaceNodeName = name;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        if (surfaceNode == nullptr) {
            return nullptr;
        }
        surfaceNode->SetBounds(rect);
        surfaceNode->SetFrame(rect);
        surfaceNode->SetBackgroundColor(colorValue);
        surfaceNode->SetLeashPersistentId(surfaceNode->GetId());
        return surfaceNode;
    }

    std::shared_ptr<OHOS::Rosen::RSDisplayNode> CreateDispalyNodeWithConfig(
        ScreenId screenId, const Vector4f& rect, uint32_t colorValue, bool securityDisplay)
    {
        RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
        auto displayNode = RSDisplayNode::Create(displayNodeConfig);
        if (displayNode == nullptr) {
            return nullptr;
        }
        displayNode->SetBounds(rect);
        displayNode->SetFrame(rect);
        displayNode->SetBackgroundColor(colorValue);
        displayNode->SetSecurityDisplay(securityDisplay);
        return displayNode;
    }

    std::shared_ptr<OHOS::Rosen::RSCanvasNode> CreateCanvasNodeWithConfig(const Vector4f& rect, uint32_t colorValue)
    {
        auto canvasNode = RSCanvasNode::Create();
        if (canvasNode == nullptr) {
            return nullptr;
        }
        canvasNode->SetBounds(rect);
        canvasNode->SetFrame(rect);
        canvasNode->SetBackgroundColor(colorValue);
        return canvasNode;
    }
};

/*
 * @tc.name: CreateVirtualScreenTest001
 * @tc.desc: test CreateVirtualScreen 640*640 without surface or associatedScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("CreateVirtualScreenTest001 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
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
 * @tc.name: CreateVirtualScreenTest002
 * @tc.desc: test CreateVirtualScreen 640*1000 without surface or associatedScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest002)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("CreateVirtualScreenTest002 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
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
 * @tc.name: CreateVirtualScreenTest003
 * @tc.desc: test CreateVirtualScreen 1280*640 without surface or associatedScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest003)
{
    uint32_t width = 640 * 2;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest003", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("CreateVirtualScreenTest003 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
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
 * @tc.name: CreateVirtualScreenTest004
 * @tc.desc: test CreateVirtualScreen 640*640 with surface and mirrorScreenId screenId1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest004)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest004", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("CreateVirtualScreenTest004 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "CreateVirtualScreenTest004", width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("CreateVirtualScreenTest004 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: CreateVirtualScreenTest005
 * @tc.desc: test CreateVirtualScreen 640*1000 with surface and mirrorScreenId screenId1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest005)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest005", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("CreateVirtualScreenTest005 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "CreateVirtualScreenTest005", width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("CreateVirtualScreenTest005 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: CreateVirtualScreenTest006
 * @tc.desc: test CreateVirtualScreen 1280*640 with surface and mirrorScreenId screenId1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest006)
{
    uint32_t width = 2 * 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest006", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("CreateVirtualScreenTest006 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "CreateVirtualScreenTest006", width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("CreateVirtualScreenTest006 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: CreateVirtualScreenTest007
 * @tc.desc: test CreateVirtualScreen 1920*1280 with surface and mirrorScreenId screenId1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest007)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest007", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("CreateVirtualScreenTest007 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "CreateVirtualScreenTest007", width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("CreateVirtualScreenTest007 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetMirrorScreenVisibleRectTest001
 * @tc.desc: test SetMirrorScreenVisibleRect with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetMirrorScreenVisibleRectTest001)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetMirrorScreenVisibleRectTest001", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetMirrorScreenVisibleRectTest001 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "SetMirrorScreenVisibleRectTest001", width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetMirrorScreenVisibleRect(screenId2, { 100, 100, 100, 500 });
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetMirrorScreenVisibleRectTest001 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetMirrorScreenVisibleRectTest002
 * @tc.desc: test SetMirrorScreenVisibleRect when rect is out of screen
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetMirrorScreenVisibleRectTest002)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetMirrorScreenVisibleRectTest002", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetMirrorScreenVisibleRectTest002 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "SetMirrorScreenVisibleRectTest002", width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    // need foundation calling, or it will fail, ipc interface code access denied
    RSInterfaces::GetInstance().SetMirrorScreenVisibleRect(screenId2, { 100, 100, 100, 500 });
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetMirrorScreenVisibleRectTest002 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetMirrorScreenVisibleRectTest003
 * @tc.desc: test SetMirrorScreenVisibleRect when support rotation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetMirrorScreenVisibleRectTest003)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetMirrorScreenVisibleRectTest003", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetMirrorScreenVisibleRectTest003 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]", screenId1,
        displayNode1->GetId());
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
        "SetMirrorScreenVisibleRectTest003", width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    // need foundation calling, or it will fail, ipc interface code access denied
    RSInterfaces::GetInstance().SetMirrorScreenVisibleRect(screenId2, { 100, 100, 100, 500 }, true);
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetMirrorScreenVisibleRectTest003 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]", screenId2,
        displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleModeTest001
 * @tc.desc: test SetVirtualMirrorScreenScaleMode using FILL_MODE
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenScaleModeTest001)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualMirrorScreenScaleModeTest001", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetVirtualMirrorScreenScaleModeTest001 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "SetVirtualMirrorScreenScaleModeTest001", 2 * width, 5 * height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenScaleMode(screenId2, ScreenScaleMode::FILL_MODE);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetVirtualMirrorScreenScaleModeTest001 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 2 * width, 5 * height });
    displayNode2->SetFrame({ 0, 0, 2 * width, 5 * height });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleModeTest002
 * @tc.desc: test SetVirtualMirrorScreenScaleMode using UNISCALE_MODE
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenScaleModeTest002)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualMirrorScreenScaleModeTest002", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetVirtualMirrorScreenScaleModeTest002 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "SetVirtualMirrorScreenScaleModeTest002", 2 * width, 5 * height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenScaleMode(screenId2, ScreenScaleMode::UNISCALE_MODE);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetVirtualMirrorScreenScaleModeTest002 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 2 * width, 5 * height });
    displayNode2->SetFrame({ 0, 0, 2 * width, 5 * height });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleModeTest003
 * @tc.desc: test SetVirtualMirrorScreenScaleMode using INVALID_MODE
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenScaleModeTest003)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualMirrorScreenScaleModeTest003", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetVirtualMirrorScreenScaleModeTest003 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "SetVirtualMirrorScreenScaleModeTest003", 2 * width, 5 * height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenScaleMode(screenId2, ScreenScaleMode::INVALID_MODE);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetVirtualMirrorScreenScaleModeTest003 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 2 * width, 5 * height });
    displayNode2->SetFrame({ 0, 0, 2 * width, 5 * height });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualScreenSurfaceTest001
 * @tc.desc: test SetVirtualScreenSurface 640*1000 without surface or associatedScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenSurfaceTest001)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenSurfaceTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
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
    ASSERT_NE(displayNode, nullptr);
    LOGI("SetVirtualScreenSurfaceTest001 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
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
 * @tc.name: SetVirtualScreenSurfaceTest002
 * @tc.desc: test SetVirtualScreenSurface 640*640 without surface or associatedScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenSurfaceTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenSurfaceTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
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
    ASSERT_NE(displayNode, nullptr);
    LOGI("SetVirtualScreenSurfaceTest002 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
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
 * @tc.name: SetVirtualScreenBlackListTest001
 * @tc.desc: test SetVirtualScreenBlackList, push surfaceNode1 into screenBlackList, associatedScreenId is screenId1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenBlackListTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenBlackListTest001", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetVirtualScreenBlackListTest001 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "SetVirtualScreenBlackListTest001", width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("SetVirtualScreenBlackListTest001 surfaceId1:[%{public}" PRIu64 "]", surfaceNode1->GetId());
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(screenId2, screenBlackList);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetVirtualScreenBlackListTest001 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualScreenBlackListTest002
 * @tc.desc: test SetVirtualScreenBlackList without blacklist, associatedScreenId is 0
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenBlackListTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenBlackListTest002", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetVirtualScreenBlackListTest002 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]", screenId1,
        displayNode1->GetId());
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
        "SetVirtualScreenBlackListTest002", width, height, psurface2, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("SetVirtualScreenBlackListTest002 surfaceId1:[%{public}" PRIu64 "]", surfaceNode1->GetId());
    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);
    // Screen blocklist has no data, both are displayed
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetVirtualScreenBlackListTest002 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualScreenBlackListTest003
 * @tc.desc: test SetVirtualScreenBlackList with invalid nodeId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenBlackListTest003)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenBlackListTest003", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetVirtualScreenBlackListTest003 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "SetVirtualScreenBlackListTest003", width, height, psurface2, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("SetVirtualScreenBlackListTest003 surfaceId1:[%{public}" PRIu64 "]", surfaceNode1->GetId());

    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);

    // Screen blocklist has no data, both are displayed
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetVirtualScreenBlackListTest003 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindowTest001
 * @tc.desc: test SetCastScreenEnableSkipWindow with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetCastScreenEnableSkipWindowTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetCastScreenEnableSkipWindowTest001", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetCastScreenEnableSkipWindowTest001 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "SetCastScreenEnableSkipWindowTest001", width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("SetCastScreenEnableSkipWindowTest001 surfaceId:[%{public}" PRIu64 "]", surfaceNode1->GetId());

    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);

    // Open the public blocklist, turn on the switch to read the public blocklist, and only display ID1 in yellow
    RSInterfaces::GetInstance().SetCastScreenEnableSkipWindow(screenId2, true);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetCastScreenEnableSkipWindowTest001 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindowTest002
 * @tc.desc: test SetCastScreenEnableSkipWindow with mirrorScreenId
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetCastScreenEnableSkipWindowTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetCastScreenEnableSkipWindowTest002", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetCastScreenEnableSkipWindowTest002 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());

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
        "SetCastScreenEnableSkipWindowTest002", width, height, psurface2, screenId1, 0, {});

    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("SetCastScreenEnableSkipWindowTest002 surfaceId:[%{public}" PRIu64 "]", surfaceNode1->GetId());

    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);

    // The public blocklist is not open, displaying all
    RSInterfaces::GetInstance().SetCastScreenEnableSkipWindow(screenId2, false);
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetCastScreenEnableSkipWindowTest002 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindowTest003
 * @tc.desc: test SetCastScreenEnableSkipWindow when open EnableSkipWindow, but there is node in public blacklist
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetCastScreenEnableSkipWindowTest003)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetCastScreenEnableSkipWindowTest003", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetCastScreenEnableSkipWindowTest003 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());

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
        "SetCastScreenEnableSkipWindowTest003", width, height, psurface2, screenId1, 0, {});

    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    std::vector<uint64_t> screenBlackList = {};
    LOGI("SetCastScreenEnableSkipWindowTest003 surfaceId:[%{public}" PRIu64 "]", surfaceNode1->GetId());

    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);

    // The public blocklist is not open, displaying all
    RSInterfaces::GetInstance().SetCastScreenEnableSkipWindow(screenId2, true);
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetCastScreenEnableSkipWindowTest003 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetScreenCorrectionTest001
 * @tc.desc: test SetScreenCorrection with degree 0
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenCorrectionTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetScreenCorrectionTest001", width, height, psurface, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode, nullptr);
    LOGI("SetScreenCorrectionTest001 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
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

    RSInterfaces::GetInstance().SetScreenCorrection(screenId, ScreenRotation::ROTATION_0);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetScreenCorrectionTest002
 * @tc.desc: test SetScreenCorrection with degree 90
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenCorrectionTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetScreenCorrectionTest002", width, height, psurface, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode, nullptr);
    LOGI("SetScreenCorrectionTest002 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
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

    RSInterfaces::GetInstance().SetScreenCorrection(screenId, ScreenRotation::ROTATION_90);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetScreenCorrectionTest003
 * @tc.desc: test SetScreenCorrection with degree 180
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenCorrectionTest003)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetScreenCorrectionTest003", width, height, psurface, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode, nullptr);
    LOGI("SetScreenCorrectionTest003 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
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

    RSInterfaces::GetInstance().SetScreenCorrection(screenId, ScreenRotation::ROTATION_180);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetScreenCorrectionTest004
 * @tc.desc: test SetScreenCorrection with degree 270
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenCorrectionTest004)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetScreenCorrectionTest004", width, height, psurface, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode, nullptr);
    LOGI("SetScreenCorrectionTest004 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
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

    RSInterfaces::GetInstance().SetScreenCorrection(screenId, ScreenRotation::ROTATION_270);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotationTest001
 * @tc.desc: test SetVirtualMirrorScreenCanvasRotation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenCanvasRotationTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualMirrorScreenCanvasRotationTest001", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
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
        "SetVirtualMirrorScreenCanvasRotationTest001", width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenCanvasRotation(screenId2, false);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetVirtualMirrorScreenCanvasRotationTest001 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // To avoid rendering node2 only after node1 has already rotated, it is written here to delay the rotation
    displayNode1->SetScreenRotation(static_cast<uint32_t>(ScreenRotation::ROTATION_90));
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotationTest002
 * @tc.desc: test SetVirtualMirrorScreenCanvasRotation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenCanvasRotationTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualMirrorScreenCanvasRotationTest002", width, height, psurface1, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetVirtualMirrorScreenCanvasRotationTest002 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
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
        "SetVirtualMirrorScreenCanvasRotationTest002", width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenCanvasRotation(screenId2, true);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetVirtualMirrorScreenCanvasRotationTest002 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // To avoid rendering node2 only after node1 has already rotated, it is written here to delay the rotation
    displayNode1->SetScreenRotation(static_cast<uint32_t>(ScreenRotation::ROTATION_90));
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetScreenSecurityMaskTest001
 * @tc.desc: test SetScreenSecurityMask when has security surfacenode
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenSecurityMaskTest001)
{
    uint32_t width = 640 * 2;
    uint32_t height = 640 * 2;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetScreenSecurityMaskTest001", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetScreenSecurityMaskTest001 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 200 });
    surfaceNode0->SetFrame({ 0, 0, 100, 200 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    // Set SecurityLayer for surfaceNode0
    surfaceNode0->SetSecurityLayer(true);
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);

    surfaceNode1->SetBounds({ 0, 0, 300, 300 });
    surfaceNode1->SetFrame({ 0, 0, 400, 400 });
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
        "SetScreenSecurityMaskTest001", width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetScreenSecurityMaskTest001 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetScreenSecurityMaskTest002
 * @tc.desc: test SetScreenSecurityMask with red securitymask pixelmap
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenSecurityMaskTest002)
{
    uint32_t width = 640 * 2;
    uint32_t height = 640 * 2;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetScreenSecurityMaskTest002", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetScreenSecurityMaskTest002 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 200 });
    surfaceNode0->SetFrame({ 0, 0, 100, 200 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);
    surfaceNode0->SetSecurityLayer(true);

    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
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
        "SetScreenSecurityMaskTest002", width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    // init pixelMap
    uint32_t colorWidth = 640;
    uint32_t colorHeight = 640;
    uint32_t colorLength = colorWidth * colorHeight;
    std::vector<uint32_t> colorVec(colorLength, 0xffff0000);
    uint32_t* color = colorVec.data();
    Media::InitializationOptions opts;
    opts.size.width = static_cast<int32_t>(colorWidth);
    opts.size.height = static_cast<int32_t>(colorHeight);
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(color, colorLength, opts);
    // only foundation can call Manual block stub permission check
    RSInterfaces::GetInstance().SetScreenSecurityMask(screenId2, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetScreenSecurityMaskTest002 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetScreenSecurityMaskTest003
 * @tc.desc: test SetScreenSecurityMask with red securitymask pixelmap, but has no security surfacenode
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenSecurityMaskTest003)
{
    uint32_t width = 640 * 2;
    uint32_t height = 640 * 2;

    // mirrorSourceScreen
    auto csurface1 = IConsumerSurface::Create();
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetScreenSecurityMaskTest003", width, height, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, false, 0, true };
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode1, nullptr);
    LOGI("SetScreenSecurityMaskTest003 screenId1[%{public}" PRIu64 "], nodeId1[%{public}" PRIu64 "]",
        screenId1, displayNode1->GetId());
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 200 });
    surfaceNode0->SetFrame({ 0, 0, 100, 200 });
    surfaceNode0->SetBackgroundColor(SK_ColorYELLOW);

    displayNode1->SetBounds({ 0, 0, 1000, 1000 });
    displayNode1->SetFrame({ 0, 0, 1000, 1000 });
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
        "SetScreenSecurityMaskTest003", width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    // init pixelMap
    uint32_t colorWidth = 640;
    uint32_t colorHeight = 640;
    uint32_t colorLength = colorWidth * colorHeight;
    std::vector<uint32_t> colorVec(colorLength, 0xffff0000);
    uint32_t* color = colorVec.data();
    Media::InitializationOptions opts;
    opts.size.width = static_cast<int32_t>(colorWidth);
    opts.size.height = static_cast<int32_t>(colorHeight);
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(color, colorLength, opts);

    // only foundation can call Manual block stub permission check
    RSInterfaces::GetInstance().SetScreenSecurityMask(screenId2, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);

    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig2);
    ASSERT_NE(displayNode2, nullptr);
    LOGI("SetScreenSecurityMaskTest003 screenId2[%{public}" PRIu64 "], nodeId2[%{public}" PRIu64 "]",
        screenId2, displayNode2->GetId());
    displayNode2->SetBounds({ 0, 0, 1000, 1000 });
    displayNode2->SetFrame({ 0, 0, 1000, 1000 });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest001
 * @tc.desc: test SetVirtualScreenResolution 4000*3000
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest001)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest001", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto csurfaceSecond = IConsumerSurface::Create();
    auto producerSecond = csurfaceSecond->GetProducer();
    auto psurfaceSecond = Surface::CreateSurfaceAsProducer(producerSecond);
    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest001", 4 * width, 3 * height, psurfaceSecond, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest002
 * @tc.desc: test SetVirtualScreenResolution 3000*3000
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest002)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest002", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, 3 * width, 3 * height);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest003
 * @tc.desc: test SetVirtualScreenResolution 3000*4000
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest003)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest003", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto csurfaceSecond = IConsumerSurface::Create();
    auto producerSecond = csurfaceSecond->GetProducer();
    auto psurfaceSecond = Surface::CreateSurfaceAsProducer(producerSecond);
    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest003", 3 * width, 4 * height, psurfaceSecond, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest004
 * @tc.desc: test SetVirtualScreenResolution 0*0
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest004)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest004", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto csurfaceSecond = IConsumerSurface::Create();
    auto producerSecond = csurfaceSecond->GetProducer();
    auto psurfaceSecond = Surface::CreateSurfaceAsProducer(producerSecond);
    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest004", 0 * width, 0 * height, psurfaceSecond, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest005
 * @tc.desc: test SetVirtualScreenResolution 65536*65536
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest005)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 65536;
    uint32_t height = 65536;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest005", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto csurfaceSecond = IConsumerSurface::Create();
    auto producerSecond = csurfaceSecond->GetProducer();
    auto psurfaceSecond = Surface::CreateSurfaceAsProducer(producerSecond);
    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest005", width, height, psurfaceSecond, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: ResizeVirtualScreenTest001
 * @tc.desc: test ResizeVirtualScreen 3000*4000
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest001)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "ResizeVirtualScreenTest001", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto csurfaceSecond = IConsumerSurface::Create();
    auto producerSecond = csurfaceSecond->GetProducer();
    auto psurfaceSecond = Surface::CreateSurfaceAsProducer(producerSecond);
    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "ResizeVirtualScreenTest001", 3 * width, 4 * height, psurfaceSecond, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: ResizeVirtualScreenTest002
 * @tc.desc: test ResizeVirtualScreen 4000*3000
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest002)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "ResizeVirtualScreenTest002", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto csurfaceSecond = IConsumerSurface::Create();
    auto producerSecond = csurfaceSecond->GetProducer();
    auto psurfaceSecond = Surface::CreateSurfaceAsProducer(producerSecond);
    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "ResizeVirtualScreenTest002", 4 * width, 3 * height, psurfaceSecond, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: ResizeVirtualScreenTest003
 * @tc.desc: test ResizeVirtualScreen 2000*2000
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest003)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "ResizeVirtualScreenTest003", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, 2 * width, 2 * height);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_029
 * @tc.desc: test blacklist in virtual expand screen
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_029)
{
    Vector4f rect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", rect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f rect2(0, 0, DEFAULT_BOUND_WIDTH / 2, DEFAULT_BOUND_HEIGHT / 2);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", rect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    // create virtual screen
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MULTI_SCREEN_TEST_029", DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    // set virtual screen black list
    std::vector<NodeId> list = {surfaceNode1->GetId()};
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(screenId, list);

    // create csurface and psurface
    auto csurface = Surface::CreateSurfaceAsConsumer();
    csurface->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface, psurface);
    csurface->RegisterConsumerListener(listener);
    // set psurface
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDispalyNodeWithConfig(screenId, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_030
 * @tc.desc: test whitelist & blacklist in virtual expand screen
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_030)
{
    Vector4f rect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", rect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f rect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", rect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    // create virtual screen
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen("MULTI_SCREEN_TEST_030",
        DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, {surfaceNode1->GetId()});
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    // set virtual screen black list
    std::vector<NodeId> list = {surfaceNode1->GetId()};
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(screenId, list);

    // create csurface and psurface
    auto csurface = Surface::CreateSurfaceAsConsumer();
    csurface->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface, psurface);
    csurface->RegisterConsumerListener(listener);
    // set psurface
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDispalyNodeWithConfig(screenId, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_031
 * @tc.desc: test whitelist in virtual expand screen (has child)
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_031)
{
    Vector4f rect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", rect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f rect2(0, 0, DEFAULT_BOUND_WIDTH / 2, DEFAULT_BOUND_HEIGHT / 2);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", rect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    // set child
    surfaceNode1->RSNode::AddChild(surfaceNode2);

    // create virtual screen
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen("MULTI_SCREEN_TEST_031",
        DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, {surfaceNode1->GetId()});
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // create csurface and psurface
    auto csurface = Surface::CreateSurfaceAsConsumer();
    csurface->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface, psurface);
    csurface->RegisterConsumerListener(listener);
    // set psurface
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDispalyNodeWithConfig(screenId, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: MULTI_SCREEN_TEST_032
 * @tc.desc: test whitelist in virtual expand screen (display->canvas->surface->canvas)
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MULTI_SCREEN_TEST_032)
{
    Vector4f canvasRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto canvasNode1 = CreateCanvasNodeWithConfig(canvasRect1, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH / 2, DEFAULT_BOUND_HEIGHT / 2);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", surfaceRect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f canvasRect2(0, 0, DEFAULT_BOUND_WIDTH / 4, DEFAULT_BOUND_HEIGHT / 4);
    auto canvasNode2 = CreateCanvasNodeWithConfig(canvasRect2, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    // sub tree 1 set parent
    canvasNode1->RSNode::AddChild(surfaceNode1);
    surfaceNode1->RSNode::AddChild(canvasNode2);

    Vector4f canvasRect3(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto canvasNode3 = CreateCanvasNodeWithConfig(canvasRect3, SK_ColorRED);
    ASSERT_NE(canvasNode3, nullptr);

    Vector4f surfaceRect2(0, 0, DEFAULT_BOUND_WIDTH / 2, DEFAULT_BOUND_HEIGHT / 2);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", surfaceRect2, SK_ColorGREEN);
    ASSERT_NE(surfaceNode2, nullptr);

    Vector4f canvasRect4(0, 0, DEFAULT_BOUND_WIDTH / 4, DEFAULT_BOUND_HEIGHT / 4);
    auto canvasNode4 = CreateCanvasNodeWithConfig(canvasRect4, SK_ColorRED);
    ASSERT_NE(canvasNode4, nullptr);

    // sub tree 2 set parent
    canvasNode3->RSNode::AddChild(surfaceNode2);
    surfaceNode2->RSNode::AddChild(canvasNode4);

    // create virtual screen
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen("MULTI_SCREEN_TEST_032",
        DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, {surfaceNode2->GetId()});
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // create csurface and psurface
    auto csurface = Surface::CreateSurfaceAsConsumer();
    csurface->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface, psurface);
    csurface->RegisterConsumerListener(listener);
    // set psurface
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDispalyNodeWithConfig(screenId, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(canvasNode1);
    displayNode->RSNode::AddChild(canvasNode3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}
/*
 * @tc.name: ResizeVirtualScreenTest004
 * @tc.desc: test ResizeVirtualScreen 0*0
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest004)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "ResizeVirtualScreenTest004", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, 0 * width, 0 * height);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: ResizeVirtualScreenTest005
 * @tc.desc: test ResizeVirtualScreen 65536*65536
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest005)
{
    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds({ 0, 0, 100, 100 });
    canvasNode0->SetFrame({ 0, 0, 100, 100 });
    canvasNode0->SetBackgroundColor(SK_ColorYELLOW);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds({ 0, 0, 200, 200 });
    canvasNode1->SetFrame({ 0, 0, 200, 200 });
    canvasNode1->SetBackgroundColor(SK_ColorBLUE);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode0 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode0->SetBounds({ 0, 0, 100, 100 });
    surfaceNode0->SetFrame({ 0, 0, 100, 100 });

    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode1";
    auto surfaceNode1 = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode1->SetBounds({ 0, 0, 200, 200 });
    surfaceNode1->SetFrame({ 0, 0, 200, 200 });

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "ResizeVirtualScreenTest005", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, 1000, 1000 });
    displayNode->SetFrame({ 0, 0, 1000, 1000 });
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    uint32_t maxWidth = 65536;
    uint32_t maxHeight = 65536;
    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, maxWidth, maxHeight);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest001
 * @tc.desc: test TakeSurfaceCapture when scaleX or scaleY is less than 1.0f
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest001)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "TakeSurfaceCaptureTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId,
        .isMirrored = false,
        .mirrorNodeId = 0,
        .isSync = true
    };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("TakeSurfaceCaptureTest001 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig surfaceCaptureConfig {
        .scaleX = 0.5f,
        .scaleY = 0.5f,
        .useDma = false,
        .useCurWindow = true,
        .captureType = SurfaceCaptureType::DEFAULT_CAPTURE,
        .isSync = false,
        .mainScreenRect = {},
        .blackList = {}, // exclude surfacenode in screenshot
        .isSoloNodeUiCapture = false,
        .isHdrCapture = false,
        .needF16WindowCaptureForScRGB = false,
        .uiCaptureInRangeParam = {},
        .specifiedAreaRect = {},
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT
    };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest002
 * @tc.desc: test TakeSurfaceCapture when scaleX and scaleY is bigger than 1.0f
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest002)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "TakeSurfaceCaptureTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId,
        .isMirrored = false,
        .mirrorNodeId = 0,
        .isSync = true
    };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("TakeSurfaceCaptureTest002 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig surfaceCaptureConfig {
        .scaleX = 1.5f,
        .scaleY = 1.5f,
        .useDma = false,
        .useCurWindow = true,
        .captureType = SurfaceCaptureType::DEFAULT_CAPTURE,
        .isSync = false,
        .mainScreenRect = {},
        .blackList = {}, // exclude surfacenode in screenshot
        .isSoloNodeUiCapture = false,
        .isHdrCapture = false,
        .needF16WindowCaptureForScRGB = false,
        .uiCaptureInRangeParam = {},
        .specifiedAreaRect = {},
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT
    };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest003
 * @tc.desc: test TakeSurfaceCapture useDma
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest003)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "TakeSurfaceCaptureTest003", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId,
        .isMirrored = false,
        .mirrorNodeId = 0,
        .isSync = true
    };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("TakeSurfaceCaptureTest003 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig surfaceCaptureConfig {
        .scaleX = 1.0f,
        .scaleY = 1.0f,
        .useDma = true,
        .useCurWindow = true,
        .captureType = SurfaceCaptureType::DEFAULT_CAPTURE,
        .isSync = false,
        .mainScreenRect = {},
        .blackList = {}, // exclude surfacenode in screenshot
        .isSoloNodeUiCapture = false,
        .isHdrCapture = false,
        .needF16WindowCaptureForScRGB = false,
        .uiCaptureInRangeParam = {},
        .specifiedAreaRect = {},
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT
    };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest004
 * @tc.desc: test TakeSurfaceCapture when not useCurWindow
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest004)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "TakeSurfaceCaptureTest004", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId,
        .isMirrored = false,
        .mirrorNodeId = 0,
        .isSync = true
    };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("TakeSurfaceCaptureTest004 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig surfaceCaptureConfig {
        .scaleX = 1.0f,
        .scaleY = 1.0f,
        .useDma = false,
        .useCurWindow = false,
        .captureType = SurfaceCaptureType::DEFAULT_CAPTURE,
        .isSync = false,
        .mainScreenRect = {},
        .blackList = {}, // exclude surfacenode in screenshot
        .isSoloNodeUiCapture = false,
        .isHdrCapture = false,
        .needF16WindowCaptureForScRGB = false,
        .uiCaptureInRangeParam = {},
        .specifiedAreaRect = {},
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT
    };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest005
 * @tc.desc: test TakeSurfaceCapture when has mainScreenRect
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest005)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "TakeSurfaceCaptureTest005", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId,
        .isMirrored = false,
        .mirrorNodeId = 0,
        .isSync = true
    };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("TakeSurfaceCaptureTest005 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig surfaceCaptureConfig {
        .scaleX = 1.0f,
        .scaleY = 1.0f,
        .useDma = false,
        .useCurWindow = true,
        .captureType = SurfaceCaptureType::DEFAULT_CAPTURE,
        .isSync = false,
        .mainScreenRect = { 600, 600, 1000, 1000 },
        .blackList = {}, // exclude surfacenode in screenshot
        .isSoloNodeUiCapture = false,
        .isHdrCapture = false,
        .needF16WindowCaptureForScRGB = false,
        .uiCaptureInRangeParam = {},
        .specifiedAreaRect = {},
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT
    };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest006
 * @tc.desc: test TakeSurfaceCapture when isHdrCapture
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest006)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "TakeSurfaceCaptureTest006", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId,
        .isMirrored = false,
        .mirrorNodeId = 0,
        .isSync = true
    };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("TakeSurfaceCaptureTest006 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig surfaceCaptureConfig {
        .scaleX = 1.0f,
        .scaleY = 1.0f,
        .useDma = false,
        .useCurWindow = true,
        .captureType = SurfaceCaptureType::DEFAULT_CAPTURE,
        .isSync = false,
        .mainScreenRect = {},
        .blackList = {}, // exclude surfacenode in screenshot
        .isSoloNodeUiCapture = false,
        .isHdrCapture = true,
        .needF16WindowCaptureForScRGB = false,
        .uiCaptureInRangeParam = {},
        .specifiedAreaRect = {},
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT
    };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest007
 * @tc.desc: test TakeSurfaceCapture when backGroundColor is not TRANSPARENT
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest007)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "TakeSurfaceCaptureTest007", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId,
        .isMirrored = false,
        .mirrorNodeId = 0,
        .isSync = true
    };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("TakeSurfaceCaptureTest007 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig surfaceCaptureConfig {
        .scaleX = 1.0f,
        .scaleY = 1.0f,
        .useDma = false,
        .useCurWindow = true,
        .captureType = SurfaceCaptureType::DEFAULT_CAPTURE,
        .isSync = false,
        .mainScreenRect = {},
        .blackList = {}, // exclude surfacenode in screenshot
        .isSoloNodeUiCapture = false,
        .isHdrCapture = false,
        .needF16WindowCaptureForScRGB = false,
        .uiCaptureInRangeParam = {},
        .specifiedAreaRect = {},
        .backGroundColor = Drawing::Color::COLOR_YELLOW
    };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest008
 * @tc.desc: test TakeSurfaceCapture when push surfacenode into blacklist
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest008)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "TakeSurfaceCaptureTest008", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId,
        .isMirrored = false,
        .mirrorNodeId = 0,
        .isSync = true
    };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("TakeSurfaceCaptureTest008 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSSurfaceCaptureConfig surfaceCaptureConfig {
        .scaleX = 1.0f,
        .scaleY = 1.0f,
        .useDma = false,
        .useCurWindow = true,
        .captureType = SurfaceCaptureType::DEFAULT_CAPTURE,
        .isSync = false,
        .mainScreenRect = {},
        .blackList = { surfaceNode->GetId() }, // exclude surfacenode in screenshot
        .isSoloNodeUiCapture = false,
        .isHdrCapture = false,
        .needF16WindowCaptureForScRGB = false,
        .uiCaptureInRangeParam = {},
        .specifiedAreaRect = {},
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT
    };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualScreenStatusTest001
 * @tc.desc: test SetVirtualScreenStatus VIRTUAL_SCREEN_PAUSE
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenStatusTest001)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenStatusTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("SetVirtualScreenStatusTest001 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
    
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSInterfaces::GetInstance().SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE);
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
 * @tc.name: SetVirtualScreenStatusTest002
 * @tc.desc: test SetVirtualScreenStatus VIRTUAL_SCREEN_PLAY
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenStatusTest002)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenStatusTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("SetVirtualScreenStatusTest002 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
    
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSInterfaces::GetInstance().SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);
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
 * @tc.name: SetVirtualScreenStatusTest003
 * @tc.desc: test SetVirtualScreenStatus VIRTUAL_SCREEN_INVALID_STATUS
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenStatusTest003)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenStatusTest003", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("SetVirtualScreenStatusTest003 screenId[%{public}" PRIu64 "], nodeId[%{public}" PRIu64 "]",
        screenId, displayNode->GetId());
    
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 0, 0, 1080, 1080 });
    canvasNode->SetFrame({ 0, 0, 1080, 1080 });
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestSurfaceNode0";
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({ 0, 0, 1080, 1080 });
    surfaceNode->SetFrame({ 0, 0, 1080, 1080 });
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBounds({ 0, 0, 1920, 1080 });
    displayNode->SetFrame({ 0, 0, 1920, 1080 });
    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    RSInterfaces::GetInstance().SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}
} // namespace OHOS::Rosen