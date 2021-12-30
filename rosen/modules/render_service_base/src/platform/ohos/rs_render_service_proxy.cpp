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

#include "rs_render_service_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

namespace OHOS {
namespace Rosen {

RSRenderServiceProxy::RSRenderServiceProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIRenderService>(impl)
{
}

void RSRenderServiceProxy::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if(!data.WriteInterfaceToken(RSRenderServiceProxy::GetDescriptor())) {
        return;
    }

    if (!data.WriteParcelable(transactionData.get())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t err = Remote()->SendRequest(RSIRenderService::COMMIT_TRANSACTION, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

sptr<Surface> RSRenderServiceProxy::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteUint64(config.id)) {
        return nullptr;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    int32_t err = Remote()->SendRequest(RSIRenderService::CREATE_SURFACE, data, reply, option);
    if (err != NO_ERROR) {
        return nullptr;
    }
    sptr<IRemoteObject> surfaceObject = reply.ReadRemoteObject();
    sptr<IBufferProducer> bp = iface_cast<IBufferProducer>(surfaceObject);
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bp);
    return surface;
}

ScreenId RSRenderServiceProxy::GetDefaultScreenId()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if(!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return INVALID_SCREEN_ID;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    int32_t err = Remote()->SendRequest(RSIRenderService::GET_DEFAULT_SCREEN_ID, data, reply, option);
    if (err != NO_ERROR) {
        return INVALID_SCREEN_ID;
    }

    ScreenId id = reply.ReadUint64();
    return id;
}

ScreenId RSRenderServiceProxy::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if(!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return INVALID_SCREEN_ID;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteString(name);
    data.WriteUint32(width);
    data.WriteUint32(height);
    auto producer = surface->GetProducer();
    data.WriteRemoteObject(producer->AsObject());
    data.WriteUint64(mirrorId);
    data.WriteInt32(flags);
    int32_t err = Remote()->SendRequest(RSIRenderService::CREATE_VIRTUAL_DISPLAY, data, reply, option);
    if (err != NO_ERROR) {
        return INVALID_SCREEN_ID;
    }

    ScreenId id = reply.ReadUint64();
    return id;
}

void RSRenderServiceProxy::RemoveVirtualScreen(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if(!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    data.WriteUint64(id);
    int32_t err = Remote()->SendRequest(RSIRenderService::REMOVE_VIRTUAL_DISPLAY, data, reply, option);
    if (err != NO_ERROR) {
        // TODO: Error log.
    }
}

void RSRenderServiceProxy::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    if (callback == nullptr) {
        // TODO: Error Log
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if(!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    data.WriteRemoteObject(callback->AsObject());
    int32_t err = Remote()->SendRequest(RSIRenderService::SET_SCREEN_CHANGE_CALLBACK, data, reply, option);
    if (err != NO_ERROR) {
        // TODO: Error log.
    }
}

void RSRenderServiceProxy::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint64(id);
    data.WriteUint32(modeId);
    int32_t err = Remote()->SendRequest(RSIRenderService::SET_SCREEN_ACTIVE_MODE, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

void RSRenderServiceProxy::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint64(id);
    data.WriteUint32(static_cast<uint32_t>(status));
    int32_t err = Remote()->SendRequest(RSIRenderService::SET_SCREEN_POWER_STATUS, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

RSScreenModeInfo RSRenderServiceProxy::GetScreenActiveMode(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSScreenModeInfo screenModeInfo;

    if (!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return screenModeInfo;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint64(id);
    int32_t err = Remote()->SendRequest(RSIRenderService::GET_SCREEN_ACTIVE_MODE, data, reply, option);
    if (err != NO_ERROR) {
        return screenModeInfo;
    }

    sptr<RSScreenModeInfo> pScreenModeInfo(reply.ReadParcelable<RSScreenModeInfo>());
    if (pScreenModeInfo == nullptr) {
        return screenModeInfo;
    }
    screenModeInfo = *pScreenModeInfo;
    return screenModeInfo;
}

std::vector<RSScreenModeInfo> RSRenderServiceProxy::GetScreenSupportedModes(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<RSScreenModeInfo> screenSupportedModes;

    if(!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return screenSupportedModes;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint64(id);
    int32_t err = Remote()->SendRequest(RSIRenderService::GET_SCREEN_SUPPORTED_MODES, data, reply, option);
    if (err != NO_ERROR) {
        return screenSupportedModes;
    }

    uint64_t modeCount = reply.ReadUint64();
    screenSupportedModes.resize(modeCount);
    for (uint64_t modeIndex = 0; modeIndex < modeCount; modeIndex++) {
        sptr<RSScreenModeInfo> itemScreenMode = reply.ReadParcelable<RSScreenModeInfo>();
        if (itemScreenMode == nullptr) {
            continue;
        } else {
            screenSupportedModes[modeIndex] = *itemScreenMode;
        }
    }
    return screenSupportedModes;
}

RSScreenCapability RSRenderServiceProxy::GetScreenCapability(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSScreenCapability screenCapability;
    if (!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return screenCapability;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint64(id);
    int32_t err = Remote()->SendRequest(RSIRenderService::GET_SCREEN_CAPABILITY, data, reply, option);
    if (err != NO_ERROR) {
        return screenCapability;
    }

    sptr<RSScreenCapability> pScreenCapability(reply.ReadParcelable<RSScreenCapability>());
    if (pScreenCapability == nullptr) {
        return screenCapability;
    }
    screenCapability = *pScreenCapability;
    return screenCapability;
}

ScreenPowerStatus RSRenderServiceProxy::GetScreenPowerStatus(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return INVAILD_POWER_STATUS;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint64(id);
    int32_t err = Remote()->SendRequest(RSIRenderService::GET_SCREEN_POWER_STATUS, data, reply, option);
    if (err != NO_ERROR) {
        return INVAILD_POWER_STATUS;
    }
    return static_cast<ScreenPowerStatus>(reply.ReadUint32());
}

RSScreenData RSRenderServiceProxy::GetScreenData(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSScreenData screenData;
    if (!data.WriteInterfaceToken(RSIRenderService::GetDescriptor())) {
        return screenData;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint64(id);
    int32_t err = Remote()->SendRequest(RSIRenderService::GET_SCREEN_DATA, data, reply, option);
    if (err != NO_ERROR) {
        return screenData;
    }
    sptr<RSScreenData> pScreenData(reply.ReadParcelable<RSScreenData>());
    if (pScreenData == nullptr) {
        return screenData;
    }
    screenData = *pScreenData;
    return screenData;
}
} // namespace Rosen
} // namespace OHOS
