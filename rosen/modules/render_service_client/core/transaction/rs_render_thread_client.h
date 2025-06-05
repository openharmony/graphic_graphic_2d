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

/**
* @file rs_render_thread_client.h
* @brief Client used to communicate with the RenderThread in the application.
*/

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_CLIENT_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_CLIENT_H

#include "transaction/rs_irender_client.h"

namespace OHOS {
namespace Rosen {

class RSRenderThreadClient : public RSIRenderClient {
public:
    RSRenderThreadClient() = default;
    ~RSRenderThreadClient() = default;
    /**
     * @brief Commit transaction to the renderThread.
     * @param transactionData Application needs to send to RenderThread.
     */
    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    /**
     * @brief Excute the Synchronous task in renderThread.
     * @param task Synchronous task. must transfer maximum execution duration in ns.
     */
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_CLIENT_H
