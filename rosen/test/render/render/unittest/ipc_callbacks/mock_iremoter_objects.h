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

#ifndef MOCK_IREMOTER_OBJECTS_H
#define MOCK_IREMOTER_OBJECTS_H

#include <iremote_proxy.h>

namespace OHOS::Rosen {
class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject {u"MockIRemoteObject"}
    {
    }

    ~MockIRemoteObject()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel&, MessageParcel&, MessageOption&)
    {
        receivedCode_ = code;
        return sendRequestResult_;
    }

    bool IsProxyObject() const
    {
        return true;
    }

    bool CheckObjectLegality() const
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient>&)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient>&)
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface()
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string>&)
    {
        return 0;
    }

    void SetErrorStatus(int error)
    {
        sendRequestResult_ = error;
    }

    int sendRequestResult_ = 0;
    uint32_t receivedCode_ = 0;
};
} // namespace OHOS::Rosen
#endif // MOCK_IREMOTER_OBJECTS_H
