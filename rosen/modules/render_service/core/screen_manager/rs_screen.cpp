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

#include <cinttypes>

#include "platform/common/rs_log.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;

namespace impl {
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
        name_ = "Screen_" + std::to_string(id_);
        PhysicalScreenInit();
    }
}

RSScreen::RSScreen(const VirtualScreenConfigs &configs)
    : id_(configs.id),
      mirrorId_(configs.mirrorId),
      name_(configs.name),
      width_(configs.width),
      height_(configs.height),
      isVirtual_(true),
      producerSurface_(configs.surface),
      screenType_(RSScreenType::VIRTUAL_TYPE_SCREEN)
{
}

RSScreen::~RSScreen() noexcept
{
}

void RSScreen::PhysicalScreenInit() noexcept
{
    hdiScreen_ = HdiScreen::CreateHdiScreen(ScreenPhysicalId(id_));
    if (hdiScreen_ == nullptr) {
        RS_LOGE("RSScreen %s: RSScreen(id %" PRIu64 ") failed to CreateHdiScreens.",
            __func__, id_);
        return;
    }

    hdiScreen_->Init();
    if (hdiScreen_->GetScreenSupportedModes(supportedModes_) < 0) {
        RS_LOGE("RSScreen %s: RSScreen(id %" PRIu64 ") failed to GetScreenSupportedModes.",
            __func__, id_);
    }
    if (hdiScreen_->GetScreenCapability(capability_) < 0) {
        RS_LOGE("RSScreen %s: RSScreen(id %" PRIu64 ") failed to GetScreenCapability.",
            __func__, id_);
    }
    if (hdiScreen_->GetHDRCapabilityInfos(hdrCapability_) < 0) {
        RS_LOGE("RSScreen %s: RSScreen(id %" PRIu64 ") failed to GetHDRCapabilityInfos.",
            __func__, id_);
    }
    auto status = DispPowerStatus::POWER_STATUS_ON;
    if (hdiScreen_->SetScreenPowerStatus(status) < 0) {
        RS_LOGE("RSScreen %s: RSScreen(id %" PRIu64 ") failed to SetScreenPowerStatus.",
            __func__, id_);
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        width_ = activeMode->width;
        height_ = activeMode->height;
    }
    if (hdiScreen_->GetScreenPowerStatus(powerStatus_) < 0) {
        powerStatus_ = static_cast<DispPowerStatus>(INVALID_POWER_STATUS);
    }
    if (capability_.type == InterfaceType::DISP_INTF_MIPI) {
        screenType_ = RSScreenType::BUILT_IN_TYPE_SCREEN;
    } else {
        screenType_ = RSScreenType::EXTERNAL_TYPE_SCREEN;
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
        RS_LOGW("RSScreen %s: virtual screen not support SetActiveMode.", __func__);
        return;
    }

    if (modeId >= supportedModes_.size()) {
        RS_LOGE("RSScreen %s: set fails because the index is out of bounds.", __func__);
        return;
    }
    int32_t selectModeId = supportedModes_[modeId].id;
    if (hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) < 0) {
        RS_LOGE("RSScreen %s: Hdi SetScreenMode fails.", __func__);
        return;
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        width_ = activeMode->width;
        height_ = activeMode->height;
    }
}

void RSScreen::SetResolution(uint32_t width, uint32_t height)
{
    if (!IsVirtual()) {
        RS_LOGW("RSScreen %s: physical screen not support SetResolution.", __func__);
        return;
    }
    width_ = static_cast<int32_t>(width);
    height_ = static_cast<int32_t>(height);
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
        RS_LOGW("RSScreen %s: virtual screen not support SetPowerStatus.", __func__);
        return;
    }

    RS_LOGD("RSScreen %s SetPowerStatus, status is %u", __func__, powerStatus);
    if (hdiScreen_->SetScreenPowerStatus(static_cast<DispPowerStatus>(powerStatus)) < 0) {
        return;
    }

    if (powerStatus == DispPowerStatus::POWER_STATUS_ON) {
        RS_LOGD("RSScreen %s Enable hardware vsync", __func__);
        if (hdiScreen_->SetScreenVsyncEnabled(true) != GRAPHIC_DISPLAY_SUCCESS) {
            RS_LOGE("RSScreen %s SetScreenVsyncEnabled failed", __func__);
        }
    }
}

std::optional<DisplayModeInfo> RSScreen::GetActiveMode() const
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %s: virtual screen not support GetActiveMode.", __func__);
        return {};
    }

    uint32_t modeId = 0;

    if (hdiScreen_ == nullptr) {
        RS_LOGE("RSScreen %s: RSScreen(id %" PRIu64 ") hdiScreen is null.",
            __func__, id_);
        return {};
    }

    if (hdiScreen_->GetScreenMode(modeId) < 0) {
        RS_LOGE("RSScreen %s: RSScreen(id %" PRIu64 ") GetScreenMode failed.",
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

const std::vector<DisplayModeInfo>& RSScreen::GetSupportedModes() const
{
    return supportedModes_;
}

const DisplayCapability& RSScreen::GetCapability() const
{
    return capability_;
}

uint32_t RSScreen::GetPowerStatus() const
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %s: virtual screen not support GetPowerStatus.", __func__);
        return ScreenPowerStatus::INVALID_POWER_STATUS;
    }

    DispPowerStatus status;
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
    std::optional<DisplayModeInfo> activeMode = GetActiveMode();
    if (activeMode) {
        AppendFormat(dumpString, "  activeMode: %dx%d, refreshrate=%d\n",
            activeMode->width, activeMode->height, activeMode->freshRate);
    }
}

void RSScreen::CapabilityTypeDump(InterfaceType capabilityType, std::string& dumpString)
{
    dumpString += "type=";
    switch (capability_.type) {
        case DISP_INTF_HDMI: {
            dumpString += "DISP_INTF_HDMI, ";
            break;
        }
        case DISP_INTF_LCD: {
            dumpString += "DISP_INTF_LCD, ";
            break;
        }
        case DISP_INTF_BT1120: {
            dumpString += "DISP_INTF_BT1120, ";
            break;
        }
        case DISP_INTF_BT656: {
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
                 capability_.name, capability_.phyWidth, capability_.phyHeight,
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
                     propIndex, capability_.props[propIndex].name, capability_.props[propIndex].propId,
                     capability_.props[propIndex].value);
    }
}

void RSScreen::PowerStatusDump(std::string& dumpString)
{
    dumpString += "powerstatus=";
    switch (powerStatus_) {
        case POWER_STATUS_ON: {
            dumpString += "POWER_STATUS_ON";
            break;
        }
        case POWER_STATUS_STANDBY: {
            dumpString += "POWER_STATUS_STANDBY";
            break;
        }
        case POWER_STATUS_SUSPEND: {
            dumpString += "POWER_STATUS_SUSPEND";
            break;
        }
        case POWER_STATUS_OFF: {
            dumpString += "POWER_STATUS_OFF";
            break;
        }
        case POWER_STATUS_BUTT: {
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
        dumpString += (id_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID" : std::to_string(id_);
        dumpString += ", ";
        dumpString += "mirrorId=";
        dumpString += (mirrorId_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID" : std::to_string(mirrorId_);
        dumpString += ", ";
        AppendFormat(dumpString, "%dx%d, isvirtual=true\n", width_, height_);
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
        RS_LOGW("RSScreen %s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->Dump(dumpString);
}

void RSScreen::FpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("RSScreen %s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->DumpFps(dumpString, arg);
}

void RSScreen::ClearFpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("RSScreen %s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->ClearFpsDump(dumpString, arg);
}

void RSScreen::SetScreenBacklight(uint32_t level)
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %s: virtual screen not support SetScreenBacklight.", __func__);
        return;
    }
    if (hdiScreen_->SetScreenBacklight(level) < 0) {
        return;
    }
}

int32_t RSScreen::GetScreenBacklight() const
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %s: virtual screen not support GetScreenBacklight.", __func__);
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
    if (IsVirtual()) {
        mode.clear();
        mode = supportedVirtualColorGamuts_;
        return StatusCode::SUCCESS;
    }
    std::vector<GraphicColorGamut> hdiMode;
    int32_t result = hdiScreen_->GetScreenSupportedColorGamuts(hdiMode);
    if (result == GraphicDispErrCode::GRAPHIC_DISPLAY_SUCCESS) {
        mode.clear();
        std::transform(hdiMode.begin(), hdiMode.end(), std::back_inserter(mode),
            [](GraphicColorGamut m) { return static_cast<ScreenColorGamut>(m); });
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

int32_t RSScreen::GetScreenSupportedMetaDataKeys(std::vector<ScreenHDRMetadataKey> &keys) const
{
    if (IsVirtual()) {
        RS_LOGW("RSScreen %s: virtual screen not support GetScreenSupportedMetaDataKeys.", __func__);
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
    GraphicColorGamut hdiMode;
    int32_t result = hdiScreen_->GetScreenColorGamut(hdiMode);
    if (result == GraphicDispErrCode::GRAPHIC_DISPLAY_SUCCESS) {
        mode = static_cast<ScreenColorGamut>(hdiMode);
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

int32_t RSScreen::SetScreenColorGamut(int32_t modeIdx)
{
    if (IsVirtual()) {
        if (modeIdx >= static_cast<int32_t>(supportedVirtualColorGamuts_.size())) {
            return StatusCode::INVALID_ARGUMENTS;
        }
        currentVirtualColorGamutIdx_ = modeIdx;
        return StatusCode::SUCCESS;
    }
    std::vector<GraphicColorGamut> hdiMode;
    if (hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) != GraphicDispErrCode::GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::HDI_ERROR;
    }
    if (modeIdx >= static_cast<int32_t>(hdiMode.size())) {
        return StatusCode::INVALID_ARGUMENTS;
    }
    int32_t result = hdiScreen_->SetScreenColorGamut(hdiMode[modeIdx]);
    if (result == GraphicDispErrCode::GRAPHIC_DISPLAY_SUCCESS) {
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
    int32_t result = hdiScreen_->SetScreenGamutMap(static_cast<GamutMap>(mode));
    if (result == GraphicDispErrCode::GRAPHIC_DISPLAY_SUCCESS) {
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

int32_t RSScreen::GetScreenGamutMap(ScreenGamutMap &mode) const
{
    if (IsVirtual()) {
        mode = currentVirtualGamutMap_;
        return StatusCode::SUCCESS;
    }
    GamutMap hdiMode;
    int32_t result = hdiScreen_->GetScreenGamutMap(hdiMode);
    if (result == GraphicDispErrCode::GRAPHIC_DISPLAY_SUCCESS) {
        mode = static_cast<ScreenGamutMap>(hdiMode);
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

const HDRCapability& RSScreen::GetHDRCapability()
{
    hdrCapability_.maxLum = 1000; // maxLum now is mock data
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
} // namespace impl
} // namespace Rosen
} // namespace OHOS
