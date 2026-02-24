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
    {GRAPHIC_COLOR_GAMUT_NATIVE, GRAPHIC_CM_COLORSPACE_NONE},
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
    {GRAPHIC_CM_COLORSPACE_NONE, GRAPHIC_COLOR_GAMUT_NATIVE},
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

RSScreen::RSScreen(std::shared_ptr<HdiOutput> output) : hdiOutput_(std::move(output))
{
    ScreenId id = hdiOutput_ ? ToScreenId(hdiOutput_->GetScreenId()) : INVALID_SCREEN_ID;
    property_.SetId(id);
    property_.SetIsVirtual(false);
    hdrCapability_.formatCount = 0;
    std::string name = "Screen_" + std::to_string(id);
    property_.SetName(name);
    if (hdiOutput_) {
        property_.SetState(ScreenState::HDI_OUTPUT_ENABLE);
    } else {
        property_.SetState(ScreenState::DISABLED);
    }
    PhysicalScreenInit();
    HILOG_COMM_WARN("init physical: {id: %{public}" PRIu64 ", w * h: [%{public}u * %{public}u], "
        "screenType: %{public}u}", id, property_.GetWidth(), property_.GetHeight(), property_.GetScreenType());

    capability_.props.clear();
}

RSScreen::RSScreen(const VirtualScreenConfigs& configs)
{
    property_.SetIsVirtual(true);
    property_.SetId(configs.id);
    associatedScreenId_ = configs.associatedScreenId;
    property_.SetName(configs.name);
    property_.SetWidth(configs.width);
    property_.SetHeight(configs.height);
    property_.SetVirtualSecLayerOption(configs.flags);
    property_.SetProducerSurface(configs.surface);
    property_.SetPixelFormat(configs.pixelFormat);
    property_.SetScreenType(RSScreenType::VIRTUAL_TYPE_SCREEN);
    property_.SetWhiteList(configs.whiteList);
    if (property_.GetProducerSurface()) {
        property_.SetState(ScreenState::PRODUCER_SURFACE_ENABLE);
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
        decltype(supportedModes_.size()) modeIndex = 0;
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
        property_.SetPhyWidth(activeMode->width);
        property_.SetPhyHeight(activeMode->height);
        property_.SetWidth(activeMode->width);
        property_.SetHeight(activeMode->height);
        property_.SetRefreshRate(activeMode->freshRate);
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
        RS_LOGI("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenConnectionType.", __func__, id);
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

    if (property_.GetConnectionType() == ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL) {
        property_.SetSkipFrameStrategy(SKIP_FRAME_BY_ACTIVE_REFRESH_RATE);
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

const std::string& RSScreen::Name() const
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
    property_.SetOffsetX(offsetX);
    property_.SetOffsetY(offsetY);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

int32_t RSScreen::GetOffsetX() const
{
    return property_.GetOffsetX();
}

int32_t RSScreen::GetOffsetY() const
{
    return property_.GetOffsetY();
}

bool RSScreen::IsSamplingOn() const
{
    return property_.GetIsSamplingOn();
}

float RSScreen::GetSamplingTranslateX() const
{
    return property_.GetSamplingTranslateX();
}

float RSScreen::GetSamplingTranslateY() const
{
    return property_.GetSamplingTranslateY();
}

float RSScreen::GetSamplingScale() const
{
    return property_.GetSamplingScale();
}

RectI RSScreen::GetActiveRect() const
{
    return property_.GetActiveRect();
}

RectI RSScreen::GetMaskRect() const
{
    return property_.GetMaskRect();
}

RectI RSScreen::GetReviseRect() const
{
    return property_.GetReviseRect();
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
        property_.SetState(ScreenState::DISABLED);
        if (onPropertyChange_) {
            onPropertyChange_(property_.Clone());
        }
    }
    int32_t hdiErr = hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId));
    constexpr int32_t hdfErrNotSupport = -5;
    auto ret = StatusCode::SUCCESS;
    if (hdiErr < 0) {
        HILOG_COMM_ERROR("SetActiveMode: Hdi SetScreenMode fails.");
        if (hdiErr != hdfErrNotSupport) {
            if (resolutionChanging) {
                property_.SetState(preState);
                if (onPropertyChange_) {
                    onPropertyChange_(property_.Clone());
                }
            }
            return StatusCode::SET_RATE_ERROR;
        }

        // When HDI return HDF_ERR_NOT_SUPPORT, the hardware will downgrade to a lower resolution,
        // so it should not directly return, active mode info must be update
        ret = StatusCode::HDI_ERR_NOT_SUPPORT;
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        property_.SetPhyWidth(activeMode->width);
        property_.SetPhyHeight(activeMode->height);
        property_.SetRefreshRate(activeMode->freshRate);
        WriteHisyseventEpsLcdInfo(activeMode.value());
        RS_LOGD("%{public}s screenId:%{public}" PRIu64
            ", activeModeId: %{public}d, size:[%{public}u, %{public}u], RefreshRate:[%{public}u]",
            __func__, property_.GetId(), activeMode->id, activeMode->width, activeMode->height, activeMode->freshRate);
    }
    property_.SetState(preState);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
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

    property_.SetActiveRect(RectI(activeRect.x, activeRect.y, activeRect.w, activeRect.h));
    RS_LOGI("%{public}s success, activeRect: (%{public}" PRId32 ", %{public}" PRId32 ", "
        "%{public}" PRId32 ", %{public}" PRId32 ")", __func__, activeRect.x, activeRect.y, activeRect.w, activeRect.h);
    GraphicIRect reviseRect = activeRect;
    if (!CalculateMaskRectAndReviseRect(activeRect, reviseRect)) {
        HILOG_COMM_WARN("CalculateMaskRect failed or not need");
    }
    property_.SetReviseRect(RectI(reviseRect.x, reviseRect.y, reviseRect.w, reviseRect.h));

    if (hdiScreen_->SetScreenActiveRect(reviseRect) < 0) {
        HILOG_COMM_ERROR("SetScreenActiveRect failed: hdi SetScreenActiveRect failed, activeRect with revise:"
            "(%{public}" PRId32 ", %{public}" PRId32 ", %{public}" PRId32 ", %{public}" PRId32 ")",
            reviseRect.x, reviseRect.y, reviseRect.w, reviseRect.h);
        return StatusCode::HDI_ERROR;
    }
    HILOG_COMM_INFO("SetScreenActiveRect success, reviseRect: (%{public}" PRId32 ", %{public}" PRId32 ", "
        "%{public}" PRId32 ", %{public}" PRId32 ")", reviseRect.x, reviseRect.y, reviseRect.w, reviseRect.h);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
    return StatusCode::SUCCESS;
}

bool RSScreen::CalculateMaskRectAndReviseRect(const GraphicIRect& activeRect, GraphicIRect& reviseRect)
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
        auto maskRect = (static_cast<uint32_t>(activeRect.h) == property_.GetHeight()) ? rect[0] : rect[1];
        property_.SetMaskRect(maskRect);
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

void RSScreen::SetRogResolution(uint32_t width, uint32_t height)
{
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return;
    }

    if ((width == 0 || height == 0) ||
        (width == property_.GetWidth() && height == property_.GetHeight())) {
        RS_LOGD("%{public}s: width: %{public}u, height: %{public}u.", __func__, width, height);
        return;
    }

    if (width < property_.GetPhyWidth() && height < property_.GetPhyHeight() &&
        hdiScreen_->SetScreenOverlayResolution(width, height) < 0) {
        RS_LOGE("%{public}s: hdi set screen rog resolution failed.", __func__);
        return;
    }
    isRogResolution_ = true;
    property_.SetWidth(width);
    property_.SetHeight(height);
    RS_LOGI("%{public}s: RSScreen(id %{public}" PRIu64 "), width: %{public}u,"
        " height: %{public}u, phywidth: %{public}u, phyHeight: %{public}u.",
        __func__, property_.GetId(), width, height, property_.GetPhyWidth(), property_.GetPhyHeight());
}

int32_t RSScreen::GetRogResolution(uint32_t& width, uint32_t& height)
{
    if (isRogResolution_) {
        width = property_.GetWidth();
        height = property_.GetHeight();
        RS_LOGD("%{public}s: width: %{public}u, height: %{public}u.", __func__, width, height);
        return StatusCode::SUCCESS;
    }
    return StatusCode::INVALID_ARGUMENTS;
}

int32_t RSScreen::SetResolution(uint32_t width, uint32_t height)
{
    HILOG_COMM_INFO("SetResolution screenId:%{public}" PRIu64 " width: %{public}u height: %{public}u",
                    property_.GetId(), width, height);
    if (IsVirtual()) {
        property_.SetWidth(width);
        property_.SetHeight(height);
        if (onPropertyChange_) {
            onPropertyChange_(property_.Clone());
        }
        return StatusCode::SUCCESS;
    }
    auto phyWidth = property_.GetPhyWidth();
    auto phyHeight = property_.GetPhyHeight();
    if (width < phyWidth || height < phyHeight) {
        HILOG_COMM_ERROR("SetResolution phyWidth: %{public}u phyHeight: %{public}u", phyWidth, phyHeight);
        return StatusCode::INVALID_ARGUMENTS;
    }
    property_.SetWidth(width);
    property_.SetHeight(height);
    bool isSamplingOn = (width > phyWidth || height > phyHeight) && width > 0 && height > 0;
    property_.SetIsSamplingOn(isSamplingOn);
    if (isSamplingOn) {
        auto samplingScale = std::min(static_cast<float>(phyWidth) / width, static_cast<float>(phyHeight) / height);
        property_.SetSamplingScale(samplingScale);
        property_.SetSamplingTranslateX((phyWidth - width * samplingScale) / 2.f);
        property_.SetSamplingTranslateY((phyHeight - height * samplingScale) / 2.f);
        HILOG_COMM_INFO("SetResolution: sampling is enabled. "
            "scale: %{public}f, translateX: %{public}f, translateY: %{public}f",
            samplingScale, property_.GetSamplingTranslateX(), property_.GetSamplingTranslateY());
    }
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
    return StatusCode::SUCCESS;
}

int32_t RSScreen::GetActiveModePosByModeId(int32_t modeId) const
{
    decltype(supportedModes_.size()) modeIndex = 0;
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
        property_.SetPowerStatus(ScreenPowerStatus::INVALID_POWER_STATUS);
        if (onPropertyChange_) {
            onPropertyChange_(property_.Clone());
        }
        return StatusCode::HDI_ERROR;
    }

    property_.SetPowerStatus(static_cast<ScreenPowerStatus>(powerStatus));
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }

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
        [modeId](const auto& mode) { return static_cast<uint32_t>(mode.id) == modeId; });
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
    property_.SetPowerStatus(static_cast<ScreenPowerStatus>(status));
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
    RS_LOGW("%{public}s cached powerStatus is INVALID_POWER_STATUS and GetScreenPowerStatus %{public}u",
        __func__, static_cast<uint32_t>(status));
    return static_cast<ScreenPowerStatus>(status);
}

int32_t RSScreen::SetDualScreenState(DualScreenStatus status)
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support SetDualScreenState.", __func__);
        return StatusCode::VIRTUAL_SCREEN;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }
    uint64_t value = static_cast<uint64_t>(status);
    RS_TRACE_NAME_FMT("Screen_%llu SetDualScreenState %llu", property_.GetId(), value);
    int32_t ret = hdiScreen_->SetDisplayProperty(value);
    if (ret < 0) {
        RS_LOGE("%{public}s: failed to set DualScreenStatus. ret: %{public}d", __func__, ret);
        return StatusCode::HDI_ERROR;
    }
    return StatusCode::SUCCESS;
}

std::shared_ptr<HdiOutput> RSScreen::GetOutput() const
{
    return hdiOutput_;
}

sptr<Surface> RSScreen::GetProducerSurface() const
{
    return property_.GetProducerSurface();
}

void RSScreen::SetProducerSurface(sptr<Surface> producerSurface)
{
    property_.SetProducerSurface(producerSurface);
    if (producerSurface) {
        property_.SetState(ScreenState::PRODUCER_SURFACE_ENABLE);
    } else {
        property_.SetState(ScreenState::DISABLED);
    }
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

void RSScreen::ModeInfoDump(std::string& dumpString)
{
    decltype(supportedModes_.size()) modeIndex = 0;
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

void RSScreen::SurfaceDump(int32_t screenIndex, std::string& dumpString)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("%{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->Dump(dumpString);
}

void RSScreen::DumpCurrentFrameLayers()
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("RSScreen %{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->DumpCurrentFrameLayers();
}

void RSScreen::FpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("%{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->DumpFps(dumpString, arg);
}

void RSScreen::ClearFpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("%{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->ClearFpsDump(dumpString, arg);
}

void RSScreen::HitchsDump(int32_t screenIndex, std::string& dumpString, std::string& arg)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("%{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->DumpHitchs(dumpString, arg);
}

void RSScreen::ResizeVirtualScreen(uint32_t width, uint32_t height)
{
    if (!IsVirtual()) {
        RS_LOGW("%{public}s: physical screen not support ResizeVirtualScreen.", __func__);
        return;
    }
    property_.SetWidth(width);
    property_.SetHeight(height);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
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
    if (hdiScreen_->SetScreenBacklight(level) < 0) {
        RS_LOGE("RSScreen_%{public}" PRIu64 " SetScreenBacklight error.", id);
        return;
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

PanelPowerStatus RSScreen::GetPanelPowerStatus() const
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support GetPanelPowerStatus.", __func__);
        return PanelPowerStatus::INVALID_PANEL_POWER_STATUS;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return PanelPowerStatus::INVALID_PANEL_POWER_STATUS;
    }
    auto hdiStatus = GraphicPanelPowerStatus::GRAPHIC_PANEL_POWER_STATUS_BUTT;
    auto ret = hdiScreen_->GetPanelPowerStatus(hdiStatus);
    if ((ret < 0) || (hdiStatus >= GraphicPanelPowerStatus::GRAPHIC_PANEL_POWER_STATUS_BUTT)) {
        RS_LOGE("%{public}s failed, ret: %{public}d, hdiStatus: %{public}d",
                __func__, ret, static_cast<uint32_t>(hdiStatus));
        return PanelPowerStatus::INVALID_PANEL_POWER_STATUS;
    }
    RS_LOGI("%{public}s acquired status: %{public}d", __func__, static_cast<uint32_t>(hdiStatus));
    return static_cast<PanelPowerStatus>(hdiStatus);
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

int32_t RSScreen::GetScreenColorGamut(ScreenColorGamut& mode) const
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
        property_.SetScreenColorGamut(supportedVirtualColorGamuts_[currentVirtualColorGamutIdx_]);
        if (onPropertyChange_) {
            onPropertyChange_(property_.Clone());
        }
        return StatusCode::SUCCESS;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }
    std::vector<GraphicColorGamut> hdiMode;
    if (hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) != GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::HDI_ERROR;
    }
    if (modeIdx >= static_cast<int32_t>(hdiMode.size())) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    int32_t result = hdiScreen_->SetScreenColorGamut(hdiMode[modeIdx]);
    if (result != GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::HDI_ERROR;
    }
    currentPhysicalColorGamutIdx_ = modeIdx;
    property_.SetScreenColorGamut(supportedPhysicalColorGamuts_[currentPhysicalColorGamutIdx_]);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
    return StatusCode::SUCCESS;
}

void RSScreen::SetScreenCorrection(ScreenRotation screenRotation)
{
    RS_LOGI("%{public}s: RSScreen(id %{public}" PRIu64 "), ScreenRotation: %{public}u.", __func__,
        property_.GetId(), static_cast<uint32_t>(screenRotation));
    property_.SetScreenCorrection(screenRotation);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

ScreenRotation RSScreen::GetScreenCorrection() const
{
    return property_.GetScreenCorrection();
}

int32_t RSScreen::SetScreenGamutMap(ScreenGamutMap mode)
{
    if (IsVirtual()) {
        property_.SetScreenGamutMap(mode);
        if (onPropertyChange_) {
            onPropertyChange_(property_.Clone());
        }
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
    return StatusCode::HDI_ERROR;
}

int32_t RSScreen::GetScreenGamutMap(ScreenGamutMap& mode) const
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
    property_.SetSkipFrameInterval(skipFrameInterval);
    property_.SetSkipFrameStrategy(SKIP_FRAME_BY_INTERVAL);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

void RSScreen::SetScreenExpectedRefreshRate(uint32_t expectedRefreshRate)
{
    property_.SetExpectedRefreshRate(expectedRefreshRate);
    property_.SetSkipFrameStrategy(SKIP_FRAME_BY_REFRESH_RATE);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

uint32_t RSScreen::GetScreenSkipFrameInterval() const
{
    return property_.GetSkipFrameInterval();
}

uint32_t RSScreen::GetScreenExpectedRefreshRate() const
{
    return property_.GetExpectedRefreshRate();
}

SkipFrameStrategy RSScreen::GetScreenSkipFrameStrategy() const
{
    return property_.GetSkipFrameStrategy();
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

    property_.SetCanvasRotation(canvasRotation);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
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

    property_.SetAutoBufferRotation(isAutoRotation);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
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

    property_.SetScreenScaleMode(scaleMode);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
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
        property_.SetScreenHDRFormat(supportedVirtualHDRFormats_[currentVirtualHDRFormatIdx_]);
        if (onPropertyChange_) {
            onPropertyChange_(property_.Clone());
        }
        return StatusCode::SUCCESS;
    } else {
        // There should be some hdi operation
        if (modeIdx >= static_cast<int32_t>(hdrCapability_.formats.size())) {
            return StatusCode::INVALID_ARGUMENTS;
        }
        currentPhysicalHDRFormatIdx_ = modeIdx;
        property_.SetScreenHDRFormat(supportedPhysicalHDRFormats_[currentPhysicalHDRFormatIdx_]);
        if (onPropertyChange_) {
            onPropertyChange_(property_.Clone());
        }
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
    property_.SetPixelFormat(pixelFormat);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
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
    int32_t curIdx;
    if (IsVirtual()) {
        auto it = std::find(supportedVirtualColorGamuts_.begin(), supportedVirtualColorGamuts_.end(), dstColorGamut);
        if (it == supportedVirtualColorGamuts_.end()) {
            return StatusCode::INVALID_ARGUMENTS;
        }
        curIdx = std::distance(supportedVirtualColorGamuts_.begin(), it);
        currentVirtualColorGamutIdx_ = curIdx;
        property_.SetScreenColorGamut(supportedVirtualColorGamuts_[currentVirtualColorGamutIdx_]);
        if (onPropertyChange_) {
            onPropertyChange_(property_.Clone());
        }
        return StatusCode::SUCCESS;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return StatusCode::HDI_ERROR;
    }
    std::vector<GraphicColorGamut> hdiMode;
    if (hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) != GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::HDI_ERROR;
    }
    auto it = std::find(hdiMode.begin(), hdiMode.end(), static_cast<GraphicColorGamut>(dstColorGamut));
    if (it == hdiMode.end()) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    curIdx = std::distance(hdiMode.begin(), it);
    int32_t result = hdiScreen_->SetScreenColorGamut(hdiMode[curIdx]);
    if (result != GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::HDI_ERROR;
    }
    currentPhysicalColorGamutIdx_ = curIdx;
    property_.SetScreenColorGamut(supportedPhysicalColorGamuts_[currentPhysicalColorGamutIdx_]);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
    return StatusCode::SUCCESS;
}

std::unordered_set<NodeId> RSScreen::GetWhiteList() const
{
    return property_.GetWhiteList();
}

void RSScreen::SetWhiteList(const std::unordered_set<NodeId>& whiteList)
{
    property_.SetWhiteList(whiteList);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

void RSScreen::AddWhiteList(const std::vector<NodeId>& whiteList)
{
    property_.AddWhiteList(whiteList);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

void RSScreen::RemoveWhiteList(const std::vector<NodeId>& whiteList)
{
    property_.RemoveWhiteList(whiteList);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

void RSScreen::SetBlackList(const std::unordered_set<NodeId>& blackList)
{
    property_.SetBlackList(blackList);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

void RSScreen::SetTypeBlackList(const std::unordered_set<uint8_t>& typeBlackList)
{
    property_.SetTypeBlackList(typeBlackList);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

void RSScreen::AddBlackList(const std::vector<NodeId>& blackList)
{
    if (blackList.empty()) {
        return;
    }
    property_.AddBlackList(blackList);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

void RSScreen::RemoveBlackList(const std::vector<NodeId>& blackList)
{
    if (blackList.empty()) {
        return;
    }
    property_.RemoveBlackList(blackList);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

void RSScreen::SetCastScreenEnableSkipWindow(bool enable)
{
    property_.SetCastScreenEnableSkipWindow(enable);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

bool RSScreen::GetCastScreenEnableSkipWindow()
{
    return property_.GetCastScreenEnableSkipWindow();
}

std::unordered_set<NodeId> RSScreen::GetBlackList() const
{
    return property_.GetBlackList();
}

std::unordered_set<uint8_t> RSScreen::GetTypeBlackList() const
{
    return property_.GetTypeBlackList();
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
    property_.SetSecurityExemptionList(securityExemptionList);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

const std::vector<uint64_t> RSScreen::GetSecurityExemptionList() const
{
    return property_.GetSecurityExemptionList();
}

int32_t RSScreen::SetSecurityMask(std::shared_ptr<Media::PixelMap> securityMask)
{
    property_.SetSecurityMask(securityMask);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
    return SUCCESS;
}

std::shared_ptr<Media::PixelMap> RSScreen::GetSecurityMask() const
{
    return property_.GetSecurityMask();
}

void RSScreen::SetEnableVisibleRect(bool enable)
{
    property_.SetEnableVisibleRect(enable);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

bool RSScreen::GetEnableVisibleRect() const
{
    return property_.GetEnableVisibleRect();
}

void RSScreen::SetMainScreenVisibleRect(const Rect& mainScreenRect)
{
    property_.SetMainScreenVisibleRect(mainScreenRect);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

Rect RSScreen::GetMainScreenVisibleRect() const
{
    return property_.GetMainScreenVisibleRect();
}

bool RSScreen::GetVisibleRectSupportRotation() const
{
    return property_.GetVisibleRectSupportRotation();
}

void RSScreen::SetVisibleRectSupportRotation(bool supportRotation)
{
    property_.SetVisibleRectSupportRotation(supportRotation);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }
}

bool RSScreen::SetVirtualScreenStatus(VirtualScreenStatus screenStatus)
{
    if (!IsVirtual()) {
        return false;
    }

    property_.SetScreenStatus(screenStatus);
    if (onPropertyChange_) {
        onPropertyChange_(property_.Clone());
    }

    if (screenStatus == VirtualScreenStatus::VIRTUAL_SCREEN_PLAY) {
        virtualScreenPlay_ = true;
    }
    return true;
}

VirtualScreenStatus RSScreen::GetVirtualScreenStatus() const
{
    return property_.GetScreenStatus();
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

void RSScreen::SetHasProtectedLayer(bool hasProtectedLayer)
{
    hasProtectedLayer_ = hasProtectedLayer;
}

bool RSScreen::GetHasProtectedLayer()
{
    return hasProtectedLayer_;
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

// only used in virtual screen
bool RSScreen::GetAndResetWhiteListChange()
{
    bool expected = true;
    return whiteListChange_.compare_exchange_strong(expected, false);
}

void RSScreen::SetWhiteListChange(bool whiteListChange)
{
    whiteListChange_ = whiteListChange;
}

bool RSScreen::GetAndResetPSurfaceChange()
{
    bool expected = true;
    return pSurfaceChange_.compare_exchange_strong(expected, false);
}

// only used in virtual screen
void RSScreen::SetPSurfaceChange(bool pSurfaceChange)
{
    pSurfaceChange_ = pSurfaceChange;
}

bool RSScreen::GetAndResetVirtualScreenPlay()
{
    bool expected = true;
    return virtualScreenPlay_.compare_exchange_strong(expected, false);
}

ScreenInfo RSScreen::GetScreenInfo() const
{
    return property_.GetScreenInfo();
}

sptr<RSScreenProperty> RSScreen::GetProperty() const
{
    return property_.Clone();
}

void RSScreen::SetOnPropertyChangedCallback(std::function<void(const sptr<RSScreenProperty>&)> callback)
{
    onPropertyChange_ = std::move(callback);
}
} // namespace Rosen
} // namespace OHOS
