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

#include "rs_client_to_render_connection_proxy.h"

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

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "buffer_utils.h"
#endif

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
constexpr uint32_t MAX_DROP_FRAME_PID_LIST_SIZE = 1024;
}

RSClientToRenderConnectionProxy::RSClientToRenderConnectionProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIClientToRenderConnection>(impl)
{
}

int32_t RSClientToRenderConnectionProxy::SendRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (!Remote()) {
        return static_cast<int32_t>(RSInterfaceErrorCode::NULLPTR_ERROR);
    }
    return Remote()->SendRequest(code, data, reply, option);
}

ErrCode RSClientToRenderConnectionProxy::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    if (!transactionData) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CommitTransaction transactionData nullptr!");
        return ERR_INVALID_VALUE;
    }
    bool isUniMode = RSSystemProperties::GetUniRenderEnabled();
    transactionData->SetSendingPid(pid_);

    // split to several parcels if parcel size > PARCEL_SPLIT_THRESHOLD during marshalling
    std::vector<std::shared_ptr<MessageParcel>> parcelVector;
    auto func = [isUniMode, &parcelVector, &transactionData, this]() -> bool {
        if (isUniMode) {
            transactionData->SetIndex(transactionDataIndex_.fetch_add(1, std::memory_order_relaxed) + 1);
        } else {
            transactionData->SetIndex(transactionDataIndex_.load(std::memory_order_relaxed));
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
        uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::COMMIT_TRANSACTION);
        int retryCount = 0;
        int32_t err = NO_ERROR;
        do {
            err = Remote()->SendRequest(code, *parcel, reply, option);
            if (err != NO_ERROR && retryCount < MAX_RETRY_COUNT) {
                retryCount++;
                usleep(RETRY_WAIT_TIME_US);
            } else if (err != NO_ERROR) {
                ROSEN_LOGE("RSClientToRenderConnectionProxy::CommitTransaction SendRequest failed, "
                    "err = %{public}d, retryCount = %{public}d, data size:%{public}zu", err, retryCount,
                    parcel->GetDataSize());
                return ERR_INVALID_VALUE;
            }
        } while (err != NO_ERROR);
    }
    return ERR_OK;
}

bool RSClientToRenderConnectionProxy::FillParcelWithTransactionData(
    std::unique_ptr<RSTransactionData>& transactionData, std::shared_ptr<MessageParcel>& data)
{
    if (!data->WriteInterfaceToken(RSClientToRenderConnectionProxy::GetDescriptor())) {
        ROSEN_LOGE("FillParcelWithTransactionData WriteInterfaceToken failed");
        return false;
    }
    // write a flag at the begin of parcel to identify parcel type
    // 0: indicate normal parcel
    // 1: indicate ashmem parcel
    if (!data->WriteInt32(0)) {
        ROSEN_LOGE("FillParcelWithTransactionData WriteInt32 failed!");
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

ErrCode RSClientToRenderConnectionProxy::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr) {
        return ERR_INVALID_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSClientToRenderConnectionProxy::GetDescriptor())) {
        ROSEN_LOGE("ExecuteSynchronousTask WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!task->Marshalling(data)) {
        ROSEN_LOGE("ExecuteSynchronousTask Marshalling failed");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }

    if (task->CheckHeader(reply)) {
        task->ReadFromParcel(reply);
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId,
    bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNode: WriteInterfaceToken err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(nodeId)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNode: WriteUint64 NodeId err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(displayNodeConfig.mirrorNodeId)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNode: WriteUint64 Config.MirrorNodeId err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(displayNodeConfig.screenId)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNode: WriteUint64 Config.ScreenId err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(displayNodeConfig.isMirrored)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNode: WriteBool Config.IsMirrored err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(displayNodeConfig.mirrorSourceRotation))) {
        ROSEN_LOGE("RSRenderServiceConnectionProxy::CreateNode: WriteUint32 Config.MirrorSourceRotation err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_DISPLAY_NODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    bool enable{false};
    if (!reply.ReadBool(enable)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNode Read enable failed!");
        success = false;
        return ERR_INVALID_VALUE;
    }
    success = true;
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::CreateNode(const RSSurfaceRenderNodeConfig& config, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNode: WriteInterfaceToken err.");
        success = false;
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteUint64(config.id)) {
        ROSEN_LOGE("CreateNode: WriteUint64 Config.id err.");
        success =  false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(config.name)) {
        ROSEN_LOGE("CreateNode: WriteString Config.name err.");
        success =  false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_NODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success =  false;
        return ERR_INVALID_VALUE;
    }

    bool enable{false};
    if (!reply.ReadBool(enable)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNode Read enable failed!");
        success =  false;
        return ERR_INVALID_VALUE;
    }
    success = true;
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    sptr<Surface>& sfc, bool unobscured)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNodeAndSurface: WriteInterfaceToken err.");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteUint64(config.id)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNodeAndSurface: WriteUint64 config.id err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(config.name)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNodeAndSurface: WriteString config.name err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(config.nodeType))) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNodeAndSurface: WriteUint8 config.nodeType err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(config.isTextureExportNode)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNodeAndSurface: WriteBool config.isTextureExportNode err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(config.isSync)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNodeAndSurface: WriteBool config.isSync err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(config.surfaceWindowType))) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNodeAndSurface: WriteUint8 config.surfaceWindowType err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(unobscured)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreateNodeAndSurface: WriteBool unobscured err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);
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

ErrCode RSClientToRenderConnectionProxy::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    if (app == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);

    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterApplicationAgent: WriteInterfaceToken err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(app->AsObject())) {
        ROSEN_LOGE("%{public}s WriteRemoteObject failed", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_APPLICATION_AGENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::RegisterBufferClearListener(
    NodeId id, sptr<RSIBufferClearCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterBufferClearListener: callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterBufferClearListener: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterBufferAvailableListener: callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterBufferAvailableListener: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_BITMAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    bool result{false};
    if (!reply.ReadBool(result)) {
        success = false;
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetBitmap Read result failed");
        return READ_PARCEL_ERR;
    }
    if (!result || !RSMarshallingHelper::Unmarshalling(reply, bitmap)) {
        RS_LOGE("RSClientToRenderConnectionProxy::GetBitmap: Unmarshalling failed");
        success = false;
        return ERR_INVALID_VALUE;
    }
    success = true;
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::SetGlobalDarkColorMode(bool isDark)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("SetGlobalDarkColorMode: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteBool(isDark)) {
        ROSEN_LOGE("SetGlobalDarkColorMode: WriteBool isDark err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("GetPixelmap: WriteInterfaceToken GetDescriptor err.");
        success = false;
        return ERR_INVALID_VALUE;
    }
    RSMarshallingHelper::MarshallingTransactionVer(data);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_PIXELMAP);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        success = false;
        return ERR_INVALID_VALUE;
    }
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetPixelmap Read result failed");
        success = false;
        return READ_PARCEL_ERR;
    }
    if (!result || !RSMarshallingHelper::Unmarshalling(reply, pixelmap)) {
        RS_LOGD("RSClientToRenderConnectionProxy::GetPixelmap: GetPixelmap failed");
        success = false;
        return ERR_INVALID_VALUE;
    }
    success = true;
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::SetSystemAnimatedScenes(
    SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
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
        RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
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

ErrCode RSClientToRenderConnectionProxy::SetHardwareEnabled(NodeId id, bool isEnabled,
    SelfDrawingNodeType selfDrawingType, bool dynamicHardwareEnable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_HARDWARE_ENABLED);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetHardwareEnabled: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::SetHidePrivacyContent(NodeId id,
    bool needHidePrivacyContent, uint32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetHidePrivacyContent: Send Request err.");
        resCode = static_cast<uint32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
        return ERR_INVALID_VALUE;
    }
    resCode = reply.ReadUint32();
    return ERR_OK;
}

bool RSClientToRenderConnectionProxy::GetHighContrastTextState()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetHighContrastTextState: WriteInterfaceToken err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_HIGH_CONTRAST_TEXT_STATE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetHighContrastTextState: Send Request err.");
        return false;
    }
    return reply.ReadBool();
}

ErrCode RSClientToRenderConnectionProxy::SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteInterfaceToken err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInt32(info.pid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteInt32 pid err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(info.uid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteInt32 uid err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(info.bundleName)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteString bundleName err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(info.abilityName)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteString abilityName err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(info.focusNodeId)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteUint64 focusNodeId err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_FOCUS_APP_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: Send Request err.");
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    repCode = reply.ReadInt32();
    return ERR_OK;
}


void RSClientToRenderConnectionProxy::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
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
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::TakeSurfaceCapture: WriteInterfaceToken err.");
        return;
    }
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
    
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>>
RSClientToRenderConnectionProxy::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig, RSSurfaceCapturePermissions)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::TakeSurfaceCaptureSoloNode: WriteInterfaceToken err.");
        return pixelMapIdPairVector;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return pixelMapIdPairVector;
    }
    if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
        ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
        return pixelMapIdPairVector;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_SOLO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return pixelMapIdPairVector;
    }
    if (!RSMarshallingHelper::Unmarshalling(reply, pixelMapIdPairVector)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::TakeSurfaceCaptureSoloNode Unmarshalling failed");
    }
    return pixelMapIdPairVector;
}


void RSClientToRenderConnectionProxy::TakeSelfSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
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
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::TakeSelfSurfaceCapture: WriteInterfaceToken err.");
        return;
    }
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}


ErrCode RSClientToRenderConnectionProxy::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetWindowFreezeImmediately: WriteInterfaceToken err.");
        return ERR_INVALID_VALUE;
    }
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

    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::TakeSurfaceCaptureWithAllWindows(NodeId id,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    bool checkDrmAndSurfaceLock, RSSurfaceCapturePermissions /*permissions*/)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s GetDescriptor err", __func__);
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(checkDrmAndSurfaceLock)) {
        ROSEN_LOGE("%{public}s write checkDrmAndSurfaceLock failed", __func__);
        return ERR_INVALID_VALUE;
    }
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
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::FreezeScreen(NodeId id, bool isFreeze, bool needSync)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s GetDescriptor err", __func__);
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isFreeze)) {
        ROSEN_LOGE("%{public}s write isFreeze failed", __func__);
        return ERR_INVALID_VALUE;
    }
    
    if (!data.WriteBool(needSync)) {
        ROSEN_LOGE("%{public}s write needSync failed", __func__);
        return ERR_INVALID_VALUE;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::FREEZE_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToRenderConnectionProxy::TakeUICaptureInRange(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    RSSurfaceCapturePermissions /* permissions */)
{
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr", __func__);
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::TakeUICaptureInRange: WriteInterfaceToken err.");
        return;
    }
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}

bool RSClientToRenderConnectionProxy::WriteSurfaceCaptureConfig(
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
        !data.WriteBool(captureConfig.dynamicRangeMode.second) ||
        !data.WriteBool(captureConfig.isSyncRender)) {
        ROSEN_LOGE("WriteSurfaceCaptureConfig: WriteSurfaceCaptureConfig captureConfig err.");
        return false;
    }
    return true;
}

bool RSClientToRenderConnectionProxy::WriteSurfaceCaptureBlurParam(
    const RSSurfaceCaptureBlurParam& blurParam, MessageParcel& data)
{
    if (!data.WriteBool(blurParam.isNeedBlur) || !data.WriteFloat(blurParam.blurRadius)) {
        ROSEN_LOGE("WriteSurfaceCaptureBlurParam: WriteBool OR WriteFloat [blurParam] err.");
        return false;
    }
    return true;
}

bool RSClientToRenderConnectionProxy::WriteSurfaceCaptureAreaRect(
    const Drawing::Rect& specifiedAreaRect, MessageParcel& data)
{
    if (!data.WriteFloat(specifiedAreaRect.left_) || !data.WriteFloat(specifiedAreaRect.top_) ||
        !data.WriteFloat(specifiedAreaRect.right_) || !data.WriteFloat(specifiedAreaRect.bottom_)) {
        ROSEN_LOGE("WriteSurfaceCaptureAreaRect: WriteFloat specifiedAreaRect err.");
        return false;
    }
    return true;
}

ErrCode RSClientToRenderConnectionProxy::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetHwcNodeBounds: WriteInterfaceToken err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(rsNodeId)) {
        ROSEN_LOGE("SetHwcNodeBounds write id failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteFloat(positionX) || !data.WriteFloat(positionY) || !data.WriteFloat(positionZ) ||
        !data.WriteFloat(positionW)) {
        ROSEN_LOGE("SetHwcNodeBounds write bound failed");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_POINTER_POSITION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SetHwcNodeBounds SendRequest() error[%{public}d]", err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSClientToRenderConnectionProxy::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("GetBrightnessInfo: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(screenId)) {
        ROSEN_LOGE("GetBrightnessInfo: WriteUint64 screenId err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_BRIGHTNESS_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetBrightnessInfo Read result failed");
        return READ_PARCEL_ERR;
    }
    if (!ReadBrightnessInfo(brightnessInfo, reply)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ReadBrightnessInfo ReadBrightnessInfo failed!");
        return READ_PARCEL_ERR;
    }
    return result;
}

bool RSClientToRenderConnectionProxy::ReadBrightnessInfo(BrightnessInfo& brightnessInfo, MessageParcel& data)
{
    if (!data.ReadFloat(brightnessInfo.currentHeadroom) ||
        !data.ReadFloat(brightnessInfo.maxHeadroom) ||
        !data.ReadFloat(brightnessInfo.sdrNits)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ReadBrightnessInfo read parcel failed!");
        return false;
    }
    return true;
}

ErrCode RSClientToRenderConnectionProxy::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_SCREEN_HDR_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus SendRequest error(%{public}d)", err);
        return RS_CONNECTION_ERROR;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        uint32_t readHdrStatus{0};
        if (!reply.ReadUint32(readHdrStatus)) {
            ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus Read HDR status failed");
            return READ_PARCEL_ERR;
        }
        hdrStatus = static_cast<HdrStatus>(readHdrStatus);
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::DropFrameByPid(const std::vector<int32_t>& pidList, int32_t dropFrameLevel)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("DropFrameByPid: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    if (pidList.size() > MAX_DROP_FRAME_PID_LIST_SIZE || !data.WriteInt32Vector(pidList)) {
        ROSEN_LOGE("DropFrameByPid: WriteInt32Vector pidList err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInt32(dropFrameLevel)) {
        ROSEN_LOGE("DropFrameByPid: WriteInt32 dropFrameLevel err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::DROP_FRAME_BY_PID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::DropFrameByPid: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::SetAncoForceDoDirect(bool direct, bool& res)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("SetAncoForceDoDirect: WriteInterfaceToken GetDescriptor err.");
        res = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (data.WriteBool(direct)) {
        uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT);
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

ErrCode RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback(
    pid_t pid, uint64_t uid, sptr<RSISurfaceBufferCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback callback == nullptr");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: write Int32 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(uid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: write Uint64 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: write RemoteObject val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback: write Int32 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(uid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback: write Uint64 val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetLayerTopForHWC: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteUint64(nodeId) && data.WriteBool(isTop) && data.WriteUint32(zOrder)) {
        uint32_t code =
            static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSClientToRenderConnectionProxy::SetLayerTopForHWC: Send Request err.");
            return ERR_INVALID_VALUE;
        }
    }
    return ERR_OK;
}

void RSClientToRenderConnectionProxy::RegisterTransactionDataCallback(uint64_t token,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback callback == nullptr.");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteUint64(token)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: write multi token val err.");
        return;
    }
    if (!data.WriteUint64(timeStamp)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: write timeStamp val err.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: write Callback val err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK);
    RS_LOGD("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: timeStamp: %{public}"
        PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: Send Request err.");
        return;
    }
}

ErrCode RSClientToRenderConnectionProxy::SetWindowContainer(NodeId nodeId, bool value)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetWindowContainer: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(nodeId)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetWindowContainer: write Uint64 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(value)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetWindowContainer: write Bool val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_WINDOW_CONTAINER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetWindowContainer: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToRenderConnectionProxy::ClearUifirstCache(NodeId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearUifirstCache: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("ClearUifirstCache: WriteUint64 id err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_UIFIRST_CACHE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearUifirstCache sendrequest error : %{public}d", err);
        return;
    }
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSClientToRenderConnectionProxy::RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterCanvasCallback: WriteInterfaceToken GetDescriptor err.");
        return;
    }

    if (callback) {
        if (!data.WriteBool(true)) {
            ROSEN_LOGE("RegisterCanvasCallback: WriteBool[true] err");
            return;
        }
        if (!data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("RegisterCanvasCallback: WriteRemoteObject callback err");
            return;
        }
    } else if (!data.WriteBool(false)) {
        ROSEN_LOGE("RegisterCanvasCallback: WriteBool[false] err.");
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_CANVAS_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RegisterCanvasCallback: Send Request err, errCode=%{public}d", err);
    }
}

int32_t RSClientToRenderConnectionProxy::SubmitCanvasPreAllocatedBuffer(
    NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("SubmitCanvasPreAllocatedBuffer: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }

    if (!data.WriteUint64(nodeId)) {
        ROSEN_LOGE("SubmitCanvasPreAllocatedBuffer: WriteUint64 nodeId err.");
        return WRITE_PARCEL_ERR;
    }

    if (!data.WriteUint32(resetSurfaceIndex)) {
        ROSEN_LOGE("SubmitCanvasPreAllocatedBuffer: WriteUint32 resetSurfaceIndex err.");
        return WRITE_PARCEL_ERR;
    }

    GSError gsRet = WriteSurfaceBufferImpl(data, 0, buffer);
    if (gsRet != GSERROR_OK) {
        ROSEN_LOGE("SubmitCanvasPreAllocatedBuffer: WriteToMessageParcel err, ret=%{public}d.", gsRet);
        return WRITE_PARCEL_ERR;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SubmitCanvasPreAllocatedBuffer: Send Request err.");
        return RS_CONNECTION_ERROR;
    }

    int32_t result = 0;
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("SubmitCanvasPreAllocatedBuffer: Read result failed");
        return READ_PARCEL_ERR;
    }

    return result;
}
#endif // ROSEN_OHOS && RS_ENABLE_VK

uint32_t RSClientToRenderConnectionProxy::SetSurfaceWatermark(pid_t pid, const std::string &name,
    const std::shared_ptr<Media::PixelMap> &watermark,
    const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark: WriteInterfaceToken GetDescriptor err.");
        return WATER_MARK_WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark: WriteInt32 pid err.");
        return WATER_MARK_WRITE_PARCEL_ERR;
    }
    if (!data.WriteString(name)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark: WriteString name err.");
        return WATER_MARK_WRITE_PARCEL_ERR;
    }

    if (watermark != nullptr) {
        if (!data.WriteBool(true)) {
            ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark: pixelMap is not nullptr flag err.");
            return WATER_MARK_WRITE_PARCEL_ERR;
        }
        if (!data.WriteParcelable(watermark.get())) {
            ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark: WriteParcelable watermark.get() err.");
            return WATER_MARK_WRITE_PARCEL_ERR;
        }
    } else if (!data.WriteBool(false)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark: pixelMap is not nullptr flag err.");
        return WATER_MARK_WRITE_PARCEL_ERR;
    }

    if (!data.WriteUInt64Vector(nodeIdList)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark: write nodeIdList error.");
        return WATER_MARK_WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(watermarkType))) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark: write watermarkType error.");
        return WATER_MARK_WRITE_PARCEL_ERR;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SURFACE_WATERMARK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark: Send Request err.");
        return WATER_MARK_IPC_ERROR;
    }
    uint32_t status{0};
    if (!reply.ReadUint32(status)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetSurfaceWatermark Read status failed");
        return WATER_MARK_READ_PARCEL_ERR;
    }
    return status;
}

void RSClientToRenderConnectionProxy::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string& name,
    const std::vector<NodeId> &nodeIdList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("ClearSurfaceWatermarkForNodes: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearSurfaceWatermarkForNodes: WriteInt32 pid err.");
        return;
    }
    if (!data.WriteString(name)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearSurfaceWatermarkForNodes name err.");
        return;
    }
    if (!data.WriteUInt64Vector(nodeIdList)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearSurfaceWatermarkForNodes: write nodeIdList error.");
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearSurfaceWatermarkForNodes: Send Request err.");
        return;
    }
}
    
void RSClientToRenderConnectionProxy::ClearSurfaceWatermark(pid_t pid, const std::string &name)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearSurfaceWatermark: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearSurfaceWatermark: WriteInt32 pid err.");
        return;
    }
    if (!data.WriteString(name)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearSurfaceWatermark name err.");
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearSurfaceWatermark: Send Request err.");
        return;
    }
}

ErrCode RSClientToRenderConnectionProxy::RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterOcclusionChangeCallback: callback is nullptr.");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterOcclusionChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterOcclusionChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSClientToRenderConnectionProxy::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceOcclusionChangeCallback: callback is nullptr.");
        return INVALID_ARGUMENTS;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
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
        RSIClientToRenderConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceOcclusionChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToRenderConnectionProxy::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("UnRegisterSurfaceOcclusionChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("UnRegisterSurfaceOcclusionChangeCallback: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::UnRegisterSurfaceOcclusionChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToRenderConnectionProxy::SetLogicalCameraRotationCorrection(
    ScreenId id, ScreenRotation logicalCorrection)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("SetLogicalCameraRotationCorrection: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("SetLogicalCameraRotationCorrection: WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(logicalCorrection))) {
        ROSEN_LOGE("SetLogicalCameraRotationCorrection: WriteUint32 logicalCorrection err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetLogicalCameraRotationCorrection Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSClientToRenderConnectionProxy::RegisterFrameStabilityDetection(
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config,
    sptr<RSIFrameStabilityCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s: callback is nullptr.", __func__);
        return INVALID_ARGUMENTS;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint64(target.id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 target.id err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(target.type))) {
        ROSEN_LOGE("%{public}s: WriteUint32 target.type err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(config.stableDuration)) {
        ROSEN_LOGE("%{public}s: WriteUint32 stableDuration err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteFloat(config.changePercent)) {
        ROSEN_LOGE("%{public}s: WriteFloat changePercent err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("%{public}s: WriteRemoteObject callback->AsObject() err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_FRAME_STABILITY_DETECTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest err: %{public}d", __func__, err);
        return RS_CONNECTION_ERROR;
    }
    return SUCCESS;
}

int32_t RSClientToRenderConnectionProxy::UnregisterFrameStabilityDetection(const FrameStabilityTarget& target)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteUint64(target.id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 target.id err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(target.type))) {
        ROSEN_LOGE("%{public}s: WriteUint32 target.type err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::UNREGISTER_FRAME_STABILITY_DETECTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest err: %{public}d", __func__, err);
        return RS_CONNECTION_ERROR;
    }
    return SUCCESS;
}

int32_t RSClientToRenderConnectionProxy::StartFrameStabilityCollection(
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteUint64(target.id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 target.id err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(target.type))) {
        ROSEN_LOGE("%{public}s: WriteUint32 target.type err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(config.stableDuration)) {
        ROSEN_LOGE("%{public}s: WriteUint32 stableDuration err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteFloat(config.changePercent)) {
        ROSEN_LOGE("%{public}s: WriteFloat changePercent err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::START_FRAME_STABILITY_COLLECTION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest err: %{public}d", __func__, err);
        return RS_CONNECTION_ERROR;
    }
    return SUCCESS;
}

int32_t RSClientToRenderConnectionProxy::GetFrameStabilityResult(
    const FrameStabilityTarget& target,
    bool& result)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteUint64(target.id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 target.id err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(target.type))) {
        ROSEN_LOGE("%{public}s: WriteUint32 target.type err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_FRAME_STABILITY_RESULT);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest err: %{public}d", __func__, err);
        return RS_CONNECTION_ERROR;
    }
    int32_t repCode = 0;
    if (!reply.ReadInt32(repCode)) {
        ROSEN_LOGE("%{public}s: ReadInt32 repCode failed", __func__);
        return READ_PARCEL_ERR;
    }
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("%{public}s: ReadBool result failed", __func__);
        return READ_PARCEL_ERR;
    }
    return repCode;
}
} // namespace Rosen
} // namespace OHOS
