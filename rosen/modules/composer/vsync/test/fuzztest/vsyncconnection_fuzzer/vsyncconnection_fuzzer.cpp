/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "vsyncconnection_fuzzer.h"

#include <securec.h>

#include "vsync_distributor.h"
#include "vsync_generator.h"
#include "vsync_controller.h"
#include "vsync_connection_stub.h"

namespace OHOS {
    namespace {
        constexpr uint32_t FUNC_NUM = 6;
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        int64_t offset = GetData<int64_t>();
        int32_t rate = GetData<int32_t>();
        int64_t now = GetData<int64_t>();

        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, offset);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        sptr<Rosen::VSyncConnection> vsyncConnection = new Rosen::VSyncConnection(vsyncDistributor, "Fuzz");
        vsyncConnection->SetVSyncRate(rate);
        // 16666667 is Vsync period.
        vsyncConnection->PostEvent(now, 16666667, 0);

        vsyncConnection->RequestNextVSync();
        std::string fromWhom = "FUZZ";
        int64_t lastVSyncTS = GetData<int64_t>();
        vsyncConnection->RequestNextVSync(fromWhom, lastVSyncTS);
        int32_t fd = GetData<int32_t>();
        vsyncConnection->GetReceiveFd(fd);
        vsyncConnection->Destroy();
        bool nativeDVSyncSwitch = GetData<bool>();
        vsyncConnection->SetNativeDVSyncSwitch(nativeDVSyncSwitch);
        bool vsyncSwitch = GetData<bool>();
        vsyncConnection->SetUiDvsyncSwitch(vsyncSwitch);
        int32_t bufferCount = GetData<int32_t>();
        vsyncConnection->SetUiDvsyncConfig(bufferCount);
        MessageParcel arguments;
        MessageParcel reply;
        MessageOption option;
        uint32_t code = GetData<uint32_t>() %
            (static_cast<uint32_t>(Rosen::IVSyncConnection::IVSYNC_CONNECTION_SET_NATIVE_DVSYNC_SWITCH) + 1);
        vsyncConnection->OnRemoteRequest(code, arguments, reply, option);
        arguments.WriteInterfaceToken(vsyncConnection->metaDescriptor_);
        for (uint32_t i = 0; i < FUNC_NUM; ++i) {
            vsyncConnection->OnRemoteRequest(i, arguments, reply, option);
        }
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

