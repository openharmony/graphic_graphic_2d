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
static constexpr uint32_t EDID_DATA_MAX_SIZE = 64 * 1024;
static constexpr int MAX_VOTER_SIZE = 100; // SetWindowExpectedRefreshRate map size not exceed 100
static constexpr int ZERO = 0; // empty map size
}

RSRenderServiceConnectionProxy::RSRenderServiceConnectionProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIRenderServiceConnection>(impl)
{
}

ErrCode RSRenderServiceConnectionProxy::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    if (!transactionData) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CommitTransaction transactionData nullptr!");
        return ERR_INVALID_VALUE;
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
                return ERR_INVALID_VALUE;
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
                return ERR_INVALID_VALUE;
            }
        } while (err != NO_ERROR);
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr) {
        return ERR_INVALID_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSRenderServiceConnectionProxy::GetDescriptor())) {
        ROSEN_LOGE("ExecuteSynchronousTask WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!task->Marshalling(data)) {
        ROSEN_LOGE("ExecuteSynchronousTask Marshalling failed");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }

    if (task->CheckHeader(reply)) {
        task->ReadFromParcel(reply);
    }
    return ERR_OK;
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

    if (!RSMarshallingHelper::MarshallingTransactionVer(*data)) {
        ROSEN_LOGE("FillParcelWithTransactionData WriteVersionHeader failed!");
        return false;
    }

    {
        // 1. marshalling RSTransactionData
#ifdef RS_ENABLE_VK
        RS_TRACE_NAME_FMT("MarshRSTransactionData cmdCount: %lu, transactionFlag:[%d,%" PRIu64 "], tid:%d, "
            "timestamp:%ld", transactionData->GetCommandCount(), pid_, transactionData->GetIndex(),
            transactionData->GetSendingTid(), transactionData->GetTimestamp());
#else
        RS_TRACE_NAME_FMT("MarshRSTransactionData cmdCount: %lu, transactionFlag:[%d,%" PRIu64 "], timestamp:%ld",
            transactionData->GetCommandCount(), pid_, transactionData->GetIndex(), transactionData->GetTimestamp());
#endif
        ROSEN_LOGI_IF(DEBUG_PIPELINE,
            "MarshRSTransactionData cmdCount:%{public}lu transactionFlag:[pid:%{public}d index:%{public}" PRIu64 "]",
            transactionData->GetCommandCount(), pid_, transactionData->GetIndex());
        bool success = data->WriteParcelable(transactionData.get());
        if (!success) {
            ROSEN_LOGE("FillParcelWithTransactionData data.WriteParcelable failed!");
            return false;
        }
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

ErrCode RSRenderServiceConnectionProxy::GetUniRenderEnabled(bool& enable)
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
    if (!reply.ReadBool(enable)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetUniRenderEnabled Read enable failed!");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId,
    bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteInterfaceToken err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(nodeId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteUint64 NodeId err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(displayNodeConfig.mirrorNodeId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteUint64 Config.MirrorNodeId err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(displayNodeConfig.screenId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteUint64 Config.ScreenId err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(displayNodeConfig.isMirrored)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteBool Config.IsMirrored err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_DISPLAY_NODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }

    if (!reply.ReadBool(success)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode Read success failed!");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::CreateNode(const RSSurfaceRenderNodeConfig& config, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteUint64(config.id)) {
        ROSEN_LOGE("CreateNode: WriteUint64 Config.id err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(config.name)) {
        ROSEN_LOGE("CreateNode: WriteString Config.name err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }

    if (!reply.ReadBool(success)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode Read success failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    sptr<Surface>& sfc, bool unobscured)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteUint64(config.id)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteUint64 config.id err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(config.name)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteString config.name err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(config.nodeType))) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteUint8 config.nodeType err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(config.isTextureExportNode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteBool config.isTextureExportNode err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(config.isSync)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteBool config.isSync err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(config.surfaceWindowType))) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteUint8 config.surfaceWindowType err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(unobscured)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNodeAndSurface: WriteBool unobscured err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> surfaceObject = reply.ReadRemoteObject();
    sptr<IBufferProducer> bp = iface_cast<IBufferProducer>(surfaceObject);
    if (bp == nullptr) {
        return ERR_INVALID_VALUE;
    }
    sfc = Surface::CreateSurfaceAsProducer(bp);
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::CreateVSyncConnection(sptr<IVSyncConnection>& vsyncConn,
                                                              const std::string& name,
                                                              const sptr<VSyncIConnectionToken>& token,
                                                              VSyncConnParam vsyncConnParam)
{
    if (token == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateVSyncConnection: token is nullptr.");
        vsyncConn = nullptr;
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION);
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

ErrCode RSRenderServiceConnectionProxy::GetPixelMapByProcessId(
    std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteUint64(pid)) {
        ROSEN_LOGE("GetPixelMapByProcessId: WriteUint64 pid err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelMapByProcessId: Send Request err");
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(repCode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelMapByProcessId Read repCode failed");
        return ERR_INVALID_VALUE;
    }
    if (repCode == SUCCESS) {
        pixelMapInfoVector.clear();
        if (!RSMarshallingHelper::Unmarshalling(reply, pixelMapInfoVector)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelMapByProcessId: Unmarshalling failed");
        }
    } else {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelMapByProcessId: Invalid reply");
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::CreatePixelMapFromSurface(sptr<Surface> surface,
    const Rect &srcRect, std::shared_ptr<Media::PixelMap> &pixelMap)
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

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteInterfaceToken RSIRenderServiceConnection::GetDescriptor() err.");
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
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreatePixelMapFromSurface: Send Request err.");
        return ERR_INVALID_VALUE;
    }

    if (reply.ReadBool()) {
        pixelMap.reset(Media::PixelMap::Unmarshalling(reply));
    } else {
        ROSEN_LOGE("CreatePixelMapFromSurface: ReadBool err.");
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteInterfaceToken err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInt32(info.pid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteInt32 pid err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(info.uid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteInt32 uid err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(info.bundleName)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteString bundleName err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(info.abilityName)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteString abilityName err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(info.focusNodeId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: WriteUint64 focusNodeId err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetFocusAppInfo: Send Request err.");
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    repCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::GetDefaultScreenId(uint64_t& screenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetDefaultScreenId: WriteInterfaceToken RSIRenderServiceConnection::GetDescriptor() err.");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: sendrequest error : %{public}d", __func__, err);
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadUint64(screenId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetDefaultScreenId Read id failed");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::GetActiveScreenId(uint64_t& screenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetActiveScreenId: WriteInterfaceToken RSIRenderServiceConnection::GetDescriptor() err.");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: sendrequest error : %{public}d", __func__, err);
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadUint64(screenId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetActiveScreenId Read id failed");
        screenId = INVALID_SCREEN_ID;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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

    uint32_t size{0};
    if (!reply.ReadUint32(size)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetAllScreenIds Read size failed");
        return std::vector<ScreenId>();
    }
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
        ROSEN_LOGE("RSRenderServiceConnectionProxy::%{public}s: Send Request err.", __func__);
        return INVALID_SCREEN_ID;
    }

    uint64_t id{0};
    if (!reply.ReadUint64(id)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateVirtualScreen Read id failed");
        return INVALID_SCREEN_ID;
    }
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

ErrCode RSRenderServiceConnectionProxy::SetVirtualScreenTypeBlackList(
    ScreenId id, std::vector<NodeType>& typeBlackListVector, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenTypeBlackList: Send Request err.");
        return ERR_INVALID_VALUE;
    }

    repCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::AddVirtualScreenBlackList(
    ScreenId id, std::vector<NodeId>& blackListVector, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("AddVirtualScreenBlackList: WriteInterfaceToken GetDescriptor err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("AddVirtualScreenBlackList: WriteUint64 id err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUInt64Vector(blackListVector)) {
        ROSEN_LOGE("AddVirtualScreenBlackList: WriteUInt64Vector id err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::AddVirtualScreenBlackList: Send Request err.");
        return ERR_INVALID_VALUE;
    }

    repCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::RemoveVirtualScreenBlackList(
    ScreenId id, std::vector<NodeId>& blackListVector, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RemoveVirtualScreenBlackList: WriteInterfaceToken GetDescriptor err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RemoveVirtualScreenBlackList: WriteUint64 id err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUInt64Vector(blackListVector)) {
        ROSEN_LOGE("RemoveVirtualScreenBlackList: WriteUInt64Vector blackListVector err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RemoveVirtualScreenBlackList: Send Request err.");
        return ERR_INVALID_VALUE;
    }

    repCode = reply.ReadInt32();
    return ERR_OK;
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

    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenSecurityExemptionList Read status failed");
        return READ_PARCEL_ERR;
    }
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

    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetMirrorScreenVisibleRect Read status failed");
        return READ_PARCEL_ERR;
    }
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
    uint32_t rate{0};
    if (!reply.ReadUint32(rate)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenCurrentRefreshRate Read rate failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t refreshRateMode{0};
    if (!reply.ReadInt32(refreshRateMode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetCurrentRefreshRateMode Read refreshRateMode failed");
        return READ_PARCEL_ERR;
    }
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
    uint64_t rateCount{0};
    if (!reply.ReadUint64(rateCount)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenSupportedRefreshRates Read rateCount failed");
        return screenSupportedRates;
    }
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

ErrCode RSRenderServiceConnectionProxy::GetShowRefreshRateEnabled(bool& enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceProxy failed to get descriptor");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadBool(enable)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetCurrentRefreshRateMode Read enable failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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
    uint32_t rate{0};
    if (!reply.ReadUint32(rate)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetRealtimeRefreshRate Read rate failed");
        return READ_PARCEL_ERR;
    }
    return rate;
}

ErrCode RSRenderServiceConnectionProxy::GetRefreshInfo(pid_t pid, std::string& enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        enable = "";
        return ERR_INVALID_VALUE;
    }

    if (!reply.ReadString(enable)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetRefreshInfo Read enable failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::GetRefreshInfoToSP(NodeId id, std::string& enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_TO_SP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceProxy sendrequest error : %{public}d", err);
        enable = "";
        return ERR_INVALID_VALUE;
    }

    if (!reply.ReadString(enable)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetRefreshInfo Read enable failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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
    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetPhysicalScreenResolution Read status failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenResolution Read status failed");
        return READ_PARCEL_ERR;
    }
    return status;
}

ErrCode RSRenderServiceConnectionProxy::MarkPowerOffNeedProcessOneFrame()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::MarkPowerOffNeedProcessOneFrame: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME);
    int32_t err = SendRequest(code, data, reply, option);
    return err != NO_ERROR ? ERR_INVALID_VALUE : ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::RepaintEverything()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RepaintEverything: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPAINT_EVERYTHING);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::ForceRefreshOneFrameWithNextVSync()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ForceRefreshOneFrameWithNextVSync: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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
        ROSEN_LOGE("SetScreenPowerStatus: SendRequest failed %{public}d", err);
        return;
    }
}

ErrCode RSRenderServiceConnectionProxy::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    if (app == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteRemoteObject(app->AsObject())) {
        ROSEN_LOGE("%{public}s WriteRemoteObject failed", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSRenderServiceConnectionProxy::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect,
    std::unique_ptr<Media::PixelMap> pixelMap, RSSurfaceCapturePermissions /* permissions */)
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

    if (!WriteClientSurfacePixelMap(pixelMap, captureConfig.isClientPixelMap, data)) {
        ROSEN_LOGE("%{public}s write pixelMap failed", __func__);
        return;
    }
    
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSRenderServiceConnectionProxy::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig, RSSurfaceCapturePermissions)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return pixelMapIdPairVector;
    }
    if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
        ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
        return pixelMapIdPairVector;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_SOLO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return pixelMapIdPairVector;
    }
    if (!RSMarshallingHelper::Unmarshalling(reply, pixelMapIdPairVector)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::TakeSurfaceCaptureSoloNode Unmarshalling failed");
    }
    return pixelMapIdPairVector;
}

void RSRenderServiceConnectionProxy::TakeSelfSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig)
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}

ErrCode RSRenderServiceConnectionProxy::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isFreeze)) {
        ROSEN_LOGE("%{public}s write isFreeze failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (isFreeze) {
        if (callback == nullptr) {
            ROSEN_LOGE("%{public}s callback == nullptr", __func__);
            return ERR_INVALID_VALUE;
        }
        if (!data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("%{public}s write callback failed", __func__);
            return ERR_INVALID_VALUE;
        }
        if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
            ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
            return ERR_INVALID_VALUE;
        }
        if (!WriteSurfaceCaptureBlurParam(blurParam, data)) {
            ROSEN_LOGE("%{public}s write blurParam failed", __func__);
            return ERR_INVALID_VALUE;
        }
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSRenderServiceConnectionProxy::TakeUICaptureInRange(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE);
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
        !data.WriteBool(captureConfig.isClientPixelMap) ||
        !data.WriteFloat(captureConfig.mainScreenRect.left_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.top_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.right_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.bottom_) ||
        !data.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId) ||
        !data.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize) ||
        !data.WriteUInt64Vector(captureConfig.blackList) ||
        !data.WriteUint32(captureConfig.backGroundColor)) {
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

bool RSRenderServiceConnectionProxy::WriteClientSurfacePixelMap(
    const std::unique_ptr<Media::PixelMap>& pixelMap, bool isUsedClientPixelMap, MessageParcel& data)
{
    // use service create pixelmap
    if (!isUsedClientPixelMap) {
        return true;
    }

    // vaild pixelMap and flag appear in pairs
    if (pixelMap == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::WriteClientSurfacePixelMap pixelMap is nullptr");
        return false;
    }

    if (!data.WriteParcelable(pixelMap.get())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::WriteClientSurfacePixelMap WriteParcelable fail");
        return false;
    }

    return true;
}
ErrCode RSRenderServiceConnectionProxy::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(rsNodeId)) {
        ROSEN_LOGE("SetHwcNodeBounds write id failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteFloat(positionX) || !data.WriteFloat(positionY) || !data.WriteFloat(positionZ) ||
        !data.WriteFloat(positionW)) {
        ROSEN_LOGE("SetHwcNodeBounds write bound failed");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_POSITION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SetHwcNodeBounds SendRequest() error[%{public}d]", err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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

ErrCode RSRenderServiceConnectionProxy::GetScreenActiveMode(uint64_t id, RSScreenModeInfo& screenModeInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenActiveMode: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenActiveMode: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE);
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

    uint64_t modeCount{0};
    if (!reply.ReadUint64(modeCount)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenSupportedModes Read modeCount failed");
        return screenSupportedModes;
    }
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

ErrCode RSRenderServiceConnectionProxy::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetMemoryGraphics: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }

    uint64_t count{0};
    if (!reply.ReadUint64(count)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetMemoryGraphics Read count failed");
        return ERR_INVALID_VALUE;
    }
    size_t readableSize = reply.GetReadableBytes();
    size_t len = static_cast<size_t>(count);
    if (len > readableSize || len > memoryGraphics.max_size()) {
        RS_LOGE("RSRenderServiceConnectionProxy GetMemoryGraphics Failed to read vector, size:%{public}zu,"
            " readableSize:%{public}zu", len, readableSize);
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

ErrCode RSRenderServiceConnectionProxy::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    MemoryGraphic memoryGraphic;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetTotalAppMemSize: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }

    if (!reply.ReadFloat(cpuMemSize) || !reply.ReadFloat(gpuMemSize)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetTotalAppMemSize Read MemSize failed");
        return READ_PARCEL_ERR;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetMemoryGraphic: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("GetMemoryGraphic: WriteInt32 pid err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC);
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

ErrCode RSRenderServiceConnectionProxy::GetScreenPowerStatus(uint64_t screenId, uint32_t& status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenPowerStatus: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(screenId)) {
        ROSEN_LOGE("GetScreenPowerStatus: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    status = reply.ReadUint32();
    return ERR_OK;
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

ErrCode RSRenderServiceConnectionProxy::GetScreenBacklight(uint64_t id, int32_t& level)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenBacklight: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenBacklight: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(level)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenBacklight Read level failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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
        ROSEN_LOGE("SetScreenBacklight: SendRequest failed");
        return;
    }
}

ErrCode RSRenderServiceConnectionProxy::RegisterBufferClearListener(
    NodeId id, sptr<RSIBufferClearCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterBufferClearListener: callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterBufferClearListener: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RegisterBufferClearListener: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterBufferClearListener: WriteRemoteObject callback->AsObject() err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterBufferClearListener: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterBufferAvailableListener: callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterBufferAvailableListener: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RegisterBufferAvailableListener: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterBufferAvailableListener: WriteRemoteObject callback->AsObject() err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isFromRenderThread)) {
        ROSEN_LOGE("RegisterBufferAvailableListener: WriteBool isFromRenderThread err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterBufferAvailableListener: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenSupportedColorGamuts Read result failed");
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenSupportedMetaDataKeys Read result failed");
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenColorGamut Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result == SUCCESS) {
        uint32_t readMode{0};
        if (!reply.ReadUint32(readMode)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenColorGamut Read mode failed");
            return READ_PARCEL_ERR;
        }
        mode = static_cast<ScreenColorGamut>(readMode);
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetScreenColorGamut Read result failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetScreenGamutMap Read result failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetScreenCorrection Read result failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenGamutMap Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result == SUCCESS) {
        uint32_t readMode{0};
        if (!reply.ReadUint32(readMode)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenGamutMap Read mode failed");
            return READ_PARCEL_ERR;
        }
        mode = static_cast<ScreenGamutMap>(readMode);
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenHDRCapability Read result failed");
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

ErrCode RSRenderServiceConnectionProxy::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelFormat Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        uint32_t readFormat{0};
        if (!reply.ReadUint32(readFormat)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelFormat Read readFormat failed");
            return READ_PARCEL_ERR;
        }
        pixelFormat = static_cast<GraphicPixelFormat>(readFormat);
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    resCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::GetScreenSupportedHDRFormats(
    ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetScreenSupportedHDRFormats: WriteInterfaceToken GetDescriptor err.");
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetScreenSupportedHDRFormats: WriteUint64 id err.");
        resCode =  WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode =  RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenSupportedHDRFormats Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        hdrFormats.clear();
        std::vector<uint32_t> hdrFormatsRecv;
        reply.ReadUInt32Vector(&hdrFormatsRecv);
        std::transform(hdrFormatsRecv.begin(), hdrFormatsRecv.end(), back_inserter(hdrFormats),
                       [](uint32_t i) -> ScreenHDRFormat {return static_cast<ScreenHDRFormat>(i);});
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenHDRFormat Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        uint32_t readFormat{0};
        if (!reply.ReadUint32(readFormat)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenHDRFormat1 Read readFormat failed");
            return READ_PARCEL_ERR;
        }
        hdrFormat = static_cast<ScreenHDRFormat>(readFormat);
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetScreenHDRFormat(ScreenId id, int32_t modeIdx, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    resCode = reply.ReadInt32();
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenSupportedColorSpaces Read resCode failed");
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

ErrCode RSRenderServiceConnectionProxy::GetScreenColorSpace(
    ScreenId id, GraphicCM_ColorSpaceType& colorSpace, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenColorSpace Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        uint32_t type{0};
        if (!reply.ReadUint32(type)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenColorSpace Read type failed");
            return READ_PARCEL_ERR;
        }
        colorSpace = static_cast<GraphicCM_ColorSpaceType>(type);
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetScreenColorSpace(
    ScreenId id, GraphicCM_ColorSpaceType colorSpace, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    resCode = reply.ReadInt32();
    return ERR_OK;
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
        uint32_t type{0};
        if (!reply.ReadUint32(type)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::GetScreenType Read type failed");
            return READ_PARCEL_ERR;
        }
        screenType = static_cast<RSScreenType>(type);
    }
    return result;
}

ErrCode RSRenderServiceConnectionProxy::GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetBitmap: WriteInterfaceToken GetDescriptor err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetBitmap: WriteUint64 id err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetBitmap Read result failed");
        return READ_PARCEL_ERR;
    }
    if (!result || !RSMarshallingHelper::Unmarshalling(reply, bitmap)) {
        RS_LOGE("RSRenderServiceConnectionProxy::GetBitmap: Unmarshalling failed");
        success = false;
        return ERR_INVALID_VALUE;
    }
    success = true;
    return ERR_OK;
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
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualMirrorScreenCanvasRotation Read result failed");
        return READ_PARCEL_ERR;
    }
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
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualMirrorScreenScaleMode Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

ErrCode RSRenderServiceConnectionProxy::SetGlobalDarkColorMode(bool isDark)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetGlobalDarkColorMode: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteBool(isDark)) {
        ROSEN_LOGE("SetGlobalDarkColorMode: WriteBool isDark err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetPixelmap: WriteInterfaceToken GetDescriptor err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetPixelmap: WriteUint64 id err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(pixelmap.get())) {
        ROSEN_LOGE("GetPixelmap: WriteParcelable pixelmap.get() err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    RSMarshallingHelper::Marshalling(data, *rect);
    RSMarshallingHelper::Marshalling(data, drawCmdList);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetPixelmap Read result failed");
        return READ_PARCEL_ERR;
    }
    if (!result || !RSMarshallingHelper::Unmarshalling(reply, pixelmap)) {
        RS_LOGD("RSRenderServiceConnectionProxy::GetPixelmap: GetPixelmap failed");
        success = false;
        return ERR_INVALID_VALUE;
    }
    success = true;
    return ERR_OK;
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
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterTypeface Read result failed");
        return READ_PARCEL_ERR;
    }
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

int32_t RSRenderServiceConnectionProxy::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
    std::vector<uint8_t>& edidData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteUint64(id)) {
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DISPLAY_IDENTIFICATION_DATA);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetDisplayIdentificationData Read result failed");
        return READ_PARCEL_ERR;
    }
    if (result != SUCCESS) {
        RS_LOGE("RSRenderServiceConnectionProxy::GetDisplayIdentificationData: failed");
        return result;
    }
    if (!reply.ReadUint8(outPort)) {
        return READ_PARCEL_ERR;
    }
    uint32_t edidSize = reply.ReadUint32();
    if (edidSize == 0 || edidSize > EDID_DATA_MAX_SIZE) {
        RS_LOGE("RSRenderServiceConnectionProxy::GetDisplayIdentificationData: EdidSize failed");
        return READ_PARCEL_ERR;
    }
    edidData.resize(edidSize);
    const uint8_t *editpnt = reply.ReadBuffer(edidSize);
    if (editpnt == nullptr) {
        RS_LOGE("RSRenderServiceConnectionProxy::GetDisplayIdentificationData: ReadBuffer failed");
        return READ_PARCEL_ERR;
    }
    RS_LOGD("RSRenderServiceConnectionProxy::GetDisplayIdentificationData: EdidSize: %{public}u", edidSize);
    edidData.assign(editpnt, editpnt + edidSize);

    return result;
}

ErrCode RSRenderServiceConnectionProxy::SetScreenSkipFrameInterval(uint64_t id, uint32_t skipFrameInterval,
    int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        resCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetPixelFormat Read result failed");
        resCode = READ_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetVirtualScreenRefreshRate(
    ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate, int32_t& retVal)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        retVal = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenRefreshRate Read result failed");
        retVal = READ_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (result == SUCCESS) {
        if (!reply.ReadUint32(actualRefreshRate)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenRefreshRate Read actualRefreshRate failed");
            retVal = READ_PARCEL_ERR;
            return ERR_INVALID_VALUE;
        }
    }
    retVal = result;
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetScreenActiveRect(ScreenId id, const Rect& activeRect, uint32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadUint32(repCode)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetScreenActiveRect Read result failed");
        repCode = READ_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::RegisterOcclusionChangeCallback(
    sptr<RSIOcclusionChangeCallback> callback, int32_t& repCode)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterOcclusionChangeCallback: callback is nullptr.");
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterOcclusionChangeCallback: WriteInterfaceToken GetDescriptor err.");
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterOcclusionChangeCallback: WriteRemoteObject callback->AsObject() err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    repCode = reply.ReadInt32();
    return ERR_OK;
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetScreenHDRFormat Read result failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetScreenColorSpace Read result failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterHgmConfigChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterHgmRefreshRateModeChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSRenderServiceConnectionProxy::RegisterFirstFrameCommitCallback(
    sptr<RSIFirstFrameCommitCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ON_FIRST_FRAME_COMMIT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterFirstFrameCommitCallback: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

ErrCode RSRenderServiceConnectionProxy::AvcodecVideoStart(
    uint64_t uniqueId, std::string& surfaceName, uint32_t fps, uint64_t reportTime)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("AvcodecVideoStart: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(uniqueId)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteUint64 uniqueId err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(surfaceName)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteString surfaceName err.");
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::AvcodecVideoStart: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::AvcodecVideoStart Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

ErrCode RSRenderServiceConnectionProxy::AvcodecVideoStop(uint64_t uniqueId, std::string& surfaceName, uint32_t fps)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("AvcodecVideoStop: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(uniqueId)) {
        ROSEN_LOGE("AvcodecVideoStop: WriteUint64 uniqueId err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(surfaceName)) {
        ROSEN_LOGE("AvcodecVideoStop: WriteString surfaceName err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(fps)) {
        ROSEN_LOGE("AvcodecVideoStop: WriteUint32 fps err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::AvcodecVideoStop: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::AvcodecVideoStop Read result failed");
        return READ_PARCEL_ERR;
    }
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
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE(
            "RSRenderServiceConnectionProxy::RegisterFrameRateLinkerExpectedFpsUpdateCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

ErrCode RSRenderServiceConnectionProxy::SetAppWindowNum(uint32_t num)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetAppWindowNum: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint32(num)) {
        ROSEN_LOGE("SetAppWindowNum: WriteUint32 num err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetAppWindowNum: Send Request err.");
        return ERR_INVALID_VALUE;
    }

    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetSystemAnimatedScenes(
    SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetSystemAnimatedScenes: WriteInterfaceToken GetDescriptor err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint32(static_cast<uint32_t>(systemAnimatedScenes))) {
        ROSEN_LOGE("SetSystemAnimatedScenes: WriteUint32 systemAnimatedScenes err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isRegularAnimation)) {
        ROSEN_LOGE("SetSystemAnimatedScenes: WriteBool isRegularAnimation err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadBool(success)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetSystemAnimatedScenes Read result failed");
        success = READ_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetWatermark(const std::string& name,
    std::shared_ptr<Media::PixelMap> watermark, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WATERMARK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWatermark: Send Request err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    success = true;
    return ERR_OK;
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
    int32_t status{0};
    if (!reply.ReadInt32(status)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ResizeVirtualScreen Read status failed");
        return READ_PARCEL_ERR;
    }
    return status;
}

ErrCode RSRenderServiceConnectionProxy::ReportJankStats()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportJankStats: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportJankStats: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::ReportEventResponse(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventResponse: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportEventResponse: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::ReportEventComplete(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventComplete: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportEventComplete: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::ReportEventJankFrame(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventJankFrame: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportEventJankFrame: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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

ErrCode RSRenderServiceConnectionProxy::ReportGameStateData(GameStateData info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportGameStateData: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportGameStateDataRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::ReportGameStateData: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetHardwareEnabled(NodeId id, bool isEnabled,
    SelfDrawingNodeType selfDrawingType, bool dynamicHardwareEnable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetHardwareEnabled: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetHardwareEnabled: WriteUint64 id err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isEnabled)) {
        ROSEN_LOGE("SetHardwareEnabled: WriteBool isEnabled err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(selfDrawingType))) {
        ROSEN_LOGE("SetHardwareEnabled: WriteUint8 selfDrawingType err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(dynamicHardwareEnable)) {
        ROSEN_LOGE("SetHardwareEnabled: WriteBool dynamicHardwareEnable err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetHardwareEnabled: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent, uint32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetHidePrivacyContent: WriteInterfaceToken GetDescriptor err.");
        resCode = static_cast<uint32_t>(RSInterfaceErrorCode::WRITE_PARCEL_ERROR);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetHidePrivacyContent: WriteUint64 id err.");
        resCode = static_cast<uint32_t>(RSInterfaceErrorCode::WRITE_PARCEL_ERROR);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(needHidePrivacyContent)) {
        ROSEN_LOGE("SetHidePrivacyContent: WriteBool needHidePrivacyContent err.");
        resCode = static_cast<uint32_t>(RSInterfaceErrorCode::WRITE_PARCEL_ERROR);
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetHidePrivacyContent: Send Request err.");
        resCode = static_cast<uint32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
        return ERR_INVALID_VALUE;
    }
    resCode = reply.ReadUint32();
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("NotifyLightFactorStatus: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(lightFactorStatus)) {
        ROSEN_LOGE("NotifyLightFactorStatus: WriteInt32 lightFactorStatus err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyLightFactorStatus: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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
        ROSEN_LOGE(
            "RSRenderServiceConnectionProxy::NotifyAppStrategyConfigChangeEvent Write pakName or listSize failed.");
        return;
    }

    for (const auto& [key, value] : newConfig) {
        if (!data.WriteString(key) || !data.WriteString(value)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyAppStrategyConfigChangeEvent Write key or value failed.");
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

void RSRenderServiceConnectionProxy::SetWindowExpectedRefreshRate(
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
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowExpectedRefreshRate: Send Request err.");
        return;
    }
}

void RSRenderServiceConnectionProxy::SetWindowExpectedRefreshRate(
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
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowExpectedRefreshRate: Send Request err.");
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

ErrCode RSRenderServiceConnectionProxy::NotifySoftVsyncEvent(uint32_t pid, uint32_t rateDiscount)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifySoftVsyncEvent: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

bool RSRenderServiceConnectionProxy::NotifySoftVsyncRateDiscountEvent(uint32_t pid, const std::string &name,
    uint32_t rateDiscount)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifySoftVsyncRateDiscountEvent: Send Request err.");
        return false;
    }
    bool enable{false};
    if (!reply.ReadBool(enable)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifySoftVsyncRateDiscountEvent: Read enable failed");
        return false;
    }
    return enable;
}

ErrCode RSRenderServiceConnectionProxy::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyTouchEvent: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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

ErrCode RSRenderServiceConnectionProxy::NotifyHgmConfigEvent(const std::string &eventName, bool state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_HGMCONFIG_EVENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyHgmConfigEvent: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::NotifyXComponentExpectedFrameRate(
    const std::string& id, int32_t expectedFrameRate)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyXComponentExpectedFrameRate: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetCacheEnabledForRotation(bool isEnabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetCacheEnabledForRotation: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isEnabled)) {
        ROSEN_LOGE("SetCacheEnabledForRotation: WriteBool isEnabled err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetCacheEnabledForRotation: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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
    int32_t activeDirtyRegionInfosSize{0};
    if (!reply.ReadInt32(activeDirtyRegionInfosSize)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetActiveDirtyRegionInfo Read activeDirtyRegionInfosSize failed");
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
                "RSRenderServiceConnectionProxy::GetActiveDirtyRegionInfo Read parcel failed");
            return activeDirtyRegionInfos;
        }
        activeDirtyRegionInfos.emplace_back(
            ActiveDirtyRegionInfo(activeDirtyRegionArea, activeFramesNumber, pidOfBelongsApp, windowName));
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
    int64_t globalDirtyRegionAreas{0};
    int32_t globalFramesNumber{0};
    int32_t skipProcessFramesNumber{0};
    int32_t mostSendingPidWhenDisplayNodeSkip{0};
    if (!reply.ReadInt64(globalDirtyRegionAreas) || !reply.ReadInt32(globalFramesNumber) ||
        !reply.ReadInt32(skipProcessFramesNumber) || !reply.ReadInt32(mostSendingPidWhenDisplayNodeSkip)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetGlobalDirtyRegionInfo Read parcel failed");
        return globalDirtyRegionInfo;
    }
    return GlobalDirtyRegionInfo(
        globalDirtyRegionAreas, globalFramesNumber, skipProcessFramesNumber, mostSendingPidWhenDisplayNodeSkip);
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
    int32_t uniformRenderFrameNumber{0};
    int32_t offlineComposeFrameNumber{0};
    int32_t redrawFrameNumber{0};
    if (!reply.ReadInt32(uniformRenderFrameNumber) || !reply.ReadInt32(offlineComposeFrameNumber) ||
        !reply.ReadInt32(redrawFrameNumber)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetLayerComposeInfo Read parcel failed");
        return layerComposeInfo;
    }
    return LayerComposeInfo(uniformRenderFrameNumber, offlineComposeFrameNumber, redrawFrameNumber);
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
    int32_t size{0};
    if (!reply.ReadInt32(size)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetHwcDisabledReasonInfo Read size failed");
        return hwcDisabledReasonInfos;
    }
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

ErrCode RSRenderServiceConnectionProxy::GetHdrOnDuration(int64_t& hdrOnDuration)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("GetHdrOnDuration: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HDR_ON_DURATION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    hdrOnDuration = reply.ReadInt64();
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetVmaCacheStatus(bool flag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVmaCacheStatus: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(flag)) {
        ROSEN_LOGE("SetVmaCacheStatus: WriteBool flag err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVmaCacheStatus %d: Send Request err.", flag);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

#ifdef TP_FEATURE_ENABLE
EErrCode RSRenderServiceConnectionProxy::SetTpFeatureConfig(int32_t feature, const char* config,
    TpFeatureConfigType tpFeatureConfigType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
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

ErrCode RSRenderServiceConnectionProxy::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetCurtainScreenUsingStatus: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isCurtainScreenOn)) {
        ROSEN_LOGE("SetCurtainScreenUsingStatus: WriteBool isCurtainScreenOn err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetCurtainScreenUsingStatus: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::DropFrameByPid(const std::vector<int32_t> pidList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("DropFrameByPid: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32Vector(pidList)) {
        ROSEN_LOGE("DropFrameByPid: WriteInt32Vector pidList err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::DropFrameByPid: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSRenderServiceConnectionProxy::RegisterUIExtensionCallback(
    uint64_t userId, sptr<RSIUIExtensionCallback> callback, bool unobscured)
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
    if (data.WriteUint64(userId) && data.WriteRemoteObject(callback->AsObject()) && data.WriteBool(unobscured)) {
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            return RS_CONNECTION_ERROR;
        }
        int32_t result{0};
        if (!reply.ReadInt32(result)) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterUIExtensionCallback Read result failed");
            return READ_PARCEL_ERR;
        }
        return result;
    } else {
        ROSEN_LOGE("RegisterUIExtensionCallback: WriteUint64[userId] OR WriteRemoteObject[callback] err.");
        return RS_CONNECTION_ERROR;
    }
}

ErrCode RSRenderServiceConnectionProxy::SetVirtualScreenStatus(ScreenId id,
    VirtualScreenStatus screenStatus, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    if (!data.WriteUint8(static_cast<uint8_t>(screenStatus))) {
        ROSEN_LOGE("SetVirtualScreenStatus: WriteUint8 screenStatus err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadBool(success)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetVirtualScreenStatus Read result failed");
        return READ_PARCEL_ERR;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetAncoForceDoDirect(bool direct, bool& res)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("SetAncoForceDoDirect: WriteInterfaceToken GetDescriptor err.");
        res = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (data.WriteBool(direct)) {
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            res = false;
            return ERR_INVALID_VALUE;
        }
        res = reply.ReadBool();
        return ERR_OK;
    } else {
        ROSEN_LOGE("SetAncoForceDoDirect: WriteBool direct err.");
        res = false;
        return ERR_INVALID_VALUE;
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
void RSRenderServiceConnectionProxy::RegisterTransactionDataCallback(int32_t pid,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterTransactionDataCallback callback == nullptr.");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterTransactionDataCallback: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterTransactionDataCallback: write int32 val err.");
        return;
    }
    if (!data.WriteUint64(timeStamp)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterTransactionDataCallback: write uint64 val err.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterTransactionDataCallback: write Callback val err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK);
    RS_LOGD("RSRenderServiceConnectionProxy::RegisterTransactionDataCallback: timeStamp: %{public}"
        PRIu64 " pid: %{public}d", timeStamp, pid);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterTransactionDataCallback: Send Request err.");
        return;
    }
}

ErrCode RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback(
    pid_t pid, uint64_t uid, sptr<RSISurfaceBufferCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback callback == nullptr");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: write Int32 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(uid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: write Uint64 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: write RemoteObject val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSurfaceBufferCallback: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback: write Int32 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(uid)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback: write Uint64 val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::UnregisterSurfaceBufferCallback: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetLayerTop: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteString(nodeIdStr) && data.WriteBool(isTop)) {
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::SetLayerTop: Send Request err.");
            return ERR_INVALID_VALUE;
        }
    }
    return ERR_OK;
}

void RSRenderServiceConnectionProxy::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetColorFollow: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteString(nodeIdStr) && data.WriteBool(isColorFollow)) {
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_COLOR_FOLLOW);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::SetColorFollow: Send Request err.");
            return;
        }
    }
}

ErrCode RSRenderServiceConnectionProxy::SetWindowContainer(NodeId nodeId, bool value)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowContainer: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(nodeId)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowContainer: write Uint64 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(value)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowContainer: write Bool val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_CONTAINER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetWindowContainer: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSRenderServiceConnectionProxy::RegisterSelfDrawingNodeRectChangeCallback(
    sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    if (!callback) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSelfDrawingNodeRectChangeCallback callback is nullptr");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Send request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::RegisterSelfDrawingNodeRectChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

bool RSRenderServiceConnectionProxy::GetHighContrastTextState()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HIGH_CONTRAST_TEXT_STATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetHighContrastTextState: Send Request err.");
        return false;
    }
    return reply.ReadBool();
}

int32_t RSRenderServiceConnectionProxy::SendRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    if (!Remote()) {
        return static_cast<int32_t>(RSInterfaceErrorCode::NULLPTR_ERROR);
    }
    return Remote()->SendRequest(code, data, reply, option);
}

ErrCode RSRenderServiceConnectionProxy::NotifyScreenSwitched()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: Write InterfaceToken val err.", __func__);
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request error.", __func__);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSRenderServiceConnectionProxy::SetOverlayDisplayMode(int32_t mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: Write InterfaceToken val err.", __func__);
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(mode)) {
        ROSEN_LOGE("%{public}s: Write Int32 val err.", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OVERLAY_DISPLAY_MODE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed. err:%{public}d.", __func__, err);
        return ERR_INVALID_VALUE;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetOverlayDisplayMode Read result failed");
        return READ_PARCEL_ERR;
    }
    ROSEN_LOGI("%{public}s: mode:%{public}d, result:%{public}d.", __func__, mode, result);
    return result == 0 ? ERR_OK : ERR_INVALID_VALUE;
}
#endif

ErrCode RSRenderServiceConnectionProxy::NotifyPageName(const std::string &packageName,
    const std::string &pageName, bool isEnter)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyPageName: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteString(packageName) && data.WriteString(pageName) && data.WriteBool(isEnter)) {
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PAGE_NAME);
        int32_t err = Remote()->SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyPageName: Send Request err.");
            return ERR_INVALID_VALUE;
        }
    } else {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::NotifyPageName: write data err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::SetBehindWindowFilterEnabled(bool enabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetBehindWindowFilterEnabled WriteInterfaceToken err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteBool(enabled)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetBehindWindowFilterEnabled WriteBool err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetBehindWindowFilterEnabled sendrequest error : %{public}d", err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSRenderServiceConnectionProxy::GetBehindWindowFilterEnabled(bool& enabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetBehindWindowFilterEnabled WriteInterfaceToken err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::GetBehindWindowFilterEnabled sendrequest error : %{public}d", err);
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadBool(enabled)) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::SetBehindWindowFilterEnabled ReadBool err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSRenderServiceConnectionProxy::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return RS_CONNECTION_ERROR;
    }

    if (!data.WriteInt32(pid)) {
        return WRITE_PARCEL_ERR;
    }

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PID_GPU_MEMORY_IN_MB);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    if (!reply.ReadFloat(gpuMemInMB)) {
        return READ_PARCEL_ERR;
    }

    return err;
}
} // namespace Rosen
} // namespace OHOS
