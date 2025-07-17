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

class RSLogicalDisplayNodeTest : public RSGraphicTest {
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
};

/*
 * @tc.name: LOGICAL_DISPLAY_NODE_TEST_001
 * @tc.desc: attach two display nodes to screen node, capture displayNode0
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSLogicalDisplayNodeTest, CONTENT_DISPLAY_TEST, LOGICAL_DISPLAY_NODE_TEST_001)
{
    uint32_t width = 2048;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode0 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode0, nullptr);
    ASSERT_NE(displayNode1, nullptr);
    displayNode0->SetBounds({ 0, 0, 500, 500 });
    displayNode0->SetFrame({ 0, 0, 500, 500 });
    displayNode0->SetBackgroundColor(SK_ColorBLUE);
    displayNode1->SetBounds({ 1500, 0, 500, 500 });
    displayNode1->SetFrame({ 1500, 0, 500, 500 });
    displayNode1->SetBackgroundColor(SK_ColorRED);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode0, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: LOGICAL_DISPLAY_NODE_TEST_002
 * @tc.desc: attach two display nodes to screen node, capture displayNode1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSLogicalDisplayNodeTest, CONTENT_DISPLAY_TEST, LOGICAL_DISPLAY_NODE_TEST_002)
{
    uint32_t width = 2048;
    uint32_t height = 1000;
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode0 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode0, nullptr);
    ASSERT_NE(displayNode1, nullptr);
    displayNode0->SetBounds({ 0, 0, 500, 500 });
    displayNode0->SetFrame({ 0, 0, 500, 500 });
    displayNode0->SetBackgroundColor(SK_ColorBLUE);
    displayNode1->SetBounds({ 1500, 0, 500, 500 });
    displayNode1->SetFrame({ 1500, 0, 500, 500 });
    displayNode1->SetBackgroundColor(SK_ColorRED);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode1, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: LOGICAL_DISPLAY_NODE_TEST_003
 * @tc.desc: attach two display nodes to screen node, check output buffer
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSLogicalDisplayNodeTest, CONTENT_DISPLAY_TEST, LOGICAL_DISPLAY_NODE_TEST_003)
{
    uint32_t width = 2048;
    uint32_t height = 1000;
    auto csurface = Surface::CreateSurfaceAsConsumer();
    csurface->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface, psurface);
    csurface->RegisterConsumerListener(listener);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_003", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode0 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode0, nullptr);
    ASSERT_NE(displayNode1, nullptr);
    displayNode0->SetBounds({ 0, 0, 500, 500 });
    displayNode0->SetFrame({ 0, 0, 500, 500 });
    displayNode0->SetBackgroundColor(SK_ColorBLUE);
    displayNode1->SetBounds({ 1500, 0, 500, 500 });
    displayNode1->SetFrame({ 1500, 0, 500, 500 });
    displayNode1->SetBackgroundColor(SK_ColorRED);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: LOGICAL_DISPLAY_NODE_TEST_004
 * @tc.desc: attach two display nodes to screen node, mirror displayNode1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSLogicalDisplayNodeTest, CONTENT_DISPLAY_TEST, LOGICAL_DISPLAY_NODE_TEST_004)
{
    uint32_t width = 2048;
    uint32_t height = 1000;
    auto csurface0 = Surface::CreateSurfaceAsConsumer();
    csurface0->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer0 = csurface0->GetProducer();
    auto psurface0 = Surface::CreateSurfaceAsProducer(producer0);
    ScreenId screenId0 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_004", width, height, psurface0, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId0, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig0 = { screenId0, false, 0, true };
    auto displayNode0 = RSDisplayNode::Create(displayNodeConfig0);
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig0);
    ASSERT_NE(displayNode0, nullptr);
    ASSERT_NE(displayNode1, nullptr);
    displayNode0->SetBounds({ 0, 0, 500, 500 });
    displayNode0->SetFrame({ 0, 0, 500, 500 });
    displayNode0->SetBackgroundColor(SK_ColorBLUE);
    displayNode1->SetBounds({ 1500, 0, 500, 500 });
    displayNode1->SetFrame({ 1500, 0, 500, 500 });
    displayNode1->SetBackgroundColor(SK_ColorRED);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface1 = Surface::CreateSurfaceAsConsumer();
    csurface1->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface1, psurface1);
    csurface1->RegisterConsumerListener(listener);

    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_004_2", width, height, psurface1, screenId0, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig1 = { screenId1, true, displayNode1->GetId(), true };
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig1);
    ASSERT_NE(displayNode2, nullptr);
    displayNode2->SetBounds({ 0, 0, width, height });
    displayNode2->SetFrame({ 0, 0, width, height });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId0);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
}

/*
 * @tc.name: LOGICAL_DISPLAY_NODE_TEST_005
 * @tc.desc: attach four display nodes to screen node, mirror displayNode2
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSLogicalDisplayNodeTest, CONTENT_DISPLAY_TEST, LOGICAL_DISPLAY_NODE_TEST_005)
{
    uint32_t width = 2048;
    uint32_t height = 1000;
    auto csurface0 = IConsumerSurface::Create();
    auto producer0 = csurface0->GetProducer();
    auto psurface0 = Surface::CreateSurfaceAsProducer(producer0);
    ScreenId screenId0 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_005", width, height, psurface0, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId0, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId0, false, 0, true };
    auto displayNode0 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode3 = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode0, nullptr);
    ASSERT_NE(displayNode1, nullptr);
    ASSERT_NE(displayNode2, nullptr);
    ASSERT_NE(displayNode3, nullptr);
    displayNode0->SetBounds({ 0, 0, 500, 500 });
    displayNode0->SetFrame({ 0, 0, 500, 500 });
    displayNode0->SetBackgroundColor(SK_ColorBLUE);
    displayNode1->SetBounds({ 100, 0, 500, 500 });
    displayNode1->SetFrame({ 100, 0, 500, 500 });
    displayNode1->SetBackgroundColor(SK_ColorRED);
    displayNode2->SetBounds({ 200, 0, 500, 500 });
    displayNode2->SetFrame({ 200, 0, 500, 500 });
    displayNode2->SetBackgroundColor(SK_ColorYELLOW);
    displayNode3->SetBounds({ 300, 0, 500, 500 });
    displayNode3->SetFrame({ 300, 0, 500, 500 });
    displayNode3->SetBackgroundColor(SK_ColorGREEN);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // mirrorScreen
    auto csurface1 = Surface::CreateSurfaceAsConsumer();
    csurface1->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer1 = csurface1->GetProducer();
    auto psurface1 = Surface::CreateSurfaceAsProducer(producer1);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface1, psurface1);
    csurface1->RegisterConsumerListener(listener);

    ScreenId screenId1 = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_005_2", width, height, psurface1, screenId0, -1, {});
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);

    RSDisplayNodeConfig mirrorDisplayNodeConfig = { screenId1, true, displayNode1->GetId(), true };
    auto mirrorDisplayNode = RSDisplayNode::Create(mirrorDisplayNodeConfig);
    ASSERT_NE(mirrorDisplayNode, nullptr);
    mirrorDisplayNode->SetBounds({ 0, 0, width, height });
    mirrorDisplayNode->SetFrame({ 0, 0, width, height });
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId0);
    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId1);
}

/*
 * @tc.name: LOGICAL_DISPLAY_NODE_TEST_006
 * @tc.desc: attach four display nodes to screen node, check output buffer (z-order)
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSLogicalDisplayNodeTest, CONTENT_DISPLAY_TEST, LOGICAL_DISPLAY_NODE_TEST_006)
{
    uint32_t width = 2048;
    uint32_t height = 1000;
    auto csurface = Surface::CreateSurfaceAsConsumer();
    csurface->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface, psurface);
    csurface->RegisterConsumerListener(listener);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_006", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode0 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode3 = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode0, nullptr);
    ASSERT_NE(displayNode1, nullptr);
    ASSERT_NE(displayNode2, nullptr);
    ASSERT_NE(displayNode3, nullptr);
    displayNode0->SetBounds({ 0, 0, 500, 500 });
    displayNode0->SetFrame({ 0, 0, 500, 500 });
    displayNode0->SetBackgroundColor(SK_ColorBLUE);
    displayNode1->SetBounds({ 100, 0, 500, 500 });
    displayNode1->SetFrame({ 100, 0, 500, 500 });
    displayNode1->SetBackgroundColor(SK_ColorRED);
    displayNode2->SetBounds({ 200, 0, 500, 500 });
    displayNode2->SetFrame({ 200, 0, 500, 500 });
    displayNode2->SetBackgroundColor(SK_ColorYELLOW);
    displayNode3->SetBounds({ 300, 0, 500, 500 });
    displayNode3->SetFrame({ 300, 0, 500, 500 });
    displayNode3->SetBackgroundColor(SK_ColorGREEN);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: LOGICAL_DISPLAY_NODE_TEST_007
 * @tc.desc: attach four display nodes to screen node, remove displayNode3 from rs tree
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSLogicalDisplayNodeTest, CONTENT_DISPLAY_TEST, LOGICAL_DISPLAY_NODE_TEST_007)
{
    uint32_t width = 2048;
    uint32_t height = 1000;
    auto csurface = Surface::CreateSurfaceAsConsumer();
    csurface->SetDefaultUsage(
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    sptr<IBufferConsumerListener> listener = sptr<CustomizedBufferConsumerListener>::MakeSptr(csurface, psurface);
    csurface->RegisterConsumerListener(listener);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_007", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode0 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode3 = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode0, nullptr);
    ASSERT_NE(displayNode1, nullptr);
    ASSERT_NE(displayNode2, nullptr);
    ASSERT_NE(displayNode3, nullptr);
    displayNode0->SetBounds({ 0, 0, 500, 500 });
    displayNode0->SetFrame({ 0, 0, 500, 500 });
    displayNode0->SetBackgroundColor(SK_ColorBLUE);
    displayNode1->SetBounds({ 100, 0, 500, 500 });
    displayNode1->SetFrame({ 100, 0, 500, 500 });
    displayNode1->SetBackgroundColor(SK_ColorRED);
    displayNode2->SetBounds({ 200, 0, 500, 500 });
    displayNode2->SetFrame({ 200, 0, 500, 500 });
    displayNode2->SetBackgroundColor(SK_ColorYELLOW);
    displayNode3->SetBounds({ 300, 0, 500, 500 });
    displayNode3->SetFrame({ 300, 0, 500, 500 });
    displayNode3->SetBackgroundColor(SK_ColorGREEN);
    displayNode3->RemoveDisplayNodeFromTree();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: LOGICAL_DISPLAY_NODE_TEST_008
 * @tc.desc: attach four display nodes to screen node, capture displayNode1
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSLogicalDisplayNodeTest, CONTENT_DISPLAY_TEST, LOGICAL_DISPLAY_NODE_TEST_008)
{
    uint32_t width = 2048;
    uint32_t height = 1000;
    auto csurface = Surface::CreateSurfaceAsConsumer();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId screenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "LOGICAL_DISPLAY_NODE_TEST_008", width, height, psurface, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSDisplayNodeConfig displayNodeConfig = { screenId, false, 0, true };
    auto displayNode0 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode1 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode2 = RSDisplayNode::Create(displayNodeConfig);
    auto displayNode3 = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode0, nullptr);
    ASSERT_NE(displayNode1, nullptr);
    ASSERT_NE(displayNode2, nullptr);
    ASSERT_NE(displayNode3, nullptr);
    displayNode0->SetBounds({ 0, 0, 500, 500 });
    displayNode0->SetFrame({ 0, 0, 500, 500 });
    displayNode0->SetBackgroundColor(SK_ColorBLUE);
    displayNode1->SetBounds({ 100, 0, 500, 500 });
    displayNode1->SetFrame({ 100, 0, 500, 500 });
    displayNode1->SetBackgroundColor(SK_ColorRED);
    displayNode2->SetBounds({ 200, 0, 500, 500 });
    displayNode2->SetFrame({ 200, 0, 500, 500 });
    displayNode2->SetBackgroundColor(SK_ColorYELLOW);
    displayNode3->SetBounds({ 300, 0, 500, 500 });
    displayNode3->SetFrame({ 300, 0, 500, 500 });
    displayNode3->SetBackgroundColor(SK_ColorGREEN);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode1, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(screenId);
}

} // namespace OHOS::Rosen
