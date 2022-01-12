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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_PROXY_H

#include <memory>
#include <mutex>

#include "command/rs_command.h"
#include "common/rs_singleton.h"
#include "transaction/rs_irender_client.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
class RSSyncTask;

class RSTransactionProxy final {
public:
    static RSTransactionProxy* GetInstance();
    void SetRenderThreadClient(std::unique_ptr<RSIRenderClient>& renderThreadClient);
    void SetRenderServiceClient(const std::shared_ptr<RSIRenderClient>& renderServiceClient);

    void AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand = false);

    void FlushImplicitTransaction();
    void FlushImplicitRemoteTransaction();

    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task, bool isRenderServiceTask = false);
private:
    RSTransactionProxy();
    virtual ~RSTransactionProxy();
    static void Init();
    static void Destory();

    RSTransactionProxy(const RSTransactionProxy&) = delete;
    RSTransactionProxy(const RSTransactionProxy&&) = delete;
    RSTransactionProxy& operator=(const RSTransactionProxy&) = delete;
    RSTransactionProxy& operator=(const RSTransactionProxy&&) = delete;

    void AddCommonCommand(std::unique_ptr<RSCommand>& command);
    void AddRemoteCommand(std::unique_ptr<RSCommand>& command);

    std::mutex mutex_;
    std::unique_ptr<RSTransactionData> implicitCommonTransactionData_{std::make_unique<RSTransactionData>()};
    std::unique_ptr<RSTransactionData> implicitRemoteTransactionData_{std::make_unique<RSTransactionData>()};
    std::shared_ptr<RSIRenderClient> renderServiceClient_ = RSIRenderClient::CreateRenderServiceClient();
    std::unique_ptr<RSIRenderClient> renderThreadClient_ = nullptr;

    static std::once_flag flag_;
    static RSTransactionProxy* instance_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_PROXY_H
