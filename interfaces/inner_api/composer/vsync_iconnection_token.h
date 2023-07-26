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

#ifndef VSYNC_ICONNECTION_TOKEN_H
#define VSYNC_ICONNECTION_TOKEN_H

#include <iremote_broker.h>
#include <iremote_proxy.h>

namespace OHOS {
namespace Rosen {
class VSyncIConnectionToken : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.VSyncIConnectionToken");

    VSyncIConnectionToken() = default;
    virtual ~VSyncIConnectionToken() noexcept = default;
};

class VSyncIConnectionTokenProxy : public IRemoteProxy<VSyncIConnectionToken> {
public:
    explicit VSyncIConnectionTokenProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<VSyncIConnectionToken>(impl) {}
    virtual ~VSyncIConnectionTokenProxy() noexcept = default;

private:
    static inline BrokerDelegator<VSyncIConnectionTokenProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // VSYNC_ICONNECTION_TOKEN_H
