/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "rs_interfaces.h"

namespace OHOS {
namespace Rosen {
RSInterfaces &RSInterfaces::GetInstance()
{
    static RSInterfaces instance;
    return instance;
}

RSInterfaces::RSInterfaces() : renderServiceClient_(std::make_unique<RSRenderServiceClient>())
{
}

RSInterfaces::~RSInterfaces() noexcept
{
}

int32_t RSInterfaces::SetFocusAppInfo(FocusAppInfo& info)
{
    int32_t pid = info.pid;
    int32_t uid = info.uid;
    const std::string bundleName = info.bundleName;
    const std::string abilityName = info.abilityName;
    return renderServiceClient_->SetFocusAppInfo(pid, uid, bundleName, abilityName);
}

ScreenId RSInterfaces::GetDefaultScreenId()
{
    return renderServiceClient_->GetDefaultScreenId();
}

std::vector<ScreenId> RSInterfaces::GetAllScreenIds()
{
    return renderServiceClient_->GetAllScreenIds();
}

#if !defined(__gnu_linux__) && !defined(_WIN32) && !defined(__APPLE__)
ScreenId RSInterfaces::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int flags)
{
    return renderServiceClient_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
}

int32_t RSInterfaces::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    return renderServiceClient_->SetVirtualScreenSurface(id, surface);
}
#endif

void RSInterfaces::RemoveVirtualScreen(ScreenId id)
{
    renderServiceClient_->RemoveVirtualScreen(id);
}

int32_t RSInterfaces::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    return renderServiceClient_->SetScreenChangeCallback(callback);
}

bool RSInterfaces::TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, scaleX, scaleY);
}

bool RSInterfaces::TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, scaleX, scaleY);
}

void RSInterfaces::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    renderServiceClient_->SetScreenActiveMode(id, modeId);
}

int32_t RSInterfaces::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    return renderServiceClient_->SetVirtualScreenResolution(id, width, height);
}

RSVirtualScreenResolution RSInterfaces::GetVirtualScreenResolution(ScreenId id)
{
    return renderServiceClient_->GetVirtualScreenResolution(id);
}

void RSInterfaces::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    renderServiceClient_->SetScreenPowerStatus(id, status);
}

RSScreenModeInfo RSInterfaces::GetScreenActiveMode(ScreenId id)
{
    return renderServiceClient_->GetScreenActiveMode(id);
}

std::vector<RSScreenModeInfo> RSInterfaces::GetScreenSupportedModes(ScreenId id)
{
    return renderServiceClient_->GetScreenSupportedModes(id);
}

RSScreenCapability RSInterfaces::GetScreenCapability(ScreenId id)
{
    return renderServiceClient_->GetScreenCapability(id);
}

ScreenPowerStatus RSInterfaces::GetScreenPowerStatus(ScreenId id)
{
    return renderServiceClient_->GetScreenPowerStatus(id);
}

RSScreenData RSInterfaces::GetScreenData(ScreenId id)
{
    return renderServiceClient_->GetScreenData(id);
}

int32_t RSInterfaces::GetScreenBacklight(ScreenId id)
{
    return renderServiceClient_->GetScreenBacklight(id);
}

void RSInterfaces::SetScreenBacklight(ScreenId id, uint32_t level)
{
    renderServiceClient_->SetScreenBacklight(id, level);
}

int32_t RSInterfaces::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    return renderServiceClient_->GetScreenSupportedColorGamuts(id, mode);
}

int32_t RSInterfaces::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    return renderServiceClient_->GetScreenSupportedMetaDataKeys(id, keys);
}

int32_t RSInterfaces::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    return renderServiceClient_->GetScreenColorGamut(id, mode);
}

int32_t RSInterfaces::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    return renderServiceClient_->SetScreenColorGamut(id, modeIdx);
}

int32_t RSInterfaces::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    return renderServiceClient_->SetScreenGamutMap(id, mode);
}

int32_t RSInterfaces::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    return renderServiceClient_->GetScreenGamutMap(id, mode);
}

std::shared_ptr<VSyncReceiver> RSInterfaces::CreateVSyncReceiver(
    const std::string& name,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper)
{
    return renderServiceClient_->CreateVSyncReceiver(name, looper);
}

int32_t RSInterfaces::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    return renderServiceClient_->GetScreenHDRCapability(id, screenHdrCapability);
}

int32_t RSInterfaces::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    return renderServiceClient_->GetScreenType(id, screenType);
}

int32_t RSInterfaces::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    return renderServiceClient_->SetScreenSkipFrameInterval(id, skipFrameInterval);
}

int32_t RSInterfaces::RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback)
{
    return renderServiceClient_->RegisterOcclusionChangeCallback(callback);
}

int32_t RSInterfaces::UnRegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback)
{
    return renderServiceClient_->UnRegisterOcclusionChangeCallback(callback);
}

int32_t RSInterfaces::SetRenderModeChangeCallback(const RenderModeChangeCallback& callback)
{
    return renderServiceClient_->SetRenderModeChangeCallback(callback);
}

void RSInterfaces::UpdateRenderMode(bool isUniRender)
{
    renderServiceClient_->UpdateRenderMode(isUniRender);
}

void RSInterfaces::SetAppWindowNum(uint32_t num)
{
    renderServiceClient_->SetAppWindowNum(num);
}
} // namespace Rosen
} // namespace OHOS
