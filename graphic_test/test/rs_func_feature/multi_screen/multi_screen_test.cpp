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
#include <iostream>
#include <mutex>
#include <tuple>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "pixel_map.h"
#include "pixel_map_from_surface.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_utils.h"
#include "token_setproc.h"

#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"

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

// Stitch multiple frames horizontally into one PixelMap and save to the standard test file name.
// Each frame is placed side-by-side; the final image width = frameWidth * count, height = frameHeight.
static void SaveStitchedPixelToFile(const std::vector<std::shared_ptr<Media::PixelMap>>& frames)
{
    if (frames.empty()) {
        LOGE("SaveStitchedPixelToFile: no frames to stitch");
        return;
    }
    int32_t frameW = frames[0]->GetWidth();
    int32_t frameH = frames[0]->GetHeight();
    int32_t totalW = frameW * static_cast<int32_t>(frames.size());
    Media::InitializationOptions opts;
    opts.size.width = totalW;
    opts.size.height = frameH;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    auto stitched = Media::PixelMap::Create(opts);
    if (stitched == nullptr) {
        LOGE("SaveStitchedPixelToFile: failed to create stitched pixelMap");
        return;
    }
    auto dstAddr = const_cast<uint8_t*>(stitched->GetPixels());
    int32_t dstStride = stitched->GetRowStride();
    for (size_t i = 0; i < frames.size(); i++) {
        auto& frame = frames[i];
        int32_t srcStride = frame->GetRowStride();
        auto srcAddr = const_cast<uint8_t*>(frame->GetPixels());
        int32_t copyBytes = std::min(frameW * 4, std::min(srcStride, dstStride));
        for (int32_t y = 0; y < frameH; y++) {
            memcpy_s(dstAddr + y * dstStride + static_cast<int32_t>(i) * frameW * 4,
                dstStride - static_cast<int32_t>(i) * frameW * 4, srcAddr + y * srcStride, copyBytes);
        }
    }
    auto sharedPtr = std::shared_ptr<Media::PixelMap>(stitched.release());
    SavePixelToFile(sharedPtr);
}

void SendMessagesAndSleep()
{
    RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);
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
    void OnSurfaceCaptureHDR(
        std::shared_ptr<Media::PixelMap> pixelMap, std::shared_ptr<Media::PixelMap> pixelMapHDR) override
    {}
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

// Multi-frame listener: caches all frames in OnBufferAvailable, then stitches them into
// one image via SaveStitchedFrames() at test end. Produces a single PNG with the standard
// test file name (frame0 | frame1 | frame2 ... laid out horizontally).
class MultiFrameBufferConsumerListener : public IBufferConsumerListener {
public:
    MultiFrameBufferConsumerListener(sptr<Surface> consumerSurface, sptr<Surface> producerSurface)
        : consumerSurface_(consumerSurface), producerSurface_(producerSurface)
    {}
    ~MultiFrameBufferConsumerListener() {}

    void OnBufferAvailable() override
    {
        {
            std::lock_guard<std::mutex> lock(framesMutex_);
            LOGI("MultiFrameBufferConsumerListener::OnBufferAvailable frame:%{public}zu", frames_.size());
        }
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
            LOGE("AcquireBuffer failed");
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
        {
            std::lock_guard<std::mutex> lock(framesMutex_);
            frames_.push_back(pixelMap);
        }
    }

    void SaveStitchedFrames()
    {
        std::vector<std::shared_ptr<Media::PixelMap>> snapshot;
        {
            std::lock_guard<std::mutex> lock(framesMutex_);
            snapshot = frames_;
        }
        SaveStitchedPixelToFile(snapshot);
    }

    size_t GetFrameCount() const
    {
        std::lock_guard<std::mutex> lock(framesMutex_);
        return frames_.size();
    }

    std::vector<std::shared_ptr<Media::PixelMap>> GetFrames() const
    {
        std::lock_guard<std::mutex> lock(framesMutex_);
        return frames_;
    }

    void ClearFrames()
    {
        std::lock_guard<std::mutex> lock(framesMutex_);
        frames_.clear();
    }

private:
    sptr<Surface> consumerSurface_ = nullptr;
    sptr<Surface> producerSurface_ = nullptr;
    mutable std::mutex framesMutex_;
    std::vector<std::shared_ptr<Media::PixelMap>> frames_;
};
} // namespace

class RSMultiScreenTest : public RSGraphicTest {
public:
    std::shared_ptr<RSRenderInterface> rsRenderInterface_ = nullptr;
    // called before each tests
    void BeforeEach() override
    {
        LOGI("%{public}s BeforeEach", GetTestName().c_str());
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
        auto screenId = OHOS::Rosen::RSInterfaces::GetInstance().GetDefaultScreenId();
        sptr<IRemoteObject> connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
        rsRenderInterface_ = std::make_shared<RSRenderInterface>(connectToRender);
    }
    // called after each tests
    void AfterEach() override {}

    // Get current test name for logging, such as "RSMultiScreenTest::TakeSurfaceCaptureTest001"
    std::string GetTestName() const
    {
        const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
        if (testInfo) {
            return std::string(testInfo->test_case_name()) + "::" + testInfo->name();
        }
        return "UnknownTest";
    }

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
        RSSurfaceNodeConfig config;
        config.isSync = true;
        config.SurfaceNodeName = name;
        auto surfaceNode = RSSurfaceNode::Create(config, true, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (surfaceNode == nullptr) {
            return nullptr;
        }
        surfaceNode->SetBounds(rect);
        surfaceNode->SetFrame(rect);
        surfaceNode->SetBackgroundColor(colorValue);
        surfaceNode->SetLeashPersistentId(surfaceNode->GetId());
        return surfaceNode;
    }

    std::shared_ptr<OHOS::Rosen::RSDisplayNode> CreateDisplayNodeWithConfig(
        const RSDisplayNodeConfig& config, const Vector4f& rect, uint32_t colorValue, bool securityDisplay = false)
    {
        auto displayNode = RSDisplayNode::Create(config, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (displayNode == nullptr) {
            return nullptr;
        }
        displayNode->SetBounds(rect);
        displayNode->SetFrame(rect);
        displayNode->SetBackgroundColor(colorValue);
        displayNode->SetSecurityDisplay(securityDisplay);
        LOGI("RSMultiScreenTest::CreateDisplayNodeWithConfig, %{public}s screenId[%{public}" PRIu64
             "], nodeId[%{public}" PRIu64 "]",
            GetTestName().c_str(), config.screenId, displayNode->GetId());
        std::cout << "RSMultiScreenTest::CreateDisplayNodeWithConfig, " << GetTestName().c_str() << " screenId["
                  << config.screenId << "], nodeId[" << displayNode->GetId() << "]" << std::endl;
        return displayNode;
    }

    std::shared_ptr<OHOS::Rosen::RSCanvasNode> CreateCanvasNodeWithConfig(const Vector4f& rect, uint32_t colorValue)
    {
        auto canvasNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (canvasNode == nullptr) {
            return nullptr;
        }
        canvasNode->SetBounds(rect);
        canvasNode->SetFrame(rect);
        canvasNode->SetBackgroundColor(colorValue);
        return canvasNode;
    }

    template<typename ListenerType>
    std::tuple<sptr<Surface>, sptr<Surface>, sptr<ListenerType>> CreateSurfaceWithListener()
    {
        auto csurface = Surface::CreateSurfaceAsConsumer();
        if (csurface == nullptr) {
            return { nullptr, nullptr, nullptr };
        }
        csurface->SetDefaultUsage(
            BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
        auto producer = csurface->GetProducer();
        auto psurface = Surface::CreateSurfaceAsProducer(producer);
        sptr<ListenerType> listener = sptr<ListenerType>::MakeSptr(csurface, psurface);
        sptr<IBufferConsumerListener> baseListener = listener;
        csurface->RegisterConsumerListener(baseListener);
        return { csurface, psurface, listener };
    }

    std::shared_ptr<Media::PixelMap> CreateSecurityMaskPixelMap(
        uint32_t width, uint32_t height, uint32_t colorValue = 0xffff0000)
    {
        uint32_t colorLength = width * height;
        std::vector<uint32_t> colorVec(colorLength, colorValue);
        uint32_t* color = colorVec.data();
        Media::InitializationOptions opts;
        opts.size.width = static_cast<int32_t>(width);
        opts.size.height = static_cast<int32_t>(height);
        opts.pixelFormat = Media::PixelFormat::RGBA_8888;
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        return Media::PixelMap::Create(color, colorLength, opts);
    }
};

/*
 * @tc.name: CreateVirtualScreenTest001
 * @tc.desc: Create a standalone 640x640 EXPAND virtual screen with no surface and no associated screen, then
 * TakeSurfaceCapture to verify the displayNode renders correctly
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode, nullptr);
    SendMessagesAndSleep();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: CreateVirtualScreenTest002
 * @tc.desc: Create a standalone 640x1000 EXPAND virtual screen (different aspect ratio from 001) with no surface, then
 * TakeSurfaceCapture to verify rendering
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest002)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode, nullptr);
    SendMessagesAndSleep();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: CreateVirtualScreenTest003
 * @tc.desc: Create a standalone 1280x640 landscape EXPAND virtual screen with no surface, then TakeSurfaceCapture to
 * verify wide-aspect rendering
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest003)
{
    uint32_t width = 640 * 2;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode, nullptr);
    SendMessagesAndSleep();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: CreateVirtualScreenTest004
 * @tc.desc: Create a mirror pair: screen1 EXPAND 640x640 as source, screen2 MIRROR with psurface and
 * associatedScreenId=screenId1, verify mirror rendering
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest004)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);
    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: CreateVirtualScreenTest005
 * @tc.desc: Create a mirror pair with 640x1000 resolution (portrait aspect), screen2 MIRROR with surface, verify
 * mirrored output
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest005)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);
    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: CreateVirtualScreenTest006
 * @tc.desc: Create a mirror pair with 1280x640 landscape resolution, screen2 MIRROR with surface, verify wide-aspect
 * mirror
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest006)
{
    uint32_t width = 2 * 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);
    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: CreateVirtualScreenTest007
 * @tc.desc: Create a mirror pair with 1920x1080 FullHD resolution, screen2 MIRROR with surface, verify high-resolution
 * mirror rendering
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest007)
{
    uint32_t width = 1920;
    uint32_t height = 1080;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);
    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetMirrorScreenVisibleRectTest001
 * @tc.desc: Set mirror screen visible rect to {100,100,100,500} to clip a sub-region of the mirror output, verify
 * partial mirror display
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetMirrorScreenVisibleRectTest001)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);
    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetMirrorScreenVisibleRect(screenId2, { 100, 100, 100, 500 });
    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetMirrorScreenVisibleRectTest002
 * @tc.desc: Set mirror screen visible rect that extends beyond screen bounds, verify robustness with out-of-range rect
 * coordinates
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetMirrorScreenVisibleRectTest002)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);
    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    // need foundation calling, or it will fail, ipc interface code access denied
    RSInterfaces::GetInstance().SetMirrorScreenVisibleRect(screenId2, { 100, 100, 100, 500 });
    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetMirrorScreenVisibleRectTest003
 * @tc.desc: Set mirror screen visible rect with rotation support enabled (third param=true), verify rotated mirror
 * clipping behavior
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetMirrorScreenVisibleRectTest003)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);
    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    // need foundation calling, or it will fail, ipc interface code access denied
    RSInterfaces::GetInstance().SetMirrorScreenVisibleRect(screenId2, { 100, 100, 100, 500 }, true);
    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleModeTest001
 * @tc.desc: Set mirror screen scale mode to FILL_MODE with mirror screen 2x/5x larger than source, verify fill scaling
 * stretches content
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenScaleModeTest001)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect1(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect1, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);
    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), 2 * width, 5 * height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenScaleMode(screenId2, ScreenScaleMode::FILL_MODE);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    Vector4f displayRect2(0, 0, 2 * width, 5 * height);
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect2, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleModeTest002
 * @tc.desc: Set mirror screen scale mode to UNISCALE_MODE with mirror screen 2x/5x larger, verify uniform scale
 * preserves aspect ratio
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenScaleModeTest002)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect1(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect1, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), 2 * width, 5 * height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenScaleMode(screenId2, ScreenScaleMode::UNISCALE_MODE);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    Vector4f displayRect2(0, 0, 2 * width, 5 * height);
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect2, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleModeTest003
 * @tc.desc: Set mirror screen scale mode to INVALID_MODE, verify boundary handling of invalid scale mode enum
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenScaleModeTest003)
{
    uint32_t width = 640;
    uint32_t height = 1000;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect1(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect1, SK_ColorBLUE);
    ASSERT_NE(displayNode1, nullptr);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), 2 * width, 5 * height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenScaleMode(screenId2, ScreenScaleMode::INVALID_MODE);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    Vector4f displayRect2(0, 0, 2 * width, 5 * height);
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect2, SK_ColorBLUE);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualScreenSurfaceTest001
 * @tc.desc: Create screen without initial surface then SetVirtualScreenSurface afterwards (640x1000), add child
 * surfaceNode to verify content renders through late-bound surface
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenSurfaceTest001)
{
    uint32_t width = 640;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);

    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 640, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode, nullptr);
    auto surfaceNode = CreateSurfaceNodeWithConfig("TestVirtualScreen_005", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualScreenSurfaceTest002
 * @tc.desc: Create screen without initial surface then SetVirtualScreenSurface (640x640 square), verify late-bound
 * surface with square aspect ratio
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenSurfaceTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);

    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 640, 640);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLUE);
    ASSERT_NE(displayNode, nullptr);
    auto surfaceNode = CreateSurfaceNodeWithConfig("TestVirtualScreen_006", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualScreenBlackListTest001
 * @tc.desc: Mirror screen with surfaceNode1 in screenBlackList, both surfaceNode0 and surfaceNode1 on source, verify
 * blacklist filters surfaceNode1 from mirror
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenBlackListTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("%{public}s surfaceId1:[%{public}" PRIu64 "]", GetTestName().c_str(), surfaceNode1->GetId());
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(screenId2, screenBlackList);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualScreenBlackListTest002
 * @tc.desc: Mirror screen with blacklist set on INVALID_SCREEN_ID (public list) instead of screenId2, verify no
 * screen-level filtering when list is public only
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenBlackListTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("%{public}s surfaceId1:[%{public}" PRIu64 "]", GetTestName().c_str(), surfaceNode1->GetId());
    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);
    // Screen blocklist has no data, both are displayed
    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualScreenBlackListTest003
 * @tc.desc: Mirror screen with blacklist on INVALID_SCREEN_ID, verify robustness when no screen-specific blacklist is
 * configured
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenBlackListTest003)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("%{public}s surfaceId1:[%{public}" PRIu64 "]", GetTestName().c_str(), surfaceNode1->GetId());

    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);

    // Screen blocklist has no data, both are displayed
    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/**
 * @tc.name: BlacklistAndWhitelistTest001
 * @tc.desc: Expand screen with surfaceNode1 in blacklist; surfaceNode2 visible, verify blacklist filters designated
 * node from expand display
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest001)
{
    Vector4f rect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", rect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f rect2(0, 0, DEFAULT_BOUND_WIDTH / 2.f, DEFAULT_BOUND_HEIGHT / 2.f);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", rect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    // create virtual screen
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    // set virtual screen black list
    std::vector<NodeId> list = { surfaceNode1->GetId() };
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(screenId, list);

    // create csurface and psurface
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    // set psurface
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode2);

    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: BlacklistAndWhitelistTest002
 * @tc.desc: Expand screen with both blacklist and whitelist (whitelist via CreateVirtualScreen), surfaceNode1 in both
 * lists; verify whitelist takes precedence
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest002)
{
    Vector4f rect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", rect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f rect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", rect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    // create virtual screen
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), DEFAULT_SCREEN_WIDTH,
        DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, { surfaceNode1->GetId() });
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    // set virtual screen black list
    std::vector<NodeId> list = { surfaceNode1->GetId() };
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(screenId, list);

    // create csurface and psurface
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    // set psurface
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode2);

    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: BlacklistAndWhitelistTest003
 * @tc.desc: Expand screen with whitelist containing parent surfaceNode1 which has child surfaceNode2, verify child
 * inherits whitelist visibility from parent
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest003)
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
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), DEFAULT_SCREEN_WIDTH,
        DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, { surfaceNode1->GetId() });
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // create csurface and psurface
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    // set psurface
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);

    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: BlacklistAndWhitelistTest004
 * @tc.desc: Expand screen with deep node tree (display->canvas->surface->canvas), whitelist only surfaceNode2, verify
 * multi-level subtree whitelist propagation
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest004)
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
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), DEFAULT_SCREEN_WIDTH,
        DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, { surfaceNode2->GetId() });
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // create csurface and psurface
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    // set psurface
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(canvasNode1);
    displayNode->RSNode::AddChild(canvasNode3);

    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: BlacklistAndWhitelistTest005
 * @tc.desc: Expand screen with AddVirtualScreenWhiteList called after screen creation for surfaceNode1, verify dynamic
 * whitelist addition
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest005)
{
    Vector4f rect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", rect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f rect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", rect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().AddVirtualScreenWhiteList(screenId, { surfaceNode1->GetId() });

    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode2);

    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: BlacklistAndWhitelistTest006
 * @tc.desc: Expand screen with AddVirtualScreenWhiteList using screenId+1 (mismatch), verify whitelist robustness with
 * incorrect screenId
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest006)
{
    Vector4f rect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", rect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f rect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", rect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    // add virtual screen white list (screenId mismatch)
    RSInterfaces::GetInstance().AddVirtualScreenWhiteList(screenId + 1, { surfaceNode1->GetId() });

    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode2);

    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: BlacklistAndWhitelistTest007
 * @tc.desc: Expand screen with RemoveVirtualScreenWhiteList removing previously added whitelist node, verify dynamic
 * whitelist removal
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest007)
{
    Vector4f rect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", rect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f rect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", rect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), DEFAULT_SCREEN_WIDTH,
        DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, { surfaceNode1->GetId() });
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().RemoveVirtualScreenWhiteList(screenId, { surfaceNode1->GetId() });

    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode2);

    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: BlacklistAndWhitelistTest008
 * @tc.desc: Expand screen with RemoveVirtualScreenWhiteList using screenId+1 (mismatch), verify removal robustness with
 * incorrect screenId
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest008)
{
    Vector4f rect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestExpandScreen_01", rect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);

    Vector4f rect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestExpandScreen_02", rect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), DEFAULT_SCREEN_WIDTH,
        DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, -1, { surfaceNode1->GetId() });
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    // remove virtual screen white list (screenId mismatch)
    RSInterfaces::GetInstance().RemoveVirtualScreenWhiteList(screenId + 1, { surfaceNode1->GetId() });

    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId, psurface);

    RSDisplayNodeConfig displayNodeConfig = { screenId, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode, nullptr);
    // add surface node as display node child
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode2);

    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: BlacklistAndWhitelistTest009
 * @tc.desc: Mirror screen with AddVirtualScreenWhiteList, same as 005 but in MIRROR mode, verify whitelist behavior in
 * mirror display
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest009)
{
    // create surface node 1
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);
    // create surface node 2
    Vector4f surfaceRect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestMirrorScreen_02", surfaceRect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create disaply1
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);
    // add child
    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode2);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().AddVirtualScreenWhiteList(screenId2, { surfaceNode1->GetId() });

    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create disaply2
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, true);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: BlacklistAndWhitelistTest010
 * @tc.desc: Mirror screen with RemoveVirtualScreenWhiteList, same as 007 but in MIRROR mode, verify whitelist removal
 * in mirror display
 * @tc.type: FUNC
 * @tc.require: issue20923
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, BlacklistAndWhitelistTest010)
{
    // create surface node 1
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);
    // create surface node 2
    Vector4f surfaceRect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestMirrorScreen_02", surfaceRect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create disaply1
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);
    // add child
    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode2);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), DEFAULT_SCREEN_WIDTH,
        DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, { surfaceNode1->GetId() });
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);

    RSInterfaces::GetInstance().RemoveVirtualScreenWhiteList(screenId2, { surfaceNode1->GetId() });
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create disaply2
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, true);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindowTest001
 * @tc.desc: Mirror screen with public blacklist containing surfaceNode1 and SetCastScreenEnableSkipWindow(true), verify
 * public blacklist filtering when skip window is enabled
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetCastScreenEnableSkipWindowTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("%{public}s surfaceId:[%{public}" PRIu64 "]", GetTestName().c_str(), surfaceNode1->GetId());

    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);

    // Open the public blocklist, turn on the switch to read the public blocklist, and only display ID1 in yellow
    RSInterfaces::GetInstance().SetCastScreenEnableSkipWindow(screenId2, true);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindowTest002
 * @tc.desc: Mirror screen with public blacklist containing surfaceNode1 and SetCastScreenEnableSkipWindow(false),
 * verify all nodes visible when skip window is disabled
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetCastScreenEnableSkipWindowTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});

    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    std::vector<uint64_t> screenBlackList = { surfaceNode1->GetId() };
    LOGI("%{public}s surfaceId:[%{public}" PRIu64 "]", GetTestName().c_str(), surfaceNode1->GetId());

    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);

    // The public blocklist is not open, displaying all
    RSInterfaces::GetInstance().SetCastScreenEnableSkipWindow(screenId2, false);
    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindowTest003
 * @tc.desc: Mirror screen with empty public blacklist and SetCastScreenEnableSkipWindow(true), verify no filtering
 * occurs with empty list even when enabled
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetCastScreenEnableSkipWindowTest003)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});

    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    std::vector<uint64_t> screenBlackList = {};
    LOGI("%{public}s surfaceId:[%{public}" PRIu64 "]", GetTestName().c_str(), surfaceNode1->GetId());

    // Add blocklist to public blocklist
    RSInterfaces::GetInstance().SetVirtualScreenBlackList(INVALID_SCREEN_ID, screenBlackList);

    // The public blocklist is not open, displaying all
    RSInterfaces::GetInstance().SetCastScreenEnableSkipWindow(screenId2, true);
    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetScreenCorrectionTest001
 * @tc.desc: SetScreenCorrection with ROTATION_0 on expand screen with two surface child nodes, baseline with no
 * rotation correction applied
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenCorrectionTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().SetScreenCorrection(screenId, ScreenRotation::ROTATION_0);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetScreenCorrectionTest002
 * @tc.desc: SetScreenCorrection with ROTATION_90 on expand screen, verify 90-degree screen rotation correction
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenCorrectionTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().SetScreenCorrection(screenId, ScreenRotation::ROTATION_90);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetScreenCorrectionTest003
 * @tc.desc: SetScreenCorrection with ROTATION_180 on expand screen, verify 180-degree screen rotation correction
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenCorrectionTest003)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().SetScreenCorrection(screenId, ScreenRotation::ROTATION_180);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetScreenCorrectionTest004
 * @tc.desc: SetScreenCorrection with ROTATION_270 on expand screen, verify 270-degree screen rotation correction
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenCorrectionTest004)
{
    uint32_t width = 640;
    uint32_t height = 640;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode0";
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    surfaceNodeConfig.SurfaceNodeName = "TestsurfaceNode1";
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);

    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().SetScreenCorrection(screenId, ScreenRotation::ROTATION_270);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotationTest001
 * @tc.desc: SetVirtualMirrorScreenCanvasRotation(false) then rotate source screen 90 degrees, verify mirror without
 * canvas rotation compensation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenCanvasRotationTest001)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 200 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 300, 300 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceNode1->SetFrame({ 0, 0, 400, 400 }); // preserve different frame

    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenCanvasRotation(screenId2, false);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    // To avoid rendering node2 only after node1 has already rotated, it is written here to delay the rotation
    displayNode1->SetScreenRotation(static_cast<uint32_t>(ScreenRotation::ROTATION_90));
    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotationTest002
 * @tc.desc: SetVirtualMirrorScreenCanvasRotation(true) then rotate source screen 90 degrees, verify mirror with canvas
 * rotation compensation for correct orientation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualMirrorScreenCanvasRotationTest002)
{
    uint32_t width = 640;
    uint32_t height = 640;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 200 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 300, 300 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceNode1->SetFrame({ 0, 0, 400, 400 }); // preserve different frame

    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, -1, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSInterfaces::GetInstance().SetVirtualMirrorScreenCanvasRotation(screenId2, true);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    // To avoid rendering node2 only after node1 has already rotated, it is written here to delay the rotation
    displayNode1->SetScreenRotation(static_cast<uint32_t>(ScreenRotation::ROTATION_90));
    SendMessagesAndSleep();
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetScreenSecurityMaskTest001
 * @tc.desc: Mirror screen with security surfaceNode (SetSecurityLayer=true) but no SetScreenSecurityMask, baseline with
 * security layer present but no mask applied
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenSecurityMaskTest001)
{
    uint32_t width = 640 * 2;
    uint32_t height = 640 * 2;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 200 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    // Set SecurityLayer for surfaceNode0
    surfaceNode0->SetSecurityLayer(true);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestsurfaceNode1", { 0, 0, 300, 300 }, SK_ColorBLUE);
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceNode1->SetFrame({ 0, 0, 400, 400 }); // preserve different frame

    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetScreenSecurityMaskTest002
 * @tc.desc: Mirror screen with security surfaceNode and SetScreenSecurityMask with 640x640 red pixelMap, verify
 * security mask overlays on security surface area
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenSecurityMaskTest002)
{
    uint32_t width = 640 * 2;
    uint32_t height = 640 * 2;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 200 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);
    surfaceNode0->SetSecurityLayer(true);

    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);
    // init pixelMap
    std::shared_ptr<Media::PixelMap> pixelMap = CreateSecurityMaskPixelMap(640, 640, 0xffff0000);
    // only foundation can call Manual block stub permission check
    RSInterfaces::GetInstance().SetScreenSecurityMask(screenId2, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetScreenSecurityMaskTest003
 * @tc.desc: Mirror screen with SetScreenSecurityMask red pixelMap but no security surfaceNode, verify mask is not
 * applied without a security surface node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetScreenSecurityMaskTest003)
{
    uint32_t width = 640 * 2;
    uint32_t height = 640 * 2;

    // mirrorSourceScreen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 {
        .screenId = screenId1, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode1, nullptr);
    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestsurfaceNode0", { 0, 0, 100, 200 }, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode0, nullptr);

    displayNode1->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    // mirrorScreen
    auto [csurface2, psurface2, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface2, nullptr);
    ASSERT_NE(psurface2, nullptr);

    ScreenId screenId2 =
        RSInterfaces::GetInstance().CreateVirtualScreen(GetTestName(), width, height, psurface2, screenId1, 0, {});
    EXPECT_NE(screenId2, INVALID_SCREEN_ID);

    // init pixelMap
    std::shared_ptr<Media::PixelMap> pixelMap = CreateSecurityMaskPixelMap(640, 640, 0xffff0000);

    // only foundation can call Manual block stub permission check
    RSInterfaces::GetInstance().SetScreenSecurityMask(screenId2, pixelMap);
    usleep(SLEEP_TIME_FOR_PROXY);

    RSDisplayNodeConfig displayNodeConfig2 {
        .screenId = screenId2, .displayMode = DisplayMode::MIRROR, .mirrorNodeId = displayNode1->GetId(), .isSync = true
    };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode2, nullptr);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest001
 * @tc.desc: SetVirtualScreenResolution to 4000x3000 (4:3 landscape) obtained from second screen, verify large landscape
 * resolution applied
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest001)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), 4 * width, 3 * height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest002
 * @tc.desc: SetVirtualScreenResolution to 3000x3000 (1:1 square) directly, verify large square resolution applied
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest002)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, 3 * width, 3 * height);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest003
 * @tc.desc: SetVirtualScreenResolution to 3000x4000 (3:4 portrait) from second screen, verify large portrait resolution
 * applied
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest003)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), 3 * width, 4 * height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest004
 * @tc.desc: SetVirtualScreenResolution to 0x0, boundary test for zero resolution, verify robustness with invalid
 * dimensions
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest004)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), 0 * width, 0 * height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest005
 * @tc.desc: SetVirtualScreenResolution to 65536x65536, boundary test for oversized dimensions (2^16), verify upper
 * limit handling
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest005)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 65536;
    uint32_t height = 65536;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: ResizeVirtualScreenTest001
 * @tc.desc: ResizeVirtualScreen to 3000x4000 (3:4 portrait) from second screen, verify resize changes output buffer
 * dimensions
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest001)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), 3 * width, 4 * height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: ResizeVirtualScreenTest002
 * @tc.desc: ResizeVirtualScreen to 4000x3000 (4:3 landscape) from second screen, verify landscape resize
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest002)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    ScreenId secondScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), 4 * width, 3 * height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(secondScreenId, INVALID_SCREEN_ID);
    RSVirtualScreenResolution rsVirtualScreenResolution =
        RSInterfaces::GetInstance().GetVirtualScreenResolution(secondScreenId);

    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, rsVirtualScreenResolution.GetVirtualScreenWidth(),
        rsVirtualScreenResolution.GetVirtualScreenHeight());

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
    RSInterfaces::GetInstance().RemoveVirtualScreen(secondScreenId);
}

/*
 * @tc.name: ResizeVirtualScreenTest003
 * @tc.desc: ResizeVirtualScreen to 2000x2000 (1:1 square) directly, verify square resize
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest003)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, 2 * width, 2 * height);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: ResizeVirtualScreenTest004
 * @tc.desc: ResizeVirtualScreen to 0x0, boundary test for zero size resize, verify robustness with invalid dimensions
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest004)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, 0 * width, 0 * height);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: ResizeVirtualScreenTest005
 * @tc.desc: ResizeVirtualScreen to 65536x65536, boundary test for oversized resize (2^16), verify upper limit handling
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, ResizeVirtualScreenTest005)
{
    auto canvasNode0 = CreateCanvasNodeWithConfig({ 0, 0, 100, 100 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode0, nullptr);

    auto canvasNode1 = CreateCanvasNodeWithConfig({ 0, 0, 200, 200 }, SK_ColorBLUE);
    ASSERT_NE(canvasNode1, nullptr);

    auto surfaceNode0 = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 100, 100 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode0, nullptr);

    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestSurfaceNode1", { 0, 0, 200, 200 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode0->RSNode::AddChild(canvasNode0);
    surfaceNode1->RSNode::AddChild(canvasNode1);

    uint32_t width = 1000;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1000, 1000);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode1);
    displayNode->RSNode::AddChild(surfaceNode0);

    SendMessagesAndSleep();

    uint32_t maxWidth = 65536;
    uint32_t maxHeight = 65536;
    RSInterfaces::GetInstance().ResizeVirtualScreen(screenId, maxWidth, maxHeight);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest001
 * @tc.desc: TakeSurfaceCapture with scaleX=0.5 and scaleY=0.5 (downscale) using default useDma/useCurWindow, verify
 * downscaled capture callback
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest001)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);

    displayNode->SetBackgroundColor(SK_ColorGREEN);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();

    RSSurfaceCaptureConfig surfaceCaptureConfig { .scaleX = 0.5f,
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
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest002
 * @tc.desc: TakeSurfaceCapture with scaleX=1.5 and scaleY=1.5 (upscale) with fallback to scale=1.0 and red background
 * if callback fails, verify upscale degradation path
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest002)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();

    RSSurfaceCaptureConfig surfaceCaptureConfig { .scaleX = 1.5f,
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
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    if (!callback->isCallbackCalled_) {
        surfaceCaptureConfig.scaleX = 1.0f;
        surfaceCaptureConfig.scaleY = 1.0f;
        canvasNode->SetBackgroundColor(SK_ColorRED);
        SendMessagesAndSleep();
        rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest003
 * @tc.desc: TakeSurfaceCapture with useDma=true and scale=1.0, verify DMA buffer capture path
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest003)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();

    RSSurfaceCaptureConfig surfaceCaptureConfig { .scaleX = 1.0f,
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
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest004
 * @tc.desc: TakeSurfaceCapture with useCurWindow=false and scale=1.0, verify capture without current window binding
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest004)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();

    RSSurfaceCaptureConfig surfaceCaptureConfig { .scaleX = 1.0f,
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
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest005
 * @tc.desc: TakeSurfaceCapture with mainScreenRect={600,600,1000,1000} and scale=1.0, verify region-specific capture
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest005)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();

    RSSurfaceCaptureConfig surfaceCaptureConfig { .scaleX = 1.0f,
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
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest006
 * @tc.desc: TakeSurfaceCapture with isHdrCapture=true and fallback to false with red background if callback fails,
 * verify HDR capture path and degradation
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest006)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();

    RSSurfaceCaptureConfig surfaceCaptureConfig { .scaleX = 1.0f,
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
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    if (!callback->isCallbackCalled_) {
        surfaceCaptureConfig.isHdrCapture = false;
        canvasNode->SetBackgroundColor(SK_ColorRED);
        SendMessagesAndSleep();
        rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest007
 * @tc.desc: TakeSurfaceCapture with backGroundColor=YELLOW (non-transparent) and scale=1.0, verify non-transparent
 * background affects capture result
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest007)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();

    RSSurfaceCaptureConfig surfaceCaptureConfig { .scaleX = 1.0f,
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
        .backGroundColor = Drawing::Color::COLOR_YELLOW };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: TakeSurfaceCaptureTest008
 * @tc.desc: TakeSurfaceCapture with blackList containing the target surfaceNode itself, verify captured node is
 * excluded from the capture output
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, TakeSurfaceCaptureTest008)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    SendMessagesAndSleep();

    RSSurfaceCaptureConfig surfaceCaptureConfig { .scaleX = 1.0f,
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
        .backGroundColor = Drawing::Color::COLOR_TRANSPARENT };

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback, surfaceCaptureConfig);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualScreenStatusTest001
 * @tc.desc: SetVirtualScreenStatus to VIRTUAL_SCREEN_PAUSE, verify screen rendering enters paused state
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenStatusTest001)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    RSInterfaces::GetInstance().SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE);
    SendMessagesAndSleep();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualScreenStatusTest002
 * @tc.desc: SetVirtualScreenStatus to VIRTUAL_SCREEN_PLAY, verify screen rendering resumes playing state
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenStatusTest002)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    RSInterfaces::GetInstance().SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);
    SendMessagesAndSleep();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetVirtualScreenStatusTest003
 * @tc.desc: SetVirtualScreenStatus to VIRTUAL_SCREEN_INVALID_STATUS, boundary test for invalid status enum, verify
 * robustness
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetVirtualScreenStatusTest003)
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 1920, 1080);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN);
    ASSERT_NE(displayNode, nullptr);

    auto canvasNode = CreateCanvasNodeWithConfig({ 0, 0, 1080, 1080 }, SK_ColorYELLOW);
    ASSERT_NE(canvasNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("TestSurfaceNode0", { 0, 0, 1080, 1080 }, SK_ColorTRANSPARENT);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSNode::AddChild(canvasNode);
    displayNode->RSNode::AddChild(surfaceNode);
    RSInterfaces::GetInstance().SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS);
    SendMessagesAndSleep();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    rsRenderInterface_->TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("%{public}s TakeSurfaceCapture failed", GetTestName().c_str());
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: DrawExpandDisplayTest001
 * @tc.desc: DrawExpandDisplay with no SetPivot/SetScale on displayNode, baseline expand rendering without transform
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, DrawExpandDisplayTest001)
{
    RSInterfaces& rsInterfaces = RSInterfaces::GetInstance();

    // Create Virtual Screen
    uint32_t width = 100;
    uint32_t height = 100;
    ScreenId screenId = rsInterfaces.CreateVirtualScreen(GetTestName(), width, height, nullptr, INVALID_SCREEN_ID);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // Set Surface for Virtual Screen
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    rsInterfaces.SetVirtualScreenSurface(screenId, psurface);
    usleep(SLEEP_TIME_FOR_PROXY);

    // Create Display Node
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 200, 200);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN, true);
    ASSERT_NE(displayNode, nullptr);

    // Create Surface Node
    Vector4f surfaceNodeRect(0, 0, 100, 100);
    auto surfaceNode = CreateSurfaceNodeWithConfig(__func__, surfaceNodeRect, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);

    SendMessagesAndSleep();
    rsInterfaces.RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: DrawExpandDisplayTest002
 * @tc.desc: DrawExpandDisplay with SetPivot(0,0) and SetScale(0.5,0.5), uniform downscale from top-left origin
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, DrawExpandDisplayTest002)
{
    RSInterfaces& rsInterfaces = RSInterfaces::GetInstance();

    // Create Virtual Screen
    uint32_t width = 100;
    uint32_t height = 100;
    ScreenId screenId = rsInterfaces.CreateVirtualScreen(GetTestName(), width, height, nullptr, INVALID_SCREEN_ID);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // Set Surface for Virtual Screen
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    rsInterfaces.SetVirtualScreenSurface(screenId, psurface);
    usleep(SLEEP_TIME_FOR_PROXY);

    // Create Display Node
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 200, 200);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN, true);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetPivot(0.0f, 0.0f);
    displayNode->SetScale(0.5f, 0.5f);

    // Create Surface Node
    Vector4f surfaceNodeRect(0, 0, 100, 100);
    auto surfaceNode = CreateSurfaceNodeWithConfig(__func__, surfaceNodeRect, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);

    SendMessagesAndSleep();
    rsInterfaces.RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: DrawExpandDisplayTest003
 * @tc.desc: DrawExpandDisplay with SetPivot(0,0) and SetScale(0.5,0.25), non-uniform scale with X>Y compression ratio
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, DrawExpandDisplayTest003)
{
    RSInterfaces& rsInterfaces = RSInterfaces::GetInstance();

    // Create Virtual Screen
    uint32_t width = 100;
    uint32_t height = 100;
    ScreenId screenId = rsInterfaces.CreateVirtualScreen(GetTestName(), width, height, nullptr, INVALID_SCREEN_ID);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // Set Surface for Virtual Screen
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    rsInterfaces.SetVirtualScreenSurface(screenId, psurface);
    usleep(SLEEP_TIME_FOR_PROXY);

    // Create Display Node
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 200, 400);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN, true);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetPivot(0.0f, 0.0f);
    displayNode->SetScale(0.5f, 0.25f);

    // Create Surface Node
    Vector4f surfaceNodeRect(0, 0, 100, 100);
    auto surfaceNode = CreateSurfaceNodeWithConfig(__func__, surfaceNodeRect, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);

    SendMessagesAndSleep();
    rsInterfaces.RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: DrawExpandDisplayTest004
 * @tc.desc: DrawExpandDisplay with SetPivot(0,0) and SetScale(0.25,0.5), non-uniform scale with Y>X compression ratio
 * (symmetric to 003)
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, DrawExpandDisplayTest004)
{
    RSInterfaces& rsInterfaces = RSInterfaces::GetInstance();

    // Create Virtual Screen
    uint32_t width = 100;
    uint32_t height = 100;
    ScreenId screenId = rsInterfaces.CreateVirtualScreen(GetTestName(), width, height, nullptr, INVALID_SCREEN_ID);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // Set Surface for Virtual Screen
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    rsInterfaces.SetVirtualScreenSurface(screenId, psurface);
    usleep(SLEEP_TIME_FOR_PROXY);

    // Create Display Node
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 400, 200);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN, true);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetPivot(0.0f, 0.0f);
    displayNode->SetScale(0.25f, 0.5f);

    // Create Surface Node
    Vector4f surfaceNodeRect(0, 0, 100, 100);
    auto surfaceNode = CreateSurfaceNodeWithConfig(__func__, surfaceNodeRect, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);

    SendMessagesAndSleep();
    rsInterfaces.RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: DrawExpandDisplayTest005
 * @tc.desc: DrawExpandDisplay with SetPivot(0.25,0.5) and SetScale(0.25,0.5), non-origin pivot point affecting scale
 * transform center
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, DrawExpandDisplayTest005)
{
    RSInterfaces& rsInterfaces = RSInterfaces::GetInstance();

    // Create Virtual Screen
    uint32_t width = 100;
    uint32_t height = 100;
    ScreenId screenId = rsInterfaces.CreateVirtualScreen(GetTestName(), width, height, nullptr, INVALID_SCREEN_ID);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // Set Surface for Virtual Screen
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    rsInterfaces.SetVirtualScreenSurface(screenId, psurface);
    usleep(SLEEP_TIME_FOR_PROXY);

    // Create Display Node
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 400, 200);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN, true);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetPivot(0.25f, 0.5f);
    displayNode->SetScale(0.25f, 0.5f);

    // Create Surface Node
    Vector4f surfaceNodeRect(0, 0, 100, 100);
    auto surfaceNode = CreateSurfaceNodeWithConfig(__func__, surfaceNodeRect, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);

    SendMessagesAndSleep();
    rsInterfaces.RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: DrawExpandDisplayTest006
 * @tc.desc: DrawExpandDisplay with SetPivot(0.5,0.5) and SetScale(0.5,0.5), center pivot uniform downscale
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, DrawExpandDisplayTest006)
{
    RSInterfaces& rsInterfaces = RSInterfaces::GetInstance();

    // Create Virtual Screen
    uint32_t width = 100;
    uint32_t height = 100;
    ScreenId screenId = rsInterfaces.CreateVirtualScreen(GetTestName(), width, height, nullptr, INVALID_SCREEN_ID);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // Set Surface for Virtual Screen
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    rsInterfaces.SetVirtualScreenSurface(screenId, psurface);
    usleep(SLEEP_TIME_FOR_PROXY);

    // Create Display Node
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 200, 200);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN, true);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetPivot(0.5f, 0.5f);
    displayNode->SetScale(0.5f, 0.5f);

    // Create Surface Node
    Vector4f surfaceNodeRect(0, 0, 100, 100);
    auto surfaceNode = CreateSurfaceNodeWithConfig(__func__, surfaceNodeRect, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);

    SendMessagesAndSleep();
    rsInterfaces.RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: DrawExpandDisplayTest007
 * @tc.desc: DrawExpandDisplay with SetPivot(0.5,0.25) and SetScale(0.5,0.25), non-origin pivot combined with
 * non-uniform scale
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, DrawExpandDisplayTest007)
{
    RSInterfaces& rsInterfaces = RSInterfaces::GetInstance();

    // Create Virtual Screen
    uint32_t width = 100;
    uint32_t height = 100;
    ScreenId screenId = rsInterfaces.CreateVirtualScreen(GetTestName(), width, height, nullptr, INVALID_SCREEN_ID);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    // Set Surface for Virtual Screen
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    rsInterfaces.SetVirtualScreenSurface(screenId, psurface);
    usleep(SLEEP_TIME_FOR_PROXY);

    // Create Display Node
    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, 200, 400);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorGREEN, true);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetPivot(0.5f, 0.25f);
    displayNode->SetScale(0.5f, 0.25f);

    // Create Surface Node
    Vector4f surfaceNodeRect(0, 0, 100, 100);
    auto surfaceNode = CreateSurfaceNodeWithConfig(__func__, surfaceNodeRect, SK_ColorYELLOW);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);

    SendMessagesAndSleep();
    rsInterfaces.RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: SetSkipLayerTest001
 * @tc.desc: SetSkipLayer(true) on top-level surfaceNode in mirror screen with non-security display, verify skip layer
 * excludes node from mirror
 * @tc.type: FUNC
 * @tc.require: issue23278
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetSkipLayerTest001)
{
    // create surface node (skip layer)
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorRED);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create main display
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);
    // add child
    displayNode1->RSNode::AddChild(surfaceNode);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create mirror display (non security display)
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, false);
    ASSERT_NE(displayNode2, nullptr);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetSkipLayerTest002
 * @tc.desc: SetSkipLayer(true) on top-level surfaceNode in mirror screen with security display, verify skip layer
 * behavior under security display
 * @tc.type: FUNC
 * @tc.require: issue23278
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetSkipLayerTest002)
{
    // create surface node (skip layer)
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorRED);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create main display
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);
    // add child
    displayNode1->RSNode::AddChild(surfaceNode);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create mirror display (security display)
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, true);
    ASSERT_NE(displayNode2, nullptr);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetSkipLayerTest003
 * @tc.desc: SetSkipLayer(true) on child surfaceNode (sub-window) in non-security mirror, verify child node skip layer
 * in parent-child hierarchy
 * @tc.type: FUNC
 * @tc.require: issue23278
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetSkipLayerTest003)
{
    // create surface node 1
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);
    // create surface node 2 (skip layer)
    Vector4f surfaceRect2(0, 0, DEFAULT_BOUND_WIDTH / 2.f, DEFAULT_BOUND_HEIGHT / 2.f);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestMirrorScreen_02", surfaceRect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetSkipLayer(true);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create main display
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);

    // add child
    displayNode1->RSNode::AddChild(surfaceNode1);
    surfaceNode1->RSNode::AddChild(surfaceNode2);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create mirror display (non security display)
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, false);
    ASSERT_NE(displayNode2, nullptr);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetSkipLayerTest004
 * @tc.desc: SetSkipLayer(true) on child surfaceNode (sub-window) in security mirror, verify child skip layer under
 * security display
 * @tc.type: FUNC
 * @tc.require: issue23278
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetSkipLayerTest004)
{
    // create surface node 1
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);
    // create surface node 2 (skip layer)
    Vector4f surfaceRect2(0, 0, DEFAULT_BOUND_WIDTH / 2.f, DEFAULT_BOUND_HEIGHT / 2.f);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestMirrorScreen_02", surfaceRect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetSkipLayer(true);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create main display
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);

    // add child
    displayNode1->RSNode::AddChild(surfaceNode1);
    surfaceNode1->RSNode::AddChild(surfaceNode2);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create mirror display (security display)
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, true);
    ASSERT_NE(displayNode2, nullptr);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetSkipLayerTest005
 * @tc.desc: SetSkipLayer(true) on sibling surfaceNode in non-security mirror, verify sibling node skip layer in
 * parallel hierarchy
 * @tc.type: FUNC
 * @tc.require: issue23278
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetSkipLayerTest005)
{
    // create surface node 1
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);
    // create surface node 2 (skip layer)
    Vector4f surfaceRect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestMirrorScreen_02", surfaceRect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetSkipLayer(true);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create main display
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);

    // add child
    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode2);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create mirror display (non security display)
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, false);
    ASSERT_NE(displayNode2, nullptr);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetSkipLayerTest006
 * @tc.desc: SetSkipLayer(true) on sibling surfaceNode in security mirror, verify sibling skip layer under security
 * display
 * @tc.type: FUNC
 * @tc.require: issue23278
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetSkipLayerTest006)
{
    // create surface node 1
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode1 = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorGREEN);
    ASSERT_NE(surfaceNode1, nullptr);
    // create surface node 2 (skip layer)
    Vector4f surfaceRect2(DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode2 = CreateSurfaceNodeWithConfig("TestMirrorScreen_02", surfaceRect2, SK_ColorBLUE);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetSkipLayer(true);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create main display
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);

    // add child
    displayNode1->RSNode::AddChild(surfaceNode1);
    displayNode1->RSNode::AddChild(surfaceNode2);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create mirror display (security display)
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, true);
    ASSERT_NE(displayNode2, nullptr);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetSnapshotSkipLayerTest001
 * @tc.desc: SetSnapshotSkipLayer(true) on surfaceNode in non-security mirror, verify snapshot-specific skip layer
 * excludes node from capture
 * @tc.type: FUNC
 * @tc.require: issue23278
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetSnapshotSkipLayerTest001)
{
    // create surface node (snapshot skip layer)
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorRED);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSnapshotSkipLayer(true);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create main display
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);
    // add child
    displayNode1->RSNode::AddChild(surfaceNode);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create mirror display (non security display)
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, false);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: SetSnapshotSkipLayerTest002
 * @tc.desc: SetSnapshotSkipLayer(true) on surfaceNode in security mirror, verify snapshot skip layer behavior under
 * security display
 * @tc.type: FUNC
 * @tc.require: issue23278
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, SetSnapshotSkipLayerTest002)
{
    // create surface node (snapshot skip layer)
    Vector4f surfaceRect1(0, 0, DEFAULT_BOUND_WIDTH, DEFAULT_BOUND_HEIGHT);
    auto surfaceNode = CreateSurfaceNodeWithConfig("TestMirrorScreen_01", surfaceRect1, SK_ColorRED);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSnapshotSkipLayer(true);

    // create main screen
    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId1, INVALID_SCREEN_ID);
    // create main display
    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, DisplayMode::EXPAND, 0, true };
    Vector4f displayRect(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    auto displayNode1 = CreateDisplayNodeWithConfig(displayNodeConfig1, displayRect, SK_ColorYELLOW, true);
    ASSERT_NE(displayNode1, nullptr);
    // add child
    displayNode1->RSNode::AddChild(surfaceNode);

    // create mirror screen
    ScreenId screenId2 = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(screenId2, INVALID_SCREEN_ID);
    auto [csurface, psurface, _] = CreateSurfaceWithListener<CustomizedBufferConsumerListener>();
    ASSERT_NE(csurface, nullptr);
    ASSERT_NE(psurface, nullptr);
    RSInterfaces::GetInstance().SetVirtualScreenSurface(screenId2, psurface);

    // create mirror display (security display)
    RSDisplayNodeConfig displayNodeConfig2 = { screenId2, DisplayMode::MIRROR, displayNode1->GetId(), true };
    auto displayNode2 = CreateDisplayNodeWithConfig(displayNodeConfig2, displayRect, SK_ColorRED, true);

    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId2);
}

/*
 * @tc.name: MultiSurfaceAddRemoveTest001
 * @tc.desc: AddVirtualScreenSurface then RemoveVirtualScreenSurface on a virtual screen. Uses
 * MultiFrameBufferConsumerListener to capture 3 frames from the primary surface: frame0 = red (before adding
 * secondary), frame1 = blue (after adding secondary, primary still renders correctly), frame2 = green (after
 * removing secondary, primary still renders correctly). Stitched PNG: [red | blue | green] verifies Add/Remove
 * doesn't break primary surface rendering.
 * @tc.type: FUNC
 * @tc.require: issueMultiSurfaceAddRemove
 */
GRAPHIC_N_TEST(RSMultiScreenTest, CONTENT_DISPLAY_TEST, MultiSurfaceAddRemoveTest001)
{
    constexpr uint32_t screenWidth = 640;
    constexpr uint32_t screenHeight = 640;

    auto [csurface1, psurface1, listener] = CreateSurfaceWithListener<MultiFrameBufferConsumerListener>();
    ASSERT_NE(csurface1, nullptr);

    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface2, nullptr);
    csurface2->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer2 = csurface2->GetProducer();
    auto psurface2 = Surface::CreateSurfaceAsProducer(producer2);

    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        GetTestName(), screenWidth, screenHeight, psurface1, INVALID_SCREEN_ID, 0, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig {
        .screenId = screenId, .displayMode = DisplayMode::EXPAND, .mirrorNodeId = 0, .isSync = true
    };
    Vector4f displayRect(0, 0, screenWidth, screenHeight);
    auto displayNode = CreateDisplayNodeWithConfig(displayNodeConfig, displayRect, SK_ColorBLACK);
    ASSERT_NE(displayNode, nullptr);

    auto surfaceNode = CreateSurfaceNodeWithConfig("MS_Red", { 0, 0, screenWidth, screenHeight }, SK_ColorRED);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->RSNode::AddChild(surfaceNode);

    SendMessagesAndSleep();

    std::vector<SurfaceRegionConfig> surfaceConfigs;
    surfaceConfigs.push_back({ psurface2, RectI(0, 0, screenWidth, screenHeight) });
    RSInterfaces::GetInstance().AddVirtualScreenSurface(screenId, surfaceConfigs);

    surfaceNode->SetBackgroundColor(SK_ColorBLUE);
    SendMessagesAndSleep();

    RSInterfaces::GetInstance().RemoveVirtualScreenSurface(screenId, { psurface2 });

    surfaceNode->SetBackgroundColor(SK_ColorGREEN);
    SendMessagesAndSleep();

    listener->SaveStitchedFrames();

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}
} // namespace OHOS::Rosen
