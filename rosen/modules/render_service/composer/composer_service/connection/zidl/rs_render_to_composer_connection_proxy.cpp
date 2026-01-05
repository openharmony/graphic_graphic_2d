/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "rs_render_to_composer_connection_proxy.h"
#include <memory>
#include <vector>
#include "buffer_utils.h"
#include "graphic_common.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_ashmem_helper.h"

namespace OHOS {
namespace Rosen {
// static constexpr int32_t MAX_RETRY = 3;
static constexpr int32_t MAX_RETRY_COUNT = 20;
// static constexpr int32_t RETRY_INTERVAL = 1000; // 1000us = 1ms
static constexpr int32_t RETRY_WAIT_TIME_US = 1000; // wait 1ms before retry SendRequest

RSRenderToComposerConnectionProxy::RSRenderToComposerConnectionProxy(const sptr<IRemoteObject>& impl) :
    IRemoteProxy<IRSRenderToComposerConnection>(impl) {}

bool RSRenderToComposerConnectionProxy::CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData)
{
    if (transactionData == nullptr) {
        RS_LOGE("RSRenderToComposerConnectionProxy::CommitLayers transactionData nullptr");
        return false;
    }
    transactionData->SetSendingPid(pid_);

    std::vector<std::shared_ptr<MessageParcel>> parcelVector;
    auto fillFunc = [&parcelVector, &transactionData, this]() -> bool {
        ++transactionDataIndex_;
        transactionData->SetIndex(transactionDataIndex_);
        std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
        if (!FillParcelWithTransactionData(transactionData, parcel)) {
            RS_LOGE("FillParcelWithTransactionData failed");
            return false;
        }
        parcelVector.emplace_back(parcel);
        return true;
    };
    while (transactionData->GetMarshallingIndex() < transactionData->GetCommandCount()) {
        if (!fillFunc()) {
            return false;
        }
    }
    return SendLayers(parcelVector) == COMPOSITOR_ERROR_OK;
}

bool RSRenderToComposerConnectionProxy::FillParcelWithTransactionData(
    std::unique_ptr<RSLayerTransactionData>& transactionData, std::shared_ptr<MessageParcel>& data)
{
    if (!data->WriteInterfaceToken(GetDescriptor())) {
        RS_LOGE("FillParcelWithTransactionData WriteInterfaceToken failed");
        return false;
    }
    RS_TRACE_NAME_FMT("MarshRSLayerTransactionData cmdCount: %lu, transactionFlag:[%d, %" PRIu64 "], "
        "timestamp:%ld", transactionData->GetCommandCount(), pid_, transactionData->GetIndex(),
        transactionData->GetTimestamp());
    bool success = transactionData->Marshalling(data);
    if (!success) {
        RS_LOGE("TransactionData marshalling failed");
        return false;
    }
    return true;
}

RSComposerError RSRenderToComposerConnectionProxy::SendLayers(std::vector<std::shared_ptr<MessageParcel>>& parcels)
{
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    for (const auto& parcel : parcels) {
        MessageParcel reply;
        int32_t retryCount = 0;
        int32_t err = NO_ERROR;
        do {
            err = Remote()->SendRequest(IRENDER_TO_COMPOSER_CONNECTION_COMMIT_LAYERS, *parcel, reply, option);
            if (err != NO_ERROR && retryCount < MAX_RETRY_COUNT) {
                retryCount++;
                usleep(RETRY_WAIT_TIME_US);
            } else if (err != NO_ERROR) {
                RS_LOGE("SendLayers SendRequest failed, err:%{public}d, data size:%{public}zu", err, parcel->GetDataSize());
                return COMPOSITOR_ERROR_BINDER_ERROR;
            }
        } while (err != NO_ERROR);
        int32_t serverRet = reply.ReadInt32();
        if (serverRet != COMPOSITOR_ERROR_OK) {
            RS_LOGE("SendLayers server returned error:%{public}d", serverRet);
            return static_cast<RSComposerError>(serverRet);
        }
    }
    RS_LOGI("SendLayers success.");
    return COMPOSITOR_ERROR_OK;
}

void RSRenderToComposerConnectionProxy::ClearFrameBuffers()
{
    MessageOption option;
    MessageParcel reply;
    MessageParcel parcel;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!parcel.WriteInterfaceToken(GetDescriptor())) {
        RS_LOGE("ClearFrameBuffers WriteInterfaceToken failed");
        return;
    }
    SendRequest(IRENDER_TO_COMPOSER_CONNECTION_CLEAR_FRAME_BUFFERS, parcel, reply, option);
}

RSComposerError RSRenderToComposerConnectionProxy::SendRequest(uint32_t command, MessageParcel &arg,
    MessageParcel &reply, MessageOption &opt)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        RS_LOGE("Remote is nullptr.");
        return COMPOSITOR_ERROR_BINDER_ERROR;
    }
    int32_t ret = remote->SendRequest(command, arg, reply, opt);
    if (ret != ERR_NONE) {
        RS_LOGE("SendRequest ret: %{public}d.", ret);
        return COMPOSITOR_ERROR_BINDER_ERROR;
    }
    return COMPOSITOR_ERROR_OK;
}

void RSRenderToComposerConnectionProxy::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    MessageOption option;
    MessageParcel reply;
    MessageParcel parcel;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!parcel.WriteInterfaceToken(GetDescriptor())) {
        RS_LOGE("CleanLayerBufferBySurfaceId WriteInterfaceToken failed");
        return;
    }
    if (!parcel.WriteUint64(surfaceId)) {
        RS_LOGE("CleanLayerBufferBySurfaceId WriteInt64 failed");
        return;
    }
    SendRequest(IRENDER_TO_COMPOSER_CONNECTION_CLEAN_LAYER_BUFFER_BY_SURFACE_ID, parcel, reply, option);
}

void RSRenderToComposerConnectionProxy::ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds)
{
    MessageOption option;
    MessageParcel reply;
    MessageParcel parcel;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!parcel.WriteInterfaceToken(GetDescriptor())) {
        RS_LOGE("ClearRedrawGPUCompositionCache WriteInterfaceToken failed");
        return;
    }
    std::vector<uint64_t> bufferIdsVector(bufferIds.begin(), bufferIds.end());
    if (!parcel.WriteUInt64Vector(bufferIdsVector)) {
        RS_LOGE("ClearRedrawGPUCompositionCache WriteUInt64 failed");
        return;
    }
    int32_t err = 0;
    int32_t retryCount = 0;
    do {
        err = SendRequest(IRENDER_TO_COMPOSER_CONNECTION_CLEAR_REDRAW_GPU_COMPOSITION_CACHE, parcel, reply, option);
        if (err != NO_ERROR && retryCount < MAX_RETRY_COUNT) {
            retryCount++;
            usleep(RETRY_WAIT_TIME_US);
        } else if (err != NO_ERROR) {
            RS_LOGE("ClearRedrawGPUCompositionCache SendRequest failed, err:%{public}d", err);
            break;
        }
    } while (err != NO_ERROR);
}

void RSRenderToComposerConnectionProxy::SetScreenBacklight(uint32_t level)
{
    MessageOption option;
    MessageParcel reply;
    MessageParcel parcel;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!parcel.WriteInterfaceToken(GetDescriptor())) {
        RS_LOGE("SetScreenBacklight WriteInterfaceToken failed");
        return;
    }
    if (!parcel.WriteUint32(level)) {
        RS_LOGE("SetScreenBacklight WriteUint32 failed");
        return;
    }
    SendRequest(IRENDER_TO_COMPOSER_CONNECTION_SET_BACKLIGHT_LEVEL, parcel, reply, option);
}

void RSRenderToComposerConnectionProxy::SetComposerToRenderConnection(
    const sptr<IRSComposerToRenderConnection>& composerToRenderConn)
{
    MessageOption option;
    MessageParcel reply;
    MessageParcel parcel;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!parcel.WriteInterfaceToken(GetDescriptor())) {
        RS_LOGE("SetComposerToRenderConnection WriteInterfaceToken failed");
        return;
    }
    if (!parcel.WriteRemoteObject(composerToRenderConn->AsObject())) {
        RS_LOGE("SetComposerToRenderConnection object failed");
        return;
    }
    SendRequest(IRENDER_TO_COMPOSER_CONNECTION_SET_COMPOSER_TO_RENDER_CONNECTION, parcel, reply, option);
}

void RSRenderToComposerConnectionProxy::PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer)
{
    MessageOption option;
    MessageParcel reply;
    MessageParcel parcel;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!parcel.WriteInterfaceToken(GetDescriptor())) {
        RS_LOGE("PreAllocProtectedFrameBuffers WriteInterfaceToken failed");
        return;
    }
    uint32_t sequence = buffer->GetSeqNum();
    GSError ret = WriteSurfaceBufferImpl(parcel, sequence, buffer);
    if (ret != GSERROR_OK) {
        RS_LOGE("PreAllocProtectedFrameBuffers WriteToMessageParcel err, ret = %{public}d", ret);
        return;
    }
    SendRequest(IRENDER_TO_COMPOSER_CONNECTION_PREALLOC_PROTECTED_FRAME_BUFFERS, parcel, reply, option);
}
} // namespace Rosen
} // namespace OHOS