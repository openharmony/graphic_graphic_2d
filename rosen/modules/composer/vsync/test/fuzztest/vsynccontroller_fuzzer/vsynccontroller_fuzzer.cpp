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

#include "vsynccontroller_fuzzer.h"

#include <securec.h>

#include "vsync_controller.h"
#include "vsync_distributor.h"

namespace OHOS {
    namespace {
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
        constexpr const int32_t WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS = 5;
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

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
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
        bool enable = GetData<bool>();
        int64_t offset = GetData<int64_t>();
        int64_t now = GetData<int64_t>();
        int64_t period = GetData<int64_t>();
        uint32_t refreshRate = GetData<uint32_t>();
        uint32_t vsyncMaxRefreshRate = GetData<uint32_t>();
        VSyncMode vsyncMode = GetData<VSyncMode>();
        int64_t phaseOffset = GetData<int64_t>();
        uint64_t id = GetData<uint64_t>();
        std::vector<std::pair<uint64_t, uint32_t>> refreshRates = { { id, refreshRate } };

        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, offset);
        vsyncController->SetEnable(enable, enable);
        vsyncController->SetPhaseOffset(offset);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        vsyncController->SetCallback(vsyncDistributor);
        vsyncController->OnVSyncEvent(now, period, refreshRate, vsyncMode, vsyncMaxRefreshRate);
        vsyncController->OnPhaseOffsetChanged(phaseOffset);
        vsyncController->OnConnsRefreshRateChanged(refreshRates);

        vsyncController->SetEnable(false, enable);
        sleep(WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS);
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

