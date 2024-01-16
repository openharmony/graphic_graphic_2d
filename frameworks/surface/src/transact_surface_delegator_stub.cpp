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

#include <message_option.h>
#include <message_parcel.h>
#include "transact_surface_delegator_stub.h"
#include "buffer_log.h"
#include "sync_fence.h"

namespace OHOS {
void TransactSurfaceDelegatorStub::SetSurface(sptr<Surface> surface)
{
}

bool TransactSurfaceDelegatorStub::SetClient(sptr<IRemoteObject> surface)
{
    return true;
}


int32_t TransactSurfaceDelegatorStub::SendSelfProxy()
{
    return ERR_NONE;
}

int32_t TransactSurfaceDelegatorStub::SendMessage(int32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    return ERR_NONE;
}

NativeHandleT *TransactSurfaceDelegatorStub::ReadNativeHandle(MessageParcel &input)
{
    return nullptr;
}

NativeHandleT *TransactSurfaceDelegatorStub::ReadNativeHandleWithoutVersion(MessageParcel &input)
{
    return nullptr;
}
} // namespace OHOS
