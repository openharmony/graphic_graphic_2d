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

// Screen Manager Test: Test cases for screen management interfaces including
// physical screen get/set interfaces, virtual screen interfaces, and mirror screen interfaces.

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

// ============================================================================
// 物理屏幕 - 获取类接口测试
// ============================================================================

/*
 * @tc.name: GetActiveScreenIdTest001
 * @tc.desc: test GetActiveScreenId to get current active screen ID
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetActiveScreenIdTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    EXPECT_NE(activeScreenId, INVALID_SCREEN_ID);
    LOGI("GetActiveScreenIdTest001 activeScreenId[%{public}" PRIu64 "]", activeScreenId);
}

/*
 * @tc.name: GetAllScreenIdsTest001
 * @tc.desc: test GetAllScreenIds to get all screen IDs
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetAllScreenIdsTest001)
{
    std::vector<ScreenId> screenIds = RSInterfaces::GetInstance().GetAllScreenIds();
    EXPECT_GT(screenIds.size(), 0u);
    LOGI("GetAllScreenIdsTest001 screenIds.size()[%{public}zu]", screenIds.size());
    for (const auto& screenId : screenIds) {
        LOGI("GetAllScreenIdsTest001 screenId[%{public}" PRIu64 "]", screenId);
    }
}

/*
 * @tc.name: GetAllScreenIdsTest002
 * @tc.desc: test GetAllScreenIds after creating virtual screen
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetAllScreenIdsTest002)
{
    std::vector<ScreenId> screenIdsBefore = RSInterfaces::GetInstance().GetAllScreenIds();
    size_t countBefore = screenIdsBefore.size();

    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "GetAllScreenIdsTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<ScreenId> screenIdsAfter = RSInterfaces::GetInstance().GetAllScreenIds();
    EXPECT_EQ(screenIdsAfter.size(), countBefore + 1);

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: GetDefaultScreenIdTest001
 * @tc.desc: test GetDefaultScreenId to get default screen ID
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetDefaultScreenIdTest001)
{
    ScreenId defaultScreenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    EXPECT_NE(defaultScreenId, INVALID_SCREEN_ID);
    LOGI("GetDefaultScreenIdTest001 defaultScreenId[%{public}" PRIu64 "]", defaultScreenId);
}

/*
 * @tc.name: GetScreenTypeTest001
 * @tc.desc: test GetScreenType to get screen type
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetScreenTypeTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    ScreenType screenType = RSInterfaces::GetInstance().GetScreenType(activeScreenId);
    EXPECT_NE(screenType, ScreenType::UNKNOWN);
    LOGI("GetScreenTypeTest001 screenId[%{public}" PRIu64 "] screenType[%{public}d]",
        activeScreenId, static_cast<int>(screenType));
}

/*
 * @tc.name: GetScreenTypeTest002
 * @tc.desc: test GetScreenType for virtual screen
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetScreenTypeTest002)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "GetScreenTypeTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    ScreenType screenType = RSInterfaces::GetInstance().GetScreenType(virtualScreenId);
    EXPECT_EQ(screenType, ScreenType::VIRTUAL);

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: GetScreenPowerStatusTest001
 * @tc.desc: test GetScreenPowerStatus to get screen power status
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetScreenPowerStatusTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    ScreenPowerState powerState = RSInterfaces::GetInstance().GetScreenPowerStatus(activeScreenId);
    EXPECT_NE(powerState, ScreenPowerState::POWER_STATUS_UNKNOWN);
    LOGI("GetScreenPowerStatusTest001 screenId[%{public}" PRIu64 "] powerState[%{public}d]",
        activeScreenId, static_cast<int>(powerState));
}

/*
 * @tc.name: GetScreenDataTest001
 * @tc.desc: test GetScreenData to get screen data
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetScreenDataTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    sptr<ScreenData> screenData = RSInterfaces::GetInstance().GetScreenData(activeScreenId);
    EXPECT_NE(screenData, nullptr);
    if (screenData != nullptr) {
        LOGI("GetScreenDataTest001 screenId[%{public}" PRIu64 "] width[%{public}d] height[%{public}d]",
            activeScreenId, screenData->GetWidth(), screenData->GetHeight());
    }
}

/*
 * @tc.name: GetScreenSupportedModesTest001
 * @tc.desc: test GetScreenSupportedModes to get supported screen modes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetScreenSupportedModesTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    std::vector<ScreenModeInfo> supportedModes = RSInterfaces::GetInstance().GetScreenSupportedModes(activeScreenId);
    EXPECT_GT(supportedModes.size(), 0u);
    LOGI("GetScreenSupportedModesTest001 screenId[%{public}" PRIu64 "] supportedModes.size()[%{public}zu]",
        activeScreenId, supportedModes.size());
}

/*
 * @tc.name: GetScreenActiveModeTest001
 * @tc.desc: test GetScreenActiveMode to get current active screen mode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetScreenActiveModeTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    sptr<ScreenModeInfo> activeMode = RSInterfaces::GetInstance().GetScreenActiveMode(activeScreenId);
    EXPECT_NE(activeMode, nullptr);
    if (activeMode != nullptr) {
        LOGI("GetScreenActiveModeTest001 screenId[%{public}" PRIu64 "] width[%{public}d] height[%{public}d] refreshRate[%{public}d]",
            activeScreenId, activeMode->GetWidth(), activeMode->GetHeight(), activeMode->GetRefreshRate());
    }
}

/*
 * @tc.name: GetScreenCapabilityTest001
 * @tc.desc: test GetScreenCapability to get screen capability
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetScreenCapabilityTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    sptr<ScreenCapability> screenCapability = RSInterfaces::GetInstance().GetScreenCapability(activeScreenId);
    EXPECT_NE(screenCapability, nullptr);
    if (screenCapability != nullptr) {
        LOGI("GetScreenCapabilityTest001 screenId[%{public}" PRIu64 "] name[%{public}s]",
            activeScreenId, screenCapability->GetName().c_str());
    }
}

/*
 * @tc.name: GetRogScreenResolutionTest001
 * @tc.desc: test GetRogScreenResolution to get ROG screen resolution
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, GetRogScreenResolutionTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    sptr<ScreenResolution> resolution = RSInterfaces::GetInstance().GetRogScreenResolution(activeScreenId);
    if (resolution != nullptr) {
        LOGI("GetRogScreenResolutionTest001 screenId[%{public}" PRIu64 "] width[%{public}d] height[%{public}d]",
            activeScreenId, resolution->GetWidth(), resolution->GetHeight());
    }
}

// ============================================================================
// 物理屏幕 - 设置类接口测试
// ============================================================================

/*
 * @tc.name: SetScreenPowerStatusTest001
 * @tc.desc: test SetScreenPowerStatus to set screen power on
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenPowerStatusTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    ScreenPowerState originalState = RSInterfaces::GetInstance().GetScreenPowerStatus(activeScreenId);

    bool result = RSInterfaces::GetInstance().SetScreenPowerStatus(activeScreenId, ScreenPowerState::POWER_STATUS_ON);
    EXPECT_TRUE(result);

    usleep(SLEEP_TIME_FOR_PROXY);

    ScreenPowerState newState = RSInterfaces::GetInstance().GetScreenPowerStatus(activeScreenId);
    EXPECT_EQ(newState, ScreenPowerState::POWER_STATUS_ON);

    // Restore original state
    RSInterfaces::GetInstance().SetScreenPowerStatus(activeScreenId, originalState);
}

/*
 * @tc.name: SetScreenPowerStatusTest002
 * @tc.desc: test SetScreenPowerStatus to set screen power off
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenPowerStatusTest002)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    ScreenPowerState originalState = RSInterfaces::GetInstance().GetScreenPowerStatus(activeScreenId);

    // Skip test if original state is already off
    if (originalState == ScreenPowerState::POWER_STATUS_OFF) {
        return;
    }

    bool result = RSInterfaces::GetInstance().SetScreenPowerStatus(activeScreenId, ScreenPowerState::POWER_STATUS_OFF);
    EXPECT_TRUE(result);

    usleep(SLEEP_TIME_FOR_PROXY);

    ScreenPowerState newState = RSInterfaces::GetInstance().GetScreenPowerStatus(activeScreenId);
    EXPECT_EQ(newState, ScreenPowerState::POWER_STATUS_OFF);

    // Restore power on
    RSInterfaces::GetInstance().SetScreenPowerStatus(activeScreenId, ScreenPowerState::POWER_STATUS_ON);
}

/*
 * @tc.name: SetScreenBacklightTest001
 * @tc.desc: test SetScreenBacklight to set screen backlight brightness
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenBacklightTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    uint32_t brightness = 128; // 50% brightness

    bool result = RSInterfaces::GetInstance().SetScreenBacklight(activeScreenId, brightness);
    EXPECT_TRUE(result);

    LOGI("SetScreenBacklightTest001 screenId[%{public}" PRIu64 "] brightness[%{public}u]",
        activeScreenId, brightness);
}

/*
 * @tc.name: SetScreenActiveModeTest001
 * @tc.desc: test SetScreenActiveMode to set screen active mode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenActiveModeTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();

    // Get supported modes first
    std::vector<ScreenModeInfo> supportedModes = RSInterfaces::GetInstance().GetScreenSupportedModes(activeScreenId);
    if (supportedModes.empty()) {
        LOGW("SetScreenActiveModeTest001 No supported modes available");
        return;
    }

    // Set to first supported mode
    uint32_t modeId = supportedModes[0].GetId();
    bool result = RSInterfaces::GetInstance().SetScreenActiveMode(activeScreenId, modeId);
    EXPECT_TRUE(result);

    usleep(SLEEP_TIME_FOR_PROXY);

    LOGI("SetScreenActiveModeTest001 screenId[%{public}" PRIu64 "] modeId[%{public}u]",
        activeScreenId, modeId);
}

/*
 * @tc.name: SetScreenActiveModeTest002
 * @tc.desc: test SetScreenActiveMode with different mode index
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenActiveModeTest002)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();

    std::vector<ScreenModeInfo> supportedModes = RSInterfaces::GetInstance().GetScreenSupportedModes(activeScreenId);
    if (supportedModes.size() < 2) {
        LOGW("SetScreenActiveModeTest002 Less than 2 supported modes available");
        return;
    }

    // Set to second supported mode
    uint32_t modeId = supportedModes[1].GetId();
    bool result = RSInterfaces::GetInstance().SetScreenActiveMode(activeScreenId, modeId);
    EXPECT_TRUE(result);

    usleep(SLEEP_TIME_FOR_PROXY);

    LOGI("SetScreenActiveModeTest002 screenId[%{public}" PRIu64 "] modeId[%{public}u]",
        activeScreenId, modeId);
}

/*
 * @tc.name: SetPhysicalScreenResolutionTest001
 * @tc.desc: test SetPhysicalScreenResolution to set physical screen resolution
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetPhysicalScreenResolutionTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();

    // Get current resolution
    sptr<ScreenModeInfo> currentMode = RSInterfaces::GetInstance().GetScreenActiveMode(activeScreenId);
    if (currentMode == nullptr) {
        LOGE("SetPhysicalScreenResolutionTest001 Failed to get current mode");
        return;
    }

    uint32_t originalWidth = currentMode->GetWidth();
    uint32_t originalHeight = currentMode->GetHeight();

    // Get supported modes and find a different resolution
    std::vector<ScreenModeInfo> supportedModes = RSInterfaces::GetInstance().GetScreenSupportedModes(activeScreenId);
    uint32_t targetWidth = originalWidth;
    uint32_t targetHeight = originalHeight;

    for (const auto& mode : supportedModes) {
        if (mode.GetWidth() != originalWidth || mode.GetHeight() != originalHeight) {
            targetWidth = mode.GetWidth();
            targetHeight = mode.GetHeight();
            break;
        }
    }

    // Only test if we found a different resolution
    if (targetWidth != originalWidth || targetHeight != originalHeight) {
        bool result = RSInterfaces::GetInstance().SetPhysicalScreenResolution(
            activeScreenId, targetWidth, targetHeight);
        EXPECT_TRUE(result);

        usleep(SLEEP_TIME_FOR_PROXY);

        LOGI("SetPhysicalScreenResolutionTest001 screenId[%{public}" PRIu64 "] %ux%u -> %ux%u",
            activeScreenId, originalWidth, originalHeight, targetWidth, targetHeight);

        // Restore original resolution
        RSInterfaces::GetInstance().SetPhysicalScreenResolution(activeScreenId, originalWidth, originalHeight);
    } else {
        LOGW("SetPhysicalScreenResolutionTest001 No alternative resolution available");
    }
}

/*
 * @tc.name: SetScreenActiveRectTest001
 * @tc.desc: test SetScreenActiveRect to set screen active rectangle
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenActiveRectTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();

    // Get current mode info
    sptr<ScreenModeInfo> currentMode = RSInterfaces::GetInstance().GetScreenActiveMode(activeScreenId);
    if (currentMode == nullptr) {
        LOGE("SetScreenActiveRectTest001 Failed to get current mode");
        return;
    }

    uint32_t screenWidth = currentMode->GetWidth();
    uint32_t screenHeight = currentMode->GetHeight();

    // Set active rect to full screen
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = screenWidth;
    uint32_t height = screenHeight;

    bool result = RSInterfaces::GetInstance().SetScreenActiveRect(
        activeScreenId, { left, top, width, height });
    EXPECT_TRUE(result);

    LOGI("SetScreenActiveRectTest001 screenId[%{public}" PRIu64 "] rect[%{public}u,%{public}u,%{public}u,%{public}u]",
        activeScreenId, left, top, width, height);
}

/*
 * @tc.name: SetScreenActiveRectTest002
 * @tc.desc: test SetScreenActiveRect with partial screen area
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenActiveRectTest002)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();

    sptr<ScreenModeInfo> currentMode = RSInterfaces::GetInstance().GetScreenActiveMode(activeScreenId);
    if (currentMode == nullptr) {
        LOGE("SetScreenActiveRectTest002 Failed to get current mode");
        return;
    }

    uint32_t screenWidth = currentMode->GetWidth();
    uint32_t screenHeight = currentMode->GetHeight();

    // Set active rect to center 80% of screen
    uint32_t left = screenWidth / 10;
    uint32_t top = screenHeight / 10;
    uint32_t width = screenWidth * 8 / 10;
    uint32_t height = screenHeight * 8 / 10;

    bool result = RSInterfaces::GetInstance().SetScreenActiveRect(
        activeScreenId, { left, top, width, height });
    EXPECT_TRUE(result);

    // Reset to full screen
    RSInterfaces::GetInstance().SetScreenActiveRect(activeScreenId, { 0, 0, screenWidth, screenHeight });

    LOGI("SetScreenActiveRectTest002 screenId[%{public}" PRIu64 "] rect[%{public}u,%{public}u,%{public}u,%{public}u]",
        activeScreenId, left, top, width, height);
}

/*
 * @tc.name: SetScreenOffsetTest001
 * @tc.desc: test SetScreenOffset to set screen offset
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenOffsetTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();

    // Set offset to zero
    bool result = RSInterfaces::GetInstance().SetScreenOffset(activeScreenId, 0.0f, 0.0f);
    EXPECT_TRUE(result);

    LOGI("SetScreenOffsetTest001 screenId[%{public}" PRIu64 "] offset[0.0, 0.0]", activeScreenId);
}

/*
 * @tc.name: SetScreenOffsetTest002
 * @tc.desc: test SetScreenOffset with non-zero offset
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenOffsetTest002)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();

    float offsetX = 10.0f;
    float offsetY = 10.0f;

    bool result = RSInterfaces::GetInstance().SetScreenOffset(activeScreenId, offsetX, offsetY);
    EXPECT_TRUE(result);

    // Reset to zero offset
    RSInterfaces::GetInstance().SetScreenOffset(activeScreenId, 0.0f, 0.0f);

    LOGI("SetScreenOffsetTest002 screenId[%{public}" PRIu64 "] offset[%{public}.1f, %{public}.1f]",
        activeScreenId, offsetX, offsetY);
}

/*
 * @tc.name: SetRogScreenResolutionTest001
 * @tc.desc: test SetRogScreenResolution to set ROG screen resolution
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetRogScreenResolutionTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();

    // Get current resolution
    sptr<ScreenModeInfo> currentMode = RSInterfaces::GetInstance().GetScreenActiveMode(activeScreenId);
    if (currentMode == nullptr) {
        LOGE("SetRogScreenResolutionTest001 Failed to get current mode");
        return;
    }

    uint32_t width = currentMode->GetWidth();
    uint32_t height = currentMode->GetHeight();

    bool result = RSInterfaces::GetInstance().SetRogScreenResolution(activeScreenId, width, height);
    EXPECT_TRUE(result);

    LOGI("SetRogScreenResolutionTest001 screenId[%{public}" PRIu64 "] resolution[%{public}u,%{public}u]",
        activeScreenId, width, height);
}

// ============================================================================
// 其他接口测试
// ============================================================================

/*
 * @tc.name: SetScreenSkipFrameIntervalTest001
 * @tc.desc: test SetScreenSkipFrameInterval to set skip frame interval
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenSkipFrameIntervalTest001)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    uint32_t skipFrameInterval = 0; // No skip

    bool result = RSInterfaces::GetInstance().SetScreenSkipFrameInterval(activeScreenId, skipFrameInterval);
    EXPECT_TRUE(result);

    LOGI("SetScreenSkipFrameIntervalTest001 screenId[%{public}" PRIu64 "] skipFrameInterval[%{public}u]",
        activeScreenId, skipFrameInterval);
}

/*
 * @tc.name: SetScreenSkipFrameIntervalTest002
 * @tc.desc: test SetScreenSkipFrameInterval with skip interval
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetScreenSkipFrameIntervalTest002)
{
    ScreenId activeScreenId = RSInterfaces::GetInstance().GetActiveScreenId();
    uint32_t skipFrameInterval = 1; // Skip 1 frame

    bool result = RSInterfaces::GetInstance().SetScreenSkipFrameInterval(activeScreenId, skipFrameInterval);
    EXPECT_TRUE(result);

    // Reset to no skip
    RSInterfaces::GetInstance().SetScreenSkipFrameInterval(activeScreenId, 0);

    LOGI("SetScreenSkipFrameIntervalTest002 screenId[%{public}" PRIu64 "] skipFrameInterval[%{public}u]",
        activeScreenId, skipFrameInterval);
}

/*
 * @tc.name: SetVirtualScreenRefreshRateTest001
 * @tc.desc: test SetVirtualScreenRefreshRate to set virtual screen refresh rate
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetVirtualScreenRefreshRateTest001)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenRefreshRateTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t refreshRate = 60; // 60Hz
    bool result = RSInterfaces::GetInstance().SetVirtualScreenRefreshRate(virtualScreenId, refreshRate);
    EXPECT_TRUE(result);

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);

    LOGI("SetVirtualScreenRefreshRateTest001 screenId[%{public}" PRIu64 "] refreshRate[%{public}u]",
        virtualScreenId, refreshRate);
}

/*
 * @tc.name: SetVirtualScreenAutoRotationTest001
 * @tc.desc: test SetVirtualScreenAutoRotation to enable auto rotation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetVirtualScreenAutoRotationTest001)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenAutoRotationTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    bool result = RSInterfaces::GetInstance().SetVirtualScreenAutoRotation(virtualScreenId, true);
    EXPECT_TRUE(result);

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);

    LOGI("SetVirtualScreenAutoRotationTest001 screenId[%{public}" PRIu64 "] autoRotation[true]",
        virtualScreenId);
}

/*
 * @tc.name: SetVirtualScreenAutoRotationTest002
 * @tc.desc: test SetVirtualScreenAutoRotation to disable auto rotation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetVirtualScreenAutoRotationTest002)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenAutoRotationTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    bool result = RSInterfaces::GetInstance().SetVirtualScreenAutoRotation(virtualScreenId, false);
    EXPECT_TRUE(result);

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);

    LOGI("SetVirtualScreenAutoRotationTest002 screenId[%{public}" PRIu64 "] autoRotation[false]",
        virtualScreenId);
}

/*
 * @tc.name: SetVirtualScreenUsingStatusTest001
 * @tc.desc: test SetVirtualScreenUsingStatus to set virtual screen using status
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetVirtualScreenUsingStatusTest001)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenUsingStatusTest001", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    bool result = RSInterfaces::GetInstance().SetVirtualScreenUsingStatus(virtualScreenId, true);
    EXPECT_TRUE(result);

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);

    LOGI("SetVirtualScreenUsingStatusTest001 screenId[%{public}" PRIu64 "] usingStatus[true]",
        virtualScreenId);
}

/*
 * @tc.name: SetVirtualScreenUsingStatusTest002
 * @tc.desc: test SetVirtualScreenUsingStatus to set virtual screen not in use
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSScreenManagerTest, CONTENT_DISPLAY_TEST, SetVirtualScreenUsingStatusTest002)
{
    uint32_t width = 640;
    uint32_t height = 480;
    ScreenId virtualScreenId = RSInterfaces::GetInstance().CreateVirtualScreen(
        "SetVirtualScreenUsingStatusTest002", width, height, nullptr, INVALID_SCREEN_ID, -1, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    bool result = RSInterfaces::GetInstance().SetVirtualScreenUsingStatus(virtualScreenId, false);
    EXPECT_TRUE(result);

    RSInterfaces::GetInstance().RemoveVirtualScreen(virtualScreenId);

    LOGI("SetVirtualScreenUsingStatusTest002 screenId[%{public}" PRIu64 "] usingStatus[false]",
        virtualScreenId);
}

} // namespace OHOS::Rosen
