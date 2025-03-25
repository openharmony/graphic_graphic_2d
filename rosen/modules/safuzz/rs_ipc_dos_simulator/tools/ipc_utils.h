/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SAFUZZ_IPC_UTILS_H
#define SAFUZZ_IPC_UTILS_H

#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Rosen {
class TestService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.TestService");

    TestService() = default;
    ~TestService() noexcept override = default;
};

class CustomizedRemoteStub : public IRemoteStub<TestService> {
public:
    CustomizedRemoteStub() = default;
    ~CustomizedRemoteStub() noexcept override = default;
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_IPC_UTILS_H
