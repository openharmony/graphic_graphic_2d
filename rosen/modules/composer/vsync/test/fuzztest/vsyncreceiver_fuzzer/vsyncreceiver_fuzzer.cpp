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

#include "vsyncreceiver_fuzzer.h"

#include "vsync_receiver.h"
#include "vsync_distributor.h"
#include "vsync_controller.h"


namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        constexpr char STR_END_CHAR = '\0';
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;

        void OnVSync(int64_t now, void* data) {}
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
        std::memcpy(&object, data_ + pos, objectSize);
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = STR_END_CHAR;
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < 0) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        int64_t offset = GetData<int64_t>();
        int32_t rate = GetData<int32_t>();
        void* data1 = static_cast<void*>(GetStringFromData(STR_LEN).data());

        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, offset);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        sptr<Rosen::VSyncConnection> vsyncConnection = new Rosen::VSyncConnection(vsyncDistributor, "Fuzz");
        sptr<Rosen::VSyncReceiver> vsyncReceiver = new Rosen::VSyncReceiver(vsyncConnection);
        Rosen::VSyncReceiver::FrameCallback fcb = {
            .userData_ = data1,
            .callback_ = OnVSync,
        };
        vsyncReceiver->SetVSyncRate(fcb, rate);

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

