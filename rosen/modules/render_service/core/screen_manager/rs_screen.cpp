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
        RS_LOGW("init physical: {id: %{public}" PRIu64 ", w * h: [%{public}u * %{public}u], "
            "screenType: %{public}u}", id_, width_, height_, screenType_);
    }
    capability_.props.clear();
}

RSScreen::RSScreen(const VirtualScreenConfigs &configs)
    : id_(configs.id),
      mirroredId_(configs.mirrorId),
      name_(configs.name),
      width_(configs.width),
      height_(configs.height),
      isVirtual_(true),
      producerSurface_(configs.surface),
      pixelFormat_(configs.pixelFormat),
      screenType_(RSScreenType::VIRTUAL_TYPE_SCREEN),
      whiteList_(configs.whiteList)
{
    VirtualScreenInit();
    RS_LOGW("init virtual screen: {id: %{public}" PRIu64 ", mirroredId: %{public}" PRIu64
        ", w * h: [%{public}u * %{public}u], name: %{public}s, screenType: %{public}u, whiteList size: %{public}zu}",
        id_, mirroredId_, width_, height_, name_.c_str(), screenType_, whiteList_.size());
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
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to CreateHdiScreens.",
            __func__, id_);
        return;
    }

    hdiScreen_->Init();
    if (hdiScreen_->GetScreenSupportedModes(supportedModes_) < 0) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenSupportedModes.",
            __func__, id_);
    }

    if (hdiScreen_->GetHDRCapabilityInfos(hdrCapability_) < 0) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetHDRCapabilityInfos.",
            __func__, id_);
    }
    std::transform(hdrCapability_.formats.begin(), hdrCapability_.formats.end(),
                   back_inserter(supportedPhysicalHDRFormats_),
                   [](GraphicHDRFormat item) -> ScreenHDRFormat { return HDI_HDR_FORMAT_TO_RS_MAP[item]; });
    auto status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    if (MultiScreenParam::IsRsSetScreenPowerStatus() || id_ == 0) {
        RS_LOGI("%{public}s: RSScreen(id %{public}" PRIu64 ") start SetScreenPowerStatus to On",
            __func__, id_);
        if (hdiScreen_->SetScreenPowerStatus(status) < 0) {
            RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to SetScreenPowerStatus.",
                __func__, id_);
        } else {
            RS_LOGI("%{public}s: RSScreen(id %{public}" PRIu64 ") end SetScreenPowerStatus to On",
                __func__, id_);
        }
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        phyWidth_ = activeMode->width;
        phyHeight_ = activeMode->height;
        width_ = phyWidth_;
        height_ = phyHeight_;
    }
    if (hdiScreen_->GetScreenPowerStatus(status) < 0) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenPowerStatus.",
            __func__, id_);
        powerStatus_ = ScreenPowerStatus::INVALID_POWER_STATUS;
    } else {
        powerStatus_ = static_cast<ScreenPowerStatus>(status);
    }
    if (capability_.type == GraphicInterfaceType::GRAPHIC_DISP_INTF_MIPI) {
        screenType_ = RSScreenType::BUILT_IN_TYPE_SCREEN;
    } else {
        screenType_ = RSScreenType::EXTERNAL_TYPE_SCREEN;
    }
    ScreenCapabilityInit();

    std::vector<GraphicColorGamut> supportedColorGamuts;
    if (hdiScreen_->GetScreenSupportedColorGamuts(supportedColorGamuts) != GRAPHIC_DISPLAY_SUCCESS) {
        RS_LOGE("%{public}s: RSScreen(id %{public}" PRIu64 ") failed to GetScreenSupportedColorGamuts.",
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
    screenBacklightLevel_ = GetScreenBacklight();
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
    return id_;
}

ScreenId RSScreen::MirroredId() const
{
    return mirroredId_;
}

void RSScreen::SetMirror(ScreenId mirroredId)
{
    mirroredId_ = mirroredId;
}

const std::string& RSScreen::Name() const
{
    return name_;
}

uint32_t RSScreen::Width() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return width_;
}

uint32_t RSScreen::Height() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return height_;
}

uint32_t RSScreen::PhyWidth() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return phyWidth_;
}

uint32_t RSScreen::PhyHeight() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return phyHeight_;
}

bool RSScreen::IsSamplingOn() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return isSamplingOn_;
}

float RSScreen::GetSamplingTranslateX() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return samplingTranslateX_;
}

float RSScreen::GetSamplingTranslateY() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return samplingTranslateY_;
}

float RSScreen::GetSamplingScale() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return samplingScale_;
}

RectI RSScreen::GetActiveRect() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return activeRect_;
}

RectI RSScreen::GetMaskRect() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return maskRect_;
}

RectI RSScreen::GetReviseRect() const
{
    std::shared_lock<std::shared_mutex> lock(screenMutex_);
    return reviseRect_;
}

bool RSScreen::IsEnable() const
{
    if (id_ == INVALID_SCREEN_ID) {
        return false;
    }

    std::lock_guard<std::mutex> lock(producerSurfaceMutex_);
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
        RS_LOGE("%{public}s: set fails because the index is out of bounds.", __func__);
        return StatusCode::INVALID_ARGUMENTS;
    }
    RS_LOGW_IF(DEBUG_SCREEN, "RSScreen set active mode: %{public}u", modeId);
    int32_t selectModeId = supportedModes_[modeId].id;
    if (hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) < 0) {
        RS_LOGE("%{public}s: Hdi SetScreenMode fails.", __func__);
        return StatusCode::SET_RATE_ERROR;
    }
    auto activeMode = GetActiveMode();
    if (activeMode) {
        std::unique_lock<std::shared_mutex> lock(screenMutex_);
        phyWidth_ = activeMode->width;
        phyHeight_ = activeMode->height;
        lock.unlock();
        WriteHisyseventEpsLcdInfo(activeMode.value());
    }
    return StatusCode::SUCCESS;
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
        static_cast<uint32_t>(activeRect.x + activeRect.w) > width_ ||
        static_cast<uint32_t>(activeRect.y + activeRect.h) > height_) {
        RS_LOGW("%{public}s failed:, for activeRect: "
            "(%{public}" PRId32 ", %{public}" PRId32 ", %{public}" PRId32 ", %{public}" PRId32 ")",
            __func__, activeRect.x, activeRect.y, activeRect.w, activeRect.h);
        return StatusCode::INVALID_ARGUMENTS;
    }

    std::unique_lock<std::shared_mutex> lock(screenMutex_);
    activeRect_ = RectI(activeRect.x, activeRect.y, activeRect.w, activeRect.h);
    RS_LOGI("%{public}s success, activeRect: (%{public}" PRId32 ", %{public}" PRId32 ", "
        "%{public}" PRId32 ", %{public}" PRId32 ")", __func__, activeRect.x, activeRect.y, activeRect.w, activeRect.h);
    GraphicIRect reviseRect = activeRect;
    if (!CalculateMaskRectAndReviseRect(activeRect, reviseRect)) {
        RS_LOGW("CalculateMaskRect failed or not need");
    }
    reviseRect_ = RectI(reviseRect.x, reviseRect.y, reviseRect.w, reviseRect.h);
    lock.unlock();

    if (hdiScreen_->SetScreenActiveRect(reviseRect) < 0) {
        RS_LOGE("%{public}s failed: hdi SetScreenActiveRect failed, activeRect with revise:"
            "(%{public}" PRId32 ", %{public}" PRId32 ", %{public}" PRId32 ", %{public}" PRId32 ")",
            __func__, reviseRect.x, reviseRect.y, reviseRect.w, reviseRect.h);
        return StatusCode::HDI_ERROR;
    }
    RS_LOGI("%{public}s success, reviseRect: (%{public}" PRId32 ", %{public}" PRId32 ", "
        "%{public}" PRId32 ", %{public}" PRId32 ")", __func__, reviseRect.x, reviseRect.y, reviseRect.w, reviseRect.h);
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
        maskRect_ = (static_cast<uint32_t>(activeRect.h) == height_) ? rect[0] : rect[1];
        // Take the minimum rectangular area containing two rectangles
        reviseRect.x = std::clamp(activeRect.x, 0, std::min(activeRect.x, maskRect_.left_));
        reviseRect.y = std::clamp(activeRect.y, 0, std::min(activeRect.y, maskRect_.top_));
        reviseRect.w = std::max(activeRect.x + activeRect.w, maskRect_.left_ + maskRect_.width_) - reviseRect.x;
        reviseRect.h = std::max(activeRect.y + activeRect.h, maskRect_.top_ + maskRect_.height_) - reviseRect.y;
        RS_LOGI("%{public}s success, maskRect: %{public}s", __func__, maskRect_.ToString().c_str());
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

    std::shared_lock<std::shared_mutex> sharedLock(screenMutex_);
    if ((width == 0 || height == 0) ||
        (width == width_ && height == height_) ||
        (width > phyWidth_ || height > phyHeight_)) {
        RS_LOGD("%{public}s: width: %{public}d, height: %{public}d.", __func__, width, height);
        return;
    }
    sharedLock.unlock();

    if (hdiScreen_->SetScreenOverlayResolution(width, height) < 0) {
        RS_LOGE("%{public}s: hdi set screen rog resolution failed.", __func__);
    }
    std::lock_guard<std::shared_mutex> lock(screenMutex_);
    width_ = width;
    height_ = height;
    RS_LOGI("%{public}s: RSScreen(id %{public}" PRIu64 "), width: %{public}d,"
        " height: %{public}d, phywidth: %{public}d, phyHeight: %{public}d.",
	    __func__, id_, width_, height_, phyWidth_, phyHeight_);
}

int32_t RSScreen::SetResolution(uint32_t width, uint32_t height)
{
    RS_LOGI("%{public}s screenId:%{public}" PRIu64 " width: %{public}u height: %{public}u", __func__, id_, width,
            height);
    std::lock_guard<std::shared_mutex> lock(screenMutex_);
    if (IsVirtual()) {
        width_ = width;
        height_ = height;
        return StatusCode::SUCCESS;
    }
    if (width < phyWidth_ || height < phyHeight_) {
        RS_LOGE("%{public}s phyWidth: %{public}u phyHeight: %{public}u", __func__, phyWidth_, phyHeight_);
        return StatusCode::INVALID_ARGUMENTS;
    }
    width_ = width;
    height_ = height;
    isSamplingOn_ = width > phyWidth_ || height > phyHeight_;
    if (isSamplingOn_ && width_ > 0 && height_ > 0) {
        samplingScale_ = std::min(static_cast<float>(phyWidth_) / width_,
            static_cast<float>(phyHeight_) / height_);
        samplingTranslateX_ = (phyWidth_ - width_ * samplingScale_) / 2.f;
        samplingTranslateY_ = (phyHeight_ - height_ * samplingScale_) / 2.f;
        RS_LOGI("%{public}s: sampling is enabled. "
            "scale: %{public}f, translateX: %{public}f, translateY: %{public}f",
            __func__, samplingScale_, samplingTranslateX_, samplingTranslateY_);
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

    RS_LOGW("[UL_POWER]RSScreen_%{public}" PRIu64 " SetPowerStatus: %{public}u.", id_, powerStatus);
    RS_TRACE_NAME_FMT("[UL_POWER]Screen_%llu SetPowerStatus %u", id_, powerStatus);
    hasLogBackLightAfterPowerStatusChanged_ = false;
    if (hdiScreen_->SetScreenPowerStatus(static_cast<GraphicDispPowerStatus>(powerStatus)) < 0) {
        RS_LOGW("[UL_POWER] %{public}s failed to set power status", __func__);
        powerStatus_ = ScreenPowerStatus::INVALID_POWER_STATUS;
        return StatusCode::HDI_ERROR;
    }
    powerStatus_ = static_cast<ScreenPowerStatus>(powerStatus);

    RS_LOGW("[UL_POWER]RSScreen_%{public}" PRIu64 " SetPowerStatus: %{public}u done.", id_, powerStatus);
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
        RS_LOGE("%{public}s: id: %{public}" PRIu64 " hdiScreen is null.", __func__, id_);
        return {};
    }

    if (hdiScreen_->GetScreenMode(modeId) < 0) {
        RS_LOGE("%{public}s: id: %{public}" PRIu64 " GetScreenMode failed.", __func__, id_);
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

uint32_t RSScreen::GetPowerStatus()
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support GetPowerStatus.", __func__);
        return ScreenPowerStatus::INVALID_POWER_STATUS;
    }

    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return INVALID_POWER_STATUS;
    }

    if (powerStatus_ != ScreenPowerStatus::INVALID_POWER_STATUS) {
        return static_cast<uint32_t>(powerStatus_);
    }

    auto status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    if (hdiScreen_->GetScreenPowerStatus(status) < 0) {
        powerStatus_ = ScreenPowerStatus::INVALID_POWER_STATUS;
        RS_LOGE("%{public}s failed to get screen powerStatus", __func__);
        return INVALID_POWER_STATUS;
    }
    powerStatus_ = static_cast<ScreenPowerStatus>(status);
    RS_LOGW("%{public}s cached powerStatus is INVALID_POWER_STATUS and GetScreenPowerStatus %{public}d",
        __func__, static_cast<uint32_t>(status));
    return static_cast<uint32_t>(status);
}

std::shared_ptr<HdiOutput> RSScreen::GetOutput() const
{
    return hdiOutput_;
}

sptr<Surface> RSScreen::GetProducerSurface() const
{
    std::lock_guard<std::mutex> lock(producerSurfaceMutex_);
    return producerSurface_;
}

void RSScreen::SetProducerSurface(sptr<Surface> producerSurface)
{
    std::lock_guard<std::mutex> lock(producerSurfaceMutex_);
    producerSurface_ = producerSurface;
    isVirtualSurfaceUpdateFlag_ = true;
}

bool RSScreen::GetAndResetVirtualSurfaceUpdateFlag()
{
    return isVirtualSurfaceUpdateFlag_.exchange(false);
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
        dumpString += "mirroredId=";
        dumpString += (mirroredId_ == INVALID_SCREEN_ID) ? "INVALID_SCREEN_ID" : std::to_string(mirroredId_);
        dumpString += ", ";
        std::shared_lock<std::shared_mutex> screenLock(screenMutex_, std::defer_lock);
        std::shared_lock<std::shared_mutex> skipFrameLock(skipFrameMutex_, std::defer_lock);
        std::lock(screenLock, skipFrameLock);
        AppendFormat(dumpString, ", render resolution=%dx%d, isVirtual=true, skipFrameInterval=%d"
            ", expectedRefreshRate=%d, skipFrameStrategy=%d\n",
            width_, height_, skipFrameInterval_, expectedRefreshRate_, skipFrameStrategy_);
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
        std::shared_lock<std::shared_mutex> screenLock(screenMutex_, std::defer_lock);
        std::shared_lock<std::shared_mutex> skipFrameLock(skipFrameMutex_, std::defer_lock);
        std::lock(screenLock, skipFrameLock);
        AppendFormat(dumpString,
            ", render resolution=%dx%d, physical resolution=%dx%d, isVirtual=false, skipFrameInterval=%d"
            ", expectedRefreshRate=%d, skipFrameStrategy=%d\n",
            width_, height_, phyWidth_, phyHeight_, skipFrameInterval_, expectedRefreshRate_, skipFrameStrategy_);
        screenLock.unlock();
        skipFrameLock.unlock();
        ModeInfoDump(dumpString);
        CapabilityDump(dumpString);
        AppendFormat(dumpString,
                     "isSamplingOn=%d, samplingScale=%.2f, samplingTranslateX=%.2f, samplingTranslateY=%.2f\n",
                     isSamplingOn_, samplingScale_, samplingTranslateX_, samplingTranslateY_);
        AppendFormat(dumpString, "enableVisibleRect=%d, mainScreenVisibleRect_=[%d,%d,%d,%d]\n",
                     enableVisibleRect_.load(), mainScreenVisibleRect_.x, mainScreenVisibleRect_.y,
                     mainScreenVisibleRect_.w, mainScreenVisibleRect_.h);
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
    std::lock_guard<std::shared_mutex> lock(screenMutex_);
    width_ = width;
    height_ = height;
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
    if (!hasLogBackLightAfterPowerStatusChanged_) {
        RS_LOGI("%{public}s id: %{public}" PRIu64 ", level is %{public}u, current level is %{public}d", __func__, id_,
                level, screenBacklightLevel_);
    }

    RS_LOGD("%{public}s id: %{public}" PRIu64 ", level is %{public}u", __func__, id_, level);
    if (hdiScreen_->SetScreenBacklight(level) < 0) {
        RS_LOGE("RSScreen_%{public}" PRIu64 " SetScreenBacklight error.", id_);
        return;
    }
    if (!hasLogBackLightAfterPowerStatusChanged_) {
        RS_LOGI("%{public}s id: %{public}" PRIu64 ", level is %{public}u done, last level is %{public}d", __func__, id_,
                level, screenBacklightLevel_);
        hasLogBackLightAfterPowerStatusChanged_ = true;
    }
    std::lock_guard<std::shared_mutex> lock(screenMutex_);
    screenBacklightLevel_ = static_cast<int32_t>(level);
}

int32_t RSScreen::GetScreenBacklight() const
{
    if (IsVirtual()) {
        RS_LOGW("%{public}s: virtual screen not support GetScreenBacklight.", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }
    uint32_t level = 0;
    if (std::shared_lock<std::shared_mutex> lock(screenMutex_);
        screenBacklightLevel_ != INVALID_BACKLIGHT_VALUE) {
        return screenBacklightLevel_;
    }
    if (!hdiScreen_) {
        RS_LOGE("%{public}s failed, hdiScreen_ is nullptr", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }
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

void RSScreen::SetScreenCorrection(ScreenRotation screenRotation)
{
    RS_LOGI("%{public}s: RSScreen(id %{public}" PRIu64 ") ,ScreenRotation: %{public}d.", __func__,
        id_, static_cast<uint32_t>(screenRotation));
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

const RSScreenType& RSScreen::GetScreenType() const
{
    return screenType_;
}

void RSScreen::SetScreenSkipFrameInterval(uint32_t skipFrameInterval)
{
    std::lock_guard<std::shared_mutex> lock(skipFrameMutex_);
    skipFrameInterval_ = skipFrameInterval;
    skipFrameStrategy_ = SKIP_FRAME_BY_INTERVAL;
}

void RSScreen::SetScreenExpectedRefreshRate(uint32_t expectedRefreshRate)
{
    std::lock_guard<std::shared_mutex> lock(skipFrameMutex_);
    expectedRefreshRate_ = expectedRefreshRate;
    skipFrameStrategy_ = SKIP_FRAME_BY_REFRESH_RATE;
}

void RSScreen::SetEqualVsyncPeriod(bool isEqualVsyncPeriod)
{
    std::lock_guard<std::shared_mutex> lock(skipFrameMutex_);
    isEqualVsyncPeriod_ = isEqualVsyncPeriod;
}

uint32_t RSScreen::GetScreenSkipFrameInterval() const
{
    std::shared_lock<std::shared_mutex> lock(skipFrameMutex_);
    return skipFrameInterval_;
}

uint32_t RSScreen::GetScreenExpectedRefreshRate() const
{
    std::shared_lock<std::shared_mutex> lock(skipFrameMutex_);
    return expectedRefreshRate_;
}

SkipFrameStrategy RSScreen::GetScreenSkipFrameStrategy() const
{
    std::shared_lock<std::shared_mutex> lock(skipFrameMutex_);
    return skipFrameStrategy_;
}

bool RSScreen::GetEqualVsyncPeriod() const
{
    std::shared_lock<std::shared_mutex> lock(skipFrameMutex_);
    return isEqualVsyncPeriod_;
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

bool RSScreen::SetVirtualMirrorScreenScaleMode(ScreenScaleMode scaleMode)
{
    if (IsVirtual()) {
        scaleMode_ = scaleMode;
        return true;
    }
    return false;
}

ScreenScaleMode RSScreen::GetScaleMode() const
{
    return scaleMode_;
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
    if (result == GRAPHIC_DISPLAY_SUCCESS) {
        currentPhysicalColorGamutIdx_ = curIdx;
        return StatusCode::SUCCESS;
    }
    return StatusCode::HDI_ERROR;
}

const std::unordered_set<uint64_t>& RSScreen::GetWhiteList() const
{
    return whiteList_;
}

void RSScreen::SetBlackList(const std::unordered_set<uint64_t>& blackList)
{
    std::lock_guard<std::mutex> lock(blackListMutex_);
    blackList_ = blackList;
}

void RSScreen::SetTypeBlackList(const std::unordered_set<uint8_t>& typeBlackList)
{
    std::lock_guard<std::mutex> lock(typeBlackListMutex_);
    typeBlackList_ = typeBlackList;
}

void RSScreen::AddBlackList(const std::vector<uint64_t>& blackList)
{
    std::lock_guard<std::mutex> lock(blackListMutex_);
    blackList_.insert(blackList.cbegin(), blackList.cend());
}

void RSScreen::RemoveBlackList(const std::vector<uint64_t>& blackList)
{
    std::lock_guard<std::mutex> lock(blackListMutex_);
    for (const auto& list : blackList) {
        blackList_.erase(list);
    }
}

void RSScreen::SetCastScreenEnableSkipWindow(bool enable)
{
    skipWindow_ = enable;
}

bool RSScreen::GetCastScreenEnableSkipWindow()
{
    return skipWindow_;
}

const std::unordered_set<uint64_t> RSScreen::GetBlackList() const
{
    std::lock_guard<std::mutex> lock(blackListMutex_);
    return blackList_;
}

const std::unordered_set<uint8_t> RSScreen::GetTypeBlackList() const
{
    std::lock_guard<std::mutex> lock(typeBlackListMutex_);
    return typeBlackList_;
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
    std::lock_guard<std::mutex> lock(securityExemptionMutex_);
    securityExemptionList_ = securityExemptionList;
}

const std::vector<uint64_t> RSScreen::GetSecurityExemptionList() const
{
    std::lock_guard<std::mutex> lock(securityExemptionMutex_);
    return securityExemptionList_;
}

int32_t RSScreen::SetSecurityMask(std::shared_ptr<Media::PixelMap> securityMask)
{
    if (!IsVirtual()) {
        RS_LOGW("%{public}s not virtual screen", __func__);
        return SCREEN_NOT_FOUND;
    }
    std::lock_guard<std::mutex> lock(securityMaskMutex_);
    securityMask_ = securityMask;
    return SUCCESS;
}

std::shared_ptr<Media::PixelMap> RSScreen::GetSecurityMask() const
{
    std::lock_guard<std::mutex> lock(securityMaskMutex_);
    return securityMask_;
}

void RSScreen::SetEnableVisibleRect(bool enable)
{
    enableVisibleRect_ = enable;
}

bool RSScreen::GetEnableVisibleRect() const
{
    return enableVisibleRect_;
}

void RSScreen::SetMainScreenVisibleRect(const Rect& mainScreenRect)
{
    std::lock_guard<std::mutex> lock(visibleRectMutex_);
    mainScreenVisibleRect_ = mainScreenRect;
}

Rect RSScreen::GetMainScreenVisibleRect() const
{
    std::lock_guard<std::mutex> lock(visibleRectMutex_);
    return mainScreenVisibleRect_;
}

bool RSScreen::GetVisibleRectSupportRotation() const
{
    return isSupportRotation_;
}

void RSScreen::SetVisibleRectSupportRotation(bool supportRotation)
{
    isSupportRotation_ = supportRotation;
}

bool RSScreen::SetVirtualScreenStatus(VirtualScreenStatus screenStatus)
{
    if (IsVirtual()) {
        screenStatus_ = screenStatus;
        return true;
    }
    return false;
}

VirtualScreenStatus RSScreen::GetVirtualScreenStatus() const
{
    return screenStatus_;
}

bool RSScreen::GetDisplayPropertyForHardCursor()
{
    std::call_once(hardCursorSupportedFlag_, [this]() {
        if (hdiScreen_) {
            isHardCursorSupport_ = hdiScreen_->GetDisplayPropertyForHardCursor(id_);
        }
        RS_LOGI("%{public}s, RSScreen(id %{public}" PRIu64 ", isHardCursorSupport:%{public}d)",
            __func__, id_, isHardCursorSupport_);
    });
    return isHardCursorSupport_;
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
        RS_LOGE("%{public}s: id: %{public}" PRIu64 " is not physical display", __func__, id_);
        return HDI_ERROR;
    }

    int32_t ret = hdiScreen_->GetDisplayIdentificationData(outPort, edidData);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        RS_LOGE("%{public}s: id %{public}" PRIu64 " call hid interface failed(%{public}d)",
            __func__, id_, ret);
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
} // namespace impl
} // namespace Rosen
} // namespace OHOS
