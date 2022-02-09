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

#ifndef LOCAL_SOCAKT_PAIR_H
#define LOCAL_SOCAKT_PAIR_H

#include <sys/types.h>
#include <refbase.h>
#include "message_parcel.h"

namespace OHOS {
class LocalSocketPair : public RefBase {
public:
    LocalSocketPair();
    virtual ~LocalSocketPair();
    LocalSocketPair(const LocalSocketPair &) = delete;
    LocalSocketPair &operator=(const LocalSocketPair &) = delete;
    int32_t CreateChannel(size_t sendSize, size_t receiveSize);
    int32_t GetSendDataFd() const
    {
        return sendFd_;
    };
    int32_t GetReceiveDataFd() const
    {
        return receiveFd_;
    };

    // send sendfd to binder
    int32_t SendToBinder(MessageParcel &data);

    // send receivefd to binder
    int32_t ReceiveToBinder(MessageParcel &data);

    int32_t SendData(const void *vaddr, size_t size);
    int32_t ReceiveData(void *vaddr, size_t size);
private:
    int32_t SendFdToBinder(MessageParcel &data, int32_t &fd);
    void CloseFd(int32_t &fd);

private:
    int32_t sendFd_;
    int32_t receiveFd_;
};
}

#endif // BS_LOCAL_SOCAKTPAIR_H
