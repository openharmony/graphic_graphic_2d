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

#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "string_utils.h"
#include "hisysevent.h"

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;

namespace impl {
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

RSScreen::RSScreen(ScreenId id,
    bool isVirtual,
    std::shared_ptr<HdiOutput> output,
    sptr<Surface> surface)
    : id_(id),
      isVirtual_(isVirtual),
      hdiOutput_(std::move(output)),
      producerSurface_(std::move(surface))
{
    if (!IsVirtual()) {
        hdrCapability_.formatCount = 0;
        name_ = "Screen_" + std::to_string(id_);
        PhysicalScreenInit();
    }
    capability_.props.clear();
}

RSScreen::RSScreen(const VirtualScreenConfigs &configs)
    : id_(configs.id),
      mirrorId_(configs.mirrorId),
      name_(configs.name),
      width_(configs.width),
      height_(configs.height),
      isVirtual_(true),
      producerSurface_(configs.surface),
      pixelFormat_(configs.pixelFormat),
      screenType_(RSScreenType::VIRTUAL_TYPE_SCREEN),
      filteredAppSet_(configs.filteredAppSet)
{
    VirtualScreenInit();
}

RSScreen::~RSScreen() noexcept
{
}

void RSScreen::VirtualScreenInit() noexcept
{
    hdrCapability_.formatCount = 0;
    for (auto item : supportedVirtualHDRFormats_) {
        hdrCapability_.formats.emplace_back(RS_TO_HDI_HDR_FORMAT_MAP[item]);
        ++hdrCapability_.formatCount;
    }
}

void RSScreen::PhysicalScreenInit() noexcept
{
    hdiScreen_ = HdiScreen::CreateHdiScreen(ScreenPhysicalId(id_));
    if (hdiScreen_ == nullptr) {
        RS_LOGE("RSScreen %{public}s: RSScreen(id %{public}" PRIu64 ") failed to CreateHdiScreens.",
            __func__, id_);
        return;
    }

    hdiScreen_->Init();
    if (hdiScreen_->GetScreenSupportedModes(supportedModes_) < 0) {
        RS_LOGE("RSScreen %{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenSupportedModes.",
            __func__, id_);
    }
    
    if (hdiScreen_->GetHDRCapabilityInfos(hdrCapability_) < 0) {
        RS_LOGE("RSScreen %{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetHDRCapabilityInfos.",
            __func__, id_);
    }
    std::transform(hdrCapability_.formats.begin(), hdrCapability_.formats.end(),
                   back_inserter(supportedPhysicalHDRFormats_),
                   [](GraphicHDRFormat item) -> ScreenHDRFormat {return HDI_HDR_FORMAT_TO_RS_MAP[item];});
    auto status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    if (hdiScreen_->SetScreenPowerStatus(status) < 0) {
        RS_LOGE("RSScreen %{public}s: RSScreen(id %{public}" PRIu64 ") failed to SetScreenPowerStatus.",
            __func__, id_);
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        phyWidth_ = activeMode->width;
        phyHeight_ = activeMode->height;
        width_ = phyWidth_;
        height_ = phyHeight_;
    }
    if (hdiScreen_->GetScreenPowerStatus(powerStatus_) < 0) {
        powerStatus_ = static_cast<GraphicDispPowerStatus>(INVALID_POWER_STATUS);
    }
    if (capability_.type == GraphicInterfaceType::GRAPHIC_DISP_INTF_MIPI) {
        screenType_ = RSScreenType::BUILT_IN_TYPE_SCREEN;
    } else {
        screenType_ = RSScreenType::EXTERNAL_TYPE_SCREEN;
    }
    ScreenCapabilityInit();

    std::vector<GraphicColorGamut> supportedColorGamuts;
    if (hdiScreen_->GetScreenSupportedColorGamuts(supportedColorGamuts) != GRAPHIC_DISPLAY_SUCCESS) {
        RS_LOGE("RSScreen %{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenSupportedColorGamuts.",
            __func__, id_);
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
}

void RSScreen::ScreenCapabilityInit() noexcept
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %{public}s: this is virtual screen, use the default display capability.",  __func__);
        return;
    }
    int32_t ret = hdiScreen_->GetScreenCapability(capability_);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        RS_LOGW("RSScreen %{public}s: get display capability failed, ret is %{public}d, use the default"
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
    return id_;
}

ScreenId RSScreen::MirrorId() const
{
    return mirrorId_;
}

void RSScreen::SetMirror(ScreenId mirrorId)
{
    mirrorId_ = mirrorId;
}

const std::string& RSScreen::Name() const
{
    return name_;
}

uint32_t RSScreen::Width() const
{
    return width_;
}

uint32_t RSScreen::Height() const
{
    return height_;
}

uint32_t RSScreen::PhyWidth() const
{
    return phyWidth_;
}

uint32_t RSScreen::PhyHeight() const
{
    return phyHeight_;
}

bool RSScreen::IsEnable() const
{
    if (id_ == INVALID_SCREEN_ID) {
        return false;
    }

    if (!hdiOutput_ && !producerSurface_) {
        return false;
    }

    // [PLANNING]: maybe need more information to judge whether this screen is enable.
    return true;
}

bool RSScreen::IsVirtual() const
{
    return isVirtual_;
}

void RSScreen::SetActiveMode(uint32_t modeId)
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %{public}s: virtual screen not support SetActiveMode.", __func__);
        return;
    }

    if (modeId >= supportedModes_.size()) {
        RS_LOGE("RSScreen %{public}s: set fails because the index is out of bounds.", __func__);
        return;
    }
    int32_t selectModeId = supportedModes_[modeId].id;
    if (hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) < 0) {
        RS_LOGE("RSScreen %{public}s: Hdi SetScreenMode fails.", __func__);
        return;
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        phyWidth_ = activeMode->width;
        phyHeight_ = activeMode->height;
        static GraphicDisplayModeInfo modeInfo;
        if ((modeInfo.freshRate != activeMode->freshRate)
            || modeInfo.width != activeMode->width || modeInfo.height != activeMode->height) {
            HiSysEventWrite(HiSysEvent::Domain::GRAPHIC, "EPS_LCD_FREQ",
                HiSysEvent::EventType::STATISTIC, "SOURCERATE", modeInfo.freshRate,
                "TARGETRATE", activeMode->freshRate, "WIDTH", phyWidth_, "HEIGHT", phyHeight_);
            modeInfo = activeMode.value();
        }
    }
}

void RSScreen::SetRogResolution(uint32_t width, uint32_t height)
{
    if ((width == 0 || height == 0) ||
        (width == width_ && height == height_) ||
        (width > phyWidth_ || height > phyHeight_)) {
        RS_LOGD("RSScreen:%{public}s: width: %{public}d, height: %{public}d.", __func__, width, height);
        return;
    }
    if (hdiScreen_->SetScreenOverlayResolution(width, height) < 0) {
        RS_LOGD("RSScreen:%{public}s: hdi set screen rog resolution failed.", __func__);
    }
    width_ = width;
    height_ = height;
    RS_LOGI("RSScreen %{public}s: RSScreen(id %{public}" PRIu64 "), width: %{public}d,"
        " height: %{public}d, phywidth: %{public}d, phyHeight: %{public}d.",
	    __func__, id_, width_, height_, phyWidth_, phyHeight_);
}


void RSScreen::SetResolution(uint32_t width, uint32_t height)
{
    if (!IsVirtual()) {
        RS_LOGW("RSScreen %{public}s: physical screen not support SetResolution.", __func__);
        return;
    }
    width_ = width;
    height_ = height;
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

void RSScreen::SetPowerStatus(uint32_t powerStatus)
{
    if (IsVirtual()) {
        RS_LOGW("[UL_POWER]RSScreen %{public}s: virtual screen not support SetPowerStatus.", __func__);
        return;
    }

    RS_LOGI("[UL_POWER]RSScreen_%{public}" PRIu64 " SetPowerStatus, status is %{public}u", id_, powerStatus);
    RS_TRACE_NAME_FMT("[UL_POWER]Screen_%llu SetPowerStatus %u", id_, powerStatus);
    if (hdiScreen_->SetScreenPowerStatus(static_cast<GraphicDispPowerStatus>(powerStatus)) < 0) {
        return;
    }

    if (powerStatus == GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON ||
        powerStatus == GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON_ADVANCED) {
        RS_LOGD("RSScreen %{public}s Enable hardware vsync", __func__);
        if (hdiScreen_->SetScreenVsyncEnabled(true) != GRAPHIC_DISPLAY_SUCCESS) {
            RS_LOGE("RSScreen %{public}s SetScreenVsyncEnabled failed", __func__);
        }
    }
}

std::optional<GraphicDisplayModeInfo> RSScreen::GetActiveMode() const
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %{public}s: virtual screen not support GetActiveMode.", __func__);
        return {};
    }

    uint32_t modeId = 0;

    if (hdiScreen_ == nullptr) {
        RS_LOGE("RSScreen %{public}s: RSScreen(id %{public}" PRIu64 ") hdiScreen is null.",
            __func__, id_);
        return {};
    }

    if (hdiScreen_->GetScreenMode(modeId) < 0) {
        RS_LOGE("RSScreen %{public}s: RSScreen(id %{public}" PRIu64 ") GetScreenMode failed.",
            __func__, id_);
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

uint32_t RSScreen::GetPowerStatus() const
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %{public}s: virtual screen not support GetPowerStatus.", __func__);
        return ScreenPowerStatus::INVALID_POWER_STATUS;
    }

    GraphicDispPowerStatus status;
    if (hdiScreen_->GetScreenPowerStatus(status) < 0) {
        return INVALID_POWER_STATUS;
    }
    return static_cast<uint32_t>(status);
}

std::shared_ptr<HdiOutput> RSScreen::GetOutput() const
{
    return hdiOutput_;
}

sptr<Surface> RSScreen::GetProducerSurface() const
{
    return producerSurface_;
}

void RSScreen::SetProducerSurface(sptr<Surface> producerSurface)
{
    producerSurface_ = producerSurface;
}

void RSScreen::ModeInfoDump(std::string& dumpString)
{
    decltype(supportedModes_.size()) modeIndex = 0;
    for (; modeIndex < supportedModes_.size(); ++modeIndex) {
        AppendFormat(dumpString, "  supportedMode[%d]: %dx%d, refreshrate=%d\n",
                     modeIndex, supportedModes_[modeIndex].width,
                     supportedModes_[modeIndex].height, supportedModes_[modeIndex].freshRate);
    }
    std::optional<GraphicDisplayModeInfo> activeMode = GetActiveMode();
    if (activeMode) {
        AppendFormat(dumpString, "  activeMode: %dx%d, refreshrate=%d\n",
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
            dumpString += "INVILID_DISP_INTF, ";
            break;
    }
}

void RSScreen::CapabilityDump(std::string& dumpString)
{
    AppendFormat(dumpString, "  capability: name=%s, phywidth=%d, phyheight=%d,"
                 "supportlayers=%d, virtualDispCount=%d, propCount=%d, ",
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
    dumpString += "powerstatus=";
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
        case GRAPHIC_POWER_STATUS_BUTT: {
            dumpString += "POWER_STATUS_BUTT";
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
        dumpString += (id_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID" : std::to_string(id_);
        dumpString += ", ";
        dumpString += "mirrorId=";
        dumpString += (mirrorId_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID" : std::to_string(mirrorId_);
        dumpString += ", ";
        AppendFormat(dumpString, ", render size: %dx%d, isvirtual=true\n", width_, height_);
    } else {
        dumpString += "screen[" + std::to_string(screenIndex) + "]: ";
        dumpString += "id=";
        dumpString += (id_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID" : std::to_string(id_);
        dumpString += ", ";
        PowerStatusDump(dumpString);
        dumpString += ", ";
        dumpString += "backlight=" + std::to_string(GetScreenBacklight());
        dumpString += ", ";
        ScreenTypeDump(dumpString);
        AppendFormat(dumpString, ", render size: %dx%d, physical screen resolution: %dx%d, isvirtual=false\n",
            width_, height_, phyWidth_, phyHeight_);
        dumpString += "\n";
        ModeInfoDump(dumpString);
        CapabilityDump(dumpString);
    }
}

void RSScreen::ScreenTypeDump(std::string& dumpString)
{
    dumpString += "screenType=";
    switch (screenType_) {
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
        RS_LOGW("RSScreen %{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->Dump(dumpString);
}

void RSScreen::FpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("RSScreen %{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->DumpFps(dumpString, arg);
}

void RSScreen::ClearFpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("RSScreen %{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->ClearFpsDump(dumpString, arg);
}

void RSScreen::ResizeVirtualScreen(uint32_t width, uint32_t height)
{
    if (!IsVirtual()) {
        RS_LOGW("RSScreen %{public}s: physical screen not support ResizeVirtualScreen.", __func__);
        return;
    }
    width_ = width;
    height_ = height;
}

void RSScreen::SetScreenBacklight(uint32_t level)
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %{public}s: virtual screen not support SetScreenBacklight.", __func__);
        return;
    }
    if (hdiScreen_->SetScreenBacklight(level) < 0) {
        return;
    }
}

int32_t RSScreen::GetScreenBacklight() const
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %{public}s: virtual screen not support GetScreenBacklight.", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }
    uint32_t level = 0;
    if (hdiScreen_->GetScreenBacklight(level) < 0) {
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
        RS_LOGW("RSScreen %{public}s: virtual screen not support GetScreenSupportedMetaDataKeys.", __func__);
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
    } else {
        if (supportedPhysicalColorGamuts_.size() == 0) {
            return StatusCode::HDI_ERROR;
        }
        mode = supportedPhysicalColorGamuts_[currentPhysicalColorGamutIdx_];
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
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
        return StatusCode::SUCCESS;
    }
    std::vector<GraphicColorGamut> hdiMode;
    if (hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) != GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::HDI_ERROR;
    }
    if (modeIdx >= static_cast<int32_t>(hdiMode.size())) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    int32_t result = hdiScreen_->SetScreenColorGamut(hdiMode[modeIdx]);
    if (result == GRAPHIC_DISPLAY_SUCCESS) {
        currentPhysicalColorGamutIdx_ = modeIdx;
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

int32_t RSScreen::SetScreenGamutMap(ScreenGamutMap mode)
{
    if (IsVirtual()) {
        currentVirtualGamutMap_ = mode;
        return StatusCode::SUCCESS;
    }
    int32_t result = hdiScreen_->SetScreenGamutMap(static_cast<GraphicGamutMap>(mode));
    if (result == GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

void RSScreen::SetScreenCorrection(ScreenRotation screenRotation)
{
    RS_LOGD("RSScreen %{public}s: RSScreen(id %{public}" PRIu64 ") ,ScreenRotation: %{public}d.",
            __func__, id_, static_cast<uint32_t>(screenRotation));
    screenRotation_ = screenRotation;
}

ScreenRotation RSScreen::GetScreenCorrection() const
{
    return screenRotation_;
}

int32_t RSScreen::GetScreenGamutMap(ScreenGamutMap &mode) const
{
    if (IsVirtual()) {
        mode = currentVirtualGamutMap_;
        return StatusCode::SUCCESS;
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

const RSScreenType& RSScreen::GetScreenType() const
{
    return screenType_;
}

void RSScreen::SetScreenSkipFrameInterval(uint32_t skipFrameInterval)
{
    skipFrameInterval_ = skipFrameInterval;
}

uint32_t RSScreen::GetScreenSkipFrameInterval() const
{
    return skipFrameInterval_;
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
    if (IsVirtual()) {
        canvasRotation_ = canvasRotation;
        return true;
    }
    return false;
}

bool RSScreen::GetCanvasRotation() const
{
    return canvasRotation_;
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
        return StatusCode::SUCCESS;
    } else {
        // There should be some hdi operation
        if (modeIdx >= static_cast<int32_t>(hdrCapability_.formats.size())) {
            return StatusCode::INVALID_ARGUMENTS;
        }
        currentPhysicalHDRFormatIdx_ = modeIdx;
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

int32_t RSScreen::GetPixelFormat(GraphicPixelFormat& pixelFormat) const
{
    pixelFormat = pixelFormat_;
    return StatusCode::SUCCESS;
}

int32_t RSScreen::SetPixelFormat(GraphicPixelFormat pixelFormat)
{
    pixelFormat_ = pixelFormat;
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
        return StatusCode::SUCCESS;
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
    if (result == GRAPHIC_DISPLAY_SUCCESS) {
        currentPhysicalColorGamutIdx_ = curIdx;
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}
const std::unordered_set<uint64_t>& RSScreen::GetFilteredAppSet() const
{
    return filteredAppSet_;
}
} // namespace impl
} // namespace Rosen
} // namespace OHOS
