/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PROCESS_TRANSACTION_CONTROLLER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PROCESS_TRANSACTION_CONTROLLER_H

#include "common/rs_macros.h"
#include "ipc_callbacks/rs_sync_transaction_controller_stub.h"

namespace OHOS {
namespace Rosen {
using TransactionFinishedCallback = std::function<void()>;

class RSC_EXPORT RSProcessTransactionController : public RSSyncTransactionControllerStub {
public:
    RSProcessTransactionController() = default;
    virtual ~RSProcessTransactionController() = default;

    void CreateTransactionFinished() override;

private:
    void SetTransactionFinishedCallback(TransactionFinishedCallback& callback);

    TransactionFinishedCallback callback_;

    friend class RSTransaction;
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_PROCESS_TRANSACTION_CONTROLLER_H
