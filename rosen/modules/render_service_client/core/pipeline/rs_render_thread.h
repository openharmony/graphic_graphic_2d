/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "common/rs_thread_handler.h"
#include "common/rs_thread_looper.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_render_node.h"
#include "platform/drawing/rs_vsync_client.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
class RSRenderThread final {
public:
    static RSRenderThread& Instance();

    void Start();
    void Stop();
    void WakeUp();

    void Detach(NodeId id);
    void RecvTransactionData(std::unique_ptr<RSTransactionData>& transactionData);
    void RequestNextVSync();

    int32_t GetTid();

    std::string DumpRenderTree() const;

private:
    RSRenderThread();
    ~RSRenderThread();

    RSRenderThread(const RSRenderThread&) = delete;
    RSRenderThread(const RSRenderThread&&) = delete;
    RSRenderThread& operator=(const RSRenderThread&) = delete;
    RSRenderThread& operator=(const RSRenderThread&&) = delete;

    void RenderLoop();

    void OnVsync(uint64_t timestamp);
    void StartTimer(uint64_t interval);
    void StopTimer();
    void ProcessCommands();
    void Animate(uint64_t timestamp);
    void Render();
    void SendCommands();

    std::atomic_bool running_ = false;
    std::unique_ptr<std::thread> thread_ = nullptr;
    std::unique_ptr<RSThreadLooper> rendererLooper_ = nullptr;
    std::unique_ptr<RSThreadHandler> threadHandler_ = nullptr;
    RSTaskHandle preTask_ = nullptr;
    RSTaskHandle timeHandle_ = nullptr;
    RSTaskMessage::RSTask mainFunc_;

    std::mutex cmdMutex_;
    std::vector<std::unique_ptr<RSTransactionData>> cmds_;
    bool hasRunningAnimation_ = false;

    std::unique_ptr<RSVsyncClient> vsyncClient_ = nullptr;
    uint64_t timestamp_ = 0;
    uint64_t prevTimestamp_ = 0;
    uint64_t refreshPeriod_ = 16666667;
    int32_t tid_ = -1;

    RSContext context_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_H