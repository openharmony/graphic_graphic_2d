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

#include "rs_render_service_connection_proxy.h"

#include <algorithm>
#include <cstdint>
#include <message_option.h>
#include <message_parcel.h>
#include <vector>
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_marshalling_helper.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
static constexpr size_t ASHMEM_SIZE_THRESHOLD = 200 * 1024; // cannot > 500K in TF_ASYNC mode
static constexpr int MAX_RETRY_COUNT = 20;
static constexpr int RETRY_WAIT_TIME_US = 1000; // wait 1ms before retry SendRequest
static constexpr int MAX_SECURITY_EXEMPTION_LIST_NUMBER = 1024; // securityExemptionList size not exceed 1024
}

RSRenderServiceConnectionProxy::RSRenderServiceConnectionProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIRenderServiceConnection>(impl)
{
}

void RSRenderServiceConnectionProxy::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    if (!transactionData) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CommitTransaction transactionData nullptr!");
        return;
    }
    bool isUniMode = RSSystemProperties::GetUniRenderEnabled();
    transactionData->SetSendingPid(pid_);

    // split to several parcels if parcel size > PARCEL_SPLIT_THRESHOLD during marshalling
    std::vector<std::shared_ptr<MessageParcel>> parcelVector;
    auto func = [isUniMode, &parcelVector, &transactionData, this]() -> bool {
        if (isUniMode) {
            ++transactionDataIndex_;
        }
        transactionData->SetIndex(transactionDataIndex_);
        std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
        if (!FillParcelWithTransactionData(transactionData, parcel)) {
            ROSEN_LOGE("FillParcelWithTransactionData failed!");
            return false;
        }
        parcelVector.emplace_back(parcel);
        return true;
    };
    if (transactionData->IsNeedSync() && transactionData->IsEmpty()) {
        RS_TRACE_NAME("Commit empty syncTransaction");
        func();
    } else {
        while (transactionData->GetMarshallingIndex() < transactionData->GetCommandCount()) {
            if (!func()) {
                return;
            }
        }
    }

    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    for (const auto& parcel : parcelVector) {
        MessageParcel reply;
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION);
        int retryCount = 0;
        int32_t err = NO_ERROR;
        do {
            err = SendRequest(code, *parcel, reply, option);
            if (err != NO_ERROR && retryCount < MAX_RETRY_COUNT) {
                retryCount++;
                usleep(RETRY_WAIT_TIME_US);
            } else if (err != NO_ERROR) {
                ROSEN_LOGE("RSRenderServiceConnectionProxy::CommitTransaction SendRequest failed, "
                    "err = %{public}d, retryCount = %{public}d, data size:%{public}zu", err, retryCount,
                    parcel->GetDataSize());
                return;
            }
        } while (err != NO_ERROR);
    }
}

void RSRenderServiceConnectionProxy::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSRenderServiceConnectionProxy::GetDescriptor())) {
        ROSEN_LOGE("ExecuteSynchronousTask WriteInterfaceToken failed");
        return;
    }
    if (!task->Marshalling(data)) {
        ROSEN_LOGE("ExecuteSynchronousTask Marshalling failed");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }

    if (task->CheckHeader(reply)) {
        task->ReadFromParcel(reply);
    }
}

bool RSRenderServiceConnectionProxy::FillParcelWithTransactionData(
    std::unique_ptr<RSTransactionData>& transactionData, std::shared_ptr<MessageParcel>& data)
{
    // write a flag at the begin of parcel to identify parcel type
    // 0: indicate normal parcel
    // 1: indicate ashmem parcel
    if (!data->WriteInt32(0)) {
        ROSEN_LOGE("FillParcelWithTransactionData WriteInt32 failed");
        return false;
    }

    // 1. marshalling RSTransactionData
    RS_TRACE_BEGIN("MarshRSTransactionData cmdCount:" + std::to_string(transactionData->GetCommandCount()) +
        " transactionFlag:[" + std::to_string(pid_) + "," + std::to_string(transactionData->GetIndex()) + "]");
    ROSEN_LOGI_IF(DEBUG_PIPELINE,
        "MarshRSTransactionData cmdCount:%{public}lu transactionFlag:[pid:%{public}d index:%{public}" PRIu64 "]",
        transactionData->GetCommandCount(), pid_, transactionData->GetIndex());
    bool success = data->WriteParcelable(transactionData.get());
    RS_TRACE_END();
    if (!success) {
        ROSEN_LOGE("FillParcelWithTransactionData data.WriteParcelable failed!");
        return false;
    }

    // 2. convert data to new ashmem parcel if size over threshold
    std::shared_ptr<MessageParcel> ashmemParcel = nullptr;
    if (data->GetDataSize() > ASHMEM_SIZE_THRESHOLD) {
        ashmemParcel = RSAshmemHelper::CreateAshmemParcel(data);
    }
    if (ashmemParcel != nullptr) {
        data = ashmemParcel;
    }
    return true;
}

bool RSRenderServiceConnectionProxy::GetUniRenderEnabled()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    return reply.ReadBool();
}

bool RSRenderServiceConnectionProxy::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteInterfaceToken err.");
        return false;
    }
    if (!data.WriteUint64(nodeId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteUint64 NodeId err.");
        return false;
    }
    if (!data.WriteUint64(displayNodeConfig.mirrorNodeId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteUint64 Config.MirrorNodeId err.");
        return false;
    }
    if (!data.WriteUint64(displayNodeConfig.screenId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteUint64 Config.ScreenId err.");
        return false;
    }
    if (!data.WriteBool(displayNodeConfig.isMirrored)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteBool Config.IsMirrored err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_DISPLAY_NODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }

    return reply.ReadBool();
}

bool RSRenderServiceConnectionProxy::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteUint64(config.id)) {
        ROSEN_LOGE("CreateNode: WriteUint64 Config.id err.");
        return false;
    }
    if (!data.WriteString(config.name)) {
        ROSEN_LOGE("CreateNode: WriteString Config.name err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }

    return reply.ReadBool();
}

sptr<Surface> RSRenderServiceConnectionProxy::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    bool unobscured)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteUint64(config.id)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteUint64 config.id err.");
        return nullptr;
    }
    if (!data.WriteString(config.name)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteString config.name err.");
        return nullptr;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(config.nodeType))) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteUint8 config.nodeType err.");
        return nullptr;
    }
    if (!data.WriteBool(config.isTextureExportNode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteBool config.isTextureExportNode err.");
        return nullptr;
    }
    if (!data.WriteBool(config.isSync)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteBool config.isSync err.");
        return nullptr;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(config.surfaceWindowType))) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteUint8 config.surfaceWindowType err.");
        return nullptr;
    }
    if (!data.WriteBool(unobscured)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteBool unobscured err.");
        return nullptr;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return nullptr;
    }
    sptr<IRemoteObject> surfaceObject = reply.ReadRemoteObject();
    sptr<IBufferProducer> bp = iface_cast<IBufferProducer>(surfaceObject);
    if (bp == nullptr) {
        return nullptr;
    }
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bp);
    return surface;
}

sptr<IVSyncConnection> RSRenderServiceConnectionProxy::CreateVSyncConnection(const std::string& name,
                                                                             const sptr<VSyncIConnectionToken>& token,
                                                                             uint64_t id,
                                                                             NodeId windowNodeId,
                                                                             bool fromXcomponent)
{
    if (token == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateVSyncConnection: token is nullptr.");
        return nullptr;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteString(name)) {
        ROSEN_LOGE("CreateVSyncConnection: WriteString name err.");
        return nullptr;
    }
    if (!data.WriteRemoteObject(token->AsObject())) {
        ROSEN_LOGE("CreateVSyncConnection: WriteRemoteObject token->AsObject() err.");
        return nullptr;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("CreateVSyncConnection: WriteUint64 id err.");
        return nullptr;
    }
    if (!data.WriteUint64(windowNodeId)) {
        ROSEN_LOGE("CreateVSyncConnection: WriteUint64 windowNodeId err.");
        return nullptr;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION);
    if (!Remote()) {
        return nullptr;
    }
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return nullptr;
    }

    sptr<IRemoteObject> rObj = reply.ReadRemoteObject();
    if (rObj == nullptr) {
        return nullptr;
    }
    sptr<IVSyncConnection> conn = iface_cast<IVSyncConnection>(rObj);
    return conn;
}

int32_t RSRenderServiceConnectionProxy::GetPixelMapByProcessId(
    std::vector<std::shared_ptr<Media::PixelMap>>& pixelMapVector, pid_t pid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteUint64(pid)) {
        ROSEN_LOGE("GetPixelMapByProcessId: WriteUint64 pid err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelMapByProcessId: Send Request err");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        pixelMapVector.clear();
        if (!RSMarshallingHelper::UnmarshallingVec(reply, pixelMapVector)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelMapByProcessId: Unmarshalling failed");
        }
    } else {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelMapByProcessId: Invalid reply");
    }
    return result;
}

std::shared_ptr<Media::PixelMap> RSRenderServiceConnectionProxy::CreatePixelMapFromSurface(sptr<Surface> surface,
    const Rect &srcRect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (surface == nullptr) {
        return nullptr;
    }

    auto producer = surface->GetProducer();
    if (producer == nullptr) {
        return nullptr;
    }

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteInterfaceToken RSIRenderServiceConnection::GetDescriptor() err.");
        return nullptr;
    }
    if (!data.WriteRemoteObject(producer->AsObject())) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteRemoteObject producer->AsObject() err.");
        return nullptr;
    }
    if (!data.WriteInt32(srcRect.x) || !data.WriteInt32(srcRect.y) ||
        !data.WriteInt32(srcRect.w) || !data.WriteInt32(srcRect.h)) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteInt32 srcRect err.");
        return nullptr;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreatePixelMapFromSurface: Send Request err.");
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    if (reply.ReadBool()) {
        pixelMap.reset(Media::PixelMap::Unmarshalling(reply));
    }
    return pixelMap;
}

int32_t RSRenderServiceConnectionProxy::SetFocusAppInfo(
    int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName, uint64_t focusNodeId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteInterfaceToken err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteInt32 pid err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt32(uid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteInt32 uid err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteString(bundleName)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteString bundleName err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteString(abilityName)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteString abilityName err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint64(focusNodeId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteUint64 focusNodeId err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

ScreenId RSRenderServiceConnectionProxy::GetDefaultScreenId()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetDefaultScreenId: WriteInterfaceToken RSIRenderServiceConnection::GetDescriptor() err.");
        return INVALID_SCREEN_ID;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return INVALID_SCREEN_ID;
    }

    ScreenId id = reply.ReadUint64();
    return id;
}

ScreenId RSRenderServiceConnectionProxy::GetActiveScreenId()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetActiveScreenId: WriteInterfaceToken RSIRenderServiceConnection::GetDescriptor() err.");
        return INVALID_SCREEN_ID;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return INVALID_SCREEN_ID;
    }

    ScreenId id = reply.ReadUint64();
    return id;
}

std::vector<ScreenId> RSRenderServiceConnectionProxy::GetAllScreenIds()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<ScreenId> screenIds;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetAllScreenIds: WriteInterfaceToken RSIRenderServiceConnection::GetDescriptor() err.");
        return std::vector<ScreenId>();
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return std::vector<ScreenId>();
    }

    uint32_t size = reply.ReadUint32();
    size_t readableSize = reply.GetReadableBytes() / sizeof(ScreenId);
    size_t len = static_cast<size_t>(size);
    if (len > readableSize || len > screenIds.max_size()) {
        RS_LOGE("RSRenderServiceConnectionProxy GetAllScreenIds Failed read vector, size:%{public}zu,"
            " readableSize:%{public}zu", len, readableSize);
        return screenIds;
    }
    for (uint32_t i = 0; i < size; i++) {
        screenIds.emplace_back(reply.ReadUint64());
    }

    return screenIds;
}

ScreenId RSRenderServiceConnectionProxy::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags,
    std::vector<NodeId> whiteList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("CreateVirtualScreen: WriteInterfaceToken RSIRenderServiceConnection::GetDescriptor() err.");
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
    if (!data.WriteUint64(mirrorId)) {
        ROSEN_LOGE("CreateVirtualScreen: WriteUint64 mirrorId err.");
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return INVALID_SCREEN_ID;
    }

    ScreenId id = reply.ReadUint64();
    return id;
}

int32_t RSRenderServiceConnectionProxy::SetVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenBlackList: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenBlackList: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUInt64Vector(blackListVector)) {
        ROSEN_LOGE("SetVirtualScreenBlackList: WriteUInt64Vector blackListVector err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenBlackList: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t status = reply.ReadInt32();
    return status;
}

int32_t RSRenderServiceConnectionProxy::AddVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("AddVirtualScreenBlackList: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("AddVirtualScreenBlackList: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUInt64Vector(blackListVector)) {
        ROSEN_LOGE("AddVirtualScreenBlackList: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::AddVirtualScreenBlackList: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t status = reply.ReadInt32();
    return status;
}

int32_t RSRenderServiceConnectionProxy::RemoveVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RemoveVirtualScreenBlackList: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RemoveVirtualScreenBlackList: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUInt64Vector(blackListVector)) {
        ROSEN_LOGE("RemoveVirtualScreenBlackList: WriteUInt64Vector blackListVector err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RemoveVirtualScreenBlackList: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t status = reply.ReadInt32();
    return status;
}

int32_t RSRenderServiceConnectionProxy::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<NodeId>& securityExemptionList)
{
    if (securityExemptionList.size() > MAX_SECURITY_EXEMPTION_LIST_NUMBER) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenSecurityExemptionList: too many lists.");
        return INVALID_ARGUMENTS;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenSecurityExemptionList: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t status = reply.ReadInt32();
    return status;
}

int32_t RSRenderServiceConnectionProxy::SetScreenSecurityMask(ScreenId id,
    std::shared_ptr<Media::PixelMap> securityMask)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SECURITY_MASK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetScreenSecurityMask: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    return SUCCESS;
}

int32_t RSRenderServiceConnectionProxy::SetMirrorScreenVisibleRect(
    ScreenId id, const Rect& mainScreenRect, bool supportRotation)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        RSIRenderServiceConnectionInterfaceCode::SET_MIRROR_SCREEN_VISIBLE_RECT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetMirrorScreenVisibleRect: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t status = reply.ReadInt32();
    return status;
}

int32_t RSRenderServiceConnectionProxy::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetCastScreenEnableSkipWindow: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    if (surface == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenSurface: Send surface is nullptr!");
        return INVALID_ARGUMENTS;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenSurface: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t status = reply.ReadInt32();
    return status;
}

void RSRenderServiceConnectionProxy::RemoveVirtualScreen(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RemoveVirtualScreen: WriteInterfaceToken GetDescriptor err.");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RemoveVirtualScreen: WriteUint64 id err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RemoveVirtualScreen: Send Request err.");
        return;
    }
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
int32_t RSRenderServiceConnectionProxy::SetPointerColorInversionConfig(float darkBuffer,
    float brightBuffer, int64_t interval, int32_t rangeSize)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetPointerColorInversionConfig: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteFloat(darkBuffer)) {
        ROSEN_LOGE("SetPointerColorInversionConfig: WriteFloat darkBuffer err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteFloat(brightBuffer)) {
        ROSEN_LOGE("SetPointerColorInversionConfig: WriteFloat brightBuffer err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt64(interval)) {
        ROSEN_LOGE("SetPointerColorInversionConfig: WriteInt64 interval err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt32(rangeSize)) {
        ROSEN_LOGE("SetPointerColorInversionConfig: WriteInt32 rangeSize err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_CONFIG);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetPointerColorInversionConfig: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}
 
int32_t RSRenderServiceConnectionProxy::SetPointerColorInversionEnabled(bool enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetPointerColorInversionEnabled: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteBool(enable)) {
        ROSEN_LOGE("SetPointerColorInversionEnabled: WriteBool enable err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::DisableCursorInvert: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}
 
int32_t RSRenderServiceConnectionProxy::RegisterPointerLuminanceChangeCallback(
    sptr<RSIPointerLuminanceChangeCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterPointerLuminanceChangeCallback: callback is nullptr.");
        return INVALID_ARGUMENTS;
    }
 
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
 
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterPointerLuminanceChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
 
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterPointerLuminanceChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_POINTER_LUMINANCE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterPointerLuminanceChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}
 
int32_t RSRenderServiceConnectionProxy::UnRegisterPointerLuminanceChangeCallback()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("UnRegisterPointerLuminanceChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::UNREGISTER_POINTER_LUMINANCE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::UnRegisterPointerLuminanceChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}
#endif

int32_t RSRenderServiceConnectionProxy::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetScreenChangeCallback: callback is nullptr.");
        return INVALID_ARGUMENTS;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("SetScreenChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetScreenChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

void RSRenderServiceConnectionProxy::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenActiveMode: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenActiveMode: WriteUint64 id err.");
        return;
    }
    if (!data.WriteUint32(modeId)) {
        ROSEN_LOGE("SetScreenActiveMode: WriteUint32 modeId err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

void RSRenderServiceConnectionProxy::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}

void RSRenderServiceConnectionProxy::SetRefreshRateMode(int32_t refreshRateMode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetRefreshRateMode: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(refreshRateMode)) {
        ROSEN_LOGE("SetRefreshRateMode: WriteInt32 refreshRateMode err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}

void RSRenderServiceConnectionProxy::SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range,
    int32_t animatorExpectedFrameRate)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}
void RSRenderServiceConnectionProxy::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("UnregisterFrameRateLinker: WriteInterfaceToken GetDescriptor err.");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("UnregisterFrameRateLinker: WriteUint64 id err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}

uint32_t RSRenderServiceConnectionProxy::GetScreenCurrentRefreshRate(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenCurrentRefreshRate: WriteInterfaceToken GetDescriptor err.");
        return SUCCESS;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenCurrentRefreshRate: WriteUint64 id err.");
        return SUCCESS;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return SUCCESS;
    }
    uint32_t rate = reply.ReadUint32();
    return rate;
}

int32_t RSRenderServiceConnectionProxy::GetCurrentRefreshRateMode()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceProxy failed to get descriptor");
        return SUCCESS;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return SUCCESS;
    }
    int32_t refreshRateMode = reply.ReadInt32();
    return refreshRateMode;
}

std::vector<int32_t> RSRenderServiceConnectionProxy::GetScreenSupportedRefreshRates(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<int32_t> screenSupportedRates;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedRefreshRates: WriteInterfaceToken GetDescriptor err.");
        return screenSupportedRates;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedRefreshRates: WriteUint64 id err.");
        return screenSupportedRates;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return screenSupportedRates;
    }
    uint64_t rateCount = reply.ReadUint64();
    size_t readableSize = reply.GetReadableBytes();
    size_t len = static_cast<size_t>(rateCount);
    if (len > readableSize || len > screenSupportedRates.max_size()) {
        RS_LOGE("RSRenderServiceConnectionProxy GetScreenSupportedRefreshRates "
            "fail read vector, size : %{public}zu, readableSize : %{public}zu", len, readableSize);
        return screenSupportedRates;
    }
    screenSupportedRates.resize(rateCount);
    for (uint64_t rateIndex = 0; rateIndex < rateCount; rateIndex++) {
        screenSupportedRates[rateIndex] = reply.ReadInt32();
    }
    return screenSupportedRates;
}

bool RSRenderServiceConnectionProxy::GetShowRefreshRateEnabled()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceProxy failed to get descriptor");
        return SUCCESS;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return SUCCESS;
    }
    bool enable = reply.ReadBool();
    return enable;
}

void RSRenderServiceConnectionProxy::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetShowRefreshRateEnabled: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteBool(enabled) || !data.WriteInt32(type)) {
        ROSEN_LOGE("SetShowRefreshRateEnabled: WriteBool[enable] OR WriteInt32[type] err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return;
    }
}

uint32_t RSRenderServiceConnectionProxy::GetRealtimeRefreshRate(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetRealtimeRefreshRate: WriteInterfaceToken GetDescriptor err.");
        return SUCCESS;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetRealtimeRefreshRate: WriteUint64 id err.");
        return SUCCESS;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return SUCCESS;
    }
    uint32_t rate = reply.ReadUint32();
    return rate;
}

std::string RSRenderServiceConnectionProxy::GetRefreshInfo(pid_t pid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetRefreshInfo: WriteInterfaceToken GetDescriptor err.");
        return "";
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("GetRefreshInfo: WriteInt32 pid err.");
        return "";
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return "";
    }
    std::string enable = reply.ReadString();
    return enable;
}

int32_t RSRenderServiceConnectionProxy::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
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
    auto code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PHYSICAL_SCREEN_RESOLUTION);
    if (SendRequest(code, data, reply, option) != ERR_NONE) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetPhysicalScreenResolution: SendRequest error.");
        return RS_CONNECTION_ERROR;
    }
    int32_t status = reply.ReadInt32();
    return status;
}

int32_t RSRenderServiceConnectionProxy::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenResolution: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t status = reply.ReadInt32();
    return status;
}

void RSRenderServiceConnectionProxy::MarkPowerOffNeedProcessOneFrame()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::MarkPowerOffNeedProcessOneFrame: Send Request err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

void RSRenderServiceConnectionProxy::RepaintEverything()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RepaintEverything: Send Request err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPAINT_EVERYTHING);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

void RSRenderServiceConnectionProxy::ForceRefreshOneFrameWithNextVSync()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ForceRefreshOneFrameWithNextVSync: Send Request err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

void RSRenderServiceConnectionProxy::DisablePowerOffRenderControl(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("DisablePowerOffRenderControl: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("DisablePowerOffRenderControl: WriteUint64 id err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

void RSRenderServiceConnectionProxy::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

void RSRenderServiceConnectionProxy::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    if (app == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr", __func__);
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteRemoteObject(app->AsObject())) {
        ROSEN_LOGE("%{public}s WriteRemoteObject failed", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}

void RSRenderServiceConnectionProxy::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect, RSSurfaceCapturePermissions /* permissions */)
{
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr", __func__);
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("%{public}s write callback failed", __func__);
        return;
    }
    if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
        ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
        return;
    }
    if (!WriteSurfaceCaptureBlurParam(blurParam, data)) {
        ROSEN_LOGE("%{public}s write blurParam failed", __func__);
        return;
    }
    if (!WriteSurfaceCaptureAreaRect(specifiedAreaRect, data)) {
        ROSEN_LOGE("%{public}s write specifiedAreaRect failed", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}

void RSRenderServiceConnectionProxy::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return;
    }
    if (!data.WriteBool(isFreeze)) {
        ROSEN_LOGE("%{public}s write isFreeze failed", __func__);
        return;
    }
    if (isFreeze) {
        if (callback == nullptr) {
            ROSEN_LOGE("%{public}s callback == nullptr", __func__);
            return;
        }
        if (!data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("%{public}s write callback failed", __func__);
            return;
        }
        if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
            ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
            return;
        }
        if (!WriteSurfaceCaptureBlurParam(blurParam, data)) {
            ROSEN_LOGE("%{public}s write blurParam failed", __func__);
            return;
        }
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}

bool RSRenderServiceConnectionProxy::WriteSurfaceCaptureConfig(
    const RSSurfaceCaptureConfig& captureConfig, MessageParcel& data)
{
    if (!data.WriteFloat(captureConfig.scaleX) || !data.WriteFloat(captureConfig.scaleY) ||
        !data.WriteBool(captureConfig.useDma) || !data.WriteBool(captureConfig.useCurWindow) ||
        !data.WriteUint8(static_cast<uint8_t>(captureConfig.captureType)) || !data.WriteBool(captureConfig.isSync) ||
        !data.WriteFloat(captureConfig.mainScreenRect.left_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.top_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.right_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.bottom_)) {
        ROSEN_LOGE("WriteSurfaceCaptureConfig: WriteSurfaceCaptureConfig captureConfig err.");
        return false;
    }
    return true;
}

bool RSRenderServiceConnectionProxy::WriteSurfaceCaptureBlurParam(
    const RSSurfaceCaptureBlurParam& blurParam, MessageParcel& data)
{
    if (!data.WriteBool(blurParam.isNeedBlur) || !data.WriteFloat(blurParam.blurRadius)) {
        ROSEN_LOGE("WriteSurfaceCaptureBlurParam: WriteBool OR WriteFloat [blurParam] err.");
        return false;
    }
    return true;
}

bool RSRenderServiceConnectionProxy::WriteSurfaceCaptureAreaRect(
    const Drawing::Rect& specifiedAreaRect, MessageParcel& data)
{
    if (!data.WriteFloat(specifiedAreaRect.left_) || !data.WriteFloat(specifiedAreaRect.top_) ||
        !data.WriteFloat(specifiedAreaRect.right_) || !data.WriteFloat(specifiedAreaRect.bottom_)) {
        ROSEN_LOGE("WriteSurfaceCaptureAreaRect: WriteFloat specifiedAreaRect err.");
        return false;
    }
    return true;
}

void RSRenderServiceConnectionProxy::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(rsNodeId)) {
        ROSEN_LOGE("SetHwcNodeBounds write id failed");
        return;
    }
    if (!data.WriteFloat(positionX) || !data.WriteFloat(positionY) || !data.WriteFloat(positionZ) ||
        !data.WriteFloat(positionW)) {
        ROSEN_LOGE("SetHwcNodeBounds write bound failed");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_POSITION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SetHwcNodeBounds SendRequest() error[%{public}d]", err);
        return;
    }
}

RSVirtualScreenResolution RSRenderServiceConnectionProxy::GetVirtualScreenResolution(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSVirtualScreenResolution virtualScreenResolution;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetVirtualScreenResolution: WriteInterfaceToken GetDescriptor err.");
        return virtualScreenResolution;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetVirtualScreenResolution: WriteUint64 id err.");
        return virtualScreenResolution;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION);
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

RSScreenModeInfo RSRenderServiceConnectionProxy::GetScreenActiveMode(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSScreenModeInfo screenModeInfo;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenActiveMode: WriteInterfaceToken GetDescriptor err.");
        return screenModeInfo;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenActiveMode: WriteUint64 id err.");
        return screenModeInfo;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
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

std::vector<RSScreenModeInfo> RSRenderServiceConnectionProxy::GetScreenSupportedModes(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<RSScreenModeInfo> screenSupportedModes;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedModes: WriteInterfaceToken GetDescriptor err.");
        return screenSupportedModes;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedModes: WriteUint64 id err.");
        return screenSupportedModes;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return screenSupportedModes;
    }

    uint64_t modeCount = reply.ReadUint64();
    size_t readableSize = reply.GetReadableBytes();
    size_t len = static_cast<size_t>(modeCount);
    if (len > readableSize || len > screenSupportedModes.max_size()) {
        RS_LOGE("RSRenderServiceConnectionProxy GetScreenSupportedModes Fail read vector, size:%{public}zu,"
            "readableSize:%{public}zu", len, readableSize);
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

std::vector<MemoryGraphic> RSRenderServiceConnectionProxy::GetMemoryGraphics()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<MemoryGraphic> memoryGraphics;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetMemoryGraphics: WriteInterfaceToken GetDescriptor err.");
        return memoryGraphics;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return memoryGraphics;
    }

    uint64_t count = reply.ReadUint64();
    size_t readableSize = reply.GetReadableBytes();
    size_t len = static_cast<size_t>(count);
    if (len > readableSize || len > memoryGraphics.max_size()) {
        RS_LOGE("RSRenderServiceConnectionProxy GetMemoryGraphics Failed to read vector, size:%{public}zu,"
            " readableSize:%{public}zu", len, readableSize);
        return memoryGraphics;
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
    return memoryGraphics;
}

bool RSRenderServiceConnectionProxy::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    MemoryGraphic memoryGraphic;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetTotalAppMemSize: WriteInterfaceToken GetDescriptor err.");
        return false;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }

    cpuMemSize = reply.ReadFloat();
    gpuMemSize = reply.ReadFloat();
    return true;
}

MemoryGraphic RSRenderServiceConnectionProxy::GetMemoryGraphic(int pid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    MemoryGraphic memoryGraphic;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetMemoryGraphic: WriteInterfaceToken GetDescriptor err.");
        return memoryGraphic;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("GetMemoryGraphic: WriteInt32 pid err.");
        return memoryGraphic;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return memoryGraphic;
    }
    sptr<MemoryGraphic> pMemoryGraphic(reply.ReadParcelable<MemoryGraphic>());
    if (pMemoryGraphic == nullptr) {
        return memoryGraphic;
    }
    memoryGraphic = *pMemoryGraphic;
    return memoryGraphic;
}

RSScreenCapability RSRenderServiceConnectionProxy::GetScreenCapability(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSScreenCapability screenCapability;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenCapability: WriteInterfaceToken GetDescriptor err.");
        return screenCapability;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenCapability: WriteUint64 id err.");
        return screenCapability;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY);
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

ScreenPowerStatus RSRenderServiceConnectionProxy::GetScreenPowerStatus(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenPowerStatus: WriteInterfaceToken GetDescriptor err.");
        return INVALID_POWER_STATUS;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenPowerStatus: WriteUint64 id err.");
        return INVALID_POWER_STATUS;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return INVALID_POWER_STATUS;
    }
    return static_cast<ScreenPowerStatus>(reply.ReadUint32());
}

RSScreenData RSRenderServiceConnectionProxy::GetScreenData(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    RSScreenData screenData;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenData: WriteInterfaceToken GetDescriptor err.");
        return screenData;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenData: WriteUint64 id err.");
        return screenData;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA);
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

int32_t RSRenderServiceConnectionProxy::GetScreenBacklight(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenBacklight: WriteInterfaceToken GetDescriptor err.");
        return INVALID_BACKLIGHT_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenBacklight: WriteUint64 id err.");
        return INVALID_BACKLIGHT_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return INVALID_BACKLIGHT_VALUE;
    }
    int32_t level = reply.ReadInt32();
    return level;
}

void RSRenderServiceConnectionProxy::SetScreenBacklight(ScreenId id, uint32_t level)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}

void RSRenderServiceConnectionProxy::RegisterBufferClearListener(
    NodeId id, sptr<RSIBufferClearCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterBufferClearListener: callback is nullptr.");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterBufferClearListener: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RegisterBufferClearListener: WriteUint64 id err.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterBufferClearListener: WriteRemoteObject callback->AsObject() err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterBufferClearListener: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterBufferAvailableListener: callback is nullptr.");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterBufferAvailableListener: WriteInterfaceToken GetDescriptor err.");
        return;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RegisterBufferAvailableListener: WriteUint64 id err.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterBufferAvailableListener: WriteRemoteObject callback->AsObject() err.");
        return;
    }
    if (!data.WriteBool(isFromRenderThread)) {
        ROSEN_LOGE("RegisterBufferAvailableListener: WriteBool isFromRenderThread err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterBufferAvailableListener: Send Request err.");
        return;
    }
}

int32_t RSRenderServiceConnectionProxy::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedColorGamuts: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedColorGamuts: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
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

int32_t RSRenderServiceConnectionProxy::GetScreenSupportedMetaDataKeys(
    ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedMetaDataKeys: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedMetaDataKeys: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
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

int32_t RSRenderServiceConnectionProxy::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenColorGamut: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenColorGamut: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        mode = static_cast<ScreenColorGamut>(reply.ReadUint32());
    }
    return result;
}

int32_t RSRenderServiceConnectionProxy::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenGamutMap: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenGamutMap: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        mode = static_cast<ScreenGamutMap>(reply.ReadUint32());
    }
    return result;
}

int32_t RSRenderServiceConnectionProxy::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenHDRCapability: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenHDRCapability: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
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

int32_t RSRenderServiceConnectionProxy::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetPixelFormat: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetPixelFormat: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        pixelFormat = static_cast<GraphicPixelFormat>(reply.ReadUint32());
    }
    return result;
}

int32_t RSRenderServiceConnectionProxy::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetPixelFormat: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetPixelFormat: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(pixelFormat))) {
        ROSEN_LOGE("GetPixelFormat: WriteUint32 pixelFormat err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::GetScreenSupportedHDRFormats(
    ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedHDRFormats: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedHDRFormats: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        hdrFormats.clear();
        std::vector<uint32_t> hdrFormatsRecv;
        reply.ReadUInt32Vector(&hdrFormatsRecv);
        std::transform(hdrFormatsRecv.begin(), hdrFormatsRecv.end(), back_inserter(hdrFormats),
                       [](uint32_t i) -> ScreenHDRFormat {return static_cast<ScreenHDRFormat>(i);});
    }
    return result;
}

int32_t RSRenderServiceConnectionProxy::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenHDRFormat: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenHDRFormat: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        hdrFormat = static_cast<ScreenHDRFormat>(reply.ReadUint32());
    }
    return result;
}

int32_t RSRenderServiceConnectionProxy::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenHDRFormat: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenHDRFormat: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt32(modeIdx)) {
        ROSEN_LOGE("SetScreenHDRFormat: WriteInt32 modeIdx err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedColorSpaces: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedColorSpaces: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        colorSpaces.clear();
        std::vector<uint32_t> colorSpacesRecv;
        reply.ReadUInt32Vector(&colorSpacesRecv);
        std::transform(colorSpacesRecv.begin(), colorSpacesRecv.end(), back_inserter(colorSpaces),
                       [](uint32_t i) -> GraphicCM_ColorSpaceType {return static_cast<GraphicCM_ColorSpaceType>(i);});
    }
    return result;
}

int32_t RSRenderServiceConnectionProxy::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenColorSpace: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenColorSpace: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        colorSpace = static_cast<GraphicCM_ColorSpaceType>(reply.ReadUint32());
    }
    return result;
}

int32_t RSRenderServiceConnectionProxy::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenColorSpace: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenColorSpace: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt32(colorSpace)) {
        ROSEN_LOGE("SetScreenColorSpace: WriteInt32 colorSpace err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenType: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenType: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        screenType = static_cast<RSScreenType>(reply.ReadUint32());
    }
    return result;
}

bool RSRenderServiceConnectionProxy::GetBitmap(NodeId id, Drawing::Bitmap& bitmap)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetBitmap: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetBitmap: WriteUint64 id err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    bool result = reply.ReadBool();
    if (!result || !RSMarshallingHelper::Unmarshalling(reply, bitmap)) {
        RS_LOGE("RSRenderServiceConnectionProxy::GetBitmap: Unmarshalling failed");
        return false;
    }
    return true;
}

bool RSRenderServiceConnectionProxy::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    bool result = reply.ReadBool();
    return result;
}

bool RSRenderServiceConnectionProxy::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    bool result = reply.ReadBool();
    return result;
}

bool RSRenderServiceConnectionProxy::SetGlobalDarkColorMode(bool isDark)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetGlobalDarkColorMode: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteBool(isDark)) {
        ROSEN_LOGE("SetGlobalDarkColorMode: WriteBool isDark err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    return true;
}

bool RSRenderServiceConnectionProxy::GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetPixelmap: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetPixelmap: WriteUint64 id err.");
        return false;
    }
    if (!data.WriteParcelable(pixelmap.get())) {
        ROSEN_LOGE("GetPixelmap: WriteParcelable pixelmap.get() err.");
        return false;
    }
    RSMarshallingHelper::Marshalling(data, *rect);
    RSMarshallingHelper::Marshalling(data, drawCmdList);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    bool result = reply.ReadBool();
    if (!result || !RSMarshallingHelper::Unmarshalling(reply, pixelmap)) {
        RS_LOGD("RSRenderServiceConnectionProxy::GetPixelmap: GetPixelmap failed");
        return false;
    }
    return true;
}

bool RSRenderServiceConnectionProxy::RegisterTypeface(uint64_t globalUniqueId,
    std::shared_ptr<Drawing::Typeface>& typeface)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterTypeface: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t hash = typeface->GetHash();
    if (!data.WriteUint64(globalUniqueId)) {
        ROSEN_LOGE("RegisterTypeface: WriteUint64 globalUniqueId err.");
        return false;
    }
    if (!data.WriteUint32(hash)) {
        ROSEN_LOGE("RegisterTypeface: WriteUint32 hash err.");
        return false;
    }

    if (hash) { // if adapter does not provide hash, use old path
        MessageParcel reply2;
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NEED_REGISTER_TYPEFACE);
        int32_t err = SendRequest(code, data, reply2, option);
        if (err != NO_ERROR) {
            RS_LOGW("Check if RegisterTypeface is needed failed, err:%{public}d", err);
            return false;
        }
        if (!reply2.ReadBool()) {
            return true; // the hash exists on server, no need to resend full data
        }
    }

    RSMarshallingHelper::Marshalling(data, typeface);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGD("RSRenderServiceConnectionProxy::RegisterTypeface: RegisterTypeface failed");
        return false;
    }
    bool result = reply.ReadBool();
    return result;
}

bool RSRenderServiceConnectionProxy::UnRegisterTypeface(uint64_t globalUniqueId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("UnRegisterTypeface: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(globalUniqueId)) {
        ROSEN_LOGE("UnRegisterTypeface: WriteUint64 globalUniqueId err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGD("RSRenderServiceConnectionProxy::UnRegisterTypeface: send request failed");
        return false;
    }

    return true;
}

int32_t RSRenderServiceConnectionProxy::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenSkipFrameInterval: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenSkipFrameInterval: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(skipFrameInterval)) {
        ROSEN_LOGE("SetScreenSkipFrameInterval: WriteUint32 skipFrameInterval err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::SetVirtualScreenRefreshRate(
    ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenRefreshRate: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenRefreshRate: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(maxRefreshRate)) {
        ROSEN_LOGE("SetVirtualScreenRefreshRate: WriteUint32 maxRefreshRate err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    if (result == SUCCESS) {
        actualRefreshRate = reply.ReadUint32();
    }
    return result;
}

uint32_t RSRenderServiceConnectionProxy::SetScreenActiveRect(
    ScreenId id, const Rect& activeRect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetScreenActiveRect: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetScreenActiveRect: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt32(activeRect.x) || !data.WriteInt32(activeRect.y) ||
        !data.WriteInt32(activeRect.w) || !data.WriteInt32(activeRect.h)) {
        ROSEN_LOGE("SetScreenActiveRect: WriteInt32 activeRect err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    return reply.ReadUint32();
}

int32_t RSRenderServiceConnectionProxy::RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterOcclusionChangeCallback: callback is nullptr.");
        return INVALID_ARGUMENTS;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterOcclusionChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterOcclusionChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    } else {
        return SUCCESS;
    }
}

int32_t RSRenderServiceConnectionProxy::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceOcclusionChangeCallback: callback is nullptr.");
        return INVALID_ARGUMENTS;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterSurfaceOcclusionChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RegisterSurfaceOcclusionChangeCallback: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterSurfaceOcclusionChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteFloatVector(partitionPoints)) {
        ROSEN_LOGE("RegisterSurfaceOcclusionChangeCallback: WriteFloatVector partitionPoints err.");
        return WRITE_PARCEL_ERR;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("UnRegisterSurfaceOcclusionChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("UnRegisterSurfaceOcclusionChangeCallback: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::RegisterHgmConfigChangeCallback(sptr<RSIHgmConfigChangeCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterHgmConfigChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterHgmConfigChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterHgmConfigChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterHgmRefreshRateModeChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterHgmRefreshRateModeChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::RegisterHgmRefreshRateUpdateCallback(
    sptr<RSIHgmConfigChangeCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSRenderServiceConnectionProxy::RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid,
    sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterFrameRateLinkerExpectedFpsUpdateCallback: "
            "Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

void RSRenderServiceConnectionProxy::SetAppWindowNum(uint32_t num)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetAppWindowNum: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint32(num)) {
        ROSEN_LOGE("SetAppWindowNum: WriteUint32 num err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetAppWindowNum: Send Request err.");
        return;
    }
}

bool RSRenderServiceConnectionProxy::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetSystemAnimatedScenes: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint32(static_cast<uint32_t>(systemAnimatedScenes))) {
        ROSEN_LOGE("SetSystemAnimatedScenes: WriteUint32 systemAnimatedScenes err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    bool result = reply.ReadBool();
    return result;
}

bool RSRenderServiceConnectionProxy::SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetWatermark: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteString(name)) {
        ROSEN_LOGE("SetWatermark: WriteString name err.");
        return false;
    }
    if (!data.WriteParcelable(watermark.get())) {
        ROSEN_LOGE("SetWatermark: WriteParcelable watermark.get() err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WATERMARK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWatermark: Send Request err.");
        return false;
    }
    return true;
}

void RSRenderServiceConnectionProxy::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    if (watermarkImg == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ShowWatermark: watermarkImg is nullptr.");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ShowWatermark: Send Request err.");
        return;
    }
}

int32_t RSRenderServiceConnectionProxy::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ResizeVirtualScreen: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t status = reply.ReadInt32();
    return status;
}

void RSRenderServiceConnectionProxy::ReportJankStats()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportJankStats: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportJankStats: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::ReportEventResponse(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventResponse: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportEventResponse: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::ReportEventComplete(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventComplete: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportEventComplete: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::ReportEventJankFrame(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventJankFrame: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportEventJankFrame: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::ReportRsSceneJankStart(AppInfo info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportRsSceneJankStart: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    WriteAppInfo(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportRsSceneJankStart: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::ReportRsSceneJankEnd(AppInfo info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportRsSceneJankEnd: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    WriteAppInfo(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportRsSceneJankEnd: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::ReportDataBaseRs(
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

void RSRenderServiceConnectionProxy::WriteAppInfo(
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

void RSRenderServiceConnectionProxy::ReportGameStateDataRs(
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

void RSRenderServiceConnectionProxy::ReportGameStateData(GameStateData info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportGameStateData: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    ReportGameStateDataRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportGameStateData: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetHardwareEnabled: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetHardwareEnabled: WriteUint64 id err.");
        return;
    }
    if (!data.WriteBool(isEnabled)) {
        ROSEN_LOGE("SetHardwareEnabled: WriteBool isEnabled err.");
        return;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(selfDrawingType))) {
        ROSEN_LOGE("SetHardwareEnabled: WriteUint8 selfDrawingType err.");
        return;
    }
    if (!data.WriteBool(dynamicHardwareEnable)) {
        ROSEN_LOGE("SetHardwareEnabled: WriteBool dynamicHardwareEnable err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetHardwareEnabled: Send Request err.");
        return;
    }
}

uint32_t RSRenderServiceConnectionProxy::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetHidePrivacyContent: WriteInterfaceToken GetDescriptor err.");
        return static_cast<uint32_t>(RSInterfaceErrorCode::WRITE_PARCEL_ERROR);
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetHidePrivacyContent: WriteUint64 id err.");
        return static_cast<uint32_t>(RSInterfaceErrorCode::WRITE_PARCEL_ERROR);
    }
    if (!data.WriteBool(needHidePrivacyContent)) {
        ROSEN_LOGE("SetHidePrivacyContent: WriteBool needHidePrivacyContent err.");
        return static_cast<uint32_t>(RSInterfaceErrorCode::WRITE_PARCEL_ERROR);
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetHidePrivacyContent: Send Request err.");
        return static_cast<uint32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
    }
    return reply.ReadUint32();
}

void RSRenderServiceConnectionProxy::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyLightFactorStatus: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteInt32(lightFactorStatus)) {
        ROSEN_LOGE("NotifyLightFactorStatus: WriteInt32 lightFactorStatus err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyLightFactorStatus: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyPackageEvent: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
    const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }

    if (listSize != newConfig.size()) {
        ROSEN_LOGE("input size doesn't match");
        return;
    }

    if (!data.WriteString(pkgName) || !data.WriteUint32(listSize)) {
        return;
    }

    for (const auto& [key, value] : newConfig) {
        if (!data.WriteString(key) || !data.WriteString(value)) {
            return;
        }
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyAppStrategyConfigChangeEvent: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyRefreshRateEvent: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::NotifySoftVsyncEvent(uint32_t pid, uint32_t rateDiscount)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifySoftVsyncEvent: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteUint32(pid)) {
        ROSEN_LOGE("NotifySoftVsyncEvent: WriteUint32 pid err.");
        return;
    }
    if (!data.WriteUint32(rateDiscount)) {
        ROSEN_LOGE("NotifySoftVsyncEvent: WriteUint32 rateDiscount err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifySoftVsyncEvent: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyTouchEvent: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteInt32(touchStatus)) {
        ROSEN_LOGE("NotifyTouchEvent: WriteInt32 touchStatus err.");
        return;
    }
    if (!data.WriteInt32(touchCnt)) {
        ROSEN_LOGE("NotifyTouchEvent: WriteInt32 touchCnt err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyTouchEvent: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::NotifyDynamicModeEvent(bool enableDynamicMode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyDynamicModeEvent: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(enableDynamicMode)) {
        ROSEN_LOGE("NotifyDynamicModeEvent: WriteBool enableDynamicMode err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_DYNAMIC_MODE_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyDynamicModeEvent: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::SetCacheEnabledForRotation(bool isEnabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetCacheEnabledForRotation: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(isEnabled)) {
        ROSEN_LOGE("SetCacheEnabledForRotation: WriteBool isEnabled err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetCacheEnabledForRotation: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback)
{
    OnRemoteDiedCallback_ = callback;
}

void RSRenderServiceConnectionProxy::RunOnRemoteDiedCallback()
{
    if (OnRemoteDiedCallback_) {
        OnRemoteDiedCallback_();
    }
}

std::vector<ActiveDirtyRegionInfo> RSRenderServiceConnectionProxy::GetActiveDirtyRegionInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<ActiveDirtyRegionInfo> activeDirtyRegionInfos;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetActiveDirtyRegionInfo: WriteInterfaceToken GetDescriptor err.");
        return activeDirtyRegionInfos;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetActiveDirtyRegionInfo: Send Request err.");
        return activeDirtyRegionInfos;
    }
    int32_t activeDirtyRegionInfosSize = reply.ReadInt32();
    while (activeDirtyRegionInfosSize--) {
        activeDirtyRegionInfos.emplace_back(ActiveDirtyRegionInfo(reply.ReadInt64(), reply.ReadInt32(),
            reply.ReadInt32(), reply.ReadString()));
    }
    return activeDirtyRegionInfos;
}

GlobalDirtyRegionInfo RSRenderServiceConnectionProxy::GetGlobalDirtyRegionInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    GlobalDirtyRegionInfo globalDirtyRegionInfo;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetGlobalDirtyRegionInfo: WriteInterfaceToken GetDescriptor err.");
        return globalDirtyRegionInfo;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetGlobalDirtyRegionInfo: Send Request err.");
        return globalDirtyRegionInfo;
    }
    return GlobalDirtyRegionInfo(reply.ReadInt64(), reply.ReadInt32(), reply.ReadInt32(), reply.ReadInt32());
}

LayerComposeInfo RSRenderServiceConnectionProxy::GetLayerComposeInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    LayerComposeInfo layerComposeInfo;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetLayerComposeInfo: WriteInterfaceToken GetDescriptor err.");
        return layerComposeInfo;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetLayerComposeInfo: Send Request err.");
        return layerComposeInfo;
    }
    return LayerComposeInfo(reply.ReadInt32(), reply.ReadInt32(), reply.ReadInt32());
}

HwcDisabledReasonInfos RSRenderServiceConnectionProxy::GetHwcDisabledReasonInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    HwcDisabledReasonInfos hwcDisabledReasonInfos;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetHwcDisabledReasonInfo: WriteInterfaceToken GetDescriptor err.");
        return hwcDisabledReasonInfos;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::
        GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetHwcDisabledReasonInfo: Send Request err.");
        return hwcDisabledReasonInfos;
    }
    int32_t size = reply.ReadInt32();
    size_t readableSize = reply.GetReadableBytes() / HWC_DISABLED_REASON_INFO_MINIMUM_SIZE;
    size_t len = static_cast<size_t>(size);
    if (len > readableSize || len > hwcDisabledReasonInfos.max_size()) {
        RS_LOGE("RSRenderServiceConnectionProxy GetHwcDisabledReasonInfo Failed read vector, size:%{public}zu,"
            " readableSize:%{public}zu", len, readableSize);
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

int64_t RSRenderServiceConnectionProxy::GetHdrOnDuration()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetHdrOnDuration: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HDR_ON_DURATION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int64_t result = reply.ReadInt64();
    return result;
}

void RSRenderServiceConnectionProxy::SetVmaCacheStatus(bool flag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVmaCacheStatus: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(flag)) {
        ROSEN_LOGE("SetVmaCacheStatus: WriteBool flag err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVmaCacheStatus %d: Send Request err.", flag);
        return;
    }
}

#ifdef TP_FEATURE_ENABLE
void RSRenderServiceConnectionProxy::SetTpFeatureConfig(int32_t feature, const char* config,
    TpFeatureConfigType tpFeatureConfigType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetTpFeatureConfig: WriteInterfaceToken GetDescriptor err.");
        return;
    }

    if (!data.WriteInt32(feature)) {
        ROSEN_LOGE("SetTpFeatureConfig: WriteInt32 feature err.");
        return;
    }

    if (!data.WriteCString(config)) {
        ROSEN_LOGE("SetTpFeatureConfig: WriteCString config err.");
        return;
    }

    if (!data.WriteUint8(static_cast<uint8_t>(tpFeatureConfigType))) {
        ROSEN_LOGE("SetTpFeatureConfig: WriteUint8 tpFeatureConfigType err.");
        return;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return;
    }
}
#endif

void RSRenderServiceConnectionProxy::SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenUsingStatus: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(isVirtualScreenUsingStatus)) {
        ROSEN_LOGE("SetVirtualScreenUsingStatus: WriteBool isVirtualScreenUsingStatus err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenUsingStatus: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetCurtainScreenUsingStatus: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(isCurtainScreenOn)) {
        ROSEN_LOGE("SetCurtainScreenUsingStatus: WriteBool isCurtainScreenOn err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetCurtainScreenUsingStatus: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::DropFrameByPid(const std::vector<int32_t> pidList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("DropFrameByPid: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteInt32Vector(pidList)) {
        ROSEN_LOGE("DropFrameByPid: WriteInt32Vector pidList err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::DropFrameByPid: Send Request err.");
        return;
    }
}

int32_t RSRenderServiceConnectionProxy::RegisterUIExtensionCallback(
    uint64_t userId, sptr<RSIUIExtensionCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterUIExtensionCallback: callback is nullptr.");
        return INVALID_ARGUMENTS;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterUIExtensionCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (data.WriteUint64(userId) && data.WriteRemoteObject(callback->AsObject())) {
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            return RS_CONNECTION_ERROR;
        }
        int32_t result = reply.ReadInt32();
        return result;
    } else {
        ROSEN_LOGE("RegisterUIExtensionCallback: WriteUint64[userId] OR WriteRemoteObject[callback] err.");
        return RS_CONNECTION_ERROR;
    }
}

bool RSRenderServiceConnectionProxy::SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVirtualScreenStatus: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetVirtualScreenStatus: WriteUint64 id err.");
        return false;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(screenStatus))) {
        ROSEN_LOGE("SetVirtualScreenStatus: WriteUint8 screenStatus err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return false;
    }
    bool result = reply.ReadBool();
    return result;
}

bool RSRenderServiceConnectionProxy::SetAncoForceDoDirect(bool direct)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetAncoForceDoDirect: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (data.WriteBool(direct)) {
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            return RS_CONNECTION_ERROR;
        }
        bool result = reply.ReadBool();
        return result;
    } else {
        ROSEN_LOGE("SetAncoForceDoDirect: WriteBool direct err.");
        return RS_CONNECTION_ERROR;
    }
}

void RSRenderServiceConnectionProxy::SetFreeMultiWindowStatus(bool enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFreeMultiWindowStatus: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteBool(enable)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFreeMultiWindowStatus: write bool val err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFreeMultiWindowStatus: Send Request err.");
    }
}

void RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback(
    pid_t pid, uint64_t uid, sptr<RSISurfaceBufferCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback callback == nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: write Int32 val err.");
        return;
    }
    if (!data.WriteUint64(uid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: write Uint64 val err.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: write RemoteObject val err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback: write Int32 val err.");
        return;
    }
    if (!data.WriteUint64(uid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback: write Uint64 val err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetLayerTop: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteString(nodeIdStr) && data.WriteBool(isTop)) {
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::SetLayerTop: Send Request err.");
            return;
        }
    }
}

void RSRenderServiceConnectionProxy::SetWindowContainer(NodeId nodeId, bool value)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowContainer: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(nodeId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowContainer: write Uint64 val err.");
        return;
    }
    if (!data.WriteBool(value)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowContainer: write Bool val err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_CONTAINER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowContainer: Send Request err.");
        return;
    }
}

int32_t RSRenderServiceConnectionProxy::SendRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    if (!Remote()) {
        return static_cast<int32_t>(RSInterfaceErrorCode::NULLPTR_ERROR);
    }
    return Remote()->SendRequest(code, data, reply, option);
}

void RSRenderServiceConnectionProxy::NotifyScreenSwitched()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: Write InterfaceToken val err.", __func__);
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request error.", __func__);
        return;
    }
}
} // namespace Rosen
} // namespace OHOS
