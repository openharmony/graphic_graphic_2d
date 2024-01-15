/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef TRANSACT_SURFACE_DELEGATOR_PROXY_H
#define TRANSACT_SURFACE_DELEGATOR_PROXY_H

#include "itransact_surface_delegator.h"
#include <iremote_proxy.h>

namespace OHOS {
class TransactSurfaceDelegatorProxy : public IRemoteProxy<ITransactSurfaceDelegator> {
public:
    explicit TransactSurfaceDelegatorProxy(const sptr<IRemoteObject> &impl);
    virtual ~TransactSurfaceDelegatorProxy() noexcept = default;

private:
    static inline BrokerDelegator<TransactSurfaceDelegatorProxy> delegator_;
};
} // namespace OHOS
#endif // TRANSACT_SURFACE_DELEGATOR_PROXY_H