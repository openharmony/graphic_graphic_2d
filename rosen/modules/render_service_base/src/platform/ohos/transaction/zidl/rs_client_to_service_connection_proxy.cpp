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

#include "rs_client_to_service_connection_proxy.h"

#include <algorithm>
#include <cstdint>
#include <message_option.h>
#include <message_parcel.h>
#include <vector>
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_hrp_service.h"
#include "transaction/rs_marshalling_helper.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
static constexpr size_t ASHMEM_SIZE_THRESHOLD = 200 * 1024; // cannot > 500K in TF_ASYNC mode
static constexpr int MAX_RETRY_COUNT = 30;
static constexpr int RETRY_WAIT_TIME_US = 5000; // wait 5ms before retry SendRequest
static constexpr int MAX_SECURITY_EXEMPTION_LIST_NUMBER = 1024; // securityExemptionList size not exceed 1024
static constexpr uint32_t EDID_DATA_MAX_SIZE = 64 * 1024;
static constexpr int MAX_VOTER_SIZE = 100; // SetWindowExpectedRefreshRate map size not exceed 100
static constexpr int ZERO = 0; // empty map size
}

RSClientToServiceConnectionProxy::RSClientToServiceConnectionProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIClientToServiceConnection>(impl)
{
}

ErrCode RSClientToServiceConnectionProxy::GetUniRenderEnabled(bool& enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetUniRenderEnabled: WriteInterfaceToken RSIClientToServiceConnection::GetDescriptor() err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    if (!reply.ReadBool(enable)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetUniRenderEnabled Read enable failed!");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::CreateVSyncConnection(sptr<IVSyncConnection>& vsyncConn,
    const std::string& name, const sptr<VSyncIConnectionToken>& token, VSyncConnParam vsyncConnParam)
{
    if (token == nullptr) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::CreateVSyncConnection: token is nullptr.");
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("CreateVSyncConnection: WriteInterfaceToken RSIClientToServiceConnection::GetDescriptor() err.");
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteString(name)) {
        ROSEN_LOGE("CreateVSyncConnection: WriteString name err.");
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(token->AsObject())) {
        ROSEN_LOGE("CreateVSyncConnection: WriteRemoteObject token->AsObject() err.");
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(vsyncConnParam.id)) {
        ROSEN_LOGE("CreateVSyncConnection: WriteUint64 id err.");
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(vsyncConnParam.windowNodeId)) {
        ROSEN_LOGE("CreateVSyncConnection: WriteUint64 windowNodeId err.");
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION);
    if (!Remote()) {
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> rObj = reply.ReadRemoteObject();
    if (rObj == nullptr) {
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    vsyncConn = iface_cast<IVSyncConnection>(rObj);
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetPixelMapByProcessId(
    std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetPixelMapByProcessId: WriteInterfaceToken RSIClientToServiceConnection::GetDescriptor() err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(pid)) {
        ROSEN_LOGE("GetPixelMapByProcessId: WriteUint64 pid err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetPixelMapByProcessId: Send Request err");
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(repCode)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetPixelMapByProcessId Read repCode failed");
        return ERR_INVALID_VALUE;
    }
    if (repCode == SUCCESS) {
        pixelMapInfoVector.clear();
        if (!RSMarshallingHelper::Unmarshalling(reply, pixelMapInfoVector)) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::GetPixelMapByProcessId: Unmarshalling failed");
        }
    } else {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetPixelMapByProcessId: Invalid reply");
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::CreatePixelMapFromSurface(sptr<Surface> surface,
    const Rect &srcRect, std::shared_ptr<Media::PixelMap> &pixelMap, bool transformEnabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (surface == nullptr) {
        return ERR_INVALID_VALUE;
    }

    auto producer = surface->GetProducer();
    if (producer == nullptr) {
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteInterfaceToken RSIClientToServiceConnection::GetDescriptor() err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(producer->AsObject())) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteRemoteObject producer->AsObject() err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(srcRect.x) || !data.WriteInt32(srcRect.y) ||
        !data.WriteInt32(srcRect.w) || !data.WriteInt32(srcRect.h)) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteInt32 srcRect err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(transformEnabled)) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteBool err.");
        return -1;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::CreatePixelMapFromSurface: Send Request err.");
        return ERR_INVALID_VALUE;
    }

    if (reply.ReadBool()) {
        pixelMap.reset(Media::PixelMap::Unmarshalling(reply));
    } else {
        ROSEN_LOGE("CreatePixelMapFromSurface: ReadBool err.");
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetDefaultScreenId(uint64_t& screenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetDefaultScreenId: WriteInterfaceToken RSIClientToServiceConnection::GetDescriptor() err.");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: sendrequest error : %{public}d", __func__, err);
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadUint64(screenId)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetDefaultScreenId Read id failed");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetActiveScreenId(uint64_t& screenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetActiveScreenId: WriteInterfaceToken RSIClientToServiceConnection::GetDescriptor() err.");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: sendrequest error : %{public}d", __func__, err);
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadUint64(screenId)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetActiveScreenId Read id failed");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

std::vector<ScreenId> RSClientToServiceConnectionProxy::GetAllScreenIds()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<ScreenId> screenIds;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetAllScreenIds: WriteInterfaceToken RSIClientToServiceConnection::GetDescriptor() err.");
        return std::vector<ScreenId>();
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return std::vector<ScreenId>();
    }

    uint32_t size{0};
    if (!reply.ReadUint32(size)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetAllScreenIds Read size failed");
        return std::vector<ScreenId>();
    }
    size_t readableSize = reply.GetReadableBytes() / sizeof(ScreenId);
    size_t len = static_cast<size_t>(size);
    if (len > readableSize || len > screenIds.max_size()) {
        RS_LOGE(
            "RSClientToServiceConnectionProxy::GetDescriptor() GetAllScreenIds Failed read vector, size:%{public}zu,"
            " readableSize:%{public}zu", len, readableSize);
        return screenIds;
    }
    for (uint32_t i = 0; i < size; i++) {
        screenIds.emplace_back(reply.ReadUint64());
    }

    return screenIds;
}

ScreenId RSClientToServiceConnectionProxy::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId associatedScreenId,
    int32_t flags,
    std::vector<NodeId> whiteList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("CreateVirtualScreen: WriteInterfaceToken RSIClientToServiceConnection::GetDescriptor() err.");
        return INVALID_SCREEN_ID;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteString(name)) {
        ROSEN_LOGE("CreateVirtualScreen: WriteString name err.");
        return INVALID_SCREEN_ID;
    }
    if (!data.WriteUint32(width)) {
        ROSEN_LOGE("CreateVirtualScreen: WriteUint32 width err.");
        return INVALID_SCREEN_ID;
    }
    if (!data.WriteUint32(height)) {
        ROSEN_LOGE("CreateVirtualScreen: WriteUint32 height err.");
        return INVALID_SCREEN_ID;
    }
    if (surface != nullptr) {
        auto producer = surface->GetProducer();
        if (producer != nullptr) {
            if (!data.WriteBool(true)) {
                ROSEN_LOGE("CreateVirtualScreen: WriteBool [true] err.");
                return INVALID_SCREEN_ID;
            }
            if (!data.WriteRemoteObject(producer->AsObject())) {
                ROSEN_LOGE("CreateVirtualScreen: WriteRemoteObject producer->AsObject() err.");
                return INVALID_SCREEN_ID;
            }
        } else {
            if (!data.WriteBool(false)) {
                ROSEN_LOGE("CreateVirtualScreen: WriteBool [false] err.");
                return INVALID_SCREEN_ID;
            }
        }
    } else {
        if (!data.WriteBool(false)) {
            ROSEN_LOGE("CreateVirtualScreen: WriteBool [false] err.");
            return INVALID_SCREEN_ID;
        }
    }
    if (!data.WriteUint64(associatedScreenId)) {
        ROSEN_LOGE("CreateVirtualScreen: WriteUint64 associatedScreenId err.");
        return INVALID_SCREEN_ID;
    }
    if (!data.WriteInt32(flags)) {
        ROSEN_LOGE("CreateVirtualScreen: WriteInt32 flags err.");
        return INVALID_SCREEN_ID;
    }
    if (!data.WriteUInt64Vector(whiteList)) {
        ROSEN_LOGE("CreateVirtualScreen: WriteUInt64Vector whiteList err.");
        return INVALID_SCREEN_ID;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::%{public}s: Send Request err.", __func__);
        return INVALID_SCREEN_ID;
    }

    uint64_t id{0};
    if (!reply.ReadUint64(id)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::CreateVirtualScreen Read id failed");
        return INVALID_SCREEN_ID;
    }
    return id;
}

int32_t RSClientToServiceConnectionProxy::SetVirtualScreenBlackList(ScreenId id, const std::vector<NodeId>& blackList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 id err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUInt64Vector(blackList)) {
        ROSEN_LOGE("%{public}s: WriteUInt64Vector blackList err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        return RS_CONNECTION_ERROR;
    }

    int32_t status = reply.ReadInt32();
    return status;
}

ErrCode RSClientToServiceConnectionProxy::SetVirtualScreenTypeBlackList(
    ScreenId id, std::vector<NodeType>& typeBlackListVector, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenTypeBlackList: WriteInterfaceToken GetDescriptor err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenTypeBlackList: WriteUint64 id err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUInt8Vector(typeBlackListVector)) {
        ROSEN_LOGE("SetVirtualScreenTypeBlackList: WriteUInt8Vector typeBlackListVector err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenTypeBlackList: Send Request err.");
        return ERR_INVALID_VALUE;
    }

    repCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::AddVirtualScreenBlackList(
    ScreenId id, const std::vector<NodeId>& blackList, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 id err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUInt64Vector(blackList)) {
        ROSEN_LOGE("%{public}s: WriteUInt64Vector blackList err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }

    repCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::RemoveVirtualScreenBlackList(
    ScreenId id, const std::vector<NodeId>& blackList, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 id err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUInt64Vector(blackList)) {
        ROSEN_LOGE("%{public}s: WriteUInt64Vector blackList err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }

    repCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::AddVirtualScreenWhiteList(
    ScreenId id, const std::vector<NodeId>& whiteList, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 id err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUInt64Vector(whiteList)) {
        ROSEN_LOGE("%{public}s: WriteUInt64Vector whiteList err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_WHITELIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }

    repCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::RemoveVirtualScreenWhiteList(
    ScreenId id, const std::vector<NodeId>& whiteList, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 id err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUInt64Vector(whiteList)) {
        ROSEN_LOGE("%{public}s: WriteUInt64Vector whiteList err.", __func__);
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_WHITELIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }

    repCode = reply.ReadInt32();
    return ERR_OK;
}

int32_t RSClientToServiceConnectionProxy::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<NodeId>& securityExemptionList)
{
    if (securityExemptionList.size() > MAX_SECURITY_EXEMPTION_LIST_NUMBER) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenSecurityExemptionList: too many lists.");
        return INVALID_ARGUMENTS;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenSecurityExemptionList: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenSecurityExemptionList: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUInt64Vector(securityExemptionList)) {
        ROSEN_LOGE("SetVirtualScreenSecurityExemptionList: WriteUInt64Vector securityExemptionList err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenSecurityExemptionList: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenSecurityExemptionList Read status failed");
        return READ_PARCEL_ERR;
    }
    return status;
}

int32_t RSClientToServiceConnectionProxy::SetScreenSecurityMask(ScreenId id,
    std::shared_ptr<Media::PixelMap> securityMask)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenSecurityMask: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenSecurityMask: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }

    if (securityMask) {
        if (!data.WriteBool(true) || !data.WriteParcelable(securityMask.get())) {
            ROSEN_LOGE("SetScreenSecurityMask: WriteBool[true] OR WriteParcelable[securityMask.get()] err.");
            return WRITE_PARCEL_ERR;
        }
    } else {
        if (!data.WriteBool(false)) {
            ROSEN_LOGE("SetScreenSecurityMask: WriteBool [false] err.");
            return WRITE_PARCEL_ERR;
        }
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SECURITY_MASK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetScreenSecurityMask: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    return SUCCESS;
}

int32_t RSClientToServiceConnectionProxy::SetMirrorScreenVisibleRect(
    ScreenId id, const Rect& mainScreenRect, bool supportRotation)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetMirrorScreenVisibleRect: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetMirrorScreenVisibleRect: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt32(mainScreenRect.x) || !data.WriteInt32(mainScreenRect.y) ||
        !data.WriteInt32(mainScreenRect.w) || !data.WriteInt32(mainScreenRect.h)) {
        ROSEN_LOGE("SetMirrorScreenVisibleRect: WriteInt32 mainScreenRect err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteBool(supportRotation)) {
        ROSEN_LOGE("SetMirrorScreenVisibleRect: WriteBool supportRotation err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_MIRROR_SCREEN_VISIBLE_RECT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetMirrorScreenVisibleRect: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetMirrorScreenVisibleRect Read status failed");
        return READ_PARCEL_ERR;
    }
    return status;
}

int32_t RSClientToServiceConnectionProxy::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetCastScreenEnableSkipWindow: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetCastScreenEnableSkipWindow: WriteUint64 MessageOption::TF_ASYNC err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteBool(enable)) {
        ROSEN_LOGE("SetCastScreenEnableSkipWindow: WriteBool enable err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetCastScreenEnableSkipWindow: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSClientToServiceConnectionProxy::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    if (surface == nullptr) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenSurface: Send surface is nullptr!");
        return INVALID_ARGUMENTS;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenSurface: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenSurface: WriteUint64 MessageOption::TF_ASYNC err.");
        return WRITE_PARCEL_ERR;
    }
    auto producer = surface->GetProducer();
    if (!data.WriteRemoteObject(producer->AsObject())) {
        ROSEN_LOGE("SetVirtualScreenSurface: WriteRemoteObject producer->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenSurface: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t status = reply.ReadInt32();
    return status;
}

void RSClientToServiceConnectionProxy::RemoveVirtualScreen(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RemoveVirtualScreen: WriteInterfaceToken GetDescriptor err.");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RemoveVirtualScreen: WriteUint64 id err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RemoveVirtualScreen: Send Request err.");
        return;
    }
}

int32_t RSClientToServiceConnectionProxy::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetScreenChangeCallback: callback is nullptr.");
        return INVALID_ARGUMENTS;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("SetScreenChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetScreenChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSClientToServiceConnectionProxy::SetScreenSwitchingNotifyCallback(
    sptr<RSIScreenSwitchingNotifyCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenSwitchingNotifyCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_SYNC);

    if (callback) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("SetScreenSwitchingNotifyCallback: WriteBool[T] OR WriteRemoteObject[CB] err");
            return WRITE_PARCEL_ERR;
        }
    } else {
        if (!data.WriteBool(false)) {
            ROSEN_LOGE("SetScreenSwitchingNotifyCallback: WriteBool [false] err.");
            return WRITE_PARCEL_ERR;
        }
    }

    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SWITCHING_NOTIFY_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetScreenSwitchingNotifyCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE(
            "RSClientToServiceConnectionProxy::SetScreenSwitchingNotifyCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::SetBrightnessInfoChangeCallback(
    sptr<RSIBrightnessInfoChangeCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetBrightnessInfoChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (callback) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("SetBrightnessInfoChangeCallback: WriteBool[T] OR WriteRemoteObject[CB] err");
            return WRITE_PARCEL_ERR;
        }
    } else {
        if (!data.WriteBool(false)) {
            ROSEN_LOGE("SetBrightnessInfoChangeCallback: WriteBool [false] err.");
            return WRITE_PARCEL_ERR;
        }
    }

    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }

    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetBrightnessInfoChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

uint32_t RSClientToServiceConnectionProxy::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenActiveMode: WriteInterfaceToken GetDescriptor err.");
        return StatusCode::WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenActiveMode: WriteUint64 id err.");
        return StatusCode::WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(modeId)) {
        ROSEN_LOGE("SetScreenActiveMode: WriteUint32 modeId err.");
        return StatusCode::WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return StatusCode::IPC_ERROR;
    }

    uint32_t ret = 0;
    if (!reply.ReadUint32(ret)) {
        ROSEN_LOGE("SetScreenActiveMode: ReadUint32 for result err.");
        return StatusCode::READ_PARCEL_ERR;
    }

    return ret;
}

void RSClientToServiceConnectionProxy::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenRefreshRate: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenRefreshRate: WriteUint64 id err.");
        return;
    }
    if (!data.WriteInt32(sceneId)) {
        ROSEN_LOGE("SetScreenRefreshRate: WriteInt32 sceneId err.");
        return;
    }
    if (!data.WriteInt32(rate)) {
        ROSEN_LOGE("SetScreenRefreshRate: WriteInt32 rate err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}

void RSClientToServiceConnectionProxy::SetRefreshRateMode(int32_t refreshRateMode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetRefreshRateMode: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(refreshRateMode)) {
        ROSEN_LOGE("SetRefreshRateMode: WriteInt32 refreshRateMode err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}

void RSClientToServiceConnectionProxy::SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range,
    int32_t animatorExpectedFrameRate)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SyncFrameRateRange: WriteInterfaceToken GetDescriptor err.");
        return;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SyncFrameRateRange: WriteUint64 id err.");
        return;
    }
    if (!data.WriteUint32(range.min_) || !data.WriteUint32(range.max_) ||
        !data.WriteUint32(range.preferred_) || !data.WriteUint32(range.type_) ||
        !data.WriteUint32(range.componentScene_)) {
        ROSEN_LOGE("SyncFrameRateRange: WriteUint32 range err.");
        return;
    }
    if (!data.WriteInt32(animatorExpectedFrameRate)) {
        ROSEN_LOGE("SyncFrameRateRange: WriteInt32 animatorExpectedFrameRate err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}

void RSClientToServiceConnectionProxy::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("UnregisterFrameRateLinker: WriteInterfaceToken GetDescriptor err.");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("UnregisterFrameRateLinker: WriteUint64 id err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}

uint32_t RSClientToServiceConnectionProxy::GetScreenCurrentRefreshRate(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenCurrentRefreshRate: WriteInterfaceToken GetDescriptor err.");
        return SUCCESS;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenCurrentRefreshRate: WriteUint64 id err.");
        return SUCCESS;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return SUCCESS;
    }
    uint32_t rate{0};
    if (!reply.ReadUint32(rate)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenCurrentRefreshRate Read rate failed");
        return READ_PARCEL_ERR;
    }
    return rate;
}

int32_t RSClientToServiceConnectionProxy::GetCurrentRefreshRateMode()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceProxy failed to get descriptor");
        return SUCCESS;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return SUCCESS;
    }
    int32_t refreshRateMode{0};
    if (!reply.ReadInt32(refreshRateMode)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetCurrentRefreshRateMode Read refreshRateMode failed");
        return READ_PARCEL_ERR;
    }
    return refreshRateMode;
}

std::vector<int32_t> RSClientToServiceConnectionProxy::GetScreenSupportedRefreshRates(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<int32_t> screenSupportedRates;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedRefreshRates: WriteInterfaceToken GetDescriptor err.");
        return screenSupportedRates;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedRefreshRates: WriteUint64 id err.");
        return screenSupportedRates;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return screenSupportedRates;
    }
    uint64_t rateCount{0};
    if (!reply.ReadUint64(rateCount)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenSupportedRefreshRates Read rateCount failed");
        return screenSupportedRates;
    }
    size_t readableSize = reply.GetReadableBytes();
    size_t len = static_cast<size_t>(rateCount);
    if (len > readableSize || len > screenSupportedRates.max_size()) {
        RS_LOGE("RSClientToServiceConnectionProxy::GetDescriptor() GetScreenSupportedRefreshRates "
            "fail read vector, size : %{public}zu, readableSize : %{public}zu", len, readableSize);
        return screenSupportedRates;
    }
    screenSupportedRates.resize(rateCount);
    for (uint64_t rateIndex = 0; rateIndex < rateCount; rateIndex++) {
        screenSupportedRates[rateIndex] = reply.ReadInt32();
    }
    return screenSupportedRates;
}

ErrCode RSClientToServiceConnectionProxy::GetShowRefreshRateEnabled(bool& enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceProxy failed to get descriptor");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadBool(enable)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetCurrentRefreshRateMode Read enable failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetShowRefreshRateEnabled: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteBool(enabled) || !data.WriteInt32(type)) {
        ROSEN_LOGE("SetShowRefreshRateEnabled: WriteBool[enable] OR WriteInt32[type] err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}

uint32_t RSClientToServiceConnectionProxy::GetRealtimeRefreshRate(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetRealtimeRefreshRate: WriteInterfaceToken GetDescriptor err.");
        return SUCCESS;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetRealtimeRefreshRate: WriteUint64 id err.");
        return SUCCESS;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return SUCCESS;
    }
    uint32_t rate{0};
    if (!reply.ReadUint32(rate)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetRealtimeRefreshRate Read rate failed");
        return READ_PARCEL_ERR;
    }
    return rate;
}

ErrCode RSClientToServiceConnectionProxy::GetRefreshInfo(pid_t pid, std::string& enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetRefreshInfo: WriteInterfaceToken GetDescriptor err.");
        enable = "";
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("GetRefreshInfo: WriteInt32 pid err.");
        enable = "";
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        enable = "";
        return ERR_INVALID_VALUE;
    }

    if (!reply.ReadString(enable)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetRefreshInfo Read enable failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetRefreshInfoToSP(NodeId id, std::string& enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetRefreshInfoToSP: WriteInterfaceToken GetDescriptor err.");
        enable = "";
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetRefreshInfoToSP: WriteUint64 id err.");
        enable = "";
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO_TO_SP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        enable = "";
        return ERR_INVALID_VALUE;
    }

    if (!reply.ReadString(enable)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetRefreshInfo Read enable failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetRefreshInfoByPidAndUniqueId(
    pid_t pid, uint64_t uniqueId, std::string& enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetRefreshInfoByPidAndUniqueId: WriteInterfaceToken GetDescriptor err.");
        enable = "";
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("GetRefreshInfoByPidAndUniqueId: WriteInt32 pid err.");
        enable = "";
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(uniqueId)) {
        ROSEN_LOGE("GetRefreshInfoByPidAndUniqueId: WriteUint64 uniqueId err.");
        enable = "";
        return ERR_INVALID_VALUE;
    }
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy sendrequest error : %{public}d", err);
        enable = "";
        return ERR_INVALID_VALUE;
    }

    if (!reply.ReadString(enable)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetRefreshInfoByPidAndUniqueId Read enable failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSClientToServiceConnectionProxy::SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetRogScreenResolution: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetRogScreenResolution: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(width)) {
        ROSEN_LOGE("SetRogScreenResolution: WriteUint32 width err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(height)) {
        ROSEN_LOGE("SetRogScreenResolution: WriteUint32 height err.");
        return WRITE_PARCEL_ERR;
    }
    auto code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_ROG_SCREEN_RESOLUTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetRogScreenResolution: SendRequest error: %{public}d.", err);
        return RS_CONNECTION_ERROR;
    }
    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetRogScreenResolution Read status failed");
        return READ_PARCEL_ERR;
    }
    return status;
}

int32_t RSClientToServiceConnectionProxy::GetRogScreenResolution(ScreenId id, uint32_t& width, uint32_t& height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetRogScreenResolution: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetRogScreenResolution: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    auto code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_ROG_SCREEN_RESOLUTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetRogScreenResolution: SendRequest error: %{public}d.", err);
        return RS_CONNECTION_ERROR;
    }
    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetRogScreenResolution Read status failed");
        return READ_PARCEL_ERR;
    }
    if (!reply.ReadUint32(width)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetRogScreenResolution Read width failed");
        return READ_PARCEL_ERR;
    }
    if (!reply.ReadUint32(height)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetRogScreenResolution Read height failed");
        return READ_PARCEL_ERR;
    }
    return status;
}

int32_t RSClientToServiceConnectionProxy::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetPhysicalScreenResolution: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetPhysicalScreenResolution: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(width)) {
        ROSEN_LOGE("SetPhysicalScreenResolution: WriteUint32 width err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(height)) {
        ROSEN_LOGE("SetPhysicalScreenResolution: WriteUint32 height err.");
        return WRITE_PARCEL_ERR;
    }
    auto code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_PHYSICAL_SCREEN_RESOLUTION);
    if (SendRequest(code, data, reply, option) != ERR_NONE) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetPhysicalScreenResolution: SendRequest error.");
        return RS_CONNECTION_ERROR;
    }
    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetPhysicalScreenResolution Read status failed");
        return READ_PARCEL_ERR;
    }
    return status;
}

int32_t RSClientToServiceConnectionProxy::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenResolution: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenResolution: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(width)) {
        ROSEN_LOGE("SetVirtualScreenResolution: WriteUint32 width err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(height)) {
        ROSEN_LOGE("SetVirtualScreenResolution: WriteUint32 height err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenResolution: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenResolution Read status failed");
        return READ_PARCEL_ERR;
    }
    return status;
}

ErrCode RSClientToServiceConnectionProxy::MarkPowerOffNeedProcessOneFrame()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::MarkPowerOffNeedProcessOneFrame: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME);
    int32_t err = SendRequest(code, data, reply, option);
    return err != NO_ERROR ? ERR_INVALID_VALUE : ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::RepaintEverything()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RepaintEverything: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPAINT_EVERYTHING);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::DisablePowerOffRenderControl(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("DisablePowerOffRenderControl: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("DisablePowerOffRenderControl: WriteUint64 id err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

void RSClientToServiceConnectionProxy::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenPowerStatus: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenPowerStatus: WriteUint64 id err.");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(status))) {
        ROSEN_LOGE("SetScreenPowerStatus: WriteUint32 status err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SetScreenPowerStatus: SendRequest failed %{public}d", err);
        return;
    }
}

bool RSClientToServiceConnectionProxy::WriteSurfaceCaptureConfig(
    const RSSurfaceCaptureConfig& captureConfig, MessageParcel& data)
{
    if (!data.WriteFloat(captureConfig.scaleX) || !data.WriteFloat(captureConfig.scaleY) ||
        !data.WriteBool(captureConfig.useDma) || !data.WriteBool(captureConfig.useCurWindow) ||
        !data.WriteUint8(static_cast<uint8_t>(captureConfig.captureType)) || !data.WriteBool(captureConfig.isSync) ||
        !data.WriteBool(captureConfig.isHdrCapture) ||
        !data.WriteBool(captureConfig.needF16WindowCaptureForScRGB) ||
        !data.WriteBool(captureConfig.needErrorCode) ||
        !data.WriteFloat(captureConfig.mainScreenRect.left_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.top_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.right_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.bottom_) ||
        !data.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId) ||
        !data.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.left_) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.top_) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.right_) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.bottom_) ||
        !data.WriteUInt64Vector(captureConfig.blackList) ||
        !data.WriteUint32(captureConfig.backGroundColor) ||
        !data.WriteUint32(captureConfig.colorSpace.first) ||
        !data.WriteBool(captureConfig.colorSpace.second) ||
        !data.WriteUint32(captureConfig.dynamicRangeMode.first) ||
        !data.WriteBool(captureConfig.dynamicRangeMode.second)) {
        ROSEN_LOGE("WriteSurfaceCaptureConfig: WriteSurfaceCaptureConfig captureConfig err.");
        return false;
    }
    return true;
}

bool RSClientToServiceConnectionProxy::WriteSurfaceCaptureBlurParam(
    const RSSurfaceCaptureBlurParam& blurParam, MessageParcel& data)
{
    if (!data.WriteBool(blurParam.isNeedBlur) || !data.WriteFloat(blurParam.blurRadius)) {
        ROSEN_LOGE("WriteSurfaceCaptureBlurParam: WriteBool OR WriteFloat [blurParam] err.");
        return false;
    }
    return true;
}

bool RSClientToServiceConnectionProxy::WriteSurfaceCaptureAreaRect(
    const Drawing::Rect& specifiedAreaRect, MessageParcel& data)
{
    if (!data.WriteFloat(specifiedAreaRect.left_) || !data.WriteFloat(specifiedAreaRect.top_) ||
        !data.WriteFloat(specifiedAreaRect.right_) || !data.WriteFloat(specifiedAreaRect.bottom_)) {
        ROSEN_LOGE("WriteSurfaceCaptureAreaRect: WriteFloat specifiedAreaRect err.");
        return false;
    }
    return true;
}

RSVirtualScreenResolution RSClientToServiceConnectionProxy::GetVirtualScreenResolution(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSVirtualScreenResolution virtualScreenResolution;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetVirtualScreenResolution: WriteInterfaceToken GetDescriptor err.");
        return virtualScreenResolution;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetVirtualScreenResolution: WriteUint64 id err.");
        return virtualScreenResolution;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return virtualScreenResolution;
    }

    sptr<RSVirtualScreenResolution> pVirtualScreenResolution(reply.ReadParcelable<RSVirtualScreenResolution>());
    if (pVirtualScreenResolution == nullptr) {
        return virtualScreenResolution;
    }
    virtualScreenResolution = *pVirtualScreenResolution;
    return virtualScreenResolution;
}

ErrCode RSClientToServiceConnectionProxy::GetScreenActiveMode(uint64_t id, RSScreenModeInfo& screenModeInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenActiveMode: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenActiveMode: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: sendrequest error : %{public}d", __func__, err);
        return ERR_INVALID_VALUE;
    }

    sptr<RSScreenModeInfo> pScreenModeInfo(reply.ReadParcelable<RSScreenModeInfo>());
    if (pScreenModeInfo == nullptr) {
        ROSEN_LOGE("%{public}s: ScreenModeInfo is null", __func__);
        return ERR_INVALID_VALUE;
    }
    screenModeInfo = *pScreenModeInfo;
    return ERR_OK;
}

std::vector<RSScreenModeInfo> RSClientToServiceConnectionProxy::GetScreenSupportedModes(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<RSScreenModeInfo> screenSupportedModes;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedModes: WriteInterfaceToken GetDescriptor err.");
        return screenSupportedModes;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedModes: WriteUint64 id err.");
        return screenSupportedModes;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return screenSupportedModes;
    }

    uint64_t modeCount{0};
    if (!reply.ReadUint64(modeCount)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenSupportedModes Read modeCount failed");
        return screenSupportedModes;
    }
    size_t readableSize = reply.GetReadableBytes();
    size_t len = static_cast<size_t>(modeCount);
    if (len > readableSize || len > screenSupportedModes.max_size()) {
        RS_LOGE("RSClientToServiceConnectionProxy::GetDescriptor() GetScreenSupportedModes Fail read vector, "
                "size:%{public}zu, readableSize:%{public}zu",
            len, readableSize);
        return screenSupportedModes;
    }
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

ErrCode RSClientToServiceConnectionProxy::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetMemoryGraphics: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }

    uint64_t count{0};
    if (!reply.ReadUint64(count)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetMemoryGraphics Read count failed");
        return ERR_INVALID_VALUE;
    }
    size_t readableSize = reply.GetReadableBytes();
    size_t len = static_cast<size_t>(count);
    if (len > readableSize || len > memoryGraphics.max_size()) {
        RS_LOGE("RSClientToServiceConnectionProxy::GetDescriptor() GetMemoryGraphics Failed to read vector, "
                "size:%{public}zu, readableSize:%{public}zu",
            len, readableSize);
        return ERR_INVALID_VALUE;
    }
    memoryGraphics.resize(count);
    for (uint64_t index = 0; index < count; index++) {
        sptr<MemoryGraphic> item = reply.ReadParcelable<MemoryGraphic>();
        if (item == nullptr) {
            continue;
        } else {
            memoryGraphics[index] = *item;
        }
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    MemoryGraphic memoryGraphic;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetTotalAppMemSize: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }

    if (!reply.ReadFloat(cpuMemSize) || !reply.ReadFloat(gpuMemSize)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetTotalAppMemSize Read MemSize failed");
        return READ_PARCEL_ERR;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetMemoryGraphic: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("GetMemoryGraphic: WriteInt32 pid err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    sptr<MemoryGraphic> pMemoryGraphic(reply.ReadParcelable<MemoryGraphic>());
    if (pMemoryGraphic == nullptr) {
        return ERR_INVALID_VALUE;
    }
    memoryGraphic = *pMemoryGraphic;
    return ERR_OK;
}

RSScreenCapability RSClientToServiceConnectionProxy::GetScreenCapability(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSScreenCapability screenCapability;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenCapability: WriteInterfaceToken GetDescriptor err.");
        return screenCapability;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenCapability: WriteUint64 id err.");
        return screenCapability;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY);
    int32_t err = SendRequest(code, data, reply, option);
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

ErrCode RSClientToServiceConnectionProxy::GetScreenPowerStatus(uint64_t screenId, uint32_t& status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenPowerStatus: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(screenId)) {
        ROSEN_LOGE("GetScreenPowerStatus: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    status = reply.ReadUint32();
    return ERR_OK;
}

RSScreenData RSClientToServiceConnectionProxy::GetScreenData(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSScreenData screenData;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenData: WriteInterfaceToken GetDescriptor err.");
        return screenData;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenData: WriteUint64 id err.");
        return screenData;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_DATA);
    int32_t err = SendRequest(code, data, reply, option);
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

ErrCode RSClientToServiceConnectionProxy::GetScreenBacklight(uint64_t id, int32_t& level)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenBacklight: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenBacklight: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(level)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenBacklight Read level failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::SetScreenBacklight(ScreenId id, uint32_t level)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenBacklight: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenBacklight: WriteUint64 id err.");
        return;
    }
    if (!data.WriteUint32(level)) {
        ROSEN_LOGE("SetScreenBacklight: WriteUint32 level err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SetScreenBacklight: SendRequest failed");
        return;
    }
}

ErrCode RSClientToServiceConnectionProxy::GetPanelPowerStatus(uint64_t screenId, PanelPowerStatus& status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ROSEN_LOGE("GetPanelPowerStatus: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(screenId)) {
        ROSEN_LOGE("GetPanelPowerStatus: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PANEL_POWER_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: sendrequest error: %{public}d", __func__, err);
        return ERR_INVALID_OPERATION;
    }
    uint32_t readStatus = 0;
    if (!reply.ReadUint32(readStatus)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetPanelPowerStatus Read status failed");
        return ERR_INVALID_VALUE;
    }
    status = static_cast<PanelPowerStatus>(readStatus);
    return ERR_OK;
}

int32_t RSClientToServiceConnectionProxy::SetDualScreenState(ScreenId id, DualScreenStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetDualScreenState: WriteInterfaceToken GetDescriptor err.");
        return StatusCode::WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetDualScreenState: WriteUint64 id err.");
        return StatusCode::WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(status))) {
        ROSEN_LOGE("SetDualScreenState: WriteUint64 status err.");
        return StatusCode::WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_DUAL_SCREEN_STATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SetDualScreenState: SendRequest error: %{public}d", err);
        return StatusCode::RS_CONNECTION_ERROR;
    }
    int32_t ret{0};
    if (!reply.ReadInt32(ret)) {
        ROSEN_LOGE("SetDualScreenState: Read ret failed");
        return StatusCode::READ_PARCEL_ERR;
    }
    return ret;
}

int32_t RSClientToServiceConnectionProxy::GetScreenSupportedColorGamuts(
    ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedColorGamuts: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedColorGamuts: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenSupportedColorGamuts Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result == SUCCESS) {
        mode.clear();
        std::vector<uint32_t> modeRecv;
        reply.ReadUInt32Vector(&modeRecv);
        for (auto i : modeRecv) {
            mode.push_back(static_cast<ScreenColorGamut>(i));
        }
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::GetScreenSupportedMetaDataKeys(
    ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedMetaDataKeys: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedMetaDataKeys: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenSupportedMetaDataKeys Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result == SUCCESS) {
        keys.clear();
        std::vector<uint32_t> keyRecv;
        reply.ReadUInt32Vector(&keyRecv);
        for (auto i : keyRecv) {
            keys.push_back(static_cast<ScreenHDRMetadataKey>(i));
        }
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenColorGamut: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenColorGamut: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_GAMUT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenColorGamut Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result == SUCCESS) {
        uint32_t readMode{0};
        if (!reply.ReadUint32(readMode)) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenColorGamut Read mode failed");
            return READ_PARCEL_ERR;
        }
        mode = static_cast<ScreenColorGamut>(readMode);
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenColorGamut: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenColorGamut: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt32(modeIdx)) {
        ROSEN_LOGE("SetScreenColorGamut: WriteInt32 modeIdx err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetScreenColorGamut Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenGamutMap: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenGamutMap: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(mode)) {
        ROSEN_LOGE("SetScreenGamutMap: WriteUint32 mode err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetScreenGamutMap Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenCorrection: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenCorrection: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(screenRotation))) {
        ROSEN_LOGE("SetScreenCorrection: WriteUint32 screenRotation err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetScreenCorrection Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenGamutMap: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenGamutMap: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenGamutMap Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result == SUCCESS) {
        uint32_t readMode{0};
        if (!reply.ReadUint32(readMode)) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenGamutMap Read mode failed");
            return READ_PARCEL_ERR;
        }
        mode = static_cast<ScreenGamutMap>(readMode);
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::GetScreenHDRCapability(
    ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenHDRCapability: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenHDRCapability: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenHDRCapability Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result != SUCCESS) {
        return result;
    }
    sptr<RSScreenHDRCapability> pScreenCapability = reply.ReadParcelable<RSScreenHDRCapability>();
    if (pScreenCapability == nullptr) {
        return RS_CONNECTION_ERROR;
    }
    screenHdrCapability = *pScreenCapability;
    return SUCCESS;
}

ErrCode RSClientToServiceConnectionProxy::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetPixelFormat: WriteInterfaceToken GetDescriptor err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetPixelFormat: WriteUint64 id err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PIXEL_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetPixelFormat Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        uint32_t readFormat{0};
        if (!reply.ReadUint32(readFormat)) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::GetPixelFormat Read readFormat failed");
            return READ_PARCEL_ERR;
        }
        pixelFormat = static_cast<GraphicPixelFormat>(readFormat);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetPixelFormat: WriteInterfaceToken GetDescriptor err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetPixelFormat: WriteUint64 id err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(pixelFormat))) {
        ROSEN_LOGE("GetPixelFormat: WriteUint32 pixelFormat err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_PIXEL_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    resCode = reply.ReadInt32();
    return ERR_OK;
}

int32_t RSClientToServiceConnectionProxy::GetScreenSupportedHDRFormats(
    ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats, sptr<RSIScreenSupportedHdrFormatsCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedHDRFormats: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedHDRFormats: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (callback) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("GetScreenSupportedHDRFormats WriteRemoteObject obj failed");
            return WRITE_PARCEL_ERR;
        }
    } else {
        if (!data.WriteBool(false)) {
            ROSEN_LOGE("GetScreenSupportedHDRFormats WriteBool false failed");
            return WRITE_PARCEL_ERR;
        }
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenSupportedHDRFormats Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result == SUCCESS) {
        hdrFormats.clear();
        std::vector<uint32_t> hdrFormatsRecv;
        reply.ReadUInt32Vector(&hdrFormatsRecv);
        std::transform(hdrFormatsRecv.begin(), hdrFormatsRecv.end(), back_inserter(hdrFormats),
                       [](uint32_t i) -> ScreenHDRFormat {return static_cast<ScreenHDRFormat>(i);});
    }
    return result;
}

ErrCode RSClientToServiceConnectionProxy::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenHDRFormat: WriteInterfaceToken GetDescriptor err.");
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenHDRFormat: WriteUint64 id err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenHDRFormat Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        uint32_t readFormat{0};
        if (!reply.ReadUint32(readFormat)) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenHDRFormat1 Read readFormat failed");
            return READ_PARCEL_ERR;
        }
        hdrFormat = static_cast<ScreenHDRFormat>(readFormat);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::SetScreenHDRFormat(ScreenId id, int32_t modeIdx, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenHDRFormat: WriteInterfaceToken GetDescriptor err.");
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenHDRFormat: WriteUint64 id err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(modeIdx)) {
        ROSEN_LOGE("SetScreenHDRFormat: WriteInt32 modeIdx err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    resCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedColorSpaces: WriteInterfaceToken GetDescriptor err.");
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedColorSpaces: WriteUint64 id err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenSupportedColorSpaces Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        colorSpaces.clear();
        std::vector<uint32_t> colorSpacesRecv;
        reply.ReadUInt32Vector(&colorSpacesRecv);
        std::transform(colorSpacesRecv.begin(), colorSpacesRecv.end(), back_inserter(colorSpaces),
                       [](uint32_t i) -> GraphicCM_ColorSpaceType {return static_cast<GraphicCM_ColorSpaceType>(i);});
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetScreenColorSpace(
    ScreenId id, GraphicCM_ColorSpaceType& colorSpace, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenColorSpace: WriteInterfaceToken GetDescriptor err.");
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenColorSpace: WriteUint64 id err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenColorSpace Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        uint32_t type{0};
        if (!reply.ReadUint32(type)) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenColorSpace Read type failed");
            return READ_PARCEL_ERR;
        }
        colorSpace = static_cast<GraphicCM_ColorSpaceType>(type);
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::SetScreenColorSpace(
    ScreenId id, GraphicCM_ColorSpaceType colorSpace, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenColorSpace: WriteInterfaceToken GetDescriptor err.");
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenColorSpace: WriteUint64 id err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(colorSpace)) {
        ROSEN_LOGE("SetScreenColorSpace: WriteInt32 colorSpace err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    resCode = reply.ReadInt32();
    return ERR_OK;
}

int32_t RSClientToServiceConnectionProxy::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenType: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenType: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_TYPE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        uint32_t type{0};
        if (!reply.ReadUint32(type)) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::GetScreenType Read type failed");
            return READ_PARCEL_ERR;
        }
        screenType = static_cast<RSScreenType>(type);
    }
    return result;
}

bool RSClientToServiceConnectionProxy::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualMirrorScreenCanvasRotation: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualMirrorScreenCanvasRotation: WriteUint64 id err.");
        return false;
    }
    if (!data.WriteBool(canvasRotation)) {
        ROSEN_LOGE("SetVirtualMirrorScreenCanvasRotation: WriteBool canvasRotation err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualMirrorScreenCanvasRotation Read result failed");
        return false;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenAutoRotation: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenAutoRotation: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteBool(isAutoRotation)) {
        ROSEN_LOGE("SetVirtualScreenAutoRotation: WriteBool isAutoRotation err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_AUTO_ROTATION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::%{public}s: Send Request err.", __func__);
        return RS_CONNECTION_ERROR;
    }
    int32_t result{-1};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::%{public}s Read result failed", __func__);
        return READ_PARCEL_ERR;
    }
    return result;
}

bool RSClientToServiceConnectionProxy::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualMirrorScreenScaleMode: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualMirrorScreenScaleMode: WriteUint64 id err.");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(scaleMode))) {
        ROSEN_LOGE("SetVirtualMirrorScreenScaleMode: WriteUint32 scaleMode err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualMirrorScreenScaleMode Read result failed");
        return false;
    }
    return result;
}

void WaitNeedRegisterTypefaceReply(uint8_t rspRpc, int& retryCount)
{
    RS_LOGD("Check need register state:%{public}hhu", rspRpc);
    if (retryCount >= MAX_RETRY_COUNT) {
        RS_LOGW("Other process is registering too long, need reload full typeface.");
        return;
    }
    if (rspRpc == Drawing::REGISTERING) {
        usleep(RETRY_WAIT_TIME_US * 30); // wait 30 ms
    }
    retryCount++;
}

bool FullTypefaceHead(MessageParcel& data, uint64_t globalUniqueId, uint32_t hash)
{
    if (!data.WriteUint64(globalUniqueId)) {
        ROSEN_LOGE("RegisterTypeface: WriteUint64 globalUniqueId %{public}" PRIu64 " err.", globalUniqueId);
        return false;
    }
    if (!data.WriteUint32(hash)) {
        ROSEN_LOGE("RegisterTypeface: WriteUint32 hash %{public}u err.", hash);
        return false;
    }
    return true;
}

bool RSClientToServiceConnectionProxy::RegisterTypeface(uint64_t globalUniqueId,
    std::shared_ptr<Drawing::Typeface>& typeface)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterTypeface: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t hash = typeface->GetHash();
    if (!FullTypefaceHead(data, globalUniqueId, hash)) {
        return false;
    }

    if (hash) { // if adapter does not provide hash, use old path
        uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NEED_REGISTER_TYPEFACE);
        int retryCount = 0;
        uint8_t rspRpc = Drawing::REGISTERED;
        do {
            MessageParcel replyNeedRegister;
            int32_t err = SendRequest(code, data, replyNeedRegister, option);
            if (err != NO_ERROR || !replyNeedRegister.ReadUint8(rspRpc)) {
                RS_LOGW("Check if RegisterTypeface is needed failed, err:%{public}d", err);
                return false;
            }
            WaitNeedRegisterTypefaceReply(rspRpc, retryCount);
            if (rspRpc == Drawing::REGISTERED) {
                return true;
            }
        } while (rspRpc == Drawing::REGISTERING && retryCount <= MAX_RETRY_COUNT);
    }

    RSMarshallingHelper::Marshalling(data, typeface);

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_TYPEFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGD("RSClientToServiceConnectionProxy::RegisterTypeface: RegisterTypeface failed");
        return false;
    }
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterTypeface Read result failed");
        return false;
    }
    return result;
}

void RSClientToServiceConnectionProxy::ForceRefreshOneFrameWithNextVSync()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ForceRefreshOneFrameWithNextVSync: Send Request err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

int32_t RSClientToServiceConnectionProxy::RegisterTypeface(Drawing::SharedTypeface& sharedTypeface, int32_t& needUpdate)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        RS_LOGE("RegisterTypeface: WriteInterfaceToken GetDescriptor err.");
        return -1;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    RSMarshallingHelper::Marshalling(data, sharedTypeface);

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_SHARED_TYPEFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("RSClientToServiceConnectionProxy::RegisterTypeface: send request failed");
        return -1;
    }

    if (reply.ReadInt32(needUpdate) && needUpdate == 0) {
        return sharedTypeface.fd_;
    } else if (needUpdate == -1) {
        RS_LOGE("RegisterTypeface: Failed to register typeface in service.");
        return -1;
    }
    int32_t result = reply.ReadFileDescriptor();
    return result;
}

bool RSClientToServiceConnectionProxy::UnRegisterTypeface(uint64_t globalUniqueId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("UnRegisterTypeface: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(globalUniqueId)) {
        ROSEN_LOGE("UnRegisterTypeface: WriteUint64 globalUniqueId err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGD("RSClientToServiceConnectionProxy::UnRegisterTypeface: send request failed");
        return false;
    }

    return true;
}

int32_t RSClientToServiceConnectionProxy::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
    std::vector<uint8_t>& edidData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteUint64(id)) {
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_DISPLAY_IDENTIFICATION_DATA);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetDisplayIdentificationData Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result != SUCCESS) {
        RS_LOGE("RSClientToServiceConnectionProxy::GetDisplayIdentificationData: failed");
        return result;
    }
    if (!reply.ReadUint8(outPort)) {
        return READ_PARCEL_ERR;
    }
    uint32_t edidSize = reply.ReadUint32();
    if (edidSize == 0 || edidSize > EDID_DATA_MAX_SIZE) {
        RS_LOGE("RSClientToServiceConnectionProxy::GetDisplayIdentificationData: EdidSize failed");
        return READ_PARCEL_ERR;
    }
    edidData.resize(edidSize);
    const uint8_t *editpnt = reply.ReadBuffer(edidSize);
    if (editpnt == nullptr) {
        RS_LOGE("RSClientToServiceConnectionProxy::GetDisplayIdentificationData: ReadBuffer failed");
        return READ_PARCEL_ERR;
    }
    RS_LOGD("RSClientToServiceConnectionProxy::GetDisplayIdentificationData: EdidSize: %{public}u", edidSize);
    edidData.assign(editpnt, editpnt + edidSize);

    return result;
}

ErrCode RSClientToServiceConnectionProxy::SetScreenSkipFrameInterval(uint64_t id, uint32_t skipFrameInterval,
    int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenSkipFrameInterval: WriteInterfaceToken GetDescriptor err.");
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenSkipFrameInterval: WriteUint64 id err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(skipFrameInterval)) {
        ROSEN_LOGE("SetScreenSkipFrameInterval: WriteUint32 skipFrameInterval err.");
        resCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetPixelFormat Read result failed");
        resCode = READ_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::SetVirtualScreenRefreshRate(
    ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate, int32_t& retVal)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenRefreshRate: WriteInterfaceToken GetDescriptor err.");
        retVal = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenRefreshRate: WriteUint64 id err.");
        retVal = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(maxRefreshRate)) {
        ROSEN_LOGE("SetVirtualScreenRefreshRate: WriteUint32 maxRefreshRate err.");
        retVal = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        retVal = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenRefreshRate Read result failed");
        retVal = READ_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (result == SUCCESS) {
        if (!reply.ReadUint32(actualRefreshRate)) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenRefreshRate Read actualRefreshRate failed");
            retVal = READ_PARCEL_ERR;
            return ERR_INVALID_VALUE;
        }
    }
    retVal = result;
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::SetScreenActiveRect(ScreenId id, const Rect& activeRect, uint32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenActiveRect: WriteInterfaceToken GetDescriptor err.");
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenActiveRect: WriteUint64 id err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(activeRect.x) || !data.WriteInt32(activeRect.y) ||
        !data.WriteInt32(activeRect.w) || !data.WriteInt32(activeRect.h)) {
        ROSEN_LOGE("SetScreenActiveRect: WriteInt32 activeRect err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadUint32(repCode)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetScreenActiveRect Read result failed");
        repCode = READ_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::SetScreenOffset(ScreenId id, int32_t offSetX, int32_t offSetY)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id) || !data.WriteInt32(offSetX) || !data.WriteInt32(offSetY)) {
        ROSEN_LOGE("%{public}s: write error.", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_OFFSET);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
    }
}

void RSClientToServiceConnectionProxy::SetScreenFrameGravity(ScreenId id, int32_t gravity)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);

    if (!data.WriteUint64(id) || !data.WriteInt32(gravity)) {
        ROSEN_LOGE("%{public}s: write error.", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_FRAME_GRAVITY);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
    }
}

int32_t RSClientToServiceConnectionProxy::RegisterHgmConfigChangeCallback(sptr<RSIHgmConfigChangeCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterHgmConfigChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterHgmConfigChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterHgmConfigChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterHgmConfigChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterHgmRefreshRateModeChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterHgmRefreshRateModeChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::RegisterHgmRefreshRateUpdateCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterHgmRefreshRateUpdateCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (callback) {
        if (!data.WriteBool(true)) {
            ROSEN_LOGE("RegisterHgmRefreshRateUpdateCallback: WriteBool [true] err.");
            return WRITE_PARCEL_ERR;
        }
        if (!data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("RegisterHgmRefreshRateUpdateCallback: WriteRemoteObject callback->AsObject() err.");
            return WRITE_PARCEL_ERR;
        }
    } else {
        if (!data.WriteBool(false)) {
            ROSEN_LOGE("RegisterHgmRefreshRateUpdateCallback: WriteBool [false] err.");
            return WRITE_PARCEL_ERR;
        }
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::RegisterFirstFrameCommitCallback(
    sptr<RSIFirstFrameCommitCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterFirstFrameCommitCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (callback) {
        if (!data.WriteBool(true)) {
            ROSEN_LOGE("RegisterFirstFrameCommitCallback: WriteBool [true] err.");
            return WRITE_PARCEL_ERR;
        }
        if (!data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("RegisterFirstFrameCommitCallback: WriteRemoteObject callback->AsObject() err.");
            return WRITE_PARCEL_ERR;
        }
    } else {
        if (!data.WriteBool(false)) {
            ROSEN_LOGE("RegisterFirstFrameCommitCallback: WriteBool [false] err.");
            return WRITE_PARCEL_ERR;
        }
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::ON_FIRST_FRAME_COMMIT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterFirstFrameCommitCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

ErrCode RSClientToServiceConnectionProxy::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("AvcodecVideoStart: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUInt64Vector(uniqueIdList)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteUInt64Vector uniqueIdList err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteStringVector(surfaceNameList)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteStringVector surfaceNameList err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(fps)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteUint32 fps err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(reportTime)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteUint64 reportTime err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::AvcodecVideoStart: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::AvcodecVideoStart Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

ErrCode RSClientToServiceConnectionProxy::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("AvcodecVideoStop: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUInt64Vector(uniqueIdList)) {
        ROSEN_LOGE("AvcodecVideoStop: WriteUInt64Vector uniqueIdList err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteStringVector(surfaceNameList)) {
        ROSEN_LOGE("AvcodecVideoStop: WriteStringVector surfaceNameList err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(fps)) {
        ROSEN_LOGE("AvcodecVideoStop: WriteUint32 fps err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::AvcodecVideoStop: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::AvcodecVideoStop Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

ErrCode RSClientToServiceConnectionProxy::AvcodecVideoGet(uint64_t uniqueId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
 
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("AvcodecVideoGet: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(uniqueId)) {
        ROSEN_LOGE("AvcodecVideoGet: WriteUint64 uniqueId err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_GET);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::AvcodecVideoGet: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::AvcodecVideoGet Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}
 
ErrCode RSClientToServiceConnectionProxy::AvcodecVideoGetRecent()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
 
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("AvcodecVideoGetRecent: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_GET_RECENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::AvcodecVideoGetRecent: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::AvcodecVideoGetRecent Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid,
    sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterFrameRateLinkerExpectedFpsUpdateCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(dstPid)) {
        ROSEN_LOGE("RegisterFrameRateLinkerExpectedFpsUpdateCallback: WriteInt32 dstPid err.");
        return WRITE_PARCEL_ERR;
    }
    if (callback) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("RegisterFrameRateLinkerExpectedFpsUpdateCallback: WriteBool[T] OR WriteRemoteObject[CB] err");
            return WRITE_PARCEL_ERR;
        }
    } else {
        if (!data.WriteBool(false)) {
            ROSEN_LOGE("RegisterFrameRateLinkerExpectedFpsUpdateCallback: WriteBool [false] err.");
            return WRITE_PARCEL_ERR;
        }
    }

    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterFrameRateLinkerExpectedFpsUpdateCallback: "
            "Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE(
            "RSClientToServiceConnectionProxy::RegisterFrameRateLinkerExpectedFpsUpdateCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

ErrCode RSClientToServiceConnectionProxy::SetWatermark(const std::string& name,
    std::shared_ptr<Media::PixelMap> watermark, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetWatermark: WriteInterfaceToken GetDescriptor err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteString(name)) {
        ROSEN_LOGE("SetWatermark: WriteString name err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(watermark.get())) {
        ROSEN_LOGE("SetWatermark: WriteParcelable watermark.get() err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_WATERMARK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetWatermark: Send Request err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    success = true;
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    if (watermarkImg == nullptr) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ShowWatermark: watermarkImg is nullptr.");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ShowWatermark: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteParcelable(watermarkImg.get())) {
        ROSEN_LOGE("ShowWatermark: WriteParcelable watermarkImg.get() err.");
        return;
    }
    if (!data.WriteBool(isShow)) {
        ROSEN_LOGE("ShowWatermark: WriteBool isShow err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SHOW_WATERMARK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ShowWatermark: Send Request err.");
        return;
    }
}

int32_t RSClientToServiceConnectionProxy::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ResizeVirtualScreen: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("ResizeVirtualScreen: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(width)) {
        ROSEN_LOGE("ResizeVirtualScreen: WriteUint32 width err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(height)) {
        ROSEN_LOGE("ResizeVirtualScreen: WriteUint32 height err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ResizeVirtualScreen: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ResizeVirtualScreen Read status failed");
        return READ_PARCEL_ERR;
    }
    return status;
}

ErrCode RSClientToServiceConnectionProxy::ReportJankStats()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportJankStats: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_JANK_STATS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ReportJankStats: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::ReportEventResponse(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventResponse: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ReportEventResponse: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::ReportEventComplete(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventComplete: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ReportEventComplete: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::ReportEventJankFrame(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventJankFrame: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ReportEventJankFrame: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::ReportRsSceneJankStart(AppInfo info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportRsSceneJankStart: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    WriteAppInfo(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ReportRsSceneJankStart: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::ReportRsSceneJankEnd(AppInfo info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportRsSceneJankEnd: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    WriteAppInfo(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ReportRsSceneJankEnd: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::ReportDataBaseRs(
    MessageParcel& data, MessageParcel& reply, MessageOption& option, DataBaseRs info)
{
    if (!data.WriteInt32(info.appPid)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt32 info.appPid err.");
        return;
    }
    if (!data.WriteInt32(info.eventType)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt32 info.eventType err.");
        return;
    }
    if (!data.WriteInt32(info.versionCode)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt32 info.versionCode err.");
        return;
    }
    if (!data.WriteInt64(info.uniqueId)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt64 info.uniqueId err.");
        return;
    }
    if (!data.WriteInt64(info.inputTime)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt64 info.inputTime err.");
        return;
    }
    if (!data.WriteInt64(info.beginVsyncTime) || !data.WriteInt64(info.endVsyncTime)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt64 info.beginVsyncTime OR info.endVsyncTime err.");
        return;
    }
    if (!data.WriteBool(info.isDisplayAnimator)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteBool info.isDisplayAnimator err.");
        return;
    }
    if (!data.WriteString(info.sceneId)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.sceneId err.");
        return;
    }
    if (!data.WriteString(info.versionName)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.versionName err.");
        return;
    }
    if (!data.WriteString(info.bundleName)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.bundleName err.");
        return;
    }
    if (!data.WriteString(info.processName)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.processName err.");
        return;
    }
    if (!data.WriteString(info.abilityName)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.abilityName err.");
        return;
    }
    if (!data.WriteString(info.pageUrl)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.pageUrl err.");
        return;
    }
    if (!data.WriteString(info.sourceType)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.sourceType err.");
        return;
    }
    if (!data.WriteString(info.note)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.note err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
}

void RSClientToServiceConnectionProxy::WriteAppInfo(
    MessageParcel& data, MessageParcel& reply, MessageOption& option, AppInfo info)
{
    if (!data.WriteInt64(info.startTime)) {
        ROSEN_LOGE("WriteAppInfo: WriteInt64 info.startTime err.");
        return;
    }
    if (!data.WriteInt64(info.endTime)) {
        ROSEN_LOGE("WriteAppInfo: WriteInt64 info.endTime err.");
        return;
    }
    if (!data.WriteInt32(info.pid)) {
        ROSEN_LOGE("WriteAppInfo: WriteInt32 info.pid err.");
        return;
    }
    if (!data.WriteString(info.versionName)) {
        ROSEN_LOGE("WriteAppInfo: WriteString info.versionName err.");
        return;
    }
    if (!data.WriteInt32(info.versionCode)) {
        ROSEN_LOGE("WriteAppInfo: WriteInt32 info.versionCode err.");
        return;
    }
    if (!data.WriteString(info.bundleName)) {
        ROSEN_LOGE("WriteAppInfo: WriteString info.bundleName err.");
        return;
    }
    if (!data.WriteString(info.processName)) {
        ROSEN_LOGE("WriteAppInfo: WriteString info.processName err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
}

void RSClientToServiceConnectionProxy::ReportGameStateDataRs(
    MessageParcel& data, MessageParcel& reply, MessageOption& option, GameStateData info)
{
    if (!data.WriteInt32(info.pid)) {
        ROSEN_LOGE("ReportGameStateDataRs: WriteInt32 info.pid err.");
        return;
    }
    if (!data.WriteInt32(info.uid)) {
        ROSEN_LOGE("ReportGameStateDataRs: WriteInt32 info.uid err.");
        return;
    }
    if (!data.WriteInt32(info.state)) {
        ROSEN_LOGE("ReportGameStateDataRs: WriteInt32 info.state err.");
        return;
    }
    if (!data.WriteInt32(info.renderTid)) {
        ROSEN_LOGE("ReportGameStateDataRs: WriteInt32 info.renderTid err.");
        return;
    }
    if (!data.WriteString(info.bundleName)) {
        ROSEN_LOGE("ReportGameStateDataRs: WriteString info.bundleName err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
}

void RSClientToServiceConnectionProxy::ReportGameStateData(GameStateData info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportGameStateData: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    ReportGameStateDataRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ReportGameStateData: Send Request err.");
        return;
    }
}

ErrCode RSClientToServiceConnectionProxy::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyLightFactorStatus: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(lightFactorStatus)) {
        ROSEN_LOGE("NotifyLightFactorStatus: WriteInt32 lightFactorStatus err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyLightFactorStatus: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::NotifyPackageEvent(
    uint32_t listSize, const std::vector<std::string>& packageList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyPackageEvent: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (listSize != packageList.size()) {
        ROSEN_LOGE("input size doesn't match");
        return;
    }
    if (!data.WriteUint32(listSize)) {
        ROSEN_LOGE("NotifyPackageEvent: WriteUint32 listSize err.");
        return;
    }
    for (auto pkg : packageList) {
        if (!data.WriteString(pkg)) {
            ROSEN_LOGE("NotifyPackageEvent: WriteString pkg err.");
            return;
        }
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyPackageEvent: Send Request err.");
        return;
    }
}

void RSClientToServiceConnectionProxy::SetWindowExpectedRefreshRate(
    const std::unordered_map<uint64_t, EventInfo>& eventInfos
)
{
    auto mapSize = eventInfos.size();
    if (mapSize <= ZERO || mapSize > MAX_VOTER_SIZE) {
        ROSEN_LOGE("SetWindowExpectedRefreshRate: map size err.");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetWindowExpectedRefreshRate: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteUint32(mapSize)) {
        ROSEN_LOGE("SetWindowExpectedRefreshRate: WriteUint32 mapSize err.");
        return;
    }
    for (const auto& [key, eventInfo] : eventInfos) {
        if (!data.WriteUint64(key)) {
            ROSEN_LOGE("SetWindowExpectedRefreshRate: WriteUint64 key err.");
            return;
        }
        if (!eventInfo.Serialize(data)) {
            ROSEN_LOGE("SetWindowExpectedRefreshRate: Write eventInfo err.");
            return;
        }
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetWindowExpectedRefreshRate: Send Request err.");
        return;
    }
}

void RSClientToServiceConnectionProxy::SetWindowExpectedRefreshRate(
    const std::unordered_map<std::string, EventInfo>& eventInfos
)
{
    auto mapSize = eventInfos.size();
    if (mapSize <= ZERO || mapSize > MAX_VOTER_SIZE) {
        ROSEN_LOGE("SetWindowExpectedRefreshRate: map size err.");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetWindowExpectedRefreshRate: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteUint32(mapSize)) {
        ROSEN_LOGE("SetWindowExpectedRefreshRate: WriteUint32 mapSize err.");
        return;
    }
    for (const auto& [key, eventInfo] : eventInfos) {
        if (!data.WriteString(key)) {
            ROSEN_LOGE("SetWindowExpectedRefreshRate: WriteString key err.");
            return;
        }
        if (!eventInfo.Serialize(data)) {
            ROSEN_LOGE("SetWindowExpectedRefreshRate: Write eventInfo err.");
            return;
        }
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetWindowExpectedRefreshRate: Send Request err.");
        return;
    }
}

ErrCode RSClientToServiceConnectionProxy::NotifyAppStrategyConfigChangeEvent(
    const std::string& pkgName, uint32_t listSize, const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return ERR_INVALID_VALUE;
    }

    if (listSize != newConfig.size()) {
        ROSEN_LOGE("input size doesn't match");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteString(pkgName) || !data.WriteUint32(listSize)) {
        ROSEN_LOGE(
            "RSClientToServiceConnectionProxy::NotifyAppStrategyConfigChangeEvent Write pakName or listSize failed.");
        return ERR_INVALID_VALUE;
    }

    for (const auto& [key, value] : newConfig) {
        if (!data.WriteString(key) || !data.WriteString(value)) {
            ROSEN_LOGE(
                "RSClientToServiceConnectionProxy::NotifyAppStrategyConfigChangeEvent Write key or value failed.");
            return ERR_INVALID_VALUE;
        }
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyAppStrategyConfigChangeEvent: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyRefreshRateEvent: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteString(eventInfo.eventName)) {
        ROSEN_LOGE("NotifyRefreshRateEvent: WriteString eventInfo.eventName err.");
        return;
    }
    if (!data.WriteBool(eventInfo.eventStatus)) {
        ROSEN_LOGE("NotifyRefreshRateEvent: WriteBool eventInfo.eventStatus err.");
        return;
    }
    if (!data.WriteUint32(eventInfo.minRefreshRate)) {
        ROSEN_LOGE("NotifyRefreshRateEvent: WriteUint32 eventInfo.minRefreshRate err.");
        return;
    }
    if (!data.WriteUint32(eventInfo.maxRefreshRate)) {
        ROSEN_LOGE("NotifyRefreshRateEvent: WriteUint32 eventInfo.maxRefreshRate err.");
        return;
    }
    if (!data.WriteString(eventInfo.description)) {
        ROSEN_LOGE("NotifyRefreshRateEvent: WriteString eventInfo.description err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyRefreshRateEvent: Send Request err.");
        return;
    }
}

ErrCode RSClientToServiceConnectionProxy::NotifySoftVsyncEvent(uint32_t pid, uint32_t rateDiscount)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifySoftVsyncEvent: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(pid)) {
        ROSEN_LOGE("NotifySoftVsyncEvent: WriteUint32 pid err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(rateDiscount)) {
        ROSEN_LOGE("NotifySoftVsyncEvent: WriteUint32 rateDiscount err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifySoftVsyncEvent: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

bool RSClientToServiceConnectionProxy::NotifySoftVsyncRateDiscountEvent(uint32_t pid, const std::string &name,
    uint32_t rateDiscount)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifySoftVsyncRateDiscountEvent: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    if (!data.WriteUint32(pid)) {
        ROSEN_LOGE("NotifySoftVsyncRateDiscountEvent: WriteUint32 pid err.");
        return false;
    }
    if (!data.WriteString(name)) {
        ROSEN_LOGE("NotifySoftVsyncRateDiscountEvent: WriteString rateDiscount err.");
        return false;
    }
    if (!data.WriteUint32(rateDiscount)) {
        ROSEN_LOGE("NotifySoftVsyncRateDiscountEvent: WriteUint32 rateDiscount err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifySoftVsyncRateDiscountEvent: Send Request err.");
        return false;
    }
    bool enable{false};
    if (!reply.ReadBool(enable)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifySoftVsyncRateDiscountEvent: Read enable failed");
        return false;
    }
    return enable;
}

ErrCode RSClientToServiceConnectionProxy::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt, int32_t sourceType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyTouchEvent: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(touchStatus)) {
        ROSEN_LOGE("NotifyTouchEvent: WriteInt32 touchStatus err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(touchCnt)) {
        ROSEN_LOGE("NotifyTouchEvent: WriteInt32 touchCnt err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(sourceType)) {
        ROSEN_LOGE("NotifyTouchEvent: WriteInt32 sourceType err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyTouchEvent: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::NotifyDynamicModeEvent(bool enableDynamicMode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyDynamicModeEvent: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(enableDynamicMode)) {
        ROSEN_LOGE("NotifyDynamicModeEvent: WriteBool enableDynamicMode err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_DYNAMIC_MODE_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyDynamicModeEvent: Send Request err.");
        return;
    }
}

ErrCode RSClientToServiceConnectionProxy::NotifyHgmConfigEvent(const std::string &eventName, bool state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyHgmConfigEvent: GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(eventName)) {
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(state)) {
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_HGMCONFIG_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyHgmConfigEvent: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::NotifyXComponentExpectedFrameRate(
    const std::string& id, int32_t expectedFrameRate)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyXComponentExpectedFrameRate: GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(id)) {
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(expectedFrameRate)) {
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyXComponentExpectedFrameRate: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::SetCacheEnabledForRotation(bool isEnabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetCacheEnabledForRotation: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isEnabled)) {
        ROSEN_LOGE("SetCacheEnabledForRotation: WriteBool isEnabled err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetCacheEnabledForRotation: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback)
{
    OnRemoteDiedCallback_ = callback;
}

void RSClientToServiceConnectionProxy::RunOnRemoteDiedCallback()
{
    if (OnRemoteDiedCallback_) {
        OnRemoteDiedCallback_();
    }
}

std::vector<ActiveDirtyRegionInfo> RSClientToServiceConnectionProxy::GetActiveDirtyRegionInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<ActiveDirtyRegionInfo> activeDirtyRegionInfos;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetActiveDirtyRegionInfo: WriteInterfaceToken GetDescriptor err.");
        return activeDirtyRegionInfos;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetActiveDirtyRegionInfo: Send Request err.");
        return activeDirtyRegionInfos;
    }
    int32_t activeDirtyRegionInfosSize{0};
    if (!reply.ReadInt32(activeDirtyRegionInfosSize)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetActiveDirtyRegionInfo Read activeDirtyRegionInfosSize failed");
        return activeDirtyRegionInfos;
    }
    while (activeDirtyRegionInfosSize--) {
        int64_t activeDirtyRegionArea{0};
        int32_t activeFramesNumber{0};
        int32_t pidOfBelongsApp{0};
        std::string windowName;
        if (!reply.ReadInt64(activeDirtyRegionArea) || !reply.ReadInt32(activeFramesNumber) ||
            !reply.ReadInt32(pidOfBelongsApp) || !reply.ReadString(windowName)) {
            ROSEN_LOGE(
                "RSClientToServiceConnectionProxy::GetActiveDirtyRegionInfo Read parcel failed");
            return activeDirtyRegionInfos;
        }
        activeDirtyRegionInfos.emplace_back(
            ActiveDirtyRegionInfo(activeDirtyRegionArea, activeFramesNumber, pidOfBelongsApp, windowName));
    }
    return activeDirtyRegionInfos;
}

GlobalDirtyRegionInfo RSClientToServiceConnectionProxy::GetGlobalDirtyRegionInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    GlobalDirtyRegionInfo globalDirtyRegionInfo;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetGlobalDirtyRegionInfo: WriteInterfaceToken GetDescriptor err.");
        return globalDirtyRegionInfo;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetGlobalDirtyRegionInfo: Send Request err.");
        return globalDirtyRegionInfo;
    }
    int64_t globalDirtyRegionAreas{0};
    int32_t globalFramesNumber{0};
    int32_t skipProcessFramesNumber{0};
    int32_t commandCount{0};
    int32_t consumeBufferSize{0};
    int32_t frameAnimationCount{0};
    int32_t mostSendingPidWhenDisplayNodeSkip{0};
    if (!reply.ReadInt64(globalDirtyRegionAreas) || !reply.ReadInt32(globalFramesNumber) ||
        !reply.ReadInt32(skipProcessFramesNumber) || !reply.ReadInt32(commandCount) ||
        !reply.ReadInt32(consumeBufferSize) || !reply.ReadInt32(frameAnimationCount) ||
        !reply.ReadInt32(mostSendingPidWhenDisplayNodeSkip)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetGlobalDirtyRegionInfo Read parcel failed");
        return globalDirtyRegionInfo;
    }
    return GlobalDirtyRegionInfo(
        globalDirtyRegionAreas, globalFramesNumber, skipProcessFramesNumber, commandCount,
        consumeBufferSize, frameAnimationCount, mostSendingPidWhenDisplayNodeSkip);
}


LayerComposeInfo RSClientToServiceConnectionProxy::GetLayerComposeInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    LayerComposeInfo layerComposeInfo;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetLayerComposeInfo: WriteInterfaceToken GetDescriptor err.");
        return layerComposeInfo;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetLayerComposeInfo: Send Request err.");
        return layerComposeInfo;
    }
    int32_t uniformRenderFrameNumber{0};
    int32_t offlineComposeFrameNumber{0};
    int32_t redrawFrameNumber{0};
    int32_t drawImageNumber{0};
    if (!reply.ReadInt32(uniformRenderFrameNumber) || !reply.ReadInt32(offlineComposeFrameNumber) ||
        !reply.ReadInt32(redrawFrameNumber) || !reply.ReadInt32(drawImageNumber)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetLayerComposeInfo Read parcel failed");
        return layerComposeInfo;
    }
    return LayerComposeInfo(uniformRenderFrameNumber, offlineComposeFrameNumber, redrawFrameNumber, drawImageNumber);
}

HwcDisabledReasonInfos RSClientToServiceConnectionProxy::GetHwcDisabledReasonInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    HwcDisabledReasonInfos hwcDisabledReasonInfos;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetHwcDisabledReasonInfo: WriteInterfaceToken GetDescriptor err.");
        return hwcDisabledReasonInfos;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::
        GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetHwcDisabledReasonInfo: Send Request err.");
        return hwcDisabledReasonInfos;
    }
    int32_t size{0};
    if (!reply.ReadInt32(size)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetHwcDisabledReasonInfo Read size failed");
        return hwcDisabledReasonInfos;
    }
    size_t readableSize = reply.GetReadableBytes() / HWC_DISABLED_REASON_INFO_MINIMUM_SIZE;
    size_t len = static_cast<size_t>(size);
    if (len > readableSize || len > hwcDisabledReasonInfos.max_size()) {
        RS_LOGE("RSClientToServiceConnectionProxy::GetDescriptor() GetHwcDisabledReasonInfo Failed read vector, "
                "size:%{public}zu, readableSize:%{public}zu",
            len, readableSize);
        return hwcDisabledReasonInfos;
    }

    HwcDisabledReasonInfo hwcDisabledReasonInfo;
    while (size--) {
        for (int32_t pos = 0; pos < HwcDisabledReasons::DISABLED_REASON_LENGTH; pos++) {
            hwcDisabledReasonInfo.disabledReasonStatistics[pos] = reply.ReadInt32();
        }
        hwcDisabledReasonInfo.pidOfBelongsApp = reply.ReadInt32();
        hwcDisabledReasonInfo.nodeName = reply.ReadString();
        hwcDisabledReasonInfos.emplace_back(hwcDisabledReasonInfo);
    }
    return hwcDisabledReasonInfos;
}

ErrCode RSClientToServiceConnectionProxy::GetHdrOnDuration(int64_t& hdrOnDuration)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetHdrOnDuration: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_HDR_ON_DURATION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    hdrOnDuration = reply.ReadInt64();
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::SetVmaCacheStatus(bool flag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVmaCacheStatus: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(flag)) {
        ROSEN_LOGE("SetVmaCacheStatus: WriteBool flag err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVmaCacheStatus %d: Send Request err.", flag);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

#ifdef TP_FEATURE_ENABLE
EErrCode RSClientToServiceConnectionProxy::SetTpFeatureConfig(int32_t feature, const char* config,
    TpFeatureConfigType tpFeatureConfigType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetTpFeatureConfig: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(feature)) {
        ROSEN_LOGE("SetTpFeatureConfig: WriteInt32 feature err.");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteCString(config)) {
        ROSEN_LOGE("SetTpFeatureConfig: WriteCString config err.");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteUint8(static_cast<uint8_t>(tpFeatureConfigType))) {
        ROSEN_LOGE("SetTpFeatureConfig: WriteUint8 tpFeatureConfigType err.");
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}
#endif

void RSClientToServiceConnectionProxy::SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenUsingStatus: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(isVirtualScreenUsingStatus)) {
        ROSEN_LOGE("SetVirtualScreenUsingStatus: WriteBool isVirtualScreenUsingStatus err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenUsingStatus: Send Request err.");
        return;
    }
}

ErrCode RSClientToServiceConnectionProxy::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    return ERR_OK; // todo
}

ErrCode RSClientToServiceConnectionProxy::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    return ERR_OK; // todo
}

ErrCode RSClientToServiceConnectionProxy::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetCurtainScreenUsingStatus: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isCurtainScreenOn)) {
        ROSEN_LOGE("SetCurtainScreenUsingStatus: WriteBool isCurtainScreenOn err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetCurtainScreenUsingStatus: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSClientToServiceConnectionProxy::RegisterUIExtensionCallback(
    uint64_t userId, sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterUIExtensionCallback: callback is nullptr.");
        return INVALID_ARGUMENTS;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterUIExtensionCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (data.WriteUint64(userId) && data.WriteRemoteObject(callback->AsObject()) && data.WriteBool(unobscured)) {
        uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            return RS_CONNECTION_ERROR;
        }
        int32_t result{0};
        if (!reply.ReadInt32(result)) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterUIExtensionCallback Read result failed");
            return READ_PARCEL_ERR;
        }
        return result;
    } else {
        ROSEN_LOGE("RegisterUIExtensionCallback: WriteUint64[userId] OR WriteRemoteObject[callback] err.");
        return RS_CONNECTION_ERROR;
    }
}

ErrCode RSClientToServiceConnectionProxy::SetVirtualScreenStatus(ScreenId id,
    VirtualScreenStatus screenStatus, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenStatus: WriteInterfaceToken GetDescriptor err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenStatus: WriteUint64 id err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(screenStatus))) {
        ROSEN_LOGE("SetVirtualScreenStatus: WriteUint8 screenStatus err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadBool(success)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetVirtualScreenStatus Read result failed");
        return READ_PARCEL_ERR;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetLayerTop: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteString(nodeIdStr) && data.WriteBool(isTop)) {
        uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_LAYER_TOP);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::SetLayerTop: Send Request err.");
            return ERR_INVALID_VALUE;
        }
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetForceRefresh: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteString(nodeIdStr) && data.WriteBool(isForceRefresh)) {
        uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_FORCE_REFRESH);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::SetForceRefresh: Send Request err.");
            return ERR_INVALID_VALUE;
        }
    }
    return ERR_OK;
}

void RSClientToServiceConnectionProxy::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetColorFollow: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteString(nodeIdStr) && data.WriteBool(isColorFollow)) {
        uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_COLOR_FOLLOW);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::SetColorFollow: Send Request err.");
            return;
        }
    }
}

void RSClientToServiceConnectionProxy::SetFreeMultiWindowStatus(bool enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetFreeMultiWindowStatus: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteBool(enable)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetFreeMultiWindowStatus: write bool val err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetFreeMultiWindowStatus: Send Request err.");
    }
}

int32_t RSClientToServiceConnectionProxy::RegisterSelfDrawingNodeRectChangeCallback(
    const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    if (!callback) {
        ROSEN_LOGE("%{public}s callback is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);

    uint32_t size = constraint.pids.size();
    if (!data.WriteUint32(size)) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Write size err.");
        return WRITE_PARCEL_ERR;
    }
    for (int32_t pid : constraint.pids) {
        if (!data.WriteInt32(pid)) {
            ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Write pid err.");
            return WRITE_PARCEL_ERR;
        }
    }

    if (!data.WriteInt32(constraint.range.lowLimit.width) || !data.WriteInt32(constraint.range.lowLimit.height) ||
        !data.WriteInt32(constraint.range.highLimit.width) || !data.WriteInt32(constraint.range.highLimit.height)) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Write rectRange err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Send request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::RegisterSelfDrawingNodeRectChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::UnRegisterSelfDrawingNodeRectChangeCallback()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("UnRegisterSelfDrawingNodeRectChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);

    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("UnRegisterSelfDrawingNodeRectChangeCallback: Send request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::UnRegisterSelfDrawingNodeRectChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToServiceConnectionProxy::SendRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    if (!Remote()) {
        return static_cast<int32_t>(RSInterfaceErrorCode::NULLPTR_ERROR);
    }
    return Remote()->SendRequest(code, data, reply, option);
}

ErrCode RSClientToServiceConnectionProxy::NotifyScreenSwitched()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: Write InterfaceToken val err.", __func__);
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request error.", __func__);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::NotifyPageName(const std::string& packageName,
    const std::string& pageName, bool isEnter)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyPageName: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteString(packageName) && data.WriteString(pageName) && data.WriteBool(isEnter)) {
        uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_PAGE_NAME);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyPageName: Send Request err.");
            return ERR_INVALID_VALUE;
        }
    } else {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::NotifyPageName: write data err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSClientToServiceConnectionProxy::SetOverlayDisplayMode(int32_t mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: Write InterfaceToken val err.", __func__);
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(mode)) {
        ROSEN_LOGE("%{public}s: Write Int32 val err.", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_OVERLAY_DISPLAY_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed. err:%{public}d.", __func__, err);
        return ERR_INVALID_VALUE;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetOverlayDisplayMode Read result failed");
        return READ_PARCEL_ERR;
    }
    ROSEN_LOGI("%{public}s: mode:%{public}d, result:%{public}d.", __func__, mode, result);
    return result == 0 ? ERR_OK : ERR_INVALID_VALUE;
}
#endif

ErrCode RSClientToServiceConnectionProxy::SetBehindWindowFilterEnabled(bool enabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetBehindWindowFilterEnabled WriteInterfaceToken err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteBool(enabled)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetBehindWindowFilterEnabled WriteBool err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE(
            "RSClientToServiceConnectionProxy::SetBehindWindowFilterEnabled sendrequest error : %{public}d", err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToServiceConnectionProxy::GetBehindWindowFilterEnabled(bool& enabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::GetBehindWindowFilterEnabled WriteInterfaceToken err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE(
            "RSClientToServiceConnectionProxy::GetBehindWindowFilterEnabled sendrequest error : %{public}d", err);
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadBool(enabled)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetBehindWindowFilterEnabled ReadBool err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSClientToServiceConnectionProxy::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return RS_CONNECTION_ERROR;
    }

    if (!data.WriteInt32(pid)) {
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PID_GPU_MEMORY_IN_MB);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    if (!reply.ReadFloat(gpuMemInMB)) {
        return READ_PARCEL_ERR;
    }

    return err;
}

RetCodeHrpService RSClientToServiceConnectionProxy::ProfilerServiceOpenFile(const HrpServiceDirInfo& dirInfo,
    const std::string& fileName, int32_t flags, int& outFd)
{
    const uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ProfilerServiceOpenFile WriteInterfaceToken err.");
        return RET_HRP_SERVICE_ERR_UNKNOWN;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint32((uint32_t)dirInfo.baseDirType);
    data.WriteString(dirInfo.subDir);
    data.WriteString(dirInfo.subDir2);
    data.WriteString(fileName);
    data.WriteInt32(flags);

    int32_t err = SendRequest(code, data, reply, option);
    if (err != ERR_NONE) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ProfilerServiceOpenFile sendrequest error : %{public}d", err);
        return RET_HRP_SERVICE_ERR_PROXY_SEND_REQUEST;
    }

    int32_t retCode = RET_HRP_SERVICE_ERR_UNKNOWN;
    if (!reply.ReadInt32(retCode)) {
        return RET_HRP_SERVICE_ERR_PROXY_INVALID_RESULT;
    }

    int retFd = reply.ReadFileDescriptor();
    if (retFd == -1) {
        return retCode < 0 ? (RetCodeHrpService)retCode : RET_HRP_SERVICE_ERR_INVALID_FILE_DESCRIPTOR;
    }

    outFd = retFd;
    return (RetCodeHrpService)retCode;
}

RetCodeHrpService RSClientToServiceConnectionProxy::ProfilerServicePopulateFiles(const HrpServiceDirInfo& dirInfo,
    uint32_t firstFileIndex, std::vector<HrpServiceFileInfo>& outFiles)
{
    const uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ProfilerServicePopulateFiles WriteInterfaceToken err.");
        return RET_HRP_SERVICE_ERR_UNKNOWN;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint32((uint32_t)dirInfo.baseDirType);
    data.WriteString(dirInfo.subDir);
    data.WriteString(dirInfo.subDir2);
    data.WriteUint32(firstFileIndex);

    int32_t err = SendRequest(code, data, reply, option);
    if (err != ERR_NONE) {
        ROSEN_LOGE(
            "RSClientToServiceConnectionProxy::ProfilerServicePopulateFiles sendrequest error : %{public}d", err);
        return RET_HRP_SERVICE_ERR_PROXY_SEND_REQUEST;
    }

    int32_t retCode = RET_HRP_SERVICE_ERR_UNKNOWN;
    if (!reply.ReadInt32(retCode)) {
        return RET_HRP_SERVICE_ERR_PROXY_INVALID_RESULT;
    }
    if (retCode < RET_HRP_SERVICE_SUCCESS) {
        return (RetCodeHrpService)retCode;
    }

    uint32_t retCount = 0;
    if (!reply.ReadUint32(retCount)) {
        return RET_HRP_SERVICE_ERR_PROXY_INVALID_RESULT;
    }

    std::vector<HrpServiceFileInfo> retFiles;
    for (uint32_t i = 0; i < retCount; i++) {
        HrpServiceFileInfo fi {};
        if (!reply.ReadString(fi.name) || !reply.ReadUint32(fi.size) || !reply.ReadBool(fi.isDir) ||
            !reply.ReadUint32(fi.accessBits) || !reply.ReadUint64(fi.accessTime.sec) ||
            !reply.ReadUint64(fi.accessTime.nsec) || !reply.ReadUint64(fi.modifyTime.sec) ||
            !reply.ReadUint64(fi.modifyTime.nsec)) {
            return RET_HRP_SERVICE_ERR_PROXY_INVALID_RESULT_DATA;
        }
        retFiles.emplace_back(fi);
    }
    outFiles.swap(retFiles);
    return (RetCodeHrpService)retCode;
}

bool RSClientToServiceConnectionProxy::ProfilerIsSecureScreen()
{
    const uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::PROFILER_IS_SECURE_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ProfilerIsSecureScreen WriteInterfaceToken err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != ERR_NONE) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ProfilerIsSecureScreen sendrequest error : %{public}d", err);
        return false;
    }

    bool retValue = false;
    if (!reply.ReadBool(retValue)) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::ProfilerIsSecureScreen ReadBool err.");
        return false;
    }
    return retValue;
}

ErrCode RSClientToServiceConnectionProxy::SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList)
{
    return ERR_INVALID_VALUE;
}

ErrCode RSClientToServiceConnectionProxy::SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList)
{
    return ERR_INVALID_VALUE;
}
} // namespace Rosen
} // namespace OHOS
