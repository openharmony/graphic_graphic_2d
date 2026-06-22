/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_delegate_composite_callback_proxy.h"
#include <message_option.h>
#include <message_parcel.h>
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
namespace OHOS {
namespace Rosen {

#ifndef ROSEN_CROSS_PLATFORM
static void RsDelegateCompositeCallbackProxyPrintLog(const std::string &log)
{
    ROSEN_LOGE("DelegateModeDebugTag: %{public}s:%{public}s", __func__, log.c_str());
    RS_OPTIONAL_TRACE_NAME_FMT("DelegateModeDebugTag: %s:%s", __func__, log.c_str());
}
#endif

GSError RSWebProxyComposerCallbackProxy::OnComplete(uint64_t timestamp, uint64_t srcId, std::queue<uint64_t> &seqNums)
{
#ifndef ROSEN_CROSS_PLATFORM
    uint32_t size = seqNums.size();
    MessageOption option;
    MessageParcel arguments;
    MessageParcel reply;

    if (!arguments.WriteInterfaceToken(RSISurfaceTransactionListener::GetDescriptor())) {
        RsDelegateCompositeCallbackProxyPrintLog("OnComplete::WriteInterfaceToken fail");
        return GSERROR_BINDER;
    }

    if (!arguments.WriteUint64(timestamp) || !arguments.WriteUint64(srcId) || !arguments.WriteUint32(size)) {
        RsDelegateCompositeCallbackProxyPrintLog("OnComplete::Write timestamp/srcId/size fail");
        return GSERROR_BINDER;
    }

    RS_OPTIONAL_TRACE_NAME_FMT("RSWebProxyComposerCallbackProxy OnComplete: %llu, srcId=%llu, queueSize=%u",
        timestamp, srcId, size);
    while (!seqNums.empty()) {
        RS_OPTIONAL_TRACE_NAME_FMT("seqNum=%llu", seqNums.front());
        if (!arguments.WriteUint64(seqNums.front())) {
            RsDelegateCompositeCallbackProxyPrintLog("OnComplete::write seqnum fail");
        }
        seqNums.pop();
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIWebProxyComposerCallbackInterfaceCode::ON_COMPLETE);
    int32_t ret = Remote()->SendRequest(code, arguments, reply, option);
    if (ret != ERR_NONE) {
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
#else
    return GSERROR_INVALID_OPERATING;
#endif
}

GSError SurfaceNodeBufferReleaseCallbackProxy::OnBufferComplete(std::queue<OnCompletedRet> &queue)
{
#ifndef ROSEN_CROSS_PLATFORM
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t size = queue.size();
    if (!data.WriteInterfaceToken(RSISurfaceNodeBufferReleaseCallback::GetDescriptor())) {
        RsDelegateCompositeCallbackProxyPrintLog("OnBufferComplete::WriteInterfaceToken failed");
        return GSERROR_BINDER;
    }
    if (!data.WriteUint32(size)) {
        RsDelegateCompositeCallbackProxyPrintLog("OnBufferComplete::Write size failed");
        return GSERROR_BINDER;
    }
    while (!queue.empty()) {
        OnCompletedRet ret = queue.front();
        queue.pop();
        RS_OPTIONAL_TRACE_NAME_FMT(
            "SurfaceNodeBufferReleaseCallbackProxy nodeId=%llu, queueId=%llu, bufferSeqNum=%u, clientPid=%u",
            ret.nodeId, ret.queueId, ret.bufferSeqNum, ret.clientPid);
        if (!data.WriteUint64(ret.nodeId) || !data.WriteUint64(ret.queueId) || !data.WriteUint32(ret.bufferSeqNum) ||
#ifdef ROSEN_OHOS
            !ret.releaseFence->WriteToMessageParcel(data) ||
#endif
            !data.WriteBool(ret.isActiveGame)) {
            RsDelegateCompositeCallbackProxyPrintLog("OnBufferComplete::Write fail");
            return GSERROR_BINDER;
        }
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSISurfaceNodeBufferReleaseInterfaceCode::ON_BUFFER_COMPLETE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
#else
    return GSERROR_INVALID_OPERATING;
#endif
}
} // namespace Rosen
} // namespace OHOS