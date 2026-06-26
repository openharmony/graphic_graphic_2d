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

#include "rs_delegate_composite_callback_stub.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
namespace OHOS {
namespace Rosen {

#ifndef ROSEN_CROSS_PLATFORM
static void RsDelegateCompositeCallbackPrintLog(const std::string &log)
{
    ROSEN_LOGE("DelegateModeDebugTag: %{public}s:%{public}s", __func__, log.c_str());
    RS_OPTIONAL_TRACE_NAME_FMT("DelegateModeDebugTag: %s:%s", __func__, log.c_str());
}
#endif

int32_t RSWebProxyComposerCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& arguments,
    MessageParcel& reply, MessageOption& option)
{
#ifndef ROSEN_CROSS_PLATFORM
    RS_OPTIONAL_TRACE_NAME("RSWebProxyComposerCallbackStub OnRemoteRequest");
    (void)(option);

    auto remoteDescriptor = arguments.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        RsDelegateCompositeCallbackPrintLog("RSWebProxyComposerCallbackStub fail");
        return ERR_INVALID_STATE;
    }
    auto ret = ERR_UNKNOWN_REASON;
    switch (code) {
        case static_cast<uint32_t>(RSIWebProxyComposerCallbackInterfaceCode::ON_COMPLETE): {
            uint64_t time = 0;
            uint64_t srcId = 0;
            uint64_t seqNum = 0;
            uint32_t size = 0;
            std::queue<uint64_t> queue;
            if (!arguments.ReadUint64(time) ||!arguments.ReadUint64(srcId) ||!arguments.ReadUint32(size)) {
                RsDelegateCompositeCallbackPrintLog("read time/srcId/size fail");
                break;
            }

            RS_OPTIONAL_TRACE_NAME_FMT(
                "RSWebProxyComposerCallbackStub ON_COMPLETE:%llu, srcId=%llu, queueSize=%u",
                time, srcId, size);
            for (uint32_t i = 0; i < size; i++) {
                if (!arguments.ReadUint64(seqNum)) {
                    RsDelegateCompositeCallbackPrintLog("read seqNum fail");
                    break;
                }
                RS_OPTIONAL_TRACE_NAME_FMT("seqNums=%llu", seqNum);
                queue.push(seqNum);
            }
            OnComplete(time, srcId, queue);
            ret = ERR_NONE;
            break;
        }
        default: {
            RS_TRACE_NAME("RSWebProxyComposerCallbackStub default");
            break;
        }
    }
    return ret;
#else
    return GSERROR_INVALID_OPERATING;
#endif
}

GSError RSWebProxyComposerCallbackStub::OnComplete(uint64_t timestamp, uint64_t srcId, std::queue<uint64_t>& seqNums)
{
#ifndef ROSEN_CROSS_PLATFORM
    std::lock_guard<std::mutex> lock(mutex_);
    if (onCompleteCallback_) {
        onCompleteCallback_(timestamp, srcId, seqNums);
    }
    return GSERROR_OK;
#else
    return GSERROR_INVALID_OPERATING;
#endif
}

void RSWebProxyComposerCallbackStub::RegisterOnCompleteCallBack(OnCompleteCallback cb)
{
#ifndef ROSEN_CROSS_PLATFORM
    std::lock_guard<std::mutex> lock(mutex_);
    onCompleteCallback_ = cb;
#endif
}

void RSWebProxyComposerCallbackStub::UnRegisterOnCompleteCallBack()
{
#ifndef ROSEN_CROSS_PLATFORM
    std::lock_guard<std::mutex> lock(mutex_);
    onCompleteCallback_ = nullptr;
#endif
}

int32_t SurfaceNodeBufferReleaseCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& arguments,
    MessageParcel& reply, MessageOption& option)
{
#ifndef ROSEN_CROSS_PLATFORM
    (void)(option);
    auto remoteDescriptor = arguments.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    auto ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSISurfaceNodeBufferReleaseInterfaceCode::ON_BUFFER_COMPLETE): {
            uint32_t size = 0;
            if (!arguments.ReadUint32(size)) {
                RsDelegateCompositeCallbackPrintLog("read size fail");
                break;
            }
            std::queue<OnCompletedRet> queue;
            for (uint32_t i = 0; i < size; i++) {
                OnCompletedRet onCompletedRet;
                if (!arguments.ReadUint64(onCompletedRet.nodeId) ||!arguments.ReadUint64(onCompletedRet.queueId) ||
                    !arguments.ReadUint32(onCompletedRet.bufferSeqNum)) {
                    RsDelegateCompositeCallbackPrintLog("read onCompletedRet fail");
                    break;
                }
                RS_TRACE_NAME_FMT("SurfaceNodeBufferReleaseCallbackStub: nodeId=%llu, queueId=%llu "
                    "bufferSeqNum=%u, clientPid=%u", onCompletedRet.nodeId, onCompletedRet.queueId,
                    onCompletedRet.bufferSeqNum, onCompletedRet.clientPid);
#ifdef ROSEN_OHOS
                onCompletedRet.releaseFence = SyncFence::ReadFromMessageParcel(arguments);
#endif
                if (!onCompletedRet.releaseFence) {
                    RsDelegateCompositeCallbackPrintLog("read releaseFence fail");
                    break;
                }
                if (!arguments.ReadBool(onCompletedRet.isActiveGame)) {
                    RsDelegateCompositeCallbackPrintLog("read isActiveGame fail");
                    break;
                }
                queue.push(std::move(onCompletedRet));
            }
            OnBufferComplete(queue);
            break;
        }
        default: {
            ret = ERR_UNKNOWN_TRANSACTION;
            break;
        }
    }
    return ret;
#else
    return GSERROR_INVALID_OPERATING;
#endif
}

void SurfaceNodeBufferReleaseCallbackStub::RegisterReleaseBufferCallBack(OnCompleteCallback cb)
{
#ifndef ROSEN_CROSS_PLATFORM
    std::lock_guard<std::mutex> lock(mutex_);
    onCompleteCallback_ = cb;
#endif
}

void SurfaceNodeBufferReleaseCallbackStub::UnRegisterReleaseBufferCallBack()
{
#ifndef ROSEN_CROSS_PLATFORM
    std::lock_guard<std::mutex> lock(mutex_);
    onCompleteCallback_ = nullptr;
#endif
}

GSError SurfaceNodeBufferReleaseCallbackStub::OnBufferComplete(std::queue<OnCompletedRet> &queue)
{
#ifndef ROSEN_CROSS_PLATFORM
    std::lock_guard<std::mutex> lock(mutex_);
    if (onCompleteCallback_) {
        onCompleteCallback_(queue);
    }
    return GSERROR_OK;
#else
    return GSERROR_INVALID_OPERATING;
#endif
}
} // namespace Rosen
} // namespace OHOS