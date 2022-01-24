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

#ifndef ROSEN_RENDER_SERVICE_BASE_IAPPLICATION_RENDER_THREAD_H
#define ROSEN_RENDER_SERVICE_BASE_IAPPLICATION_RENDER_THREAD_H

#ifdef ROSEN_OHOS
#include <iremote_broker.h>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSTransactionData;
class IApplicationRenderThread : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RSIApplicationRenderThread");

    IApplicationRenderThread() = default;
    virtual ~IApplicationRenderThread() noexcept = default;

    enum {
        COMMIT_TRANSACTION,
    };

    virtual void OnTransaction(std::shared_ptr<RSTransactionData> transactionData) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_IAPPLICATION_RENDER_THREAD_H
