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
#ifndef RS_DELEGATE_COMPOSITE_LISTENER_H
#define RS_DELEGATE_COMPOSITE_LISTENER_H
#ifndef ROSEN_CROSS_PLATFORM
#include "ipc_callbacks/rs_transaction_callback.h"
#include "ipc_callbacks/rs_delegate_composite_callback.h"
#include "transaction/rs_render_pipeline_client.h"
#include "command/rs_command.h"
#include <mutex>

namespace OHOS {
namespace Rosen {
class RSC_EXPORT SurfaceTransactionListener : public RefBase
{
public:
    SurfaceTransactionListener(sptr<IRemoteObject> connectToRender);

    using OnCompleteCallback = std::function<void(uint64_t timestamp, uint64_t srcId, std::queue<uint64_t> seqNums)>;
    bool RegisterCommandCompleteCallBack(OnCompleteCallback callback);
    bool UnRegisterCommandCompleteCallBack();

    uint64_t GetUniqueId();
    std::unique_ptr<OHOS::Rosen::RSCommand> GetCommand(uint64_t& cmdSeqNum);
private:
    ~SurfaceTransactionListener();
    std::mutex mutex_;
    sptr<RSISurfaceTransactionListener> callBackStub_ = nullptr;
    uint64_t uniqueId_ = 0;
    pid_t workTid_ = 0;
    pid_t workPid_ = 0;
    uint64_t CommandSeqNum_ = 0;
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient_ = nullptr;
    sptr<IRemoteObject> connectToRender_ = nullptr;
};

class RSC_EXPORT SurfaceNodeBufferReleaseListener : public RefBase
{
public:
    static SurfaceNodeBufferReleaseListener& GetInstance();

    using OnBufferCompleteCallback = std::function<void(std::queue<OnCompletedRet> queue)>;
    bool RegisterReleaseBufferCallBack(OnBufferCompleteCallback callback);
    bool UnRegisterReleaseBufferCallBack();
    bool SetConnectToRender(sptr<IRemoteObject> connectToRender);
private:
    SurfaceNodeBufferReleaseListener();
    ~SurfaceNodeBufferReleaseListener();

    std::mutex mutex_;
    sptr<RSISurfaceNodeBufferReleaseCallback> callBackStub_ = nullptr;
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient_ = nullptr;
    uint64_t uniqueId_ = 0;
    sptr<IRemoteObject> connectToRender_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_CROSS_PLATFORM
#endif
