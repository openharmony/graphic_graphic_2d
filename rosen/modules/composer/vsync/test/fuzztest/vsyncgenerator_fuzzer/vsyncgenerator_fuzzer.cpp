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

#include "vsyncgenerator_fuzzer.h"

#include <securec.h>

#include "vsync_generator.h"
#include "vsync_controller.h"
#include "vsync_distributor.h"

namespace OHOS {
    namespace {
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
        const int64_t kMaxVsyncInterval = 16666666; // 1/60s, 60fps
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

    static int64_t SystemTime()
    {
        timespec t = {};
        clock_gettime(CLOCK_MONOTONIC, &t);
        return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; //1000000000ns = 1s
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
        int64_t period = GetData<int64_t>() % kMaxVsyncInterval; // no more than 16666666ns
        int64_t period2 = kMaxVsyncInterval + GetData<int64_t>() % 100000; // no more than 16666666 + 100000 ns
        int64_t occurTimestamp = SystemTime() - (GetData<int64_t>() % kMaxVsyncInterval);
        int64_t nextTimestamp = SystemTime() - (GetData<int64_t>() % kMaxVsyncInterval);
        int64_t occurReferenceTime = SystemTime() - (GetData<int64_t>() % kMaxVsyncInterval);
        //bool isWakeup = GetData<bool>();
        int64_t offset = GetData<int64_t>() % kMaxVsyncInterval; // no more than 16666666ns
        int phase = GetData<int64_t>() % kMaxVsyncInterval; // no more than 16666666ns
        int64_t lastTime = SystemTime() - (GetData<int64_t>() % kMaxVsyncInterval);
        int64_t now = SystemTime();
        std::pair<uint64_t, uint32_t> pair1 = {GetData<uint64_t>(), GetData<uint32_t>()};
        uint32_t generatorRefreshRate = GetData<uint32_t>();
        int64_t rsVsyncCount = GetData<int64_t>();
        int64_t expectNextVsyncTime = SystemTime() + (GetData<int64_t>() % kMaxVsyncInterval);
        int32_t offsetByPulseNum = GetData<int32_t>();
        int64_t hardwareVsyncInterval = GetData<int64_t>();
        bool frameRateChanging = GetData<bool>();
        // test
        sptr<Rosen::impl::VSyncGenerator> vsyncGenerator = new Rosen::impl::VSyncGenerator();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, offset);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        vsyncController->SetEnable(enable, enable);
        vsyncGenerator->ChangeListenerOffsetInternal();
        vsyncGenerator->ChangeListenerRefreshRatesInternal();
        vsyncGenerator->UpdateWakeupDelay(occurReferenceTime, nextTimestamp);
        vsyncGenerator->SetPendingMode(period, occurTimestamp);
        vsyncGenerator->SetVSyncMode(GetData<OHOS::VSyncMode>());
        vsyncGenerator->UpdateVSyncModeLocked();
        Rosen::impl::VSyncGenerator::Listener listener = {
            .phase_ = phase,
            .callback_ = vsyncController,
            .lastTime_ = lastTime
        };
        vsyncGenerator->UpdatePeriodLocked(period);
        vsyncGenerator->JudgeRefreshRateLocked(period2);
        vsyncGenerator->SetExpectNextVsyncTimeInternal(now);
        Rosen::VSyncGenerator::ListenerRefreshRateData listenerRefreshRateData = {
            .cb = vsyncController,
            .refreshRates = {pair1}
        };
        Rosen::VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffsetData = {
            .cb = vsyncController,
            .phaseByPulseNum = phase
        };
        vsyncGenerator->ChangeGeneratorRefreshRateModel(listenerRefreshRateData, listenerPhaseOffsetData,
            generatorRefreshRate, rsVsyncCount, expectNextVsyncTime);
        vsyncGenerator->UpdateChangeRefreshRatesLocked(listenerRefreshRateData);
        vsyncGenerator->GetVSyncMode();
        vsyncGenerator->GetVSyncMaxRefreshRate();
        vsyncGenerator->SetReferenceTimeOffset(offsetByPulseNum);
        vsyncGenerator->SetRSDistributor(vsyncDistributor);
        vsyncGenerator->PeriodCheckLocked(hardwareVsyncInterval);
        vsyncGenerator->SetAppDistributor(vsyncDistributor);
        vsyncGenerator->CalculateReferenceTimeOffsetPulseNumLocked(occurReferenceTime);
        vsyncGenerator->CheckAndUpdateReferenceTime(hardwareVsyncInterval, occurReferenceTime);
        vsyncGenerator->SetFrameRateChangingStatus(frameRateChanging);
        //TearDownTestCase
        Rosen::DestroyVSyncGenerator();
        vsyncGenerator = nullptr;
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

