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

#ifndef TRANSACT_SURFACE_DELEGATOR_STUB
#define TRANSACT_SURFACE_DELEGATOR_STUB

#include <map>
#include <iremote_stub.h>
#include "surface.h"
#include "itransact_surface_delegator.h"

namespace OHOS {
typedef struct NativeHandle {
    int version;
    int numFds;
    int numInts;
    int data[0];
}NativeHandleT;

class TransactSurfaceDelegatorStub : public IRemoteStub<ITransactSurfaceDelegator> {
public:
    ~TransactSurfaceDelegatorStub() = default;
    void SetSurface(sptr<Surface> surface);
    bool SetClient(sptr<IRemoteObject> client);

protected:
    TransactSurfaceDelegatorStub() = default;
    NativeHandleT *ReadNativeHandle(MessageParcel &input);
    NativeHandleT *ReadNativeHandleWithoutVersion(MessageParcel &input);
    int32_t SendMessage(int32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    sptr<Surface> surface_ = nullptr;

private:
    int32_t SendSelfProxy();
    sptr<IRemoteObject> client_ = nullptr;
};
} // namespace OHOS

#endif //TRANSACT_SURFACE_DELEGATOR_STUB
