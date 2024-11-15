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

#include "localsocketpair_fuzzer.h"

#include <securec.h>
#include "local_socketpair.h"
#include <message_parcel.h>
#include <message_option.h>


namespace OHOS {
    namespace {
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
        constexpr int32_t SOCKET_PAIR_SIZE = 2;
    }

    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // get data
        size_t sendSize = GetData<size_t>();
        size_t receiveSize = GetData<size_t>();
        int64_t vsyncData[3];
        vsyncData[0] = GetData<int64_t>();
        vsyncData[1] = GetData<int64_t>();
        vsyncData[2] = GetData<int64_t>(); // index 2
        int32_t fd = GetData<int32_t>();

        // test
        sptr<LocalSocketPair> socketPair = new LocalSocketPair();
        socketPair->CreateChannel(sendSize, receiveSize);
        socketPair->GetSendDataFd();
        socketPair->GetReceiveDataFd();
        MessageParcel messageParcel;
        socketPair->SendToBinder(messageParcel);
        socketPair->ReceiveToBinder(messageParcel);
        socketPair->SendData(vsyncData, sizeof(vsyncData));
        socketPair->ReceiveData(vsyncData, sizeof(vsyncData));
        socketPair->SendFdToBinder(messageParcel, fd);
        int32_t socketPairFds[SOCKET_PAIR_SIZE] = {
            socketPair->sendFd_,
            socketPair->receiveFd_,
        };
        socketPair->SetSockopt(sendSize, receiveSize, socketPairFds, SOCKET_PAIR_SIZE);
        socketPair->CloseFd(socketPair->sendFd_);
        socketPair->CloseFd(socketPair->receiveFd_);

        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

