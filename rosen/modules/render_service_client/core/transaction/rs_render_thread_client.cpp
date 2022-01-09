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

#include "rs_render_thread_client.h"

#include "pipeline/rs_render_thread.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {

std::unique_ptr<RSIRenderClient> RSIRenderClient::CreateRenderThreadClient()
{
    return std::make_unique<RSRenderThreadClient>();
}

void RSRenderThreadClient::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    RSRenderThread::Instance().RecvTransactionData(transactionData);
}

void RSRenderThreadClient::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    std::condition_variable cv;
    auto& renderThread = RSRenderThread::Instance();
    renderThread.PostTask([task, &cv, &renderThread]() {
        if (task == nullptr) {
            return;
        }
        task->Process(renderThread.GetContext());
        cv.notify_all();
    });
    cv.wait_for(lock, std::chrono::nanoseconds(task->GetTimeout()));
}

} // namespace Rosen
} // namespace OHOS
