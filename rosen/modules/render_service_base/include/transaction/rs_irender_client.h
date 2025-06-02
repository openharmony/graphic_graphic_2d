/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_CLIENT_H
#define RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_CLIENT_H

#include <memory>

#include "common/rs_common_def.h"
#include "platform/drawing/rs_surface.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
class RSSyncTask;

class RSB_EXPORT RSIRenderClient {
public:
    RSIRenderClient() = default;
    virtual ~RSIRenderClient() = default;

    virtual void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) = 0;
    virtual void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) {}

    static RSB_EXPORT std::shared_ptr<RSIRenderClient> CreateRenderServiceClient();
    static RSB_EXPORT std::unique_ptr<RSIRenderClient> CreateRenderThreadClient();

protected:
    static std::shared_ptr<RSIRenderClient> client_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_CLIENT_H
