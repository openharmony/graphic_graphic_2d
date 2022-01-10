/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "screen_manager/screen_types.h"
//#include "string_utils.h"

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
      producerSurface_(configs.surface)
{
}

RSScreen::~RSScreen() noexcept
{
}

void RSScreen::PhysicalScreenInit() noexcept
{
    hdiScreen_ = HdiScreen::CreateHdiScreen(ScreenPhysicalId(id_));
    if (hdiScreen_ == nullptr) {
        HiLog::Error(LOG_LABEL, "%{public}s: RSScreen(id %{public}" PRIu64 ") failed to CreateHdiScreens.",
            __func__, id_);
        return;
    }

    hdiScreen_->Init();
    if (hdiScreen_->GetScreenSuppportedModes(supportedModes_) < 0) {
        HiLog::Error(LOG_LABEL, "%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenSuppportedModes.",
            __func__, id_);
    }
    if (hdiScreen_->GetScreenCapability(capability_) < 0) {
        HiLog::Error(LOG_LABEL, "%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenCapability.",
            __func__, id_);
    }
    auto status = DispPowerStatus::POWER_STATUS_ON;
    if (hdiScreen_->SetScreenPowerStatus(status) < 0) {
        HiLog::Error(LOG_LABEL, "%{public}s: RSScreen(id %{public}" PRIu64 ") failed to SetScreenPowerStatus.",
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

    // TODO: maybe need more information to judge whether this screen is enable.
    return true;
}

bool RSScreen::IsVirtual() const
{
    return isVirtual_;
}

void RSScreen::SetActiveMode(uint32_t modeId)
{
    if (hdiScreen_->SetScreenMode(modeId) < 0) {
        return;
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        width_ = activeMode->width;
        height_ = activeMode->height;
    }
}

void RSScreen::SetPowerStatus(uint32_t powerStatus)
{
    if (hdiScreen_->SetScreenPowerStatus(static_cast<DispPowerStatus>(powerStatus)) < 0) {
        return;
    }
}

std::optional<DisplayModeInfo> RSScreen::GetActiveMode() const
{
    uint32_t modeId = 0;

    if (hdiScreen_ == nullptr) {
        HiLog::Error(LOG_LABEL, "%{public}s: RSScreen(id %{public}" PRIu64 ") hdiScreen is null.",
            __func__, id_);
        return {};
    }

    if (hdiScreen_->GetScreenMode(modeId) < 0) {
        HiLog::Error(LOG_LABEL, "%{public}s: RSScreen(id %{public}" PRIu64 ") GetScreenMode failed.",
            __func__, id_);
        return {};
    }

    if (supportedModes_.empty() || modeId >= supportedModes_.size()) {
        HiLog::Error(LOG_LABEL, "%{public}s: RSScreen(id %{public}" PRIu64 ") exceed mode size.",
            __func__, id_);
        return {};
    }

    return supportedModes_[modeId];
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

void RSScreen::ModeInfoDump(std::string& dumpString)
{
    decltype(supportedModes_.size()) modeIndex = 0;
    for (; modeIndex < supportedModes_.size(); ++modeIndex) {
        // ROSEN::AppendFormat(dumpString, "  supportedMode[%d]: %dx%d, freshrate=%d\n",
        //                     modeIndex, supportedModes_[modeIndex].width,
        //                     supportedModes_[modeIndex].height, supportedModes_[modeIndex].freshRate);
    }
    //std::optional<DisplayModeInfo> activeMode = GetActiveMode();
    // ROSEN::AppendFormat(dumpString, "  activeMode: %dx%d, freshrate=%d\n",
    //                     activeMode->width, activeMode->height, activeMode->freshRate);
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
    // ROSEN::AppendFormat(dumpString, "  capability: name=%s, phywidth=%d, phyheight=%d,"
    //                     "supportlayers=%d, virtualDispCount=%d, propCount=%d, ",
    //                     capability_.name, capability_.phyWidth, capability_.phyHeight,
    //                     capability_.supportLayers, capability_.virtualDispCount, capability_.propertyCount);
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
        // ROSEN::AppendFormat(dumpString, "prop[%d]: name=%s, propid=%d, value=%d\n",
        //                     capability_.props[propIndex].name, capability_.props[propIndex].propId,
        //                     capability_.props[propIndex].value);
    }
}

void RSScreen::PowerStatusDump(DispPowerStatus powerStatus, std::string& dumpString)
{
    dumpString += "powerstatus=";
    switch (powerStatus) {
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
        default:
            dumpString += "INVALID_POWER_STATUS";
            break;
    }
}


void RSScreen::DisplayDump(int32_t screenIndex, std::string& dumpString)
{
    dumpString += "-- ScreenInfo\n";
    if (isVirtual_) {
        dumpString += "screen[" + std::to_string(screenIndex) + "]: ";
        dumpString += "id=";
        dumpString += (id_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID" : std::to_string(id_);
        dumpString += ", ";
        dumpString += "mirrorId=";
        dumpString += (mirrorId_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID" : std::to_string(mirrorId_);
        dumpString += ", ";
        //ROSEN::AppendFormat(dumpString, "%dx%d, isvirtual=true\n", width_, height_);
    } else {
        dumpString += "screen[" + std::to_string(screenIndex) + "]: ";
        dumpString += "id=";
        dumpString += (id_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID" : std::to_string(id_);
        dumpString += ", ";
        PowerStatusDump(powerStatus_, dumpString);
        dumpString += "\n";
        ModeInfoDump(dumpString);
        CapabilityDump(dumpString);
    }
}

void RSScreen::SurfaceDump(int32_t screenIndex, std::string& dumpString)
{
    //hdiOutput_->Dump(dumpString);
}
} // namespace impl
} // namespace Rosen
} // namespace OHOS
