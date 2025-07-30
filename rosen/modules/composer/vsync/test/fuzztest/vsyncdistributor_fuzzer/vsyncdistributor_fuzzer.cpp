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

#include "vsyncdistributor_fuzzer.h"

#include <securec.h>

#include "vsync_distributor.h"
#include "vsync_controller.h"

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
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

    template<>
    std::string GetData()
    {
        size_t objectSize = GetData<uint8_t>();
        std::string object(objectSize, '\0');
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        object.assign(reinterpret_cast<const char*>(data_ + pos), objectSize);
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        if (strlen <= 0) {
            return "fuzz";
        }
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
        int64_t offset = GetData<int64_t>();
        int32_t rate = GetData<int32_t>();
        bool uiDVSyncSwitch = GetData<bool>();
        bool nativeDVSyncSwitch = GetData<bool>();
        int32_t bufferCount = GetData<int32_t>();
        int32_t highPriorityRate = GetData<int32_t>();
        int64_t windowNodeId = GetData<int64_t>();
        uint32_t vsyncMaxRefreshRate = GetData<uint32_t>();
        int64_t timestamp = GetData<int64_t>();
        uint32_t pid = GetData<uint32_t>();
        bool isSystemAnimateScene = GetData<bool>();
        uint64_t id = GetData<uint64_t>();
        bool isRender = GetData<bool>();
        bool delayEnable = GetData<bool>();
        bool nativeDelayEnable = GetData<bool>();

        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, offset);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        sptr<Rosen::VSyncConnection> conn = new Rosen::VSyncConnection(vsyncDistributor, "Fuzz");
        vsyncDistributor->SetVSyncRate(rate, conn);
        vsyncDistributor->SetNativeDVSyncSwitch(nativeDVSyncSwitch, conn);
        vsyncDistributor->SetUiDvsyncSwitch(uiDVSyncSwitch, conn);
        vsyncDistributor->SetUiDvsyncConfig(bufferCount, delayEnable, nativeDelayEnable);
        vsyncDistributor->SetHighPriorityVSyncRate(highPriorityRate, conn);
        vsyncDistributor->AddConnection(conn, windowNodeId);
        vsyncDistributor->RemoveConnection(conn);
        std::mutex mutex;
        std::unique_lock<std::mutex> locker(mutex);
        vsyncDistributor->WaitForVsyncOrTimeOut(locker);
        std::vector<sptr<Rosen::VSyncConnection>> conns = {conn};
        vsyncDistributor->PostVSyncEventPreProcess(timestamp, conns);
        vsyncDistributor->EnableVSync();
        vsyncDistributor->DisableVSync();
        std::string name = GetStringFromData(STR_LEN);
        vsyncDistributor->QosGetPidByName(name, pid);
        vsyncDistributor->SetQosVSyncRateByPid(pid, rate, isSystemAnimateScene);
        vsyncDistributor->SetQosVSyncRate(id, rate, isSystemAnimateScene);
        vsyncDistributor->ChangeConnsRateLocked(vsyncMaxRefreshRate);
        vsyncDistributor->SetFrameIsRender(isRender);
        vsyncDistributor->MarkRSAnimate();
        vsyncDistributor->UnmarkRSAnimate();
        vsyncDistributor->HasPendingUIRNV();
        return true;
    }

    bool DoSetQosVSyncRateByPidPublic(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        //get data
        int64_t offset  = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        int32_t rateDiscount = GetData<int32_t>();
        bool isSystemAnimateScene = GetData<int32_t>();
        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, offset);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        sptr<Rosen::VSyncConnection> conn = new Rosen::VSyncConnection(vsyncDistributor, "Fuzz");
        vsyncDistributor->SetQosVSyncRateByPidPublic(pid, rateDiscount, isSystemAnimateScene);
        return true;
    }

    bool DoSetVsyncRateDiscountLTPS(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        int64_t offset  = GetData<int32_t>();
        uint32_t pid = GetData<uint32_t>();
        std::string name = GetData<std::string>();
        uint32_t rateDiscount = GetData<uint32_t>();

        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, offset);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        sptr<Rosen::VSyncConnection> conn = new Rosen::VSyncConnection(vsyncDistributor, "Fuzz");
        vsyncDistributor->SetVsyncRateDiscountLTPS(pid, name, rateDiscount);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::DoSetQosVSyncRateByPidPublic(data, size);
    OHOS::DoSetVsyncRateDiscountLTPS(data, size);
    return 0;
}

