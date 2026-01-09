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

#include "graphic_feature_param_manager.h"
#include "rs_screen_manager_agent.h"
#include <cstdint>

#include "screen_manager/rs_screen.h"
#include "transaction/rs_client_to_render_connection.h"

namespace OHOS {
namespace Rosen {
void RSScreenManagerAgentListener::OnScreenConnected(ScreenId id,
    ScreenChangeReason reason, sptr<IRemoteObject> remoteConn)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenChangeCallback_) {
        RS_LOGW("RSScreenManagerAgentListener::%{public}s screenChangeCallback is nullptr.", __func__);
        return;
    }
    if (reason == ScreenChangeReason::HWCDEAD && (id == 0 || !MultiScreenParam::IsRsReportHwcDead())) {
        RS_LOGI("RSScreenManagerAgentListener::%{public}s don't invoke callback, screenId:%{public}" PRIu64,
                __func__, id);
        return;
    }
    RS_LOGI("%{public}s id:%{public}" PRIu64 "event connected reason %{public}u.", __func__, id,
        static_cast<uint8_t>(reason));
    screenChangeCallback_->OnScreenChanged(id, ScreenEvent::CONNECTED, reason, remoteConn);
}

void RSScreenManagerAgentListener::OnScreenDisconnected(ScreenId id, ScreenChangeReason reason)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!screenChangeCallback_) {
        RS_LOGW("RSScreenManagerAgentListener::%{public}s screenChangeCallback is nullptr.", __func__);
        return;
    }

    if (reason == ScreenChangeReason::HWCDEAD) {
        RS_LOGI("RSScreenManagerAgentListener::%{public}s don't invoke callback, screenId:%{public}" PRIu64,
                __func__, id);
        return;
    }
    RS_LOGI("%{public}s: id:%{public}" PRIu64 "event disconnected reason %{public}u.", __func__, id,
        static_cast<uint8_t>(reason));
    screenChangeCallback_->OnScreenChanged(id, ScreenEvent::DISCONNECTED, reason);
}

void RSScreenManagerAgentListener::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> screenChangeCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    screenChangeCallback_ = screenChangeCallback;
}

RSScreenManagerAgent::RSScreenManagerAgent(sptr<RSScreenManager> screenManager) : screenManager_(screenManager)
{
    agentListener_ = sptr<RSScreenManagerAgentListener>::MakeSptr();
    if (screenManager_ && agentListener_) {
        screenManager_->RegisterAgentListener(agentListener_);
    }
}

RSScreenManagerAgent::~RSScreenManagerAgent()
{
    if (screenManager_ && agentListener_) {
        screenManager_->UnRegisterAgentListener(agentListener_);
    }
}

int32_t RSScreenManagerAgent::SetScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback)
{
    if (screenManager_ && callback) {
        screenManager_->ExecuteCallback(callback);
    }
    agentListener_->SetScreenChangeCallback(callback);
    return SUCCESS;
}

void RSScreenManagerAgent::CleanVirtualScreens()
{
    if (!screenManager_) {
        RS_LOGW("RSScreenManagerAgent::CleanVirtualScreens screenManager_ is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto id : virtualScreenIds_) {
        screenManager_->RemoveVirtualScreen(id);
    }
    virtualScreenIds_.clear();
}

ErrCode RSScreenManagerAgent::GetDefaultScreenId(uint64_t& screenId)
{
    if (!screenManager_) {
        RS_LOGW("RSScreenManagerAgent::GetDefaultScreenId screenManager_ is nullptr");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_OPERATION;
    }
    screenId = screenManager_->GetDefaultScreenId();
    return ERR_OK;
}

ErrCode RSScreenManagerAgent::GetActiveScreenId(uint64_t& screenId)
{
    if (!screenManager_) {
        RS_LOGW("RSScreenManagerAgent::GetActiveScreenId screenManager_ is nullptr");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_OPERATION;
    }
    screenId = screenManager_->GetActiveScreenId();
    return ERR_OK;
}

std::vector<ScreenId> RSScreenManagerAgent::GetAllScreenIds()
{
    if (!screenManager_) {
        RS_LOGW("RSScreenManagerAgent::GetAllScreenIds screenManager_ is nullptr");
        return {};
    }
    return screenManager_->GetAllScreenIds();
}

ScreenId RSScreenManagerAgent::CreateVirtualScreen(const std::string &name, uint32_t width, uint32_t height,
    sptr<Surface> surface, ScreenId associatedScreenId, int32_t flags, std::vector<NodeId> whiteList)
{
    if (!screenManager_) {
        RS_LOGW("RSScreenManagerAgent::CreateVirtualScreen screenManager_ is nullptr");
        return INVALID_SCREEN_ID;
    }
    auto screenId = screenManager_->CreateVirtualScreen(
        name, width, height, surface, associatedScreenId, flags, whiteList);
    if (screenId != INVALID_SCREEN_ID) {
        std::lock_guard<std::mutex> lock(mutex_);
        virtualScreenIds_.insert(screenId);
    }
    return screenId;
}

void RSScreenManagerAgent::RemoveVirtualScreen(ScreenId id)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return;
    }
    screenManager_->RemoveVirtualScreen(id);
    std::lock_guard<std::mutex> lock(mutex_);
    virtualScreenIds_.erase(id);
}

int32_t RSScreenManagerAgent::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenSurface(id, surface);
}

uint32_t RSScreenManagerAgent::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenActiveMode(id, modeId);
}

void RSScreenManagerAgent::GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return;
    }
    screenManager_->GetScreenActiveMode(id, screenModeInfo);
}

std::vector<RSScreenModeInfo> RSScreenManagerAgent::GetScreenSupportedModes(ScreenId id) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return std::vector<RSScreenModeInfo>();
    }
    return screenManager_->GetScreenSupportedModes(id);
}

RSScreenCapability RSScreenManagerAgent::GetScreenCapability(ScreenId id) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return RSScreenCapability();
    }
    return screenManager_->GetScreenCapability(id);
}

ScreenPowerStatus RSScreenManagerAgent::GetScreenPowerStatus(ScreenId id) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return ScreenPowerStatus::INVALID_POWER_STATUS;
    }
    return screenManager_->GetScreenPowerStatus(id);
}

RSScreenData RSScreenManagerAgent::GetScreenData(ScreenId id) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return RSScreenData();
    }
    return screenManager_->GetScreenData(id);
}

int32_t RSScreenManagerAgent::GetScreenBacklight(ScreenId id) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }
    return screenManager_->GetScreenBacklight(id);
}

void RSScreenManagerAgent::SetScreenBacklight(ScreenId id, uint32_t level)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return;
    }
    screenManager_->SetScreenBacklight(id, level);
}

void RSScreenManagerAgent::DisablePowerOffRenderControl(ScreenId id)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return;
    }
    screenManager_->DisablePowerOffRenderControl(id);
}

int32_t RSScreenManagerAgent::SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    if (blackList.empty()) {
        RS_LOGW("%{public}s blackList is empty", __func__);
    }
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenBlackList(id, blackList);
}

int32_t RSScreenManagerAgent::SetVirtualScreenTypeBlackList(ScreenId id, const std::vector<uint8_t>& typeBlackList)
{
    if (typeBlackList.empty()) {
        RS_LOGW("%{public}s typeBlackList is empty", __func__);
    }
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenTypeBlackList(id, typeBlackList);
}

int32_t RSScreenManagerAgent::AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    if (blackList.empty()) {
        RS_LOGW("%{public}s blacklist is empty", __func__);
        return StatusCode::BLACKLIST_IS_EMPTY;
    }
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->AddVirtualScreenBlackList(id, blackList);
}

int32_t RSScreenManagerAgent::RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList)
{
    if (blackList.empty()) {
        RS_LOGW("RemoveVirtualScreenBlackList blackList is empty.");
        return StatusCode::BLACKLIST_IS_EMPTY;
    }
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->RemoveVirtualScreenBlackList(id, blackList);
}

int32_t RSScreenManagerAgent::SetScreenSwitchingNotifyCallback(sptr<RSIScreenSwitchingNotifyCallback> callback)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenSwitchingNotifyCallback(callback);
}

int32_t RSScreenManagerAgent::SetVirtualScreenSecurityExemptionList(ScreenId id,
    const std::vector<NodeId>& securityExemptionList)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

int32_t RSScreenManagerAgent::SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenSecurityMask(id, securityMask);
}

int32_t RSScreenManagerAgent::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect,
    bool supportRotation)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
}

int32_t RSScreenManagerAgent::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetCastScreenEnableSkipWindow(id, enable);
}

int32_t RSScreenManagerAgent::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetPhysicalScreenResolution(id, width, height);
}

int32_t RSScreenManagerAgent::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenResolution(id, width, height);
}

int32_t RSScreenManagerAgent::SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetRogScreenResolution(id, width, height);
}

void RSScreenManagerAgent::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return;
    }
    screenManager_->SetScreenPowerStatus(id, status);
}

void RSScreenManagerAgent::MarkPowerOffNeedProcessOneFrame()
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return;
    }
    screenManager_->MarkPowerOffNeedProcessOneFrame();
}

int32_t RSScreenManagerAgent::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->ResizeVirtualScreen(id, width, height);
}

RSVirtualScreenResolution RSScreenManagerAgent::GetVirtualScreenResolution(ScreenId id) const
{
    RSVirtualScreenResolution virtualScreenResolution;
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return virtualScreenResolution;
    }
    screenManager_->GetVirtualScreenResolution(id, virtualScreenResolution);
    return virtualScreenResolution;
}

int32_t RSScreenManagerAgent::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenColorGamut(id, modeIdx);
}

int32_t RSScreenManagerAgent::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenColorGamut(id, mode);
}

int32_t RSScreenManagerAgent::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenSupportedColorGamuts(id, mode);
}

int32_t RSScreenManagerAgent::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenGamutMap(id, mode);
}

int32_t RSScreenManagerAgent::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenGamutMap(id, mode);
}

int32_t RSScreenManagerAgent::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenHDRFormat(id, modeIdx);
}

int32_t RSScreenManagerAgent::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenHDRFormat(id, hdrFormat);
}

int32_t RSScreenManagerAgent::GetScreenSupportedHDRFormats(ScreenId id,
    std::vector<ScreenHDRFormat>& hdrFormats) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenSupportedHDRFormats(id, hdrFormats);
}

int32_t RSScreenManagerAgent::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenHDRCapability(id, screenHdrCapability);
}

int32_t RSScreenManagerAgent::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenSupportedMetaDataKeys(id, keys);
}

int32_t RSScreenManagerAgent::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenCorrection(id, screenRotation);
}

bool RSScreenManagerAgent::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

int32_t RSScreenManagerAgent::SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenAutoRotation(id, isAutoRotation);
}

bool RSScreenManagerAgent::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualMirrorScreenScaleMode(id, scaleMode);
}

int32_t RSScreenManagerAgent::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetPixelFormat(id, pixelFormat); 
}

int32_t RSScreenManagerAgent::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetPixelFormat(id, pixelFormat);
}

int32_t RSScreenManagerAgent::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenColorSpace(id, colorSpace);
}

int32_t RSScreenManagerAgent::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenColorSpace(id, colorSpace);
}

int32_t RSScreenManagerAgent::GetScreenSupportedColorSpaces(ScreenId id,
    std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenSupportedColorSpaces(id, colorSpaces);
}

int32_t RSScreenManagerAgent::GetScreenType(ScreenId id, RSScreenType& type) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetScreenType(id, type);
}

int32_t RSScreenManagerAgent::GetDisplayIdentificationData(
    ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData) const
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->GetDisplayIdentificationData(id, outPort, edidData);
}

int32_t RSScreenManagerAgent::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenSkipFrameInterval(id, skipFrameInterval);
}

int32_t RSScreenManagerAgent::SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate,
    uint32_t& actualRefreshRate)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
}

uint32_t RSScreenManagerAgent::SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetScreenActiveRect(id, activeRect);
}

void RSScreenManagerAgent::SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return;
    }
    screenManager_->SetScreenOffset(id, offsetX, offsetY);
}

bool RSScreenManagerAgent::SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return StatusCode::SCREEN_NOT_FOUND;
    }
    return screenManager_->SetVirtualScreenStatus(id, screenStatus);
}

void RSScreenManagerAgent::SetScreenSwitchStatus(ScreenId id, bool status)
{
    if (!screenManager_) {
        RS_LOGW("%{public}s screenManager_ is nullptr", __func__);
        return;
    }
    // temporary: only for main screen
    screenManager_->SetScreenSwitchStatus(0, status);
}

void RSScreenManagerAgent::SetScreenFrameGravity(ScreenId id, int32_t gravity)
{
    if (!screenManager_) {
        return;
    }
    screenManager_->SetScreenFrameGravity(id, gravity);
}

} // namespace Rosen
} // namespace OHOS