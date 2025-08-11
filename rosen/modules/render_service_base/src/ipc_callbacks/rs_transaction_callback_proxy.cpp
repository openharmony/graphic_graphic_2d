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
#include "rs_transaction_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "graphic_common.h"

#include "platform/common/rs_log.h"
namespace OHOS {
namespace Rosen {
RSTransactionCallbackProxy::RSTransactionCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSITransactionCallback>(impl)
{
}

GSError RSTransactionCallbackProxy::OnCompleted(const OnCompletedRet& ret)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor())) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnCompleted writeInterfaceToken error");
        return GSERROR_BINDER;
    }
    if (!data.WriteUint64(ret.queueId)) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnCompleted write queueId error");
        return GSERROR_BINDER;
    }
    if (!data.WriteUint32(ret.bufferSeqNum)) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnCompleted write bufferSeqNum error");
        return GSERROR_BINDER;
    }
    if (!ret.releaseFence->WriteToMessageParcel(data)) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnCompleted write releaseFence error");
        return GSERROR_BINDER;
    }
    if (!data.WriteBool(ret.isActiveGame)) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnCompleted write isActiveGame error");
        return GSERROR_BINDER;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_COMPLETED);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnCompleted SendRequest() error");
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}

GSError RSTransactionCallbackProxy::GoBackground(uint64_t queueId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor())) {
        ROSEN_LOGE("RSTransactionCallbackProxy: GoBackground writeInterfaceToken error");
        return GSERROR_BINDER;
    }
    if (!data.WriteUint64(queueId)) {
        ROSEN_LOGE("RSTransactionCallbackProxy: GoBackground write queueId error");
        return GSERROR_BINDER;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::GO_BACKGROUND);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSTransactionCallbackProxy: GoBackground SendRequest() error");
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}

GSError RSTransactionCallbackProxy::OnDropBuffers(const OnDropBuffersRet& ret)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor())) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnDropBuffers writeInterfaceToken error");
        return GSERROR_BINDER;
    }
    if (!data.WriteUint64(ret.queueId)) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnDropBuffers write queueId error");
        return GSERROR_BINDER;
    }
    if (!data.WriteUint32(ret.bufferSeqNums.size())) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnDropBuffers write bufferSeqNums error");
        return GSERROR_BINDER;
    }
    for (auto& bufferSeqNum : ret.bufferSeqNums) {
        if (!data.WriteUint32(bufferSeqNum)) {
            ROSEN_LOGE("RSTransactionCallbackProxy: OnDropBuffers write bufferSeqNum error");
            return GSERROR_BINDER;
        }
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_DROPBUFFERS);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnDropBuffers SendRequest() error");
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}

GSError RSTransactionCallbackProxy::OnSurfaceDump(OnSurfaceDumpRet& ret)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor())) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnSurfaceDump writeInterfaceToken error");
        return GSERROR_BINDER;
    }
    if (!data.WriteUint64(ret.queueId)) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnSurfaceDump write queueId error");
        return GSERROR_BINDER;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_SURFACE_DUMP);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnSurfaceDump SendRequest() error");
        return GSERROR_BINDER;
    }
    int32_t replyRet = GSERROR_BINDER;
    if (!reply.ReadInt32(replyRet)) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnSurfaceDump read Int32 error");
        return GSERROR_BINDER;
    } else if (replyRet != GSERROR_OK) {
        ROSEN_LOGE("RSTransactionCallbackProxy: OnSurfaceDump fail ret:%{public}d", replyRet);
        return static_cast<GSError>(replyRet);
    }
    if (!reply.ReadString(ret.dumpString)) {
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}

GSError RSTransactionCallbackProxy::SetDefaultWidthAndHeight(const OnSetDefaultWidthAndHeightRet& ret)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor())) {
        ROSEN_LOGE("RSTransactionCallbackProxy: SetDefaultWidthAndHeight writeInterfaceToken error");
        return GSERROR_BINDER;
    }
    if (!data.WriteUint64(ret.queueId) || !data.WriteUint32(ret.width) || !data.WriteUint32(ret.height)) {
        ROSEN_LOGE("RSTransactionCallbackProxy: SetDefaultWidthAndHeight write queueId or width or height error");
        return GSERROR_BINDER;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::SET_DEFAULT_WIDTH_AND_HEIGHT);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSTransactionCallbackProxy: SetDefaultWidthAndHeight SendRequest() error");
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}
} // namespace Rosen
} // namespace OHOS