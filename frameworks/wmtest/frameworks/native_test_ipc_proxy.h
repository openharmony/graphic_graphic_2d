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

#ifndef FRAMEWORKS_WMTEST_FRAMEWORKS_INATIVE_TEST_IPC_PROXY_H
#define FRAMEWORKS_WMTEST_FRAMEWORKS_INATIVE_TEST_IPC_PROXY_H

#include <iremote_proxy.h>

#include "inative_test_ipc.h"

namespace OHOS {
class NativeTestIpcProxy : public IRemoteProxy<INativeTestIpc> {
public:
    explicit NativeTestIpcProxy(const sptr<IRemoteObject>& impl);
    virtual ~NativeTestIpcProxy() = default;

    virtual GSError SendMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj) override;
    virtual GSError Register(int32_t sequence, sptr<INativeTestIpc> &ipc) override;
    virtual GSError OnMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj) override;

private:
    static inline BrokerDelegator<NativeTestIpcProxy> delegator_;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMTEST_FRAMEWORKS_INATIVE_TEST_IPC_PROXY_H
