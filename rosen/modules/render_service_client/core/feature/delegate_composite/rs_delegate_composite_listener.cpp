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
#ifndef ROSEN_CROSS_PLATFORM
#include "command/rs_command.h"
#include "rs_delegate_composite_listener.h"
#include "ipc_callbacks/rs_delegate_composite_callback_stub.h"
#include "rs_trace.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "transaction/rs_render_pipeline_client.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_ui_context_manager.h"
#include "command/rs_delegate_composite_command.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

static std::mutex g_seqNumMutex;
static uint64_t CreateUniqueId()
{
    std::lock_guard<std::mutex> lock(g_seqNumMutex);
    static uint64_t nextId = 0;
    static constexpr uint64_t NEXTID_MASK_48BIT = 0xFFFFFFFFFFFF;
    nextId++;
    // 0xFFFF is pid mask. 48 is pid offset.bufferId_ high 16bit is pid, low 48bit is Auto-increment id
    uint64_t uniqueId = ((static_cast<uint64_t>(getpid()) & 0xFFFF) << 48);
    uniqueId |= (nextId & NEXTID_MASK_48BIT);
    return uniqueId;
}

std::unique_ptr<OHOS::Rosen::RSCommand> SurfaceTransactionListener::GetCommand(uint64_t& cmdSeqNum)
{
#ifndef ROSEN_CROSS_PLATFORM
    std::lock_guard<std::mutex> lock(mutex_);
    if (workTid_ != gettid()) {
        return nullptr;
    }
    cmdSeqNum = CommandSeqNum_++;
    RS_TRACE_NAME_FMT("SurfaceTransactionCommand GetCommand: uniqueId=%llu, commandSeqNum=%llu",
        uniqueId_, cmdSeqNum);
    return std::make_unique<SurfaceTransactionCommand>(uniqueId_, cmdSeqNum, workPid_, workTid_);
#else
    return nullptr;
#endif
}

SurfaceTransactionListener::SurfaceTransactionListener(sptr<IRemoteObject> connectToRender)
{
    workTid_ = gettid();
    workPid_ = getpid();
    uniqueId_ = CreateUniqueId();
    connectToRender_ = connectToRender;
}

SurfaceTransactionListener::~SurfaceTransactionListener()
{
    UnRegisterCommandCompleteCallBack();
}

uint64_t SurfaceTransactionListener::GetUniqueId()
{
    return uniqueId_;
}

bool SurfaceTransactionListener::RegisterCommandCompleteCallBack(OnCompleteCallback callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (callBackStub_ != nullptr || connectToRender_ == nullptr) {
        RS_LOGE("DelegateModeDebugTag: RegisterCommandCompleteCallBack fail, repeat register");
        return false;
    }

    sptr<RSWebProxyComposerCallbackStub> callbackStubImpl = new RSWebProxyComposerCallbackStub();
    callbackStubImpl->RegisterOnCompleteCallBack(callback);
    RS_TRACE_NAME_FMT("SurfaceTransactionListener uniqueId=%llu", uniqueId_);
    auto renderPipelineClient = std::make_shared<RSRenderPipelineClient>(connectToRender_);
    if (renderPipelineClient == nullptr) {
        RS_LOGE("DelegateModeDebugTag: RegisterCommandCompleteCallBack fail, renderPipelineClient == nullptr");
        return false;
    }

    if (!renderPipelineClient->RegisterSurfaceTransactionListener(callbackStubImpl, uniqueId_)) {
        RS_LOGE("DelegateModeDebugTag: RegisterCommandCompleteCallBack fail, renderPipelineClient return false");
        return false;
    }
    renderPipelineClient_ = renderPipelineClient;
    callBackStub_ = callbackStubImpl;
    return true;
}

bool SurfaceTransactionListener::UnRegisterCommandCompleteCallBack()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callBackStub_ || !renderPipelineClient_) {
        RS_LOGE("DelegateModeDebugTag: UnRegisterCommandCompleteCallBack fail. invalid params");
        return false;
    }

    RS_TRACE_NAME_FMT("UnRegisterSurfaceTransactionListener: pid=%u, uniqueId_=%llu", workPid_, uniqueId_);
    if (!renderPipelineClient_->UnRegisterSurfaceTransactionListener(uniqueId_)) {
        RS_LOGE("DelegateModeDebugTag: UnRegisterCommandCompleteCallBack fail. renderPipelineClient return false");
        return false;
    }
    callBackStub_ = nullptr;
    renderPipelineClient_ = nullptr;
    return true;
}

SurfaceNodeBufferReleaseListener& SurfaceNodeBufferReleaseListener::GetInstance()
{
    static SurfaceNodeBufferReleaseListener instance;
    return instance;
}

SurfaceNodeBufferReleaseListener::SurfaceNodeBufferReleaseListener()
{
    uniqueId_ = CreateUniqueId();
}

bool SurfaceNodeBufferReleaseListener::RegisterReleaseBufferCallBack(OnBufferCompleteCallback cb)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (callBackStub_ != nullptr || connectToRender_ == nullptr) {
            RS_LOGE("DelegateModeDebugTag: RegisterReleaseBufferCallBack fail, same repeat register");
            return false;
        }

        RS_TRACE_NAME("RegisterReleaseBufferCallBack");

        sptr<SurfaceNodeBufferReleaseCallbackStub> callbackStubImpl = new SurfaceNodeBufferReleaseCallbackStub();
        callbackStubImpl->RegisterReleaseBufferCallBack(cb);
        RS_TRACE_NAME_FMT("SurfaceTransactionListener uniqueId=%llu", uniqueId_);
        auto renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
        if (renderPipelineClient == nullptr) {
            RS_LOGE("DelegateModeDebugTag: RegisterReleaseBufferCallBack fail, renderPipelineClient == nullptr");
            return false;
        }

        if (!renderPipelineClient->RegisterSurfaceNodeBufferReleaseListener(callbackStubImpl)) {
            RS_LOGE("DelegateModeDebugTag: RegisterSurfaceNodeBufferReleaseListener fail, renderPipelineClient fail");
            return false;
        }
        callBackStub_ = callbackStubImpl;
        renderPipelineClient_ = renderPipelineClient;
    }
    return true;
}

bool SurfaceNodeBufferReleaseListener::UnRegisterReleaseBufferCallBack()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (renderPipelineClient_ == nullptr || callBackStub_ == nullptr) {
            RS_LOGE("DelegateModeDebugTag: UnRegisterReleaseBufferCallBack fail. invalid params");
            return false;
        }

        RS_TRACE_NAME_FMT("UnRegisterReleaseBufferCallBack: pid=%u, uniqueId_=%llu", getpid(), uniqueId_);
        if (!renderPipelineClient_->UnRegisterSurfaceNodeBufferReleaseListener()) {
            RS_LOGE("DelegateModeDebugTag: UnRegisterReleaseBufferCallBack fail. renderPipelineClient_ return false");
            return false;
        }
        callBackStub_ = nullptr;
        renderPipelineClient_ = nullptr;
    }

    return true;
}

bool SurfaceNodeBufferReleaseListener::SetConnectToRender(sptr<IRemoteObject> connectToRender)
{
    if (!connectToRender) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (connectToRender_ != nullptr) {
        return false;
    }
    connectToRender_ = connectToRender;
    return true;
}

SurfaceNodeBufferReleaseListener::~SurfaceNodeBufferReleaseListener()
{
    UnRegisterReleaseBufferCallBack();
}
} // namespace Rosen
} // namespace OHOS
#endif
