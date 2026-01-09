/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_screen.h"

#include <algorithm>
#include <cinttypes>

#include "graphic_feature_param_manager.h"
#include "hgm_core.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_hisysevent.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"
#include "string_utils.h"

#undef LOG_TAG
#define LOG_TAG "RSScreen"

#define UPDATE_PROPERTY(name, value)            \
    do {                                        \
        auto prop = property_.Set##name(value); \
        NotifyScreenPropertyChange(prop);       \
    } while (0)                                 \

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;

std::map<GraphicColorGamut, GraphicCM_ColorSpaceType> RSScreen::RS_TO_COMMON_COLOR_SPACE_TYPE_MAP {
    {GRAPHIC_COLOR_GAMUT_STANDARD_BT601, GRAPHIC_CM_BT601_EBU_FULL},
    {GRAPHIC_COLOR_GAMUT_STANDARD_BT709, GRAPHIC_CM_BT709_FULL},
    {GRAPHIC_COLOR_GAMUT_SRGB, GRAPHIC_CM_SRGB_FULL},
    {GRAPHIC_COLOR_GAMUT_ADOBE_RGB, GRAPHIC_CM_ADOBERGB_FULL},
    {GRAPHIC_COLOR_GAMUT_DISPLAY_P3, GRAPHIC_CM_P3_FULL},
    {GRAPHIC_COLOR_GAMUT_BT2020, GRAPHIC_CM_DISPLAY_BT2020_SRGB},
    {GRAPHIC_COLOR_GAMUT_BT2100_PQ, GRAPHIC_CM_BT2020_PQ_FULL},
    {GRAPHIC_COLOR_GAMUT_BT2100_HLG, GRAPHIC_CM_BT2020_HLG_FULL},
    {GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, GRAPHIC_CM_DISPLAY_BT2020_SRGB},
};
std::map<GraphicCM_ColorSpaceType, GraphicColorGamut> RSScreen::COMMON_COLOR_SPACE_TYPE_TO_RS_MAP {
    {GRAPHIC_CM_BT601_EBU_FULL, GRAPHIC_COLOR_GAMUT_STANDARD_BT601},
    {GRAPHIC_CM_BT709_FULL, GRAPHIC_COLOR_GAMUT_STANDARD_BT709},
    {GRAPHIC_CM_SRGB_FULL, GRAPHIC_COLOR_GAMUT_SRGB},
    {GRAPHIC_CM_ADOBERGB_FULL, GRAPHIC_COLOR_GAMUT_ADOBE_RGB},
    {GRAPHIC_CM_P3_FULL, GRAPHIC_COLOR_GAMUT_DISPLAY_P3},
    {GRAPHIC_CM_DISPLAY_BT2020_SRGB, GRAPHIC_COLOR_GAMUT_BT2020},
    {GRAPHIC_CM_BT2020_PQ_FULL, GRAPHIC_COLOR_GAMUT_BT2100_PQ},
    {GRAPHIC_CM_BT2020_HLG_FULL, GRAPHIC_COLOR_GAMUT_BT2100_HLG},
    {GRAPHIC_CM_DISPLAY_BT2020_SRGB, GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020},
};
std::map<GraphicHDRFormat, ScreenHDRFormat> RSScreen::HDI_HDR_FORMAT_TO_RS_MAP {
    {GRAPHIC_NOT_SUPPORT_HDR, NOT_SUPPORT_HDR},
    {GRAPHIC_DOLBY_VISION, NOT_SUPPORT_HDR},
    {GRAPHIC_HDR10, VIDEO_HDR10},
    {GRAPHIC_HLG, VIDEO_HLG},
    {GRAPHIC_HDR10_PLUS, NOT_SUPPORT_HDR},
    {GRAPHIC_HDR_VIVID, VIDEO_HDR_VIVID},
};
std::map<ScreenHDRFormat, GraphicHDRFormat> RSScreen::RS_TO_HDI_HDR_FORMAT_MAP {
    {NOT_SUPPORT_HDR, GRAPHIC_NOT_SUPPORT_HDR},
    {VIDEO_HLG, GRAPHIC_HLG},
    {VIDEO_HDR10, GRAPHIC_HDR10},
    {VIDEO_HDR_VIVID, GRAPHIC_HDR_VIVID},
    {IMAGE_HDR_VIVID_DUAL, GRAPHIC_HDR_VIVID},
    {IMAGE_HDR_VIVID_SINGLE, GRAPHIC_HDR_VIVID},
    {IMAGE_HDR_ISO_DUAL, GRAPHIC_NOT_SUPPORT_HDR},
    {IMAGE_HDR_ISO_SINGLE, GRAPHIC_NOT_SUPPORT_HDR},
};

constexpr int MAX_LUM = 1000;

RSScreen::RSScreen(ScreenId id)
{
    property_.SetId(id);
    property_.SetIsVirtual(false);
    hdrCapability_.formatCount = 0;
    std::string name = "Screen_" + std::to_string(id);
    property_.SetName(name);
    property_.SetState(ScreenState::HDI_OUTPUT_ENABLE);
    PhysicalScreenInit();
    HILOG_COMM_WARN("init physical: {id: %{public}" PRIu64 ", w * h: [%{public}u * %{public}u], "
        "screenType: %{public}u}", id, property_.GetWidth(), property_.GetHeight(), property_.GetScreenType());

    capability_.props.clear();
}

RSScreen::RSScreen(const VirtualScreenConfigs &configs)
{
    property_.SetIsVirtual(true);
    property_.SetId(configs.id);
    associatedScreenId_ = configs.associatedScreenId;
    property_.SetName(configs.name);
    property_.SetResolution(std::make_pair(configs.width, configs.height));
    property_.SetVirtualSecLayerOption(configs.flags);
    property_.SetProducerSurface(configs.surface);
    property_.SetPixelFormat(configs.pixelFormat);
    property_.SetScreenType(RSScreenType::VIRTUAL_TYPE_SCREEN);
    property_.SetWhiteList(configs.whiteList);
    if (property_.GetProducerSurface()) {
        property_.SetState(ScreenState::SOFTWARE_OUTPUT_ENABLE);
    } else {
        property_.SetState(ScreenState::DISABLED);
    }
    VirtualScreenInit();
    HILOG_COMM_WARN("init virtual screen: {id: %{public}" PRIu64 ", associatedScreenId: %{public}" PRIu64", w * h: "
        "[%{public}u * %{public}u], name: %{public}s, screenType: VIRTUAL_TYPE_SCREEN, whiteList size: %{public}zu}",
        configs.id, associatedScreenId_, configs.width, configs.height, configs.name.c_str(), configs.whiteList.size());
}

void RSScreen::VirtualScreenInit() noexcept
{
    hdrCapability_.formatCount = 0;
    for (auto item : supportedVirtualHDRFormats_) {
        hdrCapability_.formats.emplace_back(RS_TO_HDI_HDR_FORMAT_MAP[item]);
        ++hdrCapability_.formatCount;
    }
    property_.SetSupportedColorGamuts(supportedVirtualColorGamuts_);
}

void RSScreen::PhysicalScreenInit() noexcept
{
    auto id = property_.GetId();
    hdiScreen_ = HdiScreen::CreateHdiScreen(ScreenPhysicalId(id));
    if (hdiScreen_ == nullptr) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to CreateHdiScreens.", __func__, id);
        return;
    }

    hdiScreen_->Init();
    if (hdiScreen_->GetScreenSupportedModes(supportedModes_) < 0) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenSupportedModes.", __func__, id);
    } else {
        std::string logString;
        size_t modeIndex = 0;
        for (; modeIndex < supportedModes_.size(); ++modeIndex) {
            AppendFormat(logString, ";supportedMode[%zu]: %ux%u, refreshRate=%u, modeId=%d",
                         modeIndex, supportedModes_[modeIndex].width, supportedModes_[modeIndex].height,
                         supportedModes_[modeIndex].freshRate, supportedModes_[modeIndex].id);
        }
        RS_LOGI("%{public}s supportedModes, id %{public}" PRIu64 "%{public}s",  __func__, id, logString.c_str());
    }

    if (hdiScreen_->GetHDRCapabilityInfos(hdrCapability_) < 0) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetHDRCapabilityInfos.", __func__, id);
    }
    std::transform(hdrCapability_.formats.begin(), hdrCapability_.formats.end(),
                   back_inserter(supportedPhysicalHDRFormats_),
                   [](GraphicHDRFormat item) -> ScreenHDRFormat { return HDI_HDR_FORMAT_TO_RS_MAP[item]; });
    auto status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    if (MultiScreenParam::IsRsSetScreenPowerStatus() || id == 0) {
        HILOG_COMM_INFO("PhysicalScreenInit: RSScreen(id %{public}" PRIu64 ") start SetScreenPowerStatus to On", id);
        if (hdiScreen_->SetScreenPowerStatus(status) < 0) {
            HILOG_COMM_ERROR("PhysicalScreenInit: RSScreen(id %{public}" PRIu64 ") failed to SetScreenPowerStatus.",
                             id);
        } else {
            HILOG_COMM_INFO("PhysicalScreenInit: RSScreen(id %{public}" PRIu64 ") end SetScreenPowerStatus to On", id);
        }
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        property_.SetResolution(std::make_pair(activeMode->width, activeMode->height));
        property_.SetPhysicalModeParams(activeMode->width, activeMode->height, activeMode->freshRate);
        RS_LOGI("%{public}s activeMode, screenId:%{public}" PRIu64
                ", activeModeId: %{public}d, size:[%{public}u, %{public}u], activeRefreshRate: %{public}u",
                __func__, id, activeMode->id, activeMode->width, activeMode->height, activeMode->freshRate);
    }
    if (hdiScreen_->GetScreenPowerStatus(status) < 0) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenPowerStatus.", __func__, id);
        property_.SetPowerStatus(ScreenPowerStatus::INVALID_POWER_STATUS);
    } else {
        property_.SetPowerStatus(static_cast<ScreenPowerStatus>(status));
    }
    if (capability_.type == GraphicInterfaceType::GRAPHIC_DISP_INTF_MIPI) {
        property_.SetScreenType(RSScreenType::BUILT_IN_TYPE_SCREEN);
    } else {
        property_.SetScreenType(RSScreenType::EXTERNAL_TYPE_SCREEN);
    }
    ScreenCapabilityInit();
    InitDisplayPropertyForHardCursor();  

    auto outType = GraphicDisplayConnectionType::GRAPHIC_DISPLAY_CONNECTION_TYPE_INTERNAL;
    if (hdiScreen_->GetScreenConnectionType(outType) != 0) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenConnectionType.", __func__, id);
        property_.SetConnectionType(ScreenConnectionType::INVALID_DISPLAY_CONNECTION_TYPE);
    } else {
        property_.SetConnectionType(static_cast<ScreenConnectionType>(outType));
    }

    std::vector<GraphicColorGamut> supportedColorGamuts;
    if (hdiScreen_->GetScreenSupportedColorGamuts(supportedColorGamuts) != GRAPHIC_DISPLAY_SUCCESS) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenSupportedColorGamuts.", __func__, id);
    } else {
        int index = 0;
        for (auto item : supportedColorGamuts) {
            supportedPhysicalColorGamuts_.push_back(static_cast<ScreenColorGamut>(item));
            if (item == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB) {
                currentPhysicalColorGamutIdx_ = index;
            }
            ++index;
        }
    }
    property_.SetSupportedColorGamuts(supportedPhysicalColorGamuts_);
    backlightLevel_ = GetScreenBacklight();
    // Enable when an external screen is connected and the vsync rate doesn't match the active refresh rate.
    if (id != 0 && MultiScreenParam::IsSkipFrameByActiveRefreshRate()) {
        property_.SetSkipFrameOption(
            DEFAULT_SKIP_FRAME_INTERVAL, INVALID_EXPECTED_REFRESH_RATE, SKIP_FRAME_BY_ACTIVE_REFRESH_RATE);
    }
}

void RSScreen::ScreenCapabilityInit() noexcept
{
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return;
    }
    int32_t ret = hdiScreen_->GetScreenCapability(capability_);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        RS_LOGE("%{public}s: get display capability failed, ret is %{public}d, use the default"
            " display capability.", __func__, ret);
        capability_ = {
            .name = "test1",
            .type = GRAPHIC_DISP_INTF_HDMI,
            .phyWidth = 1921,
            .phyHeight = 1081,
            .supportLayers = 0,
            .virtualDispCount = 0,
            .supportWriteBack = true,
            .propertyCount = 0
        };
    }
}

ScreenId RSScreen::Id() const
{
    return property_.GetId();
}

ScreenId RSScreen::GetAssociatedScreenId() const
{
    return associatedScreenId_;
}

std::string RSScreen::Name() const
{
    return property_.Name();
}

uint32_t RSScreen::Width() const
{
    return property_.GetWidth();
}

uint32_t RSScreen::Height() const
{
    return property_.GetHeight();
}

uint32_t RSScreen::PhyWidth() const
{
    return property_.GetPhyWidth();
}

uint32_t RSScreen::PhyHeight() const
{
    return property_.GetPhyHeight();
}

void RSScreen::SetScreenOffset(int32_t offsetX, int32_t offsetY)
{
    RS_LOGI("%{public}s id: %{public}" PRIu64 ", offset: [%{public}d, %{public}d]",
        __func__, property_.GetId(), offsetX, offsetY);
    auto prop = property_.SetOffset(offsetX, offsetY);
    NotifyScreenPropertyChange(prop);
}

void RSScreen::SetScreenFrameGravity(int32_t gravity)
{
    UPDATE_PROPERTY(FrameGravity, gravity);
}

bool RSScreen::IsVirtual() const
{
    return property_.IsVirtual();
}

void RSScreen::WriteHisyseventEpsLcdInfo(GraphicDisplayModeInfo& activeMode)
{
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr != nullptr && frameRateMgr->IsLtpo()) {
        RS_LOGW_IF(DEBUG_SCREEN, "RSScreen LTPO mode");
        return;
    }
    static GraphicDisplayModeInfo modeInfo;
    if ((modeInfo.freshRate != activeMode.freshRate)
        || modeInfo.width != activeMode.width || modeInfo.height != activeMode.height) {
        RS_TRACE_NAME("RSScreen::WriteHisyseventEpsLcdInfo HiSysEventWrite");
        RSHiSysEvent::EventWrite(RSEventName::EPS_LCD_FREQ, RSEventType::RS_STATISTIC,
            "SOURCERATE", modeInfo.freshRate, "TARGETRATE", activeMode.freshRate, "WIDTH", activeMode.width,
            "HEIGHT", activeMode.height);
        modeInfo = activeMode;
    }
}

uint32_t RSScreen::SetActiveMode(uint32_t modeId)
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support SetActiveMode.", __func__);
        return StatusCode::VIRTUAL_SCREEN;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }

    if (modeId >= supportedModes_.size()) {
        HILOG_COMM_ERROR("SetActiveMode: set fails because the index is out of bounds.");
        return StatusCode::INVALID_ARGUMENTS;
    }
    RS_LOGW_IF(DEBUG_SCREEN, "RSScreen set active mode: %{public}u", modeId);
    int32_t selectModeId = supportedModes_[modeId].id;
    const auto& targetModeInfo = supportedModes_[modeId];
    RS_LOGD("%{public}s, ModeId:%{public}d->%{public}d, targetMode:[(%{public}u x %{public}u) %{public}u],"
        "CurMode:[(%{public}u x %{public}u) %{public}u]", __func__, modeId, selectModeId, targetModeInfo.width,
        targetModeInfo.height, targetModeInfo.freshRate, property_.GetPhyWidth(), property_.GetPhyHeight(),
        property_.GetRefreshRate());
    bool resolutionChanging = targetModeInfo.width != property_.GetPhyWidth() ||
                              targetModeInfo.height != property_.GetPhyHeight();
    auto preState = property_.GetState();
    if (resolutionChanging) {
        UPDATE_PROPERTY(State, ScreenState::DISABLED);
    }
    int32_t hdiErr = hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId));
    constexpr int32_t hdfErrNotSupport = -2;
    auto ret = StatusCode::SUCCESS;
    if (hdiErr < 0) {
        HILOG_COMM_ERROR("SetActiveMode: Hdi SetScreenMode fails.");
        if (hdiErr != hdfErrNotSupport) {
            if (resolutionChanging) {
                UPDATE_PROPERTY(State, preState);
            }
            return StatusCode::SET_RATE_ERROR;
        }

        // When HDI return HDF_ERR_NOT_SUPPORT, the hardware will downgrade to a lower resolution,
        // so it should not directly return, active mode info must be update
        ret = StatusCode::HDI_ERR_NOT_SUPPORT;
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        auto prop = property_.SetPhysicalModeParams(activeMode->width, activeMode->height, activeMode->freshRate);
        NotifyScreenPropertyChange(prop);
        WriteHisyseventEpsLcdInfo(activeMode.value());
        RS_LOGD("%{public}s screenId:%{public}" PRIu64
            ", activeModeId: %{public}d, size:[%{public}u, %{public}u], RefreshRate:[%{public}u]",
            __func__, property_.GetId(), activeMode->id, activeMode->width, activeMode->height, activeMode->freshRate);
    }
    if (resolutionChanging) {
        UpdateSamplingScale(activeMode->width, activeMode->height, property_.GetWidth(), property_.GetHeight());
        UPDATE_PROPERTY(State, preState);
    }
    return ret;
}

uint32_t RSScreen::GetActiveRefreshRate() const
{
    return property_.GetRefreshRate();
}

uint32_t RSScreen::SetScreenActiveRect(const GraphicIRect& activeRect)
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s failed: virtual screen not support", __func__);
        return StatusCode::HDI_ERROR;
    }
    if (hdiScreen_ == nullptr) {
        RS_LOGE("%{public}s failed: hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }
    if (activeRect.x < 0 || activeRect.y < 0 || activeRect.w <= 0 || activeRect.h <= 0 ||
        static_cast<uint32_t>(activeRect.x + activeRect.w) > property_.GetWidth() ||
        static_cast<uint32_t>(activeRect.y + activeRect.h) > property_.GetHeight()) {
        HILOG_COMM_WARN("SetScreenActiveRect failed:, for activeRect: "
            "(%{public}" PRId32 ", %{public}" PRId32 ", %{public}" PRId32 ", %{public}" PRId32 ")",
            activeRect.x, activeRect.y, activeRect.w, activeRect.h);
        return StatusCode::INVALID_ARGUMENTS;
    }

    RS_LOGI("%{public}s success, activeRect: (%{public}" PRId32 ", %{public}" PRId32 ", "
        "%{public}" PRId32 ", %{public}" PRId32 ")", __func__, activeRect.x, activeRect.y, activeRect.w, activeRect.h);
    GraphicIRect reviseRect = activeRect;
    RectI maskRect;
    if (!CalculateMaskRectAndReviseRect(activeRect, reviseRect, maskRect)) {
        HILOG_COMM_WARN("CalculateMaskRect failed or not need");
    }
    auto prop = property_.SetActiveRectOption(
        RectI(activeRect.x, activeRect.y, activeRect.w, activeRect.h),
        maskRect,
        RectI(reviseRect.x, reviseRect.y, reviseRect.w, reviseRect.h));
    NotifyScreenPropertyChange(prop);

    HILOG_COMM_INFO("SetScreenActiveRect success, reviseRect: (%{public}" PRId32 ", %{public}" PRId32 ", "
        "%{public}" PRId32 ", %{public}" PRId32 ")", reviseRect.x, reviseRect.y, reviseRect.w, reviseRect.h);
    return StatusCode::SUCCESS;
}

bool RSScreen::CalculateMaskRectAndReviseRect(const GraphicIRect& activeRect, GraphicIRect& reviseRect, RectI& maskRect)
{
#ifdef ROSEN_EMULATOR
    RS_LOGD("%{public}s emulator device do not revise rect", __func__);
    return false;
#endif
    if (!RSSystemProperties::IsSuperFoldDisplay()) {
        RS_LOGE("device is not super fold display");
        return false;
    }
    if (activeRect.w > 0 && activeRect.h > 0) {
        // neet tobe configuration item
        static RectI rect[2] = {{0, 0, 0, 0}, {0, 1008, 2232, 128}};
        maskRect = (static_cast<uint32_t>(activeRect.h) == property_.GetHeight()) ? rect[0] : rect[1];
        // Take the minimum rectangular area containing two rectangles
        reviseRect.x = std::clamp(activeRect.x, 0, std::min(activeRect.x, maskRect.left_));
        reviseRect.y = std::clamp(activeRect.y, 0, std::min(activeRect.y, maskRect.top_));
        reviseRect.w = std::max(activeRect.x + activeRect.w, maskRect.left_ + maskRect.width_) - reviseRect.x;
        reviseRect.h = std::max(activeRect.y + activeRect.h, maskRect.top_ + maskRect.height_) - reviseRect.y;
        RS_LOGI("%{public}s success, maskRect: %{public}s", __func__, maskRect.ToString().c_str());
        return true;
    }
    return false;
}

int32_t RSScreen::SetResolution(uint32_t width, uint32_t height)
{
    auto phyWidth = property_.GetPhyWidth();
    auto phyHeight = property_.GetPhyHeight();
    HILOG_COMM_INFO("%{public}s screenId: %{public}" PRIu64 " width: %{public}u height: %{public}u,"
                    "phyWidth: %{public}u, phyHeight: %{public}u.",
                    __func__, property_.GetId(), width, height, phyWidth, phyHeight);
    if (IsVirtual()) {
        UPDATE_PROPERTY(Resolution, std::make_pair(width, height));
        return StatusCode::SUCCESS;
    }
    bool isValidArgs = (width > 0 && width <= phyWidth && height > 0 && height <= phyHeight) ||
                       (width >= phyWidth && height >= phyHeight);
    if (!isValidArgs) {
        HILOG_COMM_ERROR("%{public}s invalid arguments width: %{public}u, height: %{public}u", __func__, width, height);
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }

    if (width <= phyWidth && height <= phyHeight) {
        // smaller size to set rog resolution
        if (hdiScreen_->SetScreenOverlayResolution(width, height) < 0) {
            RS_LOGE("%{public}s: hdi set screen rog resolution failed.", __func__);
            return StatusCode::HDI_ERROR;
        }
    }
    UPDATE_PROPERTY(Resolution, std::make_pair(width, height));
    UpdateSamplingScale(phyWidth, phyHeight, width, height);
    return StatusCode::SUCCESS;
}

void RSScreen::UpdateSamplingScale(uint32_t phyWidth, uint32_t phyHeight, uint32_t width, uint32_t height)
{
    bool isSamplingOn = (width >= phyWidth && height >= phyHeight) && !(width == phyWidth && height == phyHeight);
    float samplingScale = 1.f;
    float translateX = 0.f;
    float translateY = 0.f;
    if (isSamplingOn) {
        samplingScale = std::min(static_cast<float>(phyWidth) / width, static_cast<float>(phyHeight) / height);
        translateX = (phyWidth - width * samplingScale) / 2.f;
        translateY = (phyHeight - height * samplingScale) / 2.f;
        HILOG_COMM_INFO("%{public}s: sampling is enabled. "
            "scale: %{public}f, translateX: %{public}f, translateY: %{public}f",
            __func__, samplingScale, translateX, translateY);
    }
    auto prop = property_.SetSamplingOption(isSamplingOn, translateX, translateY, samplingScale);
    NotifyScreenPropertyChange(prop);
}

int32_t RSScreen::GetActiveModePosByModeId(int32_t modeId) const
{
    size_t modeIndex = 0;
    for (; modeIndex < supportedModes_.size(); ++modeIndex) {
        if (supportedModes_[modeIndex].id == modeId) {
            return static_cast<int32_t>(modeIndex);
        }
    }
    return -1;
}

int32_t RSScreen::SetPowerStatus(uint32_t powerStatus)
{
    if (!hdiScreen_) {
        RS_LOGE("[UL_POWER] %{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }
    if (IsVirtual()) {
        RS_LOGW("[UL_POWER] %{public}s: virtual screen not support SetPowerStatus.", __func__);
        return StatusCode::HDI_ERROR;
    }

    auto id = property_.GetId();
    HILOG_COMM_WARN("[UL_POWER]RSScreen_%{public}" PRIu64 " SetPowerStatus: %{public}u.", id, powerStatus);
    RS_TRACE_NAME_FMT("[UL_POWER]Screen_%llu SetPowerStatus %u", id, powerStatus);
    hasLogBackLightAfterPowerStatusChanged_ = false;
    if (hdiScreen_->SetScreenPowerStatus(static_cast<GraphicDispPowerStatus>(powerStatus)) < 0) {
        HILOG_COMM_WARN("[UL_POWER] SetPowerStatus failed to set power status");
        UPDATE_PROPERTY(PowerStatus, ScreenPowerStatus::INVALID_POWER_STATUS);
        return StatusCode::HDI_ERROR;
    }

    UPDATE_PROPERTY(PowerStatus, static_cast<ScreenPowerStatus>(powerStatus));
    HILOG_COMM_WARN("[UL_POWER]RSScreen_%{public}" PRIu64 " SetPowerStatus: %{public}u done.", id, powerStatus);
    return StatusCode::SUCCESS;
}

std::optional<GraphicDisplayModeInfo> RSScreen::GetActiveMode() const
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support GetActiveMode.", __func__);
        return {};
    }

    uint32_t modeId = 0;

    if (hdiScreen_ == nullptr) {
        RS_LOGE("%{public}s: id: %{public}" PRIu64 " hdiScreen is null.", __func__, property_.GetId());
        return {};
    }

    if (hdiScreen_->GetScreenMode(modeId) < 0) {
        RS_LOGE("%{public}s: id: %{public}" PRIu64 " GetScreenMode failed.", __func__, property_.GetId());
        return {};
    }

    auto iter = std::find_if(supportedModes_.cbegin(), supportedModes_.cend(),
        [modeId](const auto &mode) { return static_cast<uint32_t>(mode.id) == modeId; });
    if (iter == supportedModes_.cend()) {
        return {};
    }

    return *iter;
}

const std::vector<GraphicDisplayModeInfo>& RSScreen::GetSupportedModes() const
{
    return supportedModes_;
}

const GraphicDisplayCapability& RSScreen::GetCapability() const
{
    return capability_;
}

ScreenPowerStatus RSScreen::GetPowerStatus()
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support GetPowerStatus.", __func__);
        return ScreenPowerStatus::INVALID_POWER_STATUS;
    }

    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return INVALID_POWER_STATUS;
    }

    auto curPowerStatus = property_.GetPowerStatus();
    if (curPowerStatus != ScreenPowerStatus::INVALID_POWER_STATUS) {
        return curPowerStatus;
    }

    auto status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    if (hdiScreen_->GetScreenPowerStatus(status) < 0) {
        RS_LOGE("%{public}s failed to get screen powerStatus", __func__);
        return INVALID_POWER_STATUS;
    }
    UPDATE_PROPERTY(PowerStatus, static_cast<ScreenPowerStatus>(status));
    RS_LOGW("%{public}s cached powerStatus is INVALID_POWER_STATUS and GetScreenPowerStatus %{public}u",
        __func__, static_cast<uint32_t>(status));
    return static_cast<ScreenPowerStatus>(status);
}

sptr<Surface> RSScreen::GetProducerSurface() const
{
    return property_.GetProducerSurface();
}

void RSScreen::SetProducerSurface(sptr<Surface> producerSurface)
{
    UPDATE_PROPERTY(ProducerSurface, producerSurface);
    if (producerSurface) {
        UPDATE_PROPERTY(State, ScreenState::SOFTWARE_OUTPUT_ENABLE);
    } else {
        UPDATE_PROPERTY(State, ScreenState::DISABLED);
    }
}

void RSScreen::ModeInfoDump(std::string& dumpString)
{
    size_t modeIndex = 0;
    for (; modeIndex < supportedModes_.size(); ++modeIndex) {
        AppendFormat(dumpString, "supportedMode[%d]: %dx%d, refreshRate=%d\n",
                     modeIndex, supportedModes_[modeIndex].width,
                     supportedModes_[modeIndex].height, supportedModes_[modeIndex].freshRate);
    }
    std::optional<GraphicDisplayModeInfo> activeMode = GetActiveMode();
    if (activeMode) {
        AppendFormat(dumpString, "activeMode: %dx%d, refreshRate=%d\n",
            activeMode->width, activeMode->height, activeMode->freshRate);
    }
}

void RSScreen::CapabilityTypeDump(GraphicInterfaceType capabilityType, std::string& dumpString)
{
    dumpString += "type=";
    switch (capabilityType) {
        case GRAPHIC_DISP_INTF_HDMI: {
            dumpString += "DISP_INTF_HDMI, ";
            break;
        }
        case GRAPHIC_DISP_INTF_LCD: {
            dumpString += "DISP_INTF_LCD, ";
            break;
        }
        case GRAPHIC_DISP_INTF_BT1120: {
            dumpString += "DISP_INTF_BT1120, ";
            break;
        }
        case GRAPHIC_DISP_INTF_BT656: {
            dumpString += "DISP_INTF_BT656, ";
            break;
        }
        default:
            dumpString += "INVALID_DISP_INTF, ";
            break;
    }
}

void RSScreen::CapabilityDump(std::string& dumpString)
{
    AppendFormat(dumpString, "name=%s, phyWidth=%d, phyHeight=%d, "
                 "supportLayers=%d, virtualDispCount=%d, propertyCount=%d, ",
                 capability_.name.c_str(), capability_.phyWidth, capability_.phyHeight,
                 capability_.supportLayers, capability_.virtualDispCount, capability_.propertyCount);
    CapabilityTypeDump(capability_.type, dumpString);
    dumpString += "supportWriteBack=";
    dumpString += (capability_.supportWriteBack) ? "true" : "false";
    dumpString += "\n";
    PropDump(dumpString);
}

void RSScreen::PropDump(std::string& dumpString)
{
    decltype(capability_.propertyCount) propIndex = 0;
    for (; propIndex < capability_.propertyCount; ++propIndex) {
        AppendFormat(dumpString, "prop[%u]: name=%s, propid=%d, value=%d\n",
                     propIndex, capability_.props[propIndex].name.c_str(), capability_.props[propIndex].propId,
                     capability_.props[propIndex].value);
    }
}

void RSScreen::PowerStatusDump(std::string& dumpString)
{
    dumpString += "powerStatus=";
    switch (GetPowerStatus()) {
        case GRAPHIC_POWER_STATUS_ON: {
            dumpString += "POWER_STATUS_ON";
            break;
        }
        case GRAPHIC_POWER_STATUS_STANDBY: {
            dumpString += "POWER_STATUS_STANDBY";
            break;
        }
        case GRAPHIC_POWER_STATUS_SUSPEND: {
            dumpString += "POWER_STATUS_SUSPEND";
            break;
        }
        case GRAPHIC_POWER_STATUS_OFF: {
            dumpString += "POWER_STATUS_OFF";
            break;
        }
        case GRAPHIC_POWER_STATUS_OFF_FAKE: {
            dumpString += "POWER_STATUS_OFF_FAKE";
            break;
        }
        case GRAPHIC_POWER_STATUS_ON_ADVANCED: {
            dumpString += "POWER_STATUS_ON_ADVANCED";
            break;
        }
        case GRAPHIC_POWER_STATUS_OFF_ADVANCED: {
            dumpString += "POWER_STATUS_OFF_ADVANCED";
            break;
        }
        case GRAPHIC_POWER_STATUS_DOZE: {
            dumpString += "POWER_STATUS_DOZE";
            break;
        }
        case GRAPHIC_POWER_STATUS_DOZE_SUSPEND: {
            dumpString += "POWER_STATUS_DOZE_SUSPEND";
            break;
        }
        case GRAPHIC_POWER_STATUS_BUTT: {
            dumpString += "POWER_STATUS_BUTT";
            break;
        }
        default: {
            dumpString += "INVALID_POWER_STATUS";
            break;
        }
    }
}

void RSScreen::DisplayDump(int32_t screenIndex, std::string& dumpString)
{
    dumpString += "-- ScreenInfo\n";
    if (IsVirtual()) {
        dumpString += "screen[" + std::to_string(screenIndex) + "]: ";
        dumpString += "id=";
        dumpString += (property_.GetId() == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID"
                                                               : std::to_string(property_.GetId());
        dumpString += ", ";
        dumpString += "associatedScreenId=";
        dumpString += (associatedScreenId_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID"
                                                                 : std::to_string(associatedScreenId_);
        dumpString += ", ";
        AppendFormat(dumpString, ", render resolution=%dx%d, isVirtual=true, skipFrameInterval=%d"
            ", expectedRefreshRate=%d, skipFrameStrategy=%d\n",
            property_.GetWidth(), property_.GetHeight(), property_.GetSkipFrameInterval(),
            property_.GetExpectedRefreshRate(), property_.GetSkipFrameStrategy());
    } else {
        dumpString += "screen[" + std::to_string(screenIndex) + "]: ";
        dumpString += "id=";
        dumpString += (property_.GetId() == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID"
                                                               : std::to_string(property_.GetId());
        dumpString += ", ";
        PowerStatusDump(dumpString);
        dumpString += ", ";
        dumpString += "backlight=" + std::to_string(GetScreenBacklight());
        dumpString += ", ";
        ScreenTypeDump(dumpString);
        AppendFormat(dumpString,
            ", render resolution=%dx%d, physical resolution=%dx%d, isVirtual=false, skipFrameInterval=%d"
            ", expectedRefreshRate=%d, skipFrameStrategy=%d\n",
            property_.GetWidth(), property_.GetHeight(), property_.GetPhyWidth(), property_.GetPhyHeight(),
            property_.GetSkipFrameInterval(), property_.GetExpectedRefreshRate(), property_.GetSkipFrameStrategy());
        ModeInfoDump(dumpString);
        CapabilityDump(dumpString);
        AppendFormat(dumpString,
                     "isSamplingOn=%d, samplingScale=%.2f, samplingTranslateX=%.2f, samplingTranslateY=%.2f\n",
                     property_.GetIsSamplingOn(), property_.GetSamplingScale(),
                     property_.GetSamplingTranslateX(), property_.GetSamplingTranslateY());
        AppendFormat(dumpString, "enableVisibleRect=%d, mainScreenVisibleRect=[%d,%d,%d,%d]\n",
                     property_.GetEnableVisibleRect(),
                     property_.GetMainScreenVisibleRect().x, property_.GetMainScreenVisibleRect().y,
                     property_.GetMainScreenVisibleRect().w, property_.GetMainScreenVisibleRect().h);
    }
}

void RSScreen::ScreenTypeDump(std::string& dumpString)
{
    dumpString += "screenType=";
    switch (property_.GetScreenType()) {
        case RSScreenType::BUILT_IN_TYPE_SCREEN: {
            dumpString += "BUILT_IN_TYPE";
            break;
        }
        case RSScreenType::EXTERNAL_TYPE_SCREEN: {
            dumpString += "EXTERNAL_TYPE";
            break;
        }
        case RSScreenType::VIRTUAL_TYPE_SCREEN: {
            dumpString += "VIRTUAL_TYPE";
            break;
        }
        default: {
            dumpString += "UNKNOWN_TYPE";
            break;
        }
    }
}

void RSScreen::ResizeVirtualScreen(uint32_t width, uint32_t height)
{
    if (width == property_.GetWidth() && height == property_.GetHeight()) {
        return;
    }
    if (!IsVirtual()) {
        RS_LOGW("%{public}s: physical screen not support ResizeVirtualScreen.", __func__);
        return;
    }
    UPDATE_PROPERTY(Resolution, std::make_pair(width, height));
}

void RSScreen::SetScreenBacklight(uint32_t level)
{
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return;
    }
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support SetScreenBacklight.", __func__);
        return;
    }
    auto id = property_.GetId();
    if (!hasLogBackLightAfterPowerStatusChanged_) {
        HILOG_COMM_INFO("SetScreenBacklight id: %{public}" PRIu64 ", level is %{public}u, current level is %{public}d",
                        id, level, backlightLevel_.load());
    }

    RS_LOGD("%{public}s id: %{public}" PRIu64 ", level is %{public}u", __func__, id, level);

    if (onBackLightChange_) {
        onBackLightChange_(property_.GetId(), level);
    }
    if (!hasLogBackLightAfterPowerStatusChanged_) {
        HILOG_COMM_INFO("SetScreenBacklight id: %{public}" PRIu64 ", level %{public}u done, last level is %{public}d",
            id, level, backlightLevel_.load());
        hasLogBackLightAfterPowerStatusChanged_ = true;
    }
    backlightLevel_.store(static_cast<int32_t>(level));
}

int32_t RSScreen::GetScreenBacklight() const
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support GetScreenBacklight.", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }
    if (backlightLevel_ != INVALID_BACKLIGHT_VALUE) {
        return backlightLevel_;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }
    uint32_t level = 0;
    if (hdiScreen_->GetScreenBacklight(level) < 0) {
        RS_LOGE("%{public}s failed, level is invalid", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }
    return static_cast<int32_t>(level);
}

int32_t RSScreen::GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut> &mode) const
{
    mode.clear();
    if (IsVirtual()) {
        mode = supportedVirtualColorGamuts_;
    } else {
        mode = supportedPhysicalColorGamuts_;
    }
    if (mode.size() == 0) {
        return StatusCode::HDI_ERROR;
    }
    return StatusCode::SUCCESS;
}

int32_t RSScreen::GetScreenSupportedMetaDataKeys(std::vector<ScreenHDRMetadataKey> &keys) const
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support GetScreenSupportedMetaDataKeys.", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }

    // ScreenHDRMetadataKey now is mock data.
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_X);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_Y);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_GREEN_PRIMARY_X);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_GREEN_PRIMARY_Y);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_BLUE_PRIMARY_X);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_BLUE_PRIMARY_Y);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_WHITE_PRIMARY_X);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_WHITE_PRIMARY_Y);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_MAX_LUMINANCE);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_MIN_LUMINANCE);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_MAX_CONTENT_LIGHT_LEVEL);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_MAX_FRAME_AVERAGE_LIGHT_LEVEL);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_HDR10_PLUS);
    keys.push_back(ScreenHDRMetadataKey::MATAKEY_HDR_VIVID);
    return StatusCode::SUCCESS;
}

int32_t RSScreen::GetScreenColorGamut(ScreenColorGamut &mode) const
{
    if (IsVirtual()) {
        mode = supportedVirtualColorGamuts_[currentVirtualColorGamutIdx_];
        return StatusCode::SUCCESS;
    }
    if (supportedPhysicalColorGamuts_.size() == 0) {
        RS_LOGE("%{public}s failed", __func__);
        return StatusCode::HDI_ERROR;
    }
    mode = supportedPhysicalColorGamuts_[currentPhysicalColorGamutIdx_];
    return StatusCode::SUCCESS;
}

int32_t RSScreen::SetScreenColorGamut(int32_t modeIdx)
{
    if (modeIdx < 0) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (IsVirtual()) {
        if (modeIdx >= static_cast<int32_t>(supportedVirtualColorGamuts_.size())) {
            return StatusCode::INVALID_ARGUMENTS;
        }
        currentVirtualColorGamutIdx_ = modeIdx;
        UPDATE_PROPERTY(ScreenColorGamut, supportedVirtualColorGamuts_[modeIdx]);
        return StatusCode::SUCCESS;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }
    if (modeIdx >= static_cast<int32_t>(supportedPhysicalColorGamuts_.size())) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    auto screenColorGamut = supportedPhysicalColorGamuts_[modeIdx];
    int32_t result = hdiScreen_->SetScreenColorGamut(static_cast<GraphicColorGamut>(screenColorGamut));
    if (result != GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::HDI_ERROR;
    }
    currentPhysicalColorGamutIdx_ = modeIdx;
    UPDATE_PROPERTY(ScreenColorGamut, screenColorGamut);
    return StatusCode::SUCCESS;
}

void RSScreen::SetScreenCorrection(ScreenRotation screenRotation)
{
    RS_LOGI("%{public}s: RSScreen(id %{public}" PRIu64 "), ScreenRotation: %{public}u.", __func__,
        property_.GetId(), static_cast<uint32_t>(screenRotation));
    UPDATE_PROPERTY(ScreenCorrection, screenRotation);
}

int32_t RSScreen::SetScreenGamutMap(ScreenGamutMap mode)
{
    if (IsVirtual()) {
        UPDATE_PROPERTY(ScreenGamutMap, mode);
        return StatusCode::SUCCESS;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }
    int32_t result = hdiScreen_->SetScreenGamutMap(static_cast<GraphicGamutMap>(mode));
    if (result == GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::SUCCESS;
    }
    UPDATE_PROPERTY(ScreenGamutMap, mode);
    return StatusCode::HDI_ERROR;
}

int32_t RSScreen::GetScreenGamutMap(ScreenGamutMap &mode) const
{
    if (IsVirtual()) {
        mode = property_.GetScreenGamutMap();
        return StatusCode::SUCCESS;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }
    GraphicGamutMap hdiMode;
    int32_t result = hdiScreen_->GetScreenGamutMap(hdiMode);
    if (result == GRAPHIC_DISPLAY_SUCCESS) {
        mode = static_cast<ScreenGamutMap>(hdiMode);
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

const GraphicHDRCapability& RSScreen::GetHDRCapability()
{
    hdrCapability_.maxLum = MAX_LUM; // mock data
    return hdrCapability_;
}

RSScreenType RSScreen::GetScreenType() const
{
    return property_.GetScreenType();
}

ScreenConnectionType RSScreen::GetConnectionType() const
{
    return property_.GetConnectionType();
}

void RSScreen::SetScreenSkipFrameInterval(uint32_t skipFrameInterval)
{
    auto prop = property_.SetSkipFrameOption(skipFrameInterval, INVALID_EXPECTED_REFRESH_RATE, SKIP_FRAME_BY_INTERVAL);
    NotifyScreenPropertyChange(prop);
}

void RSScreen::SetScreenExpectedRefreshRate(uint32_t expectedRefreshRate)
{
    auto prop = property_.SetSkipFrameOption(
        DEFAULT_SKIP_FRAME_INTERVAL, expectedRefreshRate, SKIP_FRAME_BY_REFRESH_RATE);
    NotifyScreenPropertyChange(prop);
}

void RSScreen::SetScreenVsyncEnabled(bool enabled) const
{
    if (IsVirtual()) {
        return;
    }
    if (hdiScreen_ != nullptr) {
        hdiScreen_->SetScreenVsyncEnabled(enabled);
    }
}

bool RSScreen::SetVirtualMirrorScreenCanvasRotation(bool canvasRotation)
{
    if (!IsVirtual()) {
        return false;
    }

    UPDATE_PROPERTY(CanvasRotation, canvasRotation);
    return true;
}

bool RSScreen::GetCanvasRotation() const
{
    return property_.GetCanvasRotation();
}

int32_t RSScreen::SetVirtualScreenAutoRotation(bool isAutoRotation)
{
    if (!IsVirtual()) {
        return StatusCode::INVALID_ARGUMENTS;
    }

    UPDATE_PROPERTY(AutoBufferRotation, isAutoRotation);
    return StatusCode::SUCCESS;
}

bool RSScreen::GetVirtualScreenAutoRotation() const
{
    return property_.GetAutoBufferRotation();
}

bool RSScreen::SetVirtualMirrorScreenScaleMode(ScreenScaleMode scaleMode)
{
    if (!IsVirtual()) {
        return false;
    }

    UPDATE_PROPERTY(ScreenScaleMode, scaleMode);
    return true;
}

ScreenScaleMode RSScreen::GetScaleMode() const
{
    return property_.GetScreenScaleMode();
}

int32_t RSScreen::GetScreenSupportedHDRFormats(std::vector<ScreenHDRFormat>& hdrFormats) const
{
    hdrFormats.clear();
    if (IsVirtual()) {
        hdrFormats = supportedVirtualHDRFormats_;
    } else {
        hdrFormats = supportedPhysicalHDRFormats_;
    }
    if (hdrFormats.size() == 0) {
        return StatusCode::HDI_ERROR;
    }
    return StatusCode::SUCCESS;
}

int32_t RSScreen::GetScreenHDRFormat(ScreenHDRFormat& hdrFormat) const
{
    if (IsVirtual()) {
        hdrFormat = supportedVirtualHDRFormats_[currentVirtualHDRFormatIdx_];
        return StatusCode::SUCCESS;
    } else {
        if (supportedPhysicalHDRFormats_.size() == 0) {
            return StatusCode::HDI_ERROR;
        }
        hdrFormat = supportedPhysicalHDRFormats_[currentPhysicalHDRFormatIdx_];
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

int32_t RSScreen::SetScreenHDRFormat(int32_t modeIdx)
{
    if (modeIdx < 0) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    if (IsVirtual()) {
        if (modeIdx >= static_cast<int32_t>(supportedVirtualHDRFormats_.size())) {
            return StatusCode::INVALID_ARGUMENTS;
        }
        currentVirtualHDRFormatIdx_ = modeIdx;
        UPDATE_PROPERTY(ScreenHDRFormat, supportedVirtualHDRFormats_[modeIdx]);
        return StatusCode::SUCCESS;
    } else {
        // There should be some hdi operation
        if (modeIdx >= static_cast<int32_t>(hdrCapability_.formats.size())) {
            return StatusCode::INVALID_ARGUMENTS;
        }
        currentPhysicalHDRFormatIdx_ = modeIdx;
        UPDATE_PROPERTY(ScreenHDRFormat, supportedPhysicalHDRFormats_[modeIdx]);
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

int32_t RSScreen::GetPixelFormat(GraphicPixelFormat& pixelFormat) const
{
    pixelFormat = property_.GetPixelFormat();
    return StatusCode::SUCCESS;
}

int32_t RSScreen::SetPixelFormat(GraphicPixelFormat pixelFormat)
{
    UPDATE_PROPERTY(PixelFormat, pixelFormat);
    return StatusCode::SUCCESS;
}

int32_t RSScreen::GetScreenSupportedColorSpaces(std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const
{
    colorSpaces.clear();
    if (IsVirtual()) {
        std::transform(supportedVirtualColorGamuts_.begin(), supportedVirtualColorGamuts_.end(),
                       back_inserter(colorSpaces),
                       [](ScreenColorGamut item) -> GraphicCM_ColorSpaceType {
                            return RS_TO_COMMON_COLOR_SPACE_TYPE_MAP[static_cast<GraphicColorGamut>(item)];
                        });
    } else {
        std::transform(supportedPhysicalColorGamuts_.begin(), supportedPhysicalColorGamuts_.end(),
                       back_inserter(colorSpaces),
                       [](ScreenColorGamut item) -> GraphicCM_ColorSpaceType {
                            return RS_TO_COMMON_COLOR_SPACE_TYPE_MAP[static_cast<GraphicColorGamut>(item)];
                        });
    }
    if (colorSpaces.size() == 0) {
        return StatusCode::HDI_ERROR;
    }
    return StatusCode::SUCCESS;
}

int32_t RSScreen::GetScreenColorSpace(GraphicCM_ColorSpaceType& colorSpace) const
{
    ScreenColorGamut curGamut;
    int32_t result = GetScreenColorGamut(curGamut);
    colorSpace = RS_TO_COMMON_COLOR_SPACE_TYPE_MAP[static_cast<GraphicColorGamut>(curGamut)];
    return result;
}

int32_t RSScreen::SetScreenColorSpace(GraphicCM_ColorSpaceType colorSpace)
{
    auto iter = COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.find(colorSpace);
    if (iter == COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end()) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    ScreenColorGamut dstColorGamut = static_cast<ScreenColorGamut>(iter->second);
    if (IsVirtual()) {
        auto it = std::find(supportedVirtualColorGamuts_.begin(), supportedVirtualColorGamuts_.end(), dstColorGamut);
        if (it == supportedVirtualColorGamuts_.end()) {
            return StatusCode::INVALID_ARGUMENTS;
        }
        currentVirtualColorGamutIdx_ = std::distance(supportedVirtualColorGamuts_.begin(), it);
        UPDATE_PROPERTY(ScreenColorGamut, dstColorGamut);
        return StatusCode::SUCCESS;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }

    auto it = std::find(supportedPhysicalColorGamuts_.begin(), supportedPhysicalColorGamuts_.end(), dstColorGamut);
    if (it == supportedPhysicalColorGamuts_.end()) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    int32_t result = hdiScreen_->SetScreenColorGamut(static_cast<GraphicColorGamut>(dstColorGamut));
    if (result != GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::HDI_ERROR;
    }
    currentPhysicalColorGamutIdx_ = std::distance(supportedPhysicalColorGamuts_.begin(), it);
    UPDATE_PROPERTY(ScreenColorGamut, dstColorGamut);
    return StatusCode::SUCCESS;
}

std::unordered_set<uint64_t> RSScreen::GetWhiteList() const
{
    return property_.GetWhiteList();
}

void RSScreen::SetBlackList(const std::unordered_set<uint64_t>& blackList)
{
    UPDATE_PROPERTY(BlackList, blackList);
}

void RSScreen::SetTypeBlackList(const std::unordered_set<uint8_t>& typeBlackList)
{
    UPDATE_PROPERTY(TypeBlackList, typeBlackList);
}

void RSScreen::AddBlackList(const std::vector<uint64_t>& blackList)
{
    if (blackList.empty()) {
        return;
    }
    auto prop = property_.AddBlackList(blackList);
    NotifyScreenPropertyChange(prop);
}

void RSScreen::RemoveBlackList(const std::vector<uint64_t>& blackList)
{
    if (blackList.empty()) {
        return;
    }
    auto prop = property_.RemoveBlackList(blackList);
    NotifyScreenPropertyChange(prop);
}

void RSScreen::SetCastScreenEnableSkipWindow(bool enable)
{
    UPDATE_PROPERTY(CastScreenEnableSkipWindow, enable);
}

std::unordered_set<uint64_t> RSScreen::GetBlackList() const
{
    return property_.GetBlackList();
}

int32_t RSScreen::SetScreenConstraint(uint64_t frameId, uint64_t timestamp, ScreenConstraintType type)
{
    if (IsVirtual()) {
        return StatusCode::SUCCESS;
    }
    if (hdiScreen_ != nullptr) {
        int32_t result = hdiScreen_->SetScreenConstraint(frameId, timestamp, static_cast<uint32_t>(type));
        if (result == GRAPHIC_DISPLAY_SUCCESS) {
            return StatusCode::SUCCESS;
        }
    }
    return StatusCode::HDI_ERROR;
}

void RSScreen::SetSecurityExemptionList(const std::vector<uint64_t>& securityExemptionList)
{
    UPDATE_PROPERTY(SecurityExemptionList, securityExemptionList);
}

int32_t RSScreen::SetSecurityMask(std::shared_ptr<Media::PixelMap> securityMask)
{
    UPDATE_PROPERTY(SecurityMask, securityMask);
    return SUCCESS;
}

void RSScreen::SetVisibleRectOption(bool enable, const Rect& mainScreenRect, bool supportRotation)
{
    auto prop = property_.SetVisibleRectOption(enable, mainScreenRect, supportRotation);
    NotifyScreenPropertyChange(prop);
}

bool RSScreen::SetVirtualScreenStatus(VirtualScreenStatus screenStatus)
{
    if (!IsVirtual()) {
        return false;
    }

    UPDATE_PROPERTY(ScreenStatus, screenStatus);
    return true;
}

void RSScreen::InitDisplayPropertyForHardCursor()
{
    if (!hdiScreen_) {
        return;
    }

    bool isHardCursorSupport = hdiScreen_->GetDisplayPropertyForHardCursor(property_.GetId());
    property_.SetIsHardCursorSupport(isHardCursorSupport);
    RS_LOGI("%{public}s, RSScreen(id %{public}" PRIu64 ", isHardCursorSupport:%{public}d)",
        __func__, property_.GetId(), property_.GetIsHardCursorSupport());
}

int32_t RSScreen::GetDisplayIdentificationData(uint8_t& outPort, std::vector<uint8_t>& edidData) const
{
    if (hdiScreen_ == nullptr) {
        RS_LOGE("%{public}s: id: %{public}" PRIu64 " is not physical display", __func__, property_.GetId());
        return HDI_ERROR;
    }

    int32_t ret = hdiScreen_->GetDisplayIdentificationData(outPort, edidData);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        RS_LOGE("%{public}s: id %{public}" PRIu64 " call hid interface failed(%{public}d)",
            __func__, property_.GetId(), ret);
        return HDI_ERROR;
    }
    RS_LOGD("%{public}s:: EdidSize: %{public}zu", __func__, edidData.size());
    return SUCCESS;
}

int32_t RSScreen::SetScreenLinearMatrix(const std::vector<float> &matrix)
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support SetScreenLinearMatrix.", __func__);
        return StatusCode::VIRTUAL_SCREEN;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }
    if (linearMatrix_ == matrix) {
        return StatusCode::SUCCESS;
    }
    if (hdiScreen_->SetScreenLinearMatrix(matrix) < 0) {
        RS_LOGI("%{public}s failed, matrix is invalid", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }

    linearMatrix_ = matrix;
    return StatusCode::SUCCESS;
}

sptr<RSScreenProperty> RSScreen::GetProperty() const
{
    return property_.Clone();
}

ScreenInfo RSScreen::GetScreenInfo() const
{
    return property_.GetScreenInfo();
}

void RSScreen::SetOnPropertyChangedCallback(OnPropertyChangeCallback callback)
{
    onPropertyChange_ = std::move(callback);
}

void RSScreen::SetOnBacklightChangedCallback(std::function<void(ScreenId, uint32_t)> callback)
{
    onBackLightChange_ = callback;
}

void RSScreen::SetDisablePowerOffRenderControl(bool disable)
{
    UPDATE_PROPERTY(DisablePowerOffRenderControl, disable);
}

void RSScreen::SetScreenSwitchStatus(bool status)
{
    UPDATE_PROPERTY(ScreenSwitchStatus, status);
}

void RSScreen::NotifyScreenPropertyChange(std::pair<ScreenPropertyType, const sptr<ScreenPropertyBase>&> prop)
{
    if (onPropertyChange_ && prop.second) {
        onPropertyChange_(property_.GetId(), prop.first, prop.second);
    }
}
} // namespace Rosen
} // namespace OHOS
