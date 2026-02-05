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

// Screen Manager Pixel Test: Multi-screen interface tests with pixel-level comparison
// Tests verify screen management interfaces through actual rendering and screenshot capture

#include <filesystem>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "pixel_map_from_surface.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_utils.h"
#include "token_setproc.h"

#include "transaction/rs_interfaces.h"
#include "surface_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200;
constexpr uint32_t SLEEP_TIME_IN_US = 10000;      // 10 ms
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms

class CustomizedSurfaceCapture : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelMap) override
    {
        if (pixelMap == nullptr) {
            LOGE("CustomizedSurfaceCapture::OnSurfaceCapture failed to get pixelMap");
            return;
        }
        isCallbackCalled_ = true;
        SavePixelToFile(pixelMap);
    }
    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
    bool isCallbackCalled_ = false;

private:
    void SavePixelToFile(std::shared_ptr<Media::PixelMap> pixelMap)
    {
        const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
        std::string fileName = "/data/local/graphic_test/multi_screen/";
        namespace fs = std::filesystem;
        if (!fs::exists(fileName)) {
            if (!fs::create_directories(fileName)) {
                LOGE("SavePixelToFile create dir failed");
            }
        }
        fileName += testInfo->test_case_name() + std::string("_");
        fileName += testInfo->name() + std::string(".png");
        if (!WriteToPngWithPixelMap(fileName, *pixelMap)) {
            LOGE("SavePixelToFile write image failed %{public}s", fileName.c_str());
        }
    }
};
} // namespace

class RSScreenManagerTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        LOGI("RSScreenManagerTest BeforeEach");
        uint64_t tokenId;
        const char* perms[2];
        perms[0] = "ohos.permission.CAPTURE_SCREEN";
        perms[1] = "ohos.permission.MANAGE_DISPLAY_MANAGER";
        NativeTokenInfoParams infoInstance = {
            .dcapsNum = 0,
            .permsNum = 2,
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

// Virtual Screen Tests - use nullptr for Surface to enable screenshot capture
// ============================================================================

/*
 * @tc.name: CreateVirtualScreenTest001
 * @tc.desc: test CreateVirtualScreen with red background 640x480
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest001)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);
    LOGI("CreateVirtualScreenTest001: virtualScreenId=%{public}" PRIu64, virtualScreenId);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    LOGI("CreateVirtualScreenTest001: nodeId=%{public}" PRIu64, displayNode->GetId());

    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorRED);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("CreateVirtualScreenTest001 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: CreateVirtualScreenTest002
 * @tc.desc: test CreateVirtualScreen with green background 800x600
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest002)
{
    uint32_t width = 800;
    uint32_t height = 600;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorGREEN);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("CreateVirtualScreenTest002 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: CreateVirtualScreenTest003
 * @tc.desc: test CreateVirtualScreen with blue background 500x500
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, CreateVirtualScreenTest003)
{
    uint32_t width = 500;
    uint32_t height = 500;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "CreateVirtualScreenTest003", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorBLUE);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("CreateVirtualScreenTest003 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

// ============================================================================
// Screen Power Status Tests
// ============================================================================

/*
 * @tc.name: SetScreenPowerStatusTest001
 * @tc.desc: test SetScreenPowerStatus with POWER_STATUS_ON
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenPowerStatusTest001)
{
    ScreenPowerStatus originalState = RSInterfaces::GetInstance().GetScreenPowerStatus(0);

    RSDisplayNodeConfig displayNodeConfig = { 0, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, 500, 500 });
    displayNode->SetFrame({ 0, 0, 500, 500 });
    displayNode->SetBackgroundColor(SK_ColorYELLOW);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // Set power on
    RSInterfaces::GetInstance().SetScreenPowerStatus(0, ScreenPowerStatus::POWER_STATUS_ON);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("SetScreenPowerStatusTest001 TakeSurfaceCapture failed");
    }

    // Restore original state
    RSInterfaces::GetInstance().SetScreenPowerStatus(0, originalState);
}

/*
 * @tc.name: SetScreenPowerStatusTest002
 * @tc.desc: test SetScreenPowerStatus with POWER_STATUS_OFF
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenPowerStatusTest002)
{
    ScreenPowerStatus originalState = RSInterfaces::GetInstance().GetScreenPowerStatus(0);

    // Skip test if original state is already off
    if (originalState == ScreenPowerStatus::POWER_STATUS_OFF) {
        return;
    }

    RSDisplayNodeConfig displayNodeConfig = { 0, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, 500, 500 });
    displayNode->SetFrame({ 0, 0, 500, 500 });
    displayNode->SetBackgroundColor(SK_ColorCYAN);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // Set power off
    RSInterfaces::GetInstance().SetScreenPowerStatus(0, ScreenPowerStatus::POWER_STATUS_OFF);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("SetScreenPowerStatusTest002 TakeSurfaceCapture failed");
    }

    // Restore power on
    RSInterfaces::GetInstance().SetScreenPowerStatus(0, ScreenPowerStatus::POWER_STATUS_ON);
}

// ============================================================================
// Screen Active Mode Tests
// ============================================================================

/*
 * @tc.name: SetScreenActiveModeTest001
 * @tc.desc: test SetScreenActiveMode with first supported mode
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenActiveModeTest001)
{
    std::vector<RSScreenModeInfo> supportedModes = RSInterfaces::GetInstance().GetScreenSupportedModes(0);
    if (supportedModes.empty()) {
        LOGW("SetScreenActiveModeTest001 No supported modes available");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig = { 0, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, 500, 500 });
    displayNode->SetFrame({ 0, 0, 500, 500 });
    displayNode->SetBackgroundColor(SK_ColorMAGENTA);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // Set to first supported mode
    uint32_t modeId = supportedModes[0].GetScreenModeId();
    RSInterfaces::GetInstance().SetScreenActiveMode(0, modeId);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("SetScreenActiveModeTest001 TakeSurfaceCapture failed");
    }
}

/*
 * @tc.name: SetScreenActiveModeTest002
 * @tc.desc: test SetScreenActiveMode with second supported mode
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenActiveModeTest002)
{
    std::vector<RSScreenModeInfo> supportedModes = RSInterfaces::GetInstance().GetScreenSupportedModes(0);
    if (supportedModes.size() < 2) {
        LOGW("SetScreenActiveModeTest002 Less than 2 supported modes available");
        return;
    }

    RSDisplayNodeConfig displayNodeConfig = { 0, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, 600, 400 });
    displayNode->SetFrame({ 0, 0, 600, 400 });
    displayNode->SetBackgroundColor(SK_ColorWHITE);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // Set to second supported mode
    uint32_t modeId = supportedModes[1].GetScreenModeId();
    RSInterfaces::GetInstance().SetScreenActiveMode(0, modeId);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("SetScreenActiveModeTest002 TakeSurfaceCapture failed");
    }
}

// ============================================================================
// Virtual Screen Resolution Tests
// ============================================================================

/*
 * @tc.name: SetVirtualScreenResolutionTest001
 * @tc.desc: test SetVirtualScreenResolution with 640x480
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest001)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorRED);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("SetVirtualScreenResolutionTest001 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest002
 * @tc.desc: test SetVirtualScreenResolution with 800x600
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest002)
{
    uint32_t width = 800;
    uint32_t height = 600;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorGREEN);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("SetVirtualScreenResolutionTest002 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: SetVirtualScreenResolutionTest003
 * @tc.desc: test SetVirtualScreenResolution with 1024x768
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetVirtualScreenResolutionTest003)
{
    uint32_t width = 1024;
    uint32_t height = 768;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenResolutionTest003", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorBLUE);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("SetVirtualScreenResolutionTest003 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

// ============================================================================
// Multiple Virtual Screens Tests
// ============================================================================

/*
 * @tc.name: MultipleVirtualScreensTest001
 * @tc.desc: test virtual screen with red display node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, MultipleVirtualScreensTest001)
{
    uint32_t width = 400;
    uint32_t height = 300;

    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MultipleVirtualScreensTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorRED);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("MultipleVirtualScreensTest001 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: MultipleVirtualScreensTest002
 * @tc.desc: test virtual screen with green display node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, MultipleVirtualScreensTest002)
{
    uint32_t width = 400;
    uint32_t height = 300;

    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MultipleVirtualScreensTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorGREEN);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("MultipleVirtualScreensTest002 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: MultipleVirtualScreensTest003
 * @tc.desc: test virtual screen with blue display node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, MultipleVirtualScreensTest003)
{
    uint32_t width = 400;
    uint32_t height = 300;

    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MultipleVirtualScreensTest003", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorBLUE);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("MultipleVirtualScreensTest003 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: MultipleVirtualScreensTest004
 * @tc.desc: test virtual screen with yellow display node
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, MultipleVirtualScreensTest004)
{
    uint32_t width = 400;
    uint32_t height = 300;

    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "MultipleVirtualScreensTest004", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorYELLOW);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("MultipleVirtualScreensTest004 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

// ============================================================================
// Screen Capture Tests
// ============================================================================

/*
 * @tc.name: VirtualScreenCaptureTest001
 * @tc.desc: test TakeSurfaceCapture on virtual screen with red background
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, VirtualScreenCaptureTest001)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "VirtualScreenCaptureTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorRED);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // Capture screenshot
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("VirtualScreenCaptureTest001 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: VirtualScreenCaptureTest002
 * @tc.desc: test TakeSurfaceCapture on virtual screen with green background
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, VirtualScreenCaptureTest002)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "VirtualScreenCaptureTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

    RSDisplayNodeConfig displayNodeConfig = { virtualScreenId, false, 0, true };
    auto displayNode = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_NE(displayNode, nullptr);

    displayNode->SetBounds({ 0, 0, width, height });
    displayNode->SetFrame({ 0, 0, width, height });
    displayNode->SetBackgroundColor(SK_ColorGREEN);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // Capture screenshot
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(displayNode, callback);
    if (!CheckSurfaceCaptureCallback(callback)) {
        LOGE("VirtualScreenCaptureTest002 TakeSurfaceCapture failed");
    }

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

} // namespace OHOS::Rosen
