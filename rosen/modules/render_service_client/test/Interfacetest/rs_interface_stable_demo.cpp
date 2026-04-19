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

/**
 * @file rs_interface_stable_demo.cpp
 * @brief Comprehensive stability test for all RSInterfaces APIs
 * @description Tests all interfaces with various parameter combinations:
 *              - Normal values
 *              - Null values (where applicable)
 *              - Boundary values
 *              - Edge cases
 */

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <thread>
#include <chrono>

#include "rs_interface_client_frame_test.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_pipeline_client.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "transaction/rs_hgm_config_data.h"
#include "transaction/rs_occlusion_data.h"
#include "transaction/rs_self_drawing_node_rect_data.h"
#include "transaction/rs_uiextension_data.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_canvas_node.h"
#include "surface_utils.h"
#include "feature/capture/rs_ui_capture.h"
#include "pipeline/rs_surface_render_node.h"
#include "common/rs_common_def.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace std;
namespace OHOS {
namespace Rosen {
// ============================================================================
// Test Helper Classes
// ============================================================================
class CustomizedSurfaceCapture : public SurfaceCaptureCallback {
public:
    CustomizedSurfaceCapture() {}
    ~CustomizedSurfaceCapture() override {}
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override {}
    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
};

class TestSurfaceBufferCallback : public SurfaceBufferCallback {
public:
    void OnFinish(const FinishCallbackRet& ret) override {}
    void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) override {}
};

// Note: Other callbacks (ScreenChangeCallback, OcclusionChangeCallback, etc.) are std::function types, not classes

// ============================================================================
// 01. Screen ID Query Tests
// ============================================================================
void TestScreenIdQueries(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    // Normal call
    auto defaultScreenId = rsInterfaces.GetDefaultScreenId();
    auto activeScreenId = rsInterfaces.GetActiveScreenId();
    auto allScreenIds = rsInterfaces.GetAllScreenIds();
    (void)defaultScreenId;
    (void)activeScreenId;
    (void)allScreenIds;
}

// ============================================================================
// 02. Virtual Screen Management Tests
// ============================================================================
void TestVirtualScreenCreation(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Create real Surface for testing (refer to real test cases)
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface ? csurface->GetProducer() : nullptr;
    auto psurface = producer ? Surface::CreateSurfaceAsProducer(producer) : nullptr;

    std::vector<NodeId> whiteList = {1};

    // Normal call - using real Surface
    ScreenId screenId1 = rsInterfaces.CreateVirtualScreen("TestScreen1", 1920, 1080, psurface,
        INVALID_SCREEN_ID, -1, whiteList);
    ScreenId screenId2 = rsInterfaces.CreateVirtualScreen("TestScreen2", 1280, 720, psurface);

    // Normal call - using nullptr (allowed in certain scenarios)
    ScreenId screenId3 = rsInterfaces.CreateVirtualScreen("TestScreen3", 720, 1280, nullptr, INVALID_SCREEN_ID, -1);

    // Boundary value test - using small size real Surface
    ScreenId screenId4 = rsInterfaces.CreateVirtualScreen("SmallScreen", 320, 180, psurface, INVALID_SCREEN_ID, 0);
    ScreenId screenId5 = rsInterfaces.CreateVirtualScreen("MinScreen", 0, 0, nullptr, 0, 0, {});

    // Boundary value - large size
    ScreenId screenId6 = rsInterfaces.CreateVirtualScreen("LargeScreen", 1344, 2772, psurface);

    // Abnormal call - empty name
    ScreenId screenId7 = rsInterfaces.CreateVirtualScreen("", 1920, 1080, psurface);

    // Abnormal call - invalid associatedScreenId
    ScreenId screenId8 = rsInterfaces.CreateVirtualScreen("InvalidScreen", 1920, 1080, nullptr, -1, 0, {});

    (void)screenId3;
    (void)screenId5;
    (void)screenId7;
    (void)screenId8;

    // Clean up created virtual screen (avoid memory leak)
    if (screenId1 != INVALID_SCREEN_ID) rsInterfaces.RemoveVirtualScreen(screenId1);
    if (screenId2 != INVALID_SCREEN_ID) rsInterfaces.RemoveVirtualScreen(screenId2);
    if (screenId4 != INVALID_SCREEN_ID) rsInterfaces.RemoveVirtualScreen(screenId4);
    if (screenId6 != INVALID_SCREEN_ID) rsInterfaces.RemoveVirtualScreen(screenId6);
}

void TestVirtualScreenBlackList(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    std::vector<NodeId> blackList = {1, 2, 3};
    std::vector<NodeId> emptyList = {};

    // Normal call
    rsInterfaces.SetVirtualScreenBlackList(screenId, blackList);
    rsInterfaces.AddVirtualScreenBlackList(screenId, blackList);
    rsInterfaces.RemoveVirtualScreenBlackList(screenId, blackList);

    // Abnormal call - empty list
    rsInterfaces.SetVirtualScreenBlackList(screenId, emptyList);
    rsInterfaces.AddVirtualScreenBlackList(screenId, emptyList);
    rsInterfaces.RemoveVirtualScreenBlackList(screenId, emptyList);

    // Abnormal call - invalid screenId
    rsInterfaces.SetVirtualScreenBlackList(0, blackList);
    rsInterfaces.SetVirtualScreenBlackList(-1, blackList);
}

void TestVirtualScreenWhiteList(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    std::vector<NodeId> whiteList = {4, 5, 6};
    std::vector<NodeId> emptyList = {};

    // Normal call
    rsInterfaces.AddVirtualScreenWhiteList(screenId, whiteList);
    rsInterfaces.RemoveVirtualScreenWhiteList(screenId, whiteList);

    // Abnormal call - empty list
    rsInterfaces.AddVirtualScreenWhiteList(screenId, emptyList);
    rsInterfaces.RemoveVirtualScreenWhiteList(screenId, emptyList);

    // Abnormal call - invalid screenId
    rsInterfaces.AddVirtualScreenWhiteList(0, whiteList);
    rsInterfaces.RemoveVirtualScreenWhiteList(-1, whiteList);
}

void TestVirtualScreenTypeBlackList(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    std::vector<NodeType> typeBlackList = {1, 2};
    std::vector<NodeType> emptyList = {};

    // Normal call
    rsInterfaces.SetVirtualScreenTypeBlackList(screenId, typeBlackList);

    // Abnormal call - empty list
    rsInterfaces.SetVirtualScreenTypeBlackList(screenId, emptyList);

    // Abnormal call - invalid screenId
    rsInterfaces.SetVirtualScreenTypeBlackList(0, typeBlackList);
}

void TestVirtualScreenSecurityExemption(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    std::vector<NodeId> securityList = {7, 8, 9};
    auto pixelMap = std::make_shared<Media::PixelMap>();

    // Normal call
    rsInterfaces.SetVirtualScreenSecurityExemptionList(screenId, securityList);
    rsInterfaces.SetScreenSecurityMask(screenId, pixelMap);

    // Abnormal call - empty list
    rsInterfaces.SetVirtualScreenSecurityExemptionList(screenId, {});

    // Abnormal call - null pointer
    rsInterfaces.SetScreenSecurityMask(screenId, nullptr);
    rsInterfaces.SetScreenSecurityMask(0, pixelMap);
}

void TestVirtualScreenMirror(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    Rect rect = {0, 0, 1920, 1080};
    rsInterfaces.SetMirrorScreenVisibleRect(screenId, rect, false);
    rsInterfaces.SetMirrorScreenVisibleRect(screenId, rect, true);
    rsInterfaces.SetCastScreenEnableSkipWindow(screenId, true);
    rsInterfaces.SetCastScreenEnableSkipWindow(screenId, false);

    // Abnormal call - empty Rect
    rsInterfaces.SetMirrorScreenVisibleRect(screenId, Rect(), false);

    // Abnormal call - invalid screenId
    rsInterfaces.SetMirrorScreenVisibleRect(0, rect, false);
}

void TestVirtualScreenSurface(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Create real Surface for testing
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface ? csurface->GetProducer() : nullptr;
    auto psurface = producer ? Surface::CreateSurfaceAsProducer(producer) : nullptr;

    // Normal call - using real Surface
    rsInterfaces.SetVirtualScreenSurface(screenId, psurface);

    // Normal call - using nullptr (allowed in certain scenarios)
    rsInterfaces.SetVirtualScreenSurface(screenId, nullptr);

    // Abnormal call - invalid screenId
    rsInterfaces.SetVirtualScreenSurface(0, psurface);
    rsInterfaces.SetVirtualScreenSurface(0, nullptr);

    // Clean up
    rsInterfaces.RemoveVirtualScreen(screenId);
}

void TestVirtualScreenConfiguration(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    rsInterfaces.SetVirtualMirrorScreenCanvasRotation(screenId, true);
    rsInterfaces.SetVirtualMirrorScreenCanvasRotation(screenId, false);
    rsInterfaces.SetVirtualScreenAutoRotation(screenId, true);
    rsInterfaces.SetVirtualScreenAutoRotation(screenId, false);

    rsInterfaces.SetVirtualMirrorScreenScaleMode(screenId, ScreenScaleMode::FILL_MODE);
    rsInterfaces.SetVirtualMirrorScreenScaleMode(screenId, ScreenScaleMode::UNISCALE_MODE);

    rsInterfaces.SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);
    rsInterfaces.SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE);

    rsInterfaces.SetVirtualScreenUsingStatus(true);
    rsInterfaces.SetVirtualScreenUsingStatus(false);

    // Abnormal call - invalid screenId
    rsInterfaces.SetVirtualMirrorScreenCanvasRotation(0, true);
    rsInterfaces.SetVirtualScreenAutoRotation(0, true);
}

void TestScreenResolution(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    rsInterfaces.SetRogScreenResolution(screenId, 1920, 1080);
    rsInterfaces.SetPhysicalScreenResolution(screenId, 1920, 1080);
    rsInterfaces.SetVirtualScreenResolution(screenId, 1920, 1080);
    rsInterfaces.ResizeVirtualScreen(screenId, 1920, 1080);

    uint32_t width, height;
    rsInterfaces.GetRogScreenResolution(screenId, width, height);

    auto virtualRes = rsInterfaces.GetVirtualScreenResolution(screenId);

    // Boundary value - minimum
    rsInterfaces.SetRogScreenResolution(screenId, 0, 0);
    rsInterfaces.SetPhysicalScreenResolution(screenId, 0, 0);
    rsInterfaces.SetVirtualScreenResolution(screenId, 0, 0);
    rsInterfaces.ResizeVirtualScreen(screenId, 0, 0);

    // Boundary value - maximum
    rsInterfaces.SetRogScreenResolution(screenId, UINT32_MAX, UINT32_MAX);
    rsInterfaces.SetPhysicalScreenResolution(screenId, UINT32_MAX, UINT32_MAX);
    rsInterfaces.SetVirtualScreenResolution(screenId, UINT32_MAX, UINT32_MAX);
    rsInterfaces.ResizeVirtualScreen(screenId, UINT32_MAX, UINT32_MAX);

    // Abnormal call - invalid screenId
    rsInterfaces.SetRogScreenResolution(0, 1920, 1080);
    rsInterfaces.SetRogScreenResolution(-1, 1920, 1080);
}

// ============================================================================
// 03. Screen Callback Tests
// ============================================================================
void TestScreenChangeCallback(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    ScreenChangeCallback screenCallback = [](ScreenId screenId, ScreenEvent eventType, ScreenChangeReason reason) {};
    rsInterfaces.SetScreenChangeCallback(screenCallback);

    ScreenSwitchingNotifyCallback switchingCallback = [](bool isSwitching) {};
    rsInterfaces.SetScreenSwitchingNotifyCallback(switchingCallback);

    // Abnormal call - empty callback
    rsInterfaces.SetScreenChangeCallback(nullptr);
    rsInterfaces.SetScreenSwitchingNotifyCallback(nullptr);
}

void TestBrightnessCallback(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    BrightnessInfoChangeCallback brightnessCallback = [](ScreenId id, BrightnessInfo info) {};
    rsInterfaces.SetBrightnessInfoChangeCallback(brightnessCallback);

    BrightnessInfo brightnessInfo;
    rsInterfaces.GetBrightnessInfo(screenId, brightnessInfo);

    // Abnormal call - empty callback
    rsInterfaces.SetBrightnessInfoChangeCallback(nullptr);

    // Abnormal call - invalid screenId
    rsInterfaces.GetBrightnessInfo(0, brightnessInfo);
    rsInterfaces.GetBrightnessInfo(-1, brightnessInfo);
}

// ============================================================================
// 04. Watermark Tests
// ============================================================================
void TestWatermark(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Create real PixelMap data (refer to real test cases)
    const uint32_t color[8] = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    const int32_t offset = 0;
    Media::InitializationOptions opts;
    opts.size.width = 3;
    opts.size.height = 2;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    int32_t stride = opts.size.width;
    std::unique_ptr<Media::PixelMap> pixelMap1 = Media::PixelMap::Create(color, colorLength, offset, stride, opts);
    auto watermark = std::shared_ptr<Media::PixelMap>(pixelMap1.release());

    pid_t pid = getpid();
    std::vector<NodeId> nodeIdList = {1, 2, 3};

    // Normal call - using real PixelMap data
    rsInterfaces.SetWatermark("TestWatermark", watermark, SaSurfaceWatermarkMaxSize::SA_WATER_MARK_DEFAULT_SIZE);
    rsInterfaces.SetWatermark("TestWatermark", watermark, SaSurfaceWatermarkMaxSize::SA_WATER_MARK_MIDDLE_SIZE);
    rsInterfaces.SetWatermark("TestWatermark", watermark, SaSurfaceWatermarkMaxSize::SA_WATER_MARK_BOTTOM);

    rsInterfaces.SetSurfaceWatermark(pid, "SurfaceWatermark", watermark,
        nodeIdList, SurfaceWatermarkType::CUSTOM_WATER_MARK);
    rsInterfaces.SetSurfaceWatermark(pid, "SurfaceWatermark", watermark,
        nodeIdList, SurfaceWatermarkType::SYSTEM_WATER_MARK);

    rsInterfaces.ClearSurfaceWatermarkForNodes(pid, "TestWatermark", nodeIdList);
    rsInterfaces.ClearSurfaceWatermark(pid, "TestWatermark");

    rsInterfaces.ShowWatermark(watermark, true);
    rsInterfaces.ShowWatermark(watermark, false);

    // Abnormal call - null pointer
    rsInterfaces.SetWatermark("NullWatermark", nullptr);
    rsInterfaces.SetSurfaceWatermark(pid, "SurfaceWatermark", nullptr,
        nodeIdList, SurfaceWatermarkType::CUSTOM_WATER_MARK);

    // Abnormal call - empty name
    rsInterfaces.SetWatermark("", watermark);
    rsInterfaces.ClearSurfaceWatermark(pid, "");

    // Abnormal call - empty list
    rsInterfaces.SetSurfaceWatermark(pid, "SurfaceWatermark", watermark, {}, SurfaceWatermarkType::CUSTOM_WATER_MARK);
    rsInterfaces.ClearSurfaceWatermarkForNodes(pid, "TestWatermark", {});
}

// ============================================================================
// 05. Screen Mode and Capability Tests
// ============================================================================
void TestScreenModes(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    auto modeInfo = rsInterfaces.GetScreenActiveMode(screenId);
    auto supportedModes = rsInterfaces.GetScreenSupportedModes(screenId);
    auto capability = rsInterfaces.GetScreenCapability(screenId);
    auto screenData = rsInterfaces.GetScreenData(screenId);

    if (!supportedModes.empty()) {
        uint32_t modeId = supportedModes[0].GetScreenModeId();
        rsInterfaces.SetScreenActiveMode(screenId, modeId);
    }

    // Abnormal call - invalid screenId
    rsInterfaces.GetScreenActiveMode(0);
    rsInterfaces.GetScreenSupportedModes(0);
    rsInterfaces.GetScreenCapability(0);
    rsInterfaces.GetScreenData(0);

    // Abnormal call - invalid modeId
    rsInterfaces.SetScreenActiveMode(screenId, 0);
    rsInterfaces.SetScreenActiveMode(screenId, UINT32_MAX);
}

// ============================================================================
// 06. Memory Management Tests
// ============================================================================
void TestMemoryManagement(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    pid_t pid = getpid();

    // Normal call
    auto memGraphic = rsInterfaces.GetMemoryGraphic(pid);
    auto memGraphics = rsInterfaces.GetMemoryGraphics();

    float cpuMemSize, gpuMemSize;
    rsInterfaces.GetTotalAppMemSize(cpuMemSize, gpuMemSize);

    float gpuMemInMB;
    rsInterfaces.GetPidGpuMemoryInMB(pid, gpuMemInMB);

    std::vector<PixelMapInfo> pixelMapInfoVector;
    rsInterfaces.GetPixelMapByProcessId(pixelMapInfoVector, pid);

    // Abnormal call - invalid pid
    rsInterfaces.GetMemoryGraphic(0);
    rsInterfaces.GetMemoryGraphic(-1);
    rsInterfaces.GetPidGpuMemoryInMB(0, gpuMemInMB);
    rsInterfaces.GetPidGpuMemoryInMB(-1, gpuMemInMB);
    rsInterfaces.GetPixelMapByProcessId(pixelMapInfoVector, 0);
}

// ============================================================================
// 07. Screen Power and Rendering Tests
// ============================================================================
void TestScreenPowerControl(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    rsInterfaces.SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    rsInterfaces.SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    rsInterfaces.SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_STANDBY);

    auto powerStatus = rsInterfaces.GetScreenPowerStatus(screenId);
    auto panelStatus = rsInterfaces.GetPanelPowerStatus(screenId);
    (void)powerStatus;
    (void)panelStatus;

    Rect activeRect = {0, 0, 1920, 1080};
    rsInterfaces.SetScreenActiveRect(screenId, activeRect);

    rsInterfaces.SetScreenOffset(screenId, 0, 0);
    rsInterfaces.SetScreenOffset(screenId, 100, 100);

    rsInterfaces.MarkPowerOffNeedProcessOneFrame();
    rsInterfaces.RepaintEverything();
    rsInterfaces.DisablePowerOffRenderControl(screenId);

    // Abnormal call - empty Rect
    rsInterfaces.SetScreenActiveRect(screenId, Rect());

    // Abnormal call - negative offset
    rsInterfaces.SetScreenOffset(screenId, -100, -100);

    // Abnormal call - invalid screenId
    rsInterfaces.SetScreenPowerStatus(0, ScreenPowerStatus::POWER_STATUS_ON);
    rsInterfaces.GetScreenPowerStatus(0);
    rsInterfaces.GetPanelPowerStatus(0);
    rsInterfaces.SetScreenActiveRect(0, activeRect);
    rsInterfaces.SetScreenOffset(0, 0, 0);
    rsInterfaces.DisablePowerOffRenderControl(0);
}

// ============================================================================
// 08. Refresh Rate Tests
// ============================================================================
void TestRefreshRateControl(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    rsInterfaces.SetScreenRefreshRate(screenId, 0, 60);
    rsInterfaces.SetScreenRefreshRate(screenId, 1, 120);

    rsInterfaces.SetRefreshRateMode(0);
    rsInterfaces.SetRefreshRateMode(1);

    FrameRateLinkerId linkerId = 1;
    FrameRateRange range{30, 120, 60};
    rsInterfaces.SyncFrameRateRange(linkerId, range, 60);

    auto currentRate = rsInterfaces.GetScreenCurrentRefreshRate(screenId);
    auto rateMode = rsInterfaces.GetCurrentRefreshRateMode();
    auto supportedRates = rsInterfaces.GetScreenSupportedRefreshRates(screenId);

    rsInterfaces.SetShowRefreshRateEnabled(true);
    rsInterfaces.SetShowRefreshRateEnabled(false);
    rsInterfaces.SetShowRefreshRateEnabled(true, 1);
    rsInterfaces.SetShowRefreshRateEnabled(true, 2);

    bool showEnabled = rsInterfaces.GetShowRefreshRateEnabled();

    auto realtimeRate = rsInterfaces.GetRealtimeRefreshRate(screenId);

    (void)currentRate;
    (void)rateMode;
    (void)supportedRates;
    (void)showEnabled;
    (void)realtimeRate;

    pid_t pid = getpid();
    rsInterfaces.GetRefreshInfo(pid);
    rsInterfaces.GetRefreshInfoToSP(0);
    rsInterfaces.GetRefreshInfoByPidAndUniqueId(pid, 0);

    uint32_t actualRate;
    rsInterfaces.SetVirtualScreenRefreshRate(screenId, 60, actualRate);
    rsInterfaces.SetVirtualScreenRefreshRate(screenId, 120, actualRate);

    rsInterfaces.SetScreenSkipFrameInterval(screenId, 0);
    rsInterfaces.SetScreenSkipFrameInterval(screenId, 1);

    // Abnormal call - invalid screenId
    rsInterfaces.SetScreenRefreshRate(0, 0, 60);
    rsInterfaces.GetScreenCurrentRefreshRate(0);
    rsInterfaces.GetScreenSupportedRefreshRates(0);
    rsInterfaces.GetRealtimeRefreshRate(0);

    // Abnormal call - boundary value
    rsInterfaces.SetScreenRefreshRate(screenId, -1, 0);
    rsInterfaces.SetRefreshRateMode(-1);
    rsInterfaces.SetScreenSkipFrameInterval(screenId, UINT32_MAX);

    // Abnormal call - invalid pid
    rsInterfaces.GetRefreshInfo(0);
    rsInterfaces.GetRefreshInfoByPidAndUniqueId(0, 0);
}

// ============================================================================
// 09. Color Gamut and HDR Tests
// ============================================================================
void TestColorGamutAndHDR(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    std::vector<ScreenColorGamut> gamuts;
    rsInterfaces.GetScreenSupportedColorGamuts(screenId, gamuts);

    ScreenColorGamut colorGamut;
    rsInterfaces.GetScreenColorGamut(screenId, colorGamut);
    rsInterfaces.SetScreenColorGamut(screenId, 0);

    rsInterfaces.SetScreenGamutMap(screenId, ScreenGamutMap::GAMUT_MAP_CONSTANT);
    rsInterfaces.SetScreenGamutMap(screenId, ScreenGamutMap::GAMUT_MAP_HDR_CONSTANT);

    ScreenGamutMap gamutMap;
    rsInterfaces.GetScreenGamutMap(screenId, gamutMap);

    RSScreenHDRCapability hdrCapability;
    rsInterfaces.GetScreenHDRCapability(screenId, hdrCapability);

    std::vector<ScreenHDRFormat> hdrFormats;
    rsInterfaces.GetScreenSupportedHDRFormats(screenId, hdrFormats);

    ScreenHDRFormat hdrFormat;
    rsInterfaces.GetScreenHDRFormat(screenId, hdrFormat);

    HdrStatus hdrStatus;
    rsInterfaces.GetScreenHDRStatus(screenId, hdrStatus);
    rsInterfaces.SetScreenHDRFormat(screenId, 0);

    std::vector<ScreenHDRMetadataKey> keys;
    rsInterfaces.GetScreenSupportedMetaDataKeys(screenId, keys);

    // Abnormal call - invalid screenId
    rsInterfaces.GetScreenSupportedColorGamuts(0, gamuts);
    rsInterfaces.GetScreenColorGamut(0, colorGamut);
    rsInterfaces.SetScreenColorGamut(0, 0);
    rsInterfaces.SetScreenGamutMap(0, ScreenGamutMap::GAMUT_MAP_CONSTANT);
    rsInterfaces.GetScreenGamutMap(0, gamutMap);
    rsInterfaces.GetScreenHDRCapability(0, hdrCapability);
    rsInterfaces.GetScreenSupportedHDRFormats(0, hdrFormats);
    rsInterfaces.GetScreenHDRFormat(0, hdrFormat);
    rsInterfaces.GetScreenHDRStatus(0, hdrStatus);
    rsInterfaces.SetScreenHDRFormat(0, 0);
    rsInterfaces.GetScreenSupportedMetaDataKeys(0, keys);

    // Abnormal call - invalid modeId
    rsInterfaces.SetScreenColorGamut(screenId, -1);
    rsInterfaces.SetScreenHDRFormat(screenId, -1);
}

// ============================================================================
// 10. Pixel Format and Color Space Tests
// ============================================================================
void TestPixelFormatAndColorSpace(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    GraphicPixelFormat pixelFormat;
    rsInterfaces.GetPixelFormat(screenId, pixelFormat);
    rsInterfaces.SetPixelFormat(screenId, GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGB_565);

    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    rsInterfaces.GetScreenSupportedColorSpaces(screenId, colorSpaces);

    GraphicCM_ColorSpaceType colorSpace;
    rsInterfaces.GetScreenColorSpace(screenId, colorSpace);
    rsInterfaces.SetScreenColorSpace(screenId, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);
    rsInterfaces.SetScreenColorSpace(screenId, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL);

    // Abnormal call - invalid screenId
    rsInterfaces.GetPixelFormat(0, pixelFormat);
    rsInterfaces.SetPixelFormat(0, GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGB_565);
    rsInterfaces.GetScreenSupportedColorSpaces(0, colorSpaces);
    rsInterfaces.GetScreenColorSpace(0, colorSpace);
    rsInterfaces.SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);
}

// ============================================================================
// 11. Screen Type and Rotation Tests
// ============================================================================
void TestScreenTypeAndRotation(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    RSScreenType screenType;
    rsInterfaces.GetScreenType(screenId, screenType);

    uint8_t outPort;
    std::vector<uint8_t> edidData;
    rsInterfaces.GetDisplayIdentificationData(screenId, outPort, edidData);

    rsInterfaces.SetScreenCorrection(screenId, ScreenRotation::ROTATION_0);
    rsInterfaces.SetScreenCorrection(screenId, ScreenRotation::ROTATION_90);
    rsInterfaces.SetScreenCorrection(screenId, ScreenRotation::ROTATION_180);
    rsInterfaces.SetScreenCorrection(screenId, ScreenRotation::ROTATION_270);

    rsInterfaces.SetLogicalCameraRotationCorrection(screenId, ScreenRotation::ROTATION_0);
    rsInterfaces.SetLogicalCameraRotationCorrection(screenId, ScreenRotation::ROTATION_90);

    // Abnormal call - invalid screenId
    rsInterfaces.GetScreenType(0, screenType);
    rsInterfaces.GetDisplayIdentificationData(0, outPort, edidData);
    rsInterfaces.SetScreenCorrection(0, ScreenRotation::ROTATION_0);
    rsInterfaces.SetLogicalCameraRotationCorrection(0, ScreenRotation::ROTATION_0);
}

// ============================================================================
// 12. Screen Backlight Tests
// ============================================================================
void TestScreenBacklight(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    auto backlight = rsInterfaces.GetScreenBacklight(screenId);
    rsInterfaces.SetScreenBacklight(screenId, 100);
    (void)backlight;
    rsInterfaces.SetScreenBacklight(screenId, 50);

    // Boundary value - minimum
    rsInterfaces.SetScreenBacklight(screenId, 0);

    // Boundary value - maximum
    rsInterfaces.SetScreenBacklight(screenId, UINT32_MAX);

    // Abnormal call - invalid screenId
    rsInterfaces.GetScreenBacklight(0);
    rsInterfaces.SetScreenBacklight(0, 100);
}

// ============================================================================
// 13. Dual Screen Tests
// ============================================================================
void TestDualScreen(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    rsInterfaces.SetDualScreenState(screenId, DualScreenStatus::DUAL_SCREEN_ENTER);
    rsInterfaces.SetDualScreenState(screenId, DualScreenStatus::DUAL_SCREEN_EXIT);

    // Abnormal call - invalid screenId
    rsInterfaces.SetDualScreenState(0, DualScreenStatus::DUAL_SCREEN_ENTER);
}

// ============================================================================
// 14. Dark Color Mode Tests
// ============================================================================
void TestDarkColorMode(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetGlobalDarkColorMode(true);
    rsInterfaces.SetGlobalDarkColorMode(false);
}

// ============================================================================
// 15. VSync Receiver Tests
// ============================================================================
void TestVSyncReceiver(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Create EventHandler for testing (refer to rs_render_thread_test.cpp)
    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    // Test first overload version: CreateVSyncReceiver(name, looper = nullptr)
    auto receiver1 = rsInterfaces.CreateVSyncReceiver("TestReceiver");
    auto receiver2 = rsInterfaces.CreateVSyncReceiver("TestReceiverWithHandler", handler);

    // Test second overload version: CreateVSyncReceiver(name, id, looper, windowNodeId, fromXcomponent)
    auto receiver3 = rsInterfaces.CreateVSyncReceiver("TestReceiverWithId", 1);
    auto receiver4 = rsInterfaces.CreateVSyncReceiver("TestReceiverWithIdAndHandler", 1, handler);
    auto receiver5 = rsInterfaces.CreateVSyncReceiver("TestReceiverWithNodeId", 1, handler, 12345);
    auto receiver6 = rsInterfaces.CreateVSyncReceiver("TestReceiverFromXcomponent", 1, handler, 0, true);

    // Boundary value test - maximum id value
    auto receiver7 = rsInterfaces.CreateVSyncReceiver("TestReceiverMaxId", UINT64_MAX, handler);
    auto receiver8 = rsInterfaces.CreateVSyncReceiver("TestReceiverMaxNodeId", 1, handler, UINT64_MAX);

    // Abnormal call - empty name
    auto receiver9 = rsInterfaces.CreateVSyncReceiver("");
    auto receiver10 = rsInterfaces.CreateVSyncReceiver("", 1, handler);
}

// ============================================================================
// 16. Occlusion Callback Tests
// ============================================================================
void TestOcclusionCallbacks(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    OcclusionChangeCallback occlusionCallback = [](std::shared_ptr<RSOcclusionData> data) {};
    rsInterfaces.RegisterOcclusionChangeCallback(occlusionCallback);

    SurfaceOcclusionChangeCallback surfaceOcclusionCallback = [](float percent) {};
    std::vector<float> partitionPoints = {0.0f, 0.5f, 1.0f};
    rsInterfaces.RegisterSurfaceOcclusionChangeCallback(0, surfaceOcclusionCallback, partitionPoints);

    rsInterfaces.UnRegisterSurfaceOcclusionChangeCallback(0);

    // Abnormal call - invalid nodeId
    rsInterfaces.UnRegisterSurfaceOcclusionChangeCallback(-1);

    // Abnormal call - empty partitionPoints
    std::vector<float> emptyPoints;
    rsInterfaces.RegisterSurfaceOcclusionChangeCallback(0, surfaceOcclusionCallback, emptyPoints);
}

// ============================================================================
// 17. HGM Callback Tests
// ============================================================================
void TestHGMCallbacks(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    HgmConfigChangeCallback configCallback = [](std::shared_ptr<RSHgmConfigData> data) {};
    rsInterfaces.RegisterHgmConfigChangeCallback(configCallback);

    HgmRefreshRateModeChangeCallback modeCallback = [](int32_t mode) {};
    rsInterfaces.RegisterHgmRefreshRateModeChangeCallback(modeCallback);

    HgmRefreshRateUpdateCallback updateCallback = [](int32_t rate) {};
    rsInterfaces.RegisterHgmRefreshRateUpdateCallback(updateCallback);
    rsInterfaces.UnRegisterHgmRefreshRateUpdateCallback();

    // Abnormal call - empty callback
    rsInterfaces.RegisterHgmConfigChangeCallback(nullptr);
    rsInterfaces.RegisterHgmRefreshRateModeChangeCallback(nullptr);
    rsInterfaces.RegisterHgmRefreshRateUpdateCallback(nullptr);
}

// ============================================================================
// 18. Frame Commit Callback Tests
// ============================================================================
void TestFrameCommitCallbacks(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    FirstFrameCommitCallback commitCallback = [](uint64_t nodeId, int64_t timestamp) {};
    rsInterfaces.RegisterFirstFrameCommitCallback(commitCallback);
    rsInterfaces.UnRegisterFirstFrameCommitCallback();

    // Abnormal call - empty callback
    rsInterfaces.RegisterFirstFrameCommitCallback(nullptr);
}

// ============================================================================
// 19. Frame Rate Linker Callback Tests
// ============================================================================
void TestFrameRateLinkerCallbacks(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    FrameRateLinkerExpectedFpsUpdateCallback callback = [](int32_t pid,
        const std::string& bundleName, int32_t expectedFps) {};
    rsInterfaces.RegisterFrameRateLinkerExpectedFpsUpdateCallback(0, callback);
    rsInterfaces.UnRegisterFrameRateLinkerExpectedFpsUpdateCallback(0);

    // Abnormal call - empty callback
    rsInterfaces.RegisterFrameRateLinkerExpectedFpsUpdateCallback(0, nullptr);

    // Abnormal call - invalid pid
    rsInterfaces.UnRegisterFrameRateLinkerExpectedFpsUpdateCallback(-1);
}

// ============================================================================
// 20. Animation System Tests
// ============================================================================
void TestAnimationSystem(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetSystemAnimatedScenes(SystemAnimatedScenes::OTHERS);
    rsInterfaces.SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MISSION_CENTER);
    rsInterfaces.SetSystemAnimatedScenes(SystemAnimatedScenes::EXIT_MISSION_CENTER);
    rsInterfaces.SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MAX_WINDOW);

    rsInterfaces.SetSystemAnimatedScenes(SystemAnimatedScenes::OTHERS, true);
    rsInterfaces.SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MISSION_CENTER, false);
}

// ============================================================================
// 21. Reporting and Events Tests
// ============================================================================
void TestReportingAndEvents(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.ReportJankStats();
    rsInterfaces.NotifyLightFactorStatus(0);
    rsInterfaces.NotifyLightFactorStatus(1);

    std::vector<std::string> packageList = {"com.example.app1", "com.example.app2"};
    rsInterfaces.NotifyPackageEvent(2, packageList);

    std::string pkgName = "com.example.app";
    std::vector<std::pair<std::string, std::string>> newConfig = {{"key1", "value1"}, {"key2", "value2"}};
    rsInterfaces.NotifyAppStrategyConfigChangeEvent(pkgName, 2, newConfig);

    EventInfo eventInfo;
    rsInterfaces.NotifyRefreshRateEvent(eventInfo);

    std::unordered_map<uint64_t, EventInfo> eventInfos;
    eventInfos[0] = eventInfo;
    rsInterfaces.SetWindowExpectedRefreshRate(eventInfos);

    std::unordered_map<std::string, EventInfo> eventInfoStrs;
    eventInfoStrs["test"] = eventInfo;
    rsInterfaces.SetWindowExpectedRefreshRate(eventInfoStrs);

    rsInterfaces.NotifySoftVsyncRateDiscountEvent(0, "test", 60);
    rsInterfaces.NotifyTouchEvent(0, 1, 0);
    rsInterfaces.NotifyDynamicModeEvent(true);
    rsInterfaces.NotifyHgmConfigEvent("TestEvent", true);
    rsInterfaces.NotifyXComponentExpectedFrameRate("testId", 60);

    DataBaseRs dbInfo;
    rsInterfaces.ReportEventResponse(dbInfo);
    rsInterfaces.ReportEventComplete(dbInfo);
    rsInterfaces.ReportEventJankFrame(dbInfo);

    GameStateData gameStateData;
    rsInterfaces.ReportGameStateData(gameStateData);

    AppInfo appInfo;
    rsInterfaces.ReportRsSceneJankStart(appInfo);
    rsInterfaces.ReportRsSceneJankEnd(appInfo);

    // Abnormal call - empty list/string
    rsInterfaces.NotifyPackageEvent(0, {});
    rsInterfaces.NotifyPackageEvent(UINT32_MAX, packageList);
    rsInterfaces.NotifyAppStrategyConfigChangeEvent("", 0, {});
    rsInterfaces.SetWindowExpectedRefreshRate(std::unordered_map<uint64_t, EventInfo>());
    rsInterfaces.SetWindowExpectedRefreshRate(std::unordered_map<std::string, EventInfo>());
    rsInterfaces.NotifySoftVsyncRateDiscountEvent(0, "", 0);
    rsInterfaces.NotifyXComponentExpectedFrameRate("", 0);

    // Abnormal call - invalid value
    rsInterfaces.NotifyLightFactorStatus(-1);
    rsInterfaces.NotifyTouchEvent(-1, 0, -1);
}

// ============================================================================
// 22. Cache Control Tests
// ============================================================================
void TestCacheControl(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.EnableCacheForRotation();
    rsInterfaces.DisableCacheForRotation();
    rsInterfaces.SetVmaCacheStatus(true);
    rsInterfaces.SetVmaCacheStatus(false);
}

// ============================================================================
// 23. Remote Death Callback Tests
// ============================================================================
void TestRemoteDeathCallback(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    OnRemoteDiedCallback callback = []() {};
    rsInterfaces.SetOnRemoteDiedCallback(callback);

    // Abnormal call - empty callback
    rsInterfaces.SetOnRemoteDiedCallback(nullptr);
}

// ============================================================================
// 24. Curtain Screen Tests
// ============================================================================
void TestCurtainScreen(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetCurtainScreenUsingStatus(true);
    rsInterfaces.SetCurtainScreenUsingStatus(false);
}

// ============================================================================
// 25. Frame Dropping Tests
// ============================================================================
void TestFrameDropping(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    std::vector<int32_t> pidList = {getpid()};
    rsInterfaces.DropFrameByPid(pidList, 0);
    rsInterfaces.DropFrameByPid(pidList, 1);

    // Abnormal call - empty list
    rsInterfaces.DropFrameByPid({}, 0);

    // Abnormal call - invalid dropFrameLevel
    rsInterfaces.DropFrameByPid(pidList, -1);
}

// ============================================================================
// 26. Dirty Region Tests
// ============================================================================
void TestDirtyRegionInfo(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    auto activeDirtyRegionInfo = rsInterfaces.GetActiveDirtyRegionInfo();
    auto globalDirtyRegionInfo = rsInterfaces.GetGlobalDirtyRegionInfo();
    auto layerComposeInfo = rsInterfaces.GetLayerComposeInfo();
    auto hwcDisabledReasons = rsInterfaces.GetHwcDisabledReasonInfo();
    auto hdrDuration = rsInterfaces.GetHdrOnDuration();
    (void)activeDirtyRegionInfo;
    (void)globalDirtyRegionInfo;
    (void)layerComposeInfo;
    (void)hwcDisabledReasons;
    (void)hdrDuration;
}

// ============================================================================
// 27. Touch Panel Tests
// ============================================================================
void TestTouchPanel(InterfaceClientFrameTest &testFrame)
{
#ifdef TP_FEATURE_ENABLE
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetTpFeatureConfig(0, "config");
    rsInterfaces.SetTpFeatureConfig(1, "config", TpFeatureConfigType::DEFAULT_TP_FEATURE);
    rsInterfaces.SetTpFeatureConfig(0, "config", TpFeatureConfigType::AFT_TP_FEATURE);

    // Abnormal call - empty config
    rsInterfaces.SetTpFeatureConfig(0, "");
#else
    (void)testFrame;
#endif
}

// ============================================================================
// 28. UI Extension Callback Tests
// ============================================================================
void TestUIExtensionCallbacks(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    UIExtensionCallback callback = [](std::shared_ptr<RSUIExtensionData> data, uint64_t userId) {};
    rsInterfaces.RegisterUIExtensionCallback(0, callback);
    rsInterfaces.RegisterUIExtensionCallback(0, callback, true);
    rsInterfaces.RegisterUIExtensionCallback(0, callback, false);

    // Abnormal call - empty callback
    rsInterfaces.RegisterUIExtensionCallback(0, nullptr);
}

// ============================================================================
// 29. Transaction Callback Tests
// ============================================================================
void TestTransactionCallbacks(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    std::function<void()> callback = []() {};
    rsInterfaces.RegisterTransactionDataCallback(0, 0, callback);

    // Abnormal call - empty callback
    rsInterfaces.RegisterTransactionDataCallback(0, 0, nullptr);

    // Abnormal call - invalid token
    rsInterfaces.RegisterTransactionDataCallback(UINT64_MAX, 0, callback);
}

// ============================================================================
// 30. Canvas Callback Tests
// ============================================================================
void TestCanvasCallbacks(InterfaceClientFrameTest &testFrame)
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.RegisterCanvasCallback(nullptr);

    NodeId nodeId = 0;
    rsInterfaces.SubmitCanvasPreAllocatedBuffer(nodeId, nullptr, 0);

    // Abnormal call - invalid nodeId
    rsInterfaces.SubmitCanvasPreAllocatedBuffer(UINT64_MAX, nullptr, 0);
#else
    (void)testFrame;
#endif
}

// ============================================================================
// 31. Screen Switching Tests
// ============================================================================
void TestScreenSwitching(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.NotifyScreenSwitched();
    rsInterfaces.ForceRefreshOneFrameWithNextVSync();
}

// ============================================================================
// 32. Self Drawing Node Callback Tests
// ============================================================================
void TestSelfDrawingNodeCallbacks(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    RectConstraint constraint;
    SelfDrawingNodeRectChangeCallback callback = [](std::shared_ptr<RSSelfDrawingNodeRectData> data) {};
    rsInterfaces.RegisterSelfDrawingNodeRectChangeCallback(constraint, callback);
    rsInterfaces.UnRegisterSelfDrawingNodeRectChangeCallback();

    // Abnormal call - empty callback
    rsInterfaces.RegisterSelfDrawingNodeRectChangeCallback(constraint, nullptr);
}

// ============================================================================
// 33. Overlay Display Tests
// ============================================================================
void TestOverlayDisplay(InterfaceClientFrameTest &testFrame)
{
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetOverlayDisplayMode(0);
    rsInterfaces.SetOverlayDisplayMode(1);

    // Abnormal call - invalid mode
    rsInterfaces.SetOverlayDisplayMode(-1);
#else
    (void)testFrame;
#endif
}

// ============================================================================
// 34. Page Name Tests
// ============================================================================
void TestPageName(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.NotifyPageName("com.example.app", "MainPage", true);
    rsInterfaces.NotifyPageName("com.example.app", "MainPage", false);

    // Abnormal call - empty string
    rsInterfaces.NotifyPageName("", "", false);
}

// ============================================================================
// 35. Accessibility Tests
// ============================================================================
void TestAccessibility(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    auto highContrastState = rsInterfaces.GetHighContrastTextState();
    (void)highContrastState;
    rsInterfaces.SetBehindWindowFilterEnabled(true);
    rsInterfaces.SetBehindWindowFilterEnabled(false);

    bool enabled;
    rsInterfaces.GetBehindWindowFilterEnabled(enabled);
}

// ============================================================================
// 36. Video Codec Tests
// ============================================================================
void TestVideoCodec(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    std::vector<uint64_t> uniqueIdList = {1, 2, 3};
    std::vector<std::string> surfaceNameList = {"surface1", "surface2"};

    rsInterfaces.AvcodecVideoStart(uniqueIdList, surfaceNameList, 60, 0);
    rsInterfaces.AvcodecVideoStop(uniqueIdList, surfaceNameList, 60);

    rsInterfaces.AvcodecVideoGet(0);
    rsInterfaces.AvcodecVideoGetRecent();

    // Abnormal call - empty list
    rsInterfaces.AvcodecVideoStart({}, {}, 0, 0);
    rsInterfaces.AvcodecVideoStop({}, {}, 0);
}

// ============================================================================
// 37. Multi-Window Status Tests
// ============================================================================
void TestMultiWindowStatus(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetFreeMultiWindowStatus(true);
    rsInterfaces.SetFreeMultiWindowStatus(false);
}

// ============================================================================
// 38. Typeface Tests
// ============================================================================
void TestTypeface(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call - null pointer test
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;
    rsInterfaces.RegisterTypeface(typeface);

    // Abnormal call - invalid id
    rsInterfaces.UnRegisterTypeface(0);
    rsInterfaces.UnRegisterTypeface(UINT32_MAX);
}

// ============================================================================
// 39. Layer Management Tests
// ============================================================================
void TestLayerManagement(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetLayerTop("0", true);
    rsInterfaces.SetLayerTop("1", false);

    rsInterfaces.SetForceRefresh("0", true);
    rsInterfaces.SetForceRefresh("1", false);

    rsInterfaces.SetColorFollow("0", true);
    rsInterfaces.SetColorFollow("1", false);

    rsInterfaces.SetLayerTopForHWC(0, true, 0);
    rsInterfaces.SetLayerTopForHWC(0, false, 1);

    // Abnormal call - empty string
    rsInterfaces.SetLayerTop("", true);
    rsInterfaces.SetForceRefresh("", false);
    rsInterfaces.SetColorFollow("", true);

    // Abnormal call - invalid nodeId
    rsInterfaces.SetLayerTopForHWC(UINT64_MAX, true, 0);
}

// ============================================================================
// 40. PixelMap from Surface Tests
// ============================================================================
void TestPixelMapFromSurface(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    Rect srcRect = {0, 0, 1920, 1080};
    rsInterfaces.CreatePixelMapFromSurfaceId(0, srcRect, false);
    rsInterfaces.CreatePixelMapFromSurfaceId(0, srcRect, true);

    // Abnormal call - empty Rect
    rsInterfaces.CreatePixelMapFromSurfaceId(0, Rect(), false);

    // Abnormal call - invalid surfaceId
    rsInterfaces.CreatePixelMapFromSurfaceId(UINT64_MAX, srcRect, false);
}

// ============================================================================
// 41. HWC Node Bounds Tests
// ============================================================================
void TestHWCNodeBounds(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetHwcNodeBounds(0, 0, 0, 0, 0);
    rsInterfaces.SetHwcNodeBounds(1, 100, 100, 100, 100);

    // Abnormal call - boundary value
    rsInterfaces.SetHwcNodeBounds(-1, 0, 0, 0, 0);
    rsInterfaces.SetHwcNodeBounds(UINT64_MAX, 0, 0, 0, 0);
}

// ============================================================================
// 42. Window Container Tests
// ============================================================================
void TestWindowContainer(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetWindowContainer(0, true);
    rsInterfaces.SetWindowContainer(1, false);

    // Abnormal call - invalid nodeId
    rsInterfaces.SetWindowContainer(UINT64_MAX, true);
}

// ============================================================================
// 43. UI First Cache Tests
// ============================================================================
void TestUIFirstCache(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.ClearUifirstCache(0);
    rsInterfaces.ClearUifirstCache(1);

    // Abnormal call - invalid nodeId
    rsInterfaces.ClearUifirstCache(UINT64_MAX);
}

// ============================================================================
// 44. Surface Capture Tests (Original)
// ============================================================================
void TestSurfaceCaptureFunctionality(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    std::shared_ptr<SurfaceCaptureCallback> callback = std::make_shared<CustomizedSurfaceCapture>();
    RSSurfaceCaptureConfig captureConfig;

    // Normal call
    rsInterfaces.TakeSurfaceCapture(testFrame.GetRootSurfaceNode(), callback, captureConfig);
    rsInterfaces.TakeSurfaceCaptureWithBlur(testFrame.GetRootSurfaceNode(), callback, captureConfig);
    rsInterfaces.TakeUICaptureInRange(testFrame.GetRootSurfaceNode(),
        testFrame.GetRootCanvasNode(), 0, callback, 1.0, 1.0, true);
    rsInterfaces.TakeSurfaceCaptureSoloNodeList(testFrame.GetRootCanvasNode());
    rsInterfaces.TakeSelfSurfaceCapture(testFrame.GetRootSurfaceNode(), callback);
    rsInterfaces.TakeSurfaceCaptureWithAllWindows(nullptr, callback, captureConfig, true);

    // Abnormal call - null pointer
    rsInterfaces.TakeSurfaceCapture(testFrame.GetRootSurfaceNode(), nullptr, captureConfig);
    rsInterfaces.TakeSelfSurfaceCapture(nullptr, callback);
    rsInterfaces.TakeSelfSurfaceCapture(nullptr, nullptr);
    rsInterfaces.TakeSurfaceCaptureSoloNodeList(nullptr);
    rsInterfaces.TakeUICaptureInRange(nullptr, nullptr, 1, callback, 1.0, 1.0, true);
}

// ============================================================================
// 45. Screen Frame Gravity Tests
// ============================================================================
void TestScreenFrameGravity(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();

    // Normal call
    rsInterfaces.SetScreenFrameGravity(screenId, 0);
    rsInterfaces.SetScreenFrameGravity(screenId, 1);

    // Abnormal call - invalid screenId
    rsInterfaces.SetScreenFrameGravity(0, 0);
    rsInterfaces.SetScreenFrameGravity(-1, 0);
}

// ============================================================================
// 46. Surface Buffer Callback Tests
// ============================================================================
void TestSurfaceBufferCallbacks(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto callbackSurface = std::make_shared<TestSurfaceBufferCallback>();
    pid_t pid = getpid();

    // Normal call
    rsInterfaces.RegisterSurfaceBufferCallback(pid, 0, callbackSurface);
    rsInterfaces.RegisterSurfaceBufferCallback(pid, 1, callbackSurface);
    rsInterfaces.UnregisterSurfaceBufferCallback(pid, 0);
    rsInterfaces.UnregisterSurfaceBufferCallback(pid, 1);

    // Abnormal call - empty callback
    rsInterfaces.RegisterSurfaceBufferCallback(pid, 0, nullptr);
    rsInterfaces.RegisterSurfaceBufferCallback(pid, 0, nullptr);

    // Abnormal call - invalid uid
    rsInterfaces.UnregisterSurfaceBufferCallback(0, 1);
}

// ============================================================================
// 47. ANCO Tests
// ============================================================================
void TestANCO(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.SetAncoForceDoDirect(true);
    rsInterfaces.SetAncoForceDoDirect(false);
}

// ============================================================================
// 48. Focus App Info Tests
// ============================================================================
void TestFocusAppInfo(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    FocusAppInfo info;
    rsInterfaces.SetFocusAppInfo(info);
}

// ============================================================================
// 49. Freeze Screen Tests
// ============================================================================
void TestFreezeScreen(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // Normal call
    rsInterfaces.FreezeScreen(nullptr, true);
    rsInterfaces.FreezeScreen(nullptr, false);
}

// ============================================================================
// 50. Additional Surface Capture Tests
// ============================================================================
void TestAdditionalSurfaceCapture(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    std::shared_ptr<SurfaceCaptureCallback> callback = std::make_shared<CustomizedSurfaceCapture>();
    RSSurfaceCaptureConfig captureConfig;

    // Normal call
    rsInterfaces.TakeSurfaceCaptureForUIWithConfig(testFrame.GetRootCanvasNode(), callback, captureConfig);
    rsInterfaces.TakeSurfaceCaptureForUI(testFrame.GetRootCanvasNode(), callback, 1.0f, 1.0f, false);
    rsInterfaces.SetWindowFreezeImmediately(testFrame.GetRootSurfaceNode(), true, callback, captureConfig, 1E-6);

    // Abnormal call - null pointer
    rsInterfaces.TakeSurfaceCaptureForUIWithConfig(nullptr, callback, captureConfig);
    rsInterfaces.TakeSurfaceCaptureForUI(nullptr, callback, 1.0f, 1.0f, false);
    rsInterfaces.SetWindowFreezeImmediately(nullptr, true, callback, captureConfig, 1E-6);

    // Abnormal call - empty callback
    rsInterfaces.TakeSurfaceCaptureForUIWithConfig(testFrame.GetRootCanvasNode(), nullptr, captureConfig);
    rsInterfaces.TakeSurfaceCaptureForUI(testFrame.GetRootCanvasNode(), nullptr, 1.0f, 1.0f, false);
}

// ============================================================================
// 51. Missing Interfaces Tests (supplement missing interfaces)
// ============================================================================

// 51.1 TakeUICaptureInRangeWithConfig - completely untested interface
void TestTakeUICaptureInRangeWithConfig(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    std::shared_ptr<SurfaceCaptureCallback> callback = std::make_shared<CustomizedSurfaceCapture>();
    RSSurfaceCaptureConfig captureConfig;

    // Normal call - with complete config
    rsInterfaces.TakeUICaptureInRangeWithConfig(testFrame.GetRootSurfaceNode(),
        testFrame.GetRootCanvasNode(), true, callback, captureConfig);
    rsInterfaces.TakeUICaptureInRangeWithConfig(testFrame.GetRootSurfaceNode(),
        testFrame.GetRootCanvasNode(), false, callback, captureConfig);

    // Normal call - using default config
    rsInterfaces.TakeUICaptureInRangeWithConfig(testFrame.GetRootSurfaceNode(),
        testFrame.GetRootCanvasNode(), true, callback);
    rsInterfaces.TakeUICaptureInRangeWithConfig(testFrame.GetRootSurfaceNode(),
        testFrame.GetRootCanvasNode(), false, callback);

    // Abnormal call - null pointer
    rsInterfaces.TakeUICaptureInRangeWithConfig(nullptr, nullptr, true, callback);
    rsInterfaces.TakeUICaptureInRangeWithConfig(nullptr, nullptr, false, callback, captureConfig);

    // Abnormal call - empty callback
    rsInterfaces.TakeUICaptureInRangeWithConfig(testFrame.GetRootSurfaceNode(),
        testFrame.GetRootCanvasNode(), true, nullptr);
    rsInterfaces.TakeUICaptureInRangeWithConfig(testFrame.GetRootSurfaceNode(),
        testFrame.GetRootCanvasNode(), false, nullptr, captureConfig);
}

// 51.2 TakeSurfaceCapture overload function supplement test
void TestTakeSurfaceCaptureOverloads(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    std::shared_ptr<SurfaceCaptureCallback> callback = std::make_shared<CustomizedSurfaceCapture>();
    RSSurfaceCaptureConfig captureConfig;

    // Overload 1: TakeSurfaceCapture(RSDisplayNode, callback, config) - not tested
    // Note: Since testFrame does not provide DisplayNode, use nullptr for abnormal call test here
    std::shared_ptr<RSDisplayNode> displayNode = nullptr;
    rsInterfaces.TakeSurfaceCapture(displayNode, callback, captureConfig);  // empty DisplayNode
    rsInterfaces.TakeSurfaceCapture(displayNode, nullptr, captureConfig);  // empty DisplayNode + empty callback
    rsInterfaces.TakeSurfaceCapture(displayNode, callback, {});            // using default config

    // Overload 2: TakeSurfaceCapture(NodeId, callback, config) - not tested
    NodeId nodeId = testFrame.GetRootSurfaceNode()->GetId();
    rsInterfaces.TakeSurfaceCapture(nodeId, callback, captureConfig);
    rsInterfaces.TakeSurfaceCapture(nodeId, callback);                    // using default config
    rsInterfaces.TakeSurfaceCapture(nodeId, nullptr, captureConfig);       // empty callback
    rsInterfaces.TakeSurfaceCapture(0, callback, captureConfig);           // invalid ID
    rsInterfaces.TakeSurfaceCapture(UINT64_MAX, callback, captureConfig); // maximum ID
    rsInterfaces.TakeSurfaceCapture(0, nullptr);                           // invalid ID + empty callback

    // Original overload (RSSurfaceNode) default parameter test - using default captureConfig
    rsInterfaces.TakeSurfaceCapture(testFrame.GetRootSurfaceNode(), callback);
    rsInterfaces.TakeSurfaceCapture(testFrame.GetRootSurfaceNode(), nullptr);
    rsInterfaces.TakeSurfaceCapture(testFrame.GetRootSurfaceNode(), callback, {});
}

// 51.3 TakeSurfaceCaptureWithBlur default parameter supplement test
void TestTakeSurfaceCaptureWithBlurDefaults(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    std::shared_ptr<SurfaceCaptureCallback> callback = std::make_shared<CustomizedSurfaceCapture>();
    RSSurfaceCaptureConfig captureConfig;

    // using default captureConfig
    rsInterfaces.TakeSurfaceCaptureWithBlur(testFrame.GetRootSurfaceNode(), callback);

    // using default blurRadius (1E-6)
    rsInterfaces.TakeSurfaceCaptureWithBlur(testFrame.GetRootSurfaceNode(), callback, captureConfig);

    // custom blurRadius
    rsInterfaces.TakeSurfaceCaptureWithBlur(testFrame.GetRootSurfaceNode(), callback, captureConfig, 5.0f);
    rsInterfaces.TakeSurfaceCaptureWithBlur(testFrame.GetRootSurfaceNode(), callback, {}, 10.0f);

    // using double default parameters
    rsInterfaces.TakeSurfaceCaptureWithBlur(testFrame.GetRootSurfaceNode(), callback);

    // Abnormal call - null pointer
    rsInterfaces.TakeSurfaceCaptureWithBlur(nullptr, callback);
    rsInterfaces.TakeSurfaceCaptureWithBlur(nullptr, nullptr);
}

// 51.4 DropFrameByPid default parameter supplement test
void TestDropFrameByPidDefaults(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    std::vector<int32_t> pidList = {getpid()};

    // using default dropFrameLevel (0)
    rsInterfaces.DropFrameByPid(pidList);
    rsInterfaces.DropFrameByPid({});

    // Abnormal call - empty list
    rsInterfaces.DropFrameByPid({});
}

// 51.5 FreezeScreen default parameter supplement test
void TestFreezeScreenDefaults(InterfaceClientFrameTest &testFrame)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();

    // using default needSync (false)
    rsInterfaces.FreezeScreen(nullptr, true);
    rsInterfaces.FreezeScreen(nullptr, false);

    // explicitly specify needSync
    rsInterfaces.FreezeScreen(nullptr, true, true);
    rsInterfaces.FreezeScreen(nullptr, false, false);

    // Abnormal call - null pointer
    rsInterfaces.FreezeScreen(nullptr, true);
}

// ============================================================================
// 52. RSRenderPipelineClient Direct Interface Tests
// Tests for interfaces not exposed through RSInterfaces but available through RSRenderPipelineClient
// ============================================================================

// 52.1 Test CreateNode with RSDisplayNodeConfig
void TestCreateNodeDisplayConfig(InterfaceClientFrameTest &testFrame)
{
    auto rsClient = std::make_shared<RSRenderPipelineClient>();

    RSDisplayNodeConfig displayNodeConfig;
    displayNodeConfig.screenId = 0;
    displayNodeConfig.isMirrored = false;
    displayNodeConfig.mirrorNodeId = 0;
    displayNodeConfig.isSync = false;
    displayNodeConfig.mirrorSourceRotation = 4;
    NodeId nodeId = 1;

    // Normal call
    rsClient->CreateNode(displayNodeConfig, nodeId);

    // Test with different configs
    RSDisplayNodeConfig config2;
    config2.screenId = 1;
    config2.isMirrored = true;
    config2.mirrorNodeId = 2;
    config2.isSync = true;
    config2.mirrorSourceRotation = 0;
    NodeId nodeId2 = 2;
    rsClient->CreateNode(config2, nodeId2);

    // Test with invalid NodeId
    rsClient->CreateNode(displayNodeConfig, 0);
    rsClient->CreateNode(displayNodeConfig, UINT64_MAX);
}

// 52.2 Test CreateNode with RSSurfaceRenderNodeConfig
void TestCreateNodeSurfaceConfig(InterfaceClientFrameTest &testFrame)
{
    auto rsClient = std::make_shared<RSRenderPipelineClient>();

    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    config.name = "TestSurface";

    // Normal call
    rsClient->CreateNode(config);

    // Test with different configs
    RSSurfaceRenderNodeConfig config2;
    config2.id = 0;
    config2.name = "";
    rsClient->CreateNode(config2);

    RSSurfaceRenderNodeConfig config3;
    config3.id = UINT64_MAX;
    config3.name = "MaxIdSurface";
    rsClient->CreateNode(config3);
}

// 52.3 Test GetBitmap
void TestGetBitmap(InterfaceClientFrameTest &testFrame)
{
    auto rsClient = std::make_shared<RSRenderPipelineClient>();

    Drawing::Bitmap bitmap;
    NodeId testNodeId = 1;
    rsClient->GetBitmap(testNodeId, bitmap);

    // Test with invalid NodeId
    rsClient->GetBitmap(0, bitmap);

    // Test with maximum NodeId
    rsClient->GetBitmap(UINT64_MAX, bitmap);
}

// 52.4 Test CreateNodeAndSurface
void TestCreateNodeAndSurface(InterfaceClientFrameTest &testFrame)
{
    auto rsClient = std::make_shared<RSRenderPipelineClient>();

    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    config.name = "TestSurface";

    // Normal call
    rsClient->CreateNodeAndSurface(config);

    // With unobscured parameter
    rsClient->CreateNodeAndSurface(config, true);
    rsClient->CreateNodeAndSurface(config, false);

    // Test with different configs
    RSSurfaceRenderNodeConfig config2;
    config2.id = 0;
    config2.name = "";
    rsClient->CreateNodeAndSurface(config2);
}

// 52.5 Test GetPixelmap
void TestGetPixelmap(InterfaceClientFrameTest &testFrame)
{
    auto rsClient = std::make_shared<RSRenderPipelineClient>();

    std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
    NodeId testNodeId = 1;
    Drawing::Rect rect = {0, 0, 100, 100};
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;

    // Normal call with all parameters
    rsClient->GetPixelmap(testNodeId, pixelmap, &rect, drawCmdList);

    // With nullptr rect
    rsClient->GetPixelmap(testNodeId, pixelmap, nullptr, drawCmdList);

    // With nullptr drawCmdList
    rsClient->GetPixelmap(testNodeId, pixelmap, &rect, nullptr);

    // With nullptr for both
    rsClient->GetPixelmap(testNodeId, pixelmap, nullptr, nullptr);

    // Test with invalid NodeId
    rsClient->GetPixelmap(0, pixelmap, nullptr, nullptr);
}

// 52.6 Test SetHidePrivacyContent
void TestSetHidePrivacyContent(InterfaceClientFrameTest &testFrame)
{
    auto rsClient = std::make_shared<RSRenderPipelineClient>();

    NodeId testNodeId = 1;

    // Normal call - enable privacy
    rsClient->SetHidePrivacyContent(testNodeId, true);

    // Normal call - disable privacy
    rsClient->SetHidePrivacyContent(testNodeId, false);

    // Test with invalid NodeId
    rsClient->SetHidePrivacyContent(0, true);
    rsClient->SetHidePrivacyContent(UINT64_MAX, false);
}

// 52.7 Test SetHardwareEnabled
void TestSetHardwareEnabled(InterfaceClientFrameTest &testFrame)
{
    auto rsClient = std::make_shared<RSRenderPipelineClient>();

    NodeId testNodeId = 1;

    // Normal call - enable hardware
    rsClient->SetHardwareEnabled(testNodeId, true, SelfDrawingNodeType::DEFAULT, false);

    // Normal call - disable hardware
    rsClient->SetHardwareEnabled(testNodeId, false, SelfDrawingNodeType::DEFAULT, false);

    // With dynamic hardware enable
    rsClient->SetHardwareEnabled(testNodeId, true, SelfDrawingNodeType::DEFAULT, true);
    rsClient->SetHardwareEnabled(testNodeId, false, SelfDrawingNodeType::DEFAULT, true);

    // With different self drawing types
    rsClient->SetHardwareEnabled(testNodeId, true, SelfDrawingNodeType::IMAGE, false);
    rsClient->SetHardwareEnabled(testNodeId, true, SelfDrawingNodeType::EFFECT_NODE, false);

    // Test with invalid NodeId
    rsClient->SetHardwareEnabled(0, true, SelfDrawingNodeType::DEFAULT, false);
    rsClient->SetHardwareEnabled(UINT64_MAX, false, SelfDrawingNodeType::DEFAULT, true);
}

// 52.8 Main wrapper function - calls all RSRenderPipelineClient interface tests
void TestRSRenderPipelineClientInterfaces(InterfaceClientFrameTest &testFrame)
{
    TestCreateNodeDisplayConfig(testFrame);
    TestCreateNodeSurfaceConfig(testFrame);
    TestGetBitmap(testFrame);
    TestCreateNodeAndSurface(testFrame);
    TestGetPixelmap(testFrame);
    TestSetHidePrivacyContent(testFrame);
    TestSetHardwareEnabled(testFrame);
}
}
}

// ============================================================================
// Main Test Registration (must be in global namespace)
// ============================================================================
int main()
{
    using namespace OHOS::Rosen;

    std::cout << "=== RSInterfaces Comprehensive Stability Test ===" << std::endl;
    std::cout << "Total test suites: 50+" << std::endl;

    // 01. Screen ID Query Tests
    REGISTER_TEST("01.ScreenIdQueries", TestScreenIdQueries);

    // 02. Virtual Screen Management Tests
    REGISTER_TEST("02.VirtualScreenCreation", TestVirtualScreenCreation);
    REGISTER_TEST("02.VirtualScreenBlackList", TestVirtualScreenBlackList);
    REGISTER_TEST("02.VirtualScreenWhiteList", TestVirtualScreenWhiteList);
    REGISTER_TEST("02.VirtualScreenTypeBlackList", TestVirtualScreenTypeBlackList);
    REGISTER_TEST("02.VirtualScreenSecurityExemption", TestVirtualScreenSecurityExemption);
    REGISTER_TEST("02.VirtualScreenMirror", TestVirtualScreenMirror);
    REGISTER_TEST("02.VirtualScreenSurface", TestVirtualScreenSurface);
    REGISTER_TEST("02.VirtualScreenConfiguration", TestVirtualScreenConfiguration);
    REGISTER_TEST("02.ScreenResolution", TestScreenResolution);

    // 03. Screen Callback Tests
    REGISTER_TEST("03.ScreenChangeCallback", TestScreenChangeCallback);
    REGISTER_TEST("03.BrightnessCallback", TestBrightnessCallback);

    // 04. Watermark Tests
    REGISTER_TEST("04.Watermark", TestWatermark);

    // 05. Screen Mode and Capability Tests
    REGISTER_TEST("05.ScreenModes", TestScreenModes);

    // 06. Memory Management Tests
    REGISTER_TEST("06.MemoryManagement", TestMemoryManagement);

    // 07. Screen Power and Rendering Tests
    REGISTER_TEST("07.ScreenPowerControl", TestScreenPowerControl);

    // 08. Refresh Rate Tests
    REGISTER_TEST("08.RefreshRateControl", TestRefreshRateControl);

    // 09. Color Gamut and HDR Tests
    REGISTER_TEST("09.ColorGamutAndHDR", TestColorGamutAndHDR);

    // 10. Pixel Format and Color Space Tests
    REGISTER_TEST("10.PixelFormatAndColorSpace", TestPixelFormatAndColorSpace);

    // 11. Screen Type and Rotation Tests
    REGISTER_TEST("11.ScreenTypeAndRotation", TestScreenTypeAndRotation);

    // 12. Screen Backlight Tests
    REGISTER_TEST("12.ScreenBacklight", TestScreenBacklight);

    // 13. Dual Screen Tests
    REGISTER_TEST("13.DualScreen", TestDualScreen);

    // 14. Dark Color Mode Tests
    REGISTER_TEST("14.DarkColorMode", TestDarkColorMode);

    // 15. VSync Receiver Tests
    REGISTER_TEST("15.VSyncReceiver", TestVSyncReceiver);

    // 16. Occlusion Callback Tests
    REGISTER_TEST("16.OcclusionCallbacks", TestOcclusionCallbacks);

    // 17. HGM Callback Tests
    REGISTER_TEST("17.HGMCallbacks", TestHGMCallbacks);

    // 18. Frame Commit Callback Tests
    REGISTER_TEST("18.FrameCommitCallbacks", TestFrameCommitCallbacks);

    // 19. Frame Rate Linker Callback Tests
    REGISTER_TEST("19.FrameRateLinkerCallbacks", TestFrameRateLinkerCallbacks);

    // 20. Animation System Tests
    REGISTER_TEST("20.AnimationSystem", TestAnimationSystem);

    // 21. Reporting and Events Tests
    REGISTER_TEST("21.ReportingAndEvents", TestReportingAndEvents);

    // 22. Cache Control Tests
    REGISTER_TEST("22.CacheControl", TestCacheControl);

    // 23. Remote Death Callback Tests
    REGISTER_TEST("23.RemoteDeathCallback", TestRemoteDeathCallback);

    // 24. Curtain Screen Tests
    REGISTER_TEST("24.CurtainScreen", TestCurtainScreen);

    // 25. Frame Dropping Tests
    REGISTER_TEST("25.FrameDropping", TestFrameDropping);

    // 26. Dirty Region Tests
    REGISTER_TEST("26.DirtyRegionInfo", TestDirtyRegionInfo);

    // 27. Touch Panel Tests
    REGISTER_TEST("27.TouchPanel", TestTouchPanel);

    // 28. UI Extension Callback Tests
    REGISTER_TEST("28.UIExtensionCallbacks", TestUIExtensionCallbacks);

    // 29. Transaction Callback Tests
    REGISTER_TEST("29.TransactionCallbacks", TestTransactionCallbacks);

    // 30. Canvas Callback Tests
    REGISTER_TEST("30.CanvasCallbacks", TestCanvasCallbacks);

    // 31. Screen Switching Tests
    REGISTER_TEST("31.ScreenSwitching", TestScreenSwitching);

    // 32. Self Drawing Node Callback Tests
    REGISTER_TEST("32.SelfDrawingNodeCallbacks", TestSelfDrawingNodeCallbacks);

    // 33. Overlay Display Tests
    REGISTER_TEST("33.OverlayDisplay", TestOverlayDisplay);

    // 34. Page Name Tests
    REGISTER_TEST("34.PageName", TestPageName);

    // 35. Accessibility Tests
    REGISTER_TEST("35.Accessibility", TestAccessibility);

    // 36. Video Codec Tests
    REGISTER_TEST("36.VideoCodec", TestVideoCodec);

    // 37. Multi-Window Status Tests
    REGISTER_TEST("37.MultiWindowStatus", TestMultiWindowStatus);

    // 38. Typeface Tests
    REGISTER_TEST("38.Typeface", TestTypeface);

    // 39. Layer Management Tests
    REGISTER_TEST("39.LayerManagement", TestLayerManagement);

    // 40. PixelMap from Surface Tests
    REGISTER_TEST("40.PixelMapFromSurface", TestPixelMapFromSurface);

    // 41. HWC Node Bounds Tests
    REGISTER_TEST("41.HWCNodeBounds", TestHWCNodeBounds);

    // 42. Window Container Tests
    REGISTER_TEST("42.WindowContainer", TestWindowContainer);

    // 43. UI First Cache Tests
    REGISTER_TEST("43.UIFirstCache", TestUIFirstCache);

    // 44. Surface Capture Tests (Original)
    REGISTER_TEST("44.SurfaceCaptureFunctionality", TestSurfaceCaptureFunctionality);

    // 45. Screen Frame Gravity Tests
    REGISTER_TEST("45.ScreenFrameGravity", TestScreenFrameGravity);

    // 46. Surface Buffer Callback Tests
    REGISTER_TEST("46.SurfaceBufferCallbacks", TestSurfaceBufferCallbacks);

    // 47. ANCO Tests
    REGISTER_TEST("47.ANCO", TestANCO);

    // 48. Focus App Info Tests
    REGISTER_TEST("48.FocusAppInfo", TestFocusAppInfo);

    // 49. Freeze Screen Tests
    REGISTER_TEST("49.FreezeScreen", TestFreezeScreen);

    // 50. Additional Surface Capture Tests
    REGISTER_TEST("50.AdditionalSurfaceCapture", TestAdditionalSurfaceCapture);

    // 51. Missing Interfaces Tests (supplement missing interfaces)
    REGISTER_TEST("51.TakeUICaptureInRangeWithConfig", TestTakeUICaptureInRangeWithConfig);
    REGISTER_TEST("51.TakeSurfaceCaptureOverloads", TestTakeSurfaceCaptureOverloads);
    REGISTER_TEST("51.TakeSurfaceCaptureWithBlurDefaults", TestTakeSurfaceCaptureWithBlurDefaults);
    REGISTER_TEST("51.DropFrameByPidDefaults", TestDropFrameByPidDefaults);
    REGISTER_TEST("51.FreezeScreenDefaults", TestFreezeScreenDefaults);

    // 52. RSRenderPipelineClient Direct Interface Tests
    REGISTER_TEST("52.CreateNodeDisplayConfig", TestCreateNodeDisplayConfig);
    REGISTER_TEST("52.CreateNodeSurfaceConfig", TestCreateNodeSurfaceConfig);
    REGISTER_TEST("52.GetBitmap", TestGetBitmap);
    REGISTER_TEST("52.CreateNodeAndSurface", TestCreateNodeAndSurface);
    REGISTER_TEST("52.GetPixelmap", TestGetPixelmap);
    REGISTER_TEST("52.SetHidePrivacyContent", TestSetHidePrivacyContent);
    REGISTER_TEST("52.SetHardwareEnabled", TestSetHardwareEnabled);

    std::cout << "\n--- Running Comprehensive Tests ---\n" << std::endl;

    InterfaceClientFrameTest::GetInstance().Run();

    return EXIT_SUCCESS;
}
